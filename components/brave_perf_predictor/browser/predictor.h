#ifndef BRAVE_PERFORMANCE_PREDICTOR_PREDICTOR_H_
#define BRAVE_PERFORMANCE_PREDICTOR_PREDICTOR_H_

#include "predictor_parameters.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace brave_perf_predictor {

// Computes prediction based on the provided feature vector.
// It is the client's responsibility to provide features in
// the exact order expected by the predictor.
double predict(const std::array<double, feature_count> &features);

// Computes prediction based on key-value map of features.
// It translates the map to a feature vector internally, and
// it is the client's responsibility to ensure that all required
// features are present and only the necessary features are provided.
// The function uses 0 for any features not provided and ignores
// any extra features.
double predict(const std::unordered_map<std::string, double> &features);

}

#endif  // BRAVE_PERFORMANCE_PREDICTOR_PREDICTOR_H_