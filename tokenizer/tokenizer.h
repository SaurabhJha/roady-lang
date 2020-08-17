#ifndef TOKENIZER_TOKENIZER_H_
#define TOKENIZER_TOKENIZER_H_

#include <string>
#include <utility>
#include <vector>

#include "tokenizer/regular_expression.h"

namespace tokenizer {

enum class TokenType {
  id, number, plus, minus, star, slash, equals,
  double_equals, invalid, open_paren, closed_paren };

class Token {
 private:
  TokenType token_type_;
  std::string lexeme_;

 public:
  Token(TokenType token_type, std::string lexeme)
    :token_type_{token_type}, lexeme_{std::move(lexeme)}
  {}
  ~Token() = default;
  TokenType get_token_type();
  std::string get_lexeme();
};

class Tokenizer {
 private:
  std::vector<RegularExpression> regular_expressions_;
  std::vector<TokenType> token_types_;
  std::string input_;
  int current_input_idx_;
  bool has_more_;

 public:
  Tokenizer();
  ~Tokenizer() = default;
  void tokenize(std::string input);
  Token get_next_token();
  bool has_more();
};

}  // namespace tokenizer

#endif  // TOKENIZER_TOKENIZER_H_
