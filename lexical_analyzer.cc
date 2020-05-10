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

void FiniteAutomaton::move(string input)
{
  current_state_ = table_[current_state_][input];
}

bool FiniteAutomaton::is_dead()
{
  return current_state_ == 0;
}

bool FiniteAutomaton::has_accepted()
{
  return !(final_states_.find(current_state_) == final_states_.end());
}

void FiniteAutomaton::reset()
{
  current_state_ = initial_state_;
}

/**
 * Executes the automaton on an input string, starting from a start_index. Returns the substring
 * matched. Returns empty string if the input is not recognised by the automaton.
 * @param input_string
 * @param start_index
 * @return
 */
string FiniteAutomaton::execute_on_input_string(string input_string, int start_index)
{
  // First of all, reset the automaton state.
  reset();
  auto accepted = false;
  // We start from the index before start_index because the minimum non-empty string is a length 1 string when
  // index_at_accept == start_index.
  auto index_at_accept = start_index - 1;

  // Start from start_index and continue on with next characters till either
  // 1. the automaton is dead.
  // 2. the string is exhausted.
  for (int current_index = start_index; current_index < input_string.length(); ++current_index) {
    string current_input(1, input_string[current_index]);
    move(current_input);
    if (is_dead()) {
      break;
    }
    if (has_accepted()) {
      accepted = true;
      index_at_accept = current_index;
    }
  }

  if (accepted) {
    auto length_of_accepted_string = index_at_accept - start_index + 1;
    return input_string.substr(start_index, length_of_accepted_string);
  }
  return "";
}

FiniteAutomaton construct_automaton_for_id()
{
  TransitionTable table(4);
  unordered_set<string> alphabetical_characters = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                                                    "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
                                                    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
                                                    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
  unordered_set<string> numeric_characters = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

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
  unordered_set<string> numeric_characters = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };

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
    case (TokenType::open_paren):
      token_type_string = "(";
      break;
    case (TokenType::close_paren):
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
 * Starts from current_index_ in raw_input and gets the next token. It does so by trying all
 * automatons and selecting the token corresponding to the maximum input matched; its an
 * implementation of maximal-munch policy.
 */
Token Tokenizer::get_next_token() {
  vector<TokenTypeAutomatonPair> token_automaton_pairs;
  token_automaton_pairs.emplace_back(TokenType::id, construct_automaton_for_id());
  token_automaton_pairs.emplace_back(TokenType::number, construct_automaton_for_number());
  token_automaton_pairs.emplace_back(TokenType::plus, construct_automaton_for_special_character("+"));
  token_automaton_pairs.emplace_back(TokenType::minus, construct_automaton_for_special_character("-"));
  token_automaton_pairs.emplace_back(TokenType::star, construct_automaton_for_special_character("*"));
  token_automaton_pairs.emplace_back(TokenType::slash, construct_automaton_for_special_character("/"));
  token_automaton_pairs.emplace_back(TokenType::open_bracket, construct_automaton_for_special_character("["));
  token_automaton_pairs.emplace_back(TokenType::close_bracket, construct_automaton_for_special_character("]"));
  token_automaton_pairs.emplace_back(TokenType::open_paren, construct_automaton_for_special_character("("));
  token_automaton_pairs.emplace_back(TokenType::close_paren, construct_automaton_for_special_character(")"));
  token_automaton_pairs.emplace_back(TokenType::comma, construct_automaton_for_special_character(","));
  token_automaton_pairs.emplace_back(TokenType::equals, construct_automaton_for_special_character("="));
  token_automaton_pairs.emplace_back(TokenType::double_equals, construct_automaton_for_double_equals());

  string lexeme;
  TokenType token_type = TokenType::invalid;
  for (auto token_automaton_pair : token_automaton_pairs) {
    auto automaton = token_automaton_pair.get_automaton();
    string lexeme_candidate = automaton.execute_on_input_string(raw_input_, current_index_);
    if (lexeme_candidate.length() > lexeme.length()) {
      lexeme = lexeme_candidate;
      token_type = token_automaton_pair.get_token_type();
    }
  }

  // Update tokenizer state to set the correct input pointer.
  if (token_type == TokenType::invalid)
    // Set the state so that the tokenizer is aborted.
    current_index_ = raw_input_.length();
  else
    // Tokenizer can continue
    current_index_ += lexeme.length();

  Token token(token_type, lexeme);
  return token;
}

/**
 * Checks whether there is any more input left in this tokenizer.
 */
bool Tokenizer::is_there_more_input()
{
  return current_index_ < raw_input_.length();
}