#include "parser/grammar.h"
#include "parser/automaton.h"

int main()
{
  parser_grammar::Grammar grammar("expr'");
  parser_grammar::Production expr_prime_production("expr'", {"expr"});
  parser_grammar::Production expr_plus_production("expr", {"expr", "+", "term"});
  parser_grammar::Production expr_minus_production("expr", {"expr", "-", "term"});
  parser_grammar::Production expr_term_production("expr", {"term"});
  parser_grammar::Production term_star_production("term", {"term", "*", "factor"});
  parser_grammar::Production term_slash_production("term", {"term", "/", "factor"});
  parser_grammar::Production term_factor_production("term", {"factor"});
  parser_grammar::Production factor_number_production("factor", {"number"});
  parser_grammar::Production factor_paren_production("factor", {"(", "expr", ")"});

  grammar.add_production(expr_prime_production);
  grammar.add_production(expr_plus_production);
  grammar.add_production(expr_minus_production);
  grammar.add_production(expr_term_production);
  grammar.add_production(term_star_production);
  grammar.add_production(term_slash_production);
  grammar.add_production(term_factor_production);
  grammar.add_production(factor_number_production);
  grammar.add_production(factor_paren_production);

  parser_automaton::ParsingTable parsing_table(grammar);

  return 0;
}