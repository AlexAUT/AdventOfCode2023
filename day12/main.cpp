#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <map>
#include <numeric>
#include <ranges>
#include <scn/scan.h>
#include <string>
#include <string_view>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

constexpr char OPERATIONAL = '.';
constexpr char DAMAGED = '#';
// constexpr char UNKNOWN = '?';

const auto compareSpansForCache = [](std::span<int> s1, std::span<int> s2) {
  if (s1.size() < s2.size())
    return true;
  if (s2.size() < s1.size())
    return false;

  for (std::size_t i = 0; i < s1.size(); i++) {
    if (s1[i] < s2[i])
      return true;
  }
  return false;
};

using Cache =
    std::map<std::string_view,
             std::map<std::span<std::size_t>, std::size_t, decltype([](const std::span<std::size_t>& s1, const std::span<std::size_t>& s2) {
                        if (s1.size() < s2.size())
                          return true;
                        if (s2.size() < s1.size())
                          return false;

                        for (std::size_t i = 0; i < s1.size(); i++) {
                          if (s1[i] < s2[i])
                            return true;
                        }
                        return false;
                      })>>;

std::size_t getArrangementCount(Cache& cache, const std::string_view& map, const std::span<std::size_t> ranges)
{
  // No idea why std::span<int> does not work as a Key of a std::map
  if (auto foundMap = cache.find(map); foundMap != cache.end()) {
    if (auto found = foundMap->second.find(ranges); found != foundMap->second.end()) {
      return found->second;
    }
  }

  if (ranges.size() == 0) {
    return map.contains(DAMAGED) ? 0 : 1;
  }

  std::size_t size = ranges[0];
  std::size_t total = 0;

  for (std::size_t i = 0; i < map.size(); i++) {
    if (i + size <= map.size() && !r::contains(map | v::drop(i) | v::take(size), OPERATIONAL) &&
        (i == 0 || map[i - 1] != DAMAGED) && (i + size == map.size() || map[i + size] != DAMAGED)) {
      total += getArrangementCount(cache, i + size + 1 >= map.size() ? std::string_view{} : map.substr(i + size + 1),
                                   ranges.size() == 1 ? std::span<std::size_t>{} : ranges.subspan(1));
    }

    if (map[i] == DAMAGED) {
      break;
    }
  }

  cache[map][ranges] = total;

  return total;
}

std::size_t getArrangementCount(const std::string& row, int repeat = 0)
{
  auto split = row | v::split(' ') | v::transform([](auto&& rng) { return std::string(rng.begin(), rng.end()); });
  std::string map = *split.begin();
  std::string rangesStr = *(split | v::drop(1)).begin();
  std::vector<std::size_t> ranges =
      toVector(rangesStr | v::split(',') | v::transform([](auto&& rng) {
                 return static_cast<std::size_t>(std::stoi(std::string(rng.begin(), rng.end())));
               }));

  if (repeat > 0) {
    // inefficient but doesnt matter
    std::string origMap = map;
    auto oldSize = ranges.size();
    ranges.resize(oldSize * (1 + repeat));
    for (int i = 0; i < repeat; i++) {
      map += '?';
      map += origMap;
      std::copy(ranges.begin(), ranges.begin() + static_cast<std::vector<int>::difference_type>(oldSize),
                ranges.begin() + static_cast<std::vector<int>::difference_type>(oldSize * (i + 1)));
    }
  }
  Cache cache;
  return getArrangementCount(cache, map, ranges);
}

int64_t task1(const std::vector<std::string>& lines)
{
  return std::accumulate(lines.begin(), lines.end(), static_cast<int64_t>(0),
                         [](auto previous, const auto& row) { return previous + getArrangementCount(row); });
}

int64_t task2(const std::vector<std::string>& lines)
{
  return std::accumulate(lines.begin(), lines.end(), static_cast<int64_t>(0),
                         [](auto previous, const auto& row) { return previous + getArrangementCount(row, 4); });
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "???.### 1,1,3",
    ".??..??...?##. 1,1,3",
    "?#?#?#?#?#?#?#? 1,3,1,6",
    "????.#...#... 4,1,1",
    "????.######..#####. 1,6,5",
    "?###???????? 3,2,1",
  };

  REQUIRE(getArrangementCount(input[0]) == 1);
  REQUIRE(getArrangementCount(input[1]) == 4);
  REQUIRE(getArrangementCount(input[2]) == 1);
  REQUIRE(getArrangementCount(input[3]) == 1);
  REQUIRE(getArrangementCount(input[4]) == 4);
  REQUIRE(getArrangementCount(input[5]) == 10);
  //
  REQUIRE(getArrangementCount(input[0], 4) == 1);
  REQUIRE(getArrangementCount(input[1], 4) == 16384);
  REQUIRE(getArrangementCount(input[2], 4) == 1);
  REQUIRE(getArrangementCount(input[3], 4) == 16);
  REQUIRE(getArrangementCount(input[4], 4) == 2500);
  REQUIRE(getArrangementCount(input[5], 4) == 506250);
  
  REQUIRE(task1(input) == 21);
  REQUIRE(task2(input) == 525152);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day12/input.txt");
  //
  fmt::println("Day11 Task1 result: {}\n", task1(input));
  fmt::println("Day11 Task2 result: {}\n", task2(input));
}
