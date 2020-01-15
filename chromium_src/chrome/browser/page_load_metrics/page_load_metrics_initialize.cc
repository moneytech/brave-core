// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#define InitializePageLoadMetricsForWebContents InitializePageLoadMetricsForWebContents_Chromium
#include "../../../../../chrome/browser/page_load_metrics/page_load_metrics_initialize.cc"
#undef InitializePageLoadMetricsForWebContents

// #include "brave/browser/page_load_metrics/brave_page_load_metrics_embedder.h"

#include "brave/components/brave_perf_predictor/browser/buildflags/buildflags.h"

#if BUILDFLAG(ENABLE_BRAVE_PERF_PREDICTOR)
#include "brave/components/brave_perf_predictor/browser/perf_predictor_page_metrics_observer.h"
using brave_perf_predictor::PerfPredictorPageMetricsObserver;
#endif

namespace chrome {

namespace {

class BravePageLoadMetricsEmbedder
    : public chrome::PageLoadMetricsEmbedder {
 public:
  explicit BravePageLoadMetricsEmbedder(content::WebContents* web_contents);
  ~BravePageLoadMetricsEmbedder() override;

 protected:
  // page_load_metrics::PageLoadMetricsEmbedderBase:
  void RegisterEmbedderObservers(
      ::page_load_metrics::PageLoadTracker* tracker) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(BravePageLoadMetricsEmbedder);
};

BravePageLoadMetricsEmbedder::BravePageLoadMetricsEmbedder(
  content::WebContents* web_contents)
  : chrome::PageLoadMetricsEmbedder(web_contents) {}

BravePageLoadMetricsEmbedder::~BravePageLoadMetricsEmbedder() = default;

void BravePageLoadMetricsEmbedder::RegisterEmbedderObservers(
    page_load_metrics::PageLoadTracker* tracker) {
  PageLoadMetricsEmbedder::RegisterEmbedderObservers(tracker);

  LOG(ERROR) << "BRAVE for web contents!";
#if BUILDFLAG(ENABLE_BRAVE_PERF_PREDICTOR)
  tracker->AddObserver(std::make_unique<PerfPredictorPageMetricsObserver>());
#endif
}

}  // namespace

void InitializePageLoadMetricsForWebContents(
    content::WebContents* web_contents) {
  // Change this method? consider to modify the peer in
  // android_webview/browser/page_load_metrics/page_load_metrics_initialize.cc
  // as well.
	LOG(ERROR) << "PLM for web contents!";
  page_load_metrics::MetricsWebContentsObserver::CreateForWebContents(
    web_contents,
    std::make_unique<BravePageLoadMetricsEmbedder>(
      web_contents));
}

}  // namespace chrome
