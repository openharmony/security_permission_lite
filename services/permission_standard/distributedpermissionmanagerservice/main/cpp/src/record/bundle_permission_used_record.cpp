/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bundle_permission_used_record.h"

using namespace std;

namespace OHOS {
namespace Security {
namespace Permission {
bool BundlePermissionUsedRecord::Marshalling(Parcel &out) const
{
    if (!out.WriteString(this->deviceId)) {
        return false;
    }
    if (!out.WriteString(this->deviceLabel)) {
        return false;
    }
    if (!out.WriteString(this->bundleName)) {
        return false;
    }
    if (!out.WriteString(this->bundleLabel)) {
        return false;
    }
    if (!out.WriteInt32(this->permissionUsedRecords.size())) {
        return false;
    }
    for (const auto &record : this->permissionUsedRecords) {
        if (!out.WriteParcelable(&record)) {
            return false;
        }
    }
    return true;
}

BundlePermissionUsedRecord *BundlePermissionUsedRecord::Unmarshalling(Parcel &in)
{
    auto *bundleRecord = new (nothrow) BundlePermissionUsedRecord();
    if (bundleRecord == nullptr) {
        delete bundleRecord;
        bundleRecord = nullptr;
        return bundleRecord;
    }
    bundleRecord->deviceId = in.ReadString();
    bundleRecord->deviceLabel = in.ReadString();
    bundleRecord->bundleName = in.ReadString();
    bundleRecord->bundleLabel = in.ReadString();
    bundleRecord->applicationIconId = in.ReadInt32();
    int size = 0;
    if (!in.ReadInt32(size)) {
        delete bundleRecord;
        bundleRecord = nullptr;
        return bundleRecord;
    }
    for (int i = 0; i < size; i++) {
        sptr<PermissionUsedRecord> record = in.ReadParcelable<PermissionUsedRecord>();
        if (record == nullptr) {
            delete bundleRecord;
            bundleRecord = nullptr;
            return bundleRecord;
        }
        bundleRecord->permissionUsedRecords.push_back(*record);
    }
    return bundleRecord;
}
size_t BundlePermissionUsedRecord::GetDataSize() const
{
    size_t size = sizeof(int32_t) + deviceId.size() * sizeof(char16_t);
    size += sizeof(int32_t) + deviceLabel.size() * sizeof(char16_t);
    size += sizeof(int32_t) + bundleName.size() * sizeof(char16_t);
    size += sizeof(int32_t) + bundleLabel.size() * sizeof(char16_t);
    size += sizeof(int32_t);
    for (const auto &record : permissionUsedRecords) {
        size += record.GetDataSize();
    }
    return size;
}

nlohmann::json BundlePermissionUsedRecord::to_json(const BundlePermissionUsedRecord &bundle) const
{
    nlohmann::json jsonRecord;
    for (auto record : bundle.permissionUsedRecords) {
        jsonRecord.emplace_back(record.to_json(record));
    }
    nlohmann::json jsonObj = nlohmann::json{
        {"di", bundle.deviceId},
        {"dl", bundle.deviceLabel},
        {"bn", bundle.bundleName},
        {"bl", bundle.bundleLabel},
        {"ai", bundle.applicationIconId},
        {"pr", jsonRecord},
    };
    return jsonObj;
}

void BundlePermissionUsedRecord::from_json(const nlohmann::json &jsonObj, BundlePermissionUsedRecord &bundle)
{
    jsonObj.at("di").get_to(bundle.deviceId);
    jsonObj.at("dl").get_to(bundle.deviceLabel);
    jsonObj.at("bn").get_to(bundle.bundleName);
    jsonObj.at("bl").get_to(bundle.bundleLabel);
    jsonObj.at("ai").get_to(bundle.applicationIconId);
    std::vector<PermissionUsedRecord> recordVector;
    nlohmann::json permissionUsedRecords = jsonObj.at("pr").get<nlohmann::json>();
    for (auto record : permissionUsedRecords) {
        PermissionUsedRecord tempRecord;
        tempRecord.from_json(record, tempRecord);
        recordVector.emplace_back(tempRecord);
    }
    bundle.permissionUsedRecords = recordVector;
}

int BundlePermissionUsedRecord::TranslationIntoBundlePermissionUsedRecord(
    const GenericValues &inGenericValues, BundlePermissionUsedRecord &outBundleRecord)
{
    outBundleRecord.deviceId = inGenericValues.GetString(FIELD_DEVICE_ID);
    outBundleRecord.deviceLabel = inGenericValues.GetString(FIELD_DEVICE_NAME);
    outBundleRecord.bundleName = inGenericValues.GetString(FIELD_BUNDLE_NAME);
    outBundleRecord.bundleLabel = inGenericValues.GetString(FIELD_BUNDLE_LABEL);
    return Constant::SUCCESS;
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS