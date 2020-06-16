#include "parser/grammar.h"

#include <algorithm>

namespace parser {

std::vector<std::string> ProductionBody::get_symbols()
{
  return body_;
}

std::string Production::get_head()
{
  return head_;
}

ProductionBody Production::get_body()
{
  return body_;
}

bool Grammar::is_terminal(const std::string& grammar_symbol)
{
  return grammar_table_.find(grammar_symbol) == grammar_table_.end();
}

void Grammar::add_production(Production production)
{
  if (grammar_table_.find(production.get_head()) == grammar_table_.end()) {
    std::vector<ProductionBody> grammar_productions;
    grammar_table_[production.get_head()] = grammar_productions;
  }

  grammar_table_[production.get_head()].push_back(production.get_body());
}

std::unordered_set<std::string> Grammar::compute_first(const std::string& grammar_symbol)
{
  if (firsts_table_.find(grammar_symbol) != firsts_table_.end())
    return firsts_table_[grammar_symbol];

  std::unordered_set<std::string> first;
  if (is_terminal(grammar_symbol))
    first = {grammar_symbol};
  else {
    std::vector<ProductionBody> production_bodies = grammar_table_[grammar_symbol];
    for (auto production_body : production_bodies)
      if (production_body.get_symbols()[0] != grammar_symbol)
        for (const auto& terminal_of_recursed_symbol : compute_first(production_body.get_symbols()[0]))
          first.insert(terminal_of_recursed_symbol);
  }

  firsts_table_[grammar_symbol] = first;
  return first;
}

std::unordered_set<std::string> Grammar::compute_follow(const std::string& non_terminal)
{
  std::unordered_set<std::string> follow = {};

  for (const auto& production_head_bodies_pair : grammar_table_) {
    const auto &production_head = production_head_bodies_pair.first;
    for (auto production_body : production_head_bodies_pair.second) {
      auto production_body_symbols = production_body.get_symbols();
      auto production_body_size = production_body_symbols.size();
      for (auto i = 0; i < production_body_size; ++i) {
        if (production_body_symbols[i] == non_terminal) {
          if (i == production_body_size - 1) {
            for (const auto &terminal : compute_follow(production_head))
              follow.insert(terminal);
          } else {
            for (const auto &terminal : compute_first(production_body_symbols[i + 1]))
              follow.insert(terminal);
          }
        }
      }
    }
  }

  follows_table_[non_terminal] = follow;
  return follow;
}

}