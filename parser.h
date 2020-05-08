#ifndef ROADY_LANG_PARSER_H_
#define ROADY_LANG_PARSER_H_

#include <iostream>
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
  vector<string> apply();
  void print()
  {
    cout << left_side_ << "-> ";
    for (auto symbol : right_side_)
      cout << symbol << " ";
  }
};

/**
 * Data structure to represent a parse table row like this
 *          |term1|term2|term3|term4|
 * ---------|-----|-----|-----|-----|
 * nonterm1 |prod1|prod2|prod3|prod4|
 *
 * where nonterm1 is immaterial and not part of the data structure.
 * What we are looking for is a hash table where the keys are terminal symbols and values
 * are productions. So the above row would look like this
 *    { term1: prod1, term2: prod2, term3: prod3, term4: prod4 }
 */
class ParseTableRow {
 private:
  unordered_map<string, Production> row_;

 public:
  ParseTableRow() = default;
  ~ParseTableRow() = default;

  void add_terminal_production_pair(string terminal, Production production);
  Production operator[](string terminal);
};

/**
 * Data structure to represent a parse table like this
 *          |term1|term2|term3|term4|
 * ---------|-----|-----|-----|-----|
 * nonterm1 |prod1|     |prod3|     |
 * nonterm2 |     |prod3|     |prod4|
 *
 * What we are looking for is something where the API should look like this
 *     parse_tabletable[nonterm][term] = production
 */
class ParseTable {
 private:
  unordered_set<string> non_terminals_;
  unordered_set<string> terminals_;
  unordered_map<string, ParseTableRow> table_;

 public:
  ParseTable(unordered_set<string> non_terminals, unordered_set<string> terminals)
    :non_terminals_{non_terminals}, terminals_{terminals}
  {
    for (string non_terminal : non_terminals_) {
      ParseTableRow row;
      table_[non_terminal] = row;
    }
  }
  ~ParseTable() = default;

  void add_rule(string non_terminal, string terminal, Production production);
  bool is_non_terminal(string symbol);
  bool is_terminal(string symbol);
  ParseTableRow operator[](string non_terminal);
};

string map_token_type_to_terminal(TokenType token_type);

ParseTable construct_parse_table();

vector<Production> parse(vector<Token> tokens);

#endif //ROADY_LANG_PARSER_H_
