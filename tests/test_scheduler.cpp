#include "test_common.hpp"

#include "sim/scheduler.hpp"

using rescueops::sim::Scheduler;
using rescueops::sim::Tick;

TEST_CASE(test_scheduler_order)
{
  Scheduler s;
  std::string log;

  s.schedule(10, [&] { log += "A"; });
  s.schedule(5, [&] { log += "B"; });
  s.schedule(10, [&] { log += "C"; }); // same tick as A, should run after A because scheduled later

  s.run_due(4);
  TEST_ASSERT(log.empty());

  s.run_due(5);
  TEST_ASSERT(log == "B");

  s.run_due(10);
  TEST_ASSERT(log == "BAC");
}

int main()
{
  RUN_TEST(test_scheduler_order);
  std::cout << "All scheduler tests passed.\n";
  return 0;
}
