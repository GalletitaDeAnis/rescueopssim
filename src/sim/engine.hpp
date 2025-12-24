#pragma once
#include <cstdint>
#include <random>
#include <string>

#include "sim/scheduler.hpp"
#include "sim/world.hpp"

namespace rescueops::sim
{
  struct RunResult
  {
    Tick ticks_executed = 0;
    std::uint64_t seed = 0;
  };

  class Engine
  {
   public:
    Engine();

    // Load a scenario file (very lightweight parser: extracts a few integer fields + unit positions).
    bool load_scenario(const std::string& path);

    RunResult run(Tick ticks);

    World& world() { return world_; }
    const World& world() const { return world_; }

    Scheduler& scheduler() { return scheduler_; }
    std::mt19937_64& rng() { return rng_; }

    void set_seed(std::uint64_t seed);

   private:
    Scheduler scheduler_;
    World world_;
    std::mt19937_64 rng_;
    std::uint64_t seed_ = 0;

    static int extract_int_field(const std::string& text, const std::string& key, int fallback);
    static std::uint64_t extract_u64_field(const std::string& text, const std::string& key, std::uint64_t fallback);
    void extract_units_minimal(const std::string& text);
  };
} // namespace rescueops::sim
