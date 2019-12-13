/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_savings/browser/perf_predictor_page_metrics_observer.h"

#include "brave/browser/brave_browser_process_impl.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/performance_monitor/system_monitor.h"
#include "chrome/browser/sessions/session_tab_helper.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_features.h"
#include "components/sessions/core/session_id.h"
#include "url/gurl.h"
#include "extensions/browser/event_router.h"

namespace brave_perf_predictor {

PerfPredictorPageMetricsObserver::PerfPredictorPageMetricsObserver() = default;

PerfPredictorPageMetricsObserver::
    ~PerfPredictorPageMetricsObserver() = default;

page_load_metrics::PageLoadMetricsObserver::ObservePolicy
PerfPredictorPageMetricsObserver::OnCommit(
    content::NavigationHandle* navigation_handle,
    ukm::SourceId source_id) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  Profile* profile = Profile::FromBrowserContext(
      navigation_handle->GetWebContents()->GetBrowserContext());
  // Skip if off the record
  if (!profile || profile->IsOffTheRecord()) {
    return STOP_OBSERVING;
  }

  navigation_id_ = navigation_handle->GetNavigationId();
  // We'll be forwarding all performance metrics to the observer
  observer_ = PerfPredictorWebContentsObserver::FromWebContents(
        navigation_handle->GetWebContents());
  if (!observer_) {
    return STOP_OBSERVING;
  }
  return CONTINUE_OBSERVING;
}

page_load_metrics::PageLoadMetricsObserver::ObservePolicy
PerfPredictorPageMetricsObserver::ShouldObserveMimeType(
    const std::string& mime_type) const {
  // Observe all MIME types. We still only use actual data usage, so strange
  // cases (e.g., data:// URLs) will still record the right amount of data
  // usage.
  return CONTINUE_OBSERVING;
}

void PerfPredictorPageMetricsObserver::OnFirstContentfulPaintInPage(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (observer_) {
    observer_->OnPageLoadTimingUpdated(timing);
  }
  VLOG(2) << navigation_id_ << " paint timing "
    << "NavigationToFirstContentfulPaint "
    << timing.paint_timing->first_contentful_paint.value();
}

void PerfPredictorPageMetricsObserver::
  OnFirstMeaningfulPaintInMainFrameDocument(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (observer_) {
    observer_->OnPageLoadTimingUpdated(timing);
  }
  VLOG(2) << navigation_id_ << " paint timing "
    << "NavigationToFirstMeaningfulPaint "
    << timing.paint_timing->first_meaningful_paint.value();
}

void PerfPredictorPageMetricsObserver::OnLoadEventStart(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  if (observer_) {
    observer_->OnPageLoadTimingUpdated(timing);
  }
  VLOG(2) << navigation_id_ << " document timing "
    << "NavigationToLoadEventFired "
    << timing.document_timing->load_event_start.value();
}

}  // namespace brave_perf_predictor
