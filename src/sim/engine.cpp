#include "sim/engine.hpp"

#include <fstream>
#include <sstream>

namespace rescueops::sim
{
  Engine::Engine()
  {
    set_seed(42);
  }

  void Engine::set_seed(std::uint64_t seed)
  {
    seed_ = seed;
    rng_.seed(seed_);
  }

  int Engine::extract_int_field(const std::string& text, const std::string& key, int fallback)
  {
    // naive key finder: looks for "key" : <number>
    const std::string needle = "\"" + key + "\"";
    auto pos = text.find(needle);
    if (pos == std::string::npos) return fallback;

    pos = text.find(':', pos);
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t')) ++pos;

    bool neg = false;
    if (pos < text.size() && text[pos] == '-')
    {
      neg = true;
      ++pos;
    }

    long long value = 0;
    bool any = false;
    while (pos < text.size() && text[pos] >= '0' && text[pos] <= '9')
    {
      any = true;
      value = value * 10 + (text[pos] - '0');
      ++pos;
    }
    if (!any) return fallback;
    if (neg) value = -value;
    return static_cast<int>(value);
  }

  std::uint64_t Engine::extract_u64_field(const std::string& text, const std::string& key, std::uint64_t fallback)
  {
    const std::string needle = "\"" + key + "\"";
    auto pos = text.find(needle);
    if (pos == std::string::npos) return fallback;

    pos = text.find(':', pos);
    if (pos == std::string::npos) return fallback;
    ++pos;

    while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t')) ++pos;

    std::uint64_t value = 0;
    bool any = false;
    while (pos < text.size() && text[pos] >= '0' && text[pos] <= '9')
    {
      any = true;
      value = value * 10 + static_cast<std::uint64_t>(text[pos] - '0');
      ++pos;
    }
    return any ? value : fallback;
  }

  void Engine::extract_units_minimal(const std::string& text)
  {
    // Very minimal: scans for objects like {"name":"U1","x":1,"y":2}
    // Not a full JSON parser; good enough for starter scenarios.
    world_.units.clear();

    std::size_t pos = 0;
    std::uint32_t next_id = 1;
    while (true)
    {
      auto obj = text.find("{", pos);
      if (obj == std::string::npos) break;
      auto end = text.find("}", obj);
      if (end == std::string::npos) break;

      auto chunk = text.substr(obj, end - obj + 1);

      // Only accept objects that contain "name" and "x" and "y"
      if (chunk.find("\"name\"") != std::string::npos && chunk.find("\"x\"") != std::string::npos &&
          chunk.find("\"y\"") != std::string::npos)
      {
        Unit u;
        u.id = next_id++;

        // name
        auto npos = chunk.find("\"name\"");
        if (npos != std::string::npos)
        {
          auto colon = chunk.find(':', npos);
          if (colon != std::string::npos)
          {
            auto q1 = chunk.find('"', colon + 1);
            auto q2 = (q1 != std::string::npos) ? chunk.find('"', q1 + 1) : std::string::npos;
            if (q1 != std::string::npos && q2 != std::string::npos)
            {
              u.name = chunk.substr(q1 + 1, q2 - q1 - 1);
            }
          }
        }
        if (u.name.empty()) u.name = "unit_" + std::to_string(u.id);

        u.pos.x = extract_int_field(chunk, "x", 0);
        u.pos.y = extract_int_field(chunk, "y", 0);

        world_.units.push_back(u);
      }

      pos = end + 1;
    }

    if (world_.units.empty())
    {
      // default unit if none provided
      world_.units.push_back(Unit{1, "alpha", Vec2i{1, 1}});
    }
  }

  bool Engine::load_scenario(const std::string& path)
  {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    std::ostringstream ss;
    ss << in.rdbuf();
    const auto text = ss.str();

    // Scenario fields
    const auto seed = extract_u64_field(text, "seed", 42);
    set_seed(seed);

    world_.width = extract_int_field(text, "width", 32);
    world_.height = extract_int_field(text, "height", 18);

    extract_units_minimal(text);
    return true;
  }

  RunResult Engine::run(Tick ticks)
  {
    RunResult rr;
    rr.seed = seed_;

    // Example of scheduled recurring event: a heartbeat that runs each 50 ticks.
    for (Tick t = 0; t <= ticks; t += 50)
    {
      scheduler_.schedule(t, [this, t] {
        // deterministic no-op for now; later: metrics snapshots, comms updates, etc.
        (void)t;
      });
    }

    for (Tick t = 0; t < ticks; ++t)
    {
      scheduler_.run_due(t);

      // Trivial motion model: random walk (deterministic due to seed).
      // (Later replace with motion + planner outputs)
      if (!world_.units.empty())
      {
        std::uniform_int_distribution<int> step(-1, 1);
        for (auto& u : world_.units)
        {
          u.pos.x = std::max(0, std::min(world_.width - 1, u.pos.x + step(rng_)));
          u.pos.y = std::max(0, std::min(world_.height - 1, u.pos.y + step(rng_)));
        }
      }
      rr.ticks_executed = t + 1;
    }
    return rr;
  }

} // namespace rescueops::sim
