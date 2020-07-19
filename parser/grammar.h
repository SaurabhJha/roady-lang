#ifndef PARSER_GRAMMAR_H_
#define PARSER_GRAMMAR_H_

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace parser_grammar {

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
  std::vector<std::string> get_body() { return body_; }
  bool operator==(const Production& other_production);
};

class Grammar {
 private:
  std::string start_symbol_;
  std::vector<Production> productions_;
  std::unordered_map<std::string, std::unordered_set<std::string>>
    firsts_table_;
  std::unordered_map<std::string, std::unordered_set<std::string>>
    follows_table_;

 public:
  Grammar() = default;
  explicit Grammar(std::string start_symbol)
    :start_symbol_{std::move(start_symbol)} {}
  ~Grammar() = default;

  std::string get_start_symbol();
  std::vector<Production> get_productions();
  std::vector<Production> get_productions_of_non_terminal(const std::string& terminal);
  bool is_terminal(const std::string& grammar_symbol);
  void add_production(const Production& production);
  std::unordered_set<std::string> compute_first(
      const std::string& grammar_symbol);
  std::unordered_set<std::string> compute_follow(
      const std::string& non_terminal);
};

}  // namespace parser_grammar

#endif  // PARSER_GRAMMAR_H_
