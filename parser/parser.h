#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "parser/grammar.h"
#include "tokenizer/tokenizer.h"

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

  ParsingActionType get_action_type() {
    return action_type_;
  }
  int get_number() {
    return number_;
  }
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

std::string map_token_type_to_terminal(tokenizer::TokenType token_type);

class Parser {
 private:
  ParsingTable table_;
  Grammar grammar_;
  std::deque<int> stack_;
  bool has_accepted_;
  bool is_stuck_;
  std::vector<tokenizer::Token> input_;
  int current_position_;

 public:
  Parser() = default;
  explicit Parser(Grammar grammar);
  ~Parser() = default;

  void parse(std::vector<tokenizer::Token> tokens);
  std::pair<ParsingActionType, Production> make_next_move();
  bool has_accepted() {
    return has_accepted_;
  }
  bool is_stuck() {
    return is_stuck_;
  }
};

}  // namespace parser

#endif  // PARSER_PARSER_H_
