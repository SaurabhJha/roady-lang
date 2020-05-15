#include "./parser_common.h"

string Production::get_left_side()
{
  return left_side_;
}

vector<string> Production::get_right_side()
{
  return right_side_;
}

/**
 *
 * @param token_type
 * @return terminal_string
 * We have to define this function because we cannot have a unordered_map with
 * TokenType as key.
 */
string map_token_type_to_terminal(TokenType token_type)
{
  switch (token_type) {
    case (TokenType::id):
      return "id";
    case (TokenType::number):
      return "number";
    case (TokenType::star):
      return "*";
    case (TokenType::slash):
      return "/";
    case (TokenType::plus):
      return "+";
    case (TokenType::minus):
      return "-";
    case (TokenType::comma):
      return ",";
    case (TokenType::open_bracket):
      return "[";
    case (TokenType::close_bracket):
      return "]";
    case (TokenType::open_paren):
      return "(";
    case (TokenType::close_paren):
      return ")";
    case (TokenType::equals):
      return "=";
    case (TokenType::double_equals):
      return "==";
    case (TokenType::dollar):
      return "$";
    case (TokenType::invalid):
      return "";
  }
}

void StackRecord::set_state(int state)
{
  state_ = state;
}

int StackRecord::get_state()
{
  return state_;
}
