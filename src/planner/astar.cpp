#include "planner/astar.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <queue>

namespace rescueops::planner
{
  struct Node
  {
    int x = 0;
    int y = 0;
    int g = 0;
    int f = 0;
  };

  struct NodeCmp
  {
    bool operator()(const Node& a, const Node& b) const { return a.f > b.f; } // min-heap
  };

  static int manhattan(int x1, int y1, int x2, int y2)
  {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
  }

  std::optional<PathResult> astar(const Grid& grid, rescueops::sim::Vec2i start, rescueops::sim::Vec2i goal)
  {
    if (!grid.in_bounds(start.x, start.y) || !grid.in_bounds(goal.x, goal.y)) return std::nullopt;
    if (grid.is_blocked(start.x, start.y) || grid.is_blocked(goal.x, goal.y)) return std::nullopt;

    const int W = grid.w;
    const int H = grid.h;
    const auto idx = [W](int x, int y) { return y * W + x; };

    std::vector<int> gscore(static_cast<std::size_t>(W * H), std::numeric_limits<int>::max());
    std::vector<int> parent(static_cast<std::size_t>(W * H), -1);

    std::priority_queue<Node, std::vector<Node>, NodeCmp> open;
    const int sidx = idx(start.x, start.y);
    gscore[static_cast<std::size_t>(sidx)] = 0;
    open.push(Node{start.x, start.y, 0, manhattan(start.x, start.y, goal.x, goal.y)});

    const int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (!open.empty())
    {
      const auto cur = open.top();
      open.pop();

      if (cur.x == goal.x && cur.y == goal.y)
      {
        // reconstruct
        PathResult out;
        out.cost = cur.g;
        int c = idx(cur.x, cur.y);
        while (c != -1)
        {
          int x = c % W;
          int y = c / W;
          out.path.push_back(rescueops::sim::Vec2i{x, y});
          c = parent[static_cast<std::size_t>(c)];
        }
        std::reverse(out.path.begin(), out.path.end());
        return out;
      }

      for (auto& d : dirs)
      {
        int nx = cur.x + d[0];
        int ny = cur.y + d[1];
        if (!grid.in_bounds(nx, ny)) continue;
        if (grid.is_blocked(nx, ny)) continue;

        const int nidx = idx(nx, ny);
        const int tentative_g = cur.g + 1;

        if (tentative_g < gscore[static_cast<std::size_t>(nidx)])
        {
          gscore[static_cast<std::size_t>(nidx)] = tentative_g;
          parent[static_cast<std::size_t>(nidx)] = idx(cur.x, cur.y);
          const int h = manhattan(nx, ny, goal.x, goal.y);
          open.push(Node{nx, ny, tentative_g, tentative_g + h});
        }
      }
    }

    return std::nullopt;
  }
} // namespace rescueops::planner
