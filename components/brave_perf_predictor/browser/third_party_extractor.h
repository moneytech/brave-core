/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_PERF_PREDICTOR_BROWSER_THIRD_PARTY_EXTRACTOR_H_
#define BRAVE_COMPONENTS_BRAVE_PERF_PREDICTOR_BROWSER_THIRD_PARTY_EXTRACTOR_H_

#include <string>
#include <unordered_map>

#include "base/memory/singleton.h"
#include "base/values.h"

namespace brave_perf_predictor {

class ThirdPartyExtractor {
 public:
  static ThirdPartyExtractor* GetInstance();

  bool LoadEntities(const std::string& entities);
  base::Optional<std::string> GetEntity(const std::string& domain);

 private:
  ThirdPartyExtractor();
  ~ThirdPartyExtractor();

  bool IsInitialized() { return initialized_; }
  bool InitializeFromResource();

  friend struct base::DefaultSingletonTraits<ThirdPartyExtractor>;
  bool initialized_ = false;
  std::unordered_map<std::string, std::string> entity_by_domain_;
  std::unordered_map<std::string, std::string> entity_by_root_domain_;

  DISALLOW_COPY_AND_ASSIGN(ThirdPartyExtractor);
};

}  // namespace brave_perf_predictor

#endif  // BRAVE_COMPONENTS_BRAVE_PERF_PREDICTOR_BROWSER_THIRD_PARTY_EXTRACTOR_H_
