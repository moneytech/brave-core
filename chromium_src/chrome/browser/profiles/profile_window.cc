/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "../../../../../../chrome/browser/profiles/profile_window.cc"

#include "brave/browser/profiles/brave_profile_manager.h"

namespace profiles {

void OpenBrowserWindowForTorProfile(ProfileManager::CreateCallback callback,
                                    bool always_create,
                                    bool is_new_profile,
                                    bool unblock_extensions,
                                    Profile* profile,
                                    Profile::CreateStatus status);

#if !defined(OS_ANDROID)
void SwitchToTorProfile(ProfileManager::CreateCallback callback) {
  const base::FilePath& path = BraveProfileManager::GetTorProfilePath();
  // TODO(darkdh): profile metrics for tor
  // ProfileMetrics::LogProfileSwitch(ProfileMetrics::SWITCH_PROFILE_GUEST,
  //                                  g_browser_process->profile_manager(),
  //                                  path);
  g_browser_process->profile_manager()->CreateProfileAsync(
      path,
      base::Bind(&profiles::OpenBrowserWindowForTorProfile, callback, false,
                 false, false),
      base::string16(), std::string());
}
#endif

void CloseTorProfileWindows() {
  ProfileManager* profile_manager = g_browser_process->profile_manager();
  Profile* profile = profile_manager->GetProfileByPath(
      BraveProfileManager::GetTorProfilePath());

  if (profile) {
    BrowserList::CloseAllBrowsersWithProfile(
        profile, base::Bind(&ProfileBrowserCloseSuccess),
        BrowserList::CloseCallback(), false);
  }
}

void OpenBrowserWindowForTorProfile(ProfileManager::CreateCallback callback,
                                    bool always_create,
                                    bool is_new_profile,
                                    bool unblock_extensions,
                                    Profile* profile,
                                    Profile::CreateStatus status) {
  profiles::OpenBrowserWindowForProfile(
      callback, always_create, is_new_profile, unblock_extensions,
      profile->GetOffTheRecordProfile(), status);
}

}  // namespace profiles
