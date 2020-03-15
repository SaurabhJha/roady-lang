#include "./lexical_analyzer.h"

TransitionTable::TransitionTable(int ns) {
  number_of_states_ = ns;
  initial_state_ = 1;
  final_state_ = number_of_states_;
  for (int state = 1; state <= number_of_states_; ++state)
    table_[state] = TransitionTableRow();
}

void TransitionTable::add_transition(int prev_state, int next_state, string input) {
  table_[prev_state].add_pair(input, next_state);
}

TransitionTableRow TransitionTable::operator[](int state) {
  return table_[state];
}

int TransitionTable::move(int state, const string& input) {
  return table_[state][input];
}
