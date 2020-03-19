#include <vector>
#include <iostream> // TODO: Remove this.

#include "./parser.h"
using namespace std;

int main()
{
  Production production("E", vector<string> {"T", "E'"});
  ParseTable table;
  table.add_rule("E", "number", production);
  vector<string> test {table["E"]["number"].apply()};
  for (auto i=test.size() - 1; i!=-1; --i) {
    cout << test[i];
  }
  cout << "\n";
}