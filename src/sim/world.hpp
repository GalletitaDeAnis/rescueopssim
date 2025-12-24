#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace rescueops::sim
{
  struct Vec2i
  {
    int x = 0;
    int y = 0;
  };

  struct Unit
  {
    std::uint32_t id = 0;
    std::string name;
    Vec2i pos{};
  };

  struct World
  {
    int width = 32;
    int height = 18;
    std::vector<Unit> units;
  };
} // namespace rescueops::sim
