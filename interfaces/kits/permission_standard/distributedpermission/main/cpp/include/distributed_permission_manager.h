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

#ifndef PERMISSION_KITS_INCLUDE_DISRIBUTED_PERMISSION_MANAGER_H
#define PERMISSION_KITS_INCLUDE_DISRIBUTED_PERMISSION_MANAGER_H

#include <iostream>
#include "distributed_permission_kit.h"

namespace OHOS {
namespace Security {
namespace Permission {
class DistributedPermissionManager {
public:
    static int32_t VerifyPermissionFromRemote(
        const std::string &permission, const std::string &nodeId, const std::string &appIdInfo);

    static int32_t VerifySelfPermissionFromRemote(const std::string &permissionName, const std::string &nodeId);

    static bool CanRequestPermissionFromRemote(const std::string &permissionName, const std::string &nodeId);

    static void RequestPermissionsFromRemote(const std::vector<std::string> permissions,
        const sptr<OnRequestPermissionsResult> &callback, const std::string &nodeId, const std::string &bundleName,
        int32_t reasonResId);

    static void GrantSensitivePermissionToRemoteApp(
        const std::string &permissionName, const std::string &nodeId, int32_t ruid);
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS

#endif  // PERMISSION_KITS_INCLUDE_DISRIBUTED_PERMISSION_MANAGER_H