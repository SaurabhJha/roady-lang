#include <vector>

#include "./parser.h"
using namespace std;

vector<string> Production::apply() {
  return right_side_;
}

void ParseTableRow::add_terminal_production_pair(string terminal, Production production)
{
  row_[terminal] = production;
}

Production ParseTableRow::operator[](string terminal) {
  return row_[terminal];
}

void ParseTable::add_rule(string non_terminal, string terminal, Production production) {
  ParseTableRow row = table_[non_terminal];
  row.add_terminal_production_pair(terminal, production);
  table_[non_terminal] = row;
}

ParseTableRow ParseTable::operator[](string non_terminal) {
  return table_[non_terminal];
}
