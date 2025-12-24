#include "sim/scheduler.hpp"

namespace rescueops::sim
{
  void Scheduler::schedule(Tick at, std::function<void()> fn)
  {
    q_.push(ScheduledEvent{at, next_seq_++, std::move(fn)});
  }

  void Scheduler::run_due(Tick now)
  {
    while (!q_.empty() && q_.top().tick <= now)
    {
      auto ev = q_.top();
      q_.pop();
      if (ev.fn) ev.fn();
    }
  }

  std::size_t Scheduler::pending() const
  {
    return q_.size();
  }
} // namespace rescueops::sim
