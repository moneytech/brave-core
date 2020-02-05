/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_perf_predictor/browser/third_party_extractor.h"

#include "base/containers/flat_set.h"
#include "base/json/json_reader.h"
#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
#include "base/values.h"
#include "brave/components/brave_perf_predictor/browser/bandwidth_linreg_parameters.h"
#include "components/grit/brave_components_resources.h"
#include "third_party/re2/src/re2/re2.h"
#include "ui/base/resource/resource_bundle.h"

namespace brave_perf_predictor {

namespace {

RE2 DOMAIN_IN_URL_REGEX(":\\/\\/(.*?)(\\/|$)");
RE2 DOMAIN_CHARACTERS("([a-z0-9.-]+\\.[a-z0-9]+)");
RE2 ROOT_DOMAIN_REGEX("([^.]+\\.([^.]+|(gov|com|co|ne)\\.\\w{2})$)");

base::Optional<std::string> GetDomainFromOriginOrUrl(
    const std::string origin_or_url) {
  std::string domain;
  if (RE2::PartialMatch(origin_or_url, DOMAIN_IN_URL_REGEX, &domain))
    return domain;
  if (RE2::PartialMatch(origin_or_url, DOMAIN_CHARACTERS, &domain))
    return domain;
  return base::nullopt;
}

std::string GetRootDomain(const std::string domain) {
  std::string root_domain;
  if (RE2::PartialMatch(domain, ROOT_DOMAIN_REGEX, &root_domain)) {
    return root_domain;
  } else {
    return domain;
  }
}

}  // namespace

// static
ThirdPartyExtractor* ThirdPartyExtractor::GetInstance() {
  auto* extractor = base::Singleton<ThirdPartyExtractor>::get();
  // By default initialize from packaged resources
  if (!extractor->IsInitialized()) {
    bool initialized = extractor->InitializeFromResource();
    if (!initialized) {
      VLOG(2) << "Initialization from resource failed, marking as initialized "
              << "will not retry";
      extractor->MarkInitialized(true);
    }
  }
  return extractor;
}

bool ThirdPartyExtractor::LoadEntities(const base::StringPiece entities) {
  // Reset previous mappings
  entity_by_domain_.clear();
  entity_by_root_domain_.clear();

  // Parse the JSON
  base::Optional<base::Value> document = base::JSONReader::Read(entities);
  if (!document || !document->is_list()) {
    LOG(ERROR) << "Cannot parse the third-party entities list";
    return false;
  }

  // Collect the mappings
  for (auto& entity : document->GetList()) {
    const std::string* entity_name = entity.FindStringPath("name");
    if (!entity_name)
      continue;
    if (!relevant_entity_set.contains(*entity_name)) {
      VLOG(3) << "Irrelevant entity " << *entity_name;
      continue;
    }
    const auto* entity_domains = entity.FindListPath("domains");
    if (!entity_domains)
      continue;

    for (auto& entity_domain_it : entity_domains->GetList()) {
      if (!entity_domain_it.is_string()) {
        continue;
      }
      const base::StringPiece entity_domain(entity_domain_it.GetString());

      const auto inserted =
          entity_by_domain_.emplace(entity_domain, *entity_name);
      if (!inserted.second) {
        VLOG(2) << "Malformed data: duplicate domain " << entity_domain;
      }
      auto root_domain = GetRootDomain(entity_domain.as_string());

      auto root_entity_entry = entity_by_root_domain_.find(root_domain);
      if (root_entity_entry != entity_by_root_domain_.end() &&
          root_entity_entry->second != *entity_name) {
        // If there is a clash at root domain level, neither is correct
        entity_by_root_domain_.erase(root_entity_entry);
      } else {
        entity_by_root_domain_.emplace(root_domain, *entity_name);
      }
    }
  }

  entity_by_domain_.shrink_to_fit();
  entity_by_root_domain_.shrink_to_fit();
  initialized_ = true;

  return true;
}

base::Optional<std::string> ThirdPartyExtractor::GetEntity(
    const base::StringPiece origin_or_url) const {
  base::Optional<std::string> domain =
      GetDomainFromOriginOrUrl(origin_or_url.as_string());
  if (domain.has_value()) {
    auto domain_entry = entity_by_domain_.find(domain.value());
    if (domain_entry != entity_by_domain_.end())
      return domain_entry->second;

    auto root_domain = GetRootDomain(domain.value());
    auto root_domain_entry = entity_by_root_domain_.find(root_domain);
    if (root_domain_entry != entity_by_root_domain_.end())
      return root_domain_entry->second;
  }

  return base::nullopt;
}

ThirdPartyExtractor::ThirdPartyExtractor() = default;

ThirdPartyExtractor::~ThirdPartyExtractor() = default;

bool ThirdPartyExtractor::InitializeFromResource() {
  const auto resource_id = IDR_THIRD_PARTY_ENTITIES;

  auto& resource_bundle = ui::ResourceBundle::GetSharedInstance();
  std::string data_resource;
  if (resource_bundle.IsGzipped(resource_id)) {
    data_resource = resource_bundle.DecompressDataResource(resource_id);
  } else {
    data_resource = resource_bundle.GetRawDataResource(resource_id).as_string();
  }

  return LoadEntities(data_resource);
}

}  // namespace brave_perf_predictor
