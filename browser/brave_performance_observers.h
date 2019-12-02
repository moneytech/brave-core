/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_PERFORMANCE_OBSERVERS_H_
#define BRAVE_BROWSER_BRAVE_PERFORMANCE_OBSERVERS_H_

#include "chrome/browser/page_load_metrics/page_load_tracker.h"

namespace page_load_metrics {
class PageLoadTracker;
}

namespace brave {

void RegisterPerformanceObservers(page_load_metrics::PageLoadTracker* tracker);

}  // namespace brave

#endif  // BRAVE_BROWSER_BRAVE_PERFORMANCE_OBSERVERS_H_

