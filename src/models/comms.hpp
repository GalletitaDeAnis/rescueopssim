#pragma once
#include <cstdint>

namespace rescueops::models
{
  struct CommsLink
  {
    // very simple comms model placeholder
    double latency_ms = 50.0;
    double loss_rate = 0.01; // 1%
    std::uint32_t bandwidth_kbps = 256;
  };
} // namespace rescueops::models
