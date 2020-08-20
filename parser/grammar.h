#ifndef PARSER_PARSER_H_
#define PARSER_PARSER_H_

#include <string>
#include <utility>
#include <unordered_set>
#include <vector>

namespace parser {

class Production {
 private:
  std::string head_;
  std::vector<std::string> body_;

 public:
  Production() = default;
  Production(std::string head, std::vector<std::string> body)
    :head_{std::move(head)}, body_{std::move(body)}
  {}
  ~Production() = default;

  std::string get_head();
  std::vector<std::string> get_body();
};

class Grammar {
 private:
  std::vector<Production> productions_;
  std::string start_symbol_;

  bool is_non_terminal(const std::string& grammar_symbol);
  std::vector<Production> get_productions_of_non_terminal(
      const std::string& non_terminal);

 public:
  Grammar() = default;
  Grammar(std::vector<Production> productions, std::string start_symbol)
    :productions_{std::move(productions)},
    start_symbol_{std::move(start_symbol)}
  {}
  ~Grammar() = default;

  std::unordered_set<std::string> compute_first_set(
      const std::string& grammar_symbol);
  std::unordered_set<std::string> compute_first_set(
      const std::vector<std::string>& grammar_symbols);
  std::unordered_set<std::string> compute_follow_set(
      const std::string& non_terminal);
};

}  // namespace parser

#endif // PARSER_PARSER_H_
