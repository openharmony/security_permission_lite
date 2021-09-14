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
int32_t rUidSystem_ = 0;
int32_t rUidMorethanMaxPermission_ = 0;
int32_t rUidEQMaxPermission_ = 0;
int32_t resultCode_ = -2;
}  // namespace

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "MonitorManagerTest"};
}
class DistributedPermissionDuidTransformTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DistributedPermissionDuidTransformTest::SetUpTestCase()
{
    STDistibutePermissionUtil::Install(THIRD_MORETHAN_MAXPERMISSION_HAP_NAME);
    STDistibutePermissionUtil::Install(THIRD_EQ_MAXPERMISSION_HAP_NAME);
    STDistibutePermissionUtil::Install(THIRD_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1);

    rUidSystem_ = STDistibutePermissionUtil::GetUidByBundleName(SYSTEM_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1);
    rUidThird_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1);
    rUidMorethanMaxPermission_ =
        STDistibutePermissionUtil::GetUidByBundleName(THIRD_MORETHAN_MAXPERMISSION_BUNDLE_NAME);
    rUidEQMaxPermission_ = STDistibutePermissionUtil::GetUidByBundleName(THIRD_EQ_MAXPERMISSION_BUNDLE_NAME);

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

void DistributedPermissionDuidTransformTest::TearDownTestCase()
{
    STDistibutePermissionUtil::Uninstall(THIRD_MORETHAN_MAXPERMISSION_BUNDLE_NAME);
    STDistibutePermissionUtil::Uninstall(THIRD_EQ_MAXPERMISSION_BUNDLE_NAME);
    STDistibutePermissionUtil::Uninstall(THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1);
}

void DistributedPermissionDuidTransformTest::SetUp()
{}

void DistributedPermissionDuidTransformTest::TearDown()
{}

/**
 * @tc.number    : DPMS_AllocateDuid_0100
 * @tc.name      : AllocateDuid
 * @tc.desc      : after AllocateDuid, QueryDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0100, TestSize.Level1)
{
    int32_t allocateDuid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    int32_t queryDuid = DistributedPermissionKit::QueryDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << allocateDuid;
    GTEST_LOG_(INFO) << queryDuid;
    EXPECT_EQ(allocateDuid, queryDuid);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0200
 * @tc.name      : AllocateDuid
 * @tc.desc      : after AllocateDuid, IsDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0200, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << duid;
    bool result = DistributedPermissionKit::IsDuid(duid);
    EXPECT_TRUE(result);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0400
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the permissions of the subject, the number of permissions
 * of the application package exceeds 1024, and the synchronization fails.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0400, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidMorethanMaxPermission_);
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0500
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the permissions of the subject, the number of permissions of
 * the application package is 1024, and the synchronization is successful.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0500, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidEQMaxPermission_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0600
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the system application permissions of the subject, the
 * permissions of "only local applications are allowed to use" are not synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0600, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidSystem_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0700
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the subject's three-party application permissions, the
 * permissions of "only local applications are allowed to use" are not synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0700, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0800
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the permissions of the subject, the three-party application,
 * user_grant permissions are not synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0800, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_0900
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the system application permissions of the subject, all
 * applications, system_grant permissions are synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_0900, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidSystem_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1000
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the subject's three-party application permissions, all
 * applications, system_grant permissions are synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1000, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1100
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the authority of the subject, except for the three parties,
 the
 * authorized user_grant authority is synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1100, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidSystem_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1200
 * @tc.name      : AllocateDuid
 * @tc.desc      : When the object actively synchronizes the permissions of the subject, the permissions defined by
 the
 * three parties are not synchronized.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1200, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_GT(duid, 0);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1300
 * @tc.name      : AllocateDuid
 * @tc.desc      : There is no corresponding DUID for deviceId and rUid. When the DUID is allocated, the device
 * goes offline.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1300, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1400
 * @tc.name      : AllocateDuid
 * @tc.desc      : Invalid (length>64) deviceId, AllocateDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1400, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(
        "0123456789012345678901234567890123456789012345678901234567890123456789", rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1500
 * @tc.name      : AllocateDuid
 * @tc.desc      : Invalid (rUid<0) rUid, AllocateDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1500, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, -1);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1600
 * @tc.name      : AllocateDuid
 * @tc.desc      : System rUid, AllocateDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1600, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, 1000);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, 12601000);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1700
 * @tc.name      : AllocateDuid
 * @tc.desc      : Root rUid,AllocateDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1700, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::AllocateDuid(deviceId_, 0);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, 12600000);
}

/**
 * @tc.number    : DPMS_AllocateDuid_1800
 * @tc.name      : AllocateDuid
 * @tc.desc      : There are corresponding DUIDs for deviceId and rUid, and AllocateDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_1800, TestSize.Level1)
{
    int32_t duid1 = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    int32_t duid2 = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    EXPECT_EQ(duid1, duid2);
}

/**
 * @tc.number    : DPMS_AllocateDuid_2100
 * @tc.name      : AllocateDuid
 * @tc.desc      : AllocateDuid interface performance.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_AllocateDuid_2100, TestSize.Level1)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    int32_t result = DistributedPermissionKit::AllocateDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << result;
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
}

/**
 * @tc.number    : DPMS_QueryDuid_0100
 * @tc.name      : QueryDuid
 * @tc.desc      : deviceId is "", QueryDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_QueryDuid_0100, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::QueryDuid("", rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_QueryDuid_0200
 * @tc.name      : QueryDuid
 * @tc.desc      : Invalid (length>64) deviceId, QueryDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_QueryDuid_0200, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::QueryDuid(
        "0123456789012345678901234567890123456789012345678901234567890123456789", rUidThird_);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_QueryDuid_0300
 * @tc.name      : QueryDuid
 * @tc.desc      : Invalid (rUid<0) rUid, QueryDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_QueryDuid_0300, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::QueryDuid(deviceId_, -1);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_QueryDuid_0400
 * @tc.name      : QueryDuid
 * @tc.desc      : There is no corresponding DUID for deviceId and rUid, QueryDuid.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_QueryDuid_0400, TestSize.Level1)
{
    int32_t duid = DistributedPermissionKit::QueryDuid(deviceId_, 1);
    GTEST_LOG_(INFO) << duid;
    EXPECT_EQ(duid, resultCode_);
}

/**
 * @tc.number    : DPMS_QueryDuid_0500
 * @tc.name      : QueryDuid
 * @tc.desc      : QueryDuid interface performance.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_QueryDuid_0500, TestSize.Level1)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    int32_t result = DistributedPermissionKit::QueryDuid(deviceId_, rUidThird_);
    GTEST_LOG_(INFO) << result;
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
}

/**
 * @tc.number    : DPMS_IsDuid_0100
 * @tc.name      : IsDuid
 * @tc.desc      : The uid that does not start with 126 and the subsequent numbers 10000~19999 is not DUID.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_IsDuid_0100, TestSize.Level1)
{
    bool result = DistributedPermissionKit::IsDuid(12410000);
    EXPECT_FALSE(result);
}

/**
 * @tc.number    : DPMS_IsDuid_0200
 * @tc.name      : IsDuid
 * @tc.desc      : Beginning with 126, the subsequent numbers are not uids ranging from 10000 to 19999, is DUID.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_IsDuid_0200, TestSize.Level1)
{
    bool result = DistributedPermissionKit::IsDuid(12600001);
    EXPECT_TRUE(result);
}

/**
 * @tc.number    : DPMS_IsDuid_0300
 * @tc.name      : IsDuid
 * @tc.desc      : The uid starting with 126 and the subsequent numbers 10000~19999 is the DUID.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_IsDuid_0300, TestSize.Level1)
{
    bool result = DistributedPermissionKit::IsDuid(12610001);
    EXPECT_TRUE(result);
}

/**
 * @tc.number    : DPMS_IsDuid_0400
 * @tc.name      : IsDuid
 * @tc.desc      : uid is 12610000.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_IsDuid_0400, TestSize.Level1)
{
    bool result = DistributedPermissionKit::IsDuid(12610000);
    EXPECT_TRUE(result);
}

/**
 * @tc.number    : DPMS_IsDuid_0500
 * @tc.name      : IsDuid
 * @tc.desc      : uid is 12619999.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_IsDuid_0500, TestSize.Level1)
{
    bool result = DistributedPermissionKit::IsDuid(12619999);
    EXPECT_TRUE(result);
}

/**
 * @tc.number    : DPMS_IsDuid_0600
 * @tc.name      : IsDuid
 * @tc.desc      : IsDuid interface performance.
 */
HWTEST_F(DistributedPermissionDuidTransformTest, DPMS_IsDuid_0600, TestSize.Level1)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    bool result = DistributedPermissionKit::IsDuid(12619999);
    GTEST_LOG_(INFO) << result;
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    EXPECT_LT(fp_ms.count(), STDistibutePermissionUtil::MAX_ELAPSED);
}
