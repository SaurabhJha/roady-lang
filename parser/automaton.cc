#include "parser/automaton.h"

#include <algorithm>
#include <iterator>
#include <deque>

namespace parser_automaton {

bool operator==(LR0Item item1, LR0Item item2) {
  return (item1.get_production() == item2.get_production() &&
      item1.get_position_in_production() == item2.get_position_in_production());
}

/**
 * Implemented using a primitive quadratic time algorithm because I could not
 * get std::find to work.
 * @param item_set1
 * @param item_set2
 * @return
 */
bool operator==(LR0ItemSet item_set1, LR0ItemSet item_set2) {
  auto item_set1_vector = item_set1.get_item_set();
  auto item_set2_vector = item_set2.get_item_set();
  if (item_set1_vector.size() != item_set2_vector.size())
    return false;

  for (const auto& item_from_set1 : item_set1_vector) {
    auto found = false;
    for (const auto& item_from_set2 : item_set2_vector) {
      if (item_from_set1 == item_from_set2)
        found = true;
    }
    if (!found) {
      return false;
    }
  }

  for (const auto& item_from_set2 : item_set2_vector) {
    auto found = false;
    for (const auto& item_from_set1 : item_set1_vector) {
      if (item_from_set2 == item_from_set1)
        found = true;
    }
    if (!found) {
      return false;
    }
  }

  return true;
}

/**
 * Compute epsilon closure of an LR(0) item set using breadth first search.
 * @param production
 * @return
 */
void LR0ItemSet::compute_epsilon_closure(parser_grammar::Grammar grammar) {
  std::deque<LR0Item> queue;

  for (const auto& item : item_set_)
    queue.push_back(item);

  while (!queue.empty()) {
    auto current_item = queue.front();
    queue.pop_front();

    if (!current_item.is_reducible() && !grammar.is_terminal(current_item.get_next_symbol())) {
      // Next symbol is a non terminal so we need to consider adding the productions of this
      // non terminal to the item set.
      auto next_non_terminal = current_item.get_next_symbol();
      for (const auto& production : grammar.get_productions_of_non_terminal(next_non_terminal)) {
        auto candidate_lr0_item = LR0Item(production, 0);
        if (std::find(item_set_.begin(), item_set_.end(), candidate_lr0_item) == item_set_.end()) {
          // We have not seen this item set before. Add it to the item set and to the queue.
          item_set_.push_back(candidate_lr0_item);
          queue.push_back(candidate_lr0_item);
        }
      }
    }
  }
}

std::unordered_set<std::string> LR0ItemSet::get_next_symbols() {
  std::unordered_set<std::string> next_symbols;
  for (auto item : item_set_) {
    if (!item.is_reducible()) {
      next_symbols.insert(item.get_next_symbol());
    }
  }

  return next_symbols;
}

LR0ItemSet LR0ItemSet::get_next_item_set(const std::string& transition_symbol, const parser_grammar::Grammar& grammar) {
  LR0ItemSet next_item_set;
  for (auto current_item : item_set_) {
    if (!current_item.is_reducible() && current_item.get_next_symbol() == transition_symbol) {
      LR0Item next_item(
          current_item.get_production(),
          current_item.get_position_in_production() + 1);
      next_item_set.add_item(next_item);
    }
  }
  next_item_set.compute_epsilon_closure(grammar);

  return next_item_set;
}

/**
 * Construct an SLR(1) parsing table by doing a breadth first search on an implicit LR(0)
 * transition graph.
 * @param grammar
 */
ParsingTable::ParsingTable(parser_grammar::Grammar grammar)
  :grammar_(grammar) {

  auto start_production = grammar.get_productions_of_non_terminal(grammar.get_start_symbol())[0];
  auto start_item = LR0Item(start_production, 0);
  auto start_item_set = LR0ItemSet(start_item);
  start_item_set.compute_epsilon_closure(grammar);

  std::vector<LR0ItemSet> seen_item_sets = {start_item_set};
  std::deque<LR0ItemSet> queue = {start_item_set};

  while (!queue.empty()) {
    auto current_item_set = queue.front();
    queue.pop_front();

    for (const auto& transition_symbol : current_item_set.get_next_symbols()) {
      auto next_item_set = current_item_set.get_next_item_set(transition_symbol, grammar);
      if (std::find(
          std::begin(seen_item_sets),
          std::end(seen_item_sets),
          next_item_set
      ) == std::end(seen_item_sets)) {
        seen_item_sets.push_back(next_item_set);
        queue.push_back(next_item_set);
      }

      auto iterator1 = std::find(
          std::begin(seen_item_sets),
          std::end(seen_item_sets),
          current_item_set);
      auto iterator2 = std::find(
          std::begin(seen_item_sets),
          std::end(seen_item_sets),
          next_item_set);
      auto current_state = std::distance(
          std::begin(seen_item_sets),
          iterator1);
      auto next_state = std::distance(
          std::begin(seen_item_sets),
          iterator2);

      auto parsing_action = ParsingTableAction(ParsingTableActionType::shift, next_state);
      add_entry(current_state, transition_symbol, parsing_action);
    }
  }

  auto productions_list = grammar.get_productions();
  for (auto state_number = 0; state_number < seen_item_sets.size(); ++state_number) {
    auto item_set = seen_item_sets[state_number];
    for (auto item : item_set.get_item_set()) {
      if (item.is_reducible()) {
        auto production = item.get_production();
        auto iterator = std::find(std::begin(productions_list), std::end(productions_list), production);
        auto production_number = std::distance(
            std::begin(productions_list),
            iterator);
        auto non_terminal = production.get_head();
        for (const auto& follow_terminal : grammar.compute_follow(non_terminal)) {
          auto parsing_action = ParsingTableAction(ParsingTableActionType::reduce, production_number);
          add_entry(state_number, follow_terminal, parsing_action);
        }
      }
    }
  }
}

}  // namespace parser_automaton
