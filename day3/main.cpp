#include "common.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <fmt/format.h>
#include <iostream>
#include <map>
#include <numeric>
#include <scn/scan.h>

namespace r = std::ranges;
namespace v = std::ranges::views;

bool isSymbol(char c)
{
  return !std::isdigit(c) && c != '.';
}

bool isLocSymbol(const std::vector<std::string>& grid, std::size_t x, std::size_t y)
{
  if (y < grid.size() && x < grid[y].size()) {
    return isSymbol(grid[y][x]);
  }
  return false;
}

bool isGear(const std::vector<std::string>& grid, std::size_t x, std::size_t y)
{
  if (y < grid.size() && x < grid[y].size()) {
    return grid[y][x] == '*';
  }
  return false;
}

std::vector<int64_t> getPartNumbers(const std::vector<std::string>& grid, int64_t& sumGearRatios)
{
  std::vector<int64_t> partNumbers;
  std::vector<int64_t> gearRatios;
  std::map<std::tuple<int, int>, std::vector<int>> gearPartNumbers;
  for (std::size_t y = 0; y < grid.size(); y++) {
    auto line = grid[y];
    bool inDigit = false;
    bool isPartNumber = false;
    std::string number;
    std::vector<std::pair<int, int>> gearNeighbors;

    auto markGears = [&](std::size_t x, std::size_t y) {
      if (isGear(grid, x, y)) {
        gearNeighbors.emplace_back(x, y);
      }
    };

    for (std::size_t x = 0; x < line.size(); x++) {
      if (std::isdigit(line[x])) {
        // For the first check left and diagnoals as well
        if (!inDigit) {
          number.clear();
          gearNeighbors.clear();
          inDigit = true;
          isPartNumber = false;

          if (isLocSymbol(grid, x - 1, y) || isLocSymbol(grid, x - 1, y - 1) || isLocSymbol(grid, x - 1, y + 1)) {
            isPartNumber = true;
          }
          markGears(x - 1, y);
          markGears(x - 1, y - 1);
          markGears(x - 1, y + 1);
        }
        // Check up and down
        if (!isPartNumber && (isLocSymbol(grid, x, y + 1) || isLocSymbol(grid, x, y - 1))) {
          isPartNumber = true;
        }
        markGears(x, y - 1);
        markGears(x, y + 1);
        number.append(1, line[x]);
      } else if (inDigit) {
        // End so we have to check diagonals
        inDigit = false;
        if (isLocSymbol(grid, x, y) || isLocSymbol(grid, x, y - 1) || isLocSymbol(grid, x, y + 1)) {
          isPartNumber = true;
        }
        markGears(x, y);
        markGears(x, y - 1);
        markGears(x, y + 1);

        if (isPartNumber) {
          partNumbers.push_back(std::stoi(number));
          for (auto& gPos : gearNeighbors) {
            gearPartNumbers[gPos].emplace_back(partNumbers.back());
          }
        }
      }
      if (x == (line.size() - 1) && inDigit) {
        inDigit = false;
        if (isLocSymbol(grid, x + 1, y) || isLocSymbol(grid, x + 1, y - 1) || isLocSymbol(grid, x + 1, y + 1)) {
          isPartNumber = true;
        }
        markGears(x + 1, y);
        markGears(x + 1, y - 1);
        markGears(x + 1, y + 1);

        if (isPartNumber) {
          partNumbers.push_back(std::stoi(number));
          for (auto& gPos : gearNeighbors) {
            gearPartNumbers[gPos].emplace_back(partNumbers.back());
          }
        }
      }
    }
  }

  for (const auto& [pos, partNumbers] : gearPartNumbers) {
    if (partNumbers.size() == 2) {
      sumGearRatios += partNumbers[0] * partNumbers[1];
    }
  }

  return partNumbers;
}

int64_t task1(const std::vector<std::string>& grid)
{
  [[maybe_unused]] int64_t sumGearRatios{};
  auto partNumbers = getPartNumbers(grid, sumGearRatios);
  return std::reduce(partNumbers.begin(), partNumbers.end());
}

int64_t task2(const std::vector<std::string>& grid)
{
  int64_t sumGearRatios{};
  auto partNumbers = getPartNumbers(grid, sumGearRatios);
  return sumGearRatios;
}

TEST_CASE("Task1 Tests")
{
  std::vector<std::string> input = {
      "467..114..", "...*......", "..35..633.", "......#...", "617*......",
      ".....+.58.", "..592.....", "......755.", "...$.*....", ".664.598..",
  };

  int64_t sumGearRatios{};
  std::vector<int64_t> partNumbers = getPartNumbers(input, sumGearRatios);
  REQUIRE(partNumbers == std::vector<int64_t>{467, 35, 633, 617, 592, 755, 664, 598});
  REQUIRE(task1(input) == 4361);
  REQUIRE(sumGearRatios == 467835);
}

TEST_CASE("Task1")
{
  auto input = readLines("../../day3/input.txt");
  std::cout << std::format("Day2 Task1 result: {}\n", task1(input));
}

TEST_CASE("Task2")
{
  auto input = readLines("../../day3/input.txt");
  std::cout << std::format("Day2 Task2 result: {}\n", task2(input));
}
