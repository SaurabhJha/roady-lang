#ifndef ROADY_LANG_PARSER_H_
#define ROADY_LANG_PARSER_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

class Production {
 private:
  string left_side_;
  vector<string> right_side_;
 public:
  Production() = default;
  Production(string left_side, vector<string> right_side)
    :left_side_{left_side}, right_side_{right_side} {}
  ~Production() = default;

  vector<string> apply();
};

class ParseTableRow {
 private:
  unordered_map<string, Production> row_;
 public:
  ParseTableRow() = default;
  ~ParseTableRow() = default;

  void add_terminal_production_pair(string terminal, Production production);
  Production operator[](string terminal);
};

class ParseTable {
 private:
  unordered_set<string> non_terminals_;
  unordered_map<string, ParseTableRow> table_;
 public:
  ParseTable()
  {
    for (string non_terminal : non_terminals_) {
      ParseTableRow row;
      table_[non_terminal] = row;
    }
  }
  ~ParseTable() = default;

  void add_rule(string non_terminal, string terminal, Production production);
  ParseTableRow operator[](string non_terminal);
};

#endif //ROADY_LANG_PARSER_H_
