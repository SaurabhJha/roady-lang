#include <iostream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./lexical_analyzer.h"
#include "./parser.h"
using namespace std;

vector<string> Production::apply()
{
  return right_side_;
}

void ParseTableRow::add_terminal_production_pair(string terminal, Production production)
{
  row_[terminal] = production;
}

Production ParseTableRow::operator[](string terminal)
{
  return row_[terminal];
}

void ParseTable::add_rule(string non_terminal, string terminal, Production production)
{
  ParseTableRow row = table_[non_terminal];
  row.add_terminal_production_pair(terminal, production);
  table_[non_terminal] = row;
}

bool ParseTable::is_non_terminal(string symbol)
{
  return !(non_terminals_.find(symbol) == non_terminals_.end());
}

bool ParseTable::is_terminal(string symbol)
{
  return !(terminals_.find(symbol) == terminals_.end());
}

ParseTableRow ParseTable::operator[](string non_terminal)
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
    case (TokenType::invalid):
      return "";
  }
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
ParseTable construct_top_down_parse_table()
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

  ParseTable parse_table(non_terminals, terminals);

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

vector<Production> parse(vector<Token> tokens)
{
  auto parse_table = construct_top_down_parse_table();

  stack<string> parse_stack;
  parse_stack.push("expr");

  int token_index = 0;
  vector<Production> productions;
  while (token_index < tokens.size()) {
    Token token = tokens[token_index];
    string terminal = map_token_type_to_terminal(token.get_token_type());
    string top_stack_symbol = parse_stack.top();
    cout << "Top stack symbol: " << top_stack_symbol << ", next token: " << terminal;

    if (parse_table.is_non_terminal(top_stack_symbol) && !parse_table[top_stack_symbol][terminal].is_empty()) {
      Production production = parse_table[top_stack_symbol][terminal];
      productions.push_back(production);
      cout << ", rule applied: ";
      production.print();
      cout << "\n";
      vector<string> production_right_side = production.apply();
      parse_stack.pop();
      for (vector<string>::reverse_iterator non_terminal_p = production_right_side.rbegin();
           non_terminal_p != production_right_side.rend();
           ++non_terminal_p)
        parse_stack.push(*non_terminal_p);
    }
    else if (parse_table.is_terminal(top_stack_symbol) && top_stack_symbol == terminal) {
      cout << ", matched!\n";
      parse_stack.pop();
      ++token_index;
    }
    else {
      cout << "NO!\n";
      vector<Production> empty_productions = {};
      return empty_productions;
    }
  }

  while (token_index == tokens.size() && !(parse_stack.empty())) {
    string top_stack_symbol = parse_stack.top();
    cout << "Top stack symbol: " << top_stack_symbol << ", next token: " << "$";
    Production production = parse_table[top_stack_symbol]["$"];
    productions.push_back(production);
    cout << ", rule applied: ";
    production.print();
    cout << "\n";
    parse_stack.pop();
  }

  if (token_index == tokens.size() && parse_stack.empty()) {
    cout << "successful parse\n";
  } else {
    cout << "unsuccessful parse\n";
  }

  return productions;
}
