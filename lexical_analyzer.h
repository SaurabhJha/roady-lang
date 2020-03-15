#ifndef LEXICAL_ANALYZER_H

#include <unordered_map>
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
  int initial_state_;
  int final_state_;
  unordered_map<int, TransitionTableRow> table_;
 public:
  explicit TransitionTable(int ns);
  ~TransitionTable() = default;

  TransitionTableRow operator[](int state);

  void add_transition(int prev_state, int next_state, string input);
  int move(int state, const string &input);
};

#endif // LEXICAL_ANALYZER_H