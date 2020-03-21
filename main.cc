#include <vector>
#include <iostream> // TODO: Remove this.
#include <string>

#include "./parser.h"
#include "lexical_analyzer.h"
using namespace std;

int main()
{
  while (true) {
    cout << "> ";
    string raw_input;
    getline(cin, raw_input);
    string formatted_input = remove_whitespace(raw_input);
    parse(tokenize(formatted_input));
    cout << "\n";
  }
}