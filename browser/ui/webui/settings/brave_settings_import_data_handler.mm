/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/settings/brave_settings_import_data_handler.h"

#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/mac/foundation_util.h"
#include "base/task/post_task.h"
#include "base/task/task_traits.h"
#include "base/values.h"
#include "chrome/browser/importer/external_process_importer_host.h"
#include "chrome/common/importer/importer_data_types.h"
#include "content/public/browser/browser_thread.h"

namespace {

bool HasProperDiskAccessPermission(uint16_t imported_items) {
  DCHECK(imported_items);

  const base::FilePath& library_dir = base::mac::GetUserLibraryPath();
  const base::FilePath safari_dir = library_dir.Append("Safari");

  if (imported_items & importer::FAVORITES) {
    const base::FilePath bookmarks_path = safari_dir.Append("Bookmarks.plist");
    if(!PathIsWritable(bookmarks_path)) {
      LOG(ERROR) << __func__ << " " << bookmarks_path << " is not accessible."
                 << " Please check full disk access permission.";
      return false;
    }
  }

  if (imported_items & importer::HISTORY) {
    const base::FilePath history_path = safari_dir.Append("History.plist");
    if(!PathIsWritable(history_path)) {
      LOG(ERROR) << __func__ << " " << history_path << " is not accessible."
                 << " Please check full disk access permission.";
      return false;
    }
  }

  return true;
}

}  // namespace

using content::BrowserThread;

namespace settings {

BraveImportDataHandler::BraveImportDataHandler() : weak_factory_(this) {}
BraveImportDataHandler::~BraveImportDataHandler() = default;

void BraveImportDataHandler::StartImport(
    const importer::SourceProfile& source_profile,
    uint16_t imported_items) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);

  if (!imported_items)
    return;

  if (source_profile.importer_type == importer::TYPE_SAFARI) {
    // Start import if Brave has full disk access permission.
    // If not, show dialog that has infos about that permission.
    base::PostTaskAndReplyWithResult(
        FROM_HERE, {base::ThreadPool(), base::MayBlock()},
        base::BindOnce(&HasProperDiskAccessPermission, imported_items),
        base::BindOnce(&BraveImportDataHandler::OnGetDiskAccessPermission,
                       weak_factory_.GetWeakPtr(),
                       source_profile, imported_items));
    return;
  }

  ImportDataHandler::StartImport(source_profile, imported_items);
}

void BraveImportDataHandler::OnGetDiskAccessPermission(
    const importer::SourceProfile& source_profile,
    uint16_t imported_items,
    bool allowed) {
  if (!allowed) {
    FireWebUIListener("import-data-status-changed", base::Value("failed"));
    // TODO(simonhong): Launch dialog.
    return;
  }

  return ImportDataHandler::StartImport(source_profile, imported_items);
}

}  // namespace settings
