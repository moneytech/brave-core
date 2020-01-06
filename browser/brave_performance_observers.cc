/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "brave/browser/brave_performance_observers.h"
#include "brave/components/brave_perf_predictor/browser/buildflags/buildflags.h"

#if BUILDFLAG(ENABLE_BRAVE_PERF_PREDICTOR)
#include "brave/components/brave_perf_predictor/browser/perf_predictor_page_metrics_observer.h"
using brave_perf_predictor::PerfPredictorPageMetricsObserver;
#endif

namespace brave {

void RegisterPerformanceObservers(page_load_metrics::PageLoadTracker* tracker) {
#if BUILDFLAG(ENABLE_BRAVE_PERF_PREDICTOR)
  tracker->AddObserver(std::make_unique<PerfPredictorPageMetricsObserver>());
#endif
}

}  // namespace brave
