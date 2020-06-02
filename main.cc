#include "tokeniser/regex.h"
#include <iostream>

int main()
{
  auto automata = regex::compile_regex_to_automata("(a|b)(c|d)");
  automata.move("a");
  std::cout << automata.has_accepted() << "\n";
  std::cout << automata.has_died() << "\n";
  automata.move("d");
  std::cout << automata.has_accepted() << "\n";
  std::cout << automata.has_died() << "\n";
}