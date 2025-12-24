# RescueOpsSim

**Deterministic search-and-rescue operations simulator (C++20)** — a portfolio-grade systems project focused on **simulation correctness, reproducibility, and engineering quality**.

> **Safety / Scope**
> This repository is designed for **humanitarian & civil** use-cases (search-and-rescue, disaster response, logistics).
> It is **not** intended for weaponization, targeting, or harm.

---

## What you get (today)

- ✅ **Deterministic simulation core** (fixed-tick scheduler + stable update order)
- ✅ **Scenario-driven runs** via JSON files (`scenarios/*.json`)
- ✅ **CLI runner** (`rescue_cli`) for headless execution + minimal JSON result export
- ✅ **Planning algorithms**: A* pathfinding starter (unit-tested)
- ✅ **Tracking primitive**: simple Kalman filter starter (unit-tested)
- ✅ **Clean repo hygiene**: CMake presets, tests, CI, clang-format/tidy configs
- 🧩 **UI app stub** (`rescue_ui`) behind `RESCUEOPS_BUILD_UI` (ready for SDL2/ImGui integration)

---

## Why this is a strong C++ portfolio project

RescueOpsSim is intentionally built around the kinds of constraints you see in serious systems work:

- **Determinism & reproducibility** (same seed → same result)
- **Testable core** (simulation logic separate from UI)
- **Performance-friendly architecture** (fixed timestep, data-oriented layout ready)
- **Professional tooling** (CMake presets, CI matrix, lint configs)
- **Extensible design** (models for comms/sensors/motion, planners, scenario schema)

---

## Quickstart

### Requirements
- **CMake ≥ 3.22**
- A C++20 compiler:
  - Windows: Visual Studio 2022
  - Linux: Clang or GCC
- (Linux) `ninja-build` recommended

### Build + test (Linux)
```bash
cmake --preset linux
cmake --build --preset linux
ctest --preset linux
```

Run:
```bash
./build/linux/apps/cli/rescue_cli --scenario scenarios/tutorial_01.json --ticks 200 --out results.json
```

### Build + test (Windows / PowerShell)
```powershell
cmake --preset windows
cmake --build --preset windows
ctest --preset windows
```

Run:
```powershell
.\build\windows\RelWithDebInfo\apps\cli\rescue_cli.exe --scenario scenarios\tutorial_01.json --ticks 200 --out results.json
```

---

## CLI usage

```txt
rescue_cli --scenario <path> [--ticks N] [--seed N] [--out results.json]
```

Example:
```bash
./build/linux/apps/cli/rescue_cli --scenario scenarios/urban_rescue_10u.json --ticks 500 --seed 1337 --out out.json
```

---

## Scenario format

Scenarios are plain JSON. Minimal example:

```json
{
  "seed": 42,
  "ticks": 200,
  "world": { "width": 32, "height": 18 },
  "units": [
    { "name": "alpha", "x": 2, "y": 2 },
    { "name": "bravo", "x": 10, "y": 8 }
  ]
}
```

- `seed`: RNG seed for reproducibility  
- `ticks`: suggested run duration  
- `world`: world dimensions  
- `units`: initial unit positions

See more in `docs/SCENARIOS.md`.

---

## Determinism (how it’s enforced)

Determinism is crucial for debugging, CI, and “replayable” simulations.

RescueOpsSim follows these rules:
- Fixed-timestep simulation (`Tick`)
- Stable ordering for updates and events
- Seed-controlled RNG (scenario seed, optional CLI override)

Details: `docs/DETERMINISM.md`.

---

## Project layout

```txt
rescueopssim/
  apps/              # CLI and UI frontends
  src/
    sim/             # engine/scheduler/world
    models/          # comms/sensors/motion (starter)
    planner/         # astar/kalman (starter)
  tests/             # unit tests (ctest)
  scenarios/         # example scenarios
  docs/              # architecture + design notes
```

---

## Tooling & CI

### GitHub Actions
- Builds and runs tests on **Windows** and **Ubuntu**

Workflow: `.github/workflows/ci.yml`

### Formatting & static analysis
- `.clang-format` for consistent style
- `.clang-tidy` baseline config for modern C++ hygiene

---

## Dependencies (planned)

This repo includes a `vcpkg.json` manifest for planned optional dependencies:

- `sdl2`, `imgui` (UI visualization)
- `fmt`, `spdlog` (logging)
- `nlohmann-json` (scenario parsing upgrades)
- `entt` (ECS structure for scaling)
- `glm` (math)

**Note:** The current starter builds without requiring these packages.  
They become relevant once you expand `rescue_ui` and enhance parsing/logging.

---

## Roadmap (good “next commits”)

**V0.2**
- Replace minimal JSON parsing with `nlohmann-json`
- Add a grid map with obstacles
- A* path output + unit movement

**V0.3**
- Comms model: latency / packet loss simulation
- Sensor model: noise + dropout
- Deterministic replay recording

**V0.4**
- UI (SDL2 + ImGui): timeline, overlays, route visualization
- Monte Carlo runs with summary statistics

---

## Contributing (solo-friendly)

Recommended local workflow:
```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Format:
```bash
clang-format -i src/**/*.cpp src/**/*.hpp apps/**/*.cpp tests/**/*.cpp
```

---

## License
MIT — see `LICENSE`.

---

## Credits
Built as a systems/engineering portfolio project. Contributions and suggestions are welcome.
