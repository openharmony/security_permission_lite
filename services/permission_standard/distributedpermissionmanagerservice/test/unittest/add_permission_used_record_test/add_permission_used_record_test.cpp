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
#include <thread>
#include <functional>

#include "add_permission_used_record_test.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Security::Permission;

namespace OHOS {
pid_t IPCSkeleton::pid_ = 1;
pid_t IPCSkeleton::uid_ = 1;
std::string IPCSkeleton::localDeviceId_ = "1004";
std::string IPCSkeleton::deviceId_ = "";
}  // namespace OHOS

void AddPermissionUsedRecordTest::SetUpTestCase()
{}

void AddPermissionUsedRecordTest::TearDownTestCase()
{
    cout << "TearDownTestCase()" << endl;
}
void AddPermissionUsedRecordTest::SetUp()
{
    GenericValues null;
    DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_VISITOR, null);
    DataStorage::GetRealDataStorage().Remove(DataStorage::PERMISSION_RECORD, null);
    cout << "SetUp() is running" << endl;
}
void AddPermissionUsedRecordTest::TearDown()
{
    cout << "TearDown()" << endl;
}

void AddPermissionUsedRecordTest::SetPermissionNames(std::vector<std::string> &permissionNames)
{
    auto ite = Constant::NAME_TO_OP_CODE_MAP.begin();
    while (ite != Constant::NAME_TO_OP_CODE_MAP.end()) {
        permissionNames.emplace_back(ite->first);
        ite++;
    }
}

void AddPermissionUsedRecordTest::InitVisitorData()
{
    std::vector<GenericValues> visitor;
    GenericValues genericVisitor;

    genericVisitor.Put(FIELD_DEVICE_ID, "device_0");
    genericVisitor.Put(FIELD_DEVICE_NAME, "device_name_0");
    genericVisitor.Put(FIELD_BUNDLE_USER_ID, 0);
    genericVisitor.Put(FIELD_BUNDLE_NAME, "bundleName");
    genericVisitor.Put(FIELD_BUNDLE_LABEL, "bundleInfo.label");
    visitor.emplace_back(genericVisitor);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_VISITOR, visitor);
}

void AddPermissionUsedRecordTest::InitRecordData(int64_t timestamp)
{
    int opCode = 4;
    std::vector<GenericValues> visitor;
    DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitor);
    int visitorId = 0;
    if (visitor.size() > 0) {
        visitorId = visitor[0].GetInt(FIELD_ID);
    }

    std::vector<GenericValues> record;
    GenericValues genericRecordFore;

    genericRecordFore.Put(FIELD_TIMESTAMP, timestamp);
    genericRecordFore.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordFore.Put(FIELD_OP_CODE, opCode);
    genericRecordFore.Put(FIELD_IS_FOREGROUND, 1);
    genericRecordFore.Put(FIELD_ACCESS_COUNT, 1);
    genericRecordFore.Put(FIELD_REJECT_COUNT, 0);
    GenericValues genericRecordBack;

    genericRecordBack.Put(FIELD_TIMESTAMP, timestamp);
    genericRecordBack.Put(FIELD_VISITOR_ID, visitorId);
    genericRecordBack.Put(FIELD_OP_CODE, opCode);
    genericRecordBack.Put(FIELD_IS_FOREGROUND, 0);
    genericRecordBack.Put(FIELD_ACCESS_COUNT, 1);
    genericRecordBack.Put(FIELD_REJECT_COUNT, 0);
    record.emplace_back(genericRecordFore);
    record.emplace_back(genericRecordBack);
    DataStorage::GetRealDataStorage().Add(DataStorage::PERMISSION_RECORD, record);
}

/**
 * @tc.number: AddPermissionUsedRecord_0300
 * @tc.name: test add permission used record
 * @tc.desc: 1. permissionVisitor table already exists input data
 *           2. permissionRecord table already exists input data
 *           3. permissionRecord table needs to drop precision of timestamp
 */
HWTEST_F(AddPermissionUsedRecordTest, AddPermissionUsedRecord_0300, Function | MediumTest | Level1)
{
    InitVisitorData();
    InitRecordData(TimeUtil::GetTimestamp());
    string permissionName = "ohos.permission.READ_CONTACTS";
    string deviceId = "device_0";
    int32_t uid = 0;
    int32_t accessCount = 1;
    int32_t failCount = 0;

    visitorValues.clear();
    recordValues.clear();
    int visitorSize = 0;
    int recordSize = 0;
    int visitorResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorValues);
    int recordResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordValues);
    EXPECT_EQ(visitorResult, 0);
    EXPECT_EQ(recordResult, 0);
    visitorSize = visitorValues.size();
    recordSize = recordValues.size();
    std::string appidInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid, deviceId);
    managerService.AddPermissionsRecord(permissionName, appidInfo, accessCount, failCount);

    visitorValues.clear();
    recordValues.clear();
    visitorResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorValues);
    recordResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordValues);
    EXPECT_EQ(visitorResult, 0);
    EXPECT_EQ(recordResult, 0);
    EXPECT_EQ(visitorSize, (int)visitorValues.size());
    EXPECT_EQ(recordSize, (int)recordValues.size());
}

/**
 * @tc.number: AddPermissionUsedRecord_0400
 * @tc.name: test add permission used record
 * @tc.desc: 1. abnormal branch test
 *           2. can't get deviceInfo by deviceId
 */
HWTEST_F(AddPermissionUsedRecordTest, AddPermissionUsedRecord_0400, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.READ_CONTACTS";
    string deviceId = "device_3";
    int32_t uid = 0;
    int32_t accessCount = 0;
    int32_t failCount = 1;

    visitorValues.clear();
    recordValues.clear();
    int visitorSize = 0;
    int recordSize = 0;
    int visitorResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorValues);
    int recordResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordValues);
    EXPECT_EQ(visitorResult, 0);
    EXPECT_EQ(recordResult, 0);
    visitorSize = visitorValues.size();
    recordSize = recordValues.size();
    std::string appidInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid, deviceId);
    managerService.AddPermissionsRecord(permissionName, appidInfo, accessCount, failCount);
    visitorValues.clear();
    recordValues.clear();
    visitorResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_VISITOR, visitorValues);
    recordResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordValues);
    EXPECT_EQ(visitorResult, 0);
    EXPECT_EQ(recordResult, 0);
    EXPECT_EQ(visitorSize, (int)visitorValues.size());
    EXPECT_EQ(recordSize, (int)recordValues.size());
}

/**
 * @tc.number: AddPermissionUsedRecord_0500
 * @tc.name: test add permission used record
 * @tc.desc: 1. abnormal branch test
 *           2. can't thranslate permissionName into opCode
 */
HWTEST_F(AddPermissionUsedRecordTest, AddPermissionUsedRecord_0500, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.READ_NULL";
    string deviceId = "device_0";
    int32_t uid = 0;
    int32_t accessCount = 0;
    int32_t failCount = 1;

    recordValues.clear();
    int recordSize = 0;
    int recordResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordValues);
    EXPECT_EQ(recordResult, 0);
    recordSize = recordValues.size();
    std::string appidInfo = DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(0, uid, deviceId);
    managerService.AddPermissionsRecord(permissionName, appidInfo, accessCount, failCount);
    recordValues.clear();
    recordResult = DataStorage::GetRealDataStorage().Find(DataStorage::PERMISSION_RECORD, recordValues);
    EXPECT_EQ(recordResult, 0);

    EXPECT_EQ(recordSize, (int)recordValues.size());
}
