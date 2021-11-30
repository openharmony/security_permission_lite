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

#include "distributed_permission_manager_service.h"
#include "permission_log.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"
#include "soft_bus_manager.h"
#include "sensitive_permission_remind.h"
#include "object_device_permission_manager.h"
#include "subject_device_permission_manager.h"
#include "bundle_mgr_interface.h"
#include "permission_kit.h"
#include "distributed_permission_kit.h"
#include "init_log.h"
#include "iservice_registry.h"
#include "bms_adapter.h"
#include "pms_adapter.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_PERMISSION, "DistributedPermissionManagerService"};
}
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<DistributedPermissionManagerService>::GetInstance().get());

DistributedPermissionManagerService::DistributedPermissionManagerService()
    : SystemAbility(SA_ID_DISTRIBUTED_PERMISSION_MANAGER_SERVICE, true),
      serviceRunningState_(ServiceRunningState::STATE_NOT_START),
      runner_(nullptr)
{
    PERMISSION_LOG_INFO(LABEL, "enter");
}

DistributedPermissionManagerService::~DistributedPermissionManagerService()
{
    PERMISSION_LOG_INFO(LABEL, "exit");
}

bool DistributedPermissionManagerService::Init()
{
    runner_ = AppExecFwk::EventRunner::Create(true);
    if (!runner_) {
        PERMISSION_LOG_ERROR(LABEL, "failed to create a runner.");
        return false;
    }

    handler_ = std::make_shared<DistributedPermissionEventHandler>(runner_);
    if (!handler_) {
        PERMISSION_LOG_ERROR(LABEL, "handler_ is nullpter.");
        return false;
    }

    // init
    ObjectDevicePermissionManager::GetInstance().Init();
    PermissionBmsManager::GetInstance().Init();
    RemoteCommandManager::GetInstance().Init();
    SoftBusManager::GetInstance().Initialize();

    return true;
}

void DistributedPermissionManagerService::OnStart()
{
    INIT_LOGI("DistributedPermissionManagerService::OnStart()");
    int32_t intFlag = 2;
    int32_t sleepTime = 50;
    while (true) {
        int32_t flag = 0;
        std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
        if (bmsAdapter != nullptr) {
            sptr<AppExecFwk::IBundleMgr> iBundleManager;
            iBundleManager = bmsAdapter->GetBundleManager();
            if (iBundleManager != nullptr) {
                INIT_LOGI("iBundleManager is not nullptr.");
                flag++;
            } else {
                INIT_LOGI("iBundleManager is nullptr.");
            }
            auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (samgr == nullptr) {
                INIT_LOGI("Get SystemAbilityManager Failed");
            }
            auto object = samgr->CheckSystemAbility(Constant::DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID);
            if (object == nullptr) {
                INIT_LOGI("Get DeviceManager SystemAbility Failed");
            } else {
                INIT_LOGI("iDeviceManager is not nullptr.");
                flag++;
            }
        }
        if (flag == intFlag) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
    if (serviceRunningState_ == ServiceRunningState::STATE_RUNNING) {
        PERMISSION_LOG_ERROR(LABEL, "Distributed Manager Service has already started.");
        return;
    }

    PERMISSION_LOG_INFO(LABEL, "start");

    if (!Init()) {
        PERMISSION_LOG_ERROR(LABEL, "failed to Init service.");
        return;
    }

    runner_->Run();

    if (!Publish(this)) {
        PERMISSION_LOG_ERROR(LABEL, "failed to publish the service.");
        return;
    }

    serviceRunningState_ = ServiceRunningState::STATE_RUNNING;
}

void DistributedPermissionManagerService::OnStop()
{
    PERMISSION_LOG_INFO(LABEL, "stop");

    serviceRunningState_ = ServiceRunningState::STATE_NOT_START;
}

int32_t DistributedPermissionManagerService::AllocateDuid(const std::string &nodeId, const int32_t rUid)
{
    PERMISSION_LOG_INFO(LABEL,
        "%{public}s called, nodeId: %{public}s, rUid: %{public}d",
        __func__,
        Constant::EncryptDevId(nodeId).c_str(),
        rUid);
    const std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: invalid params!", __func__);
        return convert(Constant::INVALID_DEVICE_ID);
    }
    int32_t duid = SubjectDevicePermissionManager::GetInstance().GetDistributedUid(deviceId, rUid);
    if (DistributedDataValidator::IsDuidValid(duid)) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}d: returned: ", duid);
        return convert(duid);
    }

    duid = SubjectDevicePermissionManager::GetInstance().AllocateDistributedUid(deviceId, rUid);
    PERMISSION_LOG_INFO(LABEL, "%{public}d returned: ", duid);
    return convert(duid);
}

int32_t DistributedPermissionManagerService::QueryDuid(const std::string &nodeId, const int32_t rUid)
{
    PERMISSION_LOG_INFO(LABEL,
        "%{public}s called, nodeId: %{public}s, rUid: %{public}d",
        __func__,
        Constant::EncryptDevId(nodeId).c_str(),
        rUid);
    const std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: nodeId cannot convert to deviceId, failure.", __func__);
        return convert(Constant::INVALID_DEVICE_ID);
    }
    const int32_t duid = SubjectDevicePermissionManager::GetInstance().GetDistributedUid(deviceId, rUid);
    PERMISSION_LOG_INFO(LABEL, "%{public}d returned: ", duid);
    return convert(duid);
}

int32_t DistributedPermissionManagerService::WaitDuidReady(
    const std::string &nodeId, const int32_t rUid, const int32_t timeout)
{
    PERMISSION_LOG_INFO(LABEL,
        "%{public}s called, nodeId: %{public}s, rUid: %{public}d, timeout: %{public}d",
        __func__,
        Constant::EncryptDevId(nodeId).c_str(),
        rUid,
        timeout);

    const std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: nodeId cannot convert to deviceId, failure.", __func__);
        return Constant::INVALID_DEVICE_ID;
    }
    return SubjectDevicePermissionManager::GetInstance().WaitDuidReady(deviceId, rUid, timeout);
}

int32_t DistributedPermissionManagerService::NotifySyncPermission(
    std::string &nodeId, int uid, std::string &packageName)
{
    PERMISSION_LOG_INFO(LABEL,
        "nodeId = %{public}s, uid = %{public}d, packageName = %{public}s",
        Constant::EncryptDevId(nodeId).c_str(),
        uid,
        packageName.c_str());

    std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_DEBUG(LABEL,
            "notifySyncPermission: nodeId [%{public}s] cannot convert to deviceId, failure.",
            Constant::EncryptDevId(nodeId).c_str());
        return Constant::INVALID_DEVICE_ID;
    }
    return ObjectDevicePermissionManager::GetInstance().NotifySyncPermission(deviceId, uid, packageName);
}

int32_t DistributedPermissionManagerService::CheckDPermission(int dUid, const std::string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL,
        "%{public}s called, dUid: %{public}d, permissionName: %{public}s",
        __func__,
        dUid,
        permissionName.c_str());
    int32_t checkResult =
        SubjectDevicePermissionManager::GetInstance().CheckDistributedPermission(dUid, permissionName);
    PERMISSION_LOG_DEBUG(LABEL, "%{public}s returned: %{public}d", __func__, checkResult);
    return checkResult;
}
int32_t DistributedPermissionManagerService::CheckLocalPermission(int32_t uid, const std::string &permissionName)
{
    std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
    std::unique_ptr<PmsAdapter> pmsAdapter = std::make_unique<PmsAdapter>();
    if (iBundleManager_ == nullptr) {
        iBundleManager_ = bmsAdapter->GetBundleManager();
        if (iBundleManager_ == nullptr) {
            PERMISSION_LOG_INFO(LABEL, "%{public}s: iBundleManager_ is nullptr.", __func__);
            return Constant::PERMISSION_DENIED;
        }
    }
    if (iPermissionManager_ == nullptr) {
        iPermissionManager_ = pmsAdapter->GetPermissionManager();
        if (iPermissionManager_ == nullptr) {
            PERMISSION_LOG_INFO(LABEL, "%{public}s: iPermissionManager_ is nullptr.", __func__);
            return Constant::PERMISSION_DENIED;
        }
    }
    std::vector<std::string> bundleNames;
    bool result = iBundleManager_->GetBundlesForUid(uid, bundleNames);
    if (!result) {
        PERMISSION_LOG_ERROR(LABEL, "cannot get bundle name by uid %{public}d", uid);
        return Constant::PERMISSION_DENIED;
    }
    for (auto bundleName : bundleNames) {
        int32_t iGranted = iPermissionManager_->VerifyPermission(bundleName, permissionName, Constant::DEFAULT_USERID);
        PERMISSION_LOG_INFO(LABEL, "iGranted : %{public}d", iGranted);
        if (iGranted == Constant::PERMISSION_DENIED) {
            return Constant::PERMISSION_DENIED;
        }
    }
    return Constant::PERMISSION_GRANTED;
}
int32_t DistributedPermissionManagerService::CheckPermission(
    const std::string &permissionName, const std::string &nodeId, int32_t pid, int32_t uid)
{
    PERMISSION_LOG_INFO(LABEL,
        "permissionName = %{public}s, nodeId = %{public}s, pid = %{public}d, uid = %{public}d, ",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str(),
        pid,
        uid);
    if (!DistributedDataValidator::IsPermissionNameValid(permissionName)) {
        PERMISSION_LOG_ERROR(LABEL, "PermissionName data invalid");
        return Constant::PERMISSION_DENIED;
    }
    if (DistributedDataValidator::IsDuidValidAndNotSpecial(uid)) {
        return SubjectDevicePermissionManager::GetInstance().CheckDistributedPermission(uid, permissionName);
    }
    // If nodeId is null, check the local permission.
    if (nodeId.empty()) {
        PERMISSION_LOG_INFO(LABEL, "checkPermission: empty nodeId.");
        return CheckLocalPermission(uid, permissionName);
    }
    std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "checkPermission: nodeId cannot convert to deviceId, failure.");
        return Constant::PERMISSION_DENIED;
    }
    char localDeviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, Constant::DEVICE_UUID_LENGTH);
    if (deviceId == localDeviceId) {
        PERMISSION_LOG_INFO(LABEL, "checkPermission: check by pms because of local device.");
        return CheckLocalPermission(uid, permissionName);
    }
    // If the duid is not present, then get the duid by deviceId and uid.
    int32_t distributedUid = SubjectDevicePermissionManager::GetInstance().AllocateDistributedUid(deviceId, uid);
    return SubjectDevicePermissionManager::GetInstance().CheckDistributedPermission(distributedUid, permissionName);
}

int32_t DistributedPermissionManagerService::CheckPermission(const std::string &permissionName,
    const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "permissionName = %{public}s, appIdInfo = %{public}s", permissionName.c_str(),
        appIdInfo.c_str());
    AppIdInfo appIdInfoObj;
    if (!DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj)) {
        PERMISSION_LOG_INFO(LABEL, "appIdInfo data invalid");
        return Constant::PERMISSION_DENIED;
    }
    std::string nodeId = appIdInfoObj.deviceId;
    int32_t pid = appIdInfoObj.pid;
    int32_t uid = appIdInfoObj.uid;
    return CheckPermission(permissionName, nodeId, pid, uid);
}

int32_t DistributedPermissionManagerService::VerifyPermissionFromRemote(const std::string &permission,
    const std::string &nodeId, const std::string &appIdInfo)
{
    AppIdInfo appIdInfoObj;
    if (!DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj)) {
        PERMISSION_LOG_INFO(LABEL, "appIdInfo data invalid");
        return Constant::PERMISSION_DENIED;
    }
    PERMISSION_LOG_DEBUG(LABEL,
        "verifyPermissionFromRemote() called with: permission= %{public}s, nodeId= %{public}s, "
        " pid= %{public}d  , uid= %{public}d",
        permission.c_str(), Constant::EncryptDevId(nodeId).c_str(), appIdInfoObj.pid, appIdInfoObj.uid);

    std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL,
            "VerifyPermissionFromRemote: nodeId %{public}s cannot convert to deviceId, failure.",
            Constant::EncryptDevId(nodeId).c_str());
        return Constant::PERMISSION_DENIED;
    }
    int result = ObjectDevicePermissionManager::GetInstance().VerifyPermissionFromRemote(permission, deviceId,
        appIdInfoObj.pid, appIdInfoObj.uid);
    PERMISSION_LOG_DEBUG(LABEL, "verifyPermissionFromRemote() returned: %{public}d.", result);
    return result;
}

int32_t DistributedPermissionManagerService::VerifySelfPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    PERMISSION_LOG_DEBUG(LABEL,
        "verifyPermissionFromRemote() called with: permission= %{public}s, nodeId= %{public}s ",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str());
    const pid_t pid = IPCSkeleton::GetCallingPid();
    const pid_t uid = IPCSkeleton::GetCallingUid();
    std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL,
            "VerifySelfPermissionFromRemote: nodeId %{public}s cannot convert to deviceId, failure.",
            Constant::EncryptDevId(nodeId).c_str());
        return Constant::PERMISSION_DENIED;
    }
    int result =
        ObjectDevicePermissionManager::GetInstance().VerifyPermissionFromRemote(permissionName, deviceId, pid, uid);
    PERMISSION_LOG_DEBUG(LABEL, "verifySelfPermissionFromRemote() returned: %{public}d.", result);
    return result;
}

bool DistributedPermissionManagerService::CanRequestPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    PERMISSION_LOG_DEBUG(LABEL,
        "CanRequestPermissionFromRemote() called with: permission= %{public}s, nodeId= %{public}s ",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str());
    const pid_t pid = IPCSkeleton::GetCallingPid();
    const pid_t uid = IPCSkeleton::GetCallingUid();
    const std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL,
            "CanRequestPermissionFromRemote: nodeId %{public}s cannot convert to deviceId, failure.",
            Constant::EncryptDevId(nodeId).c_str());
        return false;
    }
    bool result =
        RequestRemotePermission::GetInstance().CanRequestPermissionFromRemote(permissionName, deviceId, pid, uid);
    PERMISSION_LOG_ERROR(LABEL, "canRequestPermissionFromRemote() returned: %{public}s .", result ? "true" : "false");
    return result;
}
void DistributedPermissionManagerService::GrantSensitivePermissionToRemoteApp(
    const std::string &permissionName, const std::string &nodeId, int ruid)
{
    PERMISSION_LOG_DEBUG(LABEL,
        "GrantSensitivePermissionToRemoteApp() called with: permission= %{public}s, nodeId= %{public}s, "
        " ruid= %{public}d ",
        permissionName.c_str(),
        Constant::EncryptDevId(nodeId).c_str(),
        ruid);
    std::string deviceId = DeviceInfoManager::GetInstance().ConvertToUniqueDisabilityIdOrFetch(nodeId);
    if (deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL,
            "GrantSensitivePermissionToRemoteApp: nodeId %{public}s cannot convert to deviceId, failure.",
            Constant::EncryptDevId(nodeId).c_str());
        return;
    }
    RequestRemotePermission::GetInstance().GrantSensitivePermissionToRemoteApp(permissionName, deviceId, ruid);
}

void DistributedPermissionManagerService::RequestPermissionsFromRemote(const std::vector<std::string> permissions,
    const sptr<OnRequestPermissionsResult> &callback, const std::string &nodeId, const std::string &bundleName,
    int32_t reasonResId)
{
    PERMISSION_LOG_DEBUG(LABEL,
        "RequestPermissionsFromRemote() called with: nodeId= %{public}s, "
        " bundleName= %{public}s , reasonResId= %{public}d",
        Constant::EncryptDevId(nodeId).c_str(), bundleName.c_str(), reasonResId);
    // RequestRemotePermission need the node Id, please do not convert it to deviceId
    RequestRemotePermission::GetInstance().RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName,
        reasonResId);
}

/**
 * Register using permission reminder to dpms at ohos side.
 *
 * @param callback instance of {@link ohos.security.permission.OnUsingPermissionReminder}.
 * @return result code indicates callback if register successfully.
 */
int32_t DistributedPermissionManagerService::RegisterUsingPermissionReminder(
    const sptr<OnUsingPermissionReminder> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::unique_lock<std::mutex> lock(mutex_);
    if (callback == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: callback is nullptr", __func__);
        return Constant::FAILURE_DPMS;
    }

    int32_t result = SensitivePermissionRemind::GetInstance().RegisterUsingPermissionReminder(callback);
    PERMISSION_LOG_DEBUG(LABEL, "RegisterUsingPermissionReminder() result: %{public}d.", result);
    return result;
}

/**
 * unregister using permission reminder to dpms at ohos side.
 *
 * @param callback instance of {@link ohos.security.permission.OnUsingPermissionReminder} has been registered.
 * @return result code indicates callback if unregister successfully.
 */
int32_t DistributedPermissionManagerService::UnregisterUsingPermissionReminder(
    const sptr<OnUsingPermissionReminder> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::unique_lock<std::mutex> lock(mutex_);
    if (callback == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: callback is nullptr", __func__);
        return Constant::FAILURE_DPMS;
    }

    int32_t result = SensitivePermissionRemind::GetInstance().UnregisterUsingPermissionReminder(callback);
    PERMISSION_LOG_DEBUG(LABEL, "UnregisterUsingPermissionReminder() result: %{public}d.", result);
    return result;
}

/**
 * Check permission and start permission using reminder if permission granted.
 *
 * @param permissionName permission name.
 * @param appIdInfo the json string indicates app information
 * @return Permission checked result, {@link #GRANTED} indicates permission granted, otherwise {@link #DENIED}.
 */
int32_t DistributedPermissionManagerService::CheckPermissionAndStartUsing(const std::string &permissionName,
    const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::unique_lock<std::mutex> lock(mutex_);

    if (permissionName.empty()) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: deviceId is null.", __func__);
        return Constant::FAILURE;
    }
    AppIdInfo appIdInfoObj;
    if (!DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj)) {
        PERMISSION_LOG_INFO(LABEL, "appIdInfo data invalid");
        return Constant::FAILURE;
    }

    if (appIdInfoObj.deviceId.empty()) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: deviceId is null.", __func__);
        return Constant::FAILURE;
    }

    bool isSevsitive = BaseRemoteCommand::IsSensitiveResource(permissionName);
    if (!isSevsitive) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: The permissionName is not sevsitive.", __func__);
        return Constant::FAILURE;
    }

    int32_t result = SensitivePermissionRemind::GetInstance().CheckPermissionAndStartUsing(permissionName,
        appIdInfoObj.deviceId, appIdInfoObj.uid, appIdInfoObj.pid);
    PERMISSION_LOG_DEBUG(LABEL, "CheckPermissionAndStartUsing() result: %{public}d.", result);
    return result;
}

/**
 * Used to permission using remind when app start using permission continuously.
 *
 * @param permName the permission name which app start using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 */
void DistributedPermissionManagerService::StartUsingPermission(const std::string &permName,
    const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::unique_lock<std::mutex> lock(mutex_);
    if (permName.empty()) {
        PERMISSION_LOG_INFO(LABEL, "invalid appIdInfo json");
        return;
    }

    AppIdInfo appIdInfoObj;
    if (!DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj)) {
        PERMISSION_LOG_INFO(LABEL, "appIdInfo data invalid");
        return;
    }

    SensitivePermissionRemind::GetInstance().StartUsingPermission(permName, appIdInfoObj.deviceId, appIdInfoObj.uid,
        appIdInfoObj.pid);
    return;
}

/**
 * Used to permission using remind when app stop using permission continuously.
 *
 * @param permName the permission name which app stop using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 */
void DistributedPermissionManagerService::StopUsingPermission(const std::string &permName, const std::string &appIdInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::unique_lock<std::mutex> lock(mutex_);
    if (permName.empty()) {
        PERMISSION_LOG_INFO(LABEL, "invalid appIdInfo json");
        return;
    }

    AppIdInfo appIdInfoObj;
    if (!DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj)) {
        PERMISSION_LOG_INFO(LABEL, "appIdInfo data invalid");
        return;
    }

    SensitivePermissionRemind::GetInstance().StopUsingPermission(permName, appIdInfoObj.deviceId, appIdInfoObj.uid,
        appIdInfoObj.pid);
    return;
}

/**
 * Get bundle info by uid
 *
 * @param bundleInfo the bundle info of app.
 * @param uid the uid from app.
 * @return result true if get successfully.
 */
bool DistributedPermissionManagerService::GetBundleInfo(AppExecFwk::BundleInfo &bundleInfo, int32_t uid)
{
    std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
    iBundleManager_ = bmsAdapter->GetBundleManager();
    std::string bundleName;
    bool result = iBundleManager_->GetBundleNameForUid(uid, bundleName);
    if (!result) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundle name by uid %{public}d", __func__, uid);
        return false;
    }
    result = iBundleManager_->GetBundleInfo(bundleName, AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (!result) {
        PERMISSION_LOG_INFO(
            LABEL, "%{public}s cannot get bundleInfo by bundleName %{public}s", __func__, bundleName.c_str());
        return false;
    }
    return true;
}

/**
 * add permission record
 *
 * @param permName the permission name which app stop using.
 * @param appIdInfo The appliaction information to be verified, include ruid, nodeId, pid and bundlename.
 * @param deviceId thd deviceId from pid json string indicates app information
 * @param failCount fail number
 */
void DistributedPermissionManagerService::AddPermissionsRecord(const std::string &permissionName,
    const std::string &appIdInfo, int32_t sucCount, int32_t failCount)
{
    PERMISSION_LOG_INFO(LABEL,
        "permissionName = %{public}s, appIdInfo = %{public}s, sucCount = "
        "%{public}d, failCount = %{public}d",
        permissionName.c_str(), appIdInfo.c_str(), sucCount, failCount);

    Permission::AppIdInfo appIdInfoObj;
    if (!DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj)) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s appIdInfo data invalid", __func__);
        return;
    }
    if (appIdInfoObj.deviceId.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s deviceId invalid", __func__);
        return;
    }
    PermissionRecordManager::GetInstance().AddPermissionsRecord(permissionName, appIdInfoObj.deviceId, appIdInfoObj.uid,
        sucCount, failCount);
}

/**
 * get permission record
 *
 * @param queryGzipStr the query string zip.
 * @param codeLen code length.
 * @param zipLen query string length.
 * @param resultStr query result string.
 * @return result code if get successfully.
 */
int32_t DistributedPermissionManagerService::GetPermissionRecords(const std::string &queryGzipStr,
    std::string &resultStr)
{
    PERMISSION_LOG_INFO(LABEL, "queryGzipStr = %{public}s, resultStr = %{public}s", queryGzipStr.c_str(),
        resultStr.c_str());
    return PermissionRecordManager::GetInstance().GetPermissionRecordsCompress(queryGzipStr, resultStr);
}

/**
 * get permission record with callback
 *
 * @param request the query string.
 * @param codeLen code length.
 * @param zipLen query string length.
 * @param callback query result callback.
 * @return result code if get successfully.
 */
int32_t DistributedPermissionManagerService::GetPermissionRecords(const std::string &request,
    const sptr<OnPermissionUsedRecord> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "request = %{public}s", request.c_str());
    if (callback == nullptr) {
        return false;
    }
    return PermissionRecordManager::GetInstance().GetPermissionRecordsAsync(request, callback);
}

/**
 * duid convert
 *
 * @param duid  duid.
 * @return result code if get successfully.
 */
int32_t DistributedPermissionManagerService::convert(int32_t duid)
{
    return (duid > Constant::SUCCESS) ? duid : Constant::FAILURE_DPMS;
}

/**
 *get event handler
 *
 * @return event handler object.
 */
std::shared_ptr<DistributedPermissionEventHandler> DistributedPermissionManagerService::GetEventHandler()
{
    return handler_;
}
} // namespace Permission
} // namespace Security
} // namespace OHOS