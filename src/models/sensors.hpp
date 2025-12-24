#pragma once
#include <cstdint>

namespace rescueops::models
{
  struct SensorNoise
  {
    double position_sigma = 0.5;
    double dropout_rate = 0.00;
  };
} // namespace rescueops::models
