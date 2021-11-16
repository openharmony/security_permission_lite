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

#ifndef PERMISSION_SERVICES_INCLUDE_DISRIBUTED_PERMISSION_MANAGER_SERVICE_H
#define PERMISSION_SERVICES_INCLUDE_DISRIBUTED_PERMISSION_MANAGER_SERVICE_H

#include <sys/types.h>
#include <unistd.h>
#include <set>
#include <cstring>
#include "app_id_info.h"
#include "constant.h"
#include "distributed_permission_stub.h"
#include "event_handler.h"
#include "nocopyable.h"
#include "parameter.h"
#include "permission_record_manager.h"
#include "singleton.h"
#include "system_ability.h"
#include "string_ex.h"
#include "distributed_permission_event_handler.h"
#include "on_using_permission_reminder.h"
#include "permission_fetcher.h"
#include "base_remote_command.h"
#include "permission_reminder_info.h"
#include "device_info_manager.h"
#include "subject_device_permission_manager.h"
#include "on_request_permissions_result.h"
#include "request_remote_permission.h"
#include "resource_manager.h"
#include "notification_helper.h"
#include "remind_info.h"

namespace OHOS {
namespace Security {
namespace Permission {
enum class ServiceRunningState { STATE_NOT_START, STATE_RUNNING };

class DistributedPermissionManagerService : public SystemAbility, public DistributedPermissionStub {
public:
    DECLEAR_SYSTEM_ABILITY(DistributedPermissionManagerService);

    DistributedPermissionManagerService();

    virtual ~DistributedPermissionManagerService() override;

    /**
     * Allocate local Duid for across-device accessing application.
     *
     * @param nodeId the Id of the caller device.
     * @param rUid the app uid of the caller application
     * @return Duid allocated from idle pool.
     */
    virtual int32_t AllocateDuid(const std::string& nodeId, const int32_t rUid) override;

    /**
     * Get allocated Duid for target across-device accessing application.
     *
     * @param nodeId the Id of the caller device.
     * @param rUid the app uid of the caller application
     * @return Duid already allocated.
     */
    virtual int32_t QueryDuid(const std::string& deviceId, const int32_t rUid) override;

    /**
     * Wait for duid assignment within a peroid of time.
     *
     * @param nodeId The Id of the caller device.
     * @param rUid The app uid of the caller application.
     * @param timeout Timeout interval for waiting.
     * @return A positive number is a duid been allocated, -1 failure, -5 wait time out.
     */
    int32_t WaitDuidReady(const std::string& nodeId, const int32_t rUid, const int32_t timeout);

    /**
     * Notify current device sync uid's permissions to peer device.
     *
     * @param nodeId The Id of the peer device.
     * @param uid Application uid.
     * @param packageName The package name.
     * @return 0 success, -1 failure.
     */
    int32_t NotifySyncPermission(std::string& nodeId, int uid, std::string& packageName);

    /**
     * Check if the given across-device accessing application has been granted permission with the given name.
     *
     * @param duid duid to check permission.
     * @param permissionName permission name.
     * @return Check Distributed Permission result.
     */
    virtual int32_t CheckDPermission(int dUid, const std::string& permissionName) override;

    /**
     * Check the given permission if granted based on nodeId, pid, uid.
     *
     * @param permissionName, The specific permission name.
     * @param nodeId, The remote device networkId.
     * @param pid, The pid of caller device.
     * @param uid, The application uid of caller device.
     * @return Permission checked result, 0: GRANTED, -1: DENIED.
     */
    virtual int32_t CheckPermission(const std::string &permissionName, const std::string &appIdInfo) override;

    /**
     * Verify if the given application has been granted permission with the given name to access the remote device.
     *
     * @param permission The permission name.
     * @param nodeId Remote device networkId, not null.
     * @param appIdInfo The appliaction information to be verified, include ruid, nodeId, pid and bundlename.
     * @return 0 Granted, -1 denied.
     */
    virtual int32_t VerifyPermissionFromRemote(const std::string &permission, const std::string &nodeId,
        const std::string &appIdInfo) override;

    /**
     * Verify self whether has been granted permission with the given name to access the remote device.
     *
     * @param permission The permission name.
     * @param nodeId Remote device networkId, not null.
     * @return 0 Granted, -1 denied.
     */
    virtual int32_t VerifySelfPermissionFromRemote(
        const std::string &permissionName, const std::string &nodeId) override;

    /**
     * Check whether this app allows you to dynamically apply for specified permissions from a specified device.
     *
     * @param permission The permission name.
     * @param nodeId Remote device networkId, not null.
     * @return true can request permission, false cannot request permission.
     */
    virtual bool CanRequestPermissionFromRemote(const std::string& permissionName, const std::string& nodeId) override;

    /**
     * Grants cross-device applications the permission to access sensitive resources on the local device.
     *
     * @param permission The permission name.
     * @param nodeId The remote device networkId, not null.
     * @param ruid The remote app uid.
     */
    virtual void GrantSensitivePermissionToRemoteApp(
        const std::string &permissionName, const std::string &nodeId, int32_t ruid) override;

    virtual int32_t RegisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder>& callback) override;

    virtual int32_t UnregisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder>& callback) override;

    virtual int32_t CheckPermissionAndStartUsing(const std::string &permissionName,
        const std::string &appIdInfo) override;

    virtual void StartUsingPermission(const std::string &permName, const std::string &appIdInfo) override;

    virtual void StopUsingPermission(const std::string &permName, const std::string &appIdInfo) override;

    /**
     * Add one permission used/access record.
     *
     * @param permissionName Permission name.
     * @param appIdInfo The appliaction information to be verified, include ruid, nodeId, pid and bundlename.
     * @param sucCount Succeed count.
     * @param failCount Failed count.
     */
    virtual void AddPermissionsRecord(const std::string &permissionName, const std::string &appIdInfo,
        int32_t sucCount, int32_t failCount) override;

    /**
     * Sychronized blocking requesting permission used records.
     *
     * @param queryGzipStr Request ziped json string.
     * @param codeLen Encode length.
     * @param zipLen Ziped length.
     * @param resultStr Response ziped json string.
     * @return 0 if succeeded; -1 if failed.
     */
    virtual int32_t GetPermissionRecords(const std::string &queryGzipStr, std::string &resultStr) override;

    /**
     * Sychronized blocking requesting permission used records.
     *
     * @param queryGzipStr Request ziped json string.
     * @param codeLen Encode length.
     * @param zipLen Ziped length.
     * @param callback The callback function provided for caller to handle response data.
     * @return 0 if succeeded; -1 if failed.
     */
    virtual int32_t GetPermissionRecords(const std::string &queryGzipStr,
        const sptr<OnPermissionUsedRecord> &callback) override;

    virtual void RequestPermissionsFromRemote(const std::vector<std::string> permissions,
        const sptr<OnRequestPermissionsResult>& callback, const std::string& nodeId, const std::string& bundleName,
        int32_t reasonResId) override;

public:
    virtual void OnStart() override;

    virtual void OnStop() override;

    std::shared_ptr<DistributedPermissionEventHandler> GetEventHandler();

private:
    bool Init();
    ServiceRunningState serviceRunningState_ = ServiceRunningState::STATE_NOT_START;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::recursive_mutex stackLock_;
    std::shared_ptr<DistributedPermissionEventHandler> handler_;
    sptr<AppExecFwk::IBundleMgr> iBundleManager_;
    sptr<Permission::IPermissionManager> iPermissionManager_;
    sptr<Notification::IAnsManager> iAnsManager_;
    const std::string APP_TYPE_OTHER = "other";
    const std::string APP_TYPE_SYSTEM = "system";
    enum {
        DENIED = -1,
        GRANTED = 0,
    };
    int32_t CheckLocalPermission(int32_t uid, const std::string &permissionName);
    int32_t CheckPermission(const std::string &permissionName, const std::string &nodeId, int32_t pid, int32_t uid);
    bool GetBundleInfo(AppExecFwk::BundleInfo &bundleInfo, int32_t uid);
    int32_t convert(int32_t duid);
    std::mutex mutex_;
    DISALLOW_COPY_AND_MOVE(DistributedPermissionManagerService);
};
} // namespace Permission
} // namespace Security
} // namespace OHOS

#endif // PERMISSION_SERVICES_INCLUDE_DISRIBUTED_PERMISSION_MANAGER_SERVICE_H
