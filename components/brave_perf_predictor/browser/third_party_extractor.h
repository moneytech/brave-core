#ifndef BRAVE_PERFORMANCE_PREDICTOR_THIRD_PARTY_EXTRACTOR_H_
#define BRAVE_PERFORMANCE_PREDICTOR_THIRD_PARTY_EXTRACTOR_H_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "base/values.h"
#include "base/memory/singleton.h"

namespace base {
  template <typename T>
  struct DefaultSingletonTraits;
}

namespace brave_perf_predictor {

class ThirdPartyExtractor {
  public:
  	static ThirdPartyExtractor* GetInstance();

    bool load_entities(const std::string& entities);
    base::Optional<std::string> get_entity(const std::string& domain);
    bool is_initialized() { return initialized_; }

  private:
  	ThirdPartyExtractor();
    ThirdPartyExtractor(const std::string& entities);
    ~ThirdPartyExtractor();

    friend struct base::DefaultSingletonTraits<ThirdPartyExtractor>;
    bool initialized_ = false;
    std::unordered_map<std::string, std::string> entity_by_domain_;
    std::unordered_map<std::string, std::string> entity_by_root_domain_;

    DISALLOW_COPY_AND_ASSIGN(ThirdPartyExtractor);
};

}

#endif  // BRAVE_PERFORMANCE_PREDICTOR_THIRD_PARTY_EXTRACTOR_H_