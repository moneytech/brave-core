/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_perf_predictor/browser/perf_predictor_tab_helper.h"

#include "base/memory/singleton.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/sessions/session_tab_helper.h"
#if !defined(OS_ANDROID)
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"
#endif
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_registry_simple.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_user_data.h"
#include "content/public/common/resource_type.h"
#include "brave/common/pref_names.h"
#include "brave/components/brave_perf_predictor/browser/third_party_extractor.h"
#include "brave/components/brave_perf_predictor/browser/third_parties.h"

using content::ResourceType;
using base::Singleton;
using content::WebContents;

namespace brave_perf_predictor {

class ThirdPartyExtractor;

PerfPredictorTabHelper::PerfPredictorTabHelper(
    content::WebContents* web_contents)
    : WebContentsObserver(web_contents),
      tab_id_(SessionTabHelper::IdForTab(web_contents)) {
  if (!tab_id_.is_valid()) {
    return;
  }

  Profile* profile = Profile::FromBrowserContext(
    web_contents->GetBrowserContext());
  if (!profile || profile->IsOffTheRecord()) {
    return;
  }

  ThirdPartyExtractor* extractor = ThirdPartyExtractor::GetInstance();
  if (!extractor->is_initialized()) {
    extractor->load_entities(static_third_party_config);
  }
  bandwidth_predictor_ = new BandwidthSavingsPredictor(extractor);
}

PerfPredictorTabHelper::~PerfPredictorTabHelper() {
  if (bandwidth_predictor_) {
    delete bandwidth_predictor_;
  }
}

void PerfPredictorTabHelper::DidStartNavigation(
    content::NavigationHandle* handle) {
  if (!handle->IsInMainFrame() || handle->IsDownload()) {
    return;
  }
  // Gather prediction of the _previous_ navigation
  if (navigation_id_ != 0) {
    RecordSaving();
  }
}

void PerfPredictorTabHelper::ReadyToCommitNavigation(
    content::NavigationHandle* handle) {
  if (!handle->IsInMainFrame() || handle->IsDownload()) {
    return;
  }
  // Reset predictor state when we're committed to this navigation
  bandwidth_predictor_->Reset();
  navigation_id_ = handle->GetNavigationId();
  VLOG(2) << tab_id_
    << " committed navigation ID " << navigation_id_
    << " to " << handle->GetURL().GetContent();
}

void PerfPredictorTabHelper::DidFinishNavigation(
    content::NavigationHandle* handle) {
  if (!handle->IsInMainFrame() ||
      !handle->HasCommitted() ||
      handle->IsDownload()) {
    return;
  }

  main_frame_url_ = handle->GetURL();
}

void PerfPredictorTabHelper::RecordSaving() {
  if (bandwidth_predictor_ && web_contents()) {
    uint64_t saving = (uint64_t)bandwidth_predictor_->predict();
    if (saving > 0) {
      PrefService* prefs = Profile::FromBrowserContext(
        web_contents()->GetBrowserContext())->
        GetOriginalProfile()->
        GetPrefs();

      VLOG(2) << "Store bandwidth saving of " << saving << " Bytes to prefs";
      prefs->SetUint64(
        kBandwidthSavedBytes,
        prefs->GetUint64(kBandwidthSavedBytes) + saving);
    }
  }
}

void PerfPredictorTabHelper::ResourceLoadComplete(
    content::RenderFrameHost* render_frame_host,
    const content::GlobalRequestID& request_id,
    const content::mojom::ResourceLoadInfo& resource_load_info) {
  if (render_frame_host && bandwidth_predictor_) {
    bandwidth_predictor_->OnResourceLoadComplete(
      main_frame_url_, resource_load_info);
  }
}

void PerfPredictorTabHelper::OnBlockedSubresource(
    const std::string& subresource) {
  if (bandwidth_predictor_) {
    bandwidth_predictor_->OnSubresourceBlocked(subresource);
  }
}

void PerfPredictorTabHelper::DidAttachInterstitialPage() {
  // web contents unloaded
  if (bandwidth_predictor_) {
    // Predict to clear the state, but don't save the result
    bandwidth_predictor_->Reset();
  }
}

void PerfPredictorTabHelper::WebContentsDestroyed() {
  // Run a prediction when Web Contents get destroyed (e.g. tab/window closed)
  RecordSaving();
}

void PerfPredictorTabHelper::OnPageLoadTimingUpdated(
    const page_load_metrics::mojom::PageLoadTiming& timing) {
  if (bandwidth_predictor_) {
    bandwidth_predictor_->OnPageLoadTimingUpdated(timing);
  }
}

// static
void PerfPredictorTabHelper::RegisterProfilePrefs(
    PrefRegistrySimple* registry) {
  registry->RegisterUint64Pref(kBandwidthSavedBytes, 0);
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(PerfPredictorTabHelper)

}  // namespace brave_perf_predictor
