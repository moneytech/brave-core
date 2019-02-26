#include "components/sync/engine_impl/get_updates_processor.h"

namespace syncer {
namespace {
SyncerError ApplyBraveRecords(sync_pb::ClientToServerResponse*, ModelTypeSet*,
                              std::unique_ptr<brave_sync::RecordsList>);
}   // namespace
}   // namespace syncer
#include "../../../../../components/sync/engine_impl/get_updates_processor.cc"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/time/time.h"
#include "brave/components/brave_sync/jslib_messages.h"
#include "components/sync/base/hash_util.h"
#include "components/sync/syncable/syncable_proto_util.h"
#include "components/sync/engine_impl/loopback_server/loopback_server_entity.h"
#include "url/gurl.h"

namespace syncer {
namespace {

using brave_sync::jslib::Bookmark;
using brave_sync::jslib::SyncRecord;
using syncable::Id;
static const char kBookmarkBarTag[] = "bookmark_bar";
static const char kOtherBookmarksTag[] = "other_bookmarks";
static const char kBookmarkBarFolderName[] = "Bookmark Bar";
static const char kOtherBookmarksFolderName[] = "Other Bookmarks";
// The parent tag for children of the root entity. Entities with this parent are
// referred to as top level enities.
static const char kRootParentTag[] = "0";

uint64_t GetIndexByOrder(const std::string& record_order) {
  uint64_t index = 0;
  char last_ch = record_order.back();
  bool result = base::StringToUint64(std::string(&last_ch), &index);
  --index;
  DCHECK(index >= 0);
  DCHECK(result);
  return index;
}

void AddBookmarkSpecifics(sync_pb::EntitySpecifics* specifics,
                          const SyncRecord* record) {
  DCHECK(specifics);
  auto bookmark = record->GetBookmark();
  sync_pb::BookmarkSpecifics* bm_specifics = specifics->mutable_bookmark();
  bm_specifics->set_url(bookmark.site.location);
  bm_specifics->set_title(bookmark.site.title);
  bm_specifics->set_creation_time_us(
    TimeToProtoTime(bookmark.site.creationTime));
  bm_specifics->set_icon_url(bookmark.site.favicon);
      // base::Time::Now().ToDeltaSinceWindowsEpoch().InMicroseconds());
  sync_pb::MetaInfo* meta_info = bm_specifics->add_meta_info();
  meta_info->set_key("order");
  meta_info->set_value(bookmark.order);
  // For GetExistingObjects lookup
  meta_info = bm_specifics->add_meta_info();
  meta_info->set_key("object_id");
  meta_info->set_value(record->objectId);
  meta_info = bm_specifics->add_meta_info();
  meta_info->set_key("parent_object_id");
  meta_info->set_value(bookmark.parentFolderObjectId);
  meta_info = bm_specifics->add_meta_info();
  meta_info->set_key("sync_timestamp");
  meta_info->set_value(std::to_string(record->syncTimestamp.ToJsTime()));
}

void ExtractBookmarkMeta(sync_pb::SyncEntity* entity,
                         sync_pb::EntitySpecifics* specifics,
                         const Bookmark& bookmark) {
  sync_pb::BookmarkSpecifics* bm_specifics = specifics->mutable_bookmark();
  for (const auto metaInfo : bookmark.metaInfo) {
    if (metaInfo.key == "originator_cache_guid") {
      entity->set_originator_cache_guid(metaInfo.value);
      sync_pb::MetaInfo* meta_info = bm_specifics->add_meta_info();
      meta_info->set_key("originator_cache_guid");
      meta_info->set_value(metaInfo.value);
    } else if (metaInfo.key == "originator_client_item_id") {
      entity->set_originator_client_item_id(metaInfo.value);
      sync_pb::MetaInfo* meta_info = bm_specifics->add_meta_info();
      meta_info->set_key("originator_client_item_id");
      meta_info->set_value(metaInfo.value);
    } else if (metaInfo.key == "version") {
      int64_t version;
      bool result = base::StringToInt64(metaInfo.value, &version);
      DCHECK(result);
      entity->set_version(version + 1);
    } else if (metaInfo.key == "mtime") {
      int64_t mtime;
      bool result = base::StringToInt64(metaInfo.value, &mtime);
      DCHECK(result);
      entity->set_mtime(mtime);
    } else if (metaInfo.key == "ctime") {
      int64_t ctime;
      bool result = base::StringToInt64(metaInfo.value, &ctime);
      DCHECK(result);
      entity->set_ctime(ctime);
    }
  }
}

void AddRootForType(sync_pb::SyncEntity* entity, ModelType type) {
  DCHECK(entity);
  sync_pb::EntitySpecifics specifics;
  AddDefaultFieldValue(type, &specifics);
  std::string server_tag = ModelTypeToRootTag(type);
  std::string name = syncer::ModelTypeToString(type);
  std::string id = LoopbackServerEntity::GetTopLevelId(type);
  entity->set_server_defined_unique_tag(server_tag);
  entity->set_id_string(id);
  entity->set_parent_id_string(kRootParentTag);
  entity->set_name(name);
  entity->set_version(1);
  entity->set_folder(true);
  entity->mutable_specifics()->CopyFrom(specifics);
}

void AddPermanentNode(sync_pb::SyncEntity* entity, const std::string& name, const std::string& tag) {
  DCHECK(entity);
  sync_pb::EntitySpecifics specifics;
  AddDefaultFieldValue(BOOKMARKS, &specifics);
  std::string parent = ModelTypeToRootTag(BOOKMARKS);
  std::string id = tag;
  std::string parent_id = LoopbackServerEntity::CreateId(BOOKMARKS, parent);
  entity->set_server_defined_unique_tag(tag);
  entity->set_id_string(id);
  entity->set_parent_id_string(parent_id);
  entity->set_name(name);
  entity->set_folder(true);
  entity->set_version(1);
  entity->mutable_specifics()->CopyFrom(specifics);
}

void AddBookmarkNode(sync_pb::SyncEntity* entity, const SyncRecord* record) {
  DCHECK(entity);
  DCHECK(record);
  DCHECK(record->has_bookmark());
  DCHECK(!record->objectId.empty());

  auto bookmark_record = record->GetBookmark();

  sync_pb::EntitySpecifics specifics;
  AddDefaultFieldValue(BOOKMARKS, &specifics);
  entity->set_id_string(record->objectId);
  if (!bookmark_record.parentFolderObjectId.empty())
    entity->set_parent_id_string(bookmark_record.parentFolderObjectId);
  else if (!bookmark_record.hideInToolbar)
    entity->set_parent_id_string(std::string(kBookmarkBarTag));
  else
    entity->set_parent_id_string(std::string(kOtherBookmarksTag));
  entity->set_non_unique_name(bookmark_record.site.title);
  entity->set_folder(bookmark_record.isFolder);

  ExtractBookmarkMeta(entity, &specifics, bookmark_record);

  // TODO(darkdh): migrate to UniquePosition
  entity->set_position_in_parent(GetIndexByOrder(bookmark_record.order));
  std::string suffix = GenerateSyncableBookmarkHash(entity->originator_cache_guid(),
                                                    entity->originator_client_item_id());
  UniquePosition uniquePosition = UniquePosition::FromInt64(GetIndexByOrder(bookmark_record.order),
                                                            suffix);
  auto* unique_position_pb = entity->mutable_unique_position();
  *unique_position_pb = uniquePosition.ToProto();
  if (record->action == SyncRecord::Action::A_DELETE)
    entity->set_deleted(true);
  else
    AddBookmarkSpecifics(&specifics, record);
  entity->mutable_specifics()->CopyFrom(specifics);
}

void ConstructUpdateResponse(sync_pb::GetUpdatesResponse* gu_response,
                             ModelTypeSet* request_types,
                             std::unique_ptr<RecordsList> records) {
  DCHECK(gu_response);
  DCHECK(request_types);
  for (ModelType type : *request_types) {
    sync_pb::DataTypeProgressMarker* marker =
      gu_response->add_new_progress_marker();
    marker->set_data_type_id(GetSpecificsFieldNumberFromModelType(type));
    marker->set_token("token");
    if (type == BOOKMARKS) {
      google::protobuf::RepeatedPtrField<sync_pb::SyncEntity> entities;
      AddRootForType(entities.Add(), BOOKMARKS);
      AddPermanentNode(entities.Add(), kBookmarkBarFolderName, kBookmarkBarTag);
      AddPermanentNode(entities.Add(), kOtherBookmarksFolderName,
                       kOtherBookmarksTag);
      if (records) {
        for (const auto& record : *records.get()) {
          AddBookmarkNode(entities.Add(), record.get());
        }
      }
      std::copy(entities.begin(), entities.end(),
                RepeatedPtrFieldBackInserter(gu_response->mutable_entries()));
    }
    gu_response->set_changes_remaining(0);
  }
}

SyncerError ApplyBraveRecords(sync_pb::ClientToServerResponse* update_response,
                              ModelTypeSet* request_types,
                              std::unique_ptr<RecordsList> records) {
  DCHECK(update_response);
  DCHECK(request_types);
  sync_pb::GetUpdatesResponse* gu_response = new sync_pb::GetUpdatesResponse();
  ConstructUpdateResponse(gu_response, request_types, std::move(records));
  update_response->set_allocated_get_updates(gu_response);
  return SyncerError(SyncerError::SYNCER_OK);
}

}   // namespace

void GetUpdatesProcessor::AddBraveRecords(
    std::unique_ptr<RecordsList> records) {
  brave_records_ = std::move(records);
}

}   // namespace syncer
