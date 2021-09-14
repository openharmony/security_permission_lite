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
#include "device_info_manager.h"
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "ipc_skeleton.h"
#include "time_util.h"
#include "distributed_permission_kit.h"
#include "sqlite_storage.h"
#include "singleton.h"
#include "bundle_info.h"
#include "on_permission_used_record_stub.h"
#include "permission_definition.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::AppExecFwk;
using namespace OHOS::DistributedHardware;

using std::string;

namespace {
std::recursive_mutex statckLock_;
string deviceId_;
string deviceName_;
int32_t pid_ = 0;
int32_t uid_ = 0;
std::string bundleName_;
bool initFlag = false;
string permName_ = "ohos.permission.WRITE_CONTACTS";
OHOS::AppExecFwk::BundleInfo bundleInfo_;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_PERMISSION, "DistributedPermissionUsedRecordsTest"};
}  // namespace

class DistributedPermissionUsedRecordsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static void RemoveStorage();
    void AddData(int64_t timestamp);
    void InitRecordData();
    void InitVisitorData(std::string bundleName);
    string GetAppIdInfo(string &deviceId_);
    void FindDataStorageAll(DataStorage::DataType dataType, std::vector<GenericValues> &result);
    void AddDistributedPermissionUsedRecord(string &permName, string &appInfo, int32_t sucCount, int32_t failCount);
};

void DistributedPermissionUsedRecordsTest::SetUpTestCase(void)
{}

void DistributedPermissionUsedRecordsTest::TearDownTestCase(void)
{}

void DistributedPermissionUsedRecordsTest::SetUp(void)
{
    if (initFlag) {
        return;
    }
    char localDeviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, Constant::DEVICE_UUID_LENGTH);
    deviceId_ = localDeviceId;
    PERMISSION_LOG_INFO(LABEL, "localDeviceId  =%{public}s ", localDeviceId);

    // install package
    {
        STDistibutePermissionUtil::Install(DPMS_THIRD_HAP_NAME);
    }
    BundleInfo bundleInfo;
    // get uid
    int systemBundleUid = STDistibutePermissionUtil::GetUidByBundleName(DPMS_THIRD_BUNDLE_NAME);
    uid_ = systemBundleUid;

    // get bundleNameTest
    bool result = STDistibutePermissionUtil::GetBundleNameForUid(uid_, bundleName_);
    if (result) {
        PERMISSION_LOG_INFO(LABEL, "GetBundleNameForUid, bundleName=%{public}s ", bundleName_.c_str());
    } else {
        PERMISSION_LOG_INFO(LABEL, "GetBundleNameForUid, failed ");
    }

    // get bundleInfo
    STDistibutePermissionUtil::GetBundleInfoByBundleName(DPMS_THIRD_BUNDLE_NAME, bundleInfo);
    PERMISSION_LOG_DEBUG(
        LABEL, "GetUidByBundleName, systemBundleUid=%{public}d,uid =%{public}d ", systemBundleUid, bundleInfo.uid);
    BundleInfo bundleInfo2;
    STDistibutePermissionUtil::GetBundleInfoByUid(systemBundleUid, bundleInfo2);
    PERMISSION_LOG_DEBUG(LABEL, "GetBundleInfoByUid bundleName = %{public}s", bundleInfo2.name.c_str());

    std::vector<GenericValues> visitor;
    string appInfo = GetAppIdInfo(deviceId_);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitor);
    deviceName_ = visitor[0].GetString(FIELD_DEVICE_NAME);

    PERMISSION_LOG_INFO(LABEL, "deviceName_  =%{public}s ", deviceName_.c_str());
    initFlag = true;
}

void DistributedPermissionUsedRecordsTest::TearDown(void)
{}

void DistributedPermissionUsedRecordsTest::InitVisitorData(std::string bundleName)
{
    std::vector<GenericValues> visitor;
    GenericValues genericVisitor;
    genericVisitor.Put(FIELD_DEVICE_ID, deviceId_);
    genericVisitor.Put(FIELD_DEVICE_NAME, deviceName_);
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
void DistributedPermissionUsedRecordsTest::InitRecordData()
{
    int opCode = 4;
    int rejectCount = 3;
    int timeHundred = 100;
    std::vector<GenericValues> visitor;
    DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitor);
    int visitorId = 0;
    if (visitor.size() > 0) {
        visitorId = visitor[0].GetInt(FIELD_ID);
    }

    std::vector<GenericValues> record;
    GenericValues genericRecordFore;
    genericRecordFore.Put(FIELD_TIMESTAMP, timeHundred);
    genericRecordFore.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordFore.Put(FIELD_OP_CODE, opCode);
    genericRecordFore.Put(FIELD_IS_FOREGROUND, 1);
    genericRecordFore.Put(FIELD_ACCESS_COUNT, rejectCount);
    genericRecordFore.Put(FIELD_REJECT_COUNT, rejectCount);

    GenericValues genericRecordBack;
    genericRecordBack.Put(FIELD_TIMESTAMP, timeHundred + timeHundred);
    genericRecordBack.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordBack.Put(FIELD_OP_CODE, opCode);
    genericRecordBack.Put(FIELD_IS_FOREGROUND, 0);
    genericRecordBack.Put(FIELD_ACCESS_COUNT, 1);
    genericRecordBack.Put(FIELD_REJECT_COUNT, 0);

    GenericValues genericRecord;
    genericRecord.Put(FIELD_TIMESTAMP, timeHundred + timeHundred + timeHundred);
    genericRecord.Put(FIELD_VISITOR_ID, visitorId);
    genericRecord.Put(FIELD_OP_CODE, 1);
    genericRecord.Put(FIELD_IS_FOREGROUND, 0);
    genericRecord.Put(FIELD_ACCESS_COUNT, 1);
    genericRecord.Put(FIELD_REJECT_COUNT, rejectCount);
    record.emplace_back(genericRecordFore);
    record.emplace_back(genericRecordBack);
    record.emplace_back(genericRecord);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_RECORD, record);
}

// delete all records
void DistributedPermissionUsedRecordsTest::RemoveStorage()
{
    GenericValues null;
    DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_VISITOR, null);
    DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_RECORD, null);
}

// create appinfo
string DistributedPermissionUsedRecordsTest::GetAppIdInfo(string &deviceId_)
{
    return DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid_, uid_, deviceId_);
}

// callback
class TestCallback : public OnPermissionUsedRecordStub {
public:
    TestCallback() = default;
    virtual ~TestCallback() = default;

    void OnQueried(ErrCode code, QueryPermissionUsedResult &result)
    {
        int successCode = 200;
        PERMISSION_LOG_DEBUG(LABEL, "------------time end-------------");
        EXPECT_EQ(code, successCode);
    }
};

// find all records
void DistributedPermissionUsedRecordsTest::FindDataStorageAll(
    DataStorage::DataType dataType, std::vector<GenericValues> &result)
{
    SqliteStorage::GetRealDataStorage().Find(dataType, result);
}

// Add data directly.
void DistributedPermissionUsedRecordsTest::AddData(int64_t timestamp)
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

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0100
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : permName is null, don't add.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0100, TestSize.Level1)
{
    RemoveStorage();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0100 start");

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId_);
    string permName;
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0100 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size());
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size());
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest PMS_Page_Ability_0100 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0200
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : appIdInfo(deviceId) is null, don't add.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0200, TestSize.Level1)
{
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0200 start");
    RemoveStorage();
    string deviceId = "";
    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0200 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size());
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size());
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0200 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0300
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : permissionVisitor table and permissionRecord table don't exist input data, add data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0300, TestSize.Level1)
{
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0300 start");
    RemoveStorage();

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0300 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size() + 1);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0300 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0400
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : permissionVisitor table already exists input data, permissionRecord table don't exist input data,
 *                 permissionRecord table add data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0400, TestSize.Level1)
{
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0400 start");
    RemoveStorage();

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0400 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    string permName = "ohos.permission.LOCATION";
    DistributedPermissionKit::AddPermissionUsedRecord(permName, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size() + 2);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0400 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0500
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : permissionVisitor table and permissionRecord table already exists input data, permissionRecord
 table
 *                 need to drop precision of timestamp, permissionRecord table update data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0500, TestSize.Level1)
{
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0500 start");
    RemoveStorage();

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0500 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    sleep(3);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size() + 1);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0500 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0600
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : permissionVisitor table and permissionRecord table already exists input data, permissionRecord
 table
 *                 does not need to drop precision of timestamp, permissionRecord table add data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0600, TestSize.Level1)
{
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0600 start");
    RemoveStorage();

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0600 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    sleep(61);
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size() + 2);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0600 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0700
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : There are more than 30 days of data in the permissionRecord table, which will be deleted when
 *                 the add operation is performed.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0700, TestSize.Level1)
{
    RemoveStorage();
    AddData(TimeUtil::GetTimestamp() - 2592000);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0700 start");

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 1);
    EXPECT_EQ((int)recordBefore.size(), 1);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0700 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size());
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0700 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0800
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : There is no more than 30 days of data in the permissionRecord table, which will not be deleted
 when
 *                 the add operation is performed.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0800, TestSize.Level1)
{
    RemoveStorage();
    AddData(TimeUtil::GetTimestamp() - 2591999);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0800 start");

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 1);
    EXPECT_EQ((int)recordBefore.size(), 1);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0800 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size() + 1);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0800 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_0900
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : AddPermissionUsedRecord permormance test, expect to take less than 0.5ms.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_0900, TestSize.Level1)
{
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0900 start");
    RemoveStorage();

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_0500 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName_.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    sleep(3);
    auto startTime = std::chrono::high_resolution_clock::now();
    PERMISSION_LOG_DEBUG(LABEL, "------------time start-------------");
    DistributedPermissionKit::AddPermissionUsedRecord(permName_, appInfo, 1, 0);
    PERMISSION_LOG_DEBUG(LABEL, "------------time end-------------");
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    EXPECT_LT(fp_ms.count(), 0.5);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size() + 1);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_0900 end");
}

/**
 * @tc.number    : DPMS_AddPermissionUsedRecord_1200
 * @tc.name      : AddPermissionUsedRecord
 * @tc.desc      : PermissionName is not exist in the translate map.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_AddPermissionUsedRecord_1200, TestSize.Level1)
{
    RemoveStorage();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_1200 start");

    std::vector<GenericValues> visitorBefore;
    std::vector<GenericValues> recordBefore;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorBefore);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordBefore);
    EXPECT_EQ((int)visitorBefore.size(), 0);
    EXPECT_EQ((int)recordBefore.size(), 0);

    string permName = "ohos.permission.NULL";
    string appInfo = GetAppIdInfo(deviceId_);
    PERMISSION_LOG_DEBUG(LABEL,
        "init DPMS_AddPermissionUsedRecord_1200 deviceId_= %{public}s, appInfo=%{public}s, permName=%{public}s",
        deviceId_.c_str(),
        appInfo.c_str(),
        permName.c_str());
    DistributedPermissionKit::AddPermissionUsedRecord(permName, appInfo, 1, 0);

    std::vector<GenericValues> visitorAfter;
    std::vector<GenericValues> recordAfter;
    FindDataStorageAll(DataStorage::PERMISSION_VISITOR, visitorAfter);
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, recordAfter);
    EXPECT_EQ((int)visitorAfter.size(), (int)visitorBefore.size() + 1);
    EXPECT_EQ((int)recordAfter.size(), (int)recordBefore.size());
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_AddPermissionUsedRecord_1200 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0100
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : There are more than 30 days of data in the permissionRecord table, which will be deleted when
 *                 the search operation is performed.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0100, TestSize.Level1)
{
    RemoveStorage();
    AddData(TimeUtil::GetTimestamp() - 2592000);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0100 start");

    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    sleep(3);
    std::vector<GenericValues> record;
    FindDataStorageAll(DataStorage::PERMISSION_RECORD, record);
    EXPECT_EQ((int)record.size(), 0);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0100 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0200
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest beginTimeMillis > endTimeMillis > 0, time conditions are invalid, find
 *                 faild.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0200, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0200 start");

    QueryPermissionUsedRequest request;
    request.beginTimeMillis = 150;
    request.endTimeMillis = 100;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, -1);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 0);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0200 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0300
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest beginTimeMillis < 0, endTimeMillis > 0, time conditions are invalid,
 find
 *                 faild.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0300, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0300 start");

    QueryPermissionUsedRequest request;
    request.beginTimeMillis = -1;
    request.endTimeMillis = 200;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, -1);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 0);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0300 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0400
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest endTimeMillis < 0, time conditions are invalid, find
 *                 faild.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0400, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0400 start");

    QueryPermissionUsedRequest request;
    request.endTimeMillis = -1;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, -1);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 0);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0400 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0500
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest beginTimeMillis = 0, endTimeMillis > 0, time conditions are valid, find
 *                 the data before endTimeMillis.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0500, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0500 start");

    QueryPermissionUsedRequest request;
    request.endTimeMillis = 250;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();
    EXPECT_EQ(state, 0);
    EXPECT_EQ(recordSize, 1);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0500 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0600
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest beginTimeMillis > 0, endTimeMillis = 0, time conditions are valid, find
 *                 the data after beginTimeMillis.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0600, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0600 start");

    QueryPermissionUsedRequest request;
    request.beginTimeMillis = 150;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();
    EXPECT_EQ(state, 0);
    EXPECT_EQ(recordSize, 2);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0600 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0700
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest beginTimeMillis = 0, endTimeMillis = 0, time conditions are valid, find
 *                 the data of all time.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0700, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0700 start");

    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();
    EXPECT_EQ(state, 0);
    EXPECT_EQ(recordSize, 2);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0700 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0800
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest flag = 0, result doesn't record detailed time.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0800, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0800 start");

    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    int detailFlag = false;
    for (auto bundle : result.bundlePermissionUsedRecords) {
        for (auto record : bundle.permissionUsedRecords) {
            if (record.accessRecordBg.size() > 0 || record.accessRecordFg.size() > 0 ||
                record.rejectRecordBg.size() > 0 || record.rejectRecordFg.size() > 0) {
                detailFlag = true;
            }
        }
    }
    EXPECT_FALSE(detailFlag);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0800 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_0900
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest flag = 1, result records detailed time.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_0900, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0900 start");

    QueryPermissionUsedRequest request;
    request.flag = 1;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    int detailFlag = false;
    for (auto bundle : result.bundlePermissionUsedRecords) {
        for (auto record : bundle.permissionUsedRecords) {
            if (record.accessRecordBg.size() > 0 || record.accessRecordFg.size() > 0 ||
                record.rejectRecordBg.size() > 0 || record.rejectRecordFg.size() > 0) {
                detailFlag = true;
            }
        }
    }
    EXPECT_TRUE(detailFlag);
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_0900 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1000
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest deviceLabel and permissionNames are not null, find out eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1000, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1000 start");

    QueryPermissionUsedRequest request;
    request.deviceLabel = deviceName_;
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);

    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].deviceLabel.c_str(), request.deviceLabel.c_str());
    EXPECT_EQ((int)result.bundlePermissionUsedRecords[0].permissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].permissionUsedRecords[0].permissionName.c_str(),
        request.permissionNames[0].c_str());

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1000 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1100
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest bundleName and permissionNames are not null, find out eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1100, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1100 start");

    QueryPermissionUsedRequest request;
    request.bundleName = "bundleName_0";
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].bundleName.c_str(), request.bundleName.c_str());
    EXPECT_EQ((int)result.bundlePermissionUsedRecords[0].permissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].permissionUsedRecords[0].permissionName.c_str(),
        request.permissionNames[0].c_str());

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1100 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1200
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest bundleName, deviceLabel and permissionNames are not null, find out
 *                 eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1200, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1200 start");

    QueryPermissionUsedRequest request;
    request.deviceLabel = deviceName_;
    request.bundleName = "bundleName_0";
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].bundleName.c_str(), request.bundleName.c_str());
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].deviceLabel.c_str(), request.deviceLabel.c_str());
    EXPECT_EQ((int)result.bundlePermissionUsedRecords[0].permissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].permissionUsedRecords[0].permissionName.c_str(),
        request.permissionNames[0].c_str());

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1200 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1300
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest bundleName and deviceLabel are not null, find out eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1300, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1300 start");

    QueryPermissionUsedRequest request;
    request.deviceLabel = deviceName_;
    request.bundleName = "bundleName_0";
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].bundleName.c_str(), request.bundleName.c_str());
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].deviceLabel.c_str(), request.deviceLabel.c_str());

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1300 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1400
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest deviceLabel is not null, find out eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1400, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1400 start");

    QueryPermissionUsedRequest request;
    request.deviceLabel = deviceName_;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].deviceLabel.c_str(), request.deviceLabel.c_str());

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1400 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1500
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest bundleName is not null, find out eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1500, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1500 start");

    QueryPermissionUsedRequest request;
    request.bundleName = "bundleName_0";
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    EXPECT_EQ(state, 0);
    EXPECT_EQ((int)result.bundlePermissionUsedRecords.size(), 1);
    EXPECT_STREQ(result.bundlePermissionUsedRecords[0].bundleName.c_str(), request.bundleName.c_str());

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1500 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1600
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest permissionNames are not null, find out eligible data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1600, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1600 start");

    QueryPermissionUsedRequest request;
    request.permissionNames.emplace_back("ohos.permission.LOCATION");
    request.permissionNames.emplace_back("ohos.permission.READ_CONTACTS");
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();
    EXPECT_EQ(state, 0);
    EXPECT_EQ(recordSize, 2);
    bool flag = false;
    auto bundle = result.bundlePermissionUsedRecords[0];
    if (bundle.permissionUsedRecords.size() > 0) {
        if (bundle.permissionUsedRecords[0].permissionName == request.permissionNames[0]) {
            EXPECT_STREQ(bundle.permissionUsedRecords[0].permissionName.c_str(), request.permissionNames[0].c_str());
            EXPECT_STREQ(bundle.permissionUsedRecords[1].permissionName.c_str(), request.permissionNames[1].c_str());
        } else {
            EXPECT_STREQ(bundle.permissionUsedRecords[0].permissionName.c_str(), request.permissionNames[1].c_str());
            EXPECT_STREQ(bundle.permissionUsedRecords[1].permissionName.c_str(), request.permissionNames[0].c_str());
        }
        flag = true;
    }

    EXPECT_TRUE(flag);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1600 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1700
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : QueryPermissionUsedRequest deviceLabel, bundleName and permissionNames are null, find out all
                   data.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1700, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1700 start");

    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    int recordSize = result.bundlePermissionUsedRecords[0].permissionUsedRecords.size();
    EXPECT_EQ(state, 0);
    EXPECT_EQ(recordSize, 2);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1700 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecords_1800
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : GetPermissionUsedRecords permormance test, expect to take less than 0.5ms.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecords_1800, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1800 start");

    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    PERMISSION_LOG_DEBUG(LABEL, "------------time start-------------");
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, result);
    PERMISSION_LOG_DEBUG(LABEL, "------------time end-------------");
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    EXPECT_EQ(state, 0);
    EXPECT_LT(fp_ms.count(), 0.5);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecords_1800 end");
}

/**
 * @tc.number    : DPMS_GetPermissionUsedRecordsAsync_0100
 * @tc.name      : GetPermissionUsedRecords
 * @tc.desc      : GetPermissionUsedRecords permormance test, expect to take less than 0.5ms.
 */
HWTEST_F(DistributedPermissionUsedRecordsTest, DPMS_GetPermissionUsedRecordsAsync_0100, TestSize.Level1)
{
    RemoveStorage();
    InitVisitorData("bundleName_0");
    InitRecordData();
    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecordsAsync_0100 start");
    auto timeFast = std::chrono::milliseconds(2000);
    OHOS::sptr<TestCallback> callback(new TestCallback());
    QueryPermissionUsedRequest request;
    QueryPermissionUsedResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    PERMISSION_LOG_DEBUG(LABEL, "------------time start-------------");
    int state = DistributedPermissionKit::GetPermissionUsedRecords(request, callback);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(timeFast);
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    EXPECT_EQ(state, 0);
    EXPECT_LT(fp_ms.count(), 0.5);

    PERMISSION_LOG_INFO(LABEL, "DistributedPermissionRecordsTest DPMS_GetPermissionUsedRecordsAsync_0100 end");
}
