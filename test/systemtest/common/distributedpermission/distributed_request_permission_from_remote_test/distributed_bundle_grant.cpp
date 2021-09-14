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
#include "system_test_distributed_permission_util.h"
#include "distributed_permission_kit.h"
#include "bundle_info.h"
#include "permission_definition.h"
#include "parameter.h"
#include "permission_kit.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using namespace OHOS::STPermissionUtil;

using std::string;

namespace {
static const std::string SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A = "com.ohos.dpmsst.service.distributed.third.a";
}  // namespace

class DistributedBundleGrantTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DistributedBundleGrantTest::SetUpTestCase(void)
{}

void DistributedBundleGrantTest::TearDownTestCase(void)
{}

void DistributedBundleGrantTest::SetUp(void)
{}

void DistributedBundleGrantTest::TearDown(void)
{}

/**
 * @tc.number    : DPMS_Distributed_BundleGrant_0100
 * @tc.name      : BundleGrant
 * @tc.desc      : SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_REQUEST_APPA
 */
HWTEST_F(DistributedBundleGrantTest, DPMS_Distributed_BundleGrant_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DistributedBundleGrantTest DPMS_Distributed_BundleGrant_0100 start";
    // STDistibutePermissionUtil::Install(SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_REQUEST_APPA);
    std::string bundleName = SYSTEM_TEST_BUNDLE_NAME_DISTRIBUTED_THIRD_A;
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    // system grant
    OHOS::Security::Permission::PermissionDef permissionDef_Camera = {.permissionName = Constant::CAMERA,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_Readhealthdata = {
        .permissionName = Constant::READ_HEALTH_DATA,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_Location = {.permissionName = Constant::LOCATION,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_Microphone = {.permissionName = Constant::MICROPHONE,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};

    permDefList.emplace_back(permissionDef_Camera);
    permDefList.emplace_back(permissionDef_Readhealthdata);
    permDefList.emplace_back(permissionDef_Location);
    permDefList.emplace_back(permissionDef_Microphone);

    PermissionKit::AddDefPermissions(permDefList);
    // system grant
    std::vector<std::string> permList_system;
    permList_system.push_back(Constant::CAMERA);
    permList_system.push_back(Constant::READ_HEALTH_DATA);
    permList_system.push_back(Constant::LOCATION);
    permList_system.push_back(Constant::MICROPHONE);

    PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList_system);
    PermissionKit::GrantSystemGrantedPermission(bundleName, Constant::CAMERA);
    PermissionKit::GrantSystemGrantedPermission(bundleName, Constant::READ_HEALTH_DATA);
    PermissionKit::GrantSystemGrantedPermission(bundleName, Constant::LOCATION);
    PermissionKit::GrantSystemGrantedPermission(bundleName, Constant::MICROPHONE);

    GTEST_LOG_(INFO) << "DistributedBundleGrantTest DPMS_Distributed_BundleGrant_0100 end";
}
