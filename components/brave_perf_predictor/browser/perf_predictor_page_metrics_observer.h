/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_brave_perf_predictor_BROWSER_PERF_PREDICTOR_PAGE_METRICS_OBSERVER_H_
#define BRAVE_COMPONENTS_brave_perf_predictor_BROWSER_PERF_PREDICTOR_PAGE_METRICS_OBSERVER_H_

#include <stdint.h>
#include <string>

#include "base/macros.h"
#include "base/sequence_checker.h"
#include "components/page_load_metrics/browser/page_load_metrics_observer.h"
#include "components/page_load_metrics/common/page_load_metrics.mojom.h"
#include "components/sessions/core/session_id.h"
#include "services/metrics/public/cpp/ukm_source_id.h"
#include "brave/components/brave_perf_predictor/browser/perf_predictor_web_contents_observer.h"

namespace content {
class BrowserContext;
class NavigationHandle;
}  // namespace content

namespace brave_perf_predictor {
// Observer responsible for recording per site performance metrics.
class PerfPredictorPageMetricsObserver
    : public page_load_metrics::PageLoadMetricsObserver {
 public:
  PerfPredictorPageMetricsObserver();
  ~PerfPredictorPageMetricsObserver() override;

  void OnFirstContentfulPaintInPage(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnFirstMeaningfulPaintInMainFrameDocument(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;
  void OnLoadEventStart(
      const page_load_metrics::mojom::PageLoadTiming& timing) override;

 private:
  ObservePolicy OnCommit(content::NavigationHandle* navigation_handle,
                         ukm::SourceId source_id) override;

  ObservePolicy ShouldObserveMimeType(
      const std::string& mime_type) const override;

  int64_t navigation_id_ = 0;

  // The browser context this navigation is operating in.
  PerfPredictorWebContentsObserver* observer_ = nullptr;

  SEQUENCE_CHECKER(sequence_checker_);
  DISALLOW_COPY_AND_ASSIGN(PerfPredictorPageMetricsObserver);
};

}  // namespace brave_perf_predictor

#endif  // BRAVE_COMPONENTS_brave_perf_predictor_BROWSER_PERF_PREDICTOR_PAGE_METRICS_OBSERVER_H_
