/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <chrono>
#include <gtest/gtest.h>
#include <mutex>
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "ipc_skeleton.h"
#include "time_util.h"
#include "distributed_permission_kit.h"
#include "bundle_info.h"
#include "app_id_info.h"
#include "permission_definition.h"
#include "parameter.h"
#include "permission_kit.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "app_mgr_service.h"
#include "module_test_dump_util.h"
#include "system_test_ability_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "ability_lifecycle_executor.h"
#include "device_info_repository.h"
#include "distributed_uid_allocator.h"

#define CMD_RESULT_BUF_SIZE 1024

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STtools;
using namespace OHOS::STABUtil;
using namespace OHOS::DistributedHardware;
using std::string;

namespace {
const string PERMISSION_NAME = "ohos.permission.GET_NERWORK_INFO";
const string NULL_PERMISSION_NAME = "";
string INVALID_PERMISSION_NAME = "";
const int32_t ROOT_UID = 12600000;
const int32_t SYSTEM_UID_TEST = 12601000;
const int32_t DUID = 12610000;
const int32_t INVALID_DUID = -1;
const string NODE_ID = "1001";
const string NULL_NODE_ID = "";
const string INVALID_NODE_ID = "12345678123456781234567812345678123456781234567812345678123456789";
const int32_t PID = getpid();
const string RESTRICTED_PERMISSION = "ohos.permission.READ_CONTACTS";
const string DEVICE_ID = "1001";
const int32_t GRANTED = 0;
const int32_t DENIED = -1;

std::recursive_mutex statckLock_;
string deviceId_;
int32_t pid_ = 0;
int32_t uid_ = 0;
int32_t hapRet = 0;
OHOS::AppExecFwk::BundleInfo bundleInfo_;
}  // namespace
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "MonitorManagerTest"};
}
class CheckDistributedPermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    void GetIPCSkeleton();
    string GetAppIdInfo(string &deviceId_);
    static bool SubscribeEvent();

    class AppEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;
    };

    static std::vector<std::string> eventList_;
    static STtools::Event event_;
    static sptr<OHOS::AppExecFwk::IAppMgr> appMs_;
    static sptr<OHOS::AAFwk::IAbilityManager> abilityMs_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;
};

std::vector<std::string> CheckDistributedPermissionTest::eventList_ = {
    "resp_com_ohos_check_self_permission_app_a1", "req_com_ohos_check_self_permission_app_a1"};
STtools::Event CheckDistributedPermissionTest::event_ = STtools::Event();
sptr<OHOS::AppExecFwk::IAppMgr> CheckDistributedPermissionTest::appMs_ = nullptr;
sptr<OHOS::AAFwk::IAbilityManager> CheckDistributedPermissionTest::abilityMs_ = nullptr;
std::shared_ptr<CheckDistributedPermissionTest::AppEventSubscriber> CheckDistributedPermissionTest::subscriber_ =
    nullptr;

bool CheckDistributedPermissionTest::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    for (const auto &e : CheckDistributedPermissionTest::eventList_) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void CheckDistributedPermissionTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    GTEST_LOG_(INFO) << "OnReceiveEvent: ret=" << data.GetWant().GetIntParam("ret", hapRet);
    GTEST_LOG_(INFO) << "OnReceiveEvent: hapRet=" << hapRet;

    auto eventName = data.GetWant().GetAction();
    auto iter = std::find(CheckDistributedPermissionTest::eventList_.begin(),
        CheckDistributedPermissionTest::eventList_.end(),
        eventName);
    if (iter != CheckDistributedPermissionTest::eventList_.end()) {
        STAbilityUtil::Completed(event_, data.GetData(), data.GetCode());
    }
}

void CheckDistributedPermissionTest::SetUpTestCase(void)
{
    SubscribeEvent();
    appMs_ = STAbilityUtil::GetAppMgrService();
    abilityMs_ = STAbilityUtil::GetAbilityManagerService();
    if (appMs_) {
        appMs_->SetAppFreezingTime(60);
        int time = 0;
        appMs_->GetAppFreezingTime(time);
        std::cout << "appMs_->GetAppFreezingTime();" << time << std::endl;
    }
}

void CheckDistributedPermissionTest::TearDownTestCase(void)
{}

void CheckDistributedPermissionTest::SetUp(void)
{
    GetIPCSkeleton();
}

void CheckDistributedPermissionTest::TearDown(void)
{}

void CheckDistributedPermissionTest::GetIPCSkeleton()
{
    pid_ = 0;
    uid_ = IPCSkeleton::GetCallingUid();
    deviceId_ = IPCSkeleton::GetLocalDeviceID();
}

// Invalid permission name,the length of permission name > 256.
string GetInValidPermission()
{
    for (int i = 0; i < 258; i++) {
        string str = "h";
        INVALID_PERMISSION_NAME = INVALID_PERMISSION_NAME + str;
    }
    return INVALID_PERMISSION_NAME;
}

/**
 * @tc.number : DPMS_CheckDPermission_0100
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is invalid, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0100 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckDPermission(INVALID_DUID, PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0100 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0200
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is a root uid, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0200 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckDPermission(ROOT_UID, PERMISSION_NAME);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0200 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0300
 * @tc.name : CheckDPermission
 * @tc.desc : If the dUid is a system uid, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0300 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckDPermission(SYSTEM_UID_TEST, PERMISSION_NAME);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0300 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0400
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is valid, but the permissionName is null, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0400 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckDPermission(DUID, NULL_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0400 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0500
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is valid, but the length of permission > 256, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0500 start";
    int32_t RESULT = -1;
    string INVALID_PERMISSION_NAME = GetInValidPermission();
    RESULT = DistributedPermissionKit::CheckDPermission(DUID, INVALID_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0500 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0700
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is valid, the length of permission <= 256, the non-sensitive permission is existent and the
 * permission is granted, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0700, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0700 start";
    int32_t RESULT = -1;
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    RESULT = DistributedPermissionKit::CheckDPermission(duid, permission_wifi);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0700 end";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
}

/**
 * @tc.number : DPMS_CheckDPermission_0800
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is valid, the length of permission <= 256, and the sensitive permission is existent,
 * return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0800, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_camera = "ohos.permission.CAMERA";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0800 start";
    int32_t RESULT = -1;
    DistributedPermissionKit::GrantSensitivePermissionToRemoteApp(permission_camera, deviceId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    RESULT = DistributedPermissionKit::CheckDPermission(duid, permission_camera);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0800 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0900
 * @tc.name : CheckDPermission
 * @tc.desc : If the duid is valid, the length of permission <= 256, and the sensitive permission is non-existent,
 * return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_0900, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_health = "ohos.permission.READ_HEALTH_DATA";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0900 start";
    int32_t RESULT = -1;
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    RESULT = DistributedPermissionKit::CheckDPermission(duid, permission_health);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_0900 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_1000
 * @tc.name : CheckDPermission
 * @tc.desc : interface performance
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckDPermission_1000, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_1000 start";
    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::CheckDPermission(duid, permission_wifi);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckDPermission_1000 end";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
}

/**
 * @tc.number : DPMS_CheckPermission_0100
 * @tc.name : CheckPermission
 * @tc.desc : If the permissionName is null，return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0100 start";
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, DUID, NODE_ID);
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckPermission(NULL_PERMISSION_NAME, appIdInfo);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0100 end";
}

/**
 * @tc.number : DPMS_CheckDPermission_0200
 * @tc.name : CheckPermission
 * @tc.desc : If the length of permission name > 256, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0200 start";
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, DUID, NODE_ID);
    int32_t RESULT = -1;
    string INVALID_PERMISSION_NAME = GetInValidPermission();
    RESULT = DistributedPermissionKit::CheckPermission(INVALID_PERMISSION_NAME, appIdInfo);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0200 end";
}

/**
 * @tc.number : DPMS_CheckPermission_0300
 * @tc.name : CheckPermission
 * @tc.desc : If the length of permission name <= 256, the uid is valid duid, and the permission is granted,
 * return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0300, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0300 start";
    int32_t RESULT = -1;
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, duid, deviceId);
    RESULT = DistributedPermissionKit::CheckPermission(permission_wifi, appIdInfo);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0300 end";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
}

/**
 * @tc.number : DPMS_CheckPermission_0400
 * @tc.name : CheckPermission
 * @tc.desc : Invalid nodeId(the length of nodeId >64), return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0400 start";
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, DUID, INVALID_NODE_ID);
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckPermission(PERMISSION_NAME, appIdInfo);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0400 end";
}

/**
 * @tc.number : DPMS_CheckPermission_0500
 * @tc.name : CheckPermission
 * @tc.desc : If the nodeId is null, the permission name is valid, and the uid is not special duid, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0500, TestSize.Level1)
{
    STDistibutePermissionUtil::Install("dpmsThirdBundle");
    std::string bundleName = "com.third.hiworld.dpmsExample";
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_network);
    permList_user.push_back(permission_wifi);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_network, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_wifi, 0);
    int32_t UID = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, UID, NULL_NODE_ID);
    int32_t RESULT = -1;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0500 start";
    RESULT = DistributedPermissionKit::CheckPermission(permission_network, appIdInfo);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0500 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_CheckPermission_0600
 * @tc.name : CheckPermission
 * @tc.desc : If the nodeId is the local deviceId, the permission name is valid, and the uid is not special
 * duid, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0600, TestSize.Level1)
{
    STDistibutePermissionUtil::Install("dpmsThirdBundle");
    std::string bundleName = "com.third.hiworld.dpmsExample";
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_network);
    permList_user.push_back(permission_wifi);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_network, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_wifi, 0);
    int32_t UID = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    char LOCAL_DEVICE_ID[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(LOCAL_DEVICE_ID, Constant::DEVICE_UUID_LENGTH);
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, UID, LOCAL_DEVICE_ID);
    int32_t RESULT = -1;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0600 start";
    RESULT = DistributedPermissionKit::CheckPermission(permission_network, appIdInfo);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0600 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_CheckPermission_0700
 * @tc.name : CheckPermission
 * @tc.desc : If the nodeId is not the local deviceId and the uid is non-special duid, the permission cache information
 * is non-existent, and the permission is existent in the map, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0700, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0700 start";
    int32_t RESULT = -1;
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, uid, deviceId);
    RESULT = DistributedPermissionKit::CheckPermission(permission_wifi, appIdInfo);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0700 end";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
}

/**
 * @tc.number : DPMS_CheckPermission_0800
 * @tc.name : CheckPermission
 * @tc.desc : If the nodeId is not the local deviceId and the uid is non-special duid, the permission information
 * is existent, and the permission is existent in the map, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0800, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0800 start";
    int32_t RESULT = -1;
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, duid, deviceId);
    RESULT = DistributedPermissionKit::CheckPermission(permission_wifi, appIdInfo);
    EXPECT_EQ(RESULT, GRANTED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0800 end";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
}

/**
 * @tc.number : DPMS_CheckPermission_0900
 * @tc.name : CheckPermission
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckPermission_0900, TestSize.Level1)
{
    std::string bundleName = "com.third.hiworld.include_use_by_local_app";
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalAppAddV1");
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(bundleName);
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto iter = list.begin();
    for (; iter != list.end(); iter++) {
        DeviceInfo info = (*iter);
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device name: %{public}s", info.deviceName.c_str());
        PERMISSION_LOG_DEBUG(
            LABEL, ">>> DistributedPermissionDuidTransformTest device type: %{public}s", info.deviceType.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device network id: %{public}s",
            info.deviceId.networkId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device udid: %{public}s",
            info.deviceId.uniqueDisabilityId.c_str());
        PERMISSION_LOG_DEBUG(LABEL,
            ">>> DistributedPermissionDuidTransformTest device uuid: %{public}s",
            info.deviceId.universallyUniqueId.c_str());
    }
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    std::string deviceId = deviceList[0].deviceId;
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0900 start";
    int32_t RESULT = -1;
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, uid, deviceId);
    auto startTime = std::chrono::high_resolution_clock::now();
    RESULT = DistributedPermissionKit::CheckPermission(permission_wifi, appIdInfo);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckPermission_0900 end";
    STDistibutePermissionUtil::Uninstall("com.third.hiworld.include_use_by_local_app");
}

/**
 * @tc.number : DPMS_CheckSelfPermission_0100
 * @tc.name : CheckSelfPermission
 * @tc.desc : If the length of permission name <= 256, and the permission is granted, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckSelfPermission_0100, TestSize.Level1)
{
    std::string hapName = "checkSelfPermissionBundle1";
    std::string abilityName = "CheckSelfPermissionAbilityA1";
    std::string bundleName = "com.third.checkSelfPermission";
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    STDistibutePermissionUtil::Install(hapName);
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_network);
    permList_user.push_back(permission_wifi);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_network, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_wifi, 0);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0100 start";
    MAP_STR_STR params;
    params["permName"] = permission_network;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    STAbilityUtil::PublishEvent("req_com_ohos_check_self_permission_app_a1", 100, "DPMS_CheckSelfPermission_0100");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CheckSelfPermission0100", GRANTED, 2), 0);
    bool ret = STAbilityUtil::StopAbility("req_com_ohos_check_self_permission_app_a1", 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0100 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_CheckSelfPermission_0200
 * @tc.name : CheckSelfPermission
 * @tc.desc : If the permission name is null, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckSelfPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0200 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckSelfPermission(NULL_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0200 end";
}

/**
 * @tc.number : DPMS_CheckSelfPermission_0300
 * @tc.name : CheckSelfPermission
 * @tc.desc : If the length of permission name> 256, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckSelfPermission_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0300 start";
    int32_t RESULT = -1;
    string INVALID_PERMISSION_NAME = GetInValidPermission();
    RESULT = DistributedPermissionKit::CheckSelfPermission(INVALID_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0300 end";
}

/**
 * @tc.number : DPMS_CheckSelfPermission_0400
 * @tc.name : CheckSelfPermission
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckSelfPermission_0400, TestSize.Level1)
{
    std::string hapName = "checkSelfPermissionBundle1";
    std::string abilityName = "CheckSelfPermissionAbilityA1";
    std::string bundleName = "com.third.checkSelfPermission";
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    STDistibutePermissionUtil::Install(hapName);
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_network);
    permList_user.push_back(permission_wifi);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_network, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_wifi, 0);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0400 start";
    MAP_STR_STR params;
    params["permName"] = permission_network;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    int32_t pass = 0;
    STAbilityUtil::PublishEvent("req_com_ohos_check_self_permission_app_a1", 100, "DPMS_CheckSelfPermission_0400");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CheckSelfPermission0400", pass, 2), 0);
    bool ret = STAbilityUtil::StopAbility("req_com_ohos_check_self_permission_app_a1", 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckSelfPermission_0400 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_CheckCallingPermission_0100
 * @tc.name : CheckCallingPermission
 * @tc.desc : If the permission name is null, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingPermission_0100 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckCallingPermission(NULL_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingPermission_0100 end";
}

/**
 * @tc.number : DPMS_CheckCallingPermission_0200
 * @tc.name : CheckCallingPermission
 * @tc.desc : If the length of permission name > 256, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingPermission_0200 start";
    int32_t RESULT = -1;
    string INVALID_PERMISSION_NAME = GetInValidPermission();
    RESULT = DistributedPermissionKit::CheckCallingPermission(INVALID_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingPermission_0200 end";
}

/**
 * @tc.number : DPMS_CheckCallingPermission_0600
 * @tc.name : CheckCallingPermission
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingPermission_0600, TestSize.Level1)
{
    std::string hapName = "checkSelfPermissionBundle1";
    std::string abilityName = "CheckSelfPermissionAbilityA1";
    std::string bundleName = "com.third.checkSelfPermission";
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    STDistibutePermissionUtil::Install(hapName);
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_network);
    permList_user.push_back(permission_wifi);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_network, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_wifi, 0);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingPermission_0600 start";
    MAP_STR_STR params;
    params["permName"] = permission_network;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    STAbilityUtil::PublishEvent("req_com_ohos_check_self_permission_app_a1", 100, "DPMS_CheckCallingPermission_0600");
    int pass = 0;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CheckCallingPermission0600", pass, 2), 0);
    bool ret = STAbilityUtil::StopAbility("req_com_ohos_check_self_permission_app_a1", 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingPermission_0600 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_CheckCallingOrSelfPermission_0100
 * @tc.name : CheckCallingOrSelfPermission
 * @tc.desc : If the permission name is null, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingOrSelfPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0100 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::CheckCallingOrSelfPermission(NULL_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0100 end";
}

/**
 * @tc.number : DPMS_CheckCallingOrSelfPermission_0200
 * @tc.name : CheckCallingOrSelfPermission
 * @tc.desc : If the length of permission name > 256, return: -1.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingOrSelfPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0200 start";
    int32_t RESULT = -1;
    string INVALID_PERMISSION_NAME = GetInValidPermission();
    RESULT = DistributedPermissionKit::CheckCallingOrSelfPermission(INVALID_PERMISSION_NAME);
    EXPECT_EQ(RESULT, DENIED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0200 end";
}

/**
 * @tc.number : DPMS_CheckCallingOrSelfPermission_0300
 * @tc.name : CheckCallingOrSelfPermission
 * @tc.desc : If the length of permission name <= 256, and the permission is granted, return: 0.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingOrSelfPermission_0300, TestSize.Level1)
{
    std::string hapName = "checkSelfPermissionBundle1";
    std::string abilityName = "CheckSelfPermissionAbilityA1";
    std::string bundleName = "com.third.checkSelfPermission";
    std::string permission_internet = "ohos.permission.INTERNET";
    STDistibutePermissionUtil::Install(hapName);
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_internet = {.permissionName = permission_internet,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_internet);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_internet);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_internet, 0);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0300 start";
    MAP_STR_STR params;
    params["permName"] = permission_internet;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    STAbilityUtil::PublishEvent(
        "req_com_ohos_check_self_permission_app_a1", 100, "DPMS_CheckCallingOrSelfPermission_0300");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CheckCallingOrSelfPermission0300", GRANTED, 2), 0);
    bool ret = STAbilityUtil::StopAbility("req_com_ohos_check_self_permission_app_a1", 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0300 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_CheckCallingOrSelfPermission_0400
 * @tc.name : CheckCallingOrSelfPermission
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallingOrSelfPermission_0400, TestSize.Level1)
{
    std::string hapName = "checkSelfPermissionBundle1";
    std::string abilityName = "CheckSelfPermissionAbilityA1";
    std::string bundleName = "com.third.checkSelfPermission";
    std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    STDistibutePermissionUtil::Install(hapName);
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_network);
    permList_user.push_back(permission_wifi);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_network, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_wifi, 0);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0400 start";
    MAP_STR_STR params;
    params["permName"] = permission_network;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    STAbilityUtil::PublishEvent(
        "req_com_ohos_check_self_permission_app_a1", 100, "DPMS_CheckCallingOrSelfPermission_0400");
    int pass = 0;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CheckCallingOrSelfPermission0400", pass, 2), 0);
    bool ret = STAbilityUtil::StopAbility("req_com_ohos_check_self_permission_app_a1", 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallingOrSelfPermission_0400 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_IsRestrictedPermission_0100
 * @tc.name : IsRestrictedPermission
 * @tc.desc : If the permission name is null, return: false.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_IsRestrictedPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0100 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::IsRestrictedPermission(NULL_PERMISSION_NAME);
    EXPECT_FALSE(RESULT);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0100 end";
}

/**
 * @tc.number : DPMS_IsRestrictedPermission_0200
 * @tc.name : CheckCallingOrSelfPermission
 * @tc.desc : If the length of permission name > 256, return: false.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_IsRestrictedPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0200 start";
    int32_t RESULT = -1;
    string INVALID_PERMISSION_NAME = GetInValidPermission();
    RESULT = DistributedPermissionKit::IsRestrictedPermission(INVALID_PERMISSION_NAME);
    EXPECT_FALSE(RESULT);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0200 end";
}

/**
 * @tc.number : DPMS_IsRestrictedPermission_0300
 * @tc.name : IsRestrictedPermission
 * @tc.desc : If the length of permission name <= 256, and the permission is restricted, return true.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_IsRestrictedPermission_0300, TestSize.Level1)
{
    STDistibutePermissionUtil::Install("dpmsThirdBundle");
    std::string bundleName = "com.third.hiworld.dpmsExample";
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_restricted = {.permissionName = RESTRICTED_PERMISSION,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 2,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_restricted);
    PermissionKit::AddDefPermissions(permDefList);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0300 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::IsRestrictedPermission(RESTRICTED_PERMISSION);
    EXPECT_TRUE(RESULT);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0300 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_IsRestrictedPermission_0400
 * @tc.name : IsRestrictedPermission
 * @tc.desc : If the length of permission name <= 256, but the permission is not restricted, return false.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_IsRestrictedPermission_0400, TestSize.Level1)
{
    STDistibutePermissionUtil::Install("dpmsThirdBundle");
    std::string bundleName = "com.third.hiworld.dpmsExample";
    std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    PermissionKit::AddDefPermissions(permDefList);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0400 start";
    int32_t RESULT = -1;
    RESULT = DistributedPermissionKit::IsRestrictedPermission(permission_network);
    EXPECT_FALSE(RESULT);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0400 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}

/**
 * @tc.number : DPMS_IsRestrictedPermission_0500
 * @tc.name : IsRestrictedPermission
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_IsRestrictedPermission_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0500 start";
    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::IsRestrictedPermission(PERMISSION_NAME);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_IsRestrictedPermission_0500 end";
}

/**
 * @tc.number : DPMS_CheckCallerPermission_0100
 * @tc.name : CheckCallerPermission
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CheckCallerPermission_0100, TestSize.Level1)
{
    std::string hapName = "checkSelfPermissionBundle1";
    std::string abilityName = "CheckSelfPermissionAbilityA1";
    std::string bundleName = "com.third.checkSelfPermission";
    std::string permission_internet = "ohos.permission.INTERNET";
    STDistibutePermissionUtil::Install(hapName);
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_internet = {.permissionName = permission_internet,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_internet);
    PermissionKit::AddDefPermissions(permDefList);
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_internet);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_internet, 0);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallerPermission_0100 start";
    MAP_STR_STR params;
    params["permName"] = permission_internet;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    STAbilityUtil::PublishEvent("req_com_ohos_check_self_permission_app_a1", 100, "DPMS_CheckCallerPermission_0100");
    int pass = 0;
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CheckCallerPermission0100", pass, 2), 0);
    bool ret = STAbilityUtil::StopAbility("req_com_ohos_check_self_permission_app_a1", 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CheckCallerPermission_0100 end";
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
    PermissionKit::RemoveDefPermissions(bundleName);
    STDistibutePermissionUtil::Uninstall(bundleName);
}
/**
 * @tc.number : DPMS_CreateAppIdInfo_pid_uid_0100
 * @tc.name : CreateAppIdInfo
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CreateAppIdInfo_pid_uid_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CreateAppIdInfo_pid_uid_0100 start";
    int32_t UID = ROOT_UID;
    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, UID);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CreateAppIdInfo_pid_uid_0100 end";
}

/**
 * @tc.number : DPMS_CreateAppIdInfo_pid_uid_deviceID_0100
 * @tc.name : CreateAppIdInfo
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CreateAppIdInfo_pid_uid_deviceID_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CreateAppIdInfo_pid_uid_deviceID_0100 start";
    int32_t UID = ROOT_UID;
    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, UID, DEVICE_ID);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CreateAppIdInfo_pid_uid_deviceID_0100 end";
}

/**
 * @tc.number : DPMS_CreateAppIdInfo_pid_uid_deviceID_bundleName_0100
 * @tc.name : CreateAppIdInfo
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_CreateAppIdInfo_pid_uid_deviceID_bundleName_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CreateAppIdInfo_pid_uid_deviceID_bundleName_0100 start";
    int32_t UID = ROOT_UID;
    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(
        PID, UID, DEVICE_ID, THIRD_MORETHAN_MAXPERMISSION_BUNDLE_NAME);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_CreateAppIdInfo_pid_uid_deviceID_bundleName_0100 end";
}

/**
 * @tc.number : DPMS_ParseAppIdInfo_appIdInfo_appIdInfoObj_0100
 * @tc.name : CreateAppIdInfo
 * @tc.desc : Interface performance.
 */
HWTEST_F(CheckDistributedPermissionTest, DPMS_ParseAppIdInfo_appIdInfo_appIdInfoObj_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_ParseAppIdInfo_appIdInfo_appIdInfoObj_0100 start";
    int32_t UID = ROOT_UID;
    string appIdInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(PID, UID, DEVICE_ID);
    AppIdInfo appIdInfoObj;
    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(appIdInfo, appIdInfoObj);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    GTEST_LOG_(INFO) << "runtime fp_ms =" << fp_ms.count();
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "CheckDistributedPermissionTest DPMS_ParseAppIdInfo_appIdInfo_appIdInfoObj_0100 end";
}