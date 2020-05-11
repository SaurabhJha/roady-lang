#ifndef ROADY_LANG_PARSER_H_
#define ROADY_LANG_PARSER_H_

#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./lexical_analyzer.h"
using namespace std;

/**
 * Data structure to represent a production of the form
 *    A -> XYZ
 * where 'A' is called the left side of the production and 'XYZ' called the right side of the production.
 */
class Production {
 private:
  string left_side_;
  vector<string> right_side_;

 public:
  Production() = default;
  Production(string left_side, vector<string> right_side)
    :left_side_{left_side}, right_side_{right_side} {}
  ~Production() = default;

  bool is_empty() { return left_side_ == "" && right_side_.size() == 0; }
  vector<string> get_right_side();
  void print()
  {
    cout << left_side_ << "-> ";
    for (auto symbol : right_side_)
      cout << symbol << " ";
  }
};

/**
 * Data structure to represent an LL(1) parsing table row like this
 *          |term1|term2|term3|term4|
 * ---------|-----|-----|-----|-----|
 * nonterm1 |prod1|prod2|prod3|prod4|
 *
 * where nonterm1 is immaterial and not part of the data structure.
 * What we are looking for is a hash table where the keys are terminal symbols and values
 * are productions. So the above row would look like this
 *    { term1: prod1, term2: prod2, term3: prod3, term4: prod4 }
 */
class TopDownParsingTableRow {
 private:
  unordered_map<string, Production> row_;

 public:
  TopDownParsingTableRow() = default;
  ~TopDownParsingTableRow() = default;

  void add_terminal_production_pair(string terminal, Production production);
  Production operator[](string terminal);
};

/**
 * Data structure to represent an LL(1) parsing table like this
 *          |term1|term2|term3|term4|
 * ---------|-----|-----|-----|-----|
 * nonterm1 |prod1|     |prod3|     |
 * nonterm2 |     |prod3|     |prod4|
 *
 * What we are looking for is something where the API should look like this
 *     parse_tabletable[nonterm][term] = production
 */
class TopDownParsingTable {
 private:
  unordered_set<string> non_terminals_;
  unordered_set<string> terminals_;
  unordered_map<string, TopDownParsingTableRow> table_;

 public:
  TopDownParsingTable() = default;
  TopDownParsingTable(unordered_set<string> non_terminals, unordered_set<string> terminals)
    :non_terminals_{non_terminals}, terminals_{terminals}
  {
    for (string non_terminal : non_terminals_) {
      TopDownParsingTableRow row;
      table_[non_terminal] = row;
    }
  }
  ~TopDownParsingTable() = default;

  void add_rule(string non_terminal, string terminal, Production production);
  bool is_non_terminal(string symbol);
  bool is_terminal(string symbol);
  TopDownParsingTableRow operator[](string non_terminal);
};

string map_token_type_to_terminal(TokenType token_type);

/**
 * An LL(1) parser implementation. It contains all the state necessary to incrementally process tokens
 * output by the tokenizer.
 */
class TopDownParser {
 private:
  stack<string> stack_;
  TopDownParsingTable parse_table_;
  vector<Production> productions_applied_;
  bool has_failed_;

  void apply_production(Production production);
  TopDownParsingTable construct_top_down_parse_table();

 public:
  TopDownParser()
  {
    parse_table_ = construct_top_down_parse_table();
    // Initialize the stack with start symbol of the grammar and end of the input symbol '$'
    stack_.push("$");
    stack_.push("expr");
    productions_applied_ = {};
    has_failed_ = false;
  }
  ~TopDownParser() = default;

  void parse_next_token(Token token);
  bool has_failed();
  vector<Production> get_productions_applied();
};

#endif //ROADY_LANG_PARSER_H_
