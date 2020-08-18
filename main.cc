#include "tokenizer/tokenizer.h"

int main() {
  auto tok = tokenizer::Tokenizer();
  tok.tokenize("12+3");
}
