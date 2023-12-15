#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <ranges>
#include <scn/scan.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

std::tuple<int, int> fiendReflections(const std::vector<std::string>& field)
{
  // fmt::println("Field:\n {}", fmt::join(field, "\n"));
  if (field.empty())
    throw std::runtime_error("Empty line?");

  std::tuple<int, int> reflectionColAndRow{0, 0};
  // Rows
  for (std::size_t i = 0; i < field.size() - 1; i++)
  {
    auto& row1 = field[i];
    auto& row2 = field[i+1];

    if (row1 == row2)
    {
      if (i != 0)
      {
        auto reflLen = std::min(i + 1, field.size() - i - 1);
        auto side1 = field | v::take(i + 1) | v::reverse | v::take(reflLen);
        auto side2 = field | v::drop(i + 1) | v::take(reflLen);

        if (!r::equal(side1, side2))
        {
          continue;
        }
      }
      std::get<1>(reflectionColAndRow) = static_cast<int>(i + 1);
      return {0, i+1};
      break;
    }
  }
  // Cols
  for (std::size_t i = 0; i < field[0].size() - 1; i++)
  {
    auto col = [&](std::size_t colIndex) { auto colView = field | v::transform([colIndex](const std::string& r) { return r[colIndex]; }); return std::string(colView.begin(), colView.end()); };

    if (r::equal(col(i), col(i+1)))
    {
      if (i != 0)
      {
        auto reflLen = std::min(i + 1, field[0].size() - i - 1);
        auto side1 = v::iota(static_cast<std::size_t>(0), i + 1) | v::transform(col) | v::reverse | v::take(reflLen);
        auto side2 = v::iota(i+1, field[0].size()) | v::transform(col) | v::take(reflLen);

        if (!r::equal(side1, side2))
        {
          continue;
        }
      }
      std::get<0>(reflectionColAndRow) = static_cast<int>(i + 1);
      return {i+1, 0};
      break;
    }
  }

  throw std::runtime_error("No reflection found?");
  return reflectionColAndRow;
}

int64_t task1(std::vector<std::string> input)
{
  auto fields = splitOnEmptyRows(input);

  int64_t result{};

  for (auto& field : fields)
  {
    auto [x, y] = fiendReflections(field);
    result += x + (100 * y);
  }
  

  return result;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "#.##..##.",
    "..#.##.#.",
    "##......#",
    "##......#",
    "..#.##.#.",
    "..##..##.",
    "#.#.##.#.",
    "",
    "#...##..#",
    "#....#..#",
    "..##..###",
    "#####.##.",
    "#####.##.",
    "..##..###",
    "#....#..#",
  };

  REQUIRE(task1(input) == 405);




}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day13/input.txt");
  //
  fmt::println("Day13 Task1 result: {}\n", task1(input));
  // fmt::println("Day11 Task2 result: {}\n", task2(input));
}
