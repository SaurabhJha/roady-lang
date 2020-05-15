#ifndef ROADY_LANG_PARSER_TOP_DOWN_H_
#define ROADY_LANG_PARSER_TOP_DOWN_H_

#include <stack>
#include <string>
#include <utility>

#include "./parser_common.h"

/**
 * Data structure to represent an LL(1) parsing table row like this
 *          |term1|term2|term3|term4|
 * ---------|-----|-----|-----|-----|
 * nonterm1 |prod1|prod2|prod3|prod4|
 *
 * where nonterm1 is immaterial and not part of the data structure.
 * What we are looking for is a hash table where the keys are terminal symbols and values
 * are productions. So the above row would look like this
 *    { term1: prod1, term2: prod2, term3: prod3, term4: prod4 }
 */
class TopDownParsingTableRow {
 private:
  unordered_map<string, Production> row_;

 public:
  TopDownParsingTableRow() = default;
  ~TopDownParsingTableRow() = default;

  void add_terminal_production_pair(const string& terminal, Production production);
  Production operator[](const string& terminal);
};

/**
 * Data structure to represent an LL(1) parsing table like this
 *          |term1|term2|term3|term4|
 * ---------|-----|-----|-----|-----|
 * nonterm1 |prod1|     |prod3|     |
 * nonterm2 |     |prod3|     |prod4|
 *
 * What we are looking for is something where the API should look like this
 *     parse_tabletable[nonterm][term] = production
 */
class TopDownParsingTable {
 private:
  unordered_set<string> non_terminals_;
  unordered_set<string> terminals_;
  unordered_map<string, TopDownParsingTableRow> table_;

 public:
  TopDownParsingTable() = default;
  TopDownParsingTable(unordered_set<string> non_terminals, unordered_set<string> terminals)
      :non_terminals_{std::move(non_terminals)}, terminals_{std::move(terminals)}
  {
    for (const string& non_terminal : non_terminals_) {
      TopDownParsingTableRow row;
      table_[non_terminal] = row;
    }
  }
  ~TopDownParsingTable() = default;

  void add_rule(const string& non_terminal, const string& terminal, Production production);
  bool is_non_terminal(const string& symbol);
  bool is_terminal(const string& symbol);
  TopDownParsingTableRow operator[](const string& non_terminal);
};
/**
 * An LL(1) parser implementation. It contains all the state necessary to incrementally process tokens
 * output by the tokenizer.
 */
class TopDownParser {
 private:
  stack<string> stack_;
  TopDownParsingTable parsing_table_;
  vector<Production> productions_applied_;
  bool has_failed_;

  void apply_production(Production production);
  TopDownParsingTable construct_parsing_table();

 public:
  TopDownParser()
  {
    parsing_table_ = construct_parsing_table();
    // Initialize the stack with start symbol of the grammar and end of the input symbol '$'
    stack_.push("$");
    stack_.push("expr");
    productions_applied_ = {};
    has_failed_ = false;
  }
  ~TopDownParser() = default;

  void parse_next_token(Token token);
  bool has_failed();
  vector<Production> get_productions_applied();
};

#endif //ROADY_LANG_PARSER_TOP_DOWN_H_