#ifndef COMMON_H_
#define COMMON_H_

#include <string>

namespace common_tokeniser
{

enum class RegexOperatorType { concat, exp, unio };
std::string trim_parenthesis(std::string regex_string);
int find_matching_parenthesis_index(std::string regex_string, int parenthesis_idx);

}

#endif // COMMON_H_”