#ifndef PARSER_AUTOMATON_H_
#define PARSER_AUTOMATON_H_

#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "parser/grammar.h"

namespace parser_automaton {

class LR0Item {
 private:
  parser_grammar::Production production_;
  int position_in_production_ = 0;

 public:
  LR0Item() = default;
  LR0Item(const parser_grammar::Production& production, int position_in_production)
    :production_{production}, position_in_production_{position_in_production} {}
  ~LR0Item() = default;

  parser_grammar::Production get_production() {
    return production_;
  }

  int get_position_in_production() {
    return position_in_production_;
  }

  bool is_reducible() {
    return position_in_production_ == production_.get_body().size();
  }

  std::string get_next_symbol() {
    return production_.get_body()[position_in_production_];
  }

  LR0Item get_next_item() {
    return LR0Item(production_, position_in_production_ + 1);
  }
};

bool operator==(LR0Item item1, LR0Item item2);

class LR0ItemSet {
 private:
  std::vector<LR0Item> item_set_ = {};

 public:
  LR0ItemSet() = default;
  explicit LR0ItemSet(const LR0Item& item) {
    item_set_.push_back(item);
  }
  ~LR0ItemSet() = default;

  std::vector<LR0Item> get_item_set() {
    return item_set_;
  }
  void add_item(LR0Item item) {
    if (std::find(
        std::begin(item_set_),
        std::end(item_set_),
        item) == std::end(item_set_))
      item_set_.push_back(item);
  }

  void compute_epsilon_closure(parser_grammar::Grammar grammar);
  std::unordered_set<std::string> get_next_symbols();
  LR0ItemSet get_next_item_set(const std::string& transition_symbol, const parser_grammar::Grammar& grammar);
};

bool operator==(LR0ItemSet item_set1, LR0ItemSet item_set2);

enum class ParsingTableActionType {shift, reduce};

class ParsingTableAction {
 private:
  ParsingTableActionType action_type_;
  int number_;

 public:
  ParsingTableAction() = default;
  ParsingTableAction(ParsingTableActionType action_type, int number)
    :action_type_{action_type}, number_{number}
    {}
  ~ParsingTableAction() = default;

  ParsingTableActionType get_action_type() {
    return action_type_;
  }
  int get_number() {
    return number_;
  }
};

class ParsingTableRow {
 private:
  std::unordered_map<std::string, ParsingTableAction> row_;
 public:
  ParsingTableRow() = default;
  ~ParsingTableRow() = default;

  ParsingTableAction operator[](const std::string& grammar_symbol) {
    return row_[grammar_symbol];
  }
  void add_symbol_action_pair(const std::string& grammar_symbol, ParsingTableAction action) {
    row_[grammar_symbol] = action;
  }
};

class ParsingTable {
 private:
  std::unordered_map<int, ParsingTableRow> table_;
  parser_grammar::Grammar grammar_;

  void add_entry(int state, const std::string& grammar_symbol, ParsingTableAction action) {
    auto row = table_[state];
    row.add_symbol_action_pair(grammar_symbol, action);
    table_[state] = row;
  }
 public:
  ParsingTable() = default;
  explicit ParsingTable(parser_grammar::Grammar grammar);
  ~ParsingTable() = default;
  ParsingTableRow operator[](int state) {
    return table_[state];
  }
};

}  // namespace parser_automaton

#endif  // PARSER_AUTOMATON_H_
