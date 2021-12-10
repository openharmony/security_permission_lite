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

#include "wait_duid_ready_test.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Security::Permission;

void WaitDuidReadyTest::SetUpTestCase()
{}
void WaitDuidReadyTest::TearDownTestCase()
{}
void WaitDuidReadyTest::SetUp()
{}
void WaitDuidReadyTest::TearDown()
{
    DistributedUidAllocator::GetInstance().Clear();
    DeviceInfoRepository::GetInstance().Clear();
}

std::unique_ptr<DistributedPermissionManagerService> WaitDuidReadyTest::GetInstance()
{
    return std::make_unique<DistributedPermissionManagerService>();
}

/**
 * @tc.number: WaitDuidReady_0001
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is invalid.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0001, Function | MediumTest | Level1)
{
    int32_t duid = GetInstance()->WaitDuidReady("", validRuid_, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, INVALID_DEVICE_ID_);
}

/**
 * @tc.number: WaitDuidReady_0002
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           ruid is invalid.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0002, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "universallyUniqueId", "uniqueDisabilityId",
        "deviceName", "deviceType");

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", -1, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, INVALID_RUID_);
}

/**
 * @tc.number: WaitDuidReady_0003
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           timeout < 0.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0003, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "universallyUniqueId", "uniqueDisabilityId",
        "deviceName", "deviceType");

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", validRuid_, -1);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, FAILURE_);
}

/**
 * @tc.number: WaitDuidReady_0004
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           timeout > MAX_WAITING_TIME(2000).
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0004, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "universallyUniqueId", "uniqueDisabilityId",
        "deviceName", "deviceType");

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", validRuid_, 2001);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, FAILURE_);
}

/**
 * @tc.number: WaitDuidReady_0005
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           peer device version is before 11.0, transfer to allocateDuid,
 *           (networkId == universallyUniqueId),
 *           duid is existing,
 *           duid is invalid.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0005, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "networkId", "uniqueDisabilityId", "deviceName",
        "deviceType");

    std::string key = DistributedUidAllocator::GetInstance().Hash("uniqueDisabilityId", validRuid_);
    DistributedUidEntity distributedUidEntity;
    distributedUidEntity.distributedUid = 12600001;
    DistributedUidAllocator::GetInstance().distributedUidMapByKey_.insert(
        std::pair<std::string, DistributedUidEntity>(key, distributedUidEntity));

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", validRuid_, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, INVALID_DISTRIBUTED_UID_);
}

/**
 * @tc.number: WaitDuidReady_0006
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           peer device version is before 11.0, transfer to allocateDuid,
 *           (networkId == universallyUniqueId),
 *           duid is existing,
 *           duid is valid,
 *           duid is existing in distributedPermissionMapping_.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0006, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "networkId", "uniqueDisabilityId", "deviceName",
        "deviceType");

    std::string key = DistributedUidAllocator::GetInstance().Hash("uniqueDisabilityId", validRuid_);
    DistributedUidEntity distributedUidEntity;
    distributedUidEntity.distributedUid = 12610001;
    DistributedUidAllocator::GetInstance().distributedUidMapByKey_.insert(
        std::pair<std::string, DistributedUidEntity>(key, distributedUidEntity));

    UidBundleBo uidBundleBo;
    SubjectDevicePermissionManager::GetInstance().distributedPermissionMapping_.insert(
        std::pair<int32_t, UidBundleBo>(12610001, uidBundleBo));

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", validRuid_, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, 12610001);
}

/**
 * @tc.number: WaitDuidReady_0007
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           duid is existing,
 *           duid is valid,
 *           duid is existing in distributedPermissionMapping_.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0007, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "universallyUniqueId", "uniqueDisabilityId",
        "deviceName", "deviceType");

    std::string key = DistributedUidAllocator::GetInstance().Hash("uniqueDisabilityId", validRuid_);
    DistributedUidEntity distributedUidEntity;
    distributedUidEntity.distributedUid = 12610001;
    DistributedUidAllocator::GetInstance().distributedUidMapByKey_.insert(
        std::pair<std::string, DistributedUidEntity>(key, distributedUidEntity));

    UidBundleBo uidBundleBo;
    SubjectDevicePermissionManager::GetInstance().distributedPermissionMapping_.insert(
        std::pair<int32_t, UidBundleBo>(12610001, uidBundleBo));

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", validRuid_, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, 12610001);
}

/**
 * @tc.number: WaitDuidReady_0008
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           duid is not existing,
 *           time out.
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0008, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "universallyUniqueId", "uniqueDisabilityId",
        "deviceName", "deviceType");

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityId", validRuid_, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, WAIT_DISTRIBUTED_UID_TIME_OUT_);
}

/**
 * @tc.number: WaitDuidReady_0009
 * @tc.name: WaitDuidReady
 * @tc.desc: deviceId is existing in deviceIdMap,
 *           peer device version is before 11.0, transfer to allocateDuid,
 *           (networkId == universallyUniqueId),
 *           duid is not existing,
 *           .
 */
HWTEST_F(WaitDuidReadyTest, WaitDuidReady_0009, Function | MediumTest | Level1)
{
    DeviceInfoRepository::GetInstance().SaveDeviceInfo("networkId", "networkId",
        "uniqueDisabilityIdCanNotGetPackgeForUid", "deviceName", "deviceType");

    int32_t duid = GetInstance()->WaitDuidReady("uniqueDisabilityIdCanNotGetPackgeForUid", validRuid_, validTime_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, CANNOT_GET_PACKAGE_FOR_UID_);
}
