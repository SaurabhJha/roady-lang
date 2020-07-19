#ifndef TOKENIZER_COMMON_H_
#define TOKENIZER_COMMON_H_

#include <string>

namespace tokenizer_common {

enum class RegexOperatorType { concat, exp, unio };
std::string trim_parenthesis(std::string regex_string);
int find_matching_parenthesis_index(
    std::string regex_string, int parenthesis_idx);

}  // namespace tokenizer_common

#endif  // TOKENIZER_COMMON_H_”
