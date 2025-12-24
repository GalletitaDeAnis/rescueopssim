#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "planner/astar.hpp"
#include "sim/engine.hpp"

// -----------------------------
// Minimal, dependency-free helpers
// -----------------------------
static void usage()
{
  std::cout << "rescue_cli --scenario <path> [--ticks N] [--seed N] [--out results.json] [--pretty]\n"
               "          [--ascii out.txt] [--emit-paths]\n";
}

static std::string read_all_text(const std::string& path)
{
  std::ifstream in(path, std::ios::binary);
  if (!in) return {};
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

static std::optional<std::string> extract_string_field(const std::string& text, const std::string& key)
{
  const std::string needle = "\"" + key + "\"";
  auto k = text.find(needle);
  if (k == std::string::npos) return std::nullopt;

  auto colon = text.find(':', k + needle.size());
  if (colon == std::string::npos) return std::nullopt;

  auto q1 = text.find('"', colon + 1);
  if (q1 == std::string::npos) return std::nullopt;
  auto q2 = text.find('"', q1 + 1);
  if (q2 == std::string::npos) return std::nullopt;

  return text.substr(q1 + 1, q2 - q1 - 1);
}

static std::optional<long long> extract_int_field(const std::string& text, const std::string& key)
{
  const std::string needle = "\"" + key + "\"";
  auto k = text.find(needle);
  if (k == std::string::npos) return std::nullopt;

  auto colon = text.find(':', k + needle.size());
  if (colon == std::string::npos) return std::nullopt;

  auto p = colon + 1;
  while (p < text.size() && std::isspace(static_cast<unsigned char>(text[p]))) ++p;

  bool neg = false;
  if (p < text.size() && text[p] == '-')
  {
    neg = true;
    ++p;
  }

  long long v = 0;
  bool any = false;
  while (p < text.size() && std::isdigit(static_cast<unsigned char>(text[p])))
  {
    any = true;
    v = v * 10 + (text[p] - '0');
    ++p;
  }
  if (!any) return std::nullopt;

  return neg ? -v : v;
}

// Extract a JSON array substring by key, with simple bracket matching.
// Returns the contents from '[' to the matching ']' inclusive.
static std::optional<std::string> extract_array_blob(const std::string& text, const std::string& key)
{
  const std::string needle = "\"" + key + "\"";
  auto k = text.find(needle);
  if (k == std::string::npos) return std::nullopt;

  auto lb = text.find('[', k + needle.size());
  if (lb == std::string::npos) return std::nullopt;

  int depth = 0;
  for (std::size_t i = lb; i < text.size(); ++i)
  {
    if (text[i] == '[') ++depth;
    else if (text[i] == ']')
    {
      --depth;
      if (depth == 0) return text.substr(lb, i - lb + 1);
    }
  }
  return std::nullopt;
}

struct Target
{
  std::string unit;
  int tx = 0;
  int ty = 0;
};

// Parse targets from: "targets": [{"unit":"alpha","tx":12,"ty":7}, ...]
static std::vector<Target> parse_targets(const std::string& scenario_text)
{
  std::vector<Target> out;

  auto blobOpt = extract_array_blob(scenario_text, "targets");
  if (!blobOpt) return out;
  const std::string blob = *blobOpt;

  std::size_t pos = 0;
  while (true)
  {
    auto o = blob.find('{', pos);
    if (o == std::string::npos) break;
    auto e = blob.find('}', o);
    if (e == std::string::npos) break;

    auto obj = blob.substr(o, e - o + 1);

    Target t;
    auto u = extract_string_field(obj, "unit");
    auto tx = extract_int_field(obj, "tx");
    auto ty = extract_int_field(obj, "ty");
    if (u && tx && ty)
    {
      t.unit = *u;
      t.tx = static_cast<int>(*tx);
      t.ty = static_cast<int>(*ty);
      out.push_back(t);
    }

    pos = e + 1;
  }
  return out;
}

// Obstacles format (single key):
// "obstacles": [
//   {"x":16,"y":0,"w":1,"h":8},   // rect
//   {"x":10,"y":10}              // single cell
// ]
static int apply_obstacles(const std::string& scenario_text, int w, int h, std::vector<std::uint8_t>& blocked)
{
  int count = 0;

  auto set_cell = [&](int x, int y) {
    if (x < 0 || y < 0 || x >= w || y >= h) return;
    const std::size_t idx = static_cast<std::size_t>(y * w + x);
    if (blocked[idx] == 0)
    {
      blocked[idx] = 1;
      ++count;
    }
  };

  auto blobOpt = extract_array_blob(scenario_text, "obstacles");
  if (!blobOpt) return 0;

  const std::string blob = *blobOpt;
  std::size_t pos = 0;
  while (true)
  {
    auto o = blob.find('{', pos);
    if (o == std::string::npos) break;
    auto e = blob.find('}', o);
    if (e == std::string::npos) break;

    const std::string obj = blob.substr(o, e - o + 1);

    auto x = extract_int_field(obj, "x");
    auto y = extract_int_field(obj, "y");
    if (!x || !y)
    {
      pos = e + 1;
      continue;
    }

    auto rw = extract_int_field(obj, "w");
    auto rh = extract_int_field(obj, "h");

    if (rw && rh)
    {
      const int W = static_cast<int>(*rw);
      const int H = static_cast<int>(*rh);
      for (int yy = 0; yy < H; ++yy)
        for (int xx = 0; xx < W; ++xx)
          set_cell(static_cast<int>(*x) + xx, static_cast<int>(*y) + yy);
    }
    else
    {
      set_cell(static_cast<int>(*x), static_cast<int>(*y));
    }

    pos = e + 1;
  }

  return count;
}

static char unit_glyph(const std::string& name)
{
  for (char c : name)
  {
    if (std::isalpha(static_cast<unsigned char>(c)))
      return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  }
  return 'U';
}

struct PlanOut
{
  std::string unit;
  rescueops::sim::Vec2i start{};
  rescueops::sim::Vec2i goal{};
  bool found = false;
  int cost = 0;
  std::vector<rescueops::sim::Vec2i> path;
};

static std::string render_ascii_map(const rescueops::sim::World& w,
                                    const std::vector<Target>& targets,
                                    const std::vector<PlanOut>& plans,
                                    const std::vector<std::uint8_t>& blocked,
                                    bool draw_paths)
{
  std::vector<std::string> grid(static_cast<std::size_t>(w.height), std::string(static_cast<std::size_t>(w.width), '.'));

  // 0) Obstacles first: '#'
  for (int y = 0; y < w.height; ++y)
  {
    for (int x = 0; x < w.width; ++x)
    {
      const std::size_t idx = static_cast<std::size_t>(y * w.width + x);
      if (idx < blocked.size() && blocked[idx] != 0)
      {
        grid[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = '#';
      }
    }
  }

  // 1) Paths next: '*' only on empty cells ('.'), never over obstacles
  if (draw_paths)
  {
    for (const auto& p : plans)
    {
      if (!p.found) continue;
      for (const auto& pt : p.path)
      {
        if (pt.x < 0 || pt.y < 0 || pt.x >= w.width || pt.y >= w.height) continue;
        char& cell = grid[static_cast<std::size_t>(pt.y)][static_cast<std::size_t>(pt.x)];
        if (cell == '.') cell = '*';
      }
    }
  }

  // 2) Targets: 'x' override '*' (but not '#')
  for (const auto& t : targets)
  {
    if (t.tx >= 0 && t.ty >= 0 && t.tx < w.width && t.ty < w.height)
    {
      char& cell = grid[static_cast<std::size_t>(t.ty)][static_cast<std::size_t>(t.tx)];
      if (cell != '#') cell = 'x';
    }
  }

  // 3) Units override everything
  for (const auto& u : w.units)
  {
    if (u.pos.x >= 0 && u.pos.y >= 0 && u.pos.x < w.width && u.pos.y < w.height)
      grid[static_cast<std::size_t>(u.pos.y)][static_cast<std::size_t>(u.pos.x)] = unit_glyph(u.name);
  }

  std::ostringstream out;
  out << "ASCII map (origin at top-left)\n";
  out << "Legend: '.' empty, '#' obstacle, '*' planned path, 'x' target, letter = unit\n\n";
  for (int y = 0; y < w.height; ++y)
  {
    out << grid[static_cast<std::size_t>(y)] << "\n";
  }
  return out.str();
}

static void write_json_vec2(std::ostream& out, int x, int y)
{
  out << "{\"x\": " << x << ", \"y\": " << y << "}";
}

static void write_path(std::ostream& out, const std::vector<rescueops::sim::Vec2i>& path, bool pretty, int indent)
{
  if (path.empty())
  {
    out << "[]";
    return;
  }
  out << "[";
  if (pretty) out << "\n";
  for (std::size_t i = 0; i < path.size(); ++i)
  {
    if (pretty) out << std::string(static_cast<std::size_t>(indent), ' ');
    write_json_vec2(out, path[i].x, path[i].y);
    if (i + 1 < path.size()) out << ",";
    if (pretty) out << "\n";
  }
  if (pretty && indent >= 2) out << std::string(static_cast<std::size_t>(indent - 2), ' ');
  out << "]";
}

static void write_results_json(std::ostream& out,
                               const std::string& scenario_path,
                               rescueops::sim::Tick ticks_requested,
                               const rescueops::sim::RunResult& rr,
                               const rescueops::sim::World& world,
                               int obstacles_count,
                               const std::vector<Target>& targets,
                               const std::vector<PlanOut>& plans,
                               bool pretty,
                               bool emit_paths)
{
  const char* nl = pretty ? "\n" : "";
  const char* sp = pretty ? " " : "";
  auto indent = [&](int n) -> std::string { return pretty ? std::string(static_cast<std::size_t>(n), ' ') : ""; };

  out << "{" << nl;
  out << indent(2) << "\"version\": \"0.3-demo-obstacles\"," << nl;

  // scenario
  out << indent(2) << "\"scenario\": {" << nl;
  out << indent(4) << "\"path\": \"" << scenario_path << "\"," << nl;
  out << indent(4) << "\"seed\": " << rr.seed << "," << nl;
  out << indent(4) << "\"ticks_requested\": " << ticks_requested << "," << nl;
  out << indent(4) << "\"ticks_executed\": " << rr.ticks_executed << nl;
  out << indent(2) << "}," << nl;

  // world
  out << indent(2) << "\"world\": {" << nl;
  out << indent(4) << "\"width\": " << world.width << "," << nl;
  out << indent(4) << "\"height\": " << world.height << "," << nl;
  out << indent(4) << "\"unit_count\": " << world.units.size() << "," << nl;
  out << indent(4) << "\"obstacles_count\": " << obstacles_count << nl;
  out << indent(2) << "}," << nl;

  // targets
  out << indent(2) << "\"targets\": [" << nl;
  for (std::size_t i = 0; i < targets.size(); ++i)
  {
    out << indent(4) << "{"
        << "\"unit\": \"" << targets[i].unit << "\"," << sp
        << "\"tx\": " << targets[i].tx << "," << sp
        << "\"ty\": " << targets[i].ty << "}";
    if (i + 1 < targets.size()) out << ",";
    out << nl;
  }
  out << indent(2) << "]," << nl;

  // units
  out << indent(2) << "\"units\": [" << nl;
  for (std::size_t i = 0; i < world.units.size(); ++i)
  {
    const auto& u = world.units[i];
    out << indent(4) << "{"
        << "\"id\": " << u.id << "," << sp
        << "\"name\": \"" << u.name << "\"," << sp
        << "\"pos\": ";
    write_json_vec2(out, u.pos.x, u.pos.y);
    out << "}";
    if (i + 1 < world.units.size()) out << ",";
    out << nl;
  }
  out << indent(2) << "]," << nl;

  // plans
  out << indent(2) << "\"plans\": [" << nl;
  for (std::size_t i = 0; i < plans.size(); ++i)
  {
    const auto& p = plans[i];
    const int steps = (p.found && emit_paths && !p.path.empty()) ? static_cast<int>(p.path.size()) - 1 : 0;

    out << indent(4) << "{" << nl;
    out << indent(6) << "\"unit\": \"" << p.unit << "\"," << nl;
    out << indent(6) << "\"start\": ";
    write_json_vec2(out, p.start.x, p.start.y);
    out << "," << nl;
    out << indent(6) << "\"goal\": ";
    write_json_vec2(out, p.goal.x, p.goal.y);
    out << "," << nl;
    out << indent(6) << "\"found\": " << (p.found ? "true" : "false") << "," << nl;
    out << indent(6) << "\"cost\": " << p.cost << "," << nl;
    out << indent(6) << "\"steps\": " << steps;

    if (emit_paths)
    {
      out << "," << nl;
      out << indent(6) << "\"path\": ";
      write_path(out, p.path, pretty, 8);
      out << nl;
    }
    else
    {
      out << nl;
    }

    out << indent(4) << "}";
    if (i + 1 < plans.size()) out << ",";
    out << nl;
  }
  out << indent(2) << "]" << nl;

  out << "}" << nl;
}

int main(int argc, char** argv)
{
  std::string scenario_path = "scenarios/tutorial_01.json";
  std::string out_path;
  std::string ascii_path;
  rescueops::sim::Tick ticks = 200;
  std::optional<std::uint64_t> seed_override;
  bool pretty = false;
  bool emit_paths = false;

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
      continue;
    }
    if (a == "--out" && i + 1 < argc)
    {
      out_path = argv[++i];
      continue;
    }
    if (a == "--pretty")
    {
      pretty = true;
      continue;
    }
    if (a == "--emit-paths")
    {
      emit_paths = true;
      continue;
    }
    if (a == "--ascii" && i + 1 < argc)
    {
      ascii_path = argv[++i];
      continue;
    }

    std::cerr << "Unknown arg: " << a << "\n";
    usage();
    return 2;
  }

  rescueops::sim::Engine eng;
  if (!eng.load_scenario(scenario_path))
  {
    std::cerr << "Failed to load scenario: " << scenario_path << "\n";
    return 1;
  }
  if (seed_override) eng.set_seed(*seed_override);

  // Parse optional demo fields from scenario text (dependency-free)
  const std::string scenario_text = read_all_text(scenario_path);
  const auto targets = parse_targets(scenario_text);

  // Run simulation core (deterministic scheduler; movement is a roadmap item)
  const auto rr = eng.run(ticks);

  // Build a planning grid from scenario (obstacles are used in A* + ASCII)
  rescueops::planner::Grid grid;
  grid.w = eng.world().width;
  grid.h = eng.world().height;
  grid.blocked.assign(static_cast<std::size_t>(grid.w * grid.h), 0);

  const int obstacles_count = apply_obstacles(scenario_text, grid.w, grid.h, grid.blocked);

  // Plan paths (A*) per-unit
  std::vector<PlanOut> plans;
  plans.reserve(eng.world().units.size());

  for (const auto& u : eng.world().units)
  {
    PlanOut po;
    po.unit = u.name;
    po.start = u.pos;

    bool has_goal = false;
    for (const auto& t : targets)
    {
      if (t.unit == u.name)
      {
        po.goal = {t.tx, t.ty};
        has_goal = true;
        break;
      }
    }

    if (!has_goal)
    {
      plans.push_back(po);
      continue;
    }

    if (po.goal.x >= 0 && po.goal.y >= 0 && po.goal.x < grid.w && po.goal.y < grid.h)
    {
      auto res = rescueops::planner::astar(grid, po.start, po.goal);
      if (res)
      {
        po.found = true;
        po.cost = res->cost;
        if (emit_paths) po.path = res->path;
      }
    }

    plans.push_back(po);
  }

  // ASCII map (now shows obstacles + optional paths)
  const std::string ascii = render_ascii_map(eng.world(), targets, plans, grid.blocked, emit_paths);
  std::cout << ascii << "\n";
  std::cout << "Obstacles loaded: " << obstacles_count << "\n";

  if (!ascii_path.empty())
  {
    std::ofstream aout(ascii_path, std::ios::binary);
    if (!aout)
    {
      std::cerr << "Failed to write ascii file: " << ascii_path << "\n";
      return 3;
    }
    aout << ascii;
    aout << "\nObstacles loaded: " << obstacles_count << "\n";
    std::cout << "Wrote: " << ascii_path << "\n";
  }

  // Results JSON
  if (!out_path.empty())
  {
    std::ofstream out(out_path, std::ios::binary);
    if (!out)
    {
      std::cerr << "Failed to open output file: " << out_path << "\n";
      return 3;
    }

    write_results_json(out, scenario_path, ticks, rr, eng.world(), obstacles_count, targets, plans, pretty, emit_paths);
    std::cout << "Wrote: " << out_path << "\n";
  }

  return 0;
}
