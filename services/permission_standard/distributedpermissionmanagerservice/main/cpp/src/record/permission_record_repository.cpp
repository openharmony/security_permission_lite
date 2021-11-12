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

#include "permission_record_repository.h"

namespace OHOS {
namespace Security {
namespace Permission {
PermissionRecordRepository &PermissionRecordRepository::GetInstance()
{
    static PermissionRecordRepository instance;
    return instance;
}

PermissionRecordRepository::PermissionRecordRepository()
{}

PermissionRecordRepository::~PermissionRecordRepository()
{}

int32_t PermissionRecordRepository::FindVisitorValues(
    GenericValues &visitorGenericValues, std::vector<GenericValues> &visitorValues)
{
    if (!PermissionRecordDao::GetInstance().FindVisitor(visitorGenericValues, visitorValues)) {
        return Constant::DATABASE_FAILED;
    }
    return Constant::SUCCESS;
}

int32_t PermissionRecordRepository::FindRecordValues(const GenericValues &recordAndGenericValues,
    const GenericValues &recordOrGenericValues, std::vector<GenericValues> &recordValues)
{
    if (!PermissionRecordDao::GetInstance().FindRecord(recordAndGenericValues, recordOrGenericValues, recordValues)) {
        return Constant::DATABASE_FAILED;
    }
    return Constant::SUCCESS;
}

bool PermissionRecordRepository::AddVisitorValues(PermissionVisitor &permissionVisitor)
{
    std::vector<GenericValues> visitorValues;
    std::vector<GenericValues> resultValues;
    GenericValues findVisitor;
    GenericValues insertVisitor;
    PermissionVisitor::TranslationIntoGenericValues(permissionVisitor, findVisitor);
    PermissionVisitor::TranslationIntoGenericValues(permissionVisitor, insertVisitor);
    findVisitor.Remove(FIELD_BUNDLE_USER_ID);
    if (!PermissionRecordDao::GetInstance().FindVisitor(findVisitor, resultValues)) {
        return false;
    }

    if (resultValues.empty()) {
        visitorValues.emplace_back(insertVisitor);
        if (!PermissionRecordDao::GetInstance().AddVisitor(visitorValues)) {
            return false;
        }
        if (!PermissionRecordDao::GetInstance().FindVisitor(visitorValues[0], resultValues)) {
            return false;
        }
    }
    PermissionVisitor::TranslationIntoPermissionVisitor(resultValues[0], permissionVisitor);
    return true;
}

bool PermissionRecordRepository::AddRecordValues(const PermissionRecord &permissionRecord)
{
    std::vector<GenericValues> recordValues;
    std::vector<GenericValues> resultValues;
    GenericValues nullGenericValues;
    GenericValues findRecord;
    GenericValues insertRecord;
    PermissionRecord::TranslationIntoGenericValues(permissionRecord, findRecord);
    PermissionRecord::TranslationIntoGenericValues(permissionRecord, insertRecord);
    findRecord.Remove(FIELD_TIMESTAMP);
    findRecord.Remove(FIELD_ACCESS_COUNT);
    findRecord.Remove(FIELD_REJECT_COUNT);
    if (!PermissionRecordDao::GetInstance().FindRecord(findRecord, nullGenericValues, resultValues)) {
        return false;
    }

    for (auto record : resultValues) {
        if (insertRecord.GetInt64(FIELD_TIMESTAMP) - record.GetInt64(FIELD_TIMESTAMP) < Constant::PRECISION) {
            int32_t accessCount = insertRecord.GetInt(FIELD_ACCESS_COUNT) + record.GetInt(FIELD_ACCESS_COUNT);
            int32_t rejectCount = insertRecord.GetInt(FIELD_REJECT_COUNT) + record.GetInt(FIELD_REJECT_COUNT);
            insertRecord.Remove(FIELD_ACCESS_COUNT);
            insertRecord.Remove(FIELD_REJECT_COUNT);

            insertRecord.Put(FIELD_ACCESS_COUNT, accessCount);
            insertRecord.Put(FIELD_REJECT_COUNT, rejectCount);
            if (!PermissionRecordDao::GetInstance().RemoveRecord(record)) {
                return false;
            }
            break;
        }
    }
    recordValues.emplace_back(insertRecord);
    if (!PermissionRecordDao::GetInstance().AddRecord(recordValues)) {
        return false;
    }
    return true;
}

bool PermissionRecordRepository::RemoveVisitor(const GenericValues &visitorValue)
{
    return PermissionRecordDao::GetInstance().RemoveVisitor(visitorValue);
}

bool PermissionRecordRepository::RemoveRecord(const GenericValues &recordValue)
{
    return PermissionRecordDao::GetInstance().RemoveRecord(recordValue);
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS