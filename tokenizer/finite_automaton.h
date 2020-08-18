#ifndef TOKENIZER_FINITE_AUTOMATON_H_
#define TOKENIZER_FINITE_AUTOMATON_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace tokenizer {

class TransitionGraphRow {
 private:
  std::unordered_map<std::string, std::unordered_set<int>> adjacency_list_row_;

 public:
  TransitionGraphRow() = default;
  ~TransitionGraphRow() = default;

  void add_input_state_pair(const std::string& input_symbol, int state);
  void increment_values(int number);
  std::unordered_set<std::string> get_non_epsilon_transitions();
  std::unordered_set<int> operator[](const std::string& input);
};

class TransitionGraph {
 private:
  std::unordered_map<int, TransitionGraphRow> adjacency_list_;

 public:
  TransitionGraph() = default;
  ~TransitionGraph() = default;

  void add_transition(
      int start_state, int end_state, const std::string& input_symbol);
  void increment_vertex_numbers(int number);
  void combine_with(const TransitionGraph& other_graph);
  TransitionGraphRow operator[](int state);
};

class DeterministicFiniteAutomaton {
 private:
  int start_state_{};
  std::unordered_set<int> final_states_;
  int current_state_{};
  TransitionGraph graph_;
  bool has_accepted_ = false;
  bool is_dead_ = false;
 public:
  DeterministicFiniteAutomaton() = default;
  DeterministicFiniteAutomaton(
      int start_state, std::unordered_set<int> final_states,
      const TransitionGraph& graph)
      :start_state_{start_state}, final_states_{std::move(final_states)},
      current_state_{start_state}, graph_{graph}
  {}
  ~DeterministicFiniteAutomaton() = default;

  void move(const std::string& input_symbol);
  void reset();
  bool has_accepted();
  bool is_dead();
};

class NonDeterministicFiniteAutomaton {
 private:
  int start_state_{};
  int final_state_{};
  TransitionGraph graph_;
  std::unordered_map<int, std::unordered_set<int>> closure_sets_;

  std::unordered_set<int> compute_closure(int state);
  std::unordered_set<int> get_next_dfa_state(
      const std::unordered_set<int>& current_dfa_state,
      const std::string& transition_symbol);
  void increment_state_numbers(int number);

 public:
  NonDeterministicFiniteAutomaton() = default;
  explicit NonDeterministicFiniteAutomaton(const std::string& input_character);
  ~NonDeterministicFiniteAutomaton() = default;

  int get_start_state();
  int get_final_state();
  void merge_on_union(NonDeterministicFiniteAutomaton other_automaton);
  void merge_on_concatenation(NonDeterministicFiniteAutomaton other_automaton);
  void apply_star();
  DeterministicFiniteAutomaton convert_to_dfa();
};

}  // namespace tokenizer

#endif  // TOKENIZER_FINITE_AUTOMATON_H_
