#pragma once
#include <optional>
#include <vector>

#include "sim/world.hpp"

namespace rescueops::planner
{
  struct Grid
  {
    int w = 0;
    int h = 0;
    std::vector<std::uint8_t> blocked; // 0 free, 1 blocked

    bool in_bounds(int x, int y) const { return x >= 0 && y >= 0 && x < w && y < h; }
    bool is_blocked(int x, int y) const { return blocked[static_cast<std::size_t>(y) * w + x] != 0; }
  };

  struct PathResult
  {
    std::vector<rescueops::sim::Vec2i> path;
    int cost = 0;
  };

  std::optional<PathResult> astar(const Grid& grid, rescueops::sim::Vec2i start, rescueops::sim::Vec2i goal);
} // namespace rescueops::planner
