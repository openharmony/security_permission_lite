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
#include "bms_adapter.h"
#include "permission_record_manager.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "PermissionRecordManager"};
}

PermissionRecordManager &PermissionRecordManager::GetInstance()
{
    static PermissionRecordManager instance;
    return instance;
}

PermissionRecordManager::PermissionRecordManager()
{}

PermissionRecordManager::~PermissionRecordManager()
{}

// add permission used record
void PermissionRecordManager::AddPermissionsRecord(const std::string &permissionName, const std::string &deviceId,
    int32_t uid, int32_t sucCount, int32_t failCount)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->rwLock_);
    PERMISSION_LOG_INFO(LABEL,
        "%{public}s called, permissionName: %{public}s, deviceId: %{public}s, uid: %{public}d, sucCount: %{public}d, "
        "failCount: %{public}d",
        __func__, permissionName.c_str(), Constant::EncryptDevId(deviceId).c_str(), uid, sucCount, failCount);

    auto DelRecordsTask = [this]() {
        PERMISSION_LOG_INFO(LABEL, "---DeletePermissionRecords task called");
        DeletePermissionRecords(Constant::RECORD_DELETE_TIME);
    };
    std::thread recordThread(DelRecordsTask);
    recordThread.detach();

    int32_t visitorId = 0;
    if (!AddVisitor(deviceId, uid, visitorId)) {
        return;
    }
    if (!AddRecord(permissionName, visitorId, uid, sucCount, failCount)) {
        return;
    }
}

bool PermissionRecordManager::AddVisitor(const std::string &deviceId, int32_t uid, int32_t &visitorId)
{
    PermissionVisitor permissionVisitor;
    if (!GetPermissionVisitor(deviceId, uid, permissionVisitor)) {
        return false;
    }
    if (!PermissionRecordRepository::GetInstance().AddVisitorValues(permissionVisitor)) {
        return false;
    }
    visitorId = permissionVisitor.id;
    return true;
}

bool PermissionRecordManager::AddRecord(const std::string &permissionName, int32_t visitorId, int32_t uid,
    int32_t sucCount, int32_t failCount)
{
    PermissionRecord permissionRecord;
    permissionRecord.visitorId = visitorId;
    permissionRecord.accessCount = sucCount;
    permissionRecord.rejectCount = failCount;
    if (!GetPermissionRecord(permissionName, visitorId, uid, permissionRecord)) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: PermissionName translate into opCode failed!", __func__);
        return false;
    }
    return PermissionRecordRepository::GetInstance().AddRecordValues(permissionRecord);
}

int32_t PermissionRecordManager::GetPermissionRecordsCompress(const std::string &queryGzipStr, std::string &resultStr)
{
    QueryPermissionUsedResult queryResult;
    std::string queryJsonStr;
    if (ZipUtils::DecompressString(queryGzipStr, queryJsonStr) != ZipUtils::OK) {
        return Constant::FAILURE;
    }
    int32_t flag = GetPermissionRecords(queryJsonStr, queryResult);
    nlohmann::json jsonObj = queryResult.to_json(queryResult);
    std::string result = jsonObj.dump();
    if (ZipUtils::CompressString(result, resultStr) != ZipUtils::OK) {
        return Constant::FAILURE;
    }
    return flag;
}

int32_t PermissionRecordManager::GetPermissionRecordsAsync(const std::string &queryGzipStr,
    const sptr<OnPermissionUsedRecord> &callback)
{
    std::string queryJsonStr;
    if (ZipUtils::DecompressString(queryGzipStr, queryJsonStr) != ZipUtils::OK) {
        return Constant::FAILURE;
    }
    auto task = [queryJsonStr, callback]() {
        PERMISSION_LOG_INFO(LABEL, "---GetPermissionRecords task called");
        QueryPermissionUsedResult defaultResult;
        PermissionRecordManager::GetInstance().GetPermissionRecords(queryJsonStr, defaultResult);
        PERMISSION_LOG_INFO(LABEL, "%{public}s callback OnQueried called, resultCode: %{public}d", __func__,
            defaultResult.code);
        callback->OnQueried(defaultResult.code, defaultResult);
    };
    std::thread recordThread(task);
    recordThread.detach();

    return Constant::SUCCESS;
}

int32_t PermissionRecordManager::GetPermissionRecords(const std::string &queryJsonStr,
    QueryPermissionUsedResult &queryResult)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called, queryJsonStr: %{public}s", __func__, queryJsonStr.c_str());

    auto DelRecordsTask = [this]() {
        PERMISSION_LOG_INFO(LABEL, "---DeletePermissionRecords task called");
        DeletePermissionRecords(Constant::RECORD_DELETE_TIME);
    };
    std::thread recordThread(DelRecordsTask);
    recordThread.detach();

    nlohmann::json requestJson = nlohmann::json::parse(queryJsonStr);
    QueryPermissionUsedRequest request;
    request.from_json(requestJson, request);
    std::vector<BundlePermissionUsedRecord> bundle;
    queryResult.beginTimeMillis = 0;
    queryResult.endTimeMillis = 0;
    bool flag = GetBundlePermissionUsedRecord(request, bundle, queryResult);
    queryResult.bundlePermissionUsedRecords = bundle;

    if (flag) {
        return Constant::SUCCESS;
    }
    return Constant::FAILURE;
}

bool PermissionRecordManager::GetBundlePermissionUsedRecord(const QueryPermissionUsedRequest &request,
    std::vector<BundlePermissionUsedRecord> &bundle, QueryPermissionUsedResult &queryResult)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    GenericValues visitorGenericValues;
    GenericValues recordAndGenericValues;
    GenericValues recordOrGenericValues;
    if (QueryPermissionUsedRequest::TranslationIntoGenericValues(request, visitorGenericValues, recordAndGenericValues,
        recordOrGenericValues) != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: time conditions are invalid!", __func__);
        return false;
    }

    std::vector<GenericValues> visitorValues;
    int32_t visitorRet =
        PermissionRecordRepository::GetInstance().FindVisitorValues(visitorGenericValues, visitorValues);
    if (visitorRet != Constant::SUCCESS) {
        queryResult.code = visitorRet;
    }
    if (visitorValues.empty()) {
        queryResult.code = Constant::SUCCESS_GET_RECORD;
        PERMISSION_LOG_INFO(LABEL, "%{public}s: visitor data is null!", __func__);
        return true;
    }

    for (const auto &visitor : visitorValues) {
        BundlePermissionUsedRecord bundleRecord;
        BundlePermissionUsedRecord::TranslationIntoBundlePermissionUsedRecord(visitor, bundleRecord);

        std::vector<GenericValues> recordValues;
        recordAndGenericValues.Put(FIELD_VISITOR_ID, visitor.GetInt(FIELD_ID));
        int32_t recordRet = PermissionRecordRepository::GetInstance().FindRecordValues(recordAndGenericValues,
            recordOrGenericValues, recordValues);
        if (recordRet != Constant::SUCCESS) {
            queryResult.code = recordRet;
        }
        recordAndGenericValues.Remove(FIELD_VISITOR_ID);
        if (!recordValues.empty()) {
            if (!GetRecordFromDB(request.flag, recordValues, bundleRecord, queryResult)) {
                return false;
            }
        }
        if (bundleRecord.permissionUsedRecords.size() != 0) {
            bundle.emplace_back(bundleRecord);
        }
    }
    queryResult.code = Constant::SUCCESS_GET_RECORD;
    return true;
}

bool PermissionRecordManager::GetRecordFromDB(int32_t allFlag, const std::vector<GenericValues> &recordValues,
    BundlePermissionUsedRecord &bundleRecord, QueryPermissionUsedResult &queryResult)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    std::vector<PermissionUsedRecord> usedRecord;
    for (auto record : recordValues) {
        PermissionUsedRecord tempRecord;
        if (record.GetInt64(FIELD_TIMESTAMP) > queryResult.endTimeMillis) {
            queryResult.endTimeMillis = record.GetInt64(FIELD_TIMESTAMP);
        }
        if ((queryResult.beginTimeMillis == 0) ? true
                                               : record.GetInt64(FIELD_TIMESTAMP) < queryResult.beginTimeMillis) {
            queryResult.beginTimeMillis = record.GetInt64(FIELD_TIMESTAMP);
        }
        record.Put(FIELD_FLAG, allFlag);
        if (PermissionUsedRecord::TranslationIntoPermissionUsedRecord(record, tempRecord) != Constant::SUCCESS) {
            PERMISSION_LOG_ERROR(LABEL,
                "%{public}s: opCode translate into PermissionName failed! Cannot recognize opCode.", __func__);
            queryResult.code = Constant::NOT_DEFINED;
            return false;
        }

        auto ite = std::find_if(usedRecord.begin(), usedRecord.end(), [tempRecord](const PermissionUsedRecord &rec) {
            return tempRecord.permissionName == rec.permissionName;
        });
        if (ite != usedRecord.end()) {
            if (allFlag == 1) {
                (*ite).UpdateRecordWithTime(tempRecord);
            } else {
                (*ite).UpdateRecord(tempRecord);
            }
        } else {
            usedRecord.emplace_back(tempRecord);
        }
    }
    bundleRecord.permissionUsedRecords = usedRecord;
    return true;
}

void PermissionRecordManager::DeletePermissionUsedRecords(int32_t uid)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->rwLock_);
    PERMISSION_LOG_INFO(LABEL, "%{public}s ------------- begin BMS------------", __func__);
    std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
    sptr<AppExecFwk::IBundleMgr> iBundleMgr;
    iBundleMgr = bmsAdapter->GetBundleManager();

    std::string bundleName;
    bool result = iBundleMgr->GetBundleNameForUid(uid, bundleName);
    if (!result) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundle name by uid %{public}d", __func__, uid);
        return;
    }
    PERMISSION_LOG_INFO(LABEL, "%{public}s ------------- end BMS------------", __func__);
    std::vector<GenericValues> visitorValues;
    GenericValues visitorGenericValues;
    visitorGenericValues.Put(FIELD_BUNDLE_NAME, bundleName);
    if (PermissionRecordRepository::GetInstance().FindVisitorValues(visitorGenericValues, visitorValues) ==
        Constant::FAILURE) {
        return;
    }
    for (auto visitor : visitorValues) {
        GenericValues record;
        record.Put(FIELD_VISITOR_ID, visitor.GetInt(FIELD_ID));
        PermissionRecordRepository::GetInstance().RemoveRecord(record);
    }
    PermissionRecordRepository::GetInstance().RemoveVisitor(visitorGenericValues);
}

int PermissionRecordManager::DeletePermissionRecords(int32_t days)
{
    OHOS::Utils::UniqueWriteGuard<OHOS::Utils::RWLock> lk(this->rwLock_);
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    std::vector<GenericValues> resultValues;
    GenericValues nullValue;
    if (PermissionRecordRepository::GetInstance().FindRecordValues(nullValue, nullValue, resultValues) ==
        Constant::FAILURE) {
        return Constant::FAILURE;
    }
    for (auto record : resultValues) {
        if (TimeUtil::GetTimestamp() - record.GetInt64(FIELD_TIMESTAMP) >= days) {
            if (!PermissionRecordRepository::GetInstance().RemoveRecord(record)) {
                return Constant::FAILURE;
            }
        }
    }
    return Constant::SUCCESS;
}

bool PermissionRecordManager::GetPermissionVisitor(const std::string &deviceId, const int uid,
    PermissionVisitor &permissionVisitor)
{
    DeviceInfo deviceInfo;
    if (!DelayedSingleton<DeviceInfoManager>::GetInstance()->GetDeviceInfo(deviceId, DeviceIdType::UNKNOWN,
        deviceInfo)) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get DeviceInfo by deviceId %{public}s", __func__,
            Constant::EncryptDevId(deviceId).c_str());
        return false;
    }
    PERMISSION_LOG_INFO(LABEL, "%{public}s ------------- begin BMS------------", __func__);
    std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
    iBundleManager_ = bmsAdapter->GetBundleManager();

    std::string bundleName;
    AppExecFwk::BundleInfo bundleInfo;
    bool result = iBundleManager_->GetBundleNameForUid(uid, bundleName);
    if (!result) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundle name by uid %{public}d", __func__, uid);
        return false;
    }
    result = iBundleManager_->GetBundleInfo(bundleName,
        AppExecFwk::BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION,
        bundleInfo);
    if (!result) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundleInfo by bundleName %{public}s", __func__,
            bundleName.c_str());
        return false;
    }
    PERMISSION_LOG_INFO(LABEL, "%{public}s ------------- end BMS------------", __func__);
    PermissionVisitor::SetPermissionVisitor(deviceId, deviceInfo.deviceName, 0, bundleName, bundleInfo.label,
        permissionVisitor);
    return true;
}

bool PermissionRecordManager::GetPermissionRecord(const std::string &permissionName, int32_t visitorId, int32_t uid,
    PermissionRecord &permissionRecord)
{
    int32_t opCode = 0;
    // blocked
    // get isforeground by uid
    bool isForeground = true;
    std::string tempName = permissionName;
    if (Constant::PermissionNameToOrFromOpCode(tempName, opCode)) {
        permissionRecord.timestamp = TimeUtil::GetTimestamp();
        permissionRecord.opCode = opCode;
        permissionRecord.isForeground = isForeground;
        return true;
    }
    return false;
}
} // namespace Permission
} // namespace Security
} // namespace OHOS