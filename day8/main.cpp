#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <ios>
#include <iterator>
#include <map>
#include <numeric>
#include <ranges>
#include <scn/scan.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct CamelNavigationSystem
{
  std::string leftRightOps;

  std::map<std::array<char, 3>, std::array<std::array<char, 3>, 2>> connections;

  static CamelNavigationSystem fromStr(const std::vector<std::string>& input)
  {
    CamelNavigationSystem navi;
    navi.leftRightOps = input[0];

    for (auto& line : input | v::drop(2)) {
      std::array<char, 3> node{};
      std::copy(line.begin(), line.begin() + 3, node.begin());
      auto leftNode = line.substr(7, 3);
      auto rightNode = line.substr(12, 3);
      std::copy(leftNode.begin(), leftNode.end(), navi.connections[node][0].begin());
      std::copy(rightNode.begin(), rightNode.end(), navi.connections[node][1].begin());
    }

    return navi;
  }
};

int64_t task1(const CamelNavigationSystem& navi)
{
  std::array<char, 3> cursor = {'A', 'A', 'A'};
  std::array<char, 3> target = {'Z', 'Z', 'Z'};

  std::size_t opIndex{};

  int64_t stepCount{};

  while (cursor != target) {
    auto connection = navi.connections.find(cursor);
    if (connection == navi.connections.end()) {
      throw std::runtime_error("Wrong turn?");
    }

    std::size_t connectionIndex = navi.leftRightOps[opIndex++] == 'L' ? 0 : 1;
    if (opIndex == navi.leftRightOps.size())
      opIndex = 0;

    cursor = connection->second.at(connectionIndex);

    stepCount++;
  }

  return stepCount;
}

int64_t task2(const CamelNavigationSystem& navi)
{
  std::size_t opIndex{};

  std::vector<std::array<char, 3>> cursors;
  for (const auto& [label, _] : navi.connections) {
    if (r::contains(label, 'A')) {
      cursors.push_back(label);
    }
  }

  int64_t stepCountLCM = 1;
  for (auto& cursor : cursors) {
    int64_t stepCount{};
    while (!r::contains(cursor, 'Z')) {
      // Same as Task1 (copy)
      auto connection = navi.connections.find(cursor);
      if (connection == navi.connections.end()) {
        throw std::runtime_error("Wrong turn?");
      }

      std::size_t connectionIndex = navi.leftRightOps[opIndex++] == 'L' ? 0 : 1;
      if (opIndex == navi.leftRightOps.size())
        opIndex = 0;

      cursor = connection->second.at(connectionIndex);

      stepCount++;
    }
    stepCountLCM = std::lcm(stepCountLCM, stepCount);
  }

  return stepCountLCM;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "LLR",
    "",
    "AAA = (BBB, BBB)",
    "BBB = (AAA, ZZZ)",
    "ZZZ = (ZZZ, ZZZ)",
  };
  // clang-format on
  // auto r = input | v::transform(CardBidPair::fromStr);
  auto navi = CamelNavigationSystem::fromStr(input);

  REQUIRE(task1(navi) == 6);

  // clang-format off
  input = {
      "RL",
      "",
      "AAA = (BBB, CCC)",
      "BBB = (DDD, EEE)",
      "CCC = (ZZZ, GGG)",
      "DDD = (DDD, DDD)",
      "EEE = (EEE, EEE)",
      "GGG = (GGG, GGG)",
      "ZZZ = (ZZZ, ZZZ)",
  };
  // clang-format on

  navi = CamelNavigationSystem::fromStr(input);

  REQUIRE(task1(navi) == 2);

  input = {
      "LR",
      "",
      "11A = (11B, XXX)",
      "11B = (XXX, 11Z)",
      "11Z = (11B, XXX)",
      "22A = (22B, XXX)",
      "22B = (22C, 22C)",
      "22C = (22Z, 22Z)",
      "22Z = (22B, 22B)",
      "XXX = (XXX, XXX)",
  };
  navi = CamelNavigationSystem::fromStr(input);

  REQUIRE(task2(navi) == 6);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day8/input.txt");
  auto navi = CamelNavigationSystem::fromStr(input);
  fmt::println("Day8 Task1 result: {}\n", task1(navi));
  fmt::println("Day7 Task2 result: {}\n", task2(navi));
}
