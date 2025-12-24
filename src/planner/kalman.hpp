#pragma once
namespace rescueops::planner
{
  // Simple 1D Kalman filter for demonstration/utility.
  // State: x, covariance: P
  struct Kalman1D
  {
    double x = 0.0;
    double P = 1.0;
    double Q = 0.01; // process noise
    double R = 0.25; // measurement noise

    void predict(double u = 0.0)
    {
      x += u;
      P += Q;
    }

    void update(double z)
    {
      const double K = P / (P + R);
      x = x + K * (z - x);
      P = (1.0 - K) * P;
    }
  };
} // namespace rescueops::planner
