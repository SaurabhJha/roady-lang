#include <deque>
#include <iterator>

#include "parser/parser.h"

namespace parser {

parser::Production LRItem::get_production() {
  return production_;
}

int LRItem::get_position_in_body() {
  return position_in_body_;
}

bool LRItem::has_position_at_the_end() {
  return production_.get_body().size() == position_in_body_;
}

std::string LRItem::get_next_symbol() {
  return production_.get_body()[position_in_body_];
}

bool operator==(LRItem lr_item_lhs, LRItem lr_item_rhs) {
  return (lr_item_lhs.get_production() == lr_item_rhs.get_production() &&
          lr_item_lhs.get_position_in_body() ==
          lr_item_rhs.get_position_in_body());
}

LRItemSet::LRItemSet(const parser::LRItem& lr_item, parser::Grammar grammar) {
  std::vector<LRItem> items = {lr_item};
  std::deque<LRItem> queue = {lr_item};

  while (!queue.empty()) {
    auto current_item = queue.front();
    queue.pop_front();
    if (!current_item.has_position_at_the_end()) {
      auto next_symbol_of_item = current_item.get_next_symbol();
      if (grammar.is_non_terminal(next_symbol_of_item)) {
        for (const auto& production : grammar.get_productions_of_non_terminal(
            next_symbol_of_item)) {
          auto candidate_item = LRItem(production, 0);
          if (std::find(std::begin(items), std::end(items), candidate_item)
              == std::end(items)) {
            items.push_back(candidate_item);
            queue.push_back(candidate_item);
          }
        }
      }
    }
  }

  items_ = items;
}

std::vector<LRItem> LRItemSet::get_items() {
  return items_;
}

std::vector<std::string> LRItemSet::get_symbols_to_reduce_on(
    parser::Grammar grammar) {
  std::vector<std::string> symbols_to_reduce_on = {};
  for (auto item : items_) {
    if (item.has_position_at_the_end()) {
      auto production_head = item.get_production().get_head();
      for (const auto& symbol : grammar.compute_follow_set(production_head)) {
        symbols_to_reduce_on.push_back(symbol);
      }
    }
  }

  return symbols_to_reduce_on;
}

bool operator==(LRItemSet lr_item_set_lhs, LRItemSet lr_item_set_rhs) {
  if (lr_item_set_lhs.get_items().size() !=
      lr_item_set_rhs.get_items().size()) {
    return false;
  }

  auto items_from_lhs = lr_item_set_lhs.get_items();
  auto items_from_rhs = lr_item_set_rhs.get_items();
  for (const auto& item_from_lhs : items_from_lhs) {
    if (std::find(std::begin(items_from_rhs),
                  std::end(items_from_rhs),
                  item_from_lhs) == std::end(items_from_rhs)) {
      return false;
    }
  }

  for (const auto& item_from_rhs : items_from_rhs) {
    if (std::find(std::begin(items_from_lhs),
                  std::end(items_from_lhs),
                  item_from_rhs) == std::end(items_from_lhs)) {
      return false;
    }
  }

  return true;
}

}  // namespace parser
