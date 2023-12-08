#include "common.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <charconv>
#include <fmt/core.h>
#include <fmt/format.h>
#include <iterator>
#include <map>
#include <ranges>
#include <scn/scan.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct CardBidPair
{
  std::array<int, 5> cards{};
  std::string_view cardsRaw;
  int64_t bid{-1};
  int strengthClass{};
  int strengthClassTask2{};

  bool operator==(const CardBidPair&) const = default;

  static CardBidPair fromStr(std::string_view v)
  {
    auto r = v | v::split(' ') | v::transform([](auto&& rng) { return std::string_view(rng); });
    CardBidPair result;
    result.cardsRaw = *r.begin();
    std::unordered_map<char, int> cardLUT = {{'A', 12}, {'K', 11}, {'Q', 10}, {'J', 9}, {'T', 8}, {'9', 7}, {'8', 6},
                                             {'7', 5},  {'6', 4},  {'5', 3},  {'4', 2}, {'3', 1}, {'2', 0}};
    auto cardValues = result.cardsRaw | v::transform([&](char c) { return cardLUT[c]; });
    if (cardValues.size() != 5) {
      throw std::runtime_error("Too many cards");
    }
    std::copy(cardValues.begin(), cardValues.end(), result.cards.begin());

    auto r2 = r | v::drop(1) | v::transform([](auto&& rng) {
                int64_t v{-1};
                std::from_chars(rng.begin(), rng.end(), v);
                return v;
              });
    result.bid = *r2.begin();

    // Calculate strength class
    auto getClass = [](const std::array<int, 13>& counts) {
      if (r::contains(counts, 5)) {
        return 6;
      } else if (r::contains(counts, 4)) {
        return 5;
      } else if (r::contains(counts, 3) && r::contains(counts, 2)) {
        return 4;
      } else if (r::contains(counts, 3)) {
        return 3;
      } else if (r::count(counts, 2) == 2) {
        return 2;
      } else if (r::contains(counts, 2)) {
        return 1;
      } else {
        return 0;
      }
    };

    std::array<int, 13> counts{};
    for (auto c : result.cards) {
      counts.at(c)++;
    }
    result.strengthClass = getClass(counts);
    // Task 2
    int jokerCount = r::count(result.cards, cardLUT['J']);
    // We should transform joker into the other most frequent card
    if (jokerCount == 5) {
      result.strengthClassTask2 = result.strengthClass;
    } else {
      // Remove jokers from counts and add them to most frequent type
      counts[cardLUT['J']] = 0;
      (*r::max_element(counts)) += jokerCount;
      result.strengthClassTask2 = getClass(counts);
    }

    return result;
  }
};

bool rankCards(CardBidPair& c1, CardBidPair& c2)
{
  if (c1.strengthClass < c2.strengthClass)
    return true;
  else if (c1.strengthClass == c2.strengthClass) {
    for (int i = 0; i < 5; i++) {
      if (c1.cards.at(i) == c2.cards.at(i)) {
        continue;
      }
      return c1.cards.at(i) < c2.cards.at(i);
    }
    throw std::runtime_error("Same hands?");
  } else {
    return false;
  }
}

bool rankCardsTask2(CardBidPair& c1, CardBidPair& c2)
{
  if (c1.strengthClassTask2 < c2.strengthClassTask2)
    return true;
  else if (c1.strengthClassTask2 == c2.strengthClassTask2) {
    for (int i = 0; i < 5; i++) {
      auto card1 = c1.cards.at(i);
      if (card1 == 9)
        card1 = -1;
      auto card2 = c2.cards.at(i);
      if (card2 == 9)
        card2 = -1;
      if (card1 == card2) {
        continue;
      }
      return card1 < card2;
    }
    throw std::runtime_error("Same hands?");
  } else {
    return false;
  }
}

int64_t task1(const std::vector<CardBidPair> input)
{
  std::vector<CardBidPair> sorted = input;
  std::sort(sorted.begin(), sorted.end(), rankCards);

  int64_t result{};
  for (std::size_t i = 0; i < sorted.size(); i++) {
    result += static_cast<int64_t>(i + 1) * sorted[i].bid;
  }
  return result;
}

int64_t task2(const std::vector<CardBidPair> input)
{
  std::vector<CardBidPair> sorted = input;
  std::sort(sorted.begin(), sorted.end(), rankCardsTask2);

  int64_t result{};
  for (std::size_t i = 0; i < sorted.size(); i++) {
    result += static_cast<int64_t>(i + 1) * sorted[i].bid;
  }
  return result;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "32T3K 765",
    "T55J5 684",
    "KK677 28",
    "KTJJT 220",
    "QQQJA 483",
  };
  // clang-format on
  auto r = input | v::transform(CardBidPair::fromStr);
  std::vector<CardBidPair> drawings(r.begin(), r.end());

  REQUIRE(drawings[0] ==
          CardBidPair{
              .cards = {1, 0, 8, 1, 11}, .cardsRaw = "32T3K", .bid = 765, .strengthClass = 1, .strengthClassTask2 = 1});
  REQUIRE(drawings[1] ==
          CardBidPair{
              .cards = {8, 3, 3, 9, 3}, .cardsRaw = "T55J5", .bid = 684, .strengthClass = 3, .strengthClassTask2 = 5});
  REQUIRE(
      drawings[4] ==
      CardBidPair{
          .cards = {10, 10, 10, 9, 12}, .cardsRaw = "QQQJA", .bid = 483, .strengthClass = 3, .strengthClassTask2 = 5});
  REQUIRE(drawings.size() == 5);

  std::vector<CardBidPair> sorted = drawings;
  std::sort(sorted.begin(), sorted.end(), rankCards);
  REQUIRE(sorted[0] == drawings[0]);
  REQUIRE(sorted[1] == drawings[3]);

  REQUIRE(task1(drawings) == 6440);
  REQUIRE(task2(drawings) == 5905);

  // clang-format on
}

TEST_CASE("Tasks")
{
  auto input = readLines("../../day7/input.txt");
  auto r = input | v::transform(CardBidPair::fromStr);
  std::vector<CardBidPair> drawings(r.begin(), r.end());
  fmt::println("Day7 Task1 result: {}\n", task1(drawings));
  fmt::println("Day7 Task2 result: {}\n", task2(drawings));
}
