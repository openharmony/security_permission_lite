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
#include <vector>
#include <mutex>
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "ipc_skeleton.h"
#include "time_util.h"
#include "distributed_permission_kit.h"
#include "sqlite_storage.h"
#include "bundle_info.h"

#include "device_info_repository.h"
#include "distributed_uid_allocator.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;
using namespace OHOS::AppExecFwk;
using namespace OHOS::DistributedHardware;
using std::string;

namespace {
string deviceId_;
int32_t rUidThird_ = 0;
}  // namespace

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "MonitorManagerTest"};
}
class SubjectNotifySyncPermissionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    int32_t AllocateDuidCheckTime(string &deviceId, int32_t rUid);
    int32_t QueryDuidCheckTime(string &deviceId, int32_t rUid);
    bool IsDuidCheckTime(int32_t uid);
};

void SubjectNotifySyncPermissionTest::SetUpTestCase()
{
    STDistibutePermissionUtil::Install("dpmsThirdIncludeUseByLocalApp");

    rUidThird_ = STDistibutePermissionUtil::GetUidByBundleName("com.third.hiworld.include_use_by_local_app");

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
    deviceId_ = deviceList[0].deviceId;
}

void SubjectNotifySyncPermissionTest::TearDownTestCase()
{}

void SubjectNotifySyncPermissionTest::SetUp()
{}

void SubjectNotifySyncPermissionTest::TearDown()
{}

/**
 * @tc.number    : DPMS_AllocateDuid_0100
 * @tc.name      : AllocateDuid
 * @tc.desc      : after AllocateDuid, QueryDuid.
 */
HWTEST_F(SubjectNotifySyncPermissionTest, SubjectNotifySyncPermissionTest_0100, TestSize.Level1)
{
    DistributedPermissionKit::AllocateDuid(deviceId_, 10000);

    auto timeFast = std::chrono::milliseconds(1000);
    std::this_thread::sleep_for(timeFast);

    DistributedPermissionKit::AllocateDuid(deviceId_, 10001);
}
