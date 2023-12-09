#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <charconv>
#include <deque>
#include <fmt/core.h>
#include <fmt/format.h>
#include <ranges>
#include <scn/scan.h>
#include <string>
#include <string_view>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

std::vector<std::vector<int>> parseSequences(const std::vector<std::string>& lines)
{
  return toVector(lines | v::transform([](const std::string& line) {
                    return toVector(line | v::split(' ') | v::transform([](auto&& rng) {
                                      std::string_view v(rng);
                                      int value = -1;
                                      std::from_chars(v.begin(), v.end(), value);
                                      return value;
                                    }));
                  }));
}

int64_t task1(std::vector<std::vector<int>> sequences)
{
  int64_t result{};
  for (auto& sequence : sequences) {
    std::vector<std::vector<int>> tree;
    tree.reserve(sequence.size());
    tree.push_back(sequence);
    fmt::println("Sequence: {}", fmt::join(tree[0], " "));

    for (std::size_t i = 1; i < sequence.size(); i++) {
      tree.push_back(toVector(tree[i - 1] | v::slide(2) | v::filter([](auto&& rng) { return rng.size() == 2; }) |
                              v::transform([](auto&& rng) { return rng[1] - rng[0]; })));
      fmt::println("Sequence: {}", fmt::join(tree[i], " "));
      if (r::all_of(tree.back(), [](auto& v) { return v == 0; })) {
        break;
      }
    }
    tree.back().push_back(0);
    fmt::println("Sequence: {}", fmt::join(tree.back(), " "));
    // Add one at the end of each sub tree
    for (auto it = tree.rbegin() + 1; it != tree.rend(); it++) {
      auto lowerLevel = (it - 1)->back();
      auto left = it->back();
      auto right = left + lowerLevel;
      it->push_back(right);
      fmt::println("Sequence: {}", fmt::join(*it, " "));
    }
    result += tree[0].back();
  }
  return result;
}

int64_t task2(std::vector<std::vector<int>> sequences)
{
  int64_t result{};
  for (auto& sequence : sequences) {
    std::vector<std::deque<int>> tree;
    tree.reserve(sequence.size());
    tree.emplace_back(sequence.begin(), sequence.end());
    fmt::println("Sequence: {}", fmt::join(tree[0], " "));

    for (std::size_t i = 1; i < sequence.size(); i++) {
      tree.push_back(toDeque(tree[i - 1] | v::slide(2) | v::filter([](auto&& rng) { return rng.size() == 2; }) |
                              v::transform([](auto&& rng) { return rng[1] - rng[0]; })));
      fmt::println("Sequence: {}", fmt::join(tree[i], " "));
      if (r::all_of(tree.back(), [](auto& v) { return v == 0; })) {
        break;
      }
    }
    tree.back().push_front(0);
    fmt::println("Sequence: {}", fmt::join(tree.back(), " "));
    // Add one at the end of each sub tree
    for (auto it = tree.rbegin() + 1; it != tree.rend(); it++) {
      auto lowerLevel = (it - 1)->front();
      auto right = it->front();
      auto left = right - lowerLevel;
      it->push_front(left);
      fmt::println("Sequence: {}", fmt::join(*it, " "));
    }
    result += tree[0].front();
  }
  return result;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "0 3 6 9 12 15",
    "1 3 6 10 15 21",
    "10 13 16 21 30 45",
  };

  auto sequences = parseSequences(input);

  REQUIRE(sequences.size() == 3);
  REQUIRE(sequences[0] == std::vector{0, 3, 6, 9, 12, 15});
  REQUIRE(sequences[1] == std::vector{1, 3, 6, 10, 15, 21});
  REQUIRE(sequences[2] == std::vector{10, 13, 16, 21, 30, 45});

  REQUIRE(task1({sequences[0]}) == 18);
  REQUIRE(task1(sequences) == 114);


  REQUIRE(task2({sequences[0]}) == -3);
  REQUIRE(task2({sequences[1]}) == 0);
  REQUIRE(task2({sequences[2]}) == 5);
  REQUIRE(task2(sequences) == 2);
  // clang-format on
  // auto r = input | v::transform(CardBidPair::fromStr);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day9/input.txt");
  auto sequences = parseSequences(input);
  fmt::println("Day9 Task1 result: {}\n", task1(sequences));
  fmt::println("Day9 Task2 result: {}\n", task2(sequences));
}
