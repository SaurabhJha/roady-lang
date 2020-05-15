#include "./parser_bottom_up.h"

void BottomUpParsingActionTableRow::add_terminal_action_pair(const string& terminal, BottomUpParsingAction action)
{
  row_[terminal] = action;
}

BottomUpParsingAction BottomUpParsingActionTableRow::operator[](const string& terminal)
{
  return row_[terminal];
}


void BottomUpParsingActionTable::add_rule(int state, const string& terminal, BottomUpParsingAction action)
{
  auto row = table_[state];
  row.add_terminal_action_pair(terminal, action);
  table_[state] = row;
}

BottomUpParsingActionTableRow BottomUpParsingActionTable::operator[](int state)
{
  return table_[state];
}

void BottomUpParsingGotoTableRow::add_nonterminal_state_pair(const string& nonterminal, int state)
{
  row_[nonterminal] = state;
}

int BottomUpParsingGotoTableRow::operator[](const string& nonterminal)
{
  return row_[nonterminal];
}

void BottomUpParsingGotoTable::add_rule(int state, const string& nonterminal, int new_state)
{
  auto row = table_[state];
  row.add_nonterminal_state_pair(nonterminal, new_state);
  table_[state] = row;
}

BottomUpParsingGotoTableRow BottomUpParsingGotoTable::operator[](int state)
{
  return table_[state];
}

void BottomUpParser::construct_productions()
{
  // expr' productions
  Production production_expr_prime = Production("expr'", vector<string> {"expr"});

  // expr productions
  Production production_expr_plus = Production("expr", vector<string> {"expr", "+", "term"});
  Production production_expr_minus = Production("expr", vector<string> {"expr", "-", "term"});
  Production production_expr_term = Production("expr", vector<string> {"term"});

  // term productions
  Production production_term_star = Production("term", vector<string> {"term", "*", "factor"});
  Production production_term_slash = Production("term", vector<string> {"term", "/", "factor"});
  Production production_term_factor = Production("term", vector<string> {"factor"});

  // factor productions
  Production production_factor_paren = Production("factor", vector<string> {"(", "expr", ")"});
  Production production_factor_number = Production("factor", vector<string> {"number"});

  numbered_reductions_[1] = production_expr_prime;
  numbered_reductions_[2] = production_expr_plus;
  numbered_reductions_[3] = production_expr_minus;
  numbered_reductions_[4] = production_expr_term;
  numbered_reductions_[5] = production_term_star;
  numbered_reductions_[6] = production_term_slash;
  numbered_reductions_[7] = production_term_factor;
  numbered_reductions_[8] = production_factor_paren;
  numbered_reductions_[9] = production_factor_number;
}

void BottomUpParser::construct_action_table()
{
  action_table_.add_rule(
      0,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      0,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      1,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 5)
  );
  action_table_.add_rule(
      1,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 6)
  );
  action_table_.add_rule(
      1,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::accept, 0)
  );

  action_table_.add_rule(
      2,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );
  action_table_.add_rule(
      2,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );
  action_table_.add_rule(
      2,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 7)
  );
  action_table_.add_rule(
      2,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 8)
  );
  action_table_.add_rule(
      2,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );
  action_table_.add_rule(
      2,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 4)
  );

  action_table_.add_rule(
      3,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );
  action_table_.add_rule(
      3,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 7)
  );

  action_table_.add_rule(
      4,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      4,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      5,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      5,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      6,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      6,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      7,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      7,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      8,
      "number",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 16)
  );
  action_table_.add_rule(
      8,
      "(",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 4)
  );

  action_table_.add_rule(
      10,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 5)
  );
  action_table_.add_rule(
      10,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 6)
  );
  action_table_.add_rule(
      10,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 15)
  );

  action_table_.add_rule(
      11,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );
  action_table_.add_rule(
      11,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );
  action_table_.add_rule(
      11,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 7)
  );
  action_table_.add_rule(
      11,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 8)
  );
  action_table_.add_rule(
      11,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );
  action_table_.add_rule(
      11,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 2)
  );

  action_table_.add_rule(
      12,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );
  action_table_.add_rule(
      12,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );
  action_table_.add_rule(
      12,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 7)
  );
  action_table_.add_rule(
      12,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::shift, 8)
  );
  action_table_.add_rule(
      12,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );
  action_table_.add_rule(
      12,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 3)
  );

  action_table_.add_rule(
      13,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );
  action_table_.add_rule(
      13,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 5)
  );

  action_table_.add_rule(
      14,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );
  action_table_.add_rule(
      14,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 6)
  );

  action_table_.add_rule(
      15,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );
  action_table_.add_rule(
      15,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 8)
  );

  action_table_.add_rule(
      16,
      "+",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "-",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "*",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "/",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      ")",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
  action_table_.add_rule(
      16,
      "$",
      BottomUpParsingAction(BottomUpParsingActionType::reduce, 9)
  );
}

void BottomUpParser::construct_goto_table()
{
  goto_table_.add_rule(0, "expr", 1);
  goto_table_.add_rule(0, "term", 2);
  goto_table_.add_rule(0, "factor", 3);

  goto_table_.add_rule(4, "expr", 10);
  goto_table_.add_rule(4, "term", 2);
  goto_table_.add_rule(4, "factor", 3);

  goto_table_.add_rule(5, "term", 11);
  goto_table_.add_rule(5, "factor", 3);

  goto_table_.add_rule(6, "term", 12);
  goto_table_.add_rule(6, "factor", 3);

  goto_table_.add_rule(7, "factor", 13);
  goto_table_.add_rule(8, "factor", 14);
}

void BottomUpParser::shift(BottomUpParsingAction next_action)
{
  if (next_action.get_action_number() != 0) {
    auto next_state = next_action.get_action_number();
    auto stack_record = StackRecord(StackRecordType::state);
    stack_record.set_state(next_state);
    stack_.push(stack_record);
  } else {
    has_failed_ = true;
  }
}

BottomUpParsingAction BottomUpParser::reduce(BottomUpParsingAction next_action, string terminal)
{
  // Get the reduction to be applied.
  auto reduction_number = next_action.get_action_number();
  auto next_reduction = numbered_reductions_[reduction_number];
  reductions_applied_.push_back(next_reduction);

  // Do the stack manipulation.
  for (auto i = 0; i < next_reduction.get_right_side().size(); ++i)
    stack_.pop();
  auto resultant_nonterminal = next_reduction.get_left_side();
  auto state_on_top = stack_.top().get_state();
  auto next_state = goto_table_[state_on_top][resultant_nonterminal];
  auto new_stack_record = StackRecord(StackRecordType::state);
  new_stack_record.set_state(next_state);
  stack_.push(new_stack_record);

  // Return the next action.
  return action_table_[next_state][terminal];
}

/**
 * An implementation of SLR(1) shift reduce parser.
 */
void BottomUpParser::parse_next_token(Token token)
{
  auto terminal = map_token_type_to_terminal(token.get_token_type());
  auto next_action = action_table_[stack_.top().get_state()][terminal];

  // Keep reducing till no more reductions are possible.
  while (next_action.get_action_type() == BottomUpParsingActionType::reduce) {
    next_action = reduce(next_action, terminal);
  }

  // Shift now if that's the next move.
  if (next_action.get_action_type() == BottomUpParsingActionType::shift) {
    shift(next_action);
  }
}

bool BottomUpParser::has_failed()
{
  return has_failed_;
}

vector<Production> BottomUpParser::get_reductions_applied()
{
  return reductions_applied_;
}




