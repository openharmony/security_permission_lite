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

#include "permission_record_dao.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "PermissionRecordDao"};
}

PermissionRecordDao &PermissionRecordDao::GetInstance()
{
    static PermissionRecordDao instance;
    return instance;
}

PermissionRecordDao::PermissionRecordDao()
{}

PermissionRecordDao::~PermissionRecordDao()
{}

bool PermissionRecordDao::FindVisitor(
    const GenericValues &visitorGenericValues, std::vector<GenericValues> &visitorValues)
{
    GenericValues nullGenericValues;
    if (DataStorage::GetRealDataStorage().FindByConditions(DataStorage::PERMISSION_VISITOR, visitorGenericValues,
        nullGenericValues, visitorValues) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: database PERMISSION_VISITOR table find failed!", __func__);
        return false;
    }
    return true;
}

bool PermissionRecordDao::FindRecord(const GenericValues &recordAndGenericValues,
    const GenericValues &recordOrGenericValues, std::vector<GenericValues> &recordValues)
{
    if (DataStorage::GetRealDataStorage().FindByConditions(DataStorage::PERMISSION_RECORD, recordAndGenericValues,
        recordOrGenericValues, recordValues) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: database PERMISSION_RECORD table find failed!", __func__);
        return false;
    }
    return true;
}

bool PermissionRecordDao::AddVisitor(const std::vector<GenericValues> &visitorValues)
{
    if (DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_VISITOR, visitorValues) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: database PERMISSION_VISITOR table add failed!", __func__);
        return false;
    }
    return true;
}

bool PermissionRecordDao::AddRecord(const std::vector<GenericValues> &recordValues)
{
    if (DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_RECORD, recordValues) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: database PERMISSION_RECORD table add failed!", __func__);
        return false;
    }
    return true;
}

bool PermissionRecordDao::RemoveVisitor(const GenericValues &visitorValue)
{
    if (DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_VISITOR, visitorValue) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: database PERMISSION_VISITOR update failed!(delete)", __func__);
        return false;
    }
    return true;
}

bool PermissionRecordDao::RemoveRecord(const GenericValues &recordValue)
{
    if (DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_RECORD, recordValue) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: database PERMISSION_RECORD update failed!(delete)", __func__);
        return false;
    }
    return true;
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS