cmake_minimum_required(VERSION 3.22)

project(RescueOpsSim
  VERSION 0.1.0
  DESCRIPTION "Deterministic search & rescue operations simulator (C++20)"
  LANGUAGES CXX
)

option(RESCUEOPS_BUILD_UI "Build UI app (optional deps)" OFF)
option(RESCUEOPS_BUILD_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- Warnings (reasonable defaults) ----------
if (MSVC)
  add_compile_options(/W4 /permissive- /Zc:__cplusplus)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------- Library: sim_core ----------
add_library(sim_core
  src/sim/engine.cpp
  src/sim/scheduler.cpp
  src/sim/world.cpp

  src/models/comms.cpp
  src/models/motion.cpp
  src/models/sensors.cpp

  src/planner/astar.cpp
  src/planner/kalman.cpp
)

target_include_directories(sim_core PUBLIC src)

# ---------- CLI app ----------
add_executable(rescue_cli apps/cli/main.cpp)
target_link_libraries(rescue_cli PRIVATE sim_core)

# ---------- UI app (stub by default; can be expanded later) ----------
if (RESCUEOPS_BUILD_UI)
  add_executable(rescue_ui apps/ui/main.cpp)
  target_link_libraries(rescue_ui PRIVATE sim_core)
endif()

# ---------- Tests ----------
if (RESCUEOPS_BUILD_TESTS)
  include(CTest)
  enable_testing()

  add_executable(test_scheduler tests/test_scheduler.cpp)
  target_link_libraries(test_scheduler PRIVATE sim_core)
  add_test(NAME test_scheduler COMMAND test_scheduler)

  add_executable(test_astar tests/test_astar.cpp)
  target_link_libraries(test_astar PRIVATE sim_core)
  add_test(NAME test_astar COMMAND test_astar)
endif()
