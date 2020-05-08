#include <unordered_set>
#include <vector>
#include <iostream>
using namespace std;

#include "./lexical_analyzer.h"

void TransitionTableRow::add_input_state_pair(string input, int state)
{
  row_[input] = state;
}

int TransitionTableRow::operator[](string input)
{
  return row_[input];
}

void TransitionTable::add_transition(int start_state, int end_state, string input)
{
  TransitionTableRow row = table_[start_state];
  row.add_input_state_pair(input, end_state);
  table_[start_state] = row;
}

TransitionTableRow TransitionTable::operator[](int state)
{
  return table_[state];
}

bool FiniteAutomaton::is_dead()
{
  return current_state_ == 0;
}

bool FiniteAutomaton::has_accepted()
{
  return !(final_states_.find(current_state_) == final_states_.end());
}

void FiniteAutomaton::move(string input)
{
  current_state_ = table_[current_state_][input];
}

FiniteAutomaton construct_automaton_for_id()
{
  TransitionTable table(4);
  unordered_set<string> alphabetical_characters = { "a","b","c","d","e","f","g","h","i","j","k","l","m",
                                                    "n","o","p","q","r","s","t","u","v","w","x","y","z",
                                                    "A","B","C","D","E","F","G","H","I","J","K","L","M",
                                                    "N","O","P","Q","R","S","T","U","V","W","X","Y","Z" };
  unordered_set<string> numeric_characters = { "0","1","2","3","4","5","6","7","8","9","0" };

  for (string letter : alphabetical_characters) {
    table.add_transition(1, 2, letter);
    table.add_transition(2, 4, letter);
    table.add_transition(3, 4, letter);
    table.add_transition(4, 4, letter);
  }
  for (string digit : numeric_characters) {
    table.add_transition(2, 3, digit);
    table.add_transition(3, 3, digit);
    table.add_transition(4, 3, digit);
  }
  int initial_state = 1;
  unordered_set<int> final_states = {2,3,4};

  return FiniteAutomaton(initial_state, final_states, table);
}

FiniteAutomaton construct_automaton_for_number()
{
  TransitionTable table(3);
  unordered_set<string> numeric_characters = { "0","1","2","3","4","5","6","7","8","9","0" };

  for (string digit : numeric_characters) {
    table.add_transition(1, 2, digit);
    table.add_transition(2, 3, digit);
    table.add_transition(3, 3, digit);
  }
  int initial_state = 1;
  unordered_set<int> final_states = {2,3};

  return FiniteAutomaton(initial_state, final_states, table);
}

FiniteAutomaton construct_automaton_for_double_equals()
{
  TransitionTable table(3);
  table.add_transition(1, 2, "=");
  table.add_transition(2, 3, "=");
  int initial_state = 1;
  unordered_set<int> final_states = {3};
  return FiniteAutomaton(initial_state, final_states, table);
}

FiniteAutomaton construct_automaton_for_special_character(string special_character)
{
  TransitionTable table(2);
  table.add_transition(1, 2, special_character);
  int initial_state = 1;
  unordered_set<int> final_states = {2};
  return FiniteAutomaton(initial_state, final_states, table);
}

void Token::print()
{
  string token_type_string;
  switch (token_type_) {
    case (TokenType::id):
      token_type_string = "id";
      break;
    case (TokenType::number):
      token_type_string = "number";
      break;
    case (TokenType::star):
      token_type_string = "*";
      break;
    case (TokenType::slash):
      token_type_string = "/";
      break;
    case (TokenType::plus):
      token_type_string = "+";
      break;
    case (TokenType::minus):
      token_type_string = "-";
      break;
    case (TokenType::comma):
      token_type_string = ",";
      break;
    case (TokenType::open_bracket):
      token_type_string = "[";
      break;
    case (TokenType::close_bracket):
      token_type_string = "]";
      break;
    case (TokenType::open_paran):
      token_type_string = "(";
      break;
    case (TokenType::close_paran):
      token_type_string = ")";
      break;
    case (TokenType::equals):
      token_type_string = "=";
      break;
    case (TokenType::double_equals):
      token_type_string = "==";
      break;
    case (TokenType::invalid):
      token_type_string = "invalid";
      break;
  }
  cout << "<" << token_type_string << ", " << lexeme_ << ">";
}

string remove_whitespace(string input)
{
  string output {""};
  for (auto character : input) {
    if (character != ' ')
      output.push_back(character);
  }
  return output;
}

/**
 * @param input
 * @param start_index
 * @return next_token
 * Tries all automatons in turn and gets the token with the longest lexeme;
 * its an implementation of maximal munch policy.
 */
Token get_next_token(string input, int start_index)
{
  vector<FiniteAutomaton> vector_of_automata = { construct_automaton_for_number(),
                                                 construct_automaton_for_id(),
                                                 construct_automaton_for_double_equals(),
                                                 construct_automaton_for_special_character("+"),
                                                 construct_automaton_for_special_character("*"),
                                                 construct_automaton_for_special_character("-"),
                                                 construct_automaton_for_special_character("/"),
                                                 construct_automaton_for_special_character("["),
                                                 construct_automaton_for_special_character("]"),
                                                 construct_automaton_for_special_character("("),
                                                 construct_automaton_for_special_character(")"),
                                                 construct_automaton_for_special_character(","),
                                                 construct_automaton_for_special_character("=") };
  // Used to correlate automata with token types. The order in `vector_of_token_types` must
  // match with `vector_of_automata`.
  vector<TokenType> vector_of_token_types = { TokenType::number,
                                              TokenType ::id,
                                              TokenType::double_equals,
                                              TokenType::plus,
                                              TokenType::star,
                                              TokenType::minus,
                                              TokenType::slash,
                                              TokenType::open_bracket,
                                              TokenType::close_bracket,
                                              TokenType::open_paran,
                                              TokenType::close_paran,
                                              TokenType::comma,
                                              TokenType::equals };

  TokenType next_token_type = TokenType::invalid;
  int max_next_token_index = start_index - 1;
  for (int i=0; i<vector_of_automata.size(); ++i) {
    FiniteAutomaton automaton = vector_of_automata[i];
    int next_lexeme_end = start_index - 1;
    for (int current_index = start_index; current_index < input.length(); ++current_index) {
      string current_input(1, input[current_index]);
      automaton.move(current_input);

      if (automaton.is_dead()) {
        break;
      }
      if (automaton.has_accepted()) {
        next_lexeme_end = current_index;
      }
    }

    if (next_lexeme_end > max_next_token_index) {
      max_next_token_index = next_lexeme_end;
      next_token_type = vector_of_token_types[i];
    }
  }

  if (max_next_token_index == start_index - 1) {
    Token next_token(TokenType::invalid, "", -1);
    return next_token;
  } else {
    Token next_token(next_token_type,
                     input.substr(start_index, max_next_token_index - start_index + 1),
                     max_next_token_index);
    return next_token;
  }
}

vector<Token> tokenize(string input)
{
  vector<Token> tokens;
  int index = 0;
  while (index < input.size()) {
    Token next_token = get_next_token(input, index);
    tokens.push_back(next_token);
    if (next_token.get_next_token_index() == index - 1)
      break;
    index = next_token.get_next_token_index() + 1;
  }
  return tokens;
}
