#ifndef TOKENIZER_REGULAR_EXPRESSION_H_
#define TOKENIZER_REGULAR_EXPRESSION_H_

#include <string>
#include <utility>

#include "tokenizer/finite_automaton.h"

namespace tokenizer {

enum class RegularExpressionOperatorType { unio, concat, star };

class RegularExpression {
 private:
  std::string expression_string_;
  std::string first_operand_;
  RegularExpressionOperatorType operator_;
  std::string second_operand_;
  tokenizer::DeterministicFiniteAutomaton automaton_;

  std::string get_first_operand();
  RegularExpressionOperatorType get_operator();
  std::string get_second_operand();
  NonDeterministicFiniteAutomaton convert_to_nfa();

 public:
  RegularExpression() = default;
  explicit RegularExpression(std::string expression_string);
  ~RegularExpression() = default;
  std::string match(const std::string& input);
};

int get_matching_parenthesis_index(std::string input);
std::string trim_parenthesis(const std::string& input);

};  // namespace tokenizer

#endif  // TOKENIZER_REGULAR_EXPRESSION_H_
