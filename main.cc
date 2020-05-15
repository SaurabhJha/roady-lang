#include <iostream>
#include <string>
#include <vector>

#include "./parser_top_down.h"
#include "./parser_bottom_up.h"
using namespace std;

int main()
{
  while (true) {
    string input_string;
    cout << "#> ";
    getline(cin, input_string);
    string processed_string = remove_whitespace(input_string);

    Tokenizer tokenizer(processed_string, 0);
    BottomUpParser parser;
    while (tokenizer.there_is_more_input()) {
      auto next_token = tokenizer.get_next_token();
      parser.parse_next_token(next_token);
      if (parser.has_failed()) {
        cout << "Failure at: " << next_token.get_lexeme() << "\n";
        break;
      }
    }
    auto end_token = Token(TokenType::dollar, "");
    parser.parse_next_token(end_token);
    if (parser.has_failed()) {
      cout << "Unsuccesful parse\n";
    } else {
      for (auto production : parser.get_reductions_applied()) {
        production.print();
        cout << "\n";
      }
      cout << "Successful parse\n";
    }
    cout << "\n";
  }
}
