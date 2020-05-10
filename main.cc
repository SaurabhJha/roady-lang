#include <iostream>
#include <string>

#include "parser.h"
#include "lexical_analyzer.h"
using namespace std;

int main()
{
  while (true) {
    string input_string;
    cout << "#> ";
    getline(cin, input_string);
    string processed_string = remove_whitespace(input_string);
    Tokenizer tokenizer(processed_string, 0);
    while (tokenizer.is_there_more_input()) {
      tokenizer.get_next_token().print();
    }
    cout << "\n";
  }
}
