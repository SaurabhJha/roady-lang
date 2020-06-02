#include "tokeniser/common.h"

#include <stack>

namespace common_tokeniser
{

/**
 * Trims surrounding parenthesis from an expression. Assumes that the outermost parenthesis are balanced.
 *
 * For example, given ((a|b)(c|d)), it will return (a|b)(c|d).
 */
std::string trim_parenthesis(std::string regex_string)
{
  if (regex_string[0] == '(') return regex_string.substr(1, regex_string.length() - 2);
  else return regex_string;
}

/**
 * Given a string and an index of the open parenthesis, finds the matching closed parenthesis
 * and returns its index. Assumes that all parenthesis are balanced.
 *
 * It does this with a simple implementation of a push-down automata.
 * @param regex_string
 * @param parenthesis_idx
 * @return
 */
int find_matching_parenthesis_index(std::string regex_string, int parenthesis_idx)
{
  std::stack<std::string> parenthesis_stack;
  parenthesis_stack.push("(");
  auto matching_parenthesis_idx = parenthesis_idx + 1;
  while (!parenthesis_stack.empty() && matching_parenthesis_idx < regex_string.length()) {
    if (regex_string[matching_parenthesis_idx] == '(')
      // New open parenthesis. Push down to the stack.
      parenthesis_stack.push("(");
    else if(regex_string[matching_parenthesis_idx] == ')')
      // Closed parenthesis. Pop the matching open parenthesis from the stack.
      parenthesis_stack.pop();
    ++matching_parenthesis_idx;
  }

  if (parenthesis_stack.empty()) return matching_parenthesis_idx;
  else return 0;
}

}