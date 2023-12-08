#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fmt/format.h>
#include <format>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <string_view>

namespace r = std::ranges;
namespace v = std::ranges::views;

int64_t extractNumber(std::string_view line, bool allowWritteDigits = false)
{
  std::size_t first_number_pos = line.size();
  std::size_t last_number_pos = 0;

  int64_t first_number = -1;
  int64_t last_number = -1;

  std::string digits = "0123456789";
  first_number_pos = std::min(first_number_pos, line.find_first_of(digits));
  if (first_number_pos != line.size()) {
    last_number_pos = std::max(last_number_pos, line.find_last_of(digits));
  }

  if (first_number_pos < line.size()) {
    first_number = line[first_number_pos] - '0';
    last_number = line[last_number_pos] - '0';
  }

  if (allowWritteDigits) {
    std::array digits = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    for (std::size_t i = 0; i < digits.size(); i++) {
      auto first_pos = line.find(digits[i]);
      auto last_pos = line.rfind(digits[i]);

      if (first_pos != std::string::npos && first_pos < first_number_pos) {
        first_number_pos = first_pos;
        first_number = static_cast<int64_t>(i);
      }
      if (last_pos != std::string::npos && last_pos > last_number_pos) {
        last_number_pos = last_pos;
        last_number = static_cast<int64_t>(i);
      }
    }
  }

  if (first_number_pos >= line.size()) {
    throw std::runtime_error("Invalid input no digit found!");
  }

  return (10 * first_number) + last_number;
}

TEST_CASE("Inputs Task1")
{
  REQUIRE(extractNumber("1abc2") == 12);
  REQUIRE(extractNumber("pqr3stu8vwx") == 38);
  REQUIRE(extractNumber("a1b2c3d4e5f") == 15);
  REQUIRE(extractNumber("treb7uchet") == 77);
}

TEST_CASE("Inputs Task2")
{
  REQUIRE(extractNumber("two1nine", true) == 29);
  REQUIRE(extractNumber("eightwothree", true) == 83);
  REQUIRE(extractNumber("abcone2threexyz", true) == 13);
  REQUIRE(extractNumber("xtwone3four", true) == 24);
  REQUIRE(extractNumber("4nineeightseven2", true) == 42);
  REQUIRE(extractNumber("zoneight234", true) == 14);
  REQUIRE(extractNumber("7pqrstsixteen", true) == 76);
}

TEST_CASE("Task1")
{
  auto inputLines = readLines("../../day1/input.txt");
  REQUIRE(inputLines.size() == 1000);
  REQUIRE(inputLines.front() == "cmpptgjc3qhcjxcbcqgqkxhrms");
  REQUIRE(inputLines.back() == "sixeightfive3sdtwo");

  int64_t result = std::accumulate(inputLines.begin(), inputLines.end(), static_cast<int64_t>(0),
                                   [](int64_t sum, const std::string& line) { return sum + extractNumber(line); });

  std::cout << std::format("Day1 Task1 result: {}\n", result);
}

TEST_CASE("Task2")
{
  auto inputLines = readLines("../../day1/input.txt");
  REQUIRE(inputLines.size() == 1000);
  REQUIRE(inputLines.front() == "cmpptgjc3qhcjxcbcqgqkxhrms");
  REQUIRE(inputLines.back() == "sixeightfive3sdtwo");

  int64_t result =
      std::accumulate(inputLines.begin(), inputLines.end(), static_cast<int64_t>(0),
                      [](int64_t sum, const std::string& line) { return sum + extractNumber(line, true); });

  std::cout << std::format("Day1 Task2 result: {}\n", result);
}
