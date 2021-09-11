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

#include "distributed_permission_manager.h"
#include "distributed_permission_manager_client.h"
#include "permission_log.h"
#include "singleton.h"

namespace OHOS {
namespace Security {
namespace Permission {
int32_t DistributedPermissionManager::VerifyPermissionFromRemote(
    const std::string &permission, const std::string &nodeId, const std::string &appIdInfo)
{
    return DelayedSingleton<DistributedPermissionManagerClient>::GetInstance()->VerifyPermissionFromRemote(
        permission, nodeId, appIdInfo);
}

int32_t DistributedPermissionManager::VerifySelfPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    return DelayedSingleton<DistributedPermissionManagerClient>::GetInstance()->VerifySelfPermissionFromRemote(
        permissionName, nodeId);
}

bool DistributedPermissionManager::CanRequestPermissionFromRemote(
    const std::string &permissionName, const std::string &nodeId)
{
    return DelayedSingleton<DistributedPermissionManagerClient>::GetInstance()->CanRequestPermissionFromRemote(
        permissionName, nodeId);
}

void DistributedPermissionManager::RequestPermissionsFromRemote(const std::vector<std::string> permissions,
    const sptr<OnRequestPermissionsResult> &callback, const std::string &nodeId, const std::string &bundleName,
    int32_t reasonResId)
{
    return DelayedSingleton<DistributedPermissionManagerClient>::GetInstance()->RequestPermissionsFromRemote(
        permissions, callback, nodeId, bundleName, reasonResId);
}

void DistributedPermissionManager::GrantSensitivePermissionToRemoteApp(
    const std::string &permissionName, const std::string &nodeId, int32_t ruid)
{
    return DelayedSingleton<DistributedPermissionManagerClient>::GetInstance()->GrantSensitivePermissionToRemoteApp(
        permissionName, nodeId, ruid);
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS