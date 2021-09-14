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

#ifndef OHOS_SYSTEM_TEST_DISTRIBUTED_PERMISSION_UTIL_H
#define OHOS_SYSTEM_TEST_DISTRIBUTED_PERMISSION_UTIL_H
#include <memory>
#include <mutex>
#include <cstdio>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <future>
#include "permission_log.h"
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "common_tool.h"
#include "install_param.h"
#include "bundle_constants.h"
#include "status_receiver_host.h"
#include "i_permission_manager.h"
#include "device_manager.h"

namespace OHOS {
namespace STPermissionUtil {

namespace {
const std::string THIRD_BUNDLE_PATH = "/data/";
const std::string BASE_BUNDLE_NAME = "com.vendor.hiworld.example";
const std::string ERROR_INSTALL_FAILED = "install failed!";
const std::string MSG_SUCCESS = "[SUCCESS]";
const std::string OPERATION_FAILED = "Failure";
const std::string OPERATION_SUCCESS = "Success";
const int32_t DEFAULT_USERID = 0;
const std::string SYSTEM_BUNDLE_NAME = "com.system.hiworld.examples1";
const std::string SYSTEM_HAP_NAME = "DPSystemBundle";
const std::string THIRD_BUNDLE_NAME = "com.third.hiworld.example1";
const std::string DPMS_THIRD_BUNDLE_NAME = "com.third.hiworld.dpmsExample";
const std::string SYSTEM_WriteContacts_HAP_NAME = "DPSystemWriteContactsBundle";
const std::string THIRD_HAP_NAME = "DPThirdBundle";
const std::string DPMS_THIRD_HAP_NAME = "dpmsThirdBundle";
const std::string DPMS_PERMISSION_RECORD_HAP_NAME1 = "PURTest1";
const std::string DPMS_PERMISSION_RECORD_HAP_NAME2 = "PURTest2";
const std::string DPMS_PERMISSION_RECORD_HAP1 = "com.system.hiworld.PURTest_C1";
const std::string DPMS_PERMISSION_RECORD_HAP2 = "com.system.hiworld.PURTest_C2";
const std::string System_OnlySystemGrant_HAP_NAME = "dpmsSystemBundleOnlySystemGrant";
const std::string System_OnlySystemGrant_BUNDLE_NAME = "com.system.hiworld.dpms_system_grant";
const std::string THIRD_MORETHAN_MAXPERMISSION_HAP_NAME = "dpmsThirddMaxPermissionExceeded";
const std::string THIRD_MORETHAN_MAXPERMISSION_BUNDLE_NAME = "com.third.hiworld.dpmsThirddMaxPermissionExceeded";
const std::string THIRD_EQ_MAXPERMISSION_HAP_NAME = "dpmsThirddEqMaxPermission";
const std::string THIRD_EQ_MAXPERMISSION_BUNDLE_NAME = "com.third.hiworld.dpmsThirddEqMaxPermission";
const std::string THIRD_HAP_NAME2 = "dpmsThirdBundle";
const std::string THIRD_BUNDLE_NAME2 = "com.third.hiworld.dpmsExample";
const std::string SYSTEM_HAP_NAME2 = "dpmsSystemBundle";
const std::string SYSTEM_BUNDLE_NAME2 = "com.system.hiworld.dpms_system_bundle";
const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_HAP_NAME = "dpmsSystemIncludeUseByLocalApp";
const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME = "com.system.hiworld.include_use_by_local_app";
const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1 = "dpmsSystemIncludeUseByLocalAppAddV1";
const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1 = "com.system.hiworld.include_use_by_local_app";
const std::string THIRD_INCLUDE_USE_BY_lOCAL_HAP_NAME = "dpmsThirdIncludeUseByLocalApp";
const std::string THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME = "com.third.hiworld.include_use_by_local_app";
const std::string THIRD_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1 = "dpmsThirdIncludeUseByLocalAppAddV1";
const std::string THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1 = "com.third.hiworld.include_use_by_local_app";
}  // namespace

class StatusReceiverImpl : public OHOS::AppExecFwk::StatusReceiverHost {
public:
    StatusReceiverImpl();
    virtual ~StatusReceiverImpl();
    virtual void OnStatusNotify(const int progress) override;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;
    std::string GetResultMsg() const;

private:
    mutable std::promise<std::string> resultMsgSignal_;

    DISALLOW_COPY_AND_MOVE(StatusReceiverImpl);
};
class STDistibutePermissionUtil {

public:
    constexpr static double MAX_ELAPSED = 1.5;
    static void Install(const std::string &hapName);
    static void ReInstall(const std::string &hapName);
    static void Upgrade(const std::string &hapName);
    static void Uninstall(const std::string &bundleName);
    static sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    static int GetBundleInfoByUid(int uid, OHOS::AppExecFwk::BundleInfo &bundleInfo);
    static bool GetBundleInfoByBundleName(std::string bundleName, OHOS::AppExecFwk::BundleInfo &bundleInfo);
    static int GetUidByBundleName(std::string bundleName);
    static bool GetBundleNameForUid(const int uid, std::string bundleName);
    static int GrantUserGrantedPermission(const std::string &bundleName, const std::string &permissionName, int userId);
    static int RevokeUserGrantedPermission(
        const std::string &bundleName, const std::string &permissionName, int userId);
    static int32_t GetTrustedDeviceList(
        std::string &packageName, std::vector<OHOS::DistributedHardware::DmDeviceInfo> &deviceList);

private:
    static void BMSInstall(const std::string &bundleFilePath, const OHOS::AppExecFwk::InstallFlag installFlag,
        std::vector<std::string> &resvec);
    static sptr<OHOS::AppExecFwk::IBundleInstaller> GetInstallerProxy();
    static bool GetBundleInfoByName(
        const std::string &appName, OHOS::AppExecFwk::BundleFlag flag, OHOS::AppExecFwk::BundleInfo &bundleInfo);
    static sptr<OHOS::Security::Permission::IPermissionManager> GetPermissionProxy();
};  // namespace DistributedHardware

}  // namespace STPermissionUtil
}  // namespace OHOS
#endif  // OHOS_SYSTEM_TEST_ABILITY_UTIL_H