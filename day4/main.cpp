#include "common.hpp"

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <iostream>
#include <numeric>
#include <ranges>
#include <scn/scan.h>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_set>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct Card
{
  int id{-1};
  std::unordered_set<int> winning;
  std::set<int> present;

  static Card fromStr(std::string_view v);

  bool operator==(const Card&) const = default;

  int64_t calculatePoints() const
  {
    int matches{};
    for (auto p : present) {
      if (winning.contains(p)) {
        matches++;
      }
    }
    if (matches == 0) {
      return 0;
    }
    return static_cast<int64_t>(1) << (matches - 1);
  }

  int64_t matches() const
  {
    int matches{};
    for (auto p : present) {
      if (winning.contains(p)) {
        matches++;
      }
    }
    if (matches == 0) {
      return 0;
    }
    return matches;
  }
};

Card Card::fromStr(std::string_view v)
{
  Card card;

  scn::ranges::subrange input(v);
  auto result = scn::scan<int>(v, "Card {}: ");
  if (result) {
    card.id = result->value();
    input = result->range();
  } else {
    throw std::runtime_error("Error parsing card id!");
  }
  scn::ranges::subrange range(v);

  while (auto result = scn::scan_value<int>(input)) {
    card.winning.insert(result->value());
    input = result->range();
  }

  if (auto result = scn::scan<std::string_view>(input, "{}"); result && result->value() == std::string_view{"|"}) {
    input = result->range();
  } else {
    throw std::runtime_error("Wrong separator?");
  }

  while (auto result = scn::scan_value<int>(input)) {
    card.present.insert(result->value());
    input = result->range();
  }

  return card;
}

std::vector<int> getCardCounts(const std::vector<Card>& cards)
{
  std::vector<int> cardCounts(cards.size(), 1);

  for (std::size_t i = 0; i < cards.size(); i++) {
    auto matches = cards[i].matches();
    auto incValue = cardCounts[i];
    for (std::size_t j = i + 1; j < std::min<std::size_t>(i + matches + 1, cards.size()); j++) {
      cardCounts[j] += incValue;
    }
  }

  return cardCounts;
}

TEST_CASE("Task1 and 2 example input")
{
  // clang-format off
  std::vector<std::string> input = {
    "Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53",
    "Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19",
    "Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1",
    "Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83",
    "Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36",
    "Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11",
  };
  // clang-format off

  auto v = input | v::transform(Card::fromStr);
  std::vector<Card> cards(v.begin(), v.end());

  REQUIRE(cards[0] == Card{.id = 1, .winning={41,48,83,86,17}, .present = {83,86,6,31,17,9,48,53}});
  REQUIRE(cards[5] == Card{.id = 6, .winning={31,18,13,56,72}, .present = {74,77,10,23,35,67,36,11}});

  REQUIRE(cards[0].calculatePoints() == 8);
  REQUIRE(cards[1].calculatePoints() == 2);
  REQUIRE(cards[2].calculatePoints() == 2);
  REQUIRE(cards[3].calculatePoints() == 1);
  REQUIRE(cards[4].calculatePoints() == 0);
  REQUIRE(cards[5].calculatePoints() == 0);

  REQUIRE(cards[0].matches() == 4);
  REQUIRE(cards[5].matches() == 0);

  auto cardCounts = getCardCounts(cards);
  REQUIRE(std::reduce(cardCounts.begin(), cardCounts.end()) == 30);
}

TEST_CASE("Task1")
{
  auto input = readLines("../../day4/input.txt");
  auto v = input | v::transform(Card::fromStr);
  std::vector<Card> cards(v.begin(), v.end());
  std::cout << std::format("Day4 Task1 result: {}\n", std::accumulate(v.begin(), v.end(), static_cast<int64_t>(0), [](int64_t p, const Card& card){ return p + card.calculatePoints(); }));
}

TEST_CASE("Task2")
{
  auto input = readLines("../../day4/input.txt");
  auto v = input | v::transform(Card::fromStr);
  std::vector<Card> cards(v.begin(), v.end());
  auto cardCounts = getCardCounts(cards);
  auto sum = std::reduce(cardCounts.begin(), cardCounts.end());
  std::cout << std::format("Day4 Task1 result: {}\n", sum);
}
