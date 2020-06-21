#ifndef REGEX_H_
#define REGEX_H_

#include <string>

#include "tokenizer/common.h"
#include "tokenizer/finite_automata.h"

namespace regex {

finite_automata::FiniteAutomata compile_regex_to_automata(const std::string& regex_string);
std::string get_first_operand(std::string regex_string);
common_tokenizer::RegexOperatorType get_operator(std::string regex_string);
std::string get_second_operand(const std::string& regex_string);

}

#endif // REGEX_H_