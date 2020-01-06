/* This file is automatically generated, do not edit directly */

#include <string>
#include <map>
#include <vector>
#include <array>

namespace brave_perf_predictor {

const double model_intercept = {{model.intercept}};
const int feature_count = {{model.coefficients | length}};
const std::array<double, feature_count> model_coefficients = { {{model.coefficients | join(',\n')}} };

const unsigned int standardise_feature_count = {{transformers.standardise.features | length}};

const std::array<double, standardise_feature_count> standardise_feature_means = {
{{transformers.standardise.mean | join(',\n')}} };

const std::array<double, standardise_feature_count> standardise_feature_scale = {
{{transformers.standardise.scale | join(',\n')}} };

const std::array<std::string, feature_count>  feature_sequence = {
  {% for feature in transformers.standardise.features %}
  "{{feature}}",
  {% endfor %}
  {% for feature in transformers.passthrough.features %}
  "{{feature}}",
  {% endfor %}
};

struct stdfactor {
    double mean, scale;
};

const std::map<std::string, stdfactor > stdfactor_map = {
  {% for feature, (mean, scale) in transformers.standardise.feature_map %}
  { "{{feature}}", { {{mean}}, {{scale}} } },
  {% endfor %}
};

}