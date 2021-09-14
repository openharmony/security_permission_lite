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
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "ipc_skeleton.h"
#include "time_util.h"

#define private public
#include "distributed_permission_manager_service.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::AppExecFwk;
using std::string;

class NotifyDeletePermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NotifyDeletePermissionTest::SetUpTestCase(void)
{}

void NotifyDeletePermissionTest::TearDownTestCase(void)
{}

void NotifyDeletePermissionTest::SetUp(void)
{}

void NotifyDeletePermissionTest::TearDown(void)
{}

HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0100 start";
    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    ObjectDevicePermissionManager::GetInstance().Clear();
    std::string deviceId = "deviceId";
    int32_t uid = 100000;
    ObjectDevicePermissionRepository::GetInstance().PutDeviceIdUidPair(deviceId, uid);

    int32_t userId = 1;
    int32_t result = ObjectDevicePermissionManager::GetInstance().RemoveNotifyPermissionMonitorUserId(userId);

    EXPECT_EQ(result, -3);

    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0100 end";
}

HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0200 start";
    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    ObjectDevicePermissionManager::GetInstance().Clear();
    std::string deviceId = "1234567890123456789012345678901234567890123456789012345678901234567890";
    int32_t uid = 100000;
    ObjectDevicePermissionRepository::GetInstance().PutDeviceIdUidPair(deviceId, uid);

    int32_t userId = 1;
    int32_t result = ObjectDevicePermissionManager::GetInstance().RemoveNotifyPermissionMonitorUserId(userId);

    EXPECT_EQ(result, -3);

    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0200 end";
}

HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0300 start";
    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    ObjectDevicePermissionManager::GetInstance().Clear();
    std::string deviceId = "1234567890123456789012345678901234567890123456789012345678901234";
    int32_t uid = 100000;
    ObjectDevicePermissionRepository::GetInstance().PutDeviceIdUidPair(deviceId, uid);

    int32_t userId = 1;
    int32_t result = ObjectDevicePermissionManager::GetInstance().RemoveNotifyPermissionMonitorUserId(userId);

    EXPECT_EQ(result, 0);

    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0300 end";
}

HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0400 start";
    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    // 主体设备的DeviceID非空且小于64，存在srcDeviceId和uid的权限信息，删除同步的权限信息时成功

    ObjectDevicePermissionManager::GetInstance().Clear();
    std::string deviceId = "deviceId";
    int32_t uid = 100000;
    ObjectDevicePermissionRepository::GetInstance().PutDeviceIdUidPair(deviceId, uid);

    int32_t userId = 1;
    int32_t result = ObjectDevicePermissionManager::GetInstance().RemoveNotifyPermissionMonitorUserId(userId);

    EXPECT_EQ(result, 0);

    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0400 end";
}

HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0500 start";
    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    // 不存在srcDeviceId和uid的权限信息，删除同步的权限信息时返回-2

    int32_t userId = 1;
    int32_t result = ObjectDevicePermissionManager::GetInstance().RemoveNotifyPermissionMonitorUserId(userId);

    EXPECT_EQ(result, -2);

    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0500 end";
}

// 两台3516能够成功组网。
HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0600 start";

    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    // .删除主体应用。
    GTEST_LOG_(INFO) << "Uninstall DPThirdBundle start.";
    STDistibutePermissionUtil::Uninstall(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Uninstall DPThirdBundle end.";

    // 判断API的返回值是否与预期结果一致。
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0600 end";
}

// 三台3516能够成功组网
HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0700 start";

    GTEST_LOG_(INFO) << "Install DPThirdBundle start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle end.";

    // .删除主体应用。
    GTEST_LOG_(INFO) << "Uninstall DPThirdBundle start.";
    STDistibutePermissionUtil::Uninstall(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Uninstall DPThirdBundle end.";

    // 两台设备上的此应用权限信息及DUID被删除
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0700 end";
}

// 两台3516能够成功组网。
HWTEST_F(NotifyDeletePermissionTest, DPMS_NotifyUidDelete_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0800 start";

    // 应用1 install
    GTEST_LOG_(INFO) << "Install DPThirdBundle1 start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle1 end.";

    // 应用2 install
    GTEST_LOG_(INFO) << "Install DPThirdBundle2 start.";
    STDistibutePermissionUtil::Install(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Install DPThirdBundle2 end.";

    // 应用1 卸载
    GTEST_LOG_(INFO) << "Uninstall DPThirdBundle start.";
    STDistibutePermissionUtil::Uninstall(THIRD_HAP_NAME);
    GTEST_LOG_(INFO) << "Uninstall DPThirdBundle end.";

    // 只删除卸载的应用权限信息及DUID
    GTEST_LOG_(INFO) << "NotifyDeletePermissionTest DPMS_NotifyUidDelete_0700 end";
}