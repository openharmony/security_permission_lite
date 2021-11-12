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

#include "check_permission_test.h"

using namespace std;
using namespace OHOS::Security::Permission;
using namespace testing::ext;
namespace OHOS {
pid_t IPCSkeleton::pid_ = 1;
pid_t IPCSkeleton::uid_ = 1;
std::string IPCSkeleton::localDeviceId_ = "1004";
std::string IPCSkeleton::deviceId_ = "";

namespace Security {
namespace Permission {
void CheckPermissionTest::SetUpTestCase()
{
    cout << "SetUpTestCase()" << endl;
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    OHOS::sptr<OHOS::IRemoteObject> permissionObject = new PermissionManagerService();
    auto sysMgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysMgr == NULL) {
        GTEST_LOG_(ERROR) << "fail to get ISystemAbilityManager";
        return;
    }
    sysMgr->AddSystemAbility(Constant::ServiceId::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    sysMgr->AddSystemAbility(Constant::ServiceId::SUBSYS_SECURITY_PERMISSION_SYS_SERVICE_ID, permissionObject);
}

void CheckPermissionTest::TearDownTestCase()
{
    cout << "TearDownTestCase()" << endl;
}
void CheckPermissionTest::SetUp()
{
    cout << "SetUp() is running" << endl;
}
void CheckPermissionTest::TearDown()
{
    cout << "TearDown()" << endl;
}

HWTEST_F(CheckPermissionTest, CheckPermission01, Function | MediumTest | Level1)
{
    string permissionName = "";
    int result = Constant::PERMISSION_DENIED;
    string appIdInfo = "{\"deviceID\":\"1001\",\"pid\":1,\"uid\":1}";
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission02, Function | MediumTest | Level1)
{
    string permissionName = "";
    string appIdInfo = "{\"deviceID\":\"1001\",\"pid\":1,\"uid\":-1}";
    int result = Constant::PERMISSION_DENIED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission03, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    string appIdInfo = "{\"deviceID\":\"\",\"pid\":1,\"uid\":1}";
    int result = Constant::PERMISSION_GRANTED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission04, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    string appIdInfo = "{\"deviceID\":\" \",\"pid\":1,\"uid\":-1}";
    int result = Constant::PERMISSION_DENIED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission05, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    string appIdInfo =
        "{\"deviceID\":\"12345678123456781234567812345678123456781234567812345678123456789\",\"pid\":1,\"uid\":-1}";
    int result = Constant::PERMISSION_DENIED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission06, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    string appIdInfo = "{\"deviceID\":\"1001\",\"pid\":1,\"uid\":1}";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("1000", "1002", "1003", "deviceName", "deviceType");
    int result = Constant::PERMISSION_DENIED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission07, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    string nodeId = "1001";
    string deviceId = "1004";
    string appIdInfo = "{\"deviceID\":\"1004\",\"pid\":1,\"uid\":12601000}";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(nodeId, "1002", deviceId, "deviceName", "deviceType");
    int result = Constant::PERMISSION_GRANTED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}

HWTEST_F(CheckPermissionTest, CheckPermission08, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    string nodeId = "1001";
    string appIdInfo = "{\"deviceID\":\"1001\",\"pid\":1,\"uid\":1}";
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(nodeId, "1002", "1003", "deviceName", "deviceType");
    std::string key = DistributedUidAllocator::GetInstance().Hash("1003", 1);
    DistributedUidEntity distributedUidEntity;
    distributedUidEntity.distributedUid = 12610001;
    DistributedUidAllocator::GetInstance().distributedUidMapByKey_.insert(
        std::pair<std::string, DistributedUidEntity>(key, distributedUidEntity));
    int result = Constant::PERMISSION_DENIED;
    int ret = DistributedPermissionManagerService().CheckPermission(permissionName, appIdInfo);
    EXPECT_EQ(result, ret);
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS