#include <iostream>
#include <string>
#include <vector>

#include "./parser.h"
#include "./lexical_analyzer.h"
using namespace std;

int main()
{
  while (true) {
    string input_string;
    cout << "#> ";
    getline(cin, input_string);
    string processed_string = remove_whitespace(input_string);
    Tokenizer tokenizer(processed_string, 0);
    vector<Token> tokens = {};
    while (tokenizer.is_there_more_input()) {
      tokens.push_back(tokenizer.get_next_token());
    }
    parse(tokens);
    cout << "\n";
  }
}
