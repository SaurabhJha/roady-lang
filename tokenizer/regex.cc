#include "tokenizer/regex.h"

namespace tokenizer_regex {

/**
 * Given a valid tokenizer_regex string, construct an equivalent finite automaton using inductive
 * definition.
 *
 * @param regex_string
 * @return
 */
tokenizer_finite_automata::FiniteAutomata compile_regex_to_automata(
    const std::string& regex_string) {
  // First operand's automaton.
  auto first_operand = get_first_operand(regex_string);
  first_operand = tokenizer_common::trim_parenthesis(first_operand);
  tokenizer_finite_automata::FiniteAutomata first_automata;
  if (first_operand.length() > 1)
    first_automata = compile_regex_to_automata(first_operand);
  else
    first_automata.add_new_transition(1, 2, first_operand);

  // If the tokenizer_regex is a single character, we are done.
  if (regex_string.length() == 1) return first_automata;

  auto op =  get_operator(regex_string);
  // If the operator is exponentiation, there is no second operand.
  if (op == tokenizer_common::RegexOperatorType::exp)
    return first_automata.compute_exponentiation();

  // Second operand's automaton.
  auto second_operand =
      tokenizer_common::trim_parenthesis(get_second_operand(regex_string));
  tokenizer_finite_automata::FiniteAutomata second_automata;
  if (second_operand.length() > 1)
    second_automata = compile_regex_to_automata(second_operand);
  else
    second_automata.add_new_transition(1, 2, second_operand);

  // Combine both operands.
  if (op == tokenizer_common::RegexOperatorType::unio)
    return first_automata.union_with(second_automata);
  else
    return first_automata.concatenate_with(second_automata);
}

std::string get_first_operand(std::string regex_string) {
  auto substring_length = 0;
  if (regex_string[0] == '(') {
    substring_length = tokenizer_common::find_matching_parenthesis_index(
        regex_string, 0);
  } else {
    substring_length = 1;
  }
  return regex_string.substr(0, substring_length);
}

tokenizer_common::RegexOperatorType get_operator(std::string regex_string) {
  auto first_operand = get_first_operand(regex_string);
  switch (regex_string[first_operand.length()]) {
    case '|': return tokenizer_common::RegexOperatorType::unio;
    case '*': return tokenizer_common::RegexOperatorType::exp;
    default: return tokenizer_common::RegexOperatorType::concat;
  }
}

std::string get_second_operand(const std::string& regex_string) {
  auto first_operand = get_first_operand(regex_string);
  auto op = get_operator(regex_string);
  auto op_length = 0;

  if (op == tokenizer_common::RegexOperatorType::unio || op ==
      tokenizer_common::RegexOperatorType::exp)
    op_length = 1;
  else
    op_length = 0;
  return regex_string.substr(first_operand.length()+op_length,
      (regex_string.length() - first_operand.length() - op_length));
}

}  // namespace tokenizer_regex
