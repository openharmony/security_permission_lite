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

#include "permission_used_record.h"

using namespace std;

namespace OHOS {
namespace Security {
namespace Permission {
bool PermissionUsedRecord::Marshalling(Parcel &out) const
{
    if (!out.WriteString(this->permissionName)) {
        return false;
    }
    if (!out.WriteInt32(this->accessCountFg)) {
        return false;
    }
    if (!out.WriteInt32(this->rejectCountFg)) {
        return false;
    }
    if (!out.WriteInt32(this->accessCountBg)) {
        return false;
    }
    if (!out.WriteInt32(this->rejectCountBg)) {
        return false;
    }
    if (!out.WriteInt64(this->lastAccessTime)) {
        return false;
    }
    if (!out.WriteInt64(this->lastRejectTime)) {
        return false;
    }
    if (!out.WriteInt64Vector(this->accessRecordFg)) {
        return false;
    }
    if (!out.WriteInt64Vector(this->rejectRecordFg)) {
        return false;
    }
    if (!out.WriteInt64Vector(this->accessRecordBg)) {
        return false;
    }
    if (!out.WriteInt64Vector(this->rejectRecordBg)) {
        return false;
    }
    return true;
}

PermissionUsedRecord *PermissionUsedRecord::Unmarshalling(Parcel &in)
{
    auto *record = new (nothrow) PermissionUsedRecord();
    if (record == nullptr) {
        delete record;
        record = nullptr;
        return record;
    }
    record->permissionName = in.ReadString();
    if (!in.ReadInt32(record->accessCountFg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt32(record->rejectCountFg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt32(record->accessCountBg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt32(record->rejectCountBg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt64(record->lastAccessTime)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt64(record->lastRejectTime)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt64Vector(&record->accessRecordFg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt64Vector(&record->rejectRecordFg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt64Vector(&record->accessRecordBg)) {
        delete record;
        record = nullptr;
        return record;
    }
    if (!in.ReadInt64Vector(&record->rejectRecordBg)) {
        delete record;
        record = nullptr;
        return record;
    }
    return record;
}

size_t PermissionUsedRecord::GetDataSize() const
{
    size_t size = sizeof(int32_t) + permissionName.size() * sizeof(char16_t);
    size += sizeof(int32_t);
    size += sizeof(int32_t);
    size += sizeof(int32_t);
    size += sizeof(int32_t);
    size += sizeof(int64_t);
    size += sizeof(int64_t);
    size += sizeof(int32_t) + accessRecordFg.size() * sizeof(int64_t);
    size += sizeof(int32_t) + rejectRecordFg.size() * sizeof(int64_t);
    size += sizeof(int32_t) + accessRecordBg.size() * sizeof(int64_t);
    size += sizeof(int32_t) + rejectRecordBg.size() * sizeof(int64_t);
    return size;
}

nlohmann::json PermissionUsedRecord::to_json(const PermissionUsedRecord &record)
{
    nlohmann::json jsonObj = nlohmann::json{{"pn", record.permissionName},
        {"acf", record.accessCountFg},
        {"rcf", record.rejectCountFg},
        {"acb", record.accessCountBg},
        {"rcb", record.rejectCountBg},
        {"lat", record.lastAccessTime},
        {"lrt", record.lastRejectTime},
        {"arf", record.accessRecordFg},
        {"rrf", record.rejectRecordFg},
        {"arb", record.accessRecordBg},
        {"rrb", record.rejectRecordBg}};
    return jsonObj;
}

void PermissionUsedRecord::from_json(const nlohmann::json &jsonObj, PermissionUsedRecord &record)
{
    if (jsonObj.find("pn") != jsonObj.end() && jsonObj.at("pn").is_string()) {
        jsonObj.at("pn").get_to(record.permissionName);
    }
    if (jsonObj.find("acf") != jsonObj.end() && jsonObj.at("acf").is_number()) {
        jsonObj.at("acf").get_to(record.accessCountFg);
    }
    if (jsonObj.find("rcf") != jsonObj.end() && jsonObj.at("rcf").is_number()) {
        jsonObj.at("rcf").get_to(record.rejectCountFg);
    }
    if (jsonObj.find("acb") != jsonObj.end() && jsonObj.at("acb").is_number()) {
        jsonObj.at("acb").get_to(record.accessCountBg);
    }
    if (jsonObj.find("rcb") != jsonObj.end() && jsonObj.at("rcb").is_number()) {
        jsonObj.at("rcb").get_to(record.rejectCountBg);
    }
    if (jsonObj.find("lat") != jsonObj.end() && jsonObj.at("lat").is_number()) {
        jsonObj.at("lat").get_to(record.lastAccessTime);
    }
    if (jsonObj.find("lrt") != jsonObj.end() && jsonObj.at("lrt").is_number()) {
        jsonObj.at("lrt").get_to(record.lastRejectTime);
    }
    if (jsonObj.find("arf") != jsonObj.end() && jsonObj.at("arf").is_array()) {
        jsonObj.at("arf").get_to(record.accessRecordFg);
    }
    if (jsonObj.find("rrf") != jsonObj.end() && jsonObj.at("rrf").is_array()) {
        jsonObj.at("rrf").get_to(record.rejectRecordFg);
    }
    if (jsonObj.find("arb") != jsonObj.end() && jsonObj.at("arb").is_array()) {
        jsonObj.at("arb").get_to(record.accessRecordBg);
    }
    if (jsonObj.find("rrb") != jsonObj.end() && jsonObj.at("rrb").is_array()) {
        jsonObj.at("rrb").get_to(record.rejectRecordBg);
    }
}

void PermissionUsedRecord::UpdateRecord(const PermissionUsedRecord &record)
{
    this->accessCountFg += record.accessCountFg;
    this->rejectCountFg += record.rejectCountFg;
    this->accessCountBg += record.accessCountBg;
    this->rejectCountBg += record.rejectCountBg;
    this->lastAccessTime =
        (this->lastAccessTime > record.lastAccessTime) ? this->lastAccessTime : record.lastAccessTime;
    this->lastRejectTime =
        (this->lastRejectTime > record.lastRejectTime) ? this->lastRejectTime : record.lastRejectTime;
}

void PermissionUsedRecord::UpdateRecordWithTime(const PermissionUsedRecord &record)
{
    UpdateRecord(record);
    if (record.accessCountFg > 0 || record.rejectCountFg > 0) {
        if (record.lastAccessTime > 0 && this->accessRecordFg.size() <= Constant::MAX_DETAIL_RECORDS) {
            this->accessRecordFg.emplace_back(record.lastAccessTime);
        } else if (record.lastRejectTime > 0 && this->rejectRecordFg.size() <= Constant::MAX_DETAIL_RECORDS) {
            this->rejectRecordFg.emplace_back(record.lastRejectTime);
        }
    } else {
        if (record.lastAccessTime > 0 && this->accessRecordBg.size() <= Constant::MAX_DETAIL_RECORDS) {
            this->accessRecordBg.emplace_back(record.lastAccessTime);
        } else if (record.lastRejectTime > 0 && this->rejectRecordBg.size() <= Constant::MAX_DETAIL_RECORDS) {
            this->rejectRecordBg.emplace_back(record.lastRejectTime);
        }
    }
}

int PermissionUsedRecord::TranslationIntoPermissionUsedRecord(
    const GenericValues &inGenericValues, PermissionUsedRecord &outPermRecord)
{
    std::string perName;
    int32_t code = inGenericValues.GetInt(FIELD_OP_CODE);
    if (!Constant::PermissionNameToOrFromOpCode(perName, code)) {
        return Constant::FAILURE;
    }
    outPermRecord.permissionName = perName;
    if (inGenericValues.GetInt(FIELD_IS_FOREGROUND) == 1) {
        outPermRecord.accessCountFg = inGenericValues.GetInt(FIELD_ACCESS_COUNT);
        outPermRecord.rejectCountFg = inGenericValues.GetInt(FIELD_REJECT_COUNT);
    } else {
        outPermRecord.accessCountBg = inGenericValues.GetInt(FIELD_ACCESS_COUNT);
        outPermRecord.rejectCountBg = inGenericValues.GetInt(FIELD_REJECT_COUNT);
    }
    if (inGenericValues.GetInt(FIELD_ACCESS_COUNT) > inGenericValues.GetInt(FIELD_REJECT_COUNT)) {
        if (inGenericValues.GetInt(FIELD_REJECT_COUNT) != 0) {
            outPermRecord.lastRejectTime = inGenericValues.GetInt64(FIELD_TIMESTAMP);
        }
        outPermRecord.lastAccessTime = inGenericValues.GetInt64(FIELD_TIMESTAMP);
    } else {
        if (inGenericValues.GetInt(FIELD_ACCESS_COUNT) != 0) {
            outPermRecord.lastAccessTime = inGenericValues.GetInt64(FIELD_TIMESTAMP);
        }
        outPermRecord.lastRejectTime = inGenericValues.GetInt64(FIELD_TIMESTAMP);
    }
    if (inGenericValues.GetInt(FIELD_FLAG) == 1) {
        if (outPermRecord.accessCountFg > 0 || outPermRecord.rejectCountFg > 0) {
            (outPermRecord.lastAccessTime > 0)
                ? outPermRecord.accessRecordFg.emplace_back(outPermRecord.lastAccessTime)
                : outPermRecord.rejectRecordFg.emplace_back(outPermRecord.lastRejectTime);
        } else {
            (outPermRecord.lastAccessTime > 0)
                ? outPermRecord.accessRecordBg.emplace_back(outPermRecord.lastAccessTime)
                : outPermRecord.rejectRecordBg.emplace_back(outPermRecord.lastRejectTime);
        }
    }

    return Constant::SUCCESS;
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS