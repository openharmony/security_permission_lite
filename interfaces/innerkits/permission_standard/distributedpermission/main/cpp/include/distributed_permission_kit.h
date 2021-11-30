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

#ifndef DISRIBUTED_PERMISSION_KIT_H
#define DISRIBUTED_PERMISSION_KIT_H

#include <iostream>
#include "app_id_info.h"
#include "constant.h"
#include "nlohmann/json.hpp"
#include "on_using_permission_reminder.h"
#include "on_permission_used_record.h"
#include "on_request_permissions_result.h"
#include "query_permission_used_request.h"

namespace OHOS {
namespace Security {
namespace Permission {
class DistributedPermissionKit {
public:
    /*!
    @brief Allocate local Duid for across-device accessing application.

    @param[in] nodeId the Id of the caller device.
    @param[in] rUid the app uid of the caller application.

    @return Duid allocated from idle pool.
    */
    static int32_t AllocateDuid(const std::string &nodeId, const int32_t rUid);

    /*!
    @brief Get allocated Duid for target across-device accessing application.

    @param[in] nodeId nodeId the Id of the caller device.
    @param[in] rUid the app uid of the caller application.

    @return Duid already allocated.
    */
    static int32_t QueryDuid(const std::string &deviceId, int32_t rUid);

    static bool IsDuid(int32_t uid);

    class AppIdInfoHelper {
    public:
        /*!
        @brief Create the json string based on pid and uid.

        @param[in] pid The pid of caller device.
        @param[in] uid The application uid of caller device.

        @return The json string included pid and uid.
        */
        static std::string CreateAppIdInfo(pid_t pid, uid_t uid);

        /*!
        @brief Create the json string based on pid, uid and deviceID.

        @param[in] pid The pid of caller device.
        @param[in] uid The application uid of caller device.
        @param[in] deviceID The deviceID of caller device.

        @return The json string included pid, uid and deviceID.
        */
        static std::string CreateAppIdInfo(pid_t pid, uid_t uid, const std::string &deviceID);

        /*!
        @brief Create the json string based on pid, uid, deviceID and bundleName.

        @param[in] pid The pid of caller device.
        @param[in] uid The application uid of caller device.
        @param[in] deviceID The deviceID of caller device.
        @param[in] bundleName The application bundleName of caller device.

        @return The json string included pid, uid, deviceID and bundleName.
        */
        static std::string CreateAppIdInfo(
            pid_t pid, uid_t uid, const std::string &deviceID, const std::string &bundleName);

        /*!
        @brief Parse the json string based on appIdInfo, and convert to appIdInfoObj.

        @param[in] appIdInfo The json string included pid, uid, deviceID and bundleName.
        @param[in] appIdInfoObj The appIdInfo Object included pid, uid, deviceID.

        @return If the appIdInfo Object is valid, return True; Otherwise, return False.
        */
        static bool ParseAppIdInfo(const std::string &appIdInfo, Permission::AppIdInfo &appIdInfoObj);
    };

    /*!
    @brief Check if the given across-device accessing application has been granted permission with the given name.

    @param[in] duid duid to check permission.
    @param[in] permissionName permission name.

    @return Check Distributed Permission result.
    */
    static int32_t CheckDPermission(int32_t dUid, const std::string &permissionName);

    /*!
    @brief Check the given permission if granted based on appIdInfo.

    @param[in] permissionName, The specific permission name.
    @param[in] appIdInfo, The json string included pid, uid, deviceID and bundleName.

    @return Permission checked result, 0: GRANTED, -1: DENIED.
    */
    static int32_t CheckPermission(const std::string &permissionName, const std::string &appIdInfo);

    /*!
    @brief Check self permission.

    @param[in] permissionName, permission name.

    @return If the permission is granted, 0: GRANTED; otherwise, -1: DENIED.
    */
    static int32_t CheckSelfPermission(const std::string &permissionName);

    /*!
    @brief Check current process's permission.

    @param[in] permissionName, permission name.

    @return If the permission is granted, 0: GRANTED; otherwise, -1: DENIED.
    */
    static int32_t CheckCallingPermission(const std::string &permissionName);

    /*!
    @brief Check current process's or self permission.

    @param[in] permissionName, permission name.

    @return If the permission is granted, 0: GRANTED; otherwise, -1: DENIED.
    */
    static int32_t CheckCallingOrSelfPermission(const std::string &permissionName);

    /*!
    @brief Check caller's permission.

    @param[in] permissionName, permission name.

    @return If the permission is granted, 0: GRANTED; otherwise, -1: DENIED.
    */
    static int32_t CheckCallerPermission(const std::string &permissionName);

    /*!
    @brief Check if the permission is restricted.

    @param[in] permissionName, permission name.

    @return If the permission is restricted, return true; otherwise, return false.
    */
    static bool IsRestrictedPermission(const std::string &permissionName);

    /*!
    @brief Verify if the given application has been granted permission with the given name to access the remote device.

    @param[in] permissionName, permission name.
    @param[in] nodeId, Remote device networkId, not null.
    @param[in] appIdInfo, The appliaction information to be verified, include ruid, nodeId, pid and bundlename.

    @return 0 Granted, -1 denied.
    */
    static int32_t VerifyPermissionFromRemote(
        const std::string &permission, const std::string &nodeId, const std::string &appIdInfo);

    /*!
    @brief Verify self whether has been granted permission with the given name to access the remote device.

    @param[in] permissionName, permission name.
    @param[in] nodeId, Remote device networkId, not null.

    @return 0 Granted, -1 denied.
    */
    static int32_t VerifySelfPermissionFromRemote(const std::string &permissionName, const std::string &nodeId);

    /*!
    @brief Check whether this app allows you to dynamically apply for specified permissions from a specified device.

    @param[in] permissionName, permission name.
    @param[in] nodeId, Remote device networkId, not null.

    @return true can request permission, false cannot request permission.
    */
    static bool CanRequestPermissionFromRemote(const std::string &permissionName, const std::string &nodeId);

    /*!
    @brief Dynamically applies for permissions from a specified device (the application result is returned
    asynchronously).

    @param[in] permissionName, permissions The permission names will request from remote.
    @param[in] callback The callback of the request operation.
    @param[in] nodeId The remote device networkId.
    @param[in] bundleName The package name that request the permissions.
    @param[in] reasonResId The reason that request the permissions.

    @return true can request permission, false cannot request permission.
    */
    static void RequestPermissionsFromRemote(const std::vector<std::string> permissions,
        const sptr<OnRequestPermissionsResult> &callback, const std::string &nodeId, const std::string &bundleName,
        int32_t reasonResId);

    /*!
    @brief Grants cross-device applications the permission to access sensitive resources on the local device.

    @param[in] permission The permission name.
    @param[in] nodeId The remote device networkId.
    @param[in] ruid The remote app uid.
    */
    static void GrantSensitivePermissionToRemoteApp(
        const std::string &permissionName, const std::string &nodeId, int32_t ruid);

    /*!
    @brief Register using permission reminder to dpms at ohos side.

    @param[in] callback instance of {@link ohos.security.permission.OnUsingPermissionReminder}.

    @return result code indicates callback if register successfully.
    */
    static int32_t RegisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback);

    /*!
    @brief unregister using permission reminder to dpms at ohos side.

    @param[in] callback instance of {@link ohos.security.permission.OnUsingPermissionReminder} has been registered.

    @return result code indicates callback if unregister successfully.
    */
    static int32_t UnregisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback);

    /*!
    @brief Check permission and start permission using reminder if permission granted.

    @param[in] permissionName permission name.
    @param[in] appIdInfo, The json string included pid, uid, deviceID and bundleName.

    @return Permission checked result, {@link #GRANTED} indicates permission granted, otherwise {@link #DENIED}.
    */
    static int32_t CheckPermissionAndStartUsing(const std::string &permissionName, const std::string &appIdInfo);

    /*!
    @brief Check permission and start permission using reminder if permission granted.

    @param[in] permissionName permission name.

    @return Permission checked result, {@link #GRANTED} indicates permission granted, otherwise {@link #DENIED}.
    */
    static int32_t CheckCallerPermissionAndStartUsing(const std::string &permissionName);

    /*!
    @brief Used to permission using remind when app start using permission continuously.

    @param[in] permName the permission name which app start using.
    @param[in] appIdInfo json string indicates app information including pid, uid and deviceID.
    */
    static void StartUsingPermission(const std::string &permName, const std::string &appIdInfo);

    /*!
    @brief Used to permission using remind when app stop using permission continuously.

    @param[in] permName the permission name which app stop using.
    @param[in] appIdInfo json string indicates app information including pid, uid and deviceID.
    */
    static void StopUsingPermission(const std::string &permName, const std::string &appIdInfo);

    /*!
    @brief Add one permission used/access record.

    @param[in] permName Permission name.
    @param[in] appIdInfo Json string indicates app information including pid, uid and deviceID.
    @param[in] sucCount Succeed count.
    @param[in] failCount Failed count.
    */
    static void AddPermissionUsedRecord(const std::string &permissionName, const std::string &appIdInfo,
        const int32_t sucCount, const int32_t failCount);

    /*!
    @brief Synchronized blocking requesting permission used records.

    @param[in] request Request dto for permission used records.
    @param[in] result Response dto for permission used records.

    @return 0 if succeeded; -1 if failed ; -2 if DPMS service failed.
    */
    static int32_t GetPermissionUsedRecords(
        const QueryPermissionUsedRequest &request, QueryPermissionUsedResult &result);

    /*!
    @brief Synchronized blocking requesting permission used records.

    @param[in] request Request dto for permission used records.
    @param[in] callback The callback function provided for caller to handle response data.

    @return 0 if succeeded; -1 if failed ; -2 if DPMS service failed.
    */
    static int32_t GetPermissionUsedRecords(
        const QueryPermissionUsedRequest &request, const sptr<OnPermissionUsedRecord> &callback);
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS

#endif  // DISRIBUTED_PERMISSION_KIT_H
