#include "tokeniser/regex.h"

namespace regex {

/**
 * Given a valid regex string, construct an equivalent finite automaton using inductive
 * definition.
 *
 * @param regex_string
 * @return
 */
finite_automata::FiniteAutomata compile_regex_to_automata(const std::string& regex_string)
{
  // First operand's automaton.
  auto first_operand = get_first_operand(regex_string);
  first_operand = common_tokeniser::trim_parenthesis(first_operand);
  finite_automata::FiniteAutomata first_automata;
  if (first_operand.length() > 1)
    first_automata = compile_regex_to_automata(first_operand);
  else
    first_automata.add_new_transition(1, 2, first_operand);

  // If the regex is a single character, we are done.
  if (regex_string.length() == 1) return first_automata;

  auto op =  get_operator(regex_string);
  // If the operator is exponentiation, there is no second operand.
  if (op == common_tokeniser::RegexOperatorType::exp) return first_automata.compute_exponentiation();

  // Second operand's automaton.
  auto second_operand = common_tokeniser::trim_parenthesis(get_second_operand(regex_string));
  finite_automata::FiniteAutomata second_automata;
  if (second_operand.length() > 1)
    second_automata = compile_regex_to_automata(second_operand);
  else
    second_automata.add_new_transition(1, 2, second_operand);

  // Combine both operands.
  if (op == common_tokeniser::RegexOperatorType::unio)
    return first_automata.union_with(second_automata);
  else
    return first_automata.concatenate_with(second_automata);
}

std::string get_first_operand(std::string regex_string)
{
  auto substring_length = 0;
  if (regex_string[0] == '(') {
    substring_length = common_tokeniser::find_matching_parenthesis_index(regex_string, 0);
  } else
    substring_length = 1;
  return regex_string.substr(0, substring_length);
}

common_tokeniser::RegexOperatorType get_operator(std::string regex_string)
{
  auto first_operand = get_first_operand(regex_string);
  switch (regex_string[first_operand.length()]) {
    case '|': return common_tokeniser::RegexOperatorType::unio;
    case '*': return common_tokeniser::RegexOperatorType::exp;
    default: return common_tokeniser::RegexOperatorType::concat;
  }
}

std::string get_second_operand(const std::string& regex_string)
{
  auto first_operand = get_first_operand(regex_string);
  auto op = get_operator(regex_string);
  auto op_length = 0;

  if (op == common_tokeniser::RegexOperatorType::unio || op == common_tokeniser::RegexOperatorType::exp)
    op_length = 1;
  else
    op_length = 0;
  return regex_string.substr(first_operand.length()+op_length, (regex_string.length() - first_operand.length() - op_length));
}

}