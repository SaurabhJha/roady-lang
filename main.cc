#include "./lexical_analyzer.h"
#include <iostream> // TODO: Remove this
#include <unordered_set> // TODO: Remove this.
#include <string>

using namespace std;

int main()
{
  while (1) {
    string input;
    cin >> input;
    for (Token token : tokenize(input)) {
      token.print();
      cout << " ";
    }
    cout << "\n";
  }
}