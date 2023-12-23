#include "common.hpp"

#include "scn/external/nanorange/nanorange.hpp"
#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <numeric>
#include <scn/scan.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// int64_t task1(std::vector<std::string> input)
// {
// }

// int64_t task2(std::vector<std::string> input)
// {
// }

int64_t hash(auto&& input)
{
  int64_t hash{};
  for (char c : input)
  {
    if (c == '\0')
      break;
    hash += c;
    hash *= 17;
    hash = hash % 256;
  }
  return hash;
}

int64_t task1(std::string_view input)
{
  auto rng = nano::views::split(input, ',') | nano::views::transform([](auto&& rng) { return hash(rng); });
  return std::reduce(rng.begin(), rng.end());
}

int64_t task2(std::string input)
{
  struct Box
  {
    std::string label;
    int lens{-1};
  };
  std::array<std::vector<Box>, 256> boxes;

  std::stringstream sstr(input);
  std::string op;
  while(std::getline(sstr, op, ','))
  {
    auto delimPos = op.find_first_of("=-");
    bool isInsert = op[delimPos] == '=';

    auto label = op.substr(0, delimPos);
    auto labelHash = hash(label);

    auto& box = boxes.at(labelHash);
    auto found = std::find_if(box.begin(), box.end(), [&label](const Box& box) { return box.label == label; });

    if (isInsert)
    {
      auto lens = std::stoi(op.substr(delimPos + 1));
      if (found == box.end())
        box.push_back({label, lens});
      else
        found->lens = lens;
    }
    else
    {
      if (found != box.end())
      {
        box.erase(found);
      }
    }
  }

  int64_t focalStr{};

  for (std::size_t boxIndex = 0; boxIndex < boxes.size(); boxIndex++)
  {
    auto& box = boxes.at(boxIndex);
    for (std::size_t slotIndex = 0; slotIndex < box.size(); slotIndex++)
    {
      focalStr += static_cast<int64_t>((boxIndex + 1) * (slotIndex + 1) * box[slotIndex].lens);
    }
  }

  return focalStr;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
  };

  REQUIRE(hash("HASH") == 52);
  REQUIRE(task1("rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7") == 1320);
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day15/input.txt");
  //
  fmt::println("Day15 Task1 result: {}", task1(input[0]));
  fmt::println("Day15 Task2 result: {}", task2(input[0]));
}
