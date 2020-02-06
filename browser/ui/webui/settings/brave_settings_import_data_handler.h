/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_SETTINGS_IMPORT_DATA_HANDLER_H_
#define BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_SETTINGS_IMPORT_DATA_HANDLER_H_

#include "base/memory/weak_ptr.h"
#include "chrome/browser/ui/webui/settings/settings_import_data_handler.h"

namespace settings {

class BraveImportDataHandler : public ImportDataHandler {
 public:
  BraveImportDataHandler();
  ~BraveImportDataHandler() override;

  BraveImportDataHandler(const BraveImportDataHandler&) = delete;
  BraveImportDataHandler& operator=(const BraveImportDataHandler&) = delete;

 private:
  // ImportDataHandler overrides:
  void StartImport(const importer::SourceProfile& source_profile,
                   uint16_t imported_items) override;

  void OnGetDiskAccessPermission(const importer::SourceProfile& source_profile,
                                 uint16_t imported_items,
                                 bool allowed);

  base::WeakPtrFactory<BraveImportDataHandler> weak_factory_;
};

}  // namespace settings

#endif  // BRAVE_BROWSER_UI_WEBUI_SETTINGS_BRAVE_SETTINGS_IMPORT_DATA_HANDLER_H_
