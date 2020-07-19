#include "./parser_top_down.h"
using namespace std;

void TopDownParsingTableRow::add_terminal_production_pair(const string& terminal, Production production)
{
  row_[terminal] = production;
}

Production TopDownParsingTableRow::operator[](const string& terminal)
{
  return row_[terminal];
}

void TopDownParsingTable::add_rule(const string& non_terminal, const string& terminal, Production production)
{
  TopDownParsingTableRow row = table_[non_terminal];
  row.add_terminal_production_pair(terminal, production);
  table_[non_terminal] = row;
}

bool TopDownParsingTable::is_non_terminal(const string& symbol)
{
  return !(non_terminals_.find(symbol) == non_terminals_.end());
}

bool TopDownParsingTable::is_terminal(const string& symbol)
{
  return !(terminals_.find(symbol) == terminals_.end());
}

TopDownParsingTableRow TopDownParsingTable::operator[](const string& non_terminal)
{
  return table_[non_terminal];
}

/**
 * Apply a production by popping the top parser_grammar symbol and pushing in the right side of the production.
 * The right side is pushed in the order such that the rightmost symbol appear at the bottom and the
 * leftmost symbol at the top, in accordance with requirement of LL(1) parser_grammar.
 *
 * Here's an example. Suppose the production is "expr' -> + term expr'". The successive stack states would
 * look like this
 *     expr' |
 *     + term expr' |
 */
void TopDownParser::apply_production(Production production)
{
  auto production_right_side = production.get_right_side();
  stack_.pop();
  for (auto next_non_terminal = production_right_side.rbegin(); next_non_terminal != production_right_side.rend();
       ++next_non_terminal)
    stack_.push(*next_non_terminal);
}

/**
 * Construct the top-down parse table for the following arithmetic expressions parser_grammar.
 *
 *   expr -> term expr'
 *   expr' -> + term expr' | - term expr' | epsilon
 *   term -> factor term'
 *   term' -> * factor term' | / factor term' | epsilon
 *   factor -> number | id | ( expr )
 */
TopDownParsingTable TopDownParser::construct_parsing_table()
{
  unordered_set<string> non_terminals = {"expr", "expr'", "term", "term'", "factor"};
  unordered_set<string> terminals = {"+", "-", "*", "/", "(", ")", "number", "id", "$"};

  Production expr_production = Production("expr", vector<string> {"term", "expr'"});
  Production expr_prime_production_plus("expr'", vector<string> {"+", "term", "expr'"});
  Production expr_prime_production_minus("expr'", vector<string> {"-", "term", "expr'"});
  Production expr_prime_production_empty("expr'", vector<string> {});

  Production term_production = Production("term", vector<string> {"factor", "term'"});
  Production term_prime_production_star = Production("term'", vector<string> {"*", "factor", "term'"});
  Production term_prime_production_slash = Production("term'", vector<string> {"/", "factor", "term'"});
  Production term_prime_production_empty = Production("term'", vector<string> {});

  Production factor_production_number = Production("factor", vector<string> {"number"});
  Production factor_production_id = Production("factor", vector<string> {"id"});
  Production factor_production_paren = Production("factor", vector<string> {"(", "expr", ")"});

  TopDownParsingTable parse_table(non_terminals, terminals);

  parse_table.add_rule("expr", "number", expr_production);
  parse_table.add_rule("expr", "(", expr_production);

  parse_table.add_rule("expr'", "+", expr_prime_production_plus);
  parse_table.add_rule("expr'", "-", expr_prime_production_minus);
  parse_table.add_rule("expr'", ")", expr_prime_production_empty);
  parse_table.add_rule("expr'", "$", expr_prime_production_empty);

  parse_table.add_rule("term", "number", term_production);
  parse_table.add_rule("term", "(", term_production);

  parse_table.add_rule("term'", "+", term_prime_production_empty);
  parse_table.add_rule("term'", "-", term_prime_production_empty);
  parse_table.add_rule("term'", "*", term_prime_production_star);
  parse_table.add_rule("term'", "/", term_prime_production_slash);
  parse_table.add_rule("term'", ")", term_prime_production_empty);
  parse_table.add_rule("term'", "$", term_prime_production_empty);

  parse_table.add_rule("factor", "number", factor_production_number);
  parse_table.add_rule("factor", "id", factor_production_id);
  parse_table.add_rule("factor", "(", factor_production_paren);

  return parse_table;
}

void TopDownParser::parse_next_token(Token token)
{
  auto incoming_terminal = map_token_type_to_terminal(token.get_token_type());
  auto top_stack_symbol = stack_.top();

  // As long as the top stack symbol is a non terminal, keep expanding it till we reach a terminal.
  while (parsing_table_.is_non_terminal(top_stack_symbol)) {
    if (parsing_table_[top_stack_symbol][incoming_terminal].is_empty()) {
      // We hit a snag. Abort here and we will handle this mismatch below.
      break;
    }
    auto next_production = parsing_table_[top_stack_symbol][incoming_terminal];
    apply_production(next_production);
    productions_applied_.push_back(next_production);
    top_stack_symbol = stack_.top();
  }

  if (top_stack_symbol == incoming_terminal) {
    // If top stack symbol is a terminal, and the incoming terminal and top stack symbol are the same, match them.
    stack_.pop();
  } else {
    has_failed_ = true;
  }
}

bool TopDownParser::has_failed()
{
  return has_failed_;
}

vector<Production> TopDownParser::get_productions_applied()
{
  return productions_applied_;
}

