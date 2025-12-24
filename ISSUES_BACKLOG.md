# ISSUES_BACKLOG (copy/paste into GitHub Issues)

Below is a curated backlog that makes this repo look like a real engineering project.
Each issue includes a suggested label set and clear acceptance criteria.

---

## V0.2 — Core usability

### 1) Upgrade scenario parsing to nlohmann-json
**Labels:** enhancement, sim, good-first-issue  
**Acceptance criteria:**
- Replace the minimal JSON parsing with `nlohmann::json`
- Validate required fields with clear error messages
- Add unit tests for parsing failures

### 2) Add obstacle grid to World
**Labels:** enhancement, sim  
**Acceptance criteria:**
- World supports blocked cells / obstacles
- Scenario JSON can define obstacles (rects or explicit cells)
- CLI run shows number of obstacles loaded

### 3) A* returns full path + cost
**Labels:** enhancement, planner, tests  
**Acceptance criteria:**
- A* returns the reconstructed path (list of points)
- Output includes path length and total cost
- Add tests for trivial and obstacle cases

### 4) Unit movement along planned route
**Labels:** enhancement, sim  
**Acceptance criteria:**
- Units move one cell per tick (or configurable speed)
- Deterministic movement (same seed => same positions)
- CLI exports final positions to results.json

### 5) Results schema v0.2
**Labels:** enhancement, cli  
**Acceptance criteria:**
- Define a stable `results.json` schema (versioned)
- Export: ticks, seed, units final positions, route metrics
- Add a docs section describing fields

---

## V0.3 — Robustness & realism (still safe)

### 6) Comms model: latency + packet loss
**Labels:** enhancement, models, sim  
**Acceptance criteria:**
- Link model includes latency distribution + loss probability
- Deterministic sampling (seeded)
- Unit tests for edge cases (0%/100% loss)

### 7) Sensor model: noise + dropout
**Labels:** enhancement, models, sim  
**Acceptance criteria:**
- Sensor reading adds noise (Gaussian or uniform)
- Dropout events are deterministic and logged
- Tests cover deterministic replay of readings

### 8) Deterministic replay file
**Labels:** enhancement, sim, tooling  
**Acceptance criteria:**
- Record a run to a replay file (binary or JSON)
- Replay produces identical results
- Add a test that compares outputs

### 9) Fault injection toggles (scenario-driven)
**Labels:** enhancement, models  
**Acceptance criteria:**
- Scenario can schedule faults (sensor dropout, comms degradation)
- Fault injection is deterministic and reproducible

---

## V0.4 — UI visualization

### 10) Minimal SDL2 window + render grid
**Labels:** enhancement, ui  
**Acceptance criteria:**
- `RESCUEOPS_BUILD_UI=ON` builds rescue_ui
- Grid drawn, obstacles shown, units drawn
- Keyboard controls: pause/play, speed up/down

### 11) ImGui debug panels (timeline + metrics)
**Labels:** enhancement, ui  
**Acceptance criteria:**
- Show tick, seed, simulation speed, unit positions
- Add a timeline scrubber for replay mode (if available)

### 12) Route overlay in UI
**Labels:** enhancement, ui, planner  
**Acceptance criteria:**
- Render planned paths on the grid
- Toggle overlay in ImGui

---

## Performance & quality

### 13) Add benchmarks for A* and scheduler
**Labels:** performance, benchmarks  
**Acceptance criteria:**
- Add Google Benchmark target
- Benchmarks run locally and document how to run
- Include baseline numbers in docs (optional)

### 14) clang-tidy baseline and CI job
**Labels:** tooling, ci  
**Acceptance criteria:**
- Add a CI step for clang-tidy (Linux)
- Document required packages
- Fail CI on new warnings (optional threshold)

### 15) Sanitizers in CI (Linux)
**Labels:** tooling, ci  
**Acceptance criteria:**
- ASan/UBSan preset builds and runs tests
- Document how to run locally

---

## Repo polish (portfolio impact)

### 16) Add screenshots + demo GIF to README
**Labels:** documentation  
**Acceptance criteria:**
- Add at least one screenshot of UI or CLI output
- Add a short demo GIF
- Update README “Demo” section

### 17) Add a CONTRIBUTING.md (tiny)
**Labels:** documentation  
**Acceptance criteria:**
- Simple workflow: build/test/format
- Code style rules
- PR checklist

### 18) Add a CHANGELOG.md
**Labels:** documentation  
**Acceptance criteria:**
- Keep a short changelog with versions and highlights

---

## Nice-to-have

### 19) Monte Carlo runner (N seeds)
**Labels:** enhancement, cli, sim  
**Acceptance criteria:**
- Run N seeds and aggregate metrics
- Output summary JSON (mean, stdev, percentiles)

### 20) ECS migration using EnTT (optional)
**Labels:** enhancement, sim, architecture  
**Acceptance criteria:**
- Convert world entities to EnTT
- Preserve determinism (document ordering rules)
