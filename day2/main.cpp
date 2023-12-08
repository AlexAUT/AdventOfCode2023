#include "common.hpp"

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>
#include <iostream>
#include <numeric>
#include <scn/scan.h>
#include <string_view>

namespace r = std::ranges;
namespace v = std::ranges::views;

struct Game
{
  int id{-1};
  struct Drawing
  {
    int red{};
    int green{};
    int blue{};

    bool operator==(const Drawing&) const = default;
  };

  std::vector<Drawing> drawings;

  bool operator==(const Game&) const = default;
  static Game fromStr(std::string_view v);
};

using Bag = Game::Drawing;

bool possible(const Game& game, Bag bag)
{
  for (const auto& drawing : game.drawings) {
    if (drawing.red > bag.red || drawing.green > bag.green || drawing.blue > bag.blue)
      return false;
  }
  return true;
}

Bag getMinimumBag(const Game& game)
{
  Bag bag = game.drawings.front();

  for (const auto& d : game.drawings) {
    bag.red = std::max(bag.red, d.red);
    bag.green = std::max(bag.green, d.green);
    bag.blue = std::max(bag.blue, d.blue);
  }

  return bag;
}

int64_t task1(const std::vector<Game>& games)
{
  Bag bag = {.red = 12, .green = 13, .blue = 14};
  return std::accumulate(games.begin(), games.end(), static_cast<int64_t>(0),
                         [bag](int64_t sum, const Game& game) { return sum + (possible(game, bag) ? game.id : 0); });
}

int64_t task2(const std::vector<Game>& games)
{
  return std::accumulate(games.begin(), games.end(), static_cast<int64_t>(0), [](int64_t sum, const Game& game) {
    auto minBag = getMinimumBag(game);
    return sum + (minBag.red * minBag.green * minBag.blue);
  });
}

std::ostream& operator<<(std::ostream& os, Game const& value)
{
  os << "id: " << value.id << ", Drawings: ";

  for (const auto& d : value.drawings) {
    os << "(" << d.red << " " << d.green << " " << d.blue << "), ";
  }
  os << "\n";

  return os;
}

std::ostream& operator<<(std::ostream& os, Game::Drawing const& value)
{
  os << "(" << value.red << " " << value.green << " " << value.blue << ")\n";

  return os;
}

Game Game::fromStr(std::string_view v)
{
  scn::ranges::subrange r = v;
  Game game;
  if (auto result = scn::scan<int>(r, "Game {}:")) {
    game.id = result->value();
    r = result->range();
  } else {
    throw std::runtime_error("Parse error!");
  }

  Drawing d{};

  while (true) {
    int count{};
    std::string_view color;
    if (auto result = scn::scan<int, std::string_view>(r, " {} {}")) {
      r = result->range();
      std::tie(count, color) = result->values();
    } else {
      game.drawings.push_back(d);
      d = {};
      break;
    }

    bool endOfDrawing = false;
    if (color.back() == ',' || color.back() == ';') {
      if (color.back() == ';') {
        endOfDrawing = true;
      }
      color = std::string_view(color.begin(), color.size() - 1);
    }

    if (color == "blue") {
      d.blue += count;
    } else if (color == "red") {
      d.red += count;
    } else if (color == "green") {
      d.green += count;
    }

    if (endOfDrawing) {
      game.drawings.push_back(d);
      d = {};
    }
  }

  return game;
}

TEST_CASE("Inputs Task1")
{
  std::vector games = {Game::fromStr("Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"),
                       Game::fromStr("Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue"),
                       Game::fromStr("Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red"),
                       Game::fromStr("Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red"),
                       Game::fromStr("Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green")};

  REQUIRE(games[0] == Game{.id = 1, .drawings = {{4, 0, 3}, {1, 2, 6}, {0, 2, 0}}});
  REQUIRE(games[1] == Game{.id = 2, .drawings = {{0, 2, 1}, {1, 3, 4}, {0, 1, 1}}});
  REQUIRE(games[2] == Game{.id = 3, .drawings = {{20, 8, 6}, {4, 13, 5}, {1, 5, 0}}});
  REQUIRE(games[3] == Game{.id = 4, .drawings = {{3, 1, 6}, {6, 3, 0}, {14, 3, 15}}});
  REQUIRE(games[4] == Game{.id = 5, .drawings = {{6, 3, 1}, {1, 2, 2}}});

  Bag task1Bag = {.red = 12, .green = 13, .blue = 14};
  REQUIRE(possible(games[0], task1Bag));
  REQUIRE(possible(games[1], task1Bag));
  REQUIRE(!possible(games[2], task1Bag));
  REQUIRE(!possible(games[3], task1Bag));
  REQUIRE(possible(games[4], task1Bag));

  REQUIRE(task1(games) == 8);
}

TEST_CASE("Inputs Task2")
{
  std::vector games = {Game::fromStr("Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"),
                       Game::fromStr("Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue"),
                       Game::fromStr("Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red"),
                       Game::fromStr("Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red"),
                       Game::fromStr("Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green")};

  REQUIRE(getMinimumBag(games[0]) == Bag{4, 2, 6});
  REQUIRE(getMinimumBag(games[1]) == Bag{1, 3, 4});
  REQUIRE(getMinimumBag(games[2]) == Bag{20, 13, 6});
  REQUIRE(getMinimumBag(games[3]) == Bag{14, 3, 15});
  REQUIRE(getMinimumBag(games[4]) == Bag{6, 3, 2});

  REQUIRE(task2(games) == 2286);
}

TEST_CASE("Task1")
{
  auto input = readLines("../../day2/input.txt");
  std::vector<Game> games;
  games.reserve(input.size());
  for (const auto& l : input) {
    games.emplace_back(Game::fromStr(l));
  }
  std::cout << std::format("Day2 Task1 result: {}\n", task1(games));
}

TEST_CASE("Task2")
{
  auto input = readLines("../../day2/input.txt");
  std::vector<Game> games;
  games.reserve(input.size());
  for (const auto& l : input) {
    games.emplace_back(Game::fromStr(l));
  }
  std::cout << std::format("Day2 Task2 result: {}\n", task2(games));
}
