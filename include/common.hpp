#pragma once

#include <deque>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

inline std::vector<std::string> readLines(const std::filesystem::path& path)
{
  std::ifstream file(path);
  if (!file.is_open()) {
    return {};
  }

  std::string line;
  std::vector<std::string> result;
  while (std::getline(file, line)) {
    result.emplace_back(std::move(line));
  }

  return result;
}

template<class T>
inline T rangeTo(auto rng)
{
  return T(rng.begin(), rng.end());
}

inline auto toVector(auto&& rng)
{
  return std::vector(rng.begin(), rng.end());
}

inline auto toDeque(auto&& rng)
{
  return std::deque(rng.begin(), rng.end());
}
