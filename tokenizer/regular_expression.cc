#include "tokenizer/regular_expression.h"

#include <deque>

namespace tokenizer {

RegularExpression::RegularExpression(std::string expression_string)
    :expression_string_{std::move(expression_string)} {
  first_operand_ = get_first_operand();
  operator_ = get_operator();
  second_operand_ = get_second_operand();
  auto nfa = convert_to_nfa();
  auto dfa = nfa.convert_to_dfa();
  automaton_ = dfa;
}

std::string RegularExpression::get_first_operand() {
  if (expression_string_.size() == 1) {
    return expression_string_;
  } else if (expression_string_[0] != '(') {
    auto first_operand_length = expression_string_.size() / 2;
    first_operand_ = expression_string_.substr(0, first_operand_length);
    if (first_operand_[first_operand_.size() - 1] == '|') {
      first_operand_ = first_operand_.substr(0, first_operand_.size() - 1);
    }
  } else {
    auto matching_parenthesis_idx = get_matching_parenthesis_index(expression_string_);
    first_operand_ = expression_string_.substr(0, matching_parenthesis_idx);
  }

  return first_operand_;
}

RegularExpressionOperatorType RegularExpression::get_operator() {
  auto first_operand_length = first_operand_.size();

  if (expression_string_[first_operand_length] == '*') {
    operator_ = RegularExpressionOperatorType::star;
  } else if (expression_string_[first_operand_length] == '|') {
    operator_ = RegularExpressionOperatorType::unio;
  } else {
    operator_ = RegularExpressionOperatorType::concat;
  }

  return operator_;
}

std::string RegularExpression::get_second_operand() {
  int operator_length;
  if (operator_ == RegularExpressionOperatorType::unio || operator_ == RegularExpressionOperatorType::star) {
    operator_length = 1;
  } else {
    operator_length = 0;
  }

  auto second_operand_start_index = first_operand_.size() + operator_length;
  auto second_operand_length = expression_string_.size() - second_operand_start_index;
  second_operand_ = expression_string_.substr(second_operand_start_index, second_operand_length);

  return second_operand_;
}

NonDeterministicFiniteAutomaton RegularExpression::convert_to_nfa() {
  if (expression_string_.length() == 1) {
    NonDeterministicFiniteAutomaton nfa(expression_string_);
    return nfa;
  }

  auto first_operand_string = get_first_operand();
  first_operand_string = trim_parenthesis(first_operand_string);
  RegularExpression first_operand_regex(first_operand_string);
  auto first_operand_nfa = first_operand_regex.convert_to_nfa();

  auto regex_operator = get_operator();
  if (regex_operator != RegularExpressionOperatorType::star) {
    auto second_operand_string = get_second_operand();
    second_operand_string = trim_parenthesis(second_operand_string);
    RegularExpression second_operand_regex(second_operand_string);
    auto second_operand_nfa = second_operand_regex.convert_to_nfa();

    if (operator_ == RegularExpressionOperatorType::unio) {
      first_operand_nfa.merge_on_union(second_operand_nfa);
    } else if (operator_ == RegularExpressionOperatorType::concat) {
      first_operand_nfa.merge_on_concatenation(second_operand_nfa);
    }
  } else {
    first_operand_nfa.apply_star();
  }
  return first_operand_nfa;
}

std::string RegularExpression::match(const std::string& input) {
  automaton_.reset();
  auto match_idx = -1;
  for (auto idx = 0; idx < input.size(); ++idx) {
    auto input_character_string = std::string(1, input[idx]);
    automaton_.move(input_character_string);
    if (automaton_.has_accepted()) {
      match_idx = idx;
    }
    if (automaton_.is_dead()) {
      break;
    }
  }

  if (match_idx == -1)
    return "";
  else if (match_idx == 0){
    return std::string(1, input[0]);
  } else {
    return input.substr(0, match_idx + 1);
  }
}

int get_matching_parenthesis_index(std::string input) {
  // If the first character is a parenthesis, match it using stack.
  std::deque<char> stack {input[0]};
  for (auto idx = 1; idx < input.size(); ++idx) {
    if (stack.empty()) {
      return idx;
    } else if (input[idx] == ')') {
      stack.pop_front();
    } else if (input[idx] == '(') {
      stack.push_front(input[idx]);
    }
  }

  if (stack.empty()) {
    return input.size() - 1;
  } else {
    return -1;
  }
}

std::string trim_parenthesis(const std::string& input) {
  if (input[0] == '(' && input[input.size() - 1] == ')')
    return input.substr(1, input.size() - 2);
  else
    return input;
}

};