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

#include "permission_definition.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "PermissionDefinition"};
}
PermissionDefinition &PermissionDefinition::GetInstance()
{
    static PermissionDefinition instance;
    return instance;
}
PermissionDefinition::PermissionDefinition()
{}
PermissionDefinition::~PermissionDefinition()
{}
bool PermissionDefinition::IsRestrictedPermission(const std::string &permissionName)
{
    if (!DistributedDataValidator::IsPermissionNameValid(permissionName)) {
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
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS