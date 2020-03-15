#ifndef LEXICAL_ANALYZER_H

#include <unordered_map>
#include <unordered_set>
#include <string>

using namespace std;

class TransitionTableRow {
  /**
   * {a: 2, b: 3, c: 4}
   */
 private:
  unordered_map<string, int> row_;

 public:
  TransitionTableRow() = default;
  ~TransitionTableRow() = default;

  int operator[](const string& input) { return row_[input]; }

  void add_pair(const string& input, int state) { row_[input] = state; }
};

class TransitionTable {
  /**
   * {
   *    1: {a: 2, b: 3},
   *    2: {b: 3, c: 4},
   * }
   */
 private:
  int number_of_states_;
  unordered_map<int, TransitionTableRow> table_;

 public:
  explicit TransitionTable(int ns);
  ~TransitionTable() = default;

  TransitionTableRow operator[](int state);

  void add_transition(int prev_state, int next_state, string input);
  int compute_next_state(int state, const string &input);
};

class FiniteAutomata {
 private:
  int initial_state_;
  unordered_set<int> final_states_;
  int current_state_;
  TransitionTable table_;

 public:
  FiniteAutomata(TransitionTable table, int initial_state, unordered_set<int> final_states)
    :table_{table}, initial_state_{initial_state},
    final_states_{final_states}, current_state_{initial_state} {}

  ~FiniteAutomata() = default;

  void move(string input);
  bool has_accepted();
};

#endif // LEXICAL_ANALYZER_H