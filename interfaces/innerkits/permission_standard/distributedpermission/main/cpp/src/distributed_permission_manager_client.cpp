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

#include "distributed_permission_manager_client.h"
#include "distributed_permission_death_recipient.h"
#include "permission_log.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "distributed_permission_kit.h"
#include "parameter.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_PERMISSION, "DistributedPermissionManagerClient"};
}
const int32_t ERROR = -1;

DistributedPermissionManagerClient &DistributedPermissionManagerClient::GetInstance()
{
    static DistributedPermissionManagerClient instance;
    return instance;
}

int32_t DistributedPermissionManagerClient::AllocateDuid(const std::string &nodeId, const int32_t rUid)
{
    PERMISSION_LOG_INFO(LABEL, "nodeId = %{public}s, rUid = %{public}d", Constant::EncryptDevId(nodeId).c_str(), rUid);
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->AllocateDuid(nodeId, rUid);
}

int32_t DistributedPermissionManagerClient::QueryDuid(const std::string &deviceId, int32_t rUid)
{
    PERMISSION_LOG_INFO(
        LABEL, "deviceId = %{public}s, rUid = %{public}d", Constant::EncryptDevId(deviceId).c_str(), rUid);
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->QueryDuid(deviceId, rUid);
}

int32_t DistributedPermissionManagerClient::CheckDPermission(int32_t dUid, const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "dUid = %{public}d, permissionName = %{public}s", dUid, permissionName.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->CheckDPermission(dUid, permissionName);
}

int32_t DistributedPermissionManagerClient::CheckPermission(const std::string &permissionName,
    const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(
        LABEL, "permissionName = %{public}s, appIdInfo = %{public}s", permissionName.c_str(), appIdInfo.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }
    return distributedPermissionProxy_->CheckPermission(permissionName, appIdInfo);
}

bool DistributedPermissionManagerClient::IsPermissionNameValid(const std::string &permissionName)
{
    return !permissionName.empty() && (permissionName.length() <= MAX_LENGTH);
}

int32_t DistributedPermissionManagerClient::CheckSelfPermission(const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "permissionName = %{public}s", permissionName.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }
    if (!DistributedPermissionManagerClient::IsPermissionNameValid(permissionName)) {
        PERMISSION_LOG_ERROR(LABEL, "CheckSelfPermission::permissionName is not valid");
        return Constant::PERMISSION_DENIED;
    }
    // CheckSelfPermission is used by applications to check whether they have certain permissions,
    // so the IPC package is used to get the process id and uid of the call source.
    pid_t pid = IPCSkeleton::GetCallingPid();
    pid_t uid = IPCSkeleton::GetCallingUid();
    std::cout << "pid:"<<pid<<std::endl;
    std::cout << "uid:"<<uid<<std::endl;
    AppIdInfo appIdInfoObj;
    std::string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid, uid, "");
    return distributedPermissionProxy_->CheckPermission(permissionName, appIdInfo);
}

int32_t DistributedPermissionManagerClient::CheckCallingPermission(const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "permissionName = %{public}s", permissionName.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->CheckCallingPermission(permissionName);
}

int32_t DistributedPermissionManagerClient::CheckCallingOrSelfPermission(const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "permissionName = %{public}s", permissionName.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->CheckCallingOrSelfPermission(permissionName);
}

int32_t DistributedPermissionManagerClient::CheckCallerPermission(const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "permissionName = %{public}s", permissionName.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->CheckCallerPermission(permissionName);
}

bool DistributedPermissionManagerClient::IsRestrictedPermission(const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "permissionName = %{public}s", permissionName.c_str());
    if (!DistributedPermissionManagerClient::IsPermissionNameValid(permissionName)) {
        PERMISSION_LOG_ERROR(LABEL, "PermissionName data invalid");
        return false;
    }
    std::unique_ptr<PmsAdapter> pmsAdapter = std::make_unique<PmsAdapter>();
    iPermissionManager_ = pmsAdapter->GetPermissionManager();
    PermissionDefParcel permissionDefResult;
    int ret = iPermissionManager_->GetDefPermission(permissionName, permissionDefResult);
    if (ret != 0) {
        PERMISSION_LOG_ERROR(LABEL, "get permission def failed");
        return false;
    }
    if (permissionDefResult.permissionDef.availableScope == Permission::AvailableScope::AVAILABLE_SCOPE_RESTRICTED) {
        return true;
    }
    return false;
}

int32_t DistributedPermissionManagerClient::VerifyPermissionFromRemote(
    const std::string &permission, const std::string &nodeId, const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "permission = %{public}s, nodeId = %{public}s, appIdInfo = %{public}s",
        permission.c_str(), Constant::EncryptDevId(nodeId).c_str(), appIdInfo.c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }
    return distributedPermissionProxy_->VerifyPermissionFromRemote(permission, nodeId, appIdInfo);
}

int32_t DistributedPermissionManagerClient::VerifySelfPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    PERMISSION_LOG_INFO(LABEL,
        "permissionName = %{public}s, nodeId = %{public}s",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->VerifySelfPermissionFromRemote(permissionName, nodeId);
}

bool DistributedPermissionManagerClient::CanRequestPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    PERMISSION_LOG_INFO(LABEL,
        "permissionName = %{public}s, nodeId = %{public}s",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str());
    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->CanRequestPermissionFromRemote(permissionName, nodeId);
}

void DistributedPermissionManagerClient::RequestPermissionsFromRemote(const std::vector<std::string> permissions,
    const sptr<OnRequestPermissionsResult> &callback, const std::string &nodeId, const std::string &bundleName,
    int32_t reasonResId)
{
    if (callback == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "RequestPermissionsFromRemote param callback is null!");
        return;
    }
    PERMISSION_LOG_INFO(LABEL,
        "bundleName = %{public}s, nodeId = %{public}s",
        bundleName.c_str(),
        Constant::EncryptDevId(nodeId).c_str());
    if (!GetDistributedPermissionProxy()) {
        return;
    }

    return distributedPermissionProxy_->RequestPermissionsFromRemote(
        permissions, callback, nodeId, bundleName, reasonResId);
}

void DistributedPermissionManagerClient::GrantSensitivePermissionToRemoteApp(
    const std::string &permissionName, const std::string &nodeId, int32_t ruid)
{
    PERMISSION_LOG_INFO(LABEL,
        "permissionName = %{public}s, nodeId = %{public}s, ruid = %{public}d",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str(),
        ruid);
    if (!GetDistributedPermissionProxy()) {
        return;
    }

    return distributedPermissionProxy_->GrantSensitivePermissionToRemoteApp(permissionName, nodeId, ruid);
}

int32_t DistributedPermissionManagerClient::RegisterUsingPermissionReminder(
    const sptr<OnUsingPermissionReminder> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "enter RegisterUsingPermissionReminder");

    if (callback == nullptr) {
        return ERROR;
    }

    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->RegisterUsingPermissionReminder(callback);
}

int32_t DistributedPermissionManagerClient::UnregisterUsingPermissionReminder(
    const sptr<OnUsingPermissionReminder> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "enter UnregisterUsingPermissionReminder");

    if (callback == nullptr) {
        return ERROR;
    }

    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    return distributedPermissionProxy_->UnregisterUsingPermissionReminder(callback);
}

int32_t DistributedPermissionManagerClient::CheckPermissionAndStartUsing(
    const std::string &permissionName, const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    if (permissionName.empty() || appIdInfo.empty()) {
        return ERROR;
    }

    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }
    return distributedPermissionProxy_->CheckPermissionAndStartUsing(permissionName, appIdInfo);
}

int32_t DistributedPermissionManagerClient::CheckCallerPermissionAndStartUsing(const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    if (permissionName.empty()) {
        return ERROR;
    }

    if (!GetDistributedPermissionProxy()) {
        return ERROR;
    }

    pid_t pid = IPCSkeleton::GetCallingPid();
    uid_t uid = IPCSkeleton::GetCallingUid();

    char deviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, Constant::DEVICE_UUID_LENGTH);

    std::string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid, uid, deviceId);

    PERMISSION_LOG_INFO(LABEL,
        "CheckCallerPermissionAndStartUsing::calling pid: %{public}d, uid: %{public}d, deviceId: %{private}s",
        pid,
        uid,
        Constant::EncryptDevId(deviceId).c_str());

    return CheckPermissionAndStartUsing(permissionName, appIdInfo);
}

void DistributedPermissionManagerClient::StartUsingPermission(const std::string &permName, const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    if (permName.empty() || appIdInfo.empty()) {
        PERMISSION_LOG_INFO(LABEL, "checkresult : Param Empty");
        return;
    }

    if (!GetDistributedPermissionProxy()) {
        return;
    }
    return distributedPermissionProxy_->StartUsingPermission(permName, appIdInfo);
}

void DistributedPermissionManagerClient::StopUsingPermission(const std::string &permName, const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    if (permName.empty() || appIdInfo.empty()) {
        PERMISSION_LOG_INFO(LABEL, "checkresult : Param Empty");
        return;
    }
    if (!GetDistributedPermissionProxy()) {
        return;
    }
    return distributedPermissionProxy_->StopUsingPermission(permName, appIdInfo);
}

void DistributedPermissionManagerClient::ResetDistributedPermissionProxy()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if ((distributedPermissionProxy_ != nullptr) && (distributedPermissionProxy_->AsObject() != nullptr)) {
        distributedPermissionProxy_->AsObject()->RemoveDeathRecipient(recipient_);
    }
    distributedPermissionProxy_ = nullptr;
}

void DistributedPermissionManagerClient::AddPermissionUsedRecord(
    const std::string &permissionName, const std::string &appIdInfo, const int32_t sucCount, const int32_t failCount)
{
    PERMISSION_LOG_INFO(LABEL, "enter");
    PERMISSION_LOG_INFO(LABEL,
        "permissionName = %{public}s, appIdInfo = %{public}s, sucCount = "
        "%{public}d, failCount = %{public}d",
        permissionName.c_str(),
        appIdInfo.c_str(),
        sucCount,
        failCount);

    if (permissionName.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s permissionName invalid", __func__);
        return;
    }

    if (!GetDistributedPermissionProxy()) {
        return;
    }
    distributedPermissionProxy_->AddPermissionsRecord(permissionName, appIdInfo, sucCount, failCount);
}

int32_t DistributedPermissionManagerClient::GetPermissionUsedRecords(const QueryPermissionUsedRequest &request,
    QueryPermissionUsedResult &result)
{
    if (!GetDistributedPermissionProxy()) {
        return Constant::FAILURE_DPMS;
    }
    nlohmann::json jsonObj = request.to_json(request);
    std::string queryJsonStr = jsonObj.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
    std::string queryGzipStr;
    if (ZipUtils::CompressString(queryJsonStr, queryGzipStr) != ZipUtils::OK) {
        return Constant::FAILURE;
    }
    std::string resultGzipStr;
    int32_t ret = distributedPermissionProxy_->GetPermissionRecords(queryGzipStr, resultGzipStr);
    std::string resultJsonStr;
    if (ZipUtils::DecompressString(resultGzipStr, resultJsonStr) != ZipUtils::OK) {
        return Constant::FAILURE;
    }
    nlohmann::json jsonRes = nlohmann::json::parse(resultJsonStr, nullptr, false);
    result.from_json(jsonRes, result);
    return ret;
}

int32_t DistributedPermissionManagerClient::GetPermissionUsedRecords(const QueryPermissionUsedRequest &request,
    const sptr<OnPermissionUsedRecord> &callback)
{
    if (!GetDistributedPermissionProxy()) {
        return Constant::FAILURE_DPMS;
    }
    nlohmann::json jsonObj = request.to_json(request);
    std::string queryJsonStr = jsonObj.dump(-1, ' ', false, nlohmann::detail::error_handler_t::replace);
    std::string queryGzipStr;
    if (ZipUtils::CompressString(queryJsonStr, queryGzipStr) != ZipUtils::OK) {
        return Constant::FAILURE;
    }
    int32_t ret = distributedPermissionProxy_->GetPermissionRecords(queryGzipStr, callback);
    return ret;
}

bool DistributedPermissionManagerClient::GetDistributedPermissionProxy()
{
    if (!distributedPermissionProxy_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!distributedPermissionProxy_) {
            sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (!systemAbilityManager) {
                PERMISSION_LOG_ERROR(LABEL, "failed to get systemAbilityManager.");
                return false;
            }

            sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
                IDistributedPermission::SA_ID_DISTRIBUTED_PERMISSION_MANAGER_SERVICE);
            if (!remoteObject) {
                PERMISSION_LOG_ERROR(LABEL, "failed to get remoteObject.");
                return false;
            }

            distributedPermissionProxy_ = iface_cast<IDistributedPermission>(remoteObject);
            if ((!distributedPermissionProxy_) || (!distributedPermissionProxy_->AsObject())) {
                PERMISSION_LOG_ERROR(LABEL, "failed to get distributedPermissionProxy_.");
                return false;
            }
            recipient_ = new (std::nothrow) DistributedPermissionDeathRecipient();
            if (!recipient_) {
                PERMISSION_LOG_ERROR(LABEL, "failed to new recipient_.");
                return false;
            }
            distributedPermissionProxy_->AsObject()->AddDeathRecipient(recipient_);
        }
    }

    return true;
}
} // namespace Permission
} // namespace Security
} // namespace OHOS