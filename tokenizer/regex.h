#ifndef TOKENIZER_REGEX_H_
#define TOKENIZER_REGEX_H_

#include <string>

#include "tokenizer/common.h"
#include "tokenizer/finite_automata.h"

namespace tokenizer_regex {

tokenizer_finite_automata::FiniteAutomata compile_regex_to_automata(
    const std::string& regex_string);
std::string get_first_operand(std::string regex_string);
tokenizer_common::RegexOperatorType get_operator(std::string regex_string);
std::string get_second_operand(const std::string& regex_string);

}

#endif  // TOKENIZER_REGEX_H_
