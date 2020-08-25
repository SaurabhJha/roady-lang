#include <iterator>
#include <utility>

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

LRItemSet::LRItemSet(
    LRItemSet item_set,
    const std::string& transition_symbol,
    const parser::Grammar& grammar) {
  std::vector<LRItem> new_items = {};

  for (auto item : item_set.get_items()) {
    if (!item.has_position_at_the_end() &&
        item.get_next_symbol() == transition_symbol) {
      auto candidate_new_item = LRItem(
          item.get_production(),
          item.get_position_in_body() + 1);
      auto candidate_new_item_set = LRItemSet(candidate_new_item, grammar);
      for (const auto& candidate_item : candidate_new_item_set.get_items()) {
        if (std::find(
            std::begin(new_items), std::end(new_items), candidate_item)
            == std::end(new_items)) {
          new_items.push_back(candidate_item);
        }
      }
    }
  }

  items_ = new_items;
}

std::vector<LRItem> LRItemSet::get_items() {
  return items_;
}

std::vector<std::pair<std::string, LRItemSet>> LRItemSet::get_transitions(
    const parser::Grammar& grammar) {
  std::vector<std::pair<std::string, LRItemSet>> transitions;

  for (auto item : items_) {
    if (!item.has_position_at_the_end()) {
      auto next_symbol = item.get_next_symbol();
      auto next_item_set = LRItemSet(*this, next_symbol, grammar);
      transitions.emplace_back(next_symbol, next_item_set);
    }
  }

  return transitions;
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

void ParsingTableRow::add_symbol_action_pair(
    const std::string& symbol, ParsingAction action) {
  row_[symbol] = action;
}

ParsingAction ParsingTableRow::operator[](const std::string& symbol) {
  if (row_.find(symbol) == row_.end()) {
    ParsingAction error_action(ParsingActionType::error, -1);
    return error_action;
  }
  return row_[symbol];
}

void ParsingTable::add_new_entry(
    int state, const std::string& symbol, ParsingAction action) {
  auto row = table_[state];
  row.add_symbol_action_pair(symbol, action);
  table_[state] = row;
}

ParsingTableRow ParsingTable::operator[](int state) {
  return table_[state];
}

std::string map_token_type_to_terminal(tokenizer::TokenType token_type) {
  if (token_type == tokenizer::TokenType::id) {
    return "id";
  } else if (token_type == tokenizer::TokenType::number) {
    return "number";
  } else if (token_type == tokenizer::TokenType::plus) {
    return "+";
  } else if (token_type == tokenizer::TokenType::minus) {
    return "-";
  } else if (token_type == tokenizer::TokenType::star) {
    return "*";
  } else if (token_type == tokenizer::TokenType::slash) {
    return "/";
  } else if (token_type == tokenizer::TokenType::equals) {
    return "=";
  } else if (token_type == tokenizer::TokenType::double_equals) {
    return "==";
  } else if (token_type == tokenizer::TokenType::open_paren) {
    return "(";
  } else if (token_type == tokenizer::TokenType::closed_paren) {
    return ")";
  } else if (token_type == tokenizer::TokenType::dollar) {
    return "$";
  }

  return "";
}

Parser::Parser(Grammar grammar) {
  grammar_ = grammar;
  stack_ = {0};
  has_accepted_ = false;
  is_stuck_ = false;

  auto start_production = grammar.get_productions_of_non_terminal(
      grammar.get_start_symbol())[0];
  auto start_lr_item = LRItem(start_production, 0);
  auto start_lr_item_set = LRItemSet(start_lr_item, grammar);

  std::deque<LRItemSet> queue = {start_lr_item_set};
  std::vector<LRItemSet> seen_item_sets = {start_lr_item_set};

  while (!queue.empty()) {
    auto current_lr_item_set = queue.front();
    queue.pop_front();
    auto current_state_number = std::distance(
        std::begin(seen_item_sets),
        std::find(
            std::begin(seen_item_sets),
            std::end(seen_item_sets),
            current_lr_item_set));

    // Populate shift moves for the current state.
    for (const auto& transition :
         current_lr_item_set.get_transitions(grammar)) {
      auto transition_symbol = transition.first;
      auto next_lr_item_set = transition.second;
      if (std::find(
          std::begin(seen_item_sets),
          std::end(seen_item_sets),
          next_lr_item_set) == std::end(seen_item_sets)) {
        seen_item_sets.push_back(next_lr_item_set);
        queue.push_back(next_lr_item_set);
      }
      auto next_state_number = std::distance(
          std::begin(seen_item_sets),
          std::find(
              std::begin(seen_item_sets),
              std::end(seen_item_sets),
              next_lr_item_set));
      auto next_action = ParsingAction(
          ParsingActionType::shift, next_state_number);
      table_.add_new_entry(
          current_state_number, transition_symbol, next_action);
    }

    // Populate reduce moves for the current state.
    for (auto lr_item : current_lr_item_set.get_items()) {
      if (lr_item.has_position_at_the_end()) {
        auto production = lr_item.get_production();
        auto production_head = production.get_head();
        auto production_number = grammar.get_production_number(production);
        for (const auto& transition_symbol :
             grammar.compute_follow_set(production_head)) {
          if (transition_symbol == "$" &&
              production_head == grammar.get_start_symbol()) {
            auto next_action = ParsingAction(
                ParsingActionType::accept, -1);
            table_.add_new_entry(
                current_state_number, transition_symbol, next_action);
          } else {
            auto next_action = ParsingAction(
                ParsingActionType::reduce, production_number);
            table_.add_new_entry(
                current_state_number, transition_symbol, next_action);
          }
        }
      }
    }
  }
}

void Parser::parse(std::vector<tokenizer::Token> tokens) {
  input_ = std::move(tokens);
  current_position_ = 0;
}

std::pair<ParsingActionType, Production> Parser::make_next_move() {
  auto next_token = input_[current_position_];
  auto token_string = map_token_type_to_terminal(next_token.get_token_type());
  auto current_state = stack_.back();
  auto next_action = table_[current_state][token_string];

  if (next_action.get_action_type() == ParsingActionType::shift) {
    auto next_state = next_action.get_number();
    stack_.push_back(next_state);
    current_position_ += 1;
    return {ParsingActionType::shift, Production()};
  } else if (next_action.get_action_type() == ParsingActionType::reduce) {
    auto production_number = next_action.get_number();
    auto production = grammar_.get_production_by_number(production_number);
    for (int idx = 0; idx < production.get_body().size(); ++idx) {
      stack_.pop_back();
    }
    auto next_state = table_[stack_.back()][production.get_head()].get_number();
    stack_.push_back(next_state);
    return {ParsingActionType::reduce, production};
  } else if (next_action.get_action_type() == ParsingActionType::accept) {
    has_accepted_ = true;
    is_stuck_ = false;
    return {ParsingActionType::accept, Production()};
  } else {
    is_stuck_ = true;
    has_accepted_ = false;
    return {ParsingActionType::error, Production()};
  }
}

}  // namespace parser
