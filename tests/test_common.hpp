#pragma once
#include <cstdlib>
#include <iostream>
#include <string>

#define TEST_ASSERT(expr)                                                                                               \
  do                                                                                                                     \
  {                                                                                                                      \
    if (!(expr))                                                                                                         \
    {                                                                                                                    \
      std::cerr << "ASSERT FAILED: " #expr << " at " << __FILE__ << ":" << __LINE__ << "\n";                             \
      std::exit(1);                                                                                                      \
    }                                                                                                                    \
  } while (0)

#define TEST_CASE(name) static void name()
#define RUN_TEST(name)                                                                                                   \
  do                                                                                                                     \
  {                                                                                                                      \
    std::cout << "[TEST] " #name "\n";                                                                                   \
    name();                                                                                                              \
  } while (0)
