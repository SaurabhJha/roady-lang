#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace parser {

class ProductionBody {
 private:
  std::vector<std::string> body_;

 public:
  ProductionBody() = default;
  ProductionBody(std::vector<std::string> body)
    :body_{std::move(body)} {}
  ~ProductionBody() = default;

  std::vector<std::string> get_symbols();
};

class Production {
 private:
  std::string head_;
  ProductionBody body_;

 public:
  Production() = default;
  Production(std::string head, std::vector<std::string> body_vector)
    :head_{std::move(head)}
  {
    ProductionBody body(std::move(body_vector));
    body_ = body;
  }
  Production(std::string head, const ProductionBody& body)
      :head_{std::move(head)}, body_{body} {}
  ~Production() = default;

  std::string get_head();
  ProductionBody get_body();
};

class Grammar {
 private:
  std::unordered_map<std::string, std::vector<ProductionBody>> grammar_table_;
  std::unordered_map<std::string, std::unordered_set<std::string>> firsts_table_;
  std::unordered_map<std::string, std::unordered_set<std::string>> follows_table_;

  bool is_terminal(const std::string& grammar_symbol);

 public:
  Grammar() = default;
  ~Grammar() = default;

  std::vector<ProductionBody> operator[](const std::string& non_terminal) { return grammar_table_[non_terminal]; }
  void add_production(Production production);
  std::unordered_set<std::string> compute_first(const std::string& grammar_symbol);
  std::unordered_set<std::string> compute_follow(const std::string& non_terminal);
};

}