#include "tokeniser/tokeniser.h"

#include <iostream>
#include <vector>

#include "tokeniser/regex.h"

namespace tokeniser
{

std::string Tokeniser::apply_automata(finite_automata::FiniteAutomata automaton)
{
  int accepted_at = 0;
  int input_idx = current_input_idx_;
  auto input_length = input_.length();
  while (input_idx < input_length) {
    automaton.move(std::string(1, input_[input_idx]));
    if (automaton.has_accepted()) {
      accepted_at = input_idx;
    }
    if (automaton.has_died()) break;
    ++input_idx;
  }
  if (accepted_at == 0)
    return "";
  else
    return input_.substr(current_input_idx_, accepted_at - current_input_idx_ + 1);
}

/**
 * We turn automata in turn and return the token corresponding to the automaton which matched the string with the
 * maximum length. Its an implementation of maximal munch policy.
 */
void tokeniser::Tokeniser::get_next_token()
{
  std::string single_alphabet_regex = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
  std::string single_number_regex = "(1|2|3|4|5|6|7|8|9|0)";
  auto id_regex = "(" + single_alphabet_regex + ")" + "(" + "(" + single_alphabet_regex + "|" + single_number_regex + ")*" + ")";
  auto number_regex = single_number_regex + "*";

  auto automata_for_id = regex::compile_regex_to_automata(id_regex);
  auto automata_for_number = regex::compile_regex_to_automata(number_regex);
  auto automata_for_plus = regex::compile_regex_to_automata("+");
  auto automata_for_star = regex::compile_regex_to_automata("*");
  auto automata_for_equals = regex::compile_regex_to_automata("=");
  auto automata_for_double_equals = regex::compile_regex_to_automata("==");

  std::vector<finite_automata::FiniteAutomata> automata_vector = {
      automata_for_id, automata_for_number, automata_for_plus, automata_for_star, automata_for_equals,
      automata_for_double_equals,
  };

  std::string lexeme;
  for (const auto& current_automata : automata_vector) {
    auto candidate_lexeme = apply_automata(current_automata);
    if (candidate_lexeme.length() > lexeme.length())
      lexeme = candidate_lexeme;
  }

  auto lexeme_length = lexeme.length();
  std::cout << lexeme << "\n";
  if (lexeme_length > 0)
    current_input_idx_ += lexeme_length;

}

}