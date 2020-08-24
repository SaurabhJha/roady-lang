#include "gtest/gtest.h"

#include "parser/parser.h"

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

TEST_F(ParserTest, ParserConstructorTest) {
  parser = parser::Parser(grammar);
}
