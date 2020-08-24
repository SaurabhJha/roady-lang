#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "parser/grammar.h"

namespace parser {

class LRItem {
 private:
  parser::Production production_;
  int position_in_body_{};

 public:
  LRItem() = default;
  LRItem(const parser::Production& production, int position_in_body)
    :production_{production}, position_in_body_{position_in_body}
  {}
  ~LRItem() = default;

  parser::Production get_production();
  int get_position_in_body();
  bool has_position_at_the_end();
  std::string get_next_symbol();
};

bool operator==(LRItem lr_item_lhs, LRItem lr_item_rhs);

class LRItemSet {
 private:
  std::vector<LRItem> items_;

 public:
  LRItemSet() = default;
  explicit LRItemSet(std::vector<LRItem> items)
    :items_{std::move(items)}
  {}
  LRItemSet(const parser::LRItem& lr_item, parser::Grammar grammar);
  LRItemSet(
      LRItemSet item_set,
      const std::string& transition_symbol,
      const parser::Grammar& grammar);
  ~LRItemSet() = default;

  std::vector<LRItem> get_items();
  std::vector<std::pair<std::string, LRItemSet>> get_transitions(
      const parser::Grammar& grammar);
};

bool operator==(LRItemSet lr_item_set_lhs, LRItemSet lr_item_set_rhs);

enum class ParsingActionType {shift, reduce, accept, error};

class ParsingAction {
 private:
  ParsingActionType action_type_;
  int number_;

 public:
  ParsingAction() = default;
  ParsingAction(ParsingActionType action_type, int number)
    :action_type_{action_type}, number_{number}
  {}
  ~ParsingAction() = default;
};

class ParsingTableRow {
 private:
  std::unordered_map<std::string, ParsingAction> row_;

 public:
  ParsingTableRow() = default;
  ~ParsingTableRow() = default;

  void add_symbol_action_pair(const std::string& symbol, ParsingAction action);
  ParsingAction operator[](const std::string& symbol);
};

class ParsingTable {
 private:
  std::unordered_map<int, ParsingTableRow> table_;

 public:
  ParsingTable() = default;
  ~ParsingTable() = default;

  void add_new_entry(int state, const std::string& symbol, ParsingAction);
  ParsingTableRow operator[](int state);
};

class Parser {
 private:
  ParsingTable table_;
  std::deque<int> stack_;

 public:
  Parser() = default;
  explicit Parser(Grammar grammar);
  ~Parser() = default;
};

}  // namespace parser

#endif  // PARSER_PARSER_H_
