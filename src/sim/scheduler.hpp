#pragma once
#include <cstdint>
#include <functional>
#include <queue>
#include <vector>

namespace rescueops::sim
{
  using Tick = std::uint64_t;

  struct ScheduledEvent
  {
    Tick tick{};
    std::uint64_t seq{}; // tie-breaker for stable ordering
    std::function<void()> fn;

    // priority_queue puts "largest" first; we invert for min-heap behavior.
    bool operator<(const ScheduledEvent& other) const
    {
      if (tick != other.tick) return tick > other.tick;
      return seq > other.seq;
    }
  };

  class Scheduler
  {
   public:
    void schedule(Tick at, std::function<void()> fn);
    void run_due(Tick now);
    std::size_t pending() const;

   private:
    std::priority_queue<ScheduledEvent> q_;
    std::uint64_t next_seq_ = 0;
  };
} // namespace rescueops::sim
