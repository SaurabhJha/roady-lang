#include "gtest/gtest.h"

#include "ast/syntax_tree.h"
#include "parser/grammar.h"
#include "parser/parser.h"
#include "tokenizer/tokenizer.h"

class SyntaxTreeTest : public ::testing::Test {
 protected:
  parser::Production start_production;
  parser::Production expr_plus_production;
  parser::SyntaxDirectedDefinition expr_plus_sdd;
  parser::Production expr_term_production;
  parser::SyntaxDirectedDefinition term_star_sdd;
  parser::Production term_star_production;
  parser::Production term_factor_production;
  parser::Production factor_number_production;
  parser::SyntaxDirectedDefinition factor_paran_sdd;
  parser::Production factor_paran_production;
  std::vector<parser::Production> productions;
  parser::Grammar grammar;
  tokenizer::Tokenizer tokenizer_for_lang;
  parser::Parser parser;

  void SetUp() override {
    start_production = parser::Production("expr'", {"expr"});
    expr_plus_sdd = parser::SyntaxDirectedDefinition(
        parser::SyntaxDirectedDefinitionType::tree,
        {0, 2},
        "+");
    expr_plus_production = parser::Production(
        "expr", {"expr", "+", "term"}, expr_plus_sdd);
    expr_term_production = parser::Production("expr", {"term"});
    term_star_sdd = parser::SyntaxDirectedDefinition(
        parser::SyntaxDirectedDefinitionType::tree,
        {0, 2},
        "*");
    term_star_production = parser::Production(
        "term", {"term", "*", "factor"}, term_star_sdd);
    term_factor_production = parser::Production("term", {"factor"});
    factor_number_production = parser::Production(
        "factor", {"number"});
    factor_paran_sdd = parser::SyntaxDirectedDefinition(
        parser::SyntaxDirectedDefinitionType::copy,
        {1});
    factor_paran_production = parser::Production(
        "factor", {"(", "expr", ")"}, factor_paran_sdd);
    productions = {
        start_production, expr_plus_production, expr_term_production,
        term_star_production, term_factor_production, factor_number_production,
        factor_paran_production};
    grammar = parser::Grammar(productions, "expr'");
    parser = parser::Parser(grammar);
  }
};

TEST_F(SyntaxTreeTest, TreeTest) {
  std::vector<tokenizer::Token> tokens;

  auto input_string = "(123+3454)*(213*13)";
  tokenizer_for_lang.tokenize(input_string);
  while (tokenizer_for_lang.has_more()) {
    tokens.push_back(tokenizer_for_lang.get_next_token());
  }
  tokens.emplace_back(tokenizer::TokenType::dollar, "");

  parser.parse(tokens);
  std::vector<std::pair<parser::ParsingActionType, parser::Production>>
      parser_outputs;
  while (!parser.has_accepted() && !parser.is_stuck()) {
    parser_outputs.push_back(parser.make_next_move());
  }

  ast::construct_syntax_tree(tokens, parser_outputs);
}
