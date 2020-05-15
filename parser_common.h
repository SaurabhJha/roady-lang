#ifndef ROADY_LANG_PARSER_COMMON_H_
#define ROADY_LANG_PARSER_COMMON_H_

#include <iostream>
#include <string>
#include <utility>
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
      :left_side_{std::move(left_side)}, right_side_{std::move(right_side)} {}
  ~Production() = default;

  bool is_empty() { return left_side_.empty() && right_side_.empty(); }
  string get_left_side();
  vector<string> get_right_side();
  void print()
  {
    cout << left_side_ << "-> ";
    for (auto symbol : right_side_)
      cout << symbol << " ";
  }
};

string map_token_type_to_terminal(TokenType token_type);

#endif //ROADY_LANG_PARSER_COMMON_H_