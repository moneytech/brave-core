#ifndef BRAVE_PERFORMANCE_PREDICTOR_BANDWIDTH_SAVINGS_PREDICTOR_H_
#define BRAVE_PERFORMANCE_PREDICTOR_BANDWIDTH_SAVINGS_PREDICTOR_H_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "third_party_extractor.h"
#include "content/public/common/resource_load_info.mojom.h"
#include "components/page_load_metrics/common/page_load_metrics.mojom.h"

namespace brave_perf_predictor {

class BandwidthSavingsPredictor {
  public:
    BandwidthSavingsPredictor(ThirdPartyExtractor* third_party_extractor);
    ~BandwidthSavingsPredictor();

    void OnPageLoadTimingUpdated(const page_load_metrics::mojom::PageLoadTiming& timing);
    void OnSubresourceBlocked(const std::string& resource_url);
    void OnResourceLoadComplete(const GURL& main_frame_url, const content::mojom::ResourceLoadInfo& resource_load_info);
    double predict();
    void Reset();

  private:
    ThirdPartyExtractor* third_party_extractor_ = nullptr;
    GURL main_frame_url_;
    std::unordered_map<std::string, double> feature_map_;
};

}

#endif  // BRAVE_PERFORMANCE_PREDICTOR_BANDWIDTH_SAVINGS_PREDICTOR_H_