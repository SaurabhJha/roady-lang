#include "parser/grammar.h"

int main()
{
  parser::Grammar grammar("expr'");
  parser::Production expr_prime_production("expr'", {"expr"});
  parser::Production expr_production1("expr", {"expr", "+", "term"});
  parser::Production expr_production2("expr", {"term"});
  parser::Production term_production1("term", {"term", "*", "factor"});
  parser::Production term_production2("term", {"factor"});
  parser::Production factor_production1("factor", {"number"});
  parser::Production factor_production2("factor", {"(", "expr", ")"});

  grammar.add_production(expr_prime_production);
  grammar.add_production(expr_production1);
  grammar.add_production(expr_production2);
  grammar.add_production(term_production1);
  grammar.add_production(term_production2);
  grammar.add_production(factor_production1);
  grammar.add_production(factor_production2);

  grammar.construct_lr0_automaton();
}