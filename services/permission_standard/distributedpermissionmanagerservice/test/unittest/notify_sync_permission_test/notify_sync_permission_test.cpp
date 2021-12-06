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

#include "gtest/gtest.h"
#define private public
#include "distributed_permission_manager_service.h"
#include "mock_bundle_mgr.h"
#include "mock_permission_mgr.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ability_manager_interface.h"

using namespace std;
using namespace testing::ext;
namespace OHOS {
pid_t IPCSkeleton::pid_ = 1;
pid_t IPCSkeleton::uid_ = 1;
std::string IPCSkeleton::localDeviceId_ = "1004";
std::string IPCSkeleton::deviceId_ = "";

namespace Security {
namespace Permission {
class NotifySyncPermissionTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {
        cout << "TearDownTestCase()" << endl;
    }
    void SetUp()
    {
        cout << "SetUp() is running" << endl;
    }
    void TearDown()
    {
        cout << "TearDown()" << endl;
        DistributedUidAllocator::GetInstance().Clear();
        DeviceInfoRepository::GetInstance().Clear();
    }
    std::unique_ptr<DistributedPermissionManagerService> GetInstance();
};
std::unique_ptr<DistributedPermissionManagerService> NotifySyncPermissionTest::GetInstance()
{
    return std::make_unique<DistributedPermissionManagerService>();
}
HWTEST_F(NotifySyncPermissionTest, NotifySyncPermission_0001, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "universallyUniqueId", "uniqueDisabilityIdSuccess",
        "deviceName", "deviceType");

    std::unique_ptr<DistributedPermissionManagerService> class_ =
        std::make_unique<DistributedPermissionManagerService>();

    std::string nodeId = "networkId";
    int uid = 1;
    std::string packageName = "packageName";

    int32_t result = class_->NotifySyncPermission(nodeId, uid, packageName);
    EXPECT_EQ(result, 0);
}
} // namespace Permission
} // namespace Security
} // namespace OHOS
