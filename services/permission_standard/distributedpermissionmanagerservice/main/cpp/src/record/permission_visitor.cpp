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

#include "permission_visitor.h"

namespace OHOS {
namespace Security {
namespace Permission {
bool PermissionVisitor::SetPermissionVisitor(const std::string &deviceId, const std::string &deviceName,
    const int32_t bundleUserId, const std::string &bundleName, const std::string &bundleLabel,
    PermissionVisitor &permissionVisitor)
{
    permissionVisitor.deviceId = deviceId;
    permissionVisitor.deviceName = deviceName;
    permissionVisitor.bundleUserId = bundleUserId;
    permissionVisitor.bundleName = bundleName;
    permissionVisitor.bundleLabel = bundleLabel;
    return true;
};

int PermissionVisitor::TranslationIntoPermissionVisitor(
    const GenericValues &inGenericValues, PermissionVisitor &outPermissionVisitor)
{
    outPermissionVisitor.id = inGenericValues.GetInt(FIELD_ID);
    outPermissionVisitor.deviceId = inGenericValues.GetString(FIELD_DEVICE_ID);
    outPermissionVisitor.deviceName = inGenericValues.GetString(FIELD_DEVICE_NAME);
    outPermissionVisitor.bundleUserId = inGenericValues.GetInt(FIELD_BUNDLE_USER_ID);
    outPermissionVisitor.bundleName = inGenericValues.GetString(FIELD_BUNDLE_NAME);
    outPermissionVisitor.bundleLabel = inGenericValues.GetString(FIELD_BUNDLE_LABEL);
    return Constant::SUCCESS;
}

int PermissionVisitor::TranslationIntoGenericValues(
    const PermissionVisitor &inPermissionVisitor, GenericValues &outGenericValues)
{
    outGenericValues.Put(FIELD_DEVICE_ID, inPermissionVisitor.deviceId);
    outGenericValues.Put(FIELD_DEVICE_NAME, inPermissionVisitor.deviceName);
    outGenericValues.Put(FIELD_BUNDLE_USER_ID, inPermissionVisitor.bundleUserId);
    outGenericValues.Put(FIELD_BUNDLE_NAME, inPermissionVisitor.bundleName);
    outGenericValues.Put(FIELD_BUNDLE_LABEL, inPermissionVisitor.bundleLabel);
    return Constant::SUCCESS;
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
