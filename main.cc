#include "./lexical_analyzer.h"
#include <iostream> // TODO: Remove this
#include <unordered_set> // TODO: Remove this.
#include <string>

using namespace std;

int main()
{
  string test_input = "12+2*3";
  // id: 2, number: 2, plus: 0, star: 0
  FiniteAutomaton automaton = construct_automaton_for_plus();
  cout << get_next_lexeme_end(test_input, 2, automaton) << "\n";
}