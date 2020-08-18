#include "utils/set_utils.h"

#include <unordered_set>

namespace utils {

std::unordered_set<int> union_two_sets(
    const std::unordered_set<int>& set1,
    const std::unordered_set<int>& set2) {
  std::unordered_set<int> two_set_union;
  for (auto element : set1) {
    two_set_union.insert(element);
  }
  for (auto element : set2) {
    two_set_union.insert(element);
  }
  return two_set_union;
}

}  // namespace utils
