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
  if (field.empty())
    throw std::runtime_error("Empty line?");

  std::tuple<int, int> reflectionColAndRow{0, 0};
  // Rows
  for (std::size_t i = 1; i < field.size(); i++) {
    auto leftIndices = v::iota(static_cast<std::size_t>(0), i) | v::reverse;
    auto rightIndices = v::iota(i, field.size());

    auto minLen = std::min(leftIndices.size(), rightIndices.size());

    auto left = leftIndices | v::take(minLen) | v::transform([&](std::size_t row) { return field[row]; });
    auto right = rightIndices | v::take(minLen) | v::transform([&](std::size_t row) { return field[row]; });

    if (!r::equal(left, right)) {
      continue;
    }
    std::get<1>(reflectionColAndRow) = static_cast<int>(i);
    break;
  }

  // Cols
  for (std::size_t i = 1; i < field[0].size(); i++) {
    auto col = [&](std::size_t colIndex) {
      auto colView = field | v::transform([colIndex](const std::string& r) { return r[colIndex]; });
      return std::string(colView.begin(), colView.end());
    };

    auto reflLen = std::min(i, field[0].size() - i);
    auto side1 = v::iota(static_cast<std::size_t>(0), i) | v::transform(col) | v::reverse | v::take(reflLen);
    auto side2 = v::iota(i, field[0].size()) | v::transform(col) | v::take(reflLen);

    if (!r::equal(side1, side2)) {
      continue;
    }
    std::get<0>(reflectionColAndRow) = static_cast<int>(i);
    break;
  }

  if (reflectionColAndRow == std::tuple(0, 0))
    throw std::runtime_error("No reflection found?");
  return reflectionColAndRow;
}

std::tuple<int, int> fiendReflectionsWithSmudges(const std::vector<std::string>& field)
{
  if (field.empty())
    throw std::runtime_error("Empty line?");

  std::tuple<int, int> reflectionColAndRow{0, 0};
  // Rows
  for (std::size_t i = 1; i < field.size(); i++) {
    auto leftIndices = v::iota(static_cast<std::size_t>(0), i) | v::reverse;
    auto rightIndices = v::iota(i, field.size());

    auto minLen = std::min(leftIndices.size(), rightIndices.size());

    auto left = leftIndices | v::take(minLen) | v::transform([&](std::size_t row) { return field[row]; });
    auto right = rightIndices | v::take(minLen) | v::transform([&](std::size_t row) { return field[row]; });

    std::size_t diffCount{};
    r::for_each(v::zip(left, right), [&diffCount](auto leftRight) {
      auto [left, right] = leftRight;
      for (auto [l, r] : v::zip(left, right)) {
        if (l != r) {
          diffCount++;
        }
      }
    });

    if (diffCount != 1) {
      continue;
    }

    std::get<1>(reflectionColAndRow) = static_cast<int>(i);
    break;
  }

  // Cols
  for (std::size_t i = 1; i < field[0].size(); i++) {
    auto col = [&](std::size_t colIndex) {
      auto colView = field | v::transform([colIndex](const std::string& r) { return r[colIndex]; });
      return std::string(colView.begin(), colView.end());
    };

    auto reflLen = std::min(i, field[0].size() - i);
    auto left = v::iota(static_cast<std::size_t>(0), i) | v::transform(col) | v::reverse | v::take(reflLen);
    auto right = v::iota(i, field[0].size()) | v::transform(col) | v::take(reflLen);

    std::size_t diffCount{};
    r::for_each(v::zip(left, right), [&diffCount](auto leftRight) {
      auto [left, right] = leftRight;
      for (auto [l, r] : v::zip(left, right)) {
        if (l != r) {
          diffCount++;
        }
      }
    });

    if (diffCount != 1) {
      continue;
    }
    std::get<0>(reflectionColAndRow) = static_cast<int>(i);
    break;
  }

  if (reflectionColAndRow == std::tuple(0, 0))
    throw std::runtime_error("No reflection found?");
  return reflectionColAndRow;
}

int64_t task1(std::vector<std::string> input)
{
  auto fields = splitOnEmptyRows(input);

  int64_t result{};

  for (auto& field : fields) {
    auto [x, y] = fiendReflections(field);
    result += x + (100 * y);
  }

  return result;
}

int64_t task2(std::vector<std::string> input)
{
  auto fields = splitOnEmptyRows(input);

  int64_t result{};

  for (auto& field : fields) {
    auto [x, y] = fiendReflectionsWithSmudges(field);
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
  REQUIRE(task2(input) == 400);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day13/input.txt");
  //
  fmt::println("Day13 Task1 result: {}", task1(input));
  fmt::println("Day13 Task2 result: {}", task2(input));
}
