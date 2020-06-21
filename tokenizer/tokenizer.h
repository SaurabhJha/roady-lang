#include <string>
#include <utility>

#include "tokenizer/finite_automata.h"

namespace tokenizer
{

class tokenizer
{
 private:
  std::string input_;
  int current_input_idx_;
  std::string apply_automata(finite_automata::FiniteAutomata automaton);

 public:
  tokenizer(std::string input, int current_input_idx)
    : input_{std::move(input)}, current_input_idx_{current_input_idx} {}
  ~tokenizer() = default;

  void get_next_token();
};

}