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
#include <gtest/gtest.h>
#define private public
#include "distributed_permission_manager_service.h"
#include "distributed_permission_kit.h"
#include "on_request_permissions_result_stub.h"
#include "system_test_distributed_permission_util.h"
#include "sensitive_resource_switch_setting.h"
#include "resource_switch_cache.h"
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
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::AppExecFwk;
using namespace OHOS::STABUtil;
using namespace OHOS::EventFwk;
using namespace OHOS::STtools;
using std::string;
using MAP_STR_STR = std::map<std::string, std::string>;
namespace OHOS {
namespace Security {
namespace Permission {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "PermissionBmsManager"};
namespace {
const string long_string_65 = "long_string_long_string_long_string_long_string_long_string_long_";
const string long_string_257 =
    "long_string_long_string_long_string_long_string_long_string_long_string_long_long_string_long_string_long_string_"
    "long_string_long_string_long_long_string_long_string_long_string_long_string_long_string_long_long_string_long_"
    "string_long_string_long_string_lo";
int32_t hapRet = 0;
static const std::string SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED =
    "com.ohos.dpmsst.service.distributed.system.signed";
static const std::string SYSTEM_TEST_DISTRIBUTED_SYSTEM_SIGNED_ABILITY = "DpmsStServiceDistributedSystemSigned";
const std::string REQ_EVENT_NAME_SYSTEM_SIGNED = "req_com_ohos_dpmsst_service_distributed_system_signed";
static const std::string SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A = "com.ohos.dpmsst.service.distributed.third.a";
static const std::string SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY = "DpmsStServiceDistributedThirdA";
const std::string REQ_EVENT_NAME_THIRD_A = "req_com_ohos_dpmsst_service_distributed_third_a";
string deviceId_;
}  // namespace
class DistributedRequestPermissionFromRemoteTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void){};
    static bool SubscribeEvent();
    class AppEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;
    };
    void SetUp()
    {
        std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
        OHOS::Security::Permission::PermissionDef permissionDef_Camera = {.permissionName = Constant::CAMERA,
            .bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED,
            .grantMode = 0,
            .availableScope = 1 << 0,
            .label = "test label",
            .labelId = 9527,
            .description = "test description",
            .descriptionId = 9528};
        permDefList.emplace_back(permissionDef_Camera);
        PermissionKit::AddDefPermissions(permDefList);
        std::vector<std::string> permList_user;
        permList_user.push_back(Constant::CAMERA);
        PermissionKit::AddUserGrantedReqPermissions(
            SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED, permList_user, 0);
        PermissionKit::GrantUserGrantedPermission(
            SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED, Constant::CAMERA, 0);
    }
    void TearDown()
    {}

public:
    string GetAppIdInfo(int32_t pid, int32_t uid);
    static std::vector<std::string> eventList_;
    static STtools::Event event_;
    static sptr<OHOS::AppExecFwk::IAppMgr> appMs_;
    static sptr<OHOS::AAFwk::IAbilityManager> abilityMs_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;
};
class TestCallback : public OnRequestPermissionsResultStub {
public:
    TestCallback() = default;
    virtual ~TestCallback() = default;
    int32_t result = -1;
    void OnResult(const std::string nodeId, std::vector<std::string> permissions, std::vector<int32_t> grantResults)
    {
        // Default only one permission, so only one grantResult
        if (grantResults[0] == Constant::PERMISSION_GRANTED) {
            result = 0;
        }
        PERMISSION_LOG_INFO(LABEL, "DistributedRequestPermissionFromRemoteTest  TestCallback :OnResult  is run!!!");
    }
    void OnCancel(const std::string nodeId, std::vector<std::string> permissions)
    {
        PERMISSION_LOG_INFO(LABEL, "DistributedRequestPermissionFromRemoteTest  TestCallback :OnCancel  is run!!!");
    }
    void OnTimeOut(const std::string nodeId, std::vector<std::string> permissions)
    {
        PERMISSION_LOG_INFO(LABEL,
            "DistributedRequestPermissionFromRemoteTest  TestCallback :OnTimeOut  is run!!! nodeId = %{public}s",
            nodeId.c_str());
    }
};
string DistributedRequestPermissionFromRemoteTest::GetAppIdInfo(int32_t pid, int32_t uid)
{
    return DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid, uid);
}
std::vector<std::string> DistributedRequestPermissionFromRemoteTest::eventList_ = {
    "resp_com_ohos_dpmsst_service_distributed_system_signed", "resp_com_ohos_dpmsst_service_distributed_third_a"};
STtools::Event DistributedRequestPermissionFromRemoteTest::event_ = STtools::Event();
sptr<OHOS::AppExecFwk::IAppMgr> DistributedRequestPermissionFromRemoteTest::appMs_ = nullptr;
sptr<OHOS::AAFwk::IAbilityManager> DistributedRequestPermissionFromRemoteTest::abilityMs_ = nullptr;
std::shared_ptr<DistributedRequestPermissionFromRemoteTest::AppEventSubscriber>
    DistributedRequestPermissionFromRemoteTest::subscriber_ = nullptr;
void DistributedRequestPermissionFromRemoteTest::SetUpTestCase(void)
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
    std::vector<DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    deviceId_ = deviceList[0].deviceId;
}
bool DistributedRequestPermissionFromRemoteTest::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    for (const auto &e : DistributedRequestPermissionFromRemoteTest::eventList_) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void DistributedRequestPermissionFromRemoteTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    GTEST_LOG_(INFO) << "OnReceiveEvent: ret=" << data.GetWant().GetIntParam("ret", hapRet);
    GTEST_LOG_(INFO) << "OnReceiveEvent: hapRet=" << hapRet;
    auto eventName = data.GetWant().GetAction();
    auto iter = std::find(DistributedRequestPermissionFromRemoteTest::eventList_.begin(),
        DistributedRequestPermissionFromRemoteTest::eventList_.end(),
        eventName);
    if (iter != DistributedRequestPermissionFromRemoteTest::eventList_.end()) {
        STAbilityUtil::Completed(event_, data.GetData(), data.GetCode());
    }
}
/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0100
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the nodeId is null, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "";
    int32_t pid = 1;
    int32_t uid = 100001;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0100 end";
}
/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0200
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the nodeId length > 64, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = long_string_65;
    int32_t pid = 1;
    int32_t uid = 100001;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);

    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0200 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0300
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is null, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0300 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = "";
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0300 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0400
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName length > 256, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0400 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = long_string_257;
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0400 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0500
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is non-sensitive permission, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0500 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = "notSensitiveResource";
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0500 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0600
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the deviceId is null, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "nodeid";
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = "";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0600 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0700
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the deviceId length > 64, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0700 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = long_string_65;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0700 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0800
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the device not online, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0800 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0800 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_0900
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the uid is root id, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0900 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 0;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_GRANTED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0900 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1000
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the uid is system id, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1000 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 1000;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_GRANTED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1000 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1100
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the uid is system signature id, verify result is 0. Has permission
 * return: 0.
 */

HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1100 start";
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    string permission = Constant::CAMERA;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_SYSTEM_SIGNED_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_SYSTEM_SIGNED, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_SYSTEM_SIGNED, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1100 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1200
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is Sensitive permissions other than
 * CAMERA、READ_HEALTH_DATA、LOCATION、MICROPHONE, verify result is -1. No permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_CALENDAR;
    string nodeId = deviceId_;
    int32_t pid = 1;
    int32_t uid = 10002;
    string appInfo = GetAppIdInfo(pid, uid);
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1200 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1300
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is CAMERA and was authorized remote device, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1300 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1300 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1400
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA and was authorized remote device, verify result is 0. Has
 * permission return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1400 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1400 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1500
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is LOCATION and was authorized remote device, verify result is 0. Has
 * permission return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1500 start ";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1500 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1600
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is MICROPHONE and was authorized remote device, verify result is 0. Has
 * permission return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1600 start";
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1600 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1700
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is CAMERA and was not authorized remote device, verify result is -1. No
 * permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1700 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;

    int32_t expectation = Constant::PERMISSION_DENIED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1700 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1800
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA and was not authorized remote device, verify result is -1.
 * No permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1800 start";
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;

    int32_t expectation = Constant::PERMISSION_DENIED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1800 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_1900
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is LOCATION and was not authorized remote device, verify result is -1. No
 * permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_1900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1900 start";
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    string deviceId = deviceId_;

    int32_t expectation = Constant::PERMISSION_DENIED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_1900 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_2000
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : If the permissionName is MICROPHONE and was not authorized remote device, verify result is -1. No
 * permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_2000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_2000 start";
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;

    int32_t expectation = Constant::PERMISSION_DENIED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_2000 end";
}

/**
 * @tc.number    : DPMS_VerifyPermissionFromRemote_2100
 * @tc.name      : VerifyPermissionFromRemote
 * @tc.desc      : Function performance test. Execution time < 1.5ms
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifyPermissionFromRemote_2100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_2100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    string nodeId = deviceId_;
    string permission = Constant::MICROPHONE;
    int32_t pid = 1;
    int32_t uid = 100001;
    string appInfo = GetAppIdInfo(pid, uid);

    auto startTime = std::chrono::high_resolution_clock::now();
    service->VerifyPermissionFromRemote(permission, nodeId, appInfo);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> execution_time = endTime - startTime;
    EXPECT_LT(execution_time.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_2100 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0100
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the nodeId is null, verify result is -3. No permission
 * return: -3.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "";
    int32_t expectation = Constant::PERMISSION_DENIED;
    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);

    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0100 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0200
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the nodeId length > 64, verify result is -3. No permission
 * return: -3.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = long_string_65;
    int32_t expectation = Constant::PERMISSION_DENIED;
    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);

    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0200 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0300
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is null, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifyPermissionFromRemote_0300 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = "";
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);

    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0300 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0400
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName length > 256, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0400 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = long_string_257;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);

    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0400 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0500
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName  is non-sensitive permission, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0500 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = "notSensitiveResource";
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);

    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0500 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0600
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the deviceId is null, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "networkId";
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = "";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0600 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0700
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the deviceId length > 64, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0700 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "networkId";
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = long_string_65;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0700 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0800
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the device not online, verify result is -1. No permission
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0800 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0800 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_0900
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the uid is root id, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0900 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_0900 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1000
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the uid is system id, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1000 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;

    int32_t result = service->VerifySelfPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1000 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1100
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the uid is system signature id, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string deviceId = deviceId_;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_SYSTEM_SIGNED_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_SYSTEM_SIGNED, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_SYSTEM_SIGNED, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1100 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1200
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is Sensitive permissions other than
 * CAMERA、READ_HEALTH_DATA、LOCATION、MICROPHONE, verify result is -1. No permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_CALENDAR;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = deviceId_;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1200 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1300
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is CAMERA and was authorized remote device, verify result is 0. Has permission
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1300 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1300 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1400
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA and was authorized remote device, verify result is 0. Has
 * permission return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1400 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    string reasonResId = "1";
    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1400 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1500
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is LOCATION and was authorized remote device, verify result is 0. Has
 * permission return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1500 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1500 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1600
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is MICROPHONE and was authorized remote device, verify result is 0. Has
 * permission return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1600 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1700
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is CAMERA and was not authorized remote device, verify result is -1. No
 * permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1700 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = deviceId_;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1700 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1800
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA and was not authorized remote device, verify result is -1.
 * No permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1800 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = deviceId_;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1800 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_1900
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is LOCATION and was not authorized remote device, verify result is -1. No
 * permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_1900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1900 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = deviceId_;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_1900 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_2000
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : If the permissionName is MICROPHONE and was not authorized remote device, verify result is -1. No
 * permission return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_2000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_2000 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;
    string deviceId = deviceId_;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "VerifySelfPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_2000 end";
}

/**
 * @tc.number    : DPMS_VerifySelfPermissionFromRemote_2100
 * @tc.name      : VerifySelfPermissionFromRemote
 * @tc.desc      : Function performance test. Execution time < 1.5ms
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_VerifySelfPermissionFromRemote_2100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_2100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;

    auto startTime = std::chrono::high_resolution_clock::now();
    service->VerifySelfPermissionFromRemote(permission, nodeId);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> execution_time = endTime - startTime;
    EXPECT_LT(execution_time.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_VerifySelfPermissionFromRemote_2100 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_0100
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the nodeId is null, result false. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "";
    bool expectation = false;

    bool result = service->CanRequestPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0100 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_0200
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the nodeId length > 64, result false. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = long_string_65;
    bool expectation = false;

    bool result = service->CanRequestPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0200 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_0300
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is null, result false. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0300 start";
    string permission = "";
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0300 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_0400
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName length > 256, result false. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0400 start";
    string permission = long_string_257;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0400 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_0500
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the device is not online, result false. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0500 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "networkId";
    bool expectation = false;

    bool result = service->CanRequestPermissionFromRemote(permission, nodeId);
    EXPECT_EQ(expectation, result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0500 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_0600
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the uid is system signature id, result true. Has permission
 * return: true.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0600 start";
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    bool expectation = 0;
    string deviceId = deviceId_;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_SYSTEM_SIGNED_ABILITY;

    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;

    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_SYSTEM_SIGNED, 123456, "CanRequestPermissionFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_SYSTEM_SIGNED, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_0600 end";
}
/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1100
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is Sensitive permissions other than
 * CAMERA、READ_HEALTH_DATA、LOCATION、MICROPHONE, result false. No permission return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    string permission = Constant::READ_CALENDAR;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1100 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1200
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is CAMERA and was authorized remote device,  result false. Has permission
 * return: true.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1200 start";
    string permission = Constant::CAMERA;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    int32_t expectationTrue = 0;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectationTrue, 3), 0);
    EXPECT_EQ(ERR_OK, eCode);
    // wait RequestPermissionsFromRemote
    auto timeFast = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(timeFast);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1200 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1300
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA and was authorized remote device, result false. Has
 * permission return: true.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1300 start";
    string permission = Constant::READ_HEALTH_DATA;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    int32_t expectationTrue = 0;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectationTrue, 3), 0);
    EXPECT_EQ(ERR_OK, eCode);
    // wait RequestPermissionsFromRemote
    auto timeFast = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(timeFast);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1300 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1400
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is LOCATION and was authorized remote device, result false. Has permission
 * return: true.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1400 start";
    string permission = Constant::LOCATION;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    int32_t expectationTrue = 0;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectationTrue, 3), 0);
    EXPECT_EQ(ERR_OK, eCode);
    // wait RequestPermissionsFromRemote
    auto timeFast = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(timeFast);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1400 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1500
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is MICROPHONE and was authorized remote device, result false. Has permission
 * return: true.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1500 start";
    string permission = Constant::MICROPHONE;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    int32_t expectationTrue = 0;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectationTrue, 3), 0);
    EXPECT_EQ(ERR_OK, eCode);
    // wait RequestPermissionsFromRemote
    auto timeFast = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(timeFast);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1500 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1600
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is CAMERA and was not authorized remote device, result true. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    string permission = Constant::CAMERA;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 1;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1600 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1700
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA and was not authorized remote device, result true. No
 * permission return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1700 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    string permission = Constant::READ_HEALTH_DATA;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 1;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1700 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1800
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is LOCATION and was not authorized remote device, result true. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1800 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    string permission = Constant::LOCATION;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 1;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1800 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_1900
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : If the permissionName is MICROPHONE and was not authorized remote device, result true. No permission
 * return: false.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_1900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1900 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    string permission = Constant::MICROPHONE;

    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 1;
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);
    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, 3), 0);
    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_1900 end";
}

/**
 * @tc.number    : DPMS_CanRequestPermissionFromRemote_2000
 * @tc.name      : CanRequestPermissionFromRemote
 * @tc.desc      : Function performance test. Execution time < 1.5ms
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_CanRequestPermissionFromRemote_2000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_2000 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "networkId";
    string deviceId = "deviceId";
    int32_t uid = IPCSkeleton::GetCallingUid();
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    std::set<std::string> permissionSet;
    permissionSet.insert(permission);
    ObjectDevicePermissionRepository::GetInstance().GetOrCreateObjectDevice(deviceId)->AddObjectUid(uid);
    ObjectDevicePermissionRepository::GetInstance().GetOrCreateObjectDevice(deviceId)->ResetGrantSensitivePermission(
        uid, permissionSet);

    auto startTime = std::chrono::high_resolution_clock::now();
    service->CanRequestPermissionFromRemote(permission, nodeId);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> execution_time = endTime - startTime;
    EXPECT_LT(execution_time.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_CanRequestPermissionFromRemote_2000 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0100
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the permissionName is null, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();

    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = deviceId_;
    string bundleName = "testBundle";
    int32_t reasonResId = 1;
    int32_t expectation = -1;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0100 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0200
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the callback function is null, no result. Request failure
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    OHOS::sptr<TestCallback> callback = nullptr;
    string nodeId = deviceId_;
    string bundleName = "testBundle";
    int32_t reasonResId = 1;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, " Callback function is null. No result! ");
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0200 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0300
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the nodeId is null, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0300 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::CAMERA);
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = "";
    string bundleName = "testBundle";
    int32_t reasonResId = 1;
    int32_t expectation = -1;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0300 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0400
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the nodeId length > 64, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0400 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::CAMERA);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = long_string_65;
    string bundleName = "testBundle";
    int32_t reasonResId = 1;
    int32_t expectation = -1;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0400 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0500
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the permissionName is non-sensitive permission, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0500 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back("notSensitiveResource");
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = deviceId_;
    string bundleName = "testBundle";
    int32_t reasonResId = 1;
    int32_t expectation = -1;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0500 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0600
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the deviceId is null, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::CAMERA);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = deviceId_;
    string bundleName = "testBundle";
    string deviceId = "";
    int32_t reasonResId = 1;
    int32_t expectation = -1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0600 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0700
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the deviceId length > 64, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0700 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::CAMERA);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = deviceId_;
    string bundleName = "testBundle";
    string deviceId = long_string_65;
    int32_t reasonResId = 1;
    int32_t expectation = -1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0700 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0800
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the device is not online, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0800 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::CAMERA);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = "networkId";
    string bundleName = "testBundle";

    int32_t reasonResId = 1;
    int32_t expectation = -1;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0800 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_0900
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the uid is root, result 0.
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0900 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::CAMERA);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = deviceId_;
    string bundleName = "testBundle";
    int32_t reasonResId = 1;
    int32_t expectation = 0;

    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);

    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->result);
    EXPECT_EQ(expectation, callback->result);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_0900 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1100
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the uid system signature, result 0.
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1100 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    string reasonResId = "1";
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_SYSTEM_SIGNED_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;

    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_SYSTEM_SIGNED, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_SYSTEM_SIGNED, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1100 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1200
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the bundleName is null, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1200 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = "";
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;

    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1200 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1300
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the reasonResId < 0, result -1. Request failure
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1300 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "-1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1300 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1500
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If request time out, result -1.
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1500 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1500 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1700
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the permissionName is Sensitive permissions other than
 * CAMERA、READ_HEALTH_DATA、LOCATION、MICROPHONE, result -1. Request failure return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1700 start";
    string permission = Constant::READ_CALENDAR;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1700 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1800
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is CAMERA and remote device user agree with authorization. result 0
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1800 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1800 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_1900
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is READ_HEALTH_DATA and remote device user agree with authorization.
 * result 0 return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_1900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1900 start";
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1900 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2000
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is LOCATION and remote device user agree with authorization. result 0
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2000 start";
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2000 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2100
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is MICROPHONE and remote device user agree with authorization. result 0
 * return: 0.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1900 start";
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = 0;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_1900 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2200
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is CAMERA and remote device user disagree with authorization. result -1
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2200 start";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2200 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2300
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is READ_HEALTH_DATA and remote device user disagree with authorization.
 * result -1 return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2300 start";
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2300 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2400
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is LOCATION and remote device user disagree with authorization. result
 * -1 return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2400 start";
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;

    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2400 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2500
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : If the request permissionName is MICROPHONE and remote device user disagree with authorization.
 * result -1 return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2500 start";
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = -1;
    string reasonResId = "1";

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;

    MAP_STR_STR params;
    params["bundleName"] = bundleName;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["reasonResId"] = reasonResId;

    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "RequestPermissionsFromRemote");

    EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 123456, "StopSelfAbility");
    EXPECT_TRUE(ret);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2500 end";
}

/**
 * @tc.number    : DPMS_RequestPermissionsFromRemote_2600
 * @tc.name      : RequestPermissionsFromRemote
 * @tc.desc      : when request app is uninstall, result -1.
 * return: -1.
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_RequestPermissionsFromRemote_2600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::vector<std::string> permissions;
    permissions.push_back(Constant::MICROPHONE);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    string nodeId = "networkId";
    string bundleName = "testBundle";
    string deviceId = "deviceId";
    int32_t reasonResId = 1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");

    auto startTime = std::chrono::high_resolution_clock::now();
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> execution_time = endTime - startTime;
    EXPECT_LT(execution_time.count(), STDistibutePermissionUtil::MAX_ELAPSED);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_RequestPermissionsFromRemote_2600 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0100
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the nodeId is null, end of processing, do not authorization
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0100 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = "";
    int32_t expectation = Constant::PERMISSION_DENIED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(nodeId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0100 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0200
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the nodeId length > 65, end of processing, do not authorization
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = long_string_65;
    int32_t expectation = Constant::PERMISSION_DENIED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(nodeId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0200 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0500
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the permissionName is Sensitive permissions other than
 * CAMERA、READ_HEALTH_DATA、LOCATION、MICROPHONE, end of processing, do not authorization return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0500 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_CALENDAR;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_DENIED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0500 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0600
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the permissionName is CAMERA , can be able to delegate authorization
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0600 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0600 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0700
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the permissionName is READ_HEALTH_DATA , can be able to delegate authorization
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0700 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::READ_HEALTH_DATA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0700 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0800
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the permissionName is LOCATION , can be able to delegate authorization
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0800 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::LOCATION;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0800 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_0900
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : If the permissionName is MICROPHONE , can be able to delegate authorization
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0900 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    int32_t RESULT = DistributedPermissionKit::CheckDPermission(duid, permission);
    EXPECT_EQ(RESULT, expectation);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_0900 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_1000
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : Interface combination test.
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_1000, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_1000 start";

    string reasonResId = "1";
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["bundleName"] = bundleName;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifyPermissionFromRemote");

    int32_t result_verify =
        STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", Constant::PERMISSION_DENIED, 3);

    EXPECT_EQ(result_verify, 0);

    if (result_verify == 0) {
        STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");
        EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);
    }

    bool ret = STAbilityUtil::StopAbility(REQ_EVENT_NAME_THIRD_A, 0, "StopSelfAbility");
    EXPECT_TRUE(ret);

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_1000 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_1100
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : Interface combination test.
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_1100, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_1100 start";
    string reasonResId = "1";
    string permission = Constant::MICROPHONE;
    string nodeId = deviceId_;
    string deviceId = deviceId_;
    int32_t expectation = Constant::PERMISSION_GRANTED;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    params["bundleName"] = bundleName;
    params["reasonResId"] = reasonResId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    ErrCode eCode = STAbilityUtil::StartAbility(want, abilityMs_, 1000);
    EXPECT_EQ(ERR_OK, eCode);

    EXPECT_EQ(
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3), 0);

    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 5, "VerifySelfPermissionFromRemote");
    int32_t result_verify =
        STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", Constant::PERMISSION_DENIED, 3);
    EXPECT_EQ(result_verify, 0);

    if (0 == result_verify) {
        int32_t canRequest = 1;
        STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 123456, "CanRequestPermissionFromRemote");
        int32_t result_request = STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", canRequest, 3);
        EXPECT_EQ(result_request, 0);

        if (result_request == 0) {
            STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, 6, "RequestPermissionsFromRemote");
            EXPECT_EQ(STAbilityUtil::WaitCompleted(event_, "RequestPermissionsFromRemote", expectation, 3), 0);
        } else {
            PERMISSION_LOG_INFO(LABEL,
                "DPMS_GrantSensitivePermissionToRemoteApp_1000 The application cannot request permission on the remote "
                "device");
        }
    } else {
        PERMISSION_LOG_INFO(LABEL, "DPMS_GrantSensitivePermissionToRemoteApp_1000 Permission is already authorized");
    }

    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_1100 end";
}

/**
 * @tc.number    : DPMS_GrantSensitivePermissionToRemoteApp_1200
 * @tc.name      : GrantSensitivePermissionToRemoteApp
 * @tc.desc      : Function performance test. Execution time < 1.5ms
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_GrantSensitivePermissionToRemoteApp_1200, TestSize.Level1)
{
    GTEST_LOG_(INFO)
        << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_1200 start";
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    string permission = Constant::CAMERA;
    string nodeId = deviceId_;
    int ruid = 102345;

    auto startTime = std::chrono::high_resolution_clock::now();
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, ruid);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> execution_time = endTime - startTime;
    EXPECT_LT(execution_time.count(), STDistibutePermissionUtil::MAX_ELAPSED);
    GTEST_LOG_(INFO) << "DistributedRequestPermissionFromRemoteTest DPMS_GrantSensitivePermissionToRemoteApp_1200 end";
}
/**
 * @tc.number    : DPMS_AllocateDuid_0100
 * @tc.name      :
 * @tc.desc      : Test Tool
 * return: .
 */
HWTEST_F(DistributedRequestPermissionFromRemoteTest, DPMS_AllocateDuid_0100, TestSize.Level1)
{
    int32_t rUidThird_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS