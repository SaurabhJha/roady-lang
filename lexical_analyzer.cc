#include <unordered_set>
#include <iostream> // TODO: Remove this.

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
  for (int state : final_states_) { // TODO: Do this in O(1) time.
    if (state == current_state_) return true;
  }
  return false;
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

FiniteAutomaton construct_automaton_for_plus()
{
  TransitionTable table(2);
  table.add_transition(1, 2, "+");
  int initial_state = 1;
  unordered_set<int> final_states = {2};
  return FiniteAutomaton(initial_state, final_states, table);
}

FiniteAutomaton construct_automaton_for_star()
{
  TransitionTable table(2);
  table.add_transition(1, 2, "*");
  int initial_state {1};
  unordered_set<int> final_states {2};
  return FiniteAutomaton(initial_state, final_states, table);
}
