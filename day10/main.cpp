#include "common.hpp"

#include <catch2/catch_test_macros.hpp>
#include <fmt/core.h>
#include <fmt/format.h>
#include <optional>
#include <queue>
#include <ranges>
#include <scn/scan.h>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct Position
{
  int x;
  int y;
  bool operator<=>(const Position&) const = default;

  Position operator+(const Position& p) const
  {
    return Position(x + p.x, y + p.y);
  }
};

struct Pipe
{
  Position e1{};
  Position e2{};
  bool startPosition{false};
};

std::vector<std::vector<Pipe>> parseField(const std::vector<std::string>& input)
{
  return toVector(input | v::transform([](const std::string& line) {
                    return toVector(line | v::transform([](char c) {
                                      switch (c) {
                                      case '.':
                                        return Pipe{};
                                      case '|':
                                        return Pipe{.e1{0, -1}, .e2{0, 1}};
                                      case '-':
                                        return Pipe{.e1{1, 0}, .e2{-1, 0}};
                                      case 'L':
                                        return Pipe{.e1{0, -1}, .e2{1, 0}};
                                      case 'J':
                                        return Pipe{.e1{0, -1}, .e2{-1, 0}};
                                      case '7':
                                        return Pipe{.e1{0, 1}, .e2{-1, 0}};
                                      case 'F':
                                        return Pipe{.e1{0, 1}, .e2{1, 0}};
                                      case 'S':
                                        return Pipe{.e1{}, .e2{}, .startPosition = true};
                                      default:
                                        throw std::runtime_error("Invalid pipe character!");
                                      }
                                    }));
                  }));
}

int64_t task1(const std::vector<std::vector<Pipe>> pipes)
{
  Position startPosition{-1, -1};
  for (int x = 0; x < static_cast<int>(pipes.size()); x++) {
    for (int y = 0; y < static_cast<int>(pipes.size()); y++) {
      if (pipes[y][x].startPosition) {
        startPosition = {x, y};
      }
    }
  }

  if (startPosition == Position{-1, -1}) {
    throw std::runtime_error("Could not find start position?");
  }

  // Assumes all rows same length
  std::vector<std::vector<int>> distances =
      toVector(pipes | v::transform([](const auto& row) { return std::vector<int>(row.size(), -1); }));

  distances[startPosition.y][startPosition.x] = 0;

  std::queue<Position> searchStack;
  searchStack.push(startPosition);

  auto printField = [&]() {
    // fmt ::println("---------------");
    // for (auto& row : distances) {
    //   fmt::println("{}", fmt::join(row, " "));
    // }
    // fmt ::println("---------------");
  };

  while (!searchStack.empty()) {
    Position c = searchStack.front();
    searchStack.pop();
    int newDistance = distances[c.y][c.x] + 1;

    auto markPosition = [&pipes, &searchStack, &distances, printField, newDistance](Position p, Position offset) {
      Position target = p + offset;
      // Check if we move to antoher pipe and the pipe is not visited
      if (target == p) {
        return false;
      }
      // Check if target is valid
      if (target.y < 0 || target.y >= static_cast<int>(pipes.size()) || target.x < 0 ||
          target.x >= static_cast<int>(pipes[target.y].size())) {
        return false;
      }

      Pipe newPipe = pipes[target.y][target.x];
      // Check if connected
      if (target + newPipe.e1 != p && target + newPipe.e2 != p) {
        return false;
      }

      if (distances[target.y][target.x] != -1) {
        // Loop?
        if (distances[target.y][target.x] == newDistance) {
          printField();
          return true;
        }
        return false;
      }

      // Check if pipe has connection
      searchStack.push(target);
      distances[target.y][target.x] = newDistance;
      return false;
    };

    if (c == startPosition) {
      if (markPosition(c, {1, 0})) {
        return newDistance;
      }
      if (markPosition(c, {-1, 0})) {
        return newDistance;
      }
      if (markPosition(c, {0, 1})) {
        return newDistance;
      }
      if (markPosition(c, {0, -1})) {
        return newDistance;
      }
    } else {
      auto p = pipes[c.y][c.x];
      if (p.e1 != Position{0, 0}) {
        if (markPosition(c, p.e1)) {
          return newDistance;
        }
        if (markPosition(c, p.e2)) {
          return newDistance;
        }
      }
    }
    printField();
  }

  return 0;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    ".....",
    ".S-7.",
    ".|.|.",
    ".L-J.",
    ".....",
  };
  // clang-format on
  REQUIRE(task1(parseField(input)) == 4);

  // clang-format off
  input = {
    "-L|F7",
    "7S-7|",
    "L|7||",
    "-L-J|",
    "L|-JF",
  };
  // clang-format on
  REQUIRE(task1(parseField(input)) == 4);

  // clang-format off
  input = {
    "..F7.",
    ".FJ|.",
    "SJ.L7",
    "|F--J",
    "LJ...",
  };
  // clang-format on
  REQUIRE(task1(parseField(input)) == 8);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day10/input.txt");
  auto field = parseField(input);
  fmt::println("Day10 Task1 result: {}\n", task1(field));
  // fmt::println("Day10 Task2 result: {}\n", task2(sequences));
}
