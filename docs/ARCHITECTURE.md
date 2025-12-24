# Architecture

This starter kit uses a simple modular layout:

- `src/sim/` core simulation (deterministic clock, scheduler, engine, world)
- `src/models/` simulation models (motion, sensors, comms) — currently minimal stubs
- `src/planner/` planning algorithms (A*, Kalman utility)
- `apps/cli/` headless runner (CI-friendly)
- `apps/ui/` placeholder for a future UI
- `tests/` unit tests

## Determinism

The engine runs in discrete **ticks** (fixed-step). The scheduler executes events at a given tick,
ensuring that given the same seed + scenario + number of ticks, outputs are reproducible.
See `docs/DETERMINISM.md`.
