#ifndef FINITE_AUTOMATA_H_
#define FINITE_AUTOMATA_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "tokeniser/common.h"

namespace finite_automata {

/**
 * A transition table row is an associative data structure where the keys are
 * input characters and values are integer states. It is supposed to be used
 * as a part of `TransitionTable`.
 */
class TransitionTableRow {
 public:
  std::unordered_map<std::string, std::unordered_set<int>> row_;

  TransitionTableRow() = default;
  ~TransitionTableRow() = default;

  std::unordered_set<int> operator[](const std::string& input)
  {
    return row_[input];
  }

  void increase_state_numbers_by(int number);
  void add_input_state_pair(const std::string& input, int next_state);
};

/**
 * A transition table is a data structure where the keys are integer states and
 * the values are rows which themselves are a key value data structure. This arrangement
 * lets us have an interface like this:
 *     transition_table[start_state][input] = end_state.
 */
class TransitionTable {
 public:
  std::unordered_map<int, TransitionTableRow> table_;

  TransitionTable() = default;
  ~TransitionTable() = default;

  TransitionTableRow operator[](int state)
  {
    return table_[state];
  }

  void increase_state_numbers_by(int number);
  void add_new_transition(int prev_state, int next_state, const std::string& input);
  TransitionTable combine_with(const TransitionTable& other_table);
};

/**
 * A finite automaton consists of
 * 1. An initial state
 * 2. A set of states designated as the final state.
 * 3. A transition table.
 *
 * This is a non-deterministic finite automata. We simulate the views by calculating epsilon closures
 * for current states on inputs.
 */
class FiniteAutomata {
 private:
  int initial_state_;
  int final_state_; // For our automata, there is only one final state.
  std::unordered_set<int> current_states_;
  std::unordered_map<int, std::unordered_set<int>> closure_sets_; // Used to memoize closure sets.
  TransitionTable table_;

  std::unordered_set<int> compute_epsilon_closure(int state);
  void reset();

 public:
  FiniteAutomata()
    :initial_state_{1}, final_state_{2}
  {
    current_states_ = compute_epsilon_closure(initial_state_);
  }

  FiniteAutomata(int initial_state, int final_state, const TransitionTable& table)
    :initial_state_{initial_state}, final_state_{final_state}, table_{table}
  {
    current_states_ = compute_epsilon_closure(initial_state);
  }

  ~FiniteAutomata() = default;

  TransitionTable get_transition_table() { return table_; }
  int get_start_state() { return initial_state_; }
  int get_final_state() { return final_state_; }

  void increase_state_numbers_by(int number);
  void add_new_transition(int prev_state, int next_state, const std::string& input);
  FiniteAutomata union_with(FiniteAutomata other_automata);
  FiniteAutomata concatenate_with(FiniteAutomata other_automata);
  FiniteAutomata compute_exponentiation();
  void move(const std::string& input);
  bool has_accepted();
  bool has_died();
};

}

#endif // FINITE_AUTOMATA_H_