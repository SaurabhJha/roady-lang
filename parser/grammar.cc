#include "parser/grammar.h"

#include <algorithm>
#include <iterator>

namespace parser_grammar {

bool Production::operator==(const Production& other_production) {
  return head_ == other_production.head_ && body_ == other_production.body_;
}

std::string Grammar::get_start_symbol() {
  return start_symbol_;
}

std::vector<Production> Grammar::get_productions() {
  return productions_;
}

std::vector<Production> Grammar::get_productions_of_non_terminal(const std::string& non_terminal) {
  std::vector<Production> productions;

  for (auto production : productions_)
    if (production.get_head() == non_terminal)
      productions.push_back(production);

  return productions;
}

bool Grammar::is_terminal(const std::string& grammar_symbol) {
  for (auto production : productions_) {
    if (production.get_head() == grammar_symbol)
      return false;
  }
  return true;
}

void Grammar::add_production(const Production& production) {
  productions_.push_back(production);
}

std::unordered_set<std::string> Grammar::compute_first(
    const std::string& grammar_symbol) {
  if (firsts_table_.find(grammar_symbol) != firsts_table_.end())
    return firsts_table_[grammar_symbol];

  std::unordered_set<std::string> first;
  if (is_terminal(grammar_symbol)) {
    first = {grammar_symbol};
  } else {
    for (auto production : productions_) {
      auto first_body_symbol = production.get_body()[0];
      if (production.get_head() == grammar_symbol &&
          first_body_symbol != grammar_symbol) {
        auto first_of_body = compute_first(first_body_symbol);
        std::set_union(
            first.begin(),
            first.end(),
            first_of_body.begin(),
            first_of_body.end(),
            std::inserter(first, first.begin()));
      }
    }
  }

  return first;
}

std::unordered_set<std::string> Grammar::compute_follow(
    const std::string& non_terminal) {
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
            std::inserter(follow, follow.begin()));
      } else if (production_body[i] ==
                 non_terminal && i == production_body_size - 1) {
        auto follow_of_head = compute_follow(production.get_head());
        std::set_union(
            follow.begin(),
            follow.end(),
            follow_of_head.begin(),
            follow_of_head.end(),
            std::inserter(follow, follow.begin()));
      }
  }

  follows_table_[non_terminal] = follow;

  return follow;
}

}  // namespace parser_grammar
