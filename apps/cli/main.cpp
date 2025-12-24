#include <fstream>
#include <iostream>
#include <string>

#include "sim/engine.hpp"

static void usage()
{
  std::cout << "rescue_cli --scenario <path> [--ticks N] [--seed N] [--out results.json]\n";
}


int main(int argc, char** argv)
{
  std::string scenario_path = "scenarios/tutorial_01.json";
  std::string out_path;
  std::uint64_t seed_override = 0;
  bool has_seed_override = false;
  rescueops::sim::Tick ticks = 200;

  for (int i = 1; i < argc; ++i)
  {
    const std::string a = argv[i];
    if (a == "--help" || a == "-h")
    {
      usage();
      return 0;
    }
    if (a == "--scenario" && i + 1 < argc)
    {
      scenario_path = argv[++i];
      continue;
    }
    if (a == "--ticks" && i + 1 < argc)
    {
      ticks = static_cast<rescueops::sim::Tick>(std::stoull(argv[++i]));
      continue;
    }
    if (a == "--seed" && i + 1 < argc)
    {
      seed_override = std::stoull(argv[++i]);
      has_seed_override = true;
      continue;
    }
    if (a == "--out" && i + 1 < argc)
    {
      out_path = argv[++i];
      continue;
    }
  }

  rescueops::sim::Engine eng;
  if (!eng.load_scenario(scenario_path))
  {
    std::cerr << "Failed to load scenario: " << scenario_path << "\n";
    std::cerr << "Tip: run from repo root so relative paths work.\n";
    return 1;
  }

  if (has_seed_override) eng.set_seed(seed_override);

  const auto rr = eng.run(ticks);

  std::cout << "Ran " << rr.ticks_executed << " ticks (seed=" << rr.seed << ")\n";
  std::cout << "Units:\n";
  for (const auto& u : eng.world().units)
  {
    std::cout << "  - " << u.name << " @ (" << u.pos.x << "," << u.pos.y << ")\n";
  }

  if (!out_path.empty())
  {
    std::ofstream out(out_path, std::ios::binary);
    if (!out)
    {
      std::cerr << "Failed to write output: " << out_path << "\n";
      return 2;
    }

    // minimal JSON output without dependencies
    out << "{\n";
    out << "  \"seed\": " << rr.seed << ",\n";
    out << "  \"ticks\": " << rr.ticks_executed << ",\n";
    out << "  \"units\": [\n";
    for (std::size_t i = 0; i < eng.world().units.size(); ++i)
    {
      const auto& u = eng.world().units[i];
      out << "    {\"id\": " << u.id << ", \"name\": \"" << u.name << "\", \"x\": " << u.pos.x << ", \"y\": " << u.pos.y
          << "}";
      if (i + 1 < eng.world().units.size()) out << ",";
      out << "\n";
    }
    out << "  ]\n";
    out << "}\n";

    std::cout << "Wrote: " << out_path << "\n";
  }

  return 0;
}
