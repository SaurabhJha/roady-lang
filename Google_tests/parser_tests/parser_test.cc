#include "gtest/gtest.h"

#include "parser/parser.h"
#include "tokenizer/tokenizer.h"

class ParserTest : public ::testing::Test {
 protected:
  parser::Production start_production;
  parser::Production expr_plus_production;
  parser::Production expr_term_production;
  parser::Production term_star_production;
  parser::Production term_factor_production;
  parser::Production factor_number_production;
  parser::Production factor_paran_production;
  std::vector<parser::Production> productions;
  parser::Grammar grammar;
  parser::Parser parser;
  tokenizer::Tokenizer tok;

  void SetUp() override {
    start_production = parser::Production("expr'", {"expr"});
    expr_plus_production = parser::Production(
        "expr", {"expr", "+", "term"});
    expr_term_production = parser::Production("expr", {"term"});
    term_star_production = parser::Production(
        "term", {"term", "*", "factor"});
    term_factor_production = parser::Production("term", {"factor"});
    factor_number_production = parser::Production(
        "factor", {"number"});
    factor_paran_production = parser::Production(
        "factor", {"(", "expr", ")"});
    productions = {
        start_production, expr_plus_production, expr_term_production,
        term_star_production, term_factor_production, factor_number_production,
        factor_paran_production};
    grammar = parser::Grammar(productions, "expr'");
    parser = parser::Parser(grammar);
    tok = tokenizer::Tokenizer();
  }
};

TEST_F(ParserTest, LRItemEqualityTest) {
  parser::LRItem item1(factor_number_production, 0);
  parser::LRItem item2(factor_number_production, 1);
  parser::LRItem item3(factor_paran_production, 0);
  parser::LRItem item4(factor_number_production, 0);

  EXPECT_FALSE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
  EXPECT_TRUE(item1 == item4);
}

TEST_F(ParserTest, LRItemSetEqualityTest) {
  parser::LRItem item1(factor_number_production, 0);
  parser::LRItem item2(factor_number_production, 1);
  parser::LRItem item3(factor_paran_production, 0);
  parser::LRItem item4(factor_number_production, 0);

  parser::LRItemSet item_set1({item1, item4});
  parser::LRItemSet item_set2({item2, item3});
  EXPECT_FALSE(item_set1 == item_set2);

  parser::LRItemSet item_set3({item1, item2, item3});
  parser::LRItemSet item_set4({item3, item2, item1});
  parser::LRItemSet item_set5({item2, item3, item1});

  EXPECT_TRUE(item_set3 == item_set4);
  EXPECT_TRUE(item_set3 == item_set5);
  EXPECT_TRUE(item_set4 == item_set5);
}

TEST_F(ParserTest, LRItemSetFromLRItemTest1) {
  parser::LRItem item(start_production, 0);
  parser::LRItemSet actual_item_set(item, grammar);
  parser::LRItemSet expected_item_set({
    parser::LRItem(start_production, 0),
    parser::LRItem(expr_plus_production, 0),
    parser::LRItem(expr_term_production, 0),
    parser::LRItem(term_star_production, 0),
    parser::LRItem(term_factor_production, 0),
    parser::LRItem(factor_number_production, 0),
    parser::LRItem(factor_paran_production, 0)});

  EXPECT_TRUE(actual_item_set == expected_item_set);
}

TEST_F(ParserTest, LRItemSetFromLRItemTest2) {
  parser::LRItem item(expr_plus_production, 1);
  parser::LRItemSet actual_item_set(item, grammar);
  parser::LRItemSet expected_item_set({
    parser::LRItem(expr_plus_production, 1)});

  EXPECT_TRUE(actual_item_set == expected_item_set);
}

TEST_F(ParserTest, LRItemSetFromLRItemTest3) {
  parser::LRItem item(expr_plus_production, 2);
  parser::LRItemSet actual_item_set(item, grammar);
  parser::LRItemSet expected_item_set({
    parser::LRItem(expr_plus_production, 2),
    parser::LRItem(term_star_production, 0),
    parser::LRItem(term_factor_production, 0),
    parser::LRItem(factor_paran_production, 0),
    parser::LRItem(factor_number_production, 0)});

  EXPECT_TRUE(expected_item_set == actual_item_set);
}

TEST_F(ParserTest, LRItemSetFromLRItemSetAndTransitionSymbol) {
  parser::LRItem item(expr_plus_production, 1);
  parser::LRItemSet input_item_set(item, grammar);
  parser::LRItemSet actual_item_set(
      input_item_set, "+", grammar);
  parser::LRItemSet expected_item_set({
    parser::LRItem(expr_plus_production, 2),
    parser::LRItem(term_star_production, 0),
    parser::LRItem(term_factor_production, 0),
    parser::LRItem(factor_paran_production, 0),
    parser::LRItem(factor_number_production, 0)});

  EXPECT_TRUE(expected_item_set == actual_item_set);
}

TEST_F(ParserTest, ParserConstructorTest1) {
  std::vector<tokenizer::Token> tokens_to_parse;
  tok.tokenize("132+13*655");
  while (tok.has_more()) {
    tokens_to_parse.push_back(tok.get_next_token());
  }
  tokens_to_parse.emplace_back(
      tokenizer::TokenType::dollar, "");

  parser.parse(tokens_to_parse);
  std::vector<std::pair<parser::ParsingActionType, parser::Production>>
    parser_outputs;
  while (!parser.has_accepted() && !parser.is_stuck()) {
    parser_outputs.push_back(parser.make_next_move());
  }

  EXPECT_TRUE(parser.has_accepted());
  EXPECT_FALSE(parser.is_stuck());
}

TEST_F(ParserTest, ParserConstructorTest2) {
  std::vector<tokenizer::Token> tokens_to_parse;
  tok.tokenize("132++");
  while (tok.has_more()) {
    tokens_to_parse.push_back(tok.get_next_token());
  }
  tokens_to_parse.emplace_back(
      tokenizer::TokenType::dollar, "");

  parser.parse(tokens_to_parse);
  std::vector<std::pair<parser::ParsingActionType, parser::Production>>
      parser_outputs;
  while (!parser.has_accepted() && !parser.is_stuck()) {
    parser_outputs.push_back(parser.make_next_move());
  }

  EXPECT_FALSE(parser.has_accepted());
  EXPECT_TRUE(parser.is_stuck());
}

TEST_F(ParserTest, ParserConstructorTest3) {
  std::vector<tokenizer::Token> tokens_to_parse;
  tok.tokenize("(123+1232)*(854+45)");
  while (tok.has_more()) {
    tokens_to_parse.push_back(tok.get_next_token());
  }
  tokens_to_parse.emplace_back(
      tokenizer::TokenType::dollar, "");

  parser.parse(tokens_to_parse);
  std::vector<std::pair<parser::ParsingActionType, parser::Production>>
      parser_outputs;
  while (!parser.has_accepted() && !parser.is_stuck()) {
    parser_outputs.push_back(parser.make_next_move());
  }

  EXPECT_TRUE(parser.has_accepted());
  EXPECT_FALSE(parser.is_stuck());
}
