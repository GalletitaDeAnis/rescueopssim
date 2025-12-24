# RescueOpsSim

[![CI](https://github.com/GalletitaDeAnis/rescueopssim/actions/workflows/ci.yml/badge.svg)](https://github.com/GalletitaDeAnis/rescueopssim/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-informational.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](#)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)](#)

**Deterministic search-and-rescue operations simulator in modern C++ (C++20).**  
A portfolio-grade systems project focused on **determinism, reproducibility, and engineering quality**: simulation core, CLI runner, unit tests, CI, and a UI stub ready for SDL2/ImGui.

> **Safety / Scope**
> RescueOpsSim is designed for **humanitarian and civil** scenarios (search & rescue, disaster response, logistics training).
> It is **not** intended for weaponization, targeting, or harm.

---

## Why this project is a strong C++ portfolio piece

RescueOpsSim intentionally targets “serious systems” constraints:

- **Determinism & reproducibility** (same scenario + seed → same result)
- **Testable architecture** (core simulation independent from UI)
- **Performance-friendly design** (fixed timestep, predictable scheduling)
- **Professional tooling** (CMake presets, CI matrix, lint/format configs)
- **Extensible model layers** (motion/sensors/comms stubs + planners)

---

## What you get (today)

- ✅ Deterministic simulation core (**fixed-tick scheduler + stable update order**)
- ✅ Scenario-driven runs via JSON (`scenarios/*.json`)
- ✅ CLI runner (`rescue_cli`) for headless execution + results export
- ✅ Planning starter: **A\*** pathfinding (**unit-tested**)
- ✅ Tracking starter: simple **Kalman filter** (**unit-tested**)
- ✅ Repo hygiene: CMake presets, GitHub Actions, clang-format/tidy configs
- 🧩 UI app stub (`rescue_ui`) behind `RESCUEOPS_BUILD_UI` (SDL2/ImGui-ready)

---

## Quickstart (Windows)

### Requirements
- **Visual Studio 2022** (C++ workload)
- **CMake ≥ 3.22** (or the one bundled with VS)
- (Optional) **Ninja** for faster builds

### Configure + build + test
From the repository root:

```powershell
cmake --preset windows
cmake --build --preset windows
ctest --preset windows
```

### Run the CLI
```powershell
.\build\windows\RelWithDebInfo\apps\cli\rescue_cli.exe --scenario scenarios\tutorial_01.json --ticks 200 --out results.json
```

---

## Quickstart (Linux)

```bash
cmake --preset linux
cmake --build --preset linux
ctest --preset linux

./build/linux/apps/cli/rescue_cli --scenario scenarios/tutorial_01.json --ticks 200 --out results.json
```

---

## CLI usage

```txt
rescue_cli --scenario <path> [--ticks N] [--seed N] [--out results.json]
```

Examples:

```bash
./build/linux/apps/cli/rescue_cli --scenario scenarios/urban_rescue_10u.json --ticks 500 --seed 1337 --out out.json
```

```powershell
.\build\windows\RelWithDebInfo\apps\cli\rescue_cli.exe --scenario scenarios\urban_rescue_10u.json --ticks 500 --seed 1337 --out out.json
```

---

## Scenario format (JSON)

Minimal example:

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

More: `docs/SCENARIOS.md`

---

## Determinism (how it’s enforced)

Determinism is crucial for debugging, CI, and replayable simulations.

RescueOpsSim follows these rules:
- **Fixed timestep** simulation (`Tick`)
- **Stable ordering** for updates and scheduled events
- **Seed-controlled RNG** (scenario seed, optional CLI override)

Details: `docs/DETERMINISM.md`

---

## Project layout

```txt
rescueopssim/
  apps/
    cli/            # headless runner (CI-friendly)
    ui/             # visualization stub (SDL2/ImGui ready)
  src/
    sim/            # engine / scheduler / world
    models/         # comms / sensors / motion (starter)
    planner/        # A* / Kalman (starter)
  tests/            # unit tests (ctest)
  scenarios/        # JSON scenarios
  docs/             # architecture + design notes
```

Architecture notes: `docs/ARCHITECTURE.md`

---

## Dependencies (optional, via vcpkg)

This repo includes a `vcpkg.json` manifest for **planned optional dependencies**:
- `fmt`, `spdlog`
- `nlohmann-json`
- `entt`
- `sdl2`, `imgui`
- `glm`

**Note:** The current starter builds without requiring these packages.  
They become relevant once you expand `rescue_ui` and upgrade JSON/logging.

### Using vcpkg on Windows (optional)

1) Install vcpkg (once) and set an environment variable:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
setx VCPKG_ROOT "C:\vcpkg"
```

2) Configure with the vcpkg toolchain:

```powershell
cmake -S . -B build\windows-vcpkg -G "Visual Studio 17 2022" -A x64 `
  -D CMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake" `
  -D RESCUEOPS_BUILD_TESTS=ON -D RESCUEOPS_BUILD_UI=ON
cmake --build build\windows-vcpkg --config RelWithDebInfo
```

---

## Build options

- `RESCUEOPS_BUILD_TESTS=ON/OFF`
- `RESCUEOPS_BUILD_UI=ON/OFF`

Presets in `CMakePresets.json` default to **tests ON** and **UI OFF**.

---

## Roadmap (portfolio-friendly)

- **V0.2**: upgrade scenario parsing to `nlohmann-json`, add obstacles + route output
- **V0.3**: comms model (latency/loss), sensor noise & dropout, deterministic replay file
- **V0.4**: UI timeline + overlays, Monte Carlo runner, summary statistics

Backlog: `ISSUES_BACKLOG.md`

---

## Contributing

This is a personal portfolio project, but contributions are welcome:

- Keep core changes **deterministic** (document ordering and RNG usage)
- Add tests for new behavior where possible
- Prefer small, reviewable PRs

---

## License

MIT — see `LICENSE`.
