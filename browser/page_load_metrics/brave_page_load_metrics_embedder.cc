/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/page_load_metrics/brave_page_load_metrics_embedder.h"

#include "brave/components/brave_perf_predictor/browser/buildflags/buildflags.h"
#include "chrome/browser/prerender/prerender_contents.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/search/search.h"
#include "extensions/buildflags/buildflags.h"

#if BUILDFLAG(ENABLE_EXTENSIONS)
#include "extensions/common/constants.h"
#endif

#if BUILDFLAG(ENABLE_BRAVE_PERF_PREDICTOR)
#include "brave/components/brave_perf_predictor/browser/perf_predictor_page_metrics_observer.h"
using brave_perf_predictor::PerfPredictorPageMetricsObserver;
#endif

namespace brave {

namespace page_load_metrics {

BravePageLoadMetricsEmbedder::BravePageLoadMetricsEmbedder(
    content::WebContents* web_contents,
    std::unique_ptr<::page_load_metrics::PageLoadMetricsEmbedderInterface>
        base_embedder)
    : PageLoadMetricsEmbedderBase(web_contents),
      base_embedder_(std::move(base_embedder)) {}

BravePageLoadMetricsEmbedder::~BravePageLoadMetricsEmbedder() = default;

void BravePageLoadMetricsEmbedder::RegisterEmbedderObservers(
    ::page_load_metrics::PageLoadTracker* tracker) {
  if (base_embedder_) {
    base_embedder_->RegisterObservers(tracker);
  }

  if (!IsPrerendering() && !IsNewTabPageUrl(web_contents()->GetURL()) &&
      !IsExtensionUrl(web_contents()->GetURL())) {
#if BUILDFLAG(ENABLE_BRAVE_PERF_PREDICTOR)
    tracker->AddObserver(std::make_unique<PerfPredictorPageMetricsObserver>());
#endif
  }
}

bool BravePageLoadMetricsEmbedder::IsPrerendering() const {
  return prerender::PrerenderContents::FromWebContents(web_contents()) !=
         nullptr;
}

bool BravePageLoadMetricsEmbedder::IsNewTabPageUrl(const GURL& url) {
  Profile* profile =
      Profile::FromBrowserContext(web_contents()->GetBrowserContext());
  if (!profile)
    return false;
  return search::IsInstantNTPURL(url, profile);
}

bool BravePageLoadMetricsEmbedder::IsPrerender(
    content::WebContents* web_contents) {
  return prerender::PrerenderContents::FromWebContents(web_contents);
}

bool BravePageLoadMetricsEmbedder::IsExtensionUrl(const GURL& url) {
#if BUILDFLAG(ENABLE_EXTENSIONS)
  return url.SchemeIs(extensions::kExtensionScheme);
#else
  return false;
#endif
}

}  // namespace page_load_metrics

}  // namespace brave
