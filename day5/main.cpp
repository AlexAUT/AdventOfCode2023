#include "common.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <scn/scan.h>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct MappingRange
{
  int64_t dstStart{-1};
  int64_t srcStart{-1};
  int64_t length{-1};

  bool operator==(const MappingRange&) const = default;
};

using MappingRanges = std::vector<MappingRange>;

struct Range
{
  int64_t start{-1};
  int64_t length{-1};
};

struct Almanac
{
  std::vector<int64_t> seeds;

  MappingRanges seedToSoil;
  MappingRanges soilToFertilizer;
  MappingRanges fertilizerToWater;
  MappingRanges waterToLight;
  MappingRanges lightToTemperature;
  MappingRanges temperatureToHumidity;
  MappingRanges humidityToLocationMap;

  static Almanac fromStr(const std::vector<std::string> v);

  static int64_t getIndexIntoMap(int64_t index, const MappingRanges& mapping);
  static void getRangesIntoMap(Range index, const MappingRanges& mapping, std::vector<Range>& mappedRanges);
};

Almanac Almanac::fromStr(const std::vector<std::string> v)
{
  Almanac almanac;

  // Seeds
  {
    scn::ranges::subrange r = v[0];
    if (auto result = scn::scan<>(r, "seeds: ")) {
      r = result->range();
    } else {
      throw std::runtime_error("Failed to parse seeds!");
    }

    while (auto result = scn::scan<int64_t>(r, "{}")) {
      almanac.seeds.push_back(result->value());
      r = result->range();
    }
  }

  std::size_t cursor = 2;
  auto parseMap = [&](std::string_view name, auto& vec) {
    if (auto result = scn::scan<std::string_view>(v[cursor++], "{} map:"); !result || result->value() != name) {
      if (result) {
        fmt::println("Wrong key: {}", result->value());
      }

      throw std::runtime_error("Wrong header of map?");
    }

    while (auto result = scn::scan<int64_t, int64_t, int64_t>(v[cursor++], "{} {} {}")) {
      auto& vals = result->values();
      vec.emplace_back(MappingRange{get<0>(vals), get<1>(vals), get<2>(vals)});

      if (cursor >= v.size()) {
        break;
      }
    }

    std::sort(vec.begin(), vec.end(),
              [](const MappingRange& a, const MappingRange& b) { return a.srcStart < b.srcStart; });
  };

  parseMap("seed-to-soil", almanac.seedToSoil);
  parseMap("soil-to-fertilizer", almanac.soilToFertilizer);
  parseMap("fertilizer-to-water", almanac.fertilizerToWater);
  parseMap("water-to-light", almanac.waterToLight);
  parseMap("light-to-temperature", almanac.lightToTemperature);
  parseMap("temperature-to-humidity", almanac.temperatureToHumidity);
  parseMap("humidity-to-location", almanac.humidityToLocationMap);

  return almanac;
}

int64_t Almanac::getIndexIntoMap(int64_t index, const MappingRanges& mapping)
{
  for (auto& m : mapping) {
    int64_t localIndex = index - m.srcStart;
    if (localIndex >= 0 && localIndex < m.length) {
      return m.dstStart + localIndex;
    }
  }
  return index;
}

void Almanac::getRangesIntoMap(Range index, const MappingRanges& mapping, std::vector<Range>& mappedRanges)
{
  // This assumes the mappings are sorted by srcStart
  int64_t cursor = index.start;
  int64_t end = index.start + index.length;

  // Find closest range
  for (const auto& mapping : mapping) {
    if (cursor >= end) {
      break;
    }
    int64_t localIndex = cursor - mapping.srcStart;

    if (localIndex < 0) {
      // We lie between two ranges => map 1:1 until range start
      auto distance = std::min(std::abs(localIndex), end - cursor);
      mappedRanges.push_back(Range{.start = cursor, .length = distance});
      cursor += distance;
      localIndex = cursor - mapping.srcStart;
      if (cursor >= end) {
        break;
      }
    }

    if (localIndex >= 0 && localIndex < mapping.length) {
      // Inside range
      int64_t distanceToAdd = std::min(mapping.length - localIndex, end - cursor);
      mappedRanges.push_back(Range{.start = mapping.dstStart + localIndex, .length = distanceToAdd});
      cursor += distanceToAdd;
    }
  }
  if (cursor < end) {
    // No mapping afterwards so copy rest of range
    auto distance = end - cursor;
    mappedRanges.push_back(Range{.start = cursor, .length = distance});
    cursor += distance;
  }
}

int64_t task1(const Almanac& a)
{
  int64_t lowestLocation = std::numeric_limits<int64_t>::max();

  for (auto index : a.seeds) {
    index = Almanac::getIndexIntoMap(index, a.seedToSoil);
    index = Almanac::getIndexIntoMap(index, a.soilToFertilizer);
    index = Almanac::getIndexIntoMap(index, a.fertilizerToWater);
    index = Almanac::getIndexIntoMap(index, a.waterToLight);
    index = Almanac::getIndexIntoMap(index, a.lightToTemperature);
    index = Almanac::getIndexIntoMap(index, a.temperatureToHumidity);
    index = Almanac::getIndexIntoMap(index, a.humidityToLocationMap);
    lowestLocation = std::min(lowestLocation, index);
  }

  return lowestLocation;
}

int64_t task2(const Almanac& a)
{
  if (a.seeds.size() % 2 != 0)
    throw std::runtime_error("Only even seed count allowed!");

  std::vector<Range> ranges;
  for (std::size_t i = 0; i < a.seeds.size(); i += 2) {
    Range rng = {.start = a.seeds[i], .length = a.seeds[i + 1]};
    ranges.push_back(rng);
  }
  std::vector<Range> newRanges;

  auto handleMapping = [&](const auto& mappingTable) {
    for (auto& r : ranges) {
      Almanac::getRangesIntoMap(r, mappingTable, newRanges);
    }
    std::swap(newRanges, ranges);
    newRanges.clear();
  };

  handleMapping(a.seedToSoil);
  handleMapping(a.soilToFertilizer);
  handleMapping(a.fertilizerToWater);
  handleMapping(a.waterToLight);
  handleMapping(a.lightToTemperature);
  handleMapping(a.temperatureToHumidity);
  handleMapping(a.humidityToLocationMap);

  int64_t lowestLocation = std::numeric_limits<int64_t>::max();
  for (auto& r : ranges) {
    lowestLocation = std::min(lowestLocation, r.start);
  }
  return lowestLocation;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "seeds: 79 14 55 13",
    "", "seed-to-soil map:",
    "50 98 2",
    "52 50 48",
    "",
    "soil-to-fertilizer map:",
    "0 15 37",
    "37 52 2",
    "39 0 15",
    "",
    "fertilizer-to-water map:",
    "49 53 8",
    "0 11 42",
    "42 0 7",
    "57 7 4",
    "",
    "water-to-light map:",
    "88 18 7",
    "18 25 70",
    "",
    "light-to-temperature map:",
    "45 77 23",
    "81 45 19",
    "68 64 13",
    "",
    "temperature-to-humidity map:",
    "0 69 1",
    "1 0 69",
    "",
    "humidity-to-location map:",
    "60 56 37",
    "56 93 4",
  };
  // clang-format off

  auto almanac = Almanac::fromStr(input);

  REQUIRE(almanac.seeds == std::vector<int64_t>{79, 14, 55, 13});
  REQUIRE(almanac.humidityToLocationMap == std::vector{MappingRange{60, 56, 37}, MappingRange{56, 93, 4}});

  REQUIRE(task1(almanac) == 35);
  REQUIRE(task2(almanac) == 46);
}

TEST_CASE("Tasks")
{
  auto input = Almanac::fromStr(readLines("../../day5/input.txt"));
  std::cout << std::format("Day5 Task1 result: {}\n", task1(input));
  std::cout << std::format("Day5 Task2 result: {}\n", task2(input));
}

