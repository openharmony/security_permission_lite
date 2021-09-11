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
#include "system_test_distributed_permission_util.h"
using namespace OHOS::AppExecFwk;
using namespace OHOS::AAFwk;
using namespace OHOS::DistributedHardware;

namespace OHOS {
namespace STPermissionUtil {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_PERMISSION, "STDistibutePermissionUtil"};
}
StatusReceiverImpl::StatusReceiverImpl()
{
    PERMISSION_LOG_INFO(LABEL, "create status receiver instance");
}

StatusReceiverImpl::~StatusReceiverImpl()
{
    PERMISSION_LOG_INFO(LABEL, "destroy status receiver instance");
}

void StatusReceiverImpl::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{
    PERMISSION_LOG_DEBUG(LABEL, "OnFinished result is %{public}d, %{public}s", resultCode, resultMsg.c_str());
    resultMsgSignal_.set_value(resultMsg);
}
void StatusReceiverImpl::OnStatusNotify(const int progress)
{
    PERMISSION_LOG_INFO(LABEL, "OnStatusNotify");
}

std::string StatusReceiverImpl::GetResultMsg() const
{
    auto future = resultMsgSignal_.get_future();
    future.wait();
    std::string resultMsg = future.get();
    if (resultMsg == MSG_SUCCESS) {
        return OPERATION_SUCCESS;
    } else {
        return OPERATION_FAILED + resultMsg;
    }
}

void STDistibutePermissionUtil::Install(const std::string &hapName)
{
    system(("chmod 644 /data/" + hapName + ".hap").c_str());
    system(("bm install -p /data/" + hapName + ".hap").c_str());
}

void STDistibutePermissionUtil::ReInstall(const std::string &hapName)
{
    system(("chmod 644 /data/" + hapName + ".hap").c_str());
    system(("bm install -r -p /data/" + hapName + ".hap").c_str());
}

void STDistibutePermissionUtil::Upgrade(const std::string &hapName)
{
    system(("chmod 644 /data/" + hapName + ".hap").c_str());
    system(("bm install -r -p /data/" + hapName + ".hap").c_str());
}

void STDistibutePermissionUtil::Uninstall(const std::string &bundleName)
{
    system(("bm uninstall -n " + bundleName).c_str());
}

sptr<IBundleMgr> STDistibutePermissionUtil::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        PERMISSION_LOG_ERROR(LABEL, "fail to get system ability mgr.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        PERMISSION_LOG_ERROR(LABEL, "fail to get bundle manager proxy.");
        return nullptr;
    }
    PERMISSION_LOG_INFO(LABEL, "get bundle manager proxy success.");
    return iface_cast<IBundleMgr>(remoteObject);
}

sptr<IBundleInstaller> STDistibutePermissionUtil::GetInstallerProxy()
{
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        PERMISSION_LOG_ERROR(LABEL, "bundle mgr proxy is nullptr.");
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        PERMISSION_LOG_ERROR(LABEL, "fail to get bundle installer proxy.");
        return nullptr;
    }

    PERMISSION_LOG_INFO(LABEL, "get bundle installer proxy success.");
    return installerProxy;
}

void STDistibutePermissionUtil::BMSInstall(
    const std::string &bundleFilePath, const InstallFlag installFlag, std::vector<std::string> &resvec)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    if (!installerProxy) {
        PERMISSION_LOG_ERROR(LABEL, "get bundle installer failed.");
        resvec.push_back(ERROR_INSTALL_FAILED);
        return;
    }
    InstallParam installParam;
    installParam.installFlag = installFlag;
    installParam.userId = Constants::DEFAULT_USERID;
    sptr<StatusReceiverImpl> statusReceiver = (new (std::nothrow) StatusReceiverImpl());
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    resvec.push_back(statusReceiver->GetResultMsg());
}

int STDistibutePermissionUtil::GetBundleInfoByUid(int uid, BundleInfo &bundleInfo)
{
    sptr<IBundleMgr> bundleMgrProxy = STDistibutePermissionUtil::GetBundleMgrProxy();
    std::string name;
    bool result = bundleMgrProxy->GetBundleNameForUid(uid, name);
    if (!result) {
        PERMISSION_LOG_ERROR(LABEL, "get bundle name failed.");
        return -1;
    }
    bool result2 = bundleMgrProxy->GetBundleInfo(name, BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (!result2) {
        PERMISSION_LOG_ERROR(LABEL, "get bundle info failed.");
        return -1;
    }
    return result2;
}

bool STDistibutePermissionUtil::GetBundleInfoByName(
    const std::string &bundleName, BundleFlag flag, BundleInfo &bundleInfo)
{
    sptr<IBundleMgr> bundleMgrProxy = STDistibutePermissionUtil::GetBundleMgrProxy();
    bool result = bundleMgrProxy->GetBundleInfo(bundleName, flag, bundleInfo);
    if (!result) {
        PERMISSION_LOG_ERROR(LABEL, "get bundle info by bundleName failed.");
        return -1;
    }
    return result;
}

bool STDistibutePermissionUtil::GetBundleInfoByBundleName(std::string bundleName, BundleInfo &bundleInfo)
{
    return GetBundleInfoByName(bundleName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
}

int STDistibutePermissionUtil::GetUidByBundleName(std::string bundleName)
{
    sptr<IBundleMgr> bundleMgrProxy = STDistibutePermissionUtil::GetBundleMgrProxy();
    return bundleMgrProxy->GetUidByBundleName(bundleName, DEFAULT_USERID);
}

bool STDistibutePermissionUtil::GetBundleNameForUid(const int uid, std::string bundleName)
{
    sptr<IBundleMgr> bundleMgrProxy = STDistibutePermissionUtil::GetBundleMgrProxy();
    return bundleMgrProxy->GetBundleNameForUid(uid, bundleName);
}

sptr<OHOS::Security::Permission::IPermissionManager> STDistibutePermissionUtil::GetPermissionProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        PERMISSION_LOG_ERROR(LABEL, "fail to get system ability mgr.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
        Security::Permission::IPermissionManager::SA_ID_PERMISSION_MANAGER_SERVICE);
    if (!remoteObject) {
        PERMISSION_LOG_ERROR(LABEL, "fail to get bundle manager proxy.");
        return nullptr;
    }

    PERMISSION_LOG_INFO(LABEL, "get bundle manager proxy success.");
    return iface_cast<Security::Permission::IPermissionManager>(remoteObject);
}

int STDistibutePermissionUtil::GrantUserGrantedPermission(
    const std::string &bundleName, const std::string &permissionName, int userId)
{
    sptr<OHOS::Security::Permission::IPermissionManager> permissionMgrProxy =
        STDistibutePermissionUtil::GetPermissionProxy();
    return permissionMgrProxy->GrantUserGrantedPermission(bundleName, permissionName, userId);
}

int STDistibutePermissionUtil::RevokeUserGrantedPermission(
    const std::string &bundleName, const std::string &permissionName, int userId)
{
    sptr<OHOS::Security::Permission::IPermissionManager> permissionMgrProxy =
        STDistibutePermissionUtil::GetPermissionProxy();
    return permissionMgrProxy->RevokeUserGrantedPermission(bundleName, permissionName, userId);
}

int32_t STDistibutePermissionUtil::GetTrustedDeviceList(std::string &packageName, std::vector<DmDeviceInfo> &deviceList)
{
    std::string extra = "";
    PERMISSION_LOG_INFO(LABEL, "GetTrustedDeviceList params : packageName = %{public}s", packageName.c_str());
    int32_t result = DeviceManager::GetInstance().GetTrustedDeviceList(packageName, extra, deviceList);
    return result;
}

}  // namespace STPermissionUtil
}  // namespace OHOS