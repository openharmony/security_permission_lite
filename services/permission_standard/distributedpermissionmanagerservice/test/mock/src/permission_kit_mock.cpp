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

#include "permission/permission_kit.h"

#include <string>
#include <vector>

#include "permission_log.h"
#include "permission_manager_client.h"
#include "data_validator.h"

namespace OHOS {
namespace Security {
namespace Permission {
using namespace std;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "PermissionKit"};
} // namespace

int PermissionKit::VerifyPermission(const string &bundleName, const string &permissionName, int userId)
{
    return 0;
}
bool PermissionKit::CanRequestPermission(const string &bundleName, const string &permissionName, int userId)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, permissionName=%{public}s, userId=%{public}d",
        bundleName.c_str(), permissionName.c_str(), userId);
    return true;
}

int PermissionKit::GrantUserGrantedPermission(const string &bundleName, const string &permissionName, int userId)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, permissionName=%{public}s, userId=%{public}d",
        bundleName.c_str(), permissionName.c_str(), userId);
    return 0;
}

int PermissionKit::GrantSystemGrantedPermission(const string &bundleName, const string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, permissionName=%{public}s", bundleName.c_str(),
        permissionName.c_str());
    return 0;
}

int PermissionKit::RevokeUserGrantedPermission(const string &bundleName, const string &permissionName, int userId)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, permissionName=%{public}s, userId=%{public}d",
        bundleName.c_str(), permissionName.c_str(), userId);
    return 0;
}

int PermissionKit::RevokeSystemGrantedPermission(const string &bundleName, const string &permissionName)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, permissionName=%{public}s", bundleName.c_str(),
        permissionName.c_str());
    return 0;
}

int PermissionKit::AddUserGrantedReqPermissions(const string &bundleName, const std::vector<string> &permList,
    int userId)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, userId=%{public}d", bundleName.c_str(), userId);
    return 0;
}

int PermissionKit::AddSystemGrantedReqPermissions(const string &bundleName, const std::vector<string> &permList)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s", bundleName.c_str());
    return 0;
}

int PermissionKit::RemoveUserGrantedReqPermissions(const string &bundleName, int userId)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    PERMISSION_LOG_INFO(LABEL, "bundleName=%{public}s, userId=%{public}d", bundleName.c_str(), userId);
    return 0;
}

int PermissionKit::RemoveSystemGrantedReqPermissions(const string &bundleName)
{
    return 0;
}

int PermissionKit::AddDefPermissions(const std::vector<PermissionDef> &permList)
{
    return 0;
}

int PermissionKit::RemoveDefPermissions(const string &bundleName)
{
    return 0;
}

int PermissionKit::GetDefPermission(const string &permissionName, PermissionDef &permissionDefResult)
{
    return 0;
}
} // namespace Permission
} // namespace Security
} // namespace OHOS