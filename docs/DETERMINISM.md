# Determinism

Why it matters:
- reproducible tests
- regression detection (replays)
- consistent benchmarks

## Rules in this project
1. Simulation time advances in **integer ticks** (fixed-step).
2. All randomness is driven by a **seeded PRNG** stored in the engine.
3. Event execution order is stable:
   - earlier tick first
   - same tick: lower sequence number first

## Notes
Floating point can break determinism across platforms. For now we keep physics trivial.
If you later add more physics/math, consider:
- fixed-point math for critical paths
- or platform constraints + tolerances
