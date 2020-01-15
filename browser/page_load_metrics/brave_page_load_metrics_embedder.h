/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_PAGE_LOAD_METRICS_BRAVE_PAGE_LOAD_METRICS_EMBEDDER_H_
#define BRAVE_BROWSER_PAGE_LOAD_METRICS_BRAVE_PAGE_LOAD_METRICS_EMBEDDER_H_

#include <memory>

#include "components/page_load_metrics/browser/page_load_metrics_embedder_base.h"
#include "components/page_load_metrics/browser/page_load_metrics_embedder_interface.h"
#include "components/page_load_metrics/browser/page_load_tracker.h"

namespace brave {

namespace page_load_metrics {

class BravePageLoadMetricsEmbedder
    : public ::page_load_metrics::PageLoadMetricsEmbedderBase {
 public:
  BravePageLoadMetricsEmbedder(
      content::WebContents* web_contents,
      std::unique_ptr<::page_load_metrics::PageLoadMetricsEmbedderInterface>
          base_embedder);
  ~BravePageLoadMetricsEmbedder() override;

  // page_load_metrics::PageLoadMetricsEmbedderBase:
  bool IsNewTabPageUrl(const GURL& url) override;
  bool IsPrerender(content::WebContents* web_contents) override;
  bool IsExtensionUrl(const GURL& url) override;

 protected:
  // page_load_metrics::PageLoadMetricsEmbedderBase:
  void RegisterEmbedderObservers(
      ::page_load_metrics::PageLoadTracker* tracker) override;
  bool IsPrerendering() const override;

 private:
  std::unique_ptr<::page_load_metrics::PageLoadMetricsEmbedderInterface>
      base_embedder_;
  DISALLOW_COPY_AND_ASSIGN(BravePageLoadMetricsEmbedder);
};
}  // namespace page_load_metrics

}  // namespace brave

#endif  // BRAVE_BROWSER_PAGE_LOAD_METRICS_BRAVE_PAGE_LOAD_METRICS_EMBEDDER_H_
