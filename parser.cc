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

ParseTable construct_parse_table()
{
  unordered_set<string> non_terminals = { "start","expr","expr'","term","term'","factor","array","rest","id_suffix",
                                          "assignee" };
  unordered_set<string> terminals = { "id","number","+","-","*","/","[","]","(",")",",","$","=","==" };

  Production production1("start", vector<string> { "id","id_suffix" });
  Production production2("start", vector<string> { "expr" });
  Production production3("id_suffix", vector<string> { "=", "assignee" });
  Production production4("id_suffix", vector<string> { "array" });
  Production production5("assignee", vector<string> { "array" });
  Production production6("assignee", vector<string> { "expr" });
  Production production1_x("term", vector<string> { "term","expr'" });
  Production production2_x("term'", vector<string> { "+","term","expr'" });
  Production production3_x("term'", vector<string> { "-","term","expr'" });
  Production production4_x("term'", vector<string> {});
  Production production5_x("term", vector<string> { "factor","term'" });
  Production production6_x("term'", vector<string> { "*","factor","term'" });
  Production production7_x("term'", vector<string> {"/", "factor", "term'" });
  Production production8_x("term'", vector<string> {});
  Production production9_x("factor", vector<string> { "number" });
  Production production10_x("factor", vector<string> { "(","expr",")" });
  Production production11_x("array", vector<string> { "[","number","rest","]" });
  Production production12_x("rest", vector<string> {",", "number", "rest" });
  Production production13_x("rest", vector<string> {});

  ParseTable parse_table(non_terminals, terminals);
  parse_table.add_rule("start", "id", production1);
  parse_table.add_rule("start", "number", production2);
  parse_table.add_rule("start", "(", production2);
  parse_table.add_rule("id_suffix", "=", production3);
  parse_table.add_rule("id_suffix", "[", production4);
  parse_table.add_rule("assignee", "number", production6);
  parse_table.add_rule("assignee", "[", production5);
  parse_table.add_rule("assignee", "(", production6);
  parse_table.add_rule("expr", "number", production1_x);
  parse_table.add_rule("expr", "(", production1_x);
  parse_table.add_rule("expr'", "+", production2_x);
  parse_table.add_rule("expr'", "-", production3_x);
  parse_table.add_rule("expr'", ")", production4_x);
  parse_table.add_rule("expr'", "$", production4_x);
  parse_table.add_rule("term", "number", production5_x);
  parse_table.add_rule("term", "(", production5_x);
  parse_table.add_rule("term'", "+", production8_x);
  parse_table.add_rule("term'", "-", production8_x);
  parse_table.add_rule("term'", "*", production6_x);
  parse_table.add_rule("term'", "/", production7_x);
  parse_table.add_rule("term'", ")", production8_x);
  parse_table.add_rule("term'", "$", production8_x);
  parse_table.add_rule("factor", "number", production9_x);
  parse_table.add_rule("factor", "(", production10_x);
  parse_table.add_rule("array", "[", production11_x);
  parse_table.add_rule("rest", ",", production12_x);
  parse_table.add_rule("rest", "]", production13_x);

  return parse_table;
}

vector<Production> parse(vector<Token> tokens)
{
  ParseTable parse_table = construct_parse_table();

  stack<string> parse_stack;
  parse_stack.push("start");

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
  return productions;
}
