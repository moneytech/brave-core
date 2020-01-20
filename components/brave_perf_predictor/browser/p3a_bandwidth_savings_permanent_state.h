/* Copyright 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_PERF_PREDICTOR_BROWSER_P3A_BANDWIDTH_SAVINGS_PERMANENT_STATE_H_
#define BRAVE_COMPONENTS_BRAVE_PERF_PREDICTOR_BROWSER_P3A_BANDWIDTH_SAVINGS_PERMANENT_STATE_H_

#include <list>

#include "base/timer/timer.h"

class PrefService;

namespace brave_perf_predictor {

class P3ABandwidthSavingsPermanentState {
 public:
  explicit P3ABandwidthSavingsPermanentState(PrefService* user_prefs);
  ~P3ABandwidthSavingsPermanentState();

  // disallow copying
  P3ABandwidthSavingsPermanentState(const P3ABandwidthSavingsPermanentState&) =
      delete;
  P3ABandwidthSavingsPermanentState& operator=(
      const P3ABandwidthSavingsPermanentState&) = delete;

  void AddSavings(uint64_t delta);

 private:
  struct DailySaving {
    base::Time day;
    uint64_t saving;
  };
  void LoadSavingsDaily();
  void SaveSavingsDaily();
  void RecordSavingsTotal();
  uint64_t GetSavingsTotal() const;

  std::list<DailySaving> daily_savings_;
  PrefService* user_prefs_ = nullptr;
};

}  // namespace brave_perf_predictor

#endif  // BRAVE_COMPONENTS_BRAVE_PERF_PREDICTOR_BROWSER_P3A_BANDWIDTH_SAVINGS_PERMANENT_STATE_H_
