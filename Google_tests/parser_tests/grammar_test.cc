#include "gtest/gtest.h"

#include "parser/grammar.h"

class GrammarTest : public ::testing::Test {
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
  }
};

TEST_F(GrammarTest, FirstSetTest) {
  auto first_set_of_start_symbol =
      grammar.compute_first_set("expr'");
  EXPECT_EQ(first_set_of_start_symbol.size(), 2);
  EXPECT_TRUE(
      first_set_of_start_symbol.find("number") !=
      first_set_of_start_symbol.end());
  EXPECT_TRUE(
      first_set_of_start_symbol.find("(") !=
      first_set_of_start_symbol.end());

  auto first_set_of_expr = grammar.compute_first_set("expr");
  EXPECT_TRUE(
      first_set_of_expr.find("number") !=
          first_set_of_expr.end());
  EXPECT_TRUE(
      first_set_of_expr.find("(") != first_set_of_expr.end());

  auto first_set_of_factor = grammar.compute_first_set("factor");
  EXPECT_TRUE(
      first_set_of_factor.find("number") !=
          first_set_of_factor.end());
  EXPECT_TRUE(
      first_set_of_factor.find("(") != first_set_of_factor.end());
}

TEST_F(GrammarTest, FollowSetTest) {
  auto follow_set_of_start_symbol = grammar.compute_follow_set(
      "expr'");
  EXPECT_EQ(follow_set_of_start_symbol.size(), 1);
  EXPECT_TRUE(
      follow_set_of_start_symbol.find("$") !=
      follow_set_of_start_symbol.end());

  auto follow_set_of_expr = grammar.compute_follow_set("expr");
  EXPECT_EQ(follow_set_of_expr.size(), 3);
  EXPECT_TRUE(
      follow_set_of_expr.find("$") != follow_set_of_expr.end());
  EXPECT_TRUE(
      follow_set_of_expr.find("+") != follow_set_of_expr.end());
  EXPECT_TRUE(
      follow_set_of_expr.find(")") != follow_set_of_expr.end());

  auto follow_set_of_factor = grammar.compute_follow_set("factor");
  EXPECT_EQ(follow_set_of_factor.size(), 4);
  EXPECT_TRUE(
      follow_set_of_factor.find("$") != follow_set_of_factor.end());
  EXPECT_TRUE(
      follow_set_of_factor.find("+") != follow_set_of_factor.end());
  EXPECT_TRUE(
      follow_set_of_factor.find("*") != follow_set_of_factor.end());
  EXPECT_TRUE(
      follow_set_of_factor.find(")") != follow_set_of_factor.end());
}