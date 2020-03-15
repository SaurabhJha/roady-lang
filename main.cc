#include "./lexical_analyzer.h"
#include <iostream>

using namespace std;

int main()
{
  TransitionTable table(3);
  table.add_transition(1, 2, "a");
  cout << table.move(1, "a") << "\n";
  // hello!!
}