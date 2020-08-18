#include "gtest/gtest.h"

#include "tokenizer/tokenizer.h"

class TokenizerTest : public ::testing::Test {
 protected:
  tokenizer::Tokenizer tokenizer_for_lang;
};

TEST_F(TokenizerTest, SimpleNumericInput) {
  tokenizer_for_lang.tokenize("123");
  auto first_token = tokenizer_for_lang.get_next_token();

  EXPECT_EQ(first_token.get_token_type(), tokenizer::TokenType::number);
  EXPECT_EQ(tokenizer_for_lang.has_more(), false);
}

TEST_F(TokenizerTest, NumericAddition) {
  tokenizer_for_lang.tokenize("12143+132");
  auto first_token = tokenizer_for_lang.get_next_token();
  auto second_token = tokenizer_for_lang.get_next_token();
  auto third_token = tokenizer_for_lang.get_next_token();

  EXPECT_EQ(first_token.get_token_type(), tokenizer::TokenType::number);
  EXPECT_EQ(first_token.get_lexeme(), "12143");
  EXPECT_EQ(second_token.get_token_type(), tokenizer::TokenType::plus);
  EXPECT_EQ(second_token.get_lexeme(), "+");
  EXPECT_EQ(third_token.get_token_type(), tokenizer::TokenType::number);
  EXPECT_EQ(third_token.get_lexeme(), "132");
  EXPECT_EQ(tokenizer_for_lang.has_more(), false);
}

TEST_F(TokenizerTest, IdDoubleEqualsNumber) {
  tokenizer_for_lang.tokenize("adf2123==3123");
  auto first_token = tokenizer_for_lang.get_next_token();
  auto second_token = tokenizer_for_lang.get_next_token();
  auto third_token = tokenizer_for_lang.get_next_token();

  EXPECT_EQ(first_token.get_token_type(), tokenizer::TokenType::id);
  EXPECT_EQ(first_token.get_lexeme(), "adf2123");
  EXPECT_EQ(second_token.get_token_type(), tokenizer::TokenType::double_equals);
  EXPECT_EQ(second_token.get_lexeme(), "==");
  EXPECT_EQ(third_token.get_token_type(), tokenizer::TokenType::number);
  EXPECT_EQ(third_token.get_lexeme(), "3123");
  EXPECT_EQ(tokenizer_for_lang.has_more(), false);
}

TEST_F(TokenizerTest_NumericAddition_Test, ParenthesizedExpression) {
  tokenizer_for_lang.tokenize("(12*845)-(9098/54)");
  auto first_token = tokenizer_for_lang.get_next_token();
  auto second_token = tokenizer_for_lang.get_next_token();
  auto third_token = tokenizer_for_lang.get_next_token();
  auto fourth_token = tokenizer_for_lang.get_next_token();
  auto fifth_token = tokenizer_for_lang.get_next_token();
  auto sixth_token = tokenizer_for_lang.get_next_token();
  auto seventh_token = tokenizer_for_lang.get_next_token();
  auto eighth_token = tokenizer_for_lang.get_next_token();
  auto ninth_token = tokenizer_for_lang.get_next_token();
  auto tenth_token = tokenizer_for_lang.get_next_token();
  auto eleventh_token = tokenizer_for_lang.get_next_token();

  EXPECT_EQ(first_token.get_token_type(),
      tokenizer::TokenType::open_paren);
  EXPECT_EQ(first_token.get_lexeme(), "(");
  EXPECT_EQ(second_token.get_token_type(),
      tokenizer::TokenType::number);
  EXPECT_EQ(second_token.get_lexeme(),
      "12");
  EXPECT_EQ(third_token.get_token_type(),
      tokenizer::TokenType::star);
  EXPECT_EQ(third_token.get_lexeme(), "*");
  EXPECT_EQ(fourth_token.get_token_type(),
      tokenizer::TokenType::number);
  EXPECT_EQ(fourth_token.get_lexeme(), "845");
  EXPECT_EQ(fifth_token.get_token_type(), tokenizer::TokenType::closed_paren);
  EXPECT_EQ(fifth_token.get_lexeme(), ")");
  EXPECT_EQ(sixth_token.get_token_type(), tokenizer::TokenType::minus);
  EXPECT_EQ(sixth_token.get_lexeme(), "-");
  EXPECT_EQ(seventh_token.get_token_type(), tokenizer::TokenType::open_paren);
  EXPECT_EQ(seventh_token.get_lexeme(), "(");
  EXPECT_EQ(eighth_token.get_token_type(), tokenizer::TokenType::number);
  EXPECT_EQ(eighth_token.get_lexeme(), "9098");
  EXPECT_EQ(ninth_token.get_token_type(), tokenizer::TokenType::slash);
  EXPECT_EQ(ninth_token.get_lexeme(), "/");
  EXPECT_EQ(tenth_token.get_token_type(), tokenizer::TokenType::number);
  EXPECT_EQ(tenth_token.get_lexeme(), "54");
  EXPECT_EQ(eleventh_token.get_token_type(),
      tokenizer::TokenType::closed_paren);
  EXPECT_EQ(eleventh_token.get_lexeme(), ")");
  EXPECT_EQ(tokenizer_for_lang.has_more(), false);
}
