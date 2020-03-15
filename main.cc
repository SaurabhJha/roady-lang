#include "./lexical_analyzer.h"
#include <iostream>
#include <unordered_set>

using namespace std;

int main()
{
  TransitionTable table(3);
  table.add_transition(1, 2, "a");
  unordered_set<int> final_states = {2,3};
  int initial_state = 1;
  FiniteAutomata automata(table, initial_state, final_states);
  automata.move("a");
  cout << automata.has_accepted() << "\n";
}