#include "test_common.hpp"

#include "planner/astar.hpp"

using rescueops::planner::Grid;
using rescueops::planner::astar;
using rescueops::sim::Vec2i;

TEST_CASE(test_astar_simple_path)
{
  Grid g;
  g.w = 5;
  g.h = 5;
  g.blocked.assign(static_cast<std::size_t>(g.w * g.h), 0);

  // add a small wall
  g.blocked[1 * g.w + 2] = 1;
  g.blocked[2 * g.w + 2] = 1;
  g.blocked[3 * g.w + 2] = 1;

  auto res = astar(g, Vec2i{0, 0}, Vec2i{4, 4});
  TEST_ASSERT(res.has_value());
  TEST_ASSERT(!res->path.empty());
  TEST_ASSERT(res->path.front().x == 0 && res->path.front().y == 0);
  TEST_ASSERT(res->path.back().x == 4 && res->path.back().y == 4);
}

int main()
{
  RUN_TEST(test_astar_simple_path);
  std::cout << "All A* tests passed.\n";
  return 0;
}
