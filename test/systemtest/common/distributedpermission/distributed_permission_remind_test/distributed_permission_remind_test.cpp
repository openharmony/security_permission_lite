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
#include <cstdio>
#include <fstream>
#include <sstream>
#include <chrono>
#include <gtest/gtest.h>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "ipc_skeleton.h"
#include "time_util.h"
#include "distributed_permission_kit.h"
#include "sqlite_storage.h"
#include "bundle_info.h"
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

#define CMD_RESULT_BUF_SIZE 1024
#define DEVICE_UUID_LENGTH 65
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::EventFwk;
using namespace OHOS::MTUtil;
using namespace OHOS::STtools;
using namespace OHOS::STABUtil;

using std::string;

namespace {

const double RunTime_DeviceInfoManager = 1;
const double RunTime_BundleManager = 1;
const double RunTime_Permissionmanager = 1;
const double RunTime_AbilityManager = 1;
const double RunTime_AnsManager = 1;
const double RunTime_IPCSkeleton = 5;
double RunTime_All = 0;

std::recursive_mutex statckLock_;
string deviceId_;
int32_t pid_ = 0;
int32_t uid_ = 0;
int32_t hapRet = 0;
string permName_ = "DistributedPermissionName";
OHOS::AppExecFwk::BundleInfo bundleInfo_;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_PERMISSION, "DistributedPermissionRemindTest"};

static const std::string SYSTEM_TEST_HAP_NAME_DPMS = "dpmsSystemBundleOnlySystemGrant";                             //系统包 不带授限权限
static const std::string SYSTEM_TEST_HAP_NAME_DPMS_S = "dpmsSystemBundle";  // 系统包，带有受限权限
static const std::string THIRD_TEST_HAP_NAME = "dpmsThirdBundle";  //第三方包 ，权限受限、敏感、非敏感权限都有

static const std::string SYSTEM_TEST_BUNDLE_NAME_DPMS = "com.system.hiworld.dpms_system_grant";
static const std::string SYSTEM_TEST_BUNDLE_NAME_DPMS_S = "com.system.hiworld.dpms_system_bundle";
static const std::string THIRD_TEST_BUNDLE_NAME = "com.third.hiworld.dpmsExample";

static const std::string SYSTEM_TEST_BUNDLE_NAME_SETTINGS = "com.ohos.settings";
static const std::string SYSTEM_TEST_BUNDLE_NAME_LAUNCHER = "com.ohos.launcher";

static const std::string PERMISSION_CAMERA = "ohos.permission.CAMERA";
static const std::string PERMISSION_LOCATION = "ohos.permission.LOCATION";
static const std::string PERMISSION_MICROPHONE = "ohos.permission.MICROPHONE";
static const std::string PERMISSION_ACTIVITY_MOTION = "ohos.permission.ACTIVITY_MOTION";
static const std::string PERMISSION_DISTRIBUTED_DATASYNC = "ohos.permission.DISTRIBUTED_DATASYNC";
static const std::string PERMISSION_TEST = "ohos.permission.TEST";

}  // namespace


class DistributedPermissionRemindTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static void RemoveStorage();
    void GetIPCSkeleton();
    string GetAppIdInfo(string &deviceId_);
    void FindStorageByDeviceId(string &deviceId_, std::vector<GenericValues> &results);
    void FindDataStorageAll(DataStorage::DataType dataType, std::vector<GenericValues> result);
    int32_t CheckDpsServiceAndStart();
    void DeleteAllRemind();
    int32_t ExecuteCMD(const char *cmd, char *result);
    void InstallBundel();
    void UnInstallBundel();
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

std::vector<std::string> DistributedPermissionRemindTest::eventList_ = {
    "resp_com_ohos_dpmsst_service_app_a1",
    "req_com_ohos_dpmsst_service_app_a1"
};
STtools::Event DistributedPermissionRemindTest::event_ = STtools::Event();
sptr<OHOS::AppExecFwk::IAppMgr> DistributedPermissionRemindTest::appMs_ = nullptr;
sptr<OHOS::AAFwk::IAbilityManager> DistributedPermissionRemindTest::abilityMs_ = nullptr;
std::shared_ptr<DistributedPermissionRemindTest::AppEventSubscriber> DistributedPermissionRemindTest::subscriber_ = nullptr;

bool DistributedPermissionRemindTest::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    for (const auto &e : DistributedPermissionRemindTest::eventList_) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void DistributedPermissionRemindTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    GTEST_LOG_(INFO) << "OnReceiveEvent: event=" << data.GetWant().GetAction();
    GTEST_LOG_(INFO) << "OnReceiveEvent: data=" << data.GetData();
    GTEST_LOG_(INFO) << "OnReceiveEvent: code=" << data.GetCode();
    GTEST_LOG_(INFO) << "OnReceiveEvent: ret=" << data.GetWant().GetIntParam("ret", hapRet);
    GTEST_LOG_(INFO) << "OnReceiveEvent: hapRet=" << hapRet;

    auto eventName = data.GetWant().GetAction();
    auto iter = std::find(DistributedPermissionRemindTest::eventList_.begin(), DistributedPermissionRemindTest::eventList_.end(), eventName);
    if (iter != DistributedPermissionRemindTest::eventList_.end()) {
        STAbilityUtil::Completed(event_, data.GetData(), data.GetCode());
    }
}

class TestCallback : public IRemoteStub<OnUsingPermissionReminder> {
public:

    TestCallback() = default;
    virtual ~TestCallback() = default;

    std::string permName = "";
    void StartUsingPermission(const PermissionReminderInfo& permReminderInfo)
    {
        EXPECT_EQ(permName, OHOS::Str16ToStr8(permReminderInfo.permName));
    }

    void StopUsingPermission(const PermissionReminderInfo& permReminderInfo)
    {
        EXPECT_EQ(permName, OHOS::Str16ToStr8(permReminderInfo.permName));
    }
};

void DistributedPermissionRemindTest::SetUpTestCase(void)
{
    RemoveStorage();

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

void DistributedPermissionRemindTest::TearDownTestCase(void)
{}

void DistributedPermissionRemindTest::SetUp(void)
{
    InstallBundel();

    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    OHOS::Security::Permission::PermissionDef permissionDef_Camera = {
        .permissionName = PERMISSION_CAMERA,
        .bundleName = THIRD_TEST_BUNDLE_NAME,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528
    };
    OHOS::Security::Permission::PermissionDef permissionDef_Location = {
        .permissionName = PERMISSION_LOCATION,
        .bundleName = THIRD_TEST_BUNDLE_NAME,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528
    };
    OHOS::Security::Permission::PermissionDef permissionDef_Microphone = {
        .permissionName = PERMISSION_MICROPHONE,
        .bundleName = THIRD_TEST_BUNDLE_NAME,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528
    };
    OHOS::Security::Permission::PermissionDef permissionDef_Activity = {
        .permissionName = PERMISSION_ACTIVITY_MOTION,
        .bundleName = SYSTEM_TEST_BUNDLE_NAME_DPMS_S,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528
    };

    permDefList.emplace_back(permissionDef_Camera);
    permDefList.emplace_back(permissionDef_Location);
    permDefList.emplace_back(permissionDef_Microphone);
    permDefList.emplace_back(permissionDef_Activity);
    PermissionKit::AddDefPermissions(permDefList);


    std::vector<std::string> permList_user;
    permList_user.push_back(PERMISSION_CAMERA);
    permList_user.push_back(PERMISSION_LOCATION);
    permList_user.push_back(PERMISSION_MICROPHONE);
    
    PermissionKit::AddUserGrantedReqPermissions(THIRD_TEST_BUNDLE_NAME, permList_user, 0);    
    PermissionKit::GrantUserGrantedPermission(THIRD_TEST_BUNDLE_NAME, PERMISSION_CAMERA, 0);
    PermissionKit::GrantUserGrantedPermission(THIRD_TEST_BUNDLE_NAME, PERMISSION_LOCATION, 0);
    PermissionKit::GrantUserGrantedPermission(THIRD_TEST_BUNDLE_NAME, PERMISSION_MICROPHONE, 0);

    std::vector<std::string> permList_sytem;
    permList_sytem.push_back(PERMISSION_ACTIVITY_MOTION);
    PermissionKit::AddSystemGrantedReqPermissions(SYSTEM_TEST_BUNDLE_NAME_DPMS_S, permList_sytem);    
    PermissionKit::GrantSystemGrantedPermission(SYSTEM_TEST_BUNDLE_NAME_DPMS_S, PERMISSION_ACTIVITY_MOTION);
}

void DistributedPermissionRemindTest::TearDown(void)
{
    GTEST_LOG_(INFO) << "void DistributedPermissionRemindTest::TearDown(void)";
    PermissionKit::RemoveDefPermissions(SYSTEM_TEST_BUNDLE_NAME_DPMS_S);
    PermissionKit::RemoveDefPermissions(THIRD_TEST_BUNDLE_NAME);
    PermissionKit::RemoveDefPermissions("com.ohos.dpmsst.service.appA");
    
    UnInstallBundel();
}

void DistributedPermissionRemindTest::InstallBundel()
{
    GetIPCSkeleton();

    std::vector<std::string> hapNameList = {
        "com.ohos.dpmsst.service.appA",
    };

    //安装测试所需hap包
    {
        STDistibutePermissionUtil::Install(THIRD_TEST_HAP_NAME);
        STDistibutePermissionUtil::Install("dpmsDataSystemTestA");
        
        //STAbilityUtil::InstallHaps(hapNameList);
        abilityMs_ = STAbilityUtil::GetAbilityManagerService();
        appMs_ = STAbilityUtil::GetAppMgrService();
        usleep(5000);
    }
}

void DistributedPermissionRemindTest::UnInstallBundel()
{
    {
        STDistibutePermissionUtil::Uninstall(THIRD_TEST_HAP_NAME);
        STDistibutePermissionUtil::Uninstall("com.ohos.amsst.appA");
    }

    STAbilityUtil::RemoveStack(1, abilityMs_, 3000, 5000);
    std::vector<std::string> vecBundleName;
    vecBundleName.push_back("com.ohos.amsst.appA");
    
    STAbilityUtil::KillBundleProcess(vecBundleName);

    STAbilityUtil::CleanMsg(event_);
}

void DistributedPermissionRemindTest::FindStorageByDeviceId(string &deviceId_, std::vector<GenericValues> &results)
{
    GenericValues andConditions;
    andConditions.Put(FIELD_DEVICE_ID, deviceId_);
    GenericValues orConditions;
    SqliteStorage::GetRealDataStorage().FindByConditions(
        DataStorage::DataType::PERMISSION_VISITOR, andConditions, orConditions, results);
}

void DistributedPermissionRemindTest::RemoveStorage()
{
    std::vector<GenericValues> visitorResults;
    DataStorage::DataType dataTypes[] = {
        DataStorage::DataType::PERMISSION_VISITOR, DataStorage::DataType::PERMISSION_RECORD};
    for (auto dataType : dataTypes) {
        SqliteStorage::GetRealDataStorage().Find(dataType, visitorResults);
        if (visitorResults.size() == 0) {
            continue;
        }
        GenericValues conditions;
        for (auto visitorResult : visitorResults) {
            conditions.Put(FIELD_ID, visitorResult.Get(FIELD_ID));
        }
        bool rmResult = SqliteStorage::GetRealDataStorage().Remove(dataType, conditions);
        PERMISSION_LOG_DEBUG(LABEL, "init rmResult = %{public}d", rmResult);
    }
}

void DistributedPermissionRemindTest::GetIPCSkeleton()
{
    //pid_ = IPCSkeleton::GetCallingPid();
    pid_ = 0;
    uid_ = IPCSkeleton::GetCallingUid();
    deviceId_ = IPCSkeleton::GetLocalDeviceID();
}

string DistributedPermissionRemindTest::GetAppIdInfo(string &deviceId_)
{
    return DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid_, uid_, deviceId_);
}

//获取权限数据库数据
void DistributedPermissionRemindTest::FindDataStorageAll(
    DataStorage::DataType dataType, std::vector<GenericValues> result)
{
    SqliteStorage::GetRealDataStorage().Find(dataType, result);
}

/*
 * cmd：待执行命令
 * result：命令输出结果
 * 函数返回：0 成功；-1 失败；
 */
int DistributedPermissionRemindTest::ExecuteCMD(const char *cmd, char *result)
{
    int iRet = -1;
    char buf_ps[CMD_RESULT_BUF_SIZE];
    char ps[CMD_RESULT_BUF_SIZE] = {0};
    FILE *ptr;

    strcpy(ps, cmd);

    if((ptr = popen(ps, "r")) != NULL)
    {
        while(fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
        {
           strcat(result, buf_ps);
           if(strlen(result) > CMD_RESULT_BUF_SIZE)
           {
               break;
           }
        }
        pclose(ptr);
        ptr = NULL;
        iRet = 0;  // 处理成功
    }
    else
    {
        printf("popen %s error\n", ps);
        iRet = -1; // 处理失败
    }

    return iRet;
}

int32_t DistributedPermissionRemindTest::CheckDpsServiceAndStart()
{
    char result[CMD_RESULT_BUF_SIZE]={0};
    ExecuteCMD("ps -ef | grep -w dps_service | grep -v grep | wc -l", result);
    string sCmdResult;
    sCmdResult = string(result);
    GTEST_LOG_(INFO) << "result : " << sCmdResult.c_str();
    // if(strncmp(result, "0", 1) == 0)
    // {
    //     ExecuteCMD("cd //", result);
    //     GTEST_LOG_(INFO) << "result : " << sCmdResult.c_str();
    //     ExecuteCMD("/system/bin/sa_main /system/profile/dps_service.xml &", result);
    //     GTEST_LOG_(INFO) << "result : " << sCmdResult.c_str();
    // }

    if(strncmp(result, "0", 1) == 0)
    {
        return -1;
    }

    return 0;
}

void DistributedPermissionRemindTest::DeleteAllRemind()
{

const std::string SYSTEM_TEST_BUNDLE_NAME_DPMS = "com.system.hiworld.dpms_system_grant";
const std::string SYSTEM_TEST_BUNDLE_NAME_DPMS_S = "com.system.hiworld.dpms_system_bundle";
const std::string THIRD_TEST_BUNDLE_NAME = "com.third.hiworld.dpmsExample";

const std::string SYSTEM_TEST_BUNDLE_NAME_SETTINGS = "com.ohos.settings";
const std::string SYSTEM_TEST_BUNDLE_NAME_LAUNCHER = "com.ohos.launcher";

    string permName = "";
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    deviceId_ = deviceId;

    // SYSTEM_TEST_BUNDLE_NAME_DPMS
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DPMS);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    // SYSTEM_TEST_BUNDLE_NAME_DPMS_S
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DPMS_S);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    // THIRD_TEST_BUNDLE_NAME
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    // SYSTEM_TEST_BUNDLE_NAME_SETTINGS
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    // SYSTEM_TEST_BUNDLE_NAME_LAUNCHER
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_LAUNCHER);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
}

/**
 * @tc.number    : DPMS_RegisterUsingPermissionReminder_0100
 * @tc.name      : RegisterUsingPermissionReminder
 * @tc.desc      : The DPMS does not start
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_RegisterUsingPermissionReminder_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_RegisterUsingPermissionReminder_0100 start";    
    OHOS::sptr<TestCallback> callback(new TestCallback());
    callback->permName = "DPMS_RegisterUsingPermissionReminder_0100";
    int32_t ret = -1;
    
    ret = DistributedPermissionKit::RegisterUsingPermissionReminder(callback);
    EXPECT_EQ(CheckDpsServiceAndStart(), ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_RegisterUsingPermissionReminder_0100 end";

    DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
}

/**
 * @tc.number    : DPMS_RegisterUsingPermissionReminder_0200
 * @tc.name      : RegisterUsingPermissionReminder
 * @tc.desc      : The callback is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_RegisterUsingPermissionReminder_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_RegisterUsingPermissionReminder_0200 start";

    OHOS::sptr<TestCallback> callback = nullptr;
    int32_t ret = -1;
    
    ret = DistributedPermissionKit::RegisterUsingPermissionReminder(callback);
    EXPECT_EQ(-1, ret);
    
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_RegisterUsingPermissionReminder_0200 end";
}

/**
 * @tc.number    : DPMS_RegisterUsingPermissionReminder_0300
 * @tc.name      : RegisterUsingPermissionReminder
 * @tc.desc      : Performance test
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_RegisterUsingPermissionReminder_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_RegisterUsingPermissionReminder_0300 start";
    
    OHOS::sptr<TestCallback> callback(new TestCallback());
    callback->permName = "DPMS_RegisterUsingPermissionReminder_0300";
    int32_t ret = -1;

    auto startTime = std::chrono::high_resolution_clock::now();
    ret = DistributedPermissionKit::RegisterUsingPermissionReminder(callback);
    auto endTime = std::chrono::high_resolution_clock::now();

    EXPECT_EQ(0, ret);

    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    RunTime_All = 0;
    RunTime_All += RunTime_DeviceInfoManager;
    RunTime_All += RunTime_BundleManager;
    RunTime_All += RunTime_Permissionmanager;
    RunTime_All += RunTime_AbilityManager;

    EXPECT_LT(fp_ms.count() - RunTime_All, STDistibutePermissionUtil::MAX_ELAPSED);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_RegisterUsingPermissionReminder_0300 end";

    //ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
}

/**
 * @tc.number    : DPMS_RegisterUsingPermissionReminder_0400
 * @tc.name      : RegisterUsingPermissionReminder
 * @tc.desc      : Stability test
 */

/**
 * @tc.number    : DPMS_RegisterUsingPermissionReminder_0500
 * @tc.name      : RegisterUsingPermissionReminder
 * @tc.desc      : Fuzz test
 */

/**
 * @tc.number    : DPMS_UnregisterUsingPermissionReminder_0100
 * @tc.name      : UnregisterUsingPermissionReminder
 * @tc.desc      : The DPMS does not start
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_UnregisterUsingPermissionReminder_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_UnregisterUsingPermissionReminder_0100 start";
    
    OHOS::sptr<TestCallback> callback(new TestCallback());
    callback->permName = "DPMS_UnregisterUsingPermissionReminder_0100";
    int32_t ret = -1;
    
    ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
    EXPECT_EQ(CheckDpsServiceAndStart(), ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_UnregisterUsingPermissionReminder_0100 end";
}

/**
 * @tc.number    : DPMS_UnregisterUsingPermissionReminder_0200
 * @tc.name      : UnregisterUsingPermissionReminder
 * @tc.desc      : The callback is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_UnregisterUsingPermissionReminder_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_UnregisterUsingPermissionReminder_0200 start";
    
    OHOS::sptr<TestCallback> callback;
    int32_t ret = -1;
    
    ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
    EXPECT_EQ(-1, ret);
    
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_UnregisterUsingPermissionReminder_0200 end";
}

/**
 * @tc.number    : DPMS_UnregisterUsingPermissionReminder_0300
 * @tc.name      : UnregisterUsingPermissionReminder
 * @tc.desc      : Performance test
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_UnregisterUsingPermissionReminder_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_UnregisterUsingPermissionReminder_0300 start";
    int32_t ret = -1;
    
    OHOS::sptr<TestCallback> callback(new TestCallback());
    callback->permName = "DPMS_UnregisterUsingPermissionReminder_0300";

    auto startTime = std::chrono::high_resolution_clock::now();
    ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;

    RunTime_All = 0;
    RunTime_All += RunTime_DeviceInfoManager;
    RunTime_All += RunTime_BundleManager;
    RunTime_All += RunTime_Permissionmanager;
    RunTime_All += RunTime_AbilityManager;

    EXPECT_LT(fp_ms.count() - RunTime_All, STDistibutePermissionUtil::MAX_ELAPSED);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_UnregisterUsingPermissionReminder_0300 end";
}

/**
 * @tc.number    : DPMS_UnregisterUsingPermissionReminder_0400
 * @tc.name      : UnregisterUsingPermissionReminder
 * @tc.desc      : Stability test 
 */

/**
 * @tc.number    : DPMS_UnregisterUsingPermissionReminder_0500
 * @tc.name      : UnregisterUsingPermissionReminder
* @tc.name       : Fuzz test
 */

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0100
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : The permission is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0100 start";
    int32_t ret = -1;

    string appInfo = GetAppIdInfo(deviceId_);
    string permName = "";
    
    ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(-1, ret);
    
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0100 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0200
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : The appInfo is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0200 start";
    int32_t ret = -1;

    string appInfo = "";
    string permName = "DPMS_CheckPermissionAndStartUsing_0200";
    
    ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(-1, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0200 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0300
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : The DPMS does not start
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0300 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    int ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(CheckDpsServiceAndStart(), ret);
    
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0300 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0400
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : Thd uid is root Uid
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0400 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_ACTIVITY_MOTION;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DPMS_S);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    int ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(0, ret);
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0400 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0500
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : Thd uid is system Uid
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0500 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_ACTIVITY_MOTION;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DPMS_S);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    int ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0500 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0600
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : Start the permission reminder and add a permission uesd record
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0600 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    int ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0600 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0700
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : The sensitive permission is not granted
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0700 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_DISTRIBUTED_DATASYNC;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    int ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(-1, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0700 end";
}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0800
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : The perimssion is not sensitive
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0800 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_TEST;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    int ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(-1, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0800 end";

}

/**
 * @tc.number    : DPMS_CheckPermissionAndStartUsing_0900
 * @tc.name      : CheckPermissionAndStartUsing
 * @tc.desc      : Performance test
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckPermissionAndStartUsing_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckPermissionAndStartUsing_0900 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    RunTime_All = 0;
    RunTime_All += RunTime_DeviceInfoManager;
    RunTime_All += RunTime_BundleManager;
    RunTime_All += RunTime_Permissionmanager;
    RunTime_All += RunTime_AbilityManager;
    RunTime_All += RunTime_AnsManager;
    RunTime_All += RunTime_IPCSkeleton;
    EXPECT_LT(fp_ms.count() - RunTime_All, STDistibutePermissionUtil::MAX_ELAPSED);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckPermissionAndStartUsing_0900 end";
}

// /**
//  * @tc.number    : DPMS_CheckPermissionAndStartUsing_1000
//  * @tc.name      : CheckPermissionAndStartUsing
//  * @tc.desc      : Stability test 
//  */

// /**
//  * @tc.number    : DPMS_CheckPermissionAndStartUsing_1100
//  * @tc.name      : CheckPermissionAndStartUsing
//  * @tc.name       : Fuzz test
//  */


/**
 * @tc.number    : DPMS_CheckCallerPermissionAndStartUsing_0100
 * @tc.name      : CheckCallerPermissionAndStartUsing
 * @tc.desc      : The permisson is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckCallerPermissionAndStartUsing_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckCallerPermissionAndStartUsing_0100 start";
    
    DeleteAllRemind();
    // usd the THIRD_TEST_HAP_NAME
    string permName = "";
    int ret = DistributedPermissionKit::CheckCallerPermissionAndStartUsing(permName);
    EXPECT_EQ(-1, ret);
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckCallerPermissionAndStartUsing_0100 end";
}

/**
 * @tc.number    : DPMS_CheckCallerPermissionAndStartUsing_0200
 * @tc.name      : CheckCallerPermissionAndStartUsing
 * @tc.desc      : The DPMS does not start
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckCallerPermissionAndStartUsing_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckCallerPermissionAndStartUsing_0200 start";

    std::string bundleName = "com.ohos.dpmsst.service.appA";
    std::string abilityName = "DpmsStServiceAbilityA1";

    std::vector<OHOS::Security::Permission::PermissionDef> permHap;
    OHOS::Security::Permission::PermissionDef permissionHap_Camera = {
        .permissionName = PERMISSION_CAMERA,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528
    };

    permHap.emplace_back(permissionHap_Camera);
    PermissionKit::AddDefPermissions(permHap);


    std::vector<std::string> permHap_user;
    permHap_user.push_back(PERMISSION_CAMERA);
    
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permHap_user, 0);    
    PermissionKit::GrantUserGrantedPermission(bundleName, PERMISSION_CAMERA, 0);

    MAP_STR_STR params;
    params["permName"] = PERMISSION_CAMERA;
    Want want = STAbilityUtil::MakeWant("device", abilityName, bundleName, params);
    STAbilityUtil::StartAbility(want, abilityMs_);

    STAbilityUtil::WaitCompleted(
        event_, "OnStart", OHOS::AppExecFwk::AbilityLifecycleExecutor::LifecycleState::INACTIVE, 110);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    OHOS::AppExecFwk::RunningProcessInfo pInfo = STAbilityUtil::GetAppProcessInfoByName(bundleName, appMs_);
    EXPECT_TRUE(pInfo.pid_ > 0);

    EXPECT_EQ(0, hapRet);

    // stop ability
    STAbilityUtil::StopAbility("req_com_ohos_dpmsst_service_app_a1", 0, "StopSelfAbility");

    STAbilityUtil::WaitCompleted(
        event_, "OnStop", OHOS::AppExecFwk::AbilityLifecycleExecutor::LifecycleState::INITIAL, 110);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckCallerPermissionAndStartUsing_0200 end";
}

// /**
//  * @tc.number    : DPMS_CheckCallerPermissionAndStartUsing_0300
//  * @tc.name      : CheckCallerPermissionAndStartUsing
//  * @tc.desc      : Performance test
//  */
// HWTEST_F(DistributedPermissionRemindTest, DPMS_CheckCallerPermissionAndStartUsing_0300, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_CheckCallerPermissionAndStartUsing_0300 start";
//
//     GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_CheckCallerPermissionAndStartUsing_0300 end";
// }

// /**
//  * @tc.number    : DPMS_CheckCallerPermissionAndStartUsing_0400
//  * @tc.name      : CheckCallerPermissionAndStartUsing
//  * @tc.desc      : Stability test
//  */

// /**
//  * @tc.number    : DPMS_CheckCallerPermissionAndStartUsing_0500
//  * @tc.name      : CheckCallerPermissionAndStartUsing
//  * @tc.desc      : Fuzz test
//  */

/**
 * @tc.number    : DPMS_StartUsingPermission_0100
 * @tc.name      : StartUsingPermission
 * @tc.desc      : PermissionName is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0100 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);

    // usd the THIRD_TEST_HAP_NAME
    string permName = "";
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0100 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0200
 * @tc.name      : StartUsingPermission
 * @tc.desc      : AppInfo is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0200 start";
    DeleteAllRemind();
    string appInfo = "";
    string permName = PERMISSION_CAMERA;

    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0200 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0300
 * @tc.name      : StartUsingPermission
 * @tc.desc      : The DPMS does not start
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0300 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0300 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0400
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0400 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0400 end";

}

/**
 * @tc.number    : DPMS_StartUsingPermission_0500
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0500 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0500 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0600
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0600 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    std::string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_LAUNCHER);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0600 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0700
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0700 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);
        permName = PERMISSION_CAMERA;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0700 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0800
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0800 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);
        permName = PERMISSION_CAMERA;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);

        permName = PERMISSION_LOCATION;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0800 end";
}

/**
 * @tc.number    : DPMS_StartUsingPermission_0900
 * @tc.name      : StartUsingPermission
 * @tc.desc      : Performance test
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StartUsingPermission_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StartUsingPermission_0900 start";
    
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;

    RunTime_All = 0;
    RunTime_All += RunTime_DeviceInfoManager;
    RunTime_All += RunTime_BundleManager;
    RunTime_All += RunTime_Permissionmanager;
    RunTime_All += RunTime_AbilityManager;
    RunTime_All += RunTime_AnsManager;
    RunTime_All += RunTime_IPCSkeleton;

    EXPECT_LT(fp_ms.count() - RunTime_All, STDistibutePermissionUtil::MAX_ELAPSED);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StartUsingPermission_0900 end";
}

// /**
//  * @tc.number    : DPMS_StartUsingPermission_1000
//  * @tc.name      : StartUsingPermission
//  * @tc.desc      : Stability test
//  */

// /**
//  * @tc.number    : DPMS_StartUsingPermission_1100
//  * @tc.name      : StartUsingPermission
//  * @tc.desc      : Fuzz test
//  */

/**
 * @tc.number    : DPMS_StopUsingPermission_0100
 * @tc.name      : StartUsingPermission
 * @tc.desc      : PermissionName is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0100 start";
    
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0100 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0200
 * @tc.name      : StartUsingPermission
 * @tc.desc      : AppInfo is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0200 start";
    
    DeleteAllRemind();
    string appInfo = "";
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0200 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0300
 * @tc.name      : StartUsingPermission
 * @tc.desc      : AppInfo is null
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0300 start";
    
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0300 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0400
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0400 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);

        permName = PERMISSION_LOCATION;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0400 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0500
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0500 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0500 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0600
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0600 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0600 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0700
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0700 start";
 
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);


    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_LAUNCHER);
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_MICROPHONE;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);


    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0700 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0800
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0800 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);
        permName = PERMISSION_CAMERA;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }

    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);
        permName = PERMISSION_CAMERA;
        DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    }


    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0800 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_0900
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_0900 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);
        permName = PERMISSION_CAMERA;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);

        permName = PERMISSION_LOCATION;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }

    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);
        permName = PERMISSION_LOCATION;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_0900 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_1000
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1000 start";

    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    auto startTime = std::chrono::high_resolution_clock::now();
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    RunTime_All = 0;
    RunTime_All += RunTime_DeviceInfoManager;
    RunTime_All += RunTime_BundleManager;
    RunTime_All += RunTime_Permissionmanager;
    RunTime_All += RunTime_AbilityManager;
    RunTime_All += RunTime_AnsManager;
    RunTime_All += RunTime_IPCSkeleton;
    EXPECT_LT(fp_ms.count() - RunTime_All, STDistibutePermissionUtil::MAX_ELAPSED);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1000 end";
}

// /**
//  * @tc.number    : DPMS_StopUsingPermission_1100
//  * @tc.name      : StartUsingPermission
//  */
// HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1100, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1100 start";

//     GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1100 end";
// }

// /**
//  * @tc.number    : DPMS_StopUsingPermission_1200
//  * @tc.name      : StartUsingPermission
//  */
// HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1200, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1200 start";

//     GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1200 end";
// }

/**
 * @tc.number    : DPMS_StopUsingPermission_1300
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1300 start";

    OHOS::sptr<TestCallback> callback(new TestCallback());
    callback->permName = "DPMS_RegisterUsingPermissionReminder_0300";
    int32_t ret = -1;

    ret = DistributedPermissionKit::RegisterUsingPermissionReminder(callback);
    EXPECT_EQ(0, ret);

    DeleteAllRemind();
    string appInfo = "";
    string permName = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_TEST_BUNDLE_NAME);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_LOCATION;
    ret = DistributedPermissionKit::CheckPermissionAndStartUsing(permName, appInfo);
    EXPECT_EQ(0, ret);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    
    ret = DistributedPermissionKit::UnregisterUsingPermissionReminder(callback);
    EXPECT_EQ(0, ret);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1300 end";
}

// /**
//  * @tc.number    : DPMS_StopUsingPermission_1400
//  * @tc.name      : StartUsingPermission
//  */
// HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1400, TestSize.Level1)
// {
//     GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1400 start";

//     GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1400 end";
// }


/**
 * @tc.number    : DPMS_StopUsingPermission_1500
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1500 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1500 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_1600
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1600 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);
    string permName = "";

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);

    permName = PERMISSION_LOCATION;
    DistributedPermissionKit::StopUsingPermission(permName, appInfo);
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1600 end";
}

/**
 * @tc.number    : DPMS_StopUsingPermission_1700
 * @tc.name      : StartUsingPermission
 */
HWTEST_F(DistributedPermissionRemindTest, DPMS_StopUsingPermission_1700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedPermissionRemindTest DPMS_StopUsingPermission_1700 start";
    DeleteAllRemind();
    string appInfo = "";
    char deviceId[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(deviceId, DEVICE_UUID_LENGTH);
    string permName = PERMISSION_CAMERA;
    uid_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_SETTINGS);
    deviceId_ = deviceId;
    appInfo = GetAppIdInfo(deviceId_);

    permName = PERMISSION_CAMERA;
    DistributedPermissionKit::StartUsingPermission(permName, appInfo);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);

    for(auto deviceinfo : deviceList)
    {
        deviceId_ = deviceinfo.deviceId;
        appInfo = GetAppIdInfo(deviceId_);

        permName = PERMISSION_LOCATION;
        DistributedPermissionKit::StartUsingPermission(permName, appInfo);
    }
    GTEST_LOG_(INFO) << "DistributedPermissionRecordsTest DPMS_StopUsingPermission_1700 end";
}

