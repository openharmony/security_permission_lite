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

#include "stress_test_api.h"

using std::string;
using namespace OHOS::STPermissionUtil;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::AppExecFwk;
using namespace OHOS::STABUtil;
using namespace OHOS::EventFwk;
using namespace OHOS::STtools;
using MAP_STR_STR = std::map<std::string, std::string>;
namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "StressTestApi"};
}

bool setFlag = false;
int32_t uid_ = 0;
string permName_ = "ohos.permission.WRITE_CONTACTS";
static const std::string SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_SYSTEM_SIGNED =
    "com.ohos.dpmsst.service.distributed.system.signed";
static const std::string SYSTEM_TEST_DISTRIBUTED_SYSTEM_SIGNED_ABILITY = "DpmsStServiceDistributedSystemSigned";
const std::string REQ_EVENT_NAME_SYSTEM_SIGNED = "req_com_ohos_dpmsst_service_distributed_system_signed";
static const std::string SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A = "com.ohos.dpmsst.service.distributed.third.a";
static const std::string SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY = "DpmsStServiceDistributedThirdA";
const std::string REQ_EVENT_NAME_THIRD_A = "req_com_ohos_dpmsst_service_distributed_third_a";
std::vector<std::string> DistibutePermissionStressTest::eventList_ = {
    "resp_com_ohos_dpmsst_service_distributed_system_signed", "resp_com_ohos_dpmsst_service_distributed_third_a"};
int firstFlag = 0;
STtools::Event DistibutePermissionStressTest::event_ = STtools::Event();
sptr<OHOS::AppExecFwk::IAppMgr> DistibutePermissionStressTest::appMs_ = nullptr;
sptr<OHOS::AAFwk::IAbilityManager> DistibutePermissionStressTest::abilityMs_ = nullptr;
std::shared_ptr<DistibutePermissionStressTest::AppEventSubscriber> DistibutePermissionStressTest::subscriber_ = nullptr;
string DistibutePermissionStressTest::GetAppIdInfo(int32_t pid, int32_t uid)
{
    return DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid, uid);
}

bool DistibutePermissionStressTest::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    for (const auto &e : DistibutePermissionStressTest::eventList_) {
        matchingSkills.AddEvent(e);
    }
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void DistibutePermissionStressTest::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    std::cout << "OnReceiveEvent: event=" << data.GetWant().GetAction() << std::endl;
    std::cout << "OnReceiveEvent: data=" << data.GetData() << std::endl;
    std::cout << "OnReceiveEvent: code=" << data.GetCode() << std::endl;

    auto eventName = data.GetWant().GetAction();
    auto iter = std::find(
        DistibutePermissionStressTest::eventList_.begin(), DistibutePermissionStressTest::eventList_.end(), eventName);
    if (iter != DistibutePermissionStressTest::eventList_.end()) {
        STAbilityUtil::Completed(event_, data.GetData(), data.GetCode());
    }
}

void RemoveStorage()
{
    GenericValues null;
    DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_VISITOR, null);
    DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_RECORD, null);
}

void SetUid()
{
    if (!setFlag) {
        {
            system(("chmod 644 /data/" + DPMS_THIRD_HAP_NAME + ".hap").c_str());
            system(("bm install -p /data/" + DPMS_THIRD_HAP_NAME + ".hap").c_str());
        }
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            PERMISSION_LOG_ERROR(LABEL, "fail to get system ability mgr.");
            return;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (!remoteObject) {
            PERMISSION_LOG_ERROR(LABEL, "fail to get bundle manager proxy.");
            return;
        }
        PERMISSION_LOG_INFO(LABEL, "get bundle manager proxy success.");

        sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
        uid_ = bundleMgrProxy->GetUidByBundleName(DPMS_THIRD_BUNDLE_NAME, DEFAULT_USERID);
        setFlag = true;
    }
}

void InitVisitorData(std::string bundleName)
{
    std::vector<GenericValues> visitor;
    GenericValues genericVisitor;
    genericVisitor.Put(FIELD_DEVICE_ID, "device_0");
    genericVisitor.Put(FIELD_DEVICE_NAME, "device_name_0");
    genericVisitor.Put(FIELD_BUNDLE_USER_ID, 0);
    genericVisitor.Put(FIELD_BUNDLE_NAME, bundleName);
    genericVisitor.Put(FIELD_BUNDLE_LABEL, "bundleInfo.label_0");
    visitor.emplace_back(genericVisitor);

    GenericValues genericVisitor1;
    genericVisitor1.Put(FIELD_DEVICE_ID, "device_1");
    genericVisitor1.Put(FIELD_DEVICE_NAME, "device_name_1");
    genericVisitor1.Put(FIELD_BUNDLE_USER_ID, 0);
    genericVisitor1.Put(FIELD_BUNDLE_NAME, "bundleName_1");
    genericVisitor1.Put(FIELD_BUNDLE_LABEL, "bundleInfo.label_1");
    visitor.emplace_back(genericVisitor1);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_VISITOR, visitor);
}
void InitRecordData()
{
    std::vector<GenericValues> visitor;
    DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitor);
    int visitorId = 0;
    if (visitor.size() > 0) {
        visitorId = visitor[0].GetInt(FIELD_ID);
    }

    std::vector<GenericValues> record;
    GenericValues genericRecordFore;
    genericRecordFore.Put(FIELD_TIMESTAMP, 100);
    genericRecordFore.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordFore.Put(FIELD_OP_CODE, 4);
    genericRecordFore.Put(FIELD_IS_FOREGROUND, 1);
    genericRecordFore.Put(FIELD_ACCESS_COUNT, 3);
    genericRecordFore.Put(FIELD_REJECT_COUNT, 2);

    GenericValues genericRecordBack;
    genericRecordBack.Put(FIELD_TIMESTAMP, 200);
    genericRecordBack.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordBack.Put(FIELD_OP_CODE, 4);
    genericRecordBack.Put(FIELD_IS_FOREGROUND, 0);
    genericRecordBack.Put(FIELD_ACCESS_COUNT, 1);
    genericRecordBack.Put(FIELD_REJECT_COUNT, 0);

    GenericValues genericRecord;
    genericRecord.Put(FIELD_TIMESTAMP, 300);
    genericRecord.Put(FIELD_VISITOR_ID, visitorId);
    genericRecord.Put(FIELD_OP_CODE, 1);
    genericRecord.Put(FIELD_IS_FOREGROUND, 0);
    genericRecord.Put(FIELD_ACCESS_COUNT, 1);
    genericRecord.Put(FIELD_REJECT_COUNT, 3);
    record.emplace_back(genericRecordFore);
    record.emplace_back(genericRecordBack);
    record.emplace_back(genericRecord);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_RECORD, record);
}

void AddData(int64_t timestamp)
{
    std::vector<GenericValues> visitor;
    GenericValues genericVisitor;

    genericVisitor.Put(FIELD_DEVICE_ID, "device_1");
    genericVisitor.Put(FIELD_DEVICE_NAME, "device_name_1");
    genericVisitor.Put(FIELD_BUNDLE_USER_ID, 0);
    genericVisitor.Put(FIELD_BUNDLE_NAME, "bundleName");
    genericVisitor.Put(FIELD_BUNDLE_LABEL, "bundleInfo.label");
    visitor.emplace_back(genericVisitor);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_VISITOR, visitor);
    visitor.clear();
    DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitor);
    int visitorId = 0;
    if (visitor.size() > 0) {
        visitorId = visitor[0].GetInt(FIELD_ID);
    }
    std::vector<GenericValues> record;
    GenericValues genericRecordFore;

    genericRecordFore.Put(FIELD_TIMESTAMP, timestamp);
    genericRecordFore.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordFore.Put(FIELD_OP_CODE, 1);
    genericRecordFore.Put(FIELD_IS_FOREGROUND, 1);
    genericRecordFore.Put(FIELD_ACCESS_COUNT, 1);
    genericRecordFore.Put(FIELD_REJECT_COUNT, 0);
    record.emplace_back(genericRecordFore);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_RECORD, record);
}

class TestCallback : public OnPermissionUsedRecordStub {
public:
    TestCallback() = default;
    virtual ~TestCallback() = default;

    void OnQueried(ErrCode code, QueryPermissionUsedResult &result)
    {}
};

bool DistibutePermissionStressTest::CheckDPermissionTest_01()
{
    const string PERMISSION_NAME = "ohos.permission.GET_NERWORK_INFO";
    const int32_t INVALID_DUID = -1;
    int result = DistributedPermissionKit::CheckDPermission(INVALID_DUID, PERMISSION_NAME);
    return result == -1 ? true : false;
}

bool DistibutePermissionStressTest::CheckDPermissionTest_02()
{
    const string PERMISSION_NAME = "ohos.permission.GET_NERWORK_INFO";
    const int32_t DUID = 10001;
    int result = DistributedPermissionKit::CheckDPermission(DUID, PERMISSION_NAME);
    return result == 0 ? true : false;
}

bool DistibutePermissionStressTest::AddPermissionUsedRecordTest_01()
{
    SetUid();
    RemoveStorage();

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorBefore);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordBefore);

    string deviceId_ = "device_0";
    string appInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid_, deviceId_);

    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorAfter);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordAfter);

    return (visitorAfter.size() == visitorBefore.size() + 1) && (recordAfter.size() == visitorBefore.size() + 1);
}

bool DistibutePermissionStressTest::AddPermissionUsedRecordTest_02()
{
    SetUid();
    RemoveStorage();
    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorBefore);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordBefore);

    string deviceId_ = "device_0";
    string appInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid_, deviceId_);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    string permName = "ohos.permission.LOCATION";
    DistributedPermissionKit::AddPermissionUsedRecord(permName, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorAfter);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordAfter);

    return (visitorAfter.size() == visitorBefore.size() + 1) && (recordAfter.size() == recordBefore.size() + 2);
}

bool DistibutePermissionStressTest::AddPermissionUsedRecordTest_03()
{
    SetUid();
    RemoveStorage();
    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorBefore);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordBefore);

    string deviceId_ = "device_0";
    string appInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid_, deviceId_);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    sleep(3);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorAfter);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordAfter);

    return (visitorAfter.size() == visitorBefore.size() + 1) && (recordAfter.size() == recordBefore.size() + 1);
}

bool DistibutePermissionStressTest::AddPermissionUsedRecordTest_04()
{
    SetUid();
    RemoveStorage();
    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorBefore);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordBefore);
    string deviceId_ = "device_0";
    string appInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid_, deviceId_);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    sleep(61);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorAfter);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordAfter);
    return (visitorAfter.size() == visitorBefore.size() + 1) && (recordAfter.size() == recordBefore.size() + 2);
}

bool DistibutePermissionStressTest::AddPermissionUsedRecordTest_05()
{
    SetUid();
    RemoveStorage();
    AddData(TimeUtil::GetTimestamp() - 2592000);

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorBefore);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordBefore);

    string deviceId_ = "device_0";
    string appInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid_, deviceId_);

    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorAfter);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordAfter);

    return (visitorAfter.size() == visitorBefore.size() + 1) && (recordAfter.size() == recordBefore.size());
}

bool DistibutePermissionStressTest::AddPermissionUsedRecordTest_06()
{
    SetUid();
    RemoveStorage();
    AddData(TimeUtil::GetTimestamp() - 2591999);

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorBefore);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordBefore);

    string deviceId_ = "device_0";
    string appInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid_, deviceId_);

    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorAfter);
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordAfter);

    return (visitorAfter.size() == visitorBefore.size() + 1) && (recordAfter.size() == recordBefore.size() + 1);
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_01()
{
    RemoveStorage();
    AddData(TimeUtil::GetTimestamp() - 2592000);

    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    sleep(3);
    std::vector<GenericValues> record;
    SqliteStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, record);
    int recordSize = (int)record.size();
    return recordSize == 0;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_02()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.endTimeMillis = 250;
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();

    return recordSize == 1;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_03()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.beginTimeMillis = 150;
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();

    return recordSize == 2;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_04()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();

    return recordSize == 2;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_05()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    bool detailFlag = false;
    for (auto bundle : result.bundlePermissionUsedRecords) {
        for (auto record : bundle.permissionUsedRecords) {
            if (record.accessRecordBg.size() > 0 || record.accessRecordFg.size() > 0 ||
                record.rejectRecordBg.size() > 0 || record.rejectRecordFg.size() > 0) {
                detailFlag = true;
            }
        }
    }

    return !detailFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_06()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.flag = 1;
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    bool detailFlag = false;
    for (auto bundle : result.bundlePermissionUsedRecords) {
        for (auto record : bundle.permissionUsedRecords) {
            if (record.accessRecordBg.size() > 0 || record.accessRecordFg.size() > 0 ||
                record.rejectRecordBg.size() > 0 || record.rejectRecordFg.size() > 0) {
                detailFlag = true;
            }
        }
    }

    return detailFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_07()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.deviceLabel = "device_name_0";
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    bool resultFlag = false;
    resultFlag = (int)result.bundlePermissionUsedRecords.size() == 1;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].deviceLabel.c_str() == request.deviceLabel.c_str()
        : resultFlag = false;
    resultFlag == true ? resultFlag = (int)result.bundlePermissionUsedRecords[0].permissionUsedRecords.size() == 1
                       : resultFlag = false;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].permissionUsedRecords[0].permissionName.c_str() ==
                       request.permissionNames[0].c_str()
        : resultFlag = false;
    return resultFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_08()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.bundleName = "bundleName_0";
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);

    bool resultFlag = false;
    resultFlag = (int)result.bundlePermissionUsedRecords.size() == 1;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].bundleName.c_str() == request.bundleName.c_str()
        : resultFlag = false;
    resultFlag == true ? resultFlag = (int)result.bundlePermissionUsedRecords[0].permissionUsedRecords.size() == 1
                       : resultFlag = false;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].permissionUsedRecords[0].permissionName.c_str() ==
                       request.permissionNames[0].c_str()
        : resultFlag = false;

    return resultFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_09()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.deviceLabel = "device_name_0";
    request.bundleName = "bundleName_0";
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);

    bool resultFlag = false;
    resultFlag = (int)result.bundlePermissionUsedRecords.size() == 1;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].bundleName.c_str() == request.bundleName.c_str()
        : resultFlag = false;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].deviceLabel.c_str() == request.deviceLabel.c_str()
        : resultFlag = false;
    resultFlag == true ? resultFlag = (int)result.bundlePermissionUsedRecords[0].permissionUsedRecords.size() == 1
                       : resultFlag = false;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].permissionUsedRecords[0].permissionName.c_str() ==
                       request.permissionNames[0].c_str()
        : resultFlag = false;

    return resultFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_10()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.deviceLabel = "device_name_0";
    request.bundleName = "bundleName_0";
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);

    bool resultFlag = false;
    resultFlag = (int)result.bundlePermissionUsedRecords.size() == 1;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].bundleName.c_str() == request.bundleName.c_str()
        : resultFlag = false;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].deviceLabel.c_str() == request.deviceLabel.c_str()
        : resultFlag = false;
    return resultFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_11()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.deviceLabel = "device_name_0";
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);

    bool resultFlag = false;
    resultFlag = (int)result.bundlePermissionUsedRecords.size() == 1;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].deviceLabel.c_str() == request.deviceLabel.c_str()
        : resultFlag = false;
    return resultFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_12()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.bundleName = "bundleName_0";
    QueryPermissionUsedResult result;
    DistributedPermissionKit::GetPermissionUsedRecords(request, result);

    bool resultFlag = false;
    resultFlag = (int)result.bundlePermissionUsedRecords.size() == 1;
    resultFlag == true
        ? resultFlag = result.bundlePermissionUsedRecords[0].bundleName.c_str() == request.bundleName.c_str()
        : resultFlag = false;

    return resultFlag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_13()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    request.permissionNames.emplace_back("ohos.permission.READ_CONTACTS");
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();

    bool flag = recordSize == 2 && state == 0;
    auto bundle = result.bundlePermissionUsedRecords[0];
    if (bundle.permissionUsedRecords.size() > 0) {
        if (bundle.permissionUsedRecords[0].permissionName == request.permissionNames[0]) {
            flag == true
                ? flag = bundle.permissionUsedRecords[0].permissionName.c_str() == request.permissionNames[0].c_str()
                : flag = false;
            flag == true
                ? flag = bundle.permissionUsedRecords[1].permissionName.c_str() == request.permissionNames[1].c_str()
                : flag = false;
        } else {
            flag == true
                ? flag = bundle.permissionUsedRecords[0].permissionName.c_str() == request.permissionNames[1].c_str()
                : flag = false;
            flag == true
                ? flag = bundle.permissionUsedRecords[1].permissionName.c_str() == request.permissionNames[0].c_str()
                : flag = false;
        }
    }

    return flag;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordTest_14()
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();

    return recordSize == 2 && state == 0;
    ;
}

bool DistibutePermissionStressTest::GetPermissionUsedRecordAsyncTest()
{
    OHOS::sptr<TestCallback> callback(new TestCallback());
    QueryPermissionUsedRequest request;
    request.beginTimeMillis = 1;
    request.endTimeMillis = 1111111111111111;
    request.deviceLabel = "device_name_0";
    request.permissionNames.emplace_back("ohos.permission.LOCATION");

    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, callback);

    return state == 0;
}

bool DistibutePermissionStressTest::DPMS_VerifyPermissionFromRemote_2200()
{
    int32_t expectation = Constant::PERMISSION_DENIED;
    remotePermissionPrepare();
    int result = 0;
    int code = 0;
    int waitTIme = 3;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, code, "VerifyPermissionFromRemote");
    result += STAbilityUtil::WaitCompleted(event_, "VerifyPermissionFromRemote", expectation, waitTIme);
    std::cout << "DPMS_VerifyPermissionFromRemote_2200:" << result << std::endl;
    return result == 0;
}
bool DistibutePermissionStressTest::DPMS_VerifySelfPermissionFromRemote_2200()
{
    remotePermissionPrepare();
    int result = 0;
    int code = 0;
    int waitTIme = 3;
    int32_t expectation = Constant::PERMISSION_DENIED;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, code, "VerifySelfPermissionFromRemote");
    result += STAbilityUtil::WaitCompleted(event_, "VerifySelfPermissionFromRemote", expectation, waitTIme);
    std::cout << "DPMS_VerifySelfPermissionFromRemote_2200:" << result << std::endl;
    return result == 0;
}
bool DistibutePermissionStressTest::DPMS_CanRequestPermissionFromRemote_2100()
{
    remotePermissionPrepare();
    int result = 0;
    int code = 0;
    int waitTIme = 3;
    int32_t expectation = 1;
    STAbilityUtil::PublishEvent(REQ_EVENT_NAME_THIRD_A, code, "CanRequestPermissionFromRemote");
    result += STAbilityUtil::WaitCompleted(event_, "CanRequestPermissionFromRemote", expectation, waitTIme);
    std::cout << "DPMS_CanRequestPermissionFromRemote_2100:" << result << std::endl;
    return result == 0;
}

bool DistibutePermissionStressTest::DPMS_GrantSensitivePermissionToRemoteApp_1300()
{
    std::string nodeId = remotePermissionPrepare();
    std::string deviceId = nodeId;
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permission = Constant::MICROPHONE;
    int32_t expectation = Constant::PERMISSION_GRANTED;
    const std::string SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A = "com.ohos.dpmsst.service.distributed.third.a";
    int32_t uid = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A);
    service->GrantSensitivePermissionToRemoteApp(permission, nodeId, uid);
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId, uid);
    int32_t result = DistributedPermissionKit::CheckDPermission(duid, permission);
    return result == expectation;
}

std::string DistibutePermissionStressTest::remotePermissionPrepare()
{
    appMs_ = STAbilityUtil::GetAppMgrService();
    abilityMs_ = STAbilityUtil::GetAbilityManagerService();
    if (appMs_) {
        appMs_->SetAppFreezingTime(60);
        int time = 0;
        appMs_->GetAppFreezingTime(time);
    }
    auto list = DeviceInfoRepository::GetInstance().ListDeviceInfo();
    auto sleepTime = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(sleepTime);

    std::vector<OHOS::DistributedHardware::DmDeviceInfo> deviceList;
    std::string str = "com.ohos.distributedmusicplayer";
    STDistibutePermissionUtil::GetTrustedDeviceList(str, deviceList);
    string permission = Constant::CAMERA;
    string deviceId = deviceList[0].deviceId;
    string nodeId = deviceId;

    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::string abilityName = SYSTEM_TEST_DISTRIBUTED_THIRD_A_ABILITY;
    MAP_STR_STR params;
    params["permission"] = permission;
    params["nodeId"] = nodeId;
    Want want = STAbilityUtil::MakeWant(deviceId, abilityName, bundleName, params);
    if (firstFlag == 0) {
        firstFlag++;
        SubscribeEvent();
        STAbilityUtil::StartAbility(want, abilityMs_, 1000);
        STAbilityUtil::WaitCompleted(event_, "OnStart", AbilityLifecycleExecutor::LifecycleState::INACTIVE, 3);
    }
    return deviceId;
}
void DistibutePermissionStressTest::GetTestFunction(std::vector<FnPtr> &vector)
{
    // vector.push_back(CheckDPermissionTest_01);
    // vector.push_back(CheckDPermissionTest_02);
    vector.push_back(AddPermissionUsedRecordTest_01);
    // vector.push_back(AddPermissionUsedRecordTest_02);
    vector.push_back(GetPermissionUsedRecordTest_01);
    vector.push_back(GetPermissionUsedRecordTest_02);
    vector.push_back(GetPermissionUsedRecordTest_03);
    vector.push_back(GetPermissionUsedRecordTest_04);
    vector.push_back(GetPermissionUsedRecordTest_05);
    vector.push_back(GetPermissionUsedRecordTest_06);
    vector.push_back(GetPermissionUsedRecordTest_07);
    vector.push_back(GetPermissionUsedRecordTest_08);
    vector.push_back(GetPermissionUsedRecordTest_09);
    vector.push_back(GetPermissionUsedRecordTest_10);
    vector.push_back(GetPermissionUsedRecordTest_11);
    vector.push_back(GetPermissionUsedRecordTest_12);
    vector.push_back(GetPermissionUsedRecordTest_13);
    vector.push_back(GetPermissionUsedRecordTest_14);
    // vector.push_back(GetPermissionUsedRecordAsyncTest);
    // vector.push_back(DeletePermissionUsedRecordTest_01);
    // vector.push_back(DeletePermissionUsedRecordTest_02);
    // vector.push_back(DPMS_VerifyPermissionFromRemote_2200);
    // vector.push_back(GetPermissionUsedRecordTest_01);
    // vector.push_back(GetPermissionUsedRecordTest_02);
    // vector.push_back(GetPermissionUsedRecordAsyncTest);
    // vector.push_back(DeletePermissionUsedRecordTest_01);
    // vector.push_back(DeletePermissionUsedRecordTest_02);
    vector.push_back(DPMS_VerifyPermissionFromRemote_2200);
    vector.push_back(DPMS_VerifySelfPermissionFromRemote_2200);
    vector.push_back(DPMS_CanRequestPermissionFromRemote_2100);
    vector.push_back(DPMS_GrantSensitivePermissionToRemoteApp_1300);
}

}  // namespace Permission
}  // namespace Security
}  // namespace OHOS