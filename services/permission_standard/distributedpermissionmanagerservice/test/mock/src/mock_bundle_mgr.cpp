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

#include "mock_bundle_mgr.h"
#include <iostream>
#include "application_info.h"

namespace OHOS {
namespace AppExecFwk {

bool BundleMgrService::GetAppsGrantedPermissions(
    const std::vector<std::string> &permissions, std::vector<std::string> &appNames)
{
    std::cout<<"GetAppsGrantedPermissions"<<std::endl;
    return true;
}
bool BundleMgrService::HasSystemCapability(const std::string &capName)
{
    std::cout<<"HasSystemCapability"<<std::endl;
    return true;
}
bool BundleMgrService::GetSystemAvailableCapabilities(std::vector<std::string> &systemCaps)
{
    std::cout<<"GetSystemAvailableCapabilities"<<std::endl;
    return true;
}
bool BundleMgrService::IsSafeMode()
{
    std::cout<<"IsSafeMode"<<std::endl;
    return true;
}
bool BundleMgrService::CleanBundleCacheFiles(
    const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback)
{
    std::cout<<"CleanBundleCacheFiles"<<std::endl;
    return true;
}
bool BundleMgrService::CleanBundleDataFiles(const std::string &bundleName)
{
    std::cout<<"CleanBundleDataFiles"<<std::endl;
    return true;
}
bool BundleMgrService::RegisterBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    std::cout<<"RegisterBundleStatusCallback"<<std::endl;
    return true;
}
bool BundleMgrService::ClearBundleStatusCallback(const sptr<IBundleStatusCallback> &bundleStatusCallback)
{
    std::cout<<"ClearBundleStatusCallback"<<std::endl;
    return true;
}
bool BundleMgrService::UnregisterBundleStatusCallback()
{
    std::cout<<"UnregisterBundleStatusCallback"<<std::endl;
    return true;
}
bool BundleMgrService::DumpInfos(const MockDumpFlag flag, const std::string &bundleName, std::string &result)
{
    std::cout<<"DumpInfos"<<std::endl;
    return true;
}
bool BundleMgrService::IsApplicationEnabled(const std::string &bundleName)
{
    std::cout<<"IsApplicationEnabled"<<std::endl;
    return true;
}
bool BundleMgrService::SetApplicationEnabled(const std::string &bundleName, bool isEnable)
{
    std::cout<<"SetApplicationEnabled"<<std::endl;
    return true;
}
bool BundleMgrService::IsAbilityEnabled(const AbilityInfo &abilityInfo)
{
    std::cout<<"IsAbilityEnabled"<<std::endl;
    return true;
}
bool BundleMgrService::SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnabled)
{
    std::cout<<"SetAbilityEnabled"<<std::endl;
    return true;
}
std::string BundleMgrService::GetAbilityIcon(const std::string &bundleName, const std::string &className)
{
    std::cout<<"GetAbilityIcon"<<std::endl;
    return "BundleName";
}
bool BundleMgrService::CanRequestPermission(
    const std::string &bundleName, const std::string &permissionName, const int userId)
{
    std::cout<<"CanRequestPermission"<<std::endl;
    return true;
}
bool BundleMgrService::RequestPermissionFromUser(
    const std::string &bundleName, const std::string &permission, const int userId)
{
    std::cout<<"RequestPermissionFromUser"<<std::endl;
    return true;
}
bool BundleMgrService::RegisterAllPermissionsChanged(const sptr<OnPermissionChangedCallback> &callback)
{
    std::cout<<"RegisterAllPermissionsChanged"<<std::endl;
    return true;
}
bool BundleMgrService::RegisterPermissionsChanged(
    const std::vector<int> &uids, const sptr<OnPermissionChangedCallback> &callback)
{
    std::cout<<"RegisterPermissionsChanged"<<std::endl;
    return true;
}
bool BundleMgrService::UnregisterPermissionsChanged(const sptr<OnPermissionChangedCallback> &callback)
{
    std::cout<<"UnregisterPermissionsChanged"<<std::endl;
    return true;
}

sptr<IBundleInstaller> BundleMgrService::GetBundleInstaller()
{
    return nullptr;
}

bool BundleMgrService::GetAllFormsInfo(std::vector<FormInfo> &formInfos)
{
    return true;
}
bool BundleMgrService::GetFormsInfoByApp(const std::string &bundleName, std::vector<FormInfo> &formInfos)
{
    return true;
}
bool BundleMgrService::GetFormsInfoByModule(
    const std::string &bundleName, const std::string &moduleName, std::vector<FormInfo> &formInfos)
{
    return true;
}
bool BundleMgrService::GetShortcutInfos(const std::string &bundleName, std::vector<ShortcutInfo> &shortcutInfos)
{
    return true;
}
bool BundleMgrService::GetApplicationInfo(
    const std::string &appName, const ApplicationFlag flag, const int userId, ApplicationInfo &appInfo)
{
    std::cout<<"GetApplicationInfo"<<std::endl;
    return true;
}
bool BundleMgrService::GetApplicationInfos(
    const ApplicationFlag flag, const int userId, std::vector<ApplicationInfo> &appInfos)
{
    return true;
}
int BundleMgrService::CheckPermission(const std::string &bundleName, const std::string &permission)
{
    return 0;
}
bool BundleMgrService::GetBundleInfo(const std::string &bundleName, const BundleFlag flag, BundleInfo &bundleInfo)
{
    std::cout<<"GetBundleInfo"<<std::endl;
    bundleInfo.name = "bundleInfo.name";
    bundleInfo.label = "bundleInfo.label";
    bundleInfo.description = "bundleInfo.description";
    bundleInfo.vendor = "bundleInfo.vendor";
    bundleInfo.versionCode = 0;
    bundleInfo.versionName = "bundleInfo.versionName";
    bundleInfo.jointUserId = "bundleInfo.jointUserId";
    bundleInfo.minSdkVersion = -1;
    bundleInfo.maxSdkVersion = -1;
    bundleInfo.mainEntry = "bundleInfo.mainEntry";
    bundleInfo.cpuAbi = "bundleInfo.cpuAbi";
    bundleInfo.appId = "bundleInfo.appId";
    bundleInfo.compatibleVersion = 0;
    bundleInfo.targetVersion = 0;
    bundleInfo.releaseType = "bundleInfo.releaseType";
    bundleInfo.uid = -1;
    bundleInfo.gid = -1;
    bundleInfo.seInfo = "bundleInfo.seInfo";
    bundleInfo.entryModuleName = "bundleInfo.entryModuleName";
    bundleInfo.isKeepAlive = false;
    bundleInfo.isNativeApp = false;
    bundleInfo.isDifferentName = false;
    bundleInfo.installTime = 0;
    bundleInfo.updateTime = 0;
    ApplicationInfo applicationInfo;
    applicationInfo.name = "applicationInfo.name";
    applicationInfo.bundleName = "applicationInfo.bundleName";
    applicationInfo.description = "applicationInfo.description";
    applicationInfo.iconPath = "applicationInfo.iconPath";
    applicationInfo.label = "applicationInfo.label";
    applicationInfo.labelId = 0;
    applicationInfo.iconId = 0;
    applicationInfo.descriptionId = 0;
    applicationInfo.deviceId = "applicationInfo.deviceId";
    applicationInfo.signatureKey = "applicationInfo.signatureKey";
    applicationInfo.isSystemApp = false;
    applicationInfo.isLauncherApp = true;
    applicationInfo.supportedModes = 0;
    applicationInfo.process = "applicationInfo.process";
    applicationInfo.entryDir = "applicationInfo.entryDir";
    applicationInfo.codePath = "applicationInfo.codePath";
    applicationInfo.dataDir = "applicationInfo.dataDir";
    applicationInfo.dataBaseDir = "applicationInfo.dataBaseDir";
    applicationInfo.cacheDir = "applicationInfo.cacheDir";
    applicationInfo.flags = 0;
    applicationInfo.enabled = false;

    bundleInfo.applicationInfo = applicationInfo;
    std::vector<std::string> reqPermissions;
    if (bundleName == "bundleName1024") {
        int max_count_1025 = 1025;
        for (int num1 = 1; num1 <= max_count_1025; num1++) {
            reqPermissions.push_back("reqPermissions" + std::to_string(num1));
        }
    } else {
        int max_count = 5;
        for (int num2 = 1; num2 <= max_count; num2++) {
            reqPermissions.push_back("reqPermissions" + std::to_string(num2));
        }
    }

    bundleInfo.reqPermissions = reqPermissions;
    std::vector<std::string> defPermissions;
    int def_max_count = 5;
    for (int num3 = 1; num3 <= def_max_count; num3++) {
        defPermissions.push_back("defPermissions" + std::to_string(num3));
    }
    bundleInfo.defPermissions = defPermissions;
    return true;
}
bool BundleMgrService::GetBundleInfos(const BundleFlag flag, std::vector<BundleInfo> &bundleInfos)
{
    std::cout<<"GetBundleInfos"<<std::endl;
    return true;
}
int BundleMgrService::GetUidByBundleName(const std::string &bundleName, const int userId)
{
    std::cout<<"GetUidByBundleName"<<std::endl;
    return 0;
}
std::string BundleMgrService::GetAppIdByBundleName(const std::string &bundleName, const int userId)
{
    std::cout<<"GetAppIdByBundleName"<<std::endl;
    return "BundleName";
}
bool BundleMgrService::GetBundleNameForUid(const int uid, std::string &bundleName)
{
    std::cout<<"GetBundleNameForUid"<<std::endl;
    if (uid == 1024) {
        bundleName = "bundleName1024";
        return true;
    }
    if (uid >= 0) {
        bundleName = "bundleName";
        return true;
    }
    return false;
}
bool BundleMgrService::GetBundlesForUid(const int uid, std::vector<std::string> &bundleNames)
{
    std::cout<<"GetBundlesForUid"<<std::endl;
    if (uid == 1024) {
        bundleNames.push_back("bundleName1024");
        return true;
    }
    if (uid >= 0) {
        bundleNames.push_back("bundleName");
        return true;
    }
    return false;
}
bool BundleMgrService::GetNameForUid(const int uid, std::string &name)
{
    std::cout<<"GetNameForUid"<<std::endl;
    return true;
}
bool BundleMgrService::GetBundleGids(const std::string &bundleName, std::vector<int> &gids)
{
    std::cout<<"GetBundleGids"<<std::endl;
    return true;
}
std::string BundleMgrService::GetAppType(const std::string &bundleName)
{
    std::cout<<"GetAppType"<<std::endl;
    return "launcher";
}
bool BundleMgrService::CheckIsSystemAppByUid(const int uid)
{
    std::cout<<"CheckIsSystemAppByUid"<<std::endl;
    return true;
}
bool BundleMgrService::GetBundleInfosByMetaData(const std::string &metaData, std::vector<BundleInfo> &bundleInfos)
{
    std::cout<<"GetBundleInfosByMetaData"<<std::endl;
    return true;
}
bool BundleMgrService::QueryAbilityInfo(const Want &want, AbilityInfo &abilityInfo)
{
    std::cout<<"QueryAbilityInfo"<<std::endl;
    return true;
}
bool BundleMgrService::QueryAbilityInfoByUri(const std::string &abilityUri, AbilityInfo &abilityInfo)
{
    std::cout<<"QueryAbilityInfoByUri"<<std::endl;
    return true;
}
bool BundleMgrService::QueryKeepAliveBundleInfos(std::vector<BundleInfo> &bundleInfos)
{
    std::cout<<"QueryKeepAliveBundleInfos"<<std::endl;
    return true;
}
std::string BundleMgrService::GetAbilityLabel(const std::string &bundleName, const std::string &className)
{
    std::cout<<"GetAbilityLabel"<<std::endl;
    return "BundleName";
}
bool BundleMgrService::GetBundleArchiveInfo(
    const std::string &hapFilePath, const BundleFlag flag, BundleInfo &bundleInfo)
{
    std::cout<<"GetBundleArchiveInfo"<<std::endl;
    return true;
}
bool BundleMgrService::GetHapModuleInfo(const AbilityInfo &abilityInfo, HapModuleInfo &hapModuleInfo)
{
    std::cout<<"GetHapModuleInfo"<<std::endl;
    return true;
}
bool BundleMgrService::GetLaunchWantForBundle(const std::string &bundleName, Want &want)
{
    std::cout<<"GetLaunchWantForBundle"<<std::endl;
    return true;
}
int BundleMgrService::CheckPublicKeys(const std::string &firstBundleName, const std::string &secondBundleName)
{
    std::cout<<"CheckPublicKeys"<<std::endl;
    return 0;
}
bool BundleMgrService::GetPermissionDef(const std::string &permissionName, PermissionDef &permissionDef)
{
    std::cout<<"GetPermissionDef"<<std::endl;
    return false;
}
bool BundleMgrService::GetAllPermissionGroupDefs(std::vector<PermissionDef> &permissionDefs)
{
    std::cout<<"GetAllPermissionGroupDefs"<<std::endl;
    return true;
}

}  // namespace AppExecFwk
}  // namespace OHOS
