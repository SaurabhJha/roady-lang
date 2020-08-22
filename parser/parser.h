#ifndef PARSER_GRAMMAR_H_
#define PARSER_GRAMMAR_H_

#include <utility>
#include <vector>

#include "parser/grammar.h"

namespace parser {

class LRItem {
 private:
  parser::Production production_;
  int position_in_body_{};

 public:
  LRItem() = default;
  LRItem(const parser::Production& production, int position_in_body)
    :production_{production}, position_in_body_{position_in_body}
  {}
  ~LRItem() = default;

  parser::Production get_production();
  int get_position_in_body();
  bool has_position_at_the_end();
  std::string get_next_symbol();
};

bool operator==(LRItem lr_item_lhs, LRItem lr_item_rhs);

class LRItemSet {
 private:
  std::vector<LRItem> items_;

 public:
  LRItemSet() = default;
  explicit LRItemSet(std::vector<LRItem> items)
    :items_{std::move(items)}
  {}
  LRItemSet(const parser::LRItem& lr_item, parser::Grammar grammar);
  ~LRItemSet() = default;

  std::vector<LRItem> get_items();
  std::vector<std::string> get_symbols_to_reduce_on(parser::Grammar grammar);
};

bool operator==(LRItemSet lr_item_set_lhs, LRItemSet lr_item_set_rhs);

}  // namespace grammar

#endif // PARSER_GRAMMAR_H_
