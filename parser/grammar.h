#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace parser {

class Production {
 private:
  std::string head_;
  std::vector<std::string> body_;

 public:
  Production() = default;
  Production(std::string head, std::vector<std::string> body)
    :head_{std::move(head)}, body_{std::move(body)} {}
  ~Production() = default;

  std::string get_head() { return head_; }
  std::vector<std::string> get_body() { return body_; };
  bool operator==(Production& other_production);
};

class LR0AutomatonRow {
 private:
  std::unordered_map<std::string, int> row_;

 public:
  LR0AutomatonRow() = default;
  ~LR0AutomatonRow() = default;

  int operator[](const std::string& transition_symbol) { return row_[transition_symbol]; }
  void add_symbol_state_pair(const std::string& transition_symbol, int state)
  {
    row_[transition_symbol] = state;
  }
};

class LR0Automaton {
 private:
  std::unordered_map<int, LR0AutomatonRow> table_;

 public:
  LR0Automaton() = default;
  ~LR0Automaton() = default;

  LR0AutomatonRow operator[](int state) { return table_[state]; }
  void add_new_transition(int start_state, const std::string& transition_symbol, int end_state)
  {
    auto row = table_[start_state];
    row.add_symbol_state_pair(transition_symbol, end_state);
    table_[start_state] = row;
  }
};

class Grammar {
 private:
  std::string start_symbol_;
  std::vector<Production> productions_;
  std::unordered_map<std::string, std::unordered_set<std::string>> firsts_table_;
  std::unordered_map<std::string, std::unordered_set<std::string>> follows_table_;

  bool is_terminal(const std::string& grammar_symbol);

 public:
  Grammar(std::string start_symbol)
    :start_symbol_{std::move(start_symbol)} {}
  ~Grammar() = default;

  void add_production(const Production& production);
  std::unordered_set<std::string> compute_first(const std::string& grammar_symbol);
  std::unordered_set<std::string> compute_follow(const std::string& non_terminal);
  LR0Automaton construct_lr0_automaton();
};

class LR0Item {
 private:
  std::vector<Production> productions_;
  std::unordered_multimap<int, int> item_positions_in_productions_;

  bool is_non_terminal(const std::string& grammar_symbol);
  std::vector<int> get_production_numbers_for_non_terminal(const std::string& non_terminal);
  void include_non_kernel_items();

 public:
  LR0Item(std::vector<Production> productions, std::unordered_multimap<int, int> item_positions_in_bodies)
    :productions_{std::move(productions)}, item_positions_in_productions_{std::move(item_positions_in_bodies)}
  {
    include_non_kernel_items();
  }
  bool operator==(const LR0Item& other_item);
  std::unordered_map<std::string, LR0Item> get_all_transition_symbol_next_item_pairs();
  LR0Item compute_next_item_on_transition_symbol(const std::string& transition_symbol);
};

}