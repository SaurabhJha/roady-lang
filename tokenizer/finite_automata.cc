#include "tokenizer/finite_automata.h"

#include <algorithm>

namespace finite_automata {

void TransitionTableRow::increase_state_numbers_by(int number)
{
  for (const auto& input_states_pair : row_) {
    auto old_next_states = input_states_pair.second;
    std::unordered_set<int> new_next_states;
    for (auto old_next_state : old_next_states)
      new_next_states.insert(old_next_state + number);
    row_[input_states_pair.first] = new_next_states;
  }
}

void TransitionTableRow::add_input_state_pair(const std::string &input, int next_state)
{
  row_[input].insert(next_state);
}

void TransitionTable::increase_state_numbers_by(int number)
{
  std::unordered_map<int, TransitionTableRow> new_table;

  for (auto& state_row_pair : table_) {
    auto old_state = state_row_pair.first;
    auto row = state_row_pair.second;
    row.increase_state_numbers_by(number);
    new_table[old_state+number] = row;
  }
  table_ = new_table;
}

void TransitionTable::add_new_transition(int prev_state, int next_state, const std::string &input)
{
  auto row = table_[prev_state];
  row.add_input_state_pair(input, next_state);
  table_[prev_state] = row;
}

TransitionTable TransitionTable::combine_with(const TransitionTable& other_table)
{
  TransitionTable combined_table;
  // Add all transitions from the first transition table to the resultant combined table.
  for (auto& state_row_pair : table_) {
    auto curr_state = state_row_pair.first;
    auto row = state_row_pair.second;
    for (const auto& input_states_pair : row.row_) {
      auto input = input_states_pair.first;
      for (auto next_state : input_states_pair.second) {
        combined_table.add_new_transition(curr_state, next_state, input);
      }
    }
  }

  // Add all transitions from the second transition table to the resultant combined table.
  for (auto& state_row_pair : other_table.table_) {
    auto curr_state = state_row_pair.first;
    auto row = state_row_pair.second;
    for (const auto& input_states_pair : row.row_) {
      auto input = input_states_pair.first;
      for (auto next_state : input_states_pair.second) {
        combined_table.add_new_transition(curr_state, next_state, input);
      }
    }
  }

  return combined_table;
}

std::unordered_set<int> FiniteAutomata::compute_epsilon_closure(int state)
{
  // Memoization.
  if (closure_sets_.find(state) != closure_sets_.end())
    return closure_sets_[state];

  std::unordered_set<int> closure_set = {state};
  for (auto curr_state : closure_set) {
    for (auto next_state : table_[curr_state][""]) {
      closure_set.insert(next_state);
      for (auto skip_state : compute_epsilon_closure(next_state))
        closure_set.insert(skip_state);
    }
  }
  closure_sets_[state] = closure_set;
  return closure_set;
}

void FiniteAutomata::reset()
{
  current_states_ = compute_epsilon_closure(initial_state_);
}

void FiniteAutomata::increase_state_numbers_by(int number)
{
  initial_state_ += number;
  final_state_ += number;
  table_.increase_state_numbers_by(number);
  reset();
}

void FiniteAutomata::add_new_transition(int prev_state, int next_state, const std::string &input)
{
  auto row = table_[prev_state];
  row.add_input_state_pair(input, next_state);
  table_.add_new_transition(prev_state, next_state, input);
}

FiniteAutomata FiniteAutomata::union_with(FiniteAutomata other_automata)
{
  // Increase all state numbers of the first automaton by 1 and of the second
  // automaton by the final state of the first automaton.
  increase_state_numbers_by(1);
  int final_state_for_first_automata = get_final_state();
  other_automata.increase_state_numbers_by(final_state_for_first_automata);

  // Combine the two transition tables into one.
  auto table1 = get_transition_table();
  auto table2 = other_automata.get_transition_table();
  auto combined_table = table1.combine_with(table2);

  // Create a new start state and a new accept state.
  int combined_start_state = 1;
  int final_state1 = get_final_state();
  int final_state2 = other_automata.get_final_state();
  int combined_final_state = std::max(final_state1, final_state2) + 1;

  // Add new transitions to complete the union. The new transitions are.
  // 1. The new start state has an epsilon transition to the start states of both operand automata.
  // 2. The final states of each operand automaton have an epsilon transition to the new final state.
  auto start_state1 = get_start_state();
  auto start_state2 = other_automata.get_start_state();
  combined_table.add_new_transition(combined_start_state, start_state1, "");
  combined_table.add_new_transition(combined_start_state, start_state2, "");
  combined_table.add_new_transition(final_state1, combined_final_state, "");
  combined_table.add_new_transition(final_state2, combined_final_state, "");

  // Construct a new automata using the new transition table.
  FiniteAutomata combined_automata(combined_start_state, combined_final_state, combined_table);
  return combined_automata;
}

FiniteAutomata FiniteAutomata::concatenate_with(FiniteAutomata other_automata)
{
  // Increase all state numbers of the first automaton by 1 and of the second
  // automaton by the final state of the first automaton.
  increase_state_numbers_by(1);
  int final_state_for_first_automata = get_final_state();
  other_automata.increase_state_numbers_by(final_state_for_first_automata);

  // Combine the two transition tables into one.
  auto table1 = get_transition_table();
  auto table2 = other_automata.get_transition_table();
  auto combined_table = table1.combine_with(table2);

  // Add a new transition to complete the concatenation. The new transitions is:
  // an epsilon transition from the final state of the first operand automaton to the start
  // state of the second operand automaton.
  auto final_state1 = get_final_state();
  auto start_state2 = other_automata.get_start_state();
  combined_table.add_new_transition(final_state1, start_state2, "");

  // Construct a new automata using the new transition table. The start state is the start state
  // of the first operand automaton and the final state is the final state of the second operand
  // automaton.
  auto start_state1 = get_start_state();
  auto final_state2 = other_automata.get_final_state();
  FiniteAutomata combined_automata(start_state1, final_state2, combined_table);
  return combined_automata;
}

FiniteAutomata FiniteAutomata::compute_exponentiation()
{
  // Increase all state numbers by 1.
  increase_state_numbers_by(1);

  // Add new transitions to complete exponentiation
  auto table = get_transition_table();
  auto old_start_state = get_start_state();
  auto new_start_state = 1;
  auto old_final_state = get_final_state();
  auto new_final_state = old_final_state + 1;
  table.add_new_transition(new_start_state, old_start_state, "");
  table.add_new_transition(old_final_state, new_final_state, "");
  table.add_new_transition(old_final_state, old_start_state, "");
  table.add_new_transition(new_start_state, new_final_state, "");

  FiniteAutomata resultant_automata(new_start_state, new_final_state, table);
  return resultant_automata;
}

void FiniteAutomata::move(const std::string& input)
{
  std::unordered_set<int> next_states;
  for (auto current_state : current_states_) {
    auto immediate_next_states = table_[current_state][input];
    for (auto immediate_next_state : immediate_next_states)
      for (auto closed_next_state : compute_epsilon_closure(immediate_next_state))
        next_states.insert(closed_next_state);
  }

  current_states_ = next_states;
}

bool FiniteAutomata::has_accepted()
{
  return current_states_.find(final_state_) != current_states_.end();
}

bool FiniteAutomata::has_died()
{
  return current_states_.empty();
}

}