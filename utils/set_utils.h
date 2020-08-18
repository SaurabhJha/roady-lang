#ifndef UTILS_SET_UTILS_H_
#define UTILS_SET_UTILS_H_

#include <unordered_set>

namespace utils {

std::unordered_set<int> union_two_sets(
    const std::unordered_set<int>& set1, const std::unordered_set<int>& set2);

}  // namespace utils

#endif  // UTILS_SET_UTILS_H_
