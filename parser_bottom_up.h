#ifndef ROADY_LANG_PARSER_BOTTOM_UP_H_
#define ROADY_LANG_PARSER_BOTTOM_UP_H_

#include <stack>
#include <string>
#include <unordered_map>

#include "./parser_common.h"
using namespace std;

enum class BottomUpParsingActionType { shift, reduce, accept };

class BottomUpParsingAction {
 private:
  BottomUpParsingActionType action_type_;
  // To be interpreted as next state if the action type is shift
  // and
  // number if action type is reduce.
  int action_number_;

 public:
  BottomUpParsingAction() = default;
  BottomUpParsingAction(BottomUpParsingActionType action_type, int action_number)
      :action_type_{action_type}, action_number_{action_number} {}
  ~BottomUpParsingAction() = default;

  BottomUpParsingActionType get_action_type() { return action_type_; }
  int get_action_number() { return action_number_; }
};

class BottomUpParsingActionTableRow {
 private:
  unordered_map<string, BottomUpParsingAction> row_;

 public:
  BottomUpParsingActionTableRow() = default;
  ~BottomUpParsingActionTableRow() = default;

  void add_terminal_action_pair(const string& terminal, BottomUpParsingAction action);
  BottomUpParsingAction operator[](const string& terminal);
};

class BottomUpParsingActionTable {
 private:
  unordered_map<int, BottomUpParsingActionTableRow> table_;

 public:
  BottomUpParsingActionTable() = default;
  ~BottomUpParsingActionTable() = default;

  void add_rule(int state, const string& terminal, BottomUpParsingAction action);
  BottomUpParsingActionTableRow operator[](int state);
};

class BottomUpParsingGotoTableRow {
 private:
  unordered_map<string, int> row_;
 public:
  void add_nonterminal_state_pair(const string& nonterminal, int state);
  int operator[](const string& nonterminal);
};


class BottomUpParsingGotoTable {
 private:
  unordered_map<int, BottomUpParsingGotoTableRow> table_;

 public:
  BottomUpParsingGotoTable() = default;
  ~BottomUpParsingGotoTable() = default;

  void add_rule(int state, const string& nonterminal, int new_state);
  BottomUpParsingGotoTableRow operator[](int state);
};

class BottomUpParser {
 private:
  unordered_map<int, Production> numbered_reductions_;
  BottomUpParsingActionTable action_table_;
  BottomUpParsingGotoTable goto_table_;
  stack<StackRecord> stack_;
  vector<Production> reductions_applied_;
  bool has_failed_;

  void construct_productions();
  void construct_action_table();
  void construct_goto_table();
  void shift(BottomUpParsingAction next_action);
  BottomUpParsingAction reduce(BottomUpParsingAction next_action, string terminal);
 public:
  BottomUpParser()
  {
    construct_productions();
    construct_action_table();
    construct_goto_table();
    StackRecord initial_stack_record = StackRecord(StackRecordType::state);
    stack_.push(initial_stack_record);
  }
  ~BottomUpParser() = default;

  void parse_next_token(Token token);
  bool has_failed();
  vector<Production> get_reductions_applied();
};

#endif //ROADY_LANG_PARSER_BOTTOM_UP_H_