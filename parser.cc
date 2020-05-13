#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./lexical_analyzer.h"
#include "./parser.h"
using namespace std;

string Production::get_left_side()
{
  return left_side_;
}

vector<string> Production::get_right_side()
{
  return right_side_;
}

void TopDownParsingTableRow::add_terminal_production_pair(string terminal, Production production)
{
  row_[terminal] = production;
}

Production TopDownParsingTableRow::operator[](string terminal)
{
  return row_[terminal];
}

void TopDownParsingTable::add_rule(string non_terminal, string terminal, Production production)
{
  TopDownParsingTableRow row = table_[non_terminal];
  row.add_terminal_production_pair(terminal, production);
  table_[non_terminal] = row;
}

bool TopDownParsingTable::is_non_terminal(string symbol)
{
  return !(non_terminals_.find(symbol) == non_terminals_.end());
}

bool TopDownParsingTable::is_terminal(string symbol)
{
  return !(terminals_.find(symbol) == terminals_.end());
}

TopDownParsingTableRow TopDownParsingTable::operator[](string non_terminal)
{
  return table_[non_terminal];
}

/**
 *
 * @param token_type
 * @return terminal_string
 * We have to define this function because we cannot have a unordered_map with
 * TokenType as key.
 */
string map_token_type_to_terminal(TokenType token_type)
{
  switch (token_type) {
    case (TokenType::id):
      return "id";
    case (TokenType::number):
      return "number";
    case (TokenType::star):
      return "*";
    case (TokenType::slash):
      return "/";
    case (TokenType::plus):
      return "+";
    case (TokenType::minus):
      return "-";
    case (TokenType::comma):
      return ",";
    case (TokenType::open_bracket):
      return "[";
    case (TokenType::close_bracket):
      return "]";
    case (TokenType::open_paren):
      return "(";
    case (TokenType::close_paren):
      return ")";
    case (TokenType::equals):
      return "=";
    case (TokenType::double_equals):
      return "==";
    case (TokenType::dollar):
      return "$";
    case (TokenType::invalid):
      return "";

  }
}

/**
 * Apply a production by popping the top grammar symbol and pushing in the right side of the production.
 * The right side is pushed in the order such that the rightmost symbol appear at the bottom and the
 * leftmost symbol at the top, in accordance with requirement of LL(1) parser.
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
 * Construct the top-down parse table for the following arithmetic expressions grammar.
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

void BottomUpParsingActionTableRow::add_terminal_action_pair(string terminal, BottomUpParsingAction action)
{
  row_[terminal] = action;
}

BottomUpParsingAction BottomUpParsingActionTableRow::operator[](string terminal)
{
  return row_[terminal];
}


void BottomUpParsingActionTable::add_rule(int state, string terminal, BottomUpParsingAction action)
{
  auto row = table_[state];
  row.add_terminal_action_pair(std::move(terminal), action);
  table_[state] = row;
}

BottomUpParsingActionTableRow BottomUpParsingActionTable::operator[](int state)
{
  return table_[state];
}

void BottomUpParsingGotoTableRow::add_nonterminal_state_pair(string nonterminal, int state)
{
  row_[nonterminal] = state;
}

int BottomUpParsingGotoTableRow::operator[](string nonterminal)
{
  return row_[nonterminal];
}

void BottomUpParsingGotoTable::add_rule(int state, string nonterminal, int new_state)
{
  auto row = table_[state];
  row.add_nonterminal_state_pair(std::move(nonterminal), new_state);
  table_[state] = row;
}

BottomUpParsingGotoTableRow BottomUpParsingGotoTable::operator[](int state)
{
  return table_[state];
}

void BottomUpParser::construct_productions()
{
  // expr' productions
  Production production_expr_prime = Production("expr'", vector<string> {"expr"});

  // expr productions
  Production production_expr_plus = Production("expr", vector<string> {"expr", "+", "term"});
  Production production_expr_minus = Production("expr", vector<string> {"expr", "-", "term"});
  Production production_expr_term = Production("expr", vector<string> {"term"});

  // term productions
  Production production_term_star = Production("term", vector<string> {"term", "*", "factor"});
  Production production_term_slash = Production("term", vector<string> {"term", "/", "factor"});
  Production production_term_factor = Production("term", vector<string> {"factor"});

  // factor productions
  Production production_factor_paren = Production("factor", vector<string> {"(", "expr", ")"});
  Production production_factor_number = Production("factor", vector<string> {"number"});

  numbered_reductions_[1] = production_expr_prime;
  numbered_reductions_[2] = production_expr_plus;
  numbered_reductions_[3] = production_expr_minus;
  numbered_reductions_[4] = production_expr_term;
  numbered_reductions_[5] = production_term_star;
  numbered_reductions_[6] = production_term_slash;
  numbered_reductions_[7] = production_term_factor;
  numbered_reductions_[8] = production_factor_paren;
  numbered_reductions_[9] = production_factor_number;
}

void BottomUpParser::construct_action_table()
{
  action_table_.add_rule(
      0,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      0,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      1,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 5)
  );
  action_table_.add_rule(
      1,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 6)
  );
  action_table_.add_rule(
      1,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::accept, 0)
  );

  action_table_.add_rule(
      2,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );
  action_table_.add_rule(
      2,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );
  action_table_.add_rule(
      2,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 7)
  );
  action_table_.add_rule(
      2,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 8)
  );
  action_table_.add_rule(
      2,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );
  action_table_.add_rule(
      2,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );

  action_table_.add_rule(
      3,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );

  action_table_.add_rule(
      4,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      4,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      5,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      5,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      6,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      6,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );


  action_table_.add_rule(
      7,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      7,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );


  action_table_.add_rule(
      8,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      8,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      10,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 5)
  );
  action_table_.add_rule(
      10,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 6)
  );
  action_table_.add_rule(
      10,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 15)
  );

  action_table_.add_rule(
      11,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );
  action_table_.add_rule(
      11,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );
  action_table_.add_rule(
      11,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 7)
  );
  action_table_.add_rule(
      11,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 8)
  );
  action_table_.add_rule(
      11,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );
  action_table_.add_rule(
      11,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );

  action_table_.add_rule(
      12,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );
  action_table_.add_rule(
      12,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );
  action_table_.add_rule(
      12,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 7)
  );
  action_table_.add_rule(
      12,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 8)
  );
  action_table_.add_rule(
      12,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );
  action_table_.add_rule(
      12,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );

  action_table_.add_rule(
      13,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );

  action_table_.add_rule(
      14,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );

  action_table_.add_rule(
      15,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );

  action_table_.add_rule(
      16,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
}

void BottomUpParser::construct_goto_table()
{
  goto_table_.add_rule(0, "expr", 1);
  goto_table_.add_rule(0, "term", 2);
  goto_table_.add_rule(0, "factor", 3);

  goto_table_.add_rule(4, "expr", 10);
  goto_table_.add_rule(4, "term", 2);
  goto_table_.add_rule(4, "factor", 3);

  goto_table_.add_rule(5, "term", 11);
  goto_table_.add_rule(5, "factor", 3);

  goto_table_.add_rule(6, "term", 12);
  goto_table_.add_rule(6, "factor", 3);

  goto_table_.add_rule(7, "factor", 13);
  goto_table_.add_rule(8, "factor", 14);
}

BottomUpParsingAction BottomUpParser::apply_reduction(BottomUpParsingAction next_action, string terminal)
{
  // Get the reduction to be applied.
  auto reduction_number = next_action.get_action_number();
  auto next_reduction = numbered_reductions_[reduction_number];
  reductions_applied_.push_back(next_reduction);

  // Do the stack manipulation.
  for (auto i = 0; i < next_reduction.get_right_side().size(); ++i)
    stack_.pop();
  auto resultant_nonterminal = next_reduction.get_left_side();
  auto state_on_top = stack_.top();
  auto next_state = goto_table_[state_on_top][resultant_nonterminal];
  stack_.push(next_state);

  // Return the next action.
  return action_table_[next_state][terminal];
}

/**
 * An implementation of SLR(1) shift reduce parser.
 */
void BottomUpParser::parse_next_token(Token token)
{
  auto terminal = map_token_type_to_terminal(token.get_token_type());
  auto next_action = action_table_[stack_.top()][terminal];

  // Keep reducing till no more reductions are possible.
  while (next_action.get_action_type() == BottomUpParsingActionType::reduce) {
    next_action = apply_reduction(next_action, terminal);
  }

  // Shift now if that's the next move.
  if (next_action.get_action_type() == BottomUpParsingActionType::shift) {
    if (next_action.get_action_number() != 0) {
      auto next_state = next_action.get_action_number();
      stack_.push(next_state);
    } else {
      has_failed_ = true;
    }
  }

  // You implicitly accept otherwise.
}

bool BottomUpParser::has_failed()
{
  return has_failed_;
}

vector<Production> BottomUpParser::get_reductions_applied()
{
  return reductions_applied_;
}


