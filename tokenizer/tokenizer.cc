#include "tokenizer/tokenizer.h"

#include <utility>

namespace tokenizer {

TokenType Token::get_token_type() {
  return token_type_;
}

std::string Token::get_lexeme() {
  return lexeme_;
}

Tokenizer::Tokenizer()
    :current_input_idx_{0}, has_more_{false} {
  RegularExpression regex_for_id("(a|b|c|d|e|f|g|h|i|j|k|l"
                                 "|m|n|o|p|q|r|s|t|u|v|w|x|y|z)"
                                 "(a|b|c|d|e|f|g|h|i|j|k|l|m|"
                                 "n|o|p|q|r|s|t|u|v|w|x|y|z"
                                 "|0|1|2|3|4|5|6|7|8|9)*");
  RegularExpression regex_for_number("(0|1|2|3|4|5|6|7|8|9)"
                                     "(0|1|2|3|4|5|6|7|8|9)*");
  RegularExpression regex_for_plus("+");
  RegularExpression regex_for_minus("-");
  RegularExpression regex_for_star("*");
  RegularExpression regex_for_slash("/");
  RegularExpression regex_for_equals("=");
  RegularExpression regex_for_double_equals("==");
  RegularExpression regex_for_open_paren("(");
  RegularExpression regex_for_closed_paren(")");

  regular_expressions_.push_back(regex_for_id);
  token_types_.push_back(TokenType::id);
  regular_expressions_.push_back(regex_for_number);
  token_types_.push_back(TokenType::number);
  regular_expressions_.push_back(regex_for_plus);
  token_types_.push_back(TokenType::plus);
  regular_expressions_.push_back(regex_for_minus);
  token_types_.push_back(TokenType::minus);
  regular_expressions_.push_back(regex_for_star);
  token_types_.push_back(TokenType::star);
  regular_expressions_.push_back(regex_for_slash);
  token_types_.push_back(TokenType::slash);
  regular_expressions_.push_back(regex_for_equals);
  token_types_.push_back(TokenType::equals);
  regular_expressions_.push_back(regex_for_double_equals);
  token_types_.push_back(TokenType::double_equals);
  regular_expressions_.push_back(regex_for_open_paren);
  token_types_.push_back(TokenType::open_paren);
  regular_expressions_.push_back(regex_for_closed_paren);
  token_types_.push_back(TokenType::closed_paren);
}

void Tokenizer::tokenize(std::string input) {
  input_ = std::move(input);
}

Token Tokenizer::get_next_token() {
  std::string lexeme;
  TokenType token_type = TokenType::invalid;

  auto input_to_match = input_.substr(current_input_idx_);
  for (auto idx = 0; idx < regular_expressions_.size(); ++idx) {
    auto regex = regular_expressions_[idx];
    auto candidate_lexeme = regex.match(input_to_match);
    if (candidate_lexeme.size() > lexeme.size()) {
      lexeme = candidate_lexeme;
      token_type = token_types_[idx];
    }
  }

  current_input_idx_ = current_input_idx_ + lexeme.size();
  if (lexeme.empty() || current_input_idx_ == input_.size()) {
    has_more_ = false;
  }

  Token token(token_type, lexeme);
  return token;
}

bool Tokenizer::has_more() {
  return has_more_;
}

}  // namespace tokenizer
