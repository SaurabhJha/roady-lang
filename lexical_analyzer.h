#ifndef ROADY_LANG_LEXICAL_ANALYZER_H_
#define ROADY_LANG_LEXICAL_ANALYZER_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

/**
 * Data structure to represent a row of transition table like this:
 *    | l | d |
 *  --|-------|
 *  x | 4 | 3 |
 *
 *  where x is immaterial and not part of this data structure.
 *  What we are looking for is a hash table where the keys are inputs and values
 *  are states. So the above row would look like this
 *    { 'l': 4, 'd': 3 }
 */
class TransitionTableRow {
 private:
  unordered_map<string, int> row_;

 public:
  TransitionTableRow() = default;
  ~TransitionTableRow() = default;

  void add_input_state_pair(string input, int state);
  int operator[](string input);
};

/**
 * Data structure to represent a transition table like this:
 *    | l | d |
 *  --|-------|
 *  1 | 2 |   |
 *  2 | 4 | 3 |
 *  3 | 4 | 3 |
 *  4 | 4 | 3 |
 *
 * It is composed of transition table rows. Specifically, each state has an
 * associated a transition table row which determines next states on different
 * inputs.
 *
 * What we are looking for is something where the API would look like this
 *       table[state][next_input] = next_state
 */
class TransitionTable {
 private:
  int number_of_states_;
  unordered_map<int, TransitionTableRow> table_;

 public:
  TransitionTable(int states)
    :number_of_states_{states}
  {
    // Create an empty row for each state.
    for (auto state=1; state<=number_of_states_; ++state) {
      TransitionTableRow current_row;
      table_[state] = current_row;
    }
  }
  ~TransitionTable() = default;

  void add_transition(int start_state, int end_state, string input);
  TransitionTableRow operator[](int state);
};

/**
 * A finite automaton consists of
 *  1. An initial state.
 *  2. A set of final states.
 *  3. A transition table.
 * This class implements this definition.
 */
class FiniteAutomaton {
 private:
  int initial_state_;
  unordered_set<int> final_states_;
  TransitionTable table_;
  int current_state_;

 public:
  FiniteAutomaton(int initial_state, unordered_set<int> final_states, TransitionTable table)
    :initial_state_{initial_state}, current_state_{initial_state}, final_states_{final_states}, table_{table} {}
  ~FiniteAutomaton() = default;

  void move(string input);
  bool is_dead();
  bool has_accepted();
  void reset();
  string execute_on_input_string(string input_string, int start_index);
};

/**
 * These are the token types we support. "invalid" token is for unrecognizable tokens.
 */
enum class TokenType { invalid, id, number, plus, minus, star, slash, open_bracket,
                       close_bracket, open_paren, close_paren, comma, equals,
                       double_equals, dollar };

/**
 * A token consists of
 * 1) a token type.
 * 2) the associated lexeme that matched.
 * 3) the index right after the current lexeme
 */
class Token
{
 private:
  TokenType token_type_;
  string lexeme_;

 public:
  Token(TokenType token_type, string lexeme)
      :token_type_{token_type}, lexeme_{lexeme} {}
  ~Token() = default;

  TokenType get_token_type() { return token_type_; };
  string get_lexeme() { return lexeme_; };
  void print();
};

/**
 * Data structure to represent a pair of TokenType and automaton. This is
 * used to make TokenType-Automaton associations.
 */
class TokenTypeAutomatonPair
{
 private:
  TokenType token_type_;
  FiniteAutomaton automaton_;

 public:
  TokenTypeAutomatonPair(TokenType token_type, const FiniteAutomaton& automaton)
    :token_type_{token_type}, automaton_{automaton} {}
  ~TokenTypeAutomatonPair() = default;

  TokenType get_token_type() { return token_type_; }
  FiniteAutomaton get_automaton() { return automaton_; }
};

/**
 * A tokenizer_tokenizer implementation. It contains all the state necessary to generate tokens one-by-one
 * from a text string.
 */
class Tokenizer
{
 private:
  string raw_input_;
  int current_index_;

 public:
  Tokenizer(string raw_input, int current_index=0)
    :current_index_{current_index}, raw_input_{std::move(raw_input)} {}
  ~Tokenizer() = default;

  Token get_next_token();
  bool there_is_more_input();
};

FiniteAutomaton construct_automaton_for_id();
FiniteAutomaton construct_automaton_for_number();
FiniteAutomaton construct_automaton_for_double_equals();
FiniteAutomaton construct_automaton_for_special_character(string special_character);

string remove_whitespace(string input);

#endif // ROADY_LANG_LEXICAL_ANALYZER_H_
