#include "tokeniser/regex.h"
#include "tokeniser/tokeniser.h"
#include <iostream>

#include <chrono>

int main()
{
  auto t1 = std::chrono::high_resolution_clock::now();
  auto test_tokeniser = tokeniser::Tokeniser("zdsdf=123", 0);
  test_tokeniser.get_next_token();
  test_tokeniser.get_next_token();
  test_tokeniser.get_next_token();

  auto t2 = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

  std::cout << "Duration: " << duration << "\n";
  return 0;
}