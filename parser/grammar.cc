#include "parser/grammar.h"

#include <set>

namespace parser {

std::string Production::get_head() {
  return head_;
}

std::vector<std::string> Production::get_body() {
  return body_;
}

bool operator==(Production production_lhs, Production production_rhs) {
  return (production_lhs.get_head() == production_rhs.get_head() &&
          production_lhs.get_body() == production_rhs.get_body());
}

bool Grammar::is_non_terminal(const std::string& grammar_symbol) {
  for (auto production : productions_) {
    if (production.get_head() == grammar_symbol) {
      return true;
    }
  }
  return false;
}

std::string Grammar::get_start_symbol() {
  return start_symbol_;
}

std::vector<Production> Grammar::get_productions_of_non_terminal(
    const std::string& non_terminal) {
  std::vector<Production> productions_of_non_terminal;

  for (auto production : productions_) {
    if (production.get_head() == non_terminal) {
      productions_of_non_terminal.push_back(production);
    }
  }

  return productions_of_non_terminal;
}

std::unordered_set<std::string> Grammar::compute_first_set(
    const std::string& grammar_symbol) {
  if (!is_non_terminal(grammar_symbol) || grammar_symbol.empty()) {
    return {grammar_symbol};
  }

  std::set<std::string> sorted_first_set;
  for (auto production : get_productions_of_non_terminal(grammar_symbol)) {
    if (production.get_head() != production.get_body()[0]) {
      auto first_set_of_production_body = compute_first_set(
          production.get_body());
      std::set<std::string> sorted_first_set_of_production_body;
      for (const auto& symbol : first_set_of_production_body) {
        sorted_first_set_of_production_body.insert(symbol);
      }

      std::set_union(
          sorted_first_set_of_production_body.begin(),
          sorted_first_set_of_production_body.end(),
          sorted_first_set.begin(),
          sorted_first_set.end(),
          std::inserter(sorted_first_set, sorted_first_set.begin()));
    }
  }

  std::unordered_set<std::string> first_set;
  for (const auto& symbol : sorted_first_set) {
    first_set.insert(symbol);
  }
  return first_set;
}

std::unordered_set<std::string> Grammar::compute_first_set(
    const std::vector<std::string>& grammar_symbols) {
  std::unordered_set<std::string> first_set;

  for (const auto& grammar_symbol : grammar_symbols) {
    auto first_set_body_symbol = compute_first_set(grammar_symbol);
    if (first_set_body_symbol.size() == 1 &&
        first_set_body_symbol.find("") != first_set_body_symbol.end()) {
      // It is a nullable symbol. Skip.
      continue;
    } else {
      first_set = first_set_body_symbol;
      break;
    }
  }

  if (first_set.empty()) {
    first_set = {""};
  }

  return first_set;
}

std::unordered_set<std::string> Grammar::compute_follow_set(
    const std::string& non_terminal) {
  std::set<std::string> sorted_follow_set;

  if (non_terminal == start_symbol_) {
    sorted_follow_set.insert("$");
  }

  for (auto production : productions_) {
    auto production_body = production.get_body();
    for (auto idx = 0; idx < production_body.size(); ++idx) {
      if (production_body[idx] == non_terminal &&
          idx < production_body.size() - 1) {
        auto next_symbol = production_body[idx + 1];
        auto first_set_of_next_symbol = compute_first_set(next_symbol);
        if (first_set_of_next_symbol.size() == 1 &&
            first_set_of_next_symbol.find("") !=
            first_set_of_next_symbol.end()) {
          continue;
        }
        std::set<std::string> sorted_first_set_of_next_symbol;
        for (const auto& symbol : first_set_of_next_symbol) {
          sorted_first_set_of_next_symbol.insert(symbol);
        }
        std::set_union(
            sorted_first_set_of_next_symbol.begin(),
            sorted_first_set_of_next_symbol.end(),
            sorted_follow_set.begin(),
            sorted_follow_set.end(),
            std::inserter(sorted_follow_set, sorted_follow_set.begin()));
      }
    }
  }

  for (auto production : productions_) {
    auto production_body = production.get_body();
    auto last_body_symbol = production_body[production_body.size() - 1];
    if (last_body_symbol == non_terminal) {
      auto sorted_follow_set_of_last_body_symbol = compute_follow_set(
          production.get_head());
      std::set_union(
          sorted_follow_set_of_last_body_symbol.begin(),
          sorted_follow_set_of_last_body_symbol.end(),
          sorted_follow_set.begin(),
          sorted_follow_set.end(),
          std::inserter(sorted_follow_set, sorted_follow_set.begin()));
    }
  }

  std::unordered_set<std::string> follow_set;
  for (const auto& symbol : sorted_follow_set) {
    follow_set.insert(symbol);
  }

  return follow_set;
}

int Grammar::get_production_number(const Production& production) {
  for (auto idx = 0; idx < productions_.size(); ++idx) {
    if (productions_[idx] == production)
      return idx;
  }
  return -1;
}

Production Grammar::get_production_by_number(int production_number) {
  return productions_[production_number];
}

}  // namespace parser
