#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> readLines(const std::filesystem::path& path)
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
