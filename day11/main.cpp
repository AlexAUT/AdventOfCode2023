#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstdlib>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iterator>
#include <numeric>
#include <ranges>
#include <scn/scan.h>
#include <set>
#include <string>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct Position
{
  int x;
  int y;
  bool operator<=>(const Position&) const = default;

  Position operator-(const Position& p) const
  {
    return Position(x - p.x, y - p.y);
  }
  Position operator+(const Position& p) const
  {
    return Position(x + p.x, y + p.y);
  }
};

// std::vector<std::string> expand(const std::vector<std::string>& map)
// {
//   std::vector<std::string> expanded;
//   std::vector<bool> colContainsGalaxy(map[0].size(), false);
//   expanded.reserve(map.size());
//   // Expand rows
//   for (const auto& row : map) {
//     if (r::find(row, '#') == row.end()) {
//       expanded.push_back(row);
//     }
//     expanded.push_back(row);
//
//     for (std::size_t j = 0; j < row.size(); j++) {
//       if (row[j] == '#') {
//         colContainsGalaxy[j] = true;
//       }
//     }
//   }
//   // Cols
//   for (auto& row : expanded) {
//     for (std::size_t i = colContainsGalaxy.size() - 1; i < colContainsGalaxy.size(); i--) {
//       if (!colContainsGalaxy[i]) {
//         row.insert(std::next(row.begin(), static_cast<std::string::difference_type>(i)), '.');
//       }
//     }
//   }
//
//   return expanded;
// }

using ExpandedColsAndRows = std::array<std::set<std::size_t>, 2>;
ExpandedColsAndRows expandedColsAndRows(const std::vector<std::string>& map)
{
  ExpandedColsAndRows result;

  std::vector<std::string> expanded;
  std::vector<bool> colContainsGalaxy(map[0].size(), false);
  expanded.reserve(map.size());
  // Expand rows
  for (std::size_t y = 0; y < map.size(); y++) {
    if (r::find(map[y], '#') == map[y].end()) {
      result[0].insert(y);
    }

    for (std::size_t x = 0; x < map[y].size(); x++) {
      if (map[y][x] == '#') {
        colContainsGalaxy[x] = true;
      }
    }
  }
  // Cols
  for (std::size_t x = 0; x < colContainsGalaxy.size(); x++) {
    if (!colContainsGalaxy[x]) {
      result[1].insert(x);
    }
  }

  return result;
}

std::vector<Position> getGalaxyLocations(const std::vector<std::string>& map)
{
  std::vector<Position> galaxies;
  for (std::size_t y = 0; y < map.size(); y++) {
    for (std::size_t x = 0; x < map[y].size(); x++) {
      if (map[y][x] == '#') {
        galaxies.push_back({static_cast<int>(x), static_cast<int>(y)});
      }
    }
  }
  return galaxies;
}

int64_t shortestPath(Position start, Position end, const ExpandedColsAndRows& expanded, int64_t factor)
{
  int64_t x_diff = std::abs(end.x - start.x);
  int64_t y_diff = std::abs(end.y - start.y);
  for (std::size_t y : v::iota(std::min(start.y, end.y), std::max(start.y, end.y) + 1)) {
    if (expanded[0].contains(y)) {
      y_diff += factor;
    }
  }
  for (std::size_t x : v::iota(std::min(start.x, end.x), std::max(start.x, end.x) + 1)) {
    if (expanded[1].contains(x)) {
      x_diff += factor;
    }
  }

  return static_cast<int64_t>(x_diff + y_diff);
}

int64_t task(const std::vector<std::string>& map, int64_t factor)
{
  auto expandedMap = expandedColsAndRows(map);
  auto galaxies = getGalaxyLocations(map);

  int64_t sumShortedPaths{};

  for (std::size_t i = 0; i < galaxies.size(); i++) {
    for (std::size_t j = i + 1; j < galaxies.size(); j++) {
      sumShortedPaths += shortestPath(galaxies[i], galaxies[j], expandedMap, factor);
    }
  }

  return sumShortedPaths;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "...#......",
    ".......#..",
    "#.........",
    "..........",
    "......#...",
    ".#........",
    ".........#",
    "..........",
    ".......#..",
    "#...#.....",
  };

  // REQUIRE(expanded == std::vector<std::string>{
  //   "....#........",
  //   ".........#...",
  //   "#............",
  //   ".............",
  //   ".............",
  //   "........#....",
  //   ".#...........",
  //   "............#",
  //   ".............",
  //   ".............",
  //   ".........#...",
  //   "#....#.......",
  //       });

  REQUIRE(getGalaxyLocations(input).size() == 9);
  // clang-format on
  REQUIRE(task(input, 1) == 374);
  REQUIRE(task(input, 9) == 1030);
  REQUIRE(task(input, 99) == 8410);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day11/input.txt");

  fmt::println("Day11 Task1 result: {}\n", task(input, 1));
  fmt::println("Day11 Task2 result: {}\n", task(input, 999999));
}
