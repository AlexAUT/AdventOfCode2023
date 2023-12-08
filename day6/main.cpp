#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <charconv>
#include <fmt/core.h>
#include <fmt/format.h>
#include <ranges>
#include <scn/scan.h>
#include <string>
#include <string_view>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

std::vector<int64_t> strToList(std::string v)
{
  auto r = v | v::split(' ') | v::drop(1) | v::filter([](auto&& rng) { return !rng.empty(); }) |
           v::transform([](auto&& rng) {
             std::string_view v(rng);
             int64_t i{};
             std::from_chars(v.begin(), v.end(), i);
             return i;
           });
  std::vector<int64_t> list(r.begin(), r.end());

  return list;
}

int64_t parseTask2(std::string v)
{
  auto r = v | v::filter([](char c) { return std::isdigit(c); });
  std::string numbers(r.begin(), r.end());

  return std::stoll(numbers);
}

int64_t numbersToBeatRecord(int64_t duration, int64_t distance)
{
  int64_t count{};
  for (int64_t time = 1; time < duration; time++) {
    int64_t speed = time;
    int64_t timeToDrive = duration - time;
    if (speed * timeToDrive > distance)
      count++;
  }

  return count;
}

int64_t task1(const std::vector<int64_t>& durations, const std::vector<int64_t>& distances)
{
  int64_t result = 0;
  for (std::size_t i = 0; i < durations.size(); i++) {
    int64_t count = numbersToBeatRecord(durations[i], distances[i]);
    result = result == 0 ? count : result * count;
  }
  return result;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "Time:      7  15   30",
    "Distance:  9  40  200",
  };
  // clang-format on

  std::vector<int64_t> times = strToList(input[0]);
  std::vector<int64_t> distances = strToList(input[1]);

  REQUIRE(times == std::vector<int64_t>{7, 15, 30});
  REQUIRE(distances == std::vector<int64_t>{9, 40, 200});

  REQUIRE(numbersToBeatRecord(times[0], distances[0]) == 4);
  REQUIRE(numbersToBeatRecord(times[1], distances[1]) == 8);
  REQUIRE(numbersToBeatRecord(times[2], distances[2]) == 9);
  REQUIRE(task1(times, distances) == 288);

  REQUIRE(parseTask2(input[0]) == 71530);
  REQUIRE(parseTask2(input[1]) == 940200);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day6/input.txt");
  {
    std::vector<int64_t> times = strToList(input[0]);
    std::vector<int64_t> distances = strToList(input[1]);

    fmt::println("Day5 Task1 result: {}\n", task1(times, distances));
  }
  {
    int64_t times = parseTask2(input[0]);
    int64_t distances = parseTask2(input[1]);

    fmt::println("Day5 Task2 result: {}\n", task1({times}, {distances}));
  }
}
