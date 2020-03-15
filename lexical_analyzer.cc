#include "./lexical_analyzer.h"

TransitionTable::TransitionTable(int ns) {
  number_of_states_ = ns;
  for (int state = 1; state <= number_of_states_; ++state)
    table_[state] = TransitionTableRow();
}

void TransitionTable::add_transition(int prev_state, int next_state, string input) {
  table_[prev_state].add_pair(input, next_state);
}

TransitionTableRow TransitionTable::operator[](int state) {
  return table_[state];
}

int TransitionTable::compute_next_state(int state, const string &input)
{
  return table_[state][input];
}

void FiniteAutomata::move(string input)
{
  current_state_ = table_.compute_next_state(current_state_, input);
}

bool FiniteAutomata::has_accepted()
{
  if (final_states_.find(current_state_) == final_states_.end())
    return false;
  else
    return true;
}