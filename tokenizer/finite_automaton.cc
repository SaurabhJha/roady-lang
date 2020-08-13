#include "tokenizer/finite_automaton.h"

#include <deque>
#include <iterator>
#include <vector>

namespace tokenizer {

void TransitionGraphRow::add_input_state_pair(const std::string& input_symbol, int state) {
  adjacency_list_row_[input_symbol].insert(state);
}

void TransitionGraphRow::increment_values(int number) {
  std::unordered_map<std::string, std::unordered_set<int>> new_adjacency_list_row;

  for (const auto& string_vertices_pair : adjacency_list_row_) {
    auto input = string_vertices_pair.first;
    auto vertices = string_vertices_pair.second;
    for (auto vertex : vertices) {
      new_adjacency_list_row[input].insert(vertex + number);
    }
  }

  adjacency_list_row_ = new_adjacency_list_row;
}

std::unordered_set<std::string> TransitionGraphRow::get_non_epsilon_transitions() {
  std::unordered_set<std::string> non_epsilon_inputs;
  for (const auto& input_states_pair : adjacency_list_row_) {
    auto input = input_states_pair.first;
    if (!input.empty())
      non_epsilon_inputs.insert(input);
  }
  return non_epsilon_inputs;
}

std::unordered_set<int> TransitionGraphRow::operator[](const std::string& input) {
  return adjacency_list_row_[input];
}

void TransitionGraph::add_transition(int start_state, int end_state, const std::string& input_symbol) {
  auto row = adjacency_list_[start_state];
  row.add_input_state_pair(input_symbol, end_state);
  adjacency_list_[start_state] = row;
}

void TransitionGraph::increment_vertex_numbers(int number) {
  std::unordered_map<int, TransitionGraphRow> new_adjacency_list;

  for (const auto& vertex_row_pair : adjacency_list_) {
    auto vertex = vertex_row_pair.first;
    auto row = vertex_row_pair.second;
    row.increment_values(number);
    new_adjacency_list[vertex + number] = row;
  }

  adjacency_list_ = new_adjacency_list;
}

void TransitionGraph::combine_with(const TransitionGraph& other_graph) {
  auto first_adjacency_list = adjacency_list_;
  auto second_adjacency_list = other_graph.adjacency_list_;

  auto combined_adjacency_list = first_adjacency_list;
  combined_adjacency_list.insert(std::begin(second_adjacency_list), std::end(second_adjacency_list));

  adjacency_list_ = combined_adjacency_list;
}

TransitionGraphRow TransitionGraph::operator[](int state) {
  return adjacency_list_[state];
}

void DeterministicFiniteAutomaton::move(const std::string &input_symbol) {
  // There is only one next state for a state input pair.
  if (!is_dead_) {
    int next_state = -1;
    for (auto state : graph_[current_state_][input_symbol])
      next_state = state;
    current_state_ = next_state;
    if (current_state_ == -1) {
      is_dead_ = true;
      has_accepted_ = false;
    } else
      has_accepted_ = std::find(
          std::begin(final_states_),
          std::end(final_states_),
          current_state_) != std::end(final_states_);
  }
}

void DeterministicFiniteAutomaton::reset() {
  current_state_ = start_state_;
  is_dead_ = false;
  has_accepted_ = false;
}

bool DeterministicFiniteAutomaton::has_accepted() {
  return has_accepted_;
}

bool DeterministicFiniteAutomaton::is_dead() {
  return is_dead_;
}

NonDeterministicFiniteAutomaton::NonDeterministicFiniteAutomaton(const std::string& input_character) {
  start_state_ = 0;
  final_state_ = 1;
  graph_.add_transition(0, 1, input_character);
}

/**
 * There was a choice of doing this algorithm with either BFS or DFS. I chose DFS to workshop
 * my recursion.
 */
std::unordered_set<int> NonDeterministicFiniteAutomaton::compute_closure(int state) {
  std::unordered_set<int> closure_set = {state};
  for (auto state_reachable_on_epsilon : graph_[state][""]) {
    auto closure_set_of_next_state = compute_closure(state_reachable_on_epsilon);
    closure_set.insert(std::begin(closure_set_of_next_state), std::end(closure_set_of_next_state));
  }

  return closure_set;
}

std::unordered_set<int> NonDeterministicFiniteAutomaton::get_next_dfa_state(
    const std::unordered_set<int>& current_dfa_state, const std::string& transition_symbol) {
  std::unordered_set<int> next_dfa_state;
  for (auto state : current_dfa_state) {
    auto adjacency_list_row = graph_[state];
    auto next_nfa_states_set = adjacency_list_row[transition_symbol];
    for (auto next_nfa_state : next_nfa_states_set)
      next_dfa_state = union_two_sets(next_dfa_state, compute_closure(next_nfa_state));
  }

  return next_dfa_state;
}

void NonDeterministicFiniteAutomaton::increment_state_numbers(int number) {
  start_state_ += number;
  final_state_ += number;
  graph_.increment_vertex_numbers(number);
}

int NonDeterministicFiniteAutomaton::get_start_state() {
  return start_state_;
}

int NonDeterministicFiniteAutomaton::get_final_state() {
  return final_state_;
}

/**
 * We merge with another automaton on union using the following steps.
 * 1. We increment first automaton's states by 1.
 * 2. We increment second automaton's states by the final state of the first automaton plus one.
 * 3. We merge both automaton's transition graphs. The increment steps were necessary to avoid duplicate states.
 * 4. We add epsilon transitions from a newly created start state to both automaton's start states.
 * 5. We add epsilon transitions from both automaton's final states to a newly created final state.
 */
void NonDeterministicFiniteAutomaton::merge_on_union(NonDeterministicFiniteAutomaton other_automaton) {
  // Increment state numbers for both this automaton and the other automaton.
  increment_state_numbers(1);
  other_automaton.increment_state_numbers(get_final_state() + 1);

  // Merge the transition graph of this automaton with the other automaton's transition graph.
  auto combined_graph = graph_;
  combined_graph.combine_with(other_automaton.graph_);

  // Add epsilon transitions from a newly created start state to both automaton's start state.
  auto new_start_state = 0;
  auto this_automaton_start_state = get_start_state();
  auto other_automaton_start_state = other_automaton.get_start_state();
  combined_graph.add_transition(new_start_state, this_automaton_start_state, "");
  combined_graph.add_transition(new_start_state, other_automaton_start_state, "");

  // Add epsilon transitions from both automaton's final states to a newly created final state.
  auto this_automaton_final_state = get_final_state();
  auto other_automaton_final_state = other_automaton.get_final_state();
  auto new_final_state = other_automaton_final_state + 1;
  combined_graph.add_transition(this_automaton_final_state, new_final_state, "");
  combined_graph.add_transition(other_automaton_final_state, new_final_state, "");

  // Finally, update the attributes of this automaton with the combined automaton's attributes.
  start_state_ = new_start_state;
  final_state_ = new_final_state;
  graph_ = combined_graph;
}

/**
 * We merge with other automaton on concatenation using the following steps.
 * 1. We increment the other automaton's states by the final state of the first automaton plus one.
 * 2. We add an epsilon transition from the final state of the first automaton to the start state of
 *    the second automaton.
 * 3. We make the start state of the first automaton the start state of the new automaton.
 * 4. We make the final state of the second automaton the final state of the new automaton.
 */
void NonDeterministicFiniteAutomaton::merge_on_concatenation(NonDeterministicFiniteAutomaton other_automaton) {
  // Increment state numbers of the other automaton.
  auto this_automaton_final_state = get_final_state();
  other_automaton.increment_state_numbers(this_automaton_final_state + 1);

  // Merge the transition graph of this automaton with other automaton's transition graph.
  auto combined_graph = graph_;
  combined_graph.combine_with(other_automaton.graph_);

  // Add an epsilon transition from the final state of the first automaton to the start state of
  // the second automaton.
  auto other_automaton_start_state = other_automaton.get_start_state();
  combined_graph.add_transition(this_automaton_final_state, other_automaton_start_state, "");

  // Finally, update the attributes of this automaton with the combined automaton's attributes.
  auto this_automaton_start_state = get_start_state();
  auto other_automaton_final_state = other_automaton.get_final_state();
  start_state_ = this_automaton_start_state;
  final_state_ = other_automaton_final_state;
  graph_ = combined_graph;
}

/**
 * We apply kleene star to an automaton using the following steps.
 * 1. We increment the states by 1.
 * 2. We introduce a new start state 0 and add an epsilon transition from the new start state to the
 *    old start state.
 * 3. We introduce a new final state <final_state_ + 1> and add an epsilon transition from the old
 *    final state to the the new final state.
 * 4. We add an epsilon transition from the old final state to the old start state.
 * 5. We add an epsilon transition from the new start state to the new final state.
 */
void NonDeterministicFiniteAutomaton::apply_star() {
  // Increment states by 1.
  increment_state_numbers(1);

  // Create a new start state and add an epsilon transition from the new start state to the old start state.
  auto old_start_state = start_state_;
  auto new_start_state = 0;
  start_state_ = new_start_state;
  graph_.add_transition(new_start_state, old_start_state, "");

  // Create a new final state and add an epsilon transition from the old final state to the new final state.
  auto old_final_state = get_final_state();
  auto new_final_state = old_final_state + 1;
  final_state_ = new_final_state;
  graph_.add_transition(old_final_state, new_final_state, "");

  // Add an epsilon transition from the old final state to the old start state.
  graph_.add_transition(old_final_state, old_start_state, "");
  // Add an epsilon transition from the new start state to the final state.
  graph_.add_transition(new_start_state, new_final_state, "");

  // Finally, update the attributes of this automaton with the combined automaton's attributes.
  // graph is already updated.
  start_state_ = new_start_state;
  final_state_ = new_final_state;
}

/**
 * Convert an NFA into a DFA using breadth first search.
 */
DeterministicFiniteAutomaton NonDeterministicFiniteAutomaton::convert_to_dfa() {
  // Doubles as a set of seen states and to assign state numbers to new DFA states.
  std::vector<std::unordered_set<int>> seen_states;
  // queue for BFS.
  std::deque<std::unordered_set<int>> queue;
  // New DFS graph.
  TransitionGraph dfa_graph;

  // Start by computing the start state of the new DFA and inserting it into
  // the seen set and the queue.
  auto dfa_start_state = compute_closure(start_state_);
  seen_states.push_back(dfa_start_state);
  queue.push_back(dfa_start_state);

  // Do the BFS.
  while (!queue.empty()) {
    auto current_dfa_state = queue.front();
    queue.pop_front();
    auto current_dfa_state_number = std::distance(
        std::begin(seen_states),
        std::find(std::begin(seen_states), std::end(seen_states), current_dfa_state));
    for (auto state : current_dfa_state) {
      auto adjacency_list_row = graph_[state];
      for (const auto& transition_symbol : adjacency_list_row.get_non_epsilon_transitions()) {
        auto next_dfa_state = get_next_dfa_state(current_dfa_state, transition_symbol);
        if (std::find(std::begin(seen_states), std::end(seen_states), next_dfa_state) == std::end(seen_states)) {
          seen_states.push_back(next_dfa_state);
          queue.push_back(next_dfa_state);
        }
        auto next_dfa_state_number = std::distance(
            std::begin(seen_states),
            std::find(
                std::begin(seen_states),
                std::end(seen_states),
                next_dfa_state));
        dfa_graph.add_transition(current_dfa_state_number, next_dfa_state_number, transition_symbol);
      }
    }
  }

  int dfa_start_state_number {0};
  std::unordered_set<int> dfa_final_state_numbers;
  for (auto dfa_state : seen_states) {
    if (std::find(std::begin(dfa_state), std::end(dfa_state), final_state_) != std::end(dfa_state)) {
      auto dfa_state_number = std::distance(
          std::begin(seen_states),
          std::find(
              std::begin(seen_states),
              std::end(seen_states),
              dfa_state));
      dfa_final_state_numbers.insert(dfa_state_number);
    }
  }

  DeterministicFiniteAutomaton automaton(dfa_start_state_number, dfa_final_state_numbers, dfa_graph);
  return automaton;
}

std::unordered_set<int> union_two_sets(const std::unordered_set<int> &set1, const std::unordered_set<int>& set2) {
  std::unordered_set<int> two_set_union;
  for (auto element : set1) {
    two_set_union.insert(element);
  }
  for (auto element : set2) {
    two_set_union.insert(element);
  }

  return two_set_union;
}

std::unordered_set<int> intersect_two_sets(const std::unordered_set<int> &set1, const std::unordered_set<int> &set2) {
  std::unordered_set<int> two_set_intersection;
  for (auto element1 : set1) {
    for (auto element2 : set2) {
      if (element1 == element2) {
        two_set_intersection.insert(element1);
      }
    }
  }
  return two_set_intersection;
}

};