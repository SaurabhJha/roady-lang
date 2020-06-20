#include "parser/grammar.h"

#include <algorithm>
#include <deque>
#include <iterator>

namespace parser {

bool Production::operator==(Production& other_production)
{
  return head_ == other_production.head_ && body_ == other_production.body_;
}

bool Grammar::is_terminal(const std::string& grammar_symbol)
{
  for (auto production : productions_) {
    if (production.get_head() == grammar_symbol)
      return false;
  }
  return true;
}

void Grammar::add_production(const Production& production)
{
  productions_.push_back(production);
}

std::unordered_set<std::string> Grammar::compute_first(const std::string& grammar_symbol)
{
  if (firsts_table_.find(grammar_symbol) != firsts_table_.end())
    return firsts_table_[grammar_symbol];

  std::unordered_set<std::string> first;
  if (is_terminal(grammar_symbol))
    first = {grammar_symbol};
  else {
    for (auto production : productions_) {
      auto first_body_symbol = production.get_body()[0];
      if (production.get_head() == grammar_symbol && first_body_symbol != grammar_symbol) {
        auto first_of_body = compute_first(first_body_symbol);
        std::set_union(
            first.begin(),
            first.end(),
            first_of_body.begin(),
            first_of_body.end(),
            std::inserter(first, first.begin())
        );
      }
    }
  }

  firsts_table_[grammar_symbol] = first;
  return first;
}

std::unordered_set<std::string> Grammar::compute_follow(const std::string& non_terminal)
{
  if (follows_table_.find(non_terminal) != follows_table_.end())
    return follows_table_[non_terminal];

  std::unordered_set<std::string> follow = {};
  if (non_terminal == start_symbol_)
    follow.insert("$");

  for (auto production : productions_) {
    auto production_body = production.get_body();
    auto production_body_size = production_body.size();
    for (auto i = 0; i < production_body.size(); ++i)
      if (production_body[i] == non_terminal && i < production_body_size - 1) {
        auto first_of_next_symbol = compute_first(production_body[i + 1]);
        std::set_union(
            follow.begin(),
            follow.end(),
            first_of_next_symbol.begin(),
            first_of_next_symbol.end(),
            std::inserter(follow, follow.begin())
        );
      } else if (production_body[i] == non_terminal && i == production_body_size - 1) {
        auto follow_of_head = compute_follow(production.get_head());
        std::set_union(
            follow.begin(),
            follow.end(),
            follow_of_head.begin(),
            follow_of_head.end(),
            std::inserter(follow, follow.begin())
        );
      }
  }

  follows_table_[non_terminal] = follow;
  return follow;
}

/**
 * Construct an LR(0) automaton using breadth-first search.
 */
LR0Automaton Grammar::construct_lr0_automaton()
{
  LR0Item first_lr0_item(productions_, {{0, 0}});
  std::vector<LR0Item> lr0_items = {first_lr0_item};
  std::deque<LR0Item> queue = {first_lr0_item};

  auto automaton = LR0Automaton();
  while (!queue.empty()) {
    auto current_item = queue.front();
    queue.pop_front();
    for (const auto& transition_symbol_next_item_pair : current_item.get_all_transition_symbol_next_item_pairs()) {
      auto transition_symbol = transition_symbol_next_item_pair.first;
      auto next_item = transition_symbol_next_item_pair.second;

      if (std::find(lr0_items.begin(), lr0_items.end(), next_item) == lr0_items.end()) {
        // If we have not seen this state before, add it to the queue for later exploration.
        queue.push_back(next_item);
        lr0_items.push_back(next_item);
      }

      auto initial_state = std::distance(lr0_items.begin(), std::find(lr0_items.begin(), lr0_items.end(), current_item));
      auto final_state = std::distance(lr0_items.begin(), std::find(lr0_items.begin(), lr0_items.end(), next_item));
      automaton.add_new_transition(initial_state, transition_symbol, final_state);
    }
  }
  return automaton;
}

bool LR0Item::is_non_terminal(const std::string& grammar_symbol)
{
  for (auto production : productions_) {
    if (production.get_head() == grammar_symbol)
      return true;
  }
  return false;
}

std::vector<int> LR0Item::get_production_numbers_for_non_terminal(const std::string& non_terminal)
{
  std::vector<int> production_numbers;
  for (auto i = 0; i < productions_.size(); ++i) {
    if (productions_[i].get_head() == non_terminal) {
      production_numbers.push_back(i);
    }
  }
  return production_numbers;
}

void LR0Item::include_non_kernel_items()
{
  std::deque<std::string> queue_of_non_terminals;
  std::unordered_set<std::string> non_terminals_seen;

  for (auto item_position_in_production : item_positions_in_productions_) {
    auto production_number = item_position_in_production.first;
    auto production = productions_[production_number];
    auto body_position = item_position_in_production.second;
    if (body_position < production.get_body().size()) {
      auto next_body_symbol = production.get_body()[body_position];
      if (is_non_terminal(next_body_symbol) && non_terminals_seen.find(next_body_symbol) == non_terminals_seen.end()) {
        non_terminals_seen.insert(next_body_symbol);
        queue_of_non_terminals.push_back(next_body_symbol);
      }
    }
  }

  while (!queue_of_non_terminals.empty()) {
    auto current_non_terminal = queue_of_non_terminals.front();
    queue_of_non_terminals.pop_front();

    for (auto production_number : get_production_numbers_for_non_terminal(current_non_terminal)) {
      std::pair<int, int> production_number_position_pair(production_number, 0);
      item_positions_in_productions_.insert(production_number_position_pair);
      // item_positions_in_productions_[production_number] = 0;

      auto first_body_symbol = productions_[production_number].get_body()[0];
      if (is_non_terminal(first_body_symbol) && non_terminals_seen.find(first_body_symbol) == non_terminals_seen.end()) {
        non_terminals_seen.insert(first_body_symbol);
        queue_of_non_terminals.push_back(first_body_symbol);
      }
    }
  }
}

bool LR0Item::operator==(const LR0Item& other_item)
{
  if (productions_.size() != other_item.productions_.size()) return false;

  for (auto i = 0; i < productions_.size(); ++i) {
    auto this_production = productions_[i];
    auto other_production = other_item.productions_[i];
    if (!(this_production == other_production))
      return false;
  }

  return (other_item.item_positions_in_productions_ == item_positions_in_productions_);
}

/**
 * Get all items reachable from the current item along with their respective transition symbols.
 * @return
 */
std::unordered_map<std::string, LR0Item> LR0Item::get_all_transition_symbol_next_item_pairs()
{
  std::unordered_map<std::string, LR0Item> next_items;

  // Firstly, get all transition symbols possible out of the current item.
  std::unordered_set<std::string> transition_symbols;
  for (auto item_position_in_production : item_positions_in_productions_) {
    auto production_number = item_position_in_production.first;
    auto production = productions_[production_number];
    auto position_index = item_position_in_production.second;
    if (position_index < production.get_body().size()) {
      auto transition_symbol = production.get_body()[position_index];
      transition_symbols.insert(transition_symbol);
    }
  }

  // Construct next items for all the transition symbols.
  for (const auto& transition_symbol : transition_symbols) {
    next_items.insert({transition_symbol, compute_next_item_on_transition_symbol(transition_symbol)});
  }
  return next_items;
}

/**
 * Given a transition symbol, construct the item that would be reached from the current item by traversing the edge
 * labelled by this transition symbol.
 */
LR0Item LR0Item::compute_next_item_on_transition_symbol(const std::string& transition_symbol)
{
  std::unordered_multimap<int, int> next_item_positions_in_productions;
  for (auto item_position_in_production : item_positions_in_productions_) {
    auto production_index = item_position_in_production.first;
    auto production = productions_[production_index];
    auto production_body_index = item_position_in_production.second;
    auto production_body = production.get_body();
    if (production_body_index < production_body.size() && production_body[production_body_index] == transition_symbol) {
      next_item_positions_in_productions.insert({production_index, production_body_index + 1});
    }
  }

  LR0Item next_lr0_item(productions_, next_item_positions_in_productions);
  return next_lr0_item;
}

}