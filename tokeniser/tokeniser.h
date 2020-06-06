#include <string>
#include <utility>

#include "tokeniser/finite_automata.h"

namespace tokeniser
{

class Tokeniser
{
 private:
  std::string input_;
  int current_input_idx_;
  std::string apply_automata(finite_automata::FiniteAutomata automaton);

 public:
  Tokeniser(std::string input, int current_input_idx)
    : input_{std::move(input)}, current_input_idx_{current_input_idx} {}
  ~Tokeniser() = default;

  void get_next_token();
};

}