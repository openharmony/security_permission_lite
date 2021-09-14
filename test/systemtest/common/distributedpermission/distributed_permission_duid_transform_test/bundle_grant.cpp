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
static const std::string permission_wifi = "ohos.permission.GET_WIFI_INFO";
static const std::string permission_network = "ohos.permission.GET_NETWORK_INFO";
static const std::string permission_location = "ohos.permission.LOCATION_IN_BACKGROUND";
static const std::string permission_camera = "ohos.permission.CAMERA";
static const std::string permission_microphone = "ohos.permission.MICROPHONE";
static const std::string permission_myself1 = "ohos.permission.MYPERMISSION_1";
static const std::string permission_myself2 = "ohos.permission.MYPERMISSION_2";
}  // namespace

class BundleGrant : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BundleGrant::SetUpTestCase(void)
{}

void BundleGrant::TearDownTestCase(void)
{}

void BundleGrant::SetUp(void)
{}

void BundleGrant::TearDown(void)
{}

/**
 * @tc.number    : DPMS_BundleGrant_0100
 * @tc.name      : BundleGrant
 * @tc.desc      : THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1
 */
HWTEST_F(BundleGrant, DPMS_BundleGrant_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0100 start";
    STDistibutePermissionUtil::Install(THIRD_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1);
    std::string bundleName = THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1;
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    // system grant
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_location = {.permissionName = permission_location,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    // user grant
    OHOS::Security::Permission::PermissionDef permissionDef_camera = {.permissionName = permission_camera,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_microphone = {.permissionName = permission_microphone,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    // custom grant
    OHOS::Security::Permission::PermissionDef permissionDef_myself1 = {.permissionName = permission_myself1,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_myself2 = {.permissionName = permission_myself2,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    permDefList.emplace_back(permissionDef_location);
    permDefList.emplace_back(permissionDef_camera);
    permDefList.emplace_back(permissionDef_microphone);
    permDefList.emplace_back(permissionDef_myself1);
    permDefList.emplace_back(permissionDef_myself2);
    PermissionKit::AddDefPermissions(permDefList);
    // system grant
    std::vector<std::string> permList_system;
    permList_system.push_back(permission_network);
    permList_system.push_back(permission_wifi);
    permList_system.push_back(permission_location);
    permList_system.push_back(permission_myself1);
    permList_system.push_back(permission_myself2);
    PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList_system);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_network);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_wifi);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_location);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_myself1);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_myself2);
    // user grant
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_camera);
    permList_user.push_back(permission_microphone);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_camera, 0);
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0100 end";
}

/**
 * @tc.number    : DPMS_BundleGrant_0200
 * @tc.name      : BundleGrant
 * @tc.desc      : SYSTEM_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1
 */
HWTEST_F(BundleGrant, DPMS_BundleGrant_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0200 start";
    std::string bundleName = SYSTEM_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1;
    std::vector<OHOS::Security::Permission::PermissionDef> permDefList;
    // system grant
    OHOS::Security::Permission::PermissionDef permissionDef_network = {.permissionName = permission_network,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_wifi = {.permissionName = permission_wifi,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_location = {.permissionName = permission_location,
        .bundleName = bundleName,
        .grantMode = 1,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    // user grant
    OHOS::Security::Permission::PermissionDef permissionDef_camera = {.permissionName = permission_camera,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    OHOS::Security::Permission::PermissionDef permissionDef_microphone = {.permissionName = permission_microphone,
        .bundleName = bundleName,
        .grantMode = 0,
        .availableScope = 1 << 0,
        .label = "test label",
        .labelId = 9527,
        .description = "test description",
        .descriptionId = 9528};
    permDefList.emplace_back(permissionDef_network);
    permDefList.emplace_back(permissionDef_wifi);
    permDefList.emplace_back(permissionDef_location);
    permDefList.emplace_back(permissionDef_camera);
    permDefList.emplace_back(permissionDef_microphone);
    PermissionKit::AddDefPermissions(permDefList);
    // system grant
    std::vector<std::string> permList_system;
    permList_system.push_back(permission_network);
    permList_system.push_back(permission_wifi);
    permList_system.push_back(permission_location);
    PermissionKit::AddSystemGrantedReqPermissions(bundleName, permList_system);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_network);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_wifi);
    PermissionKit::GrantSystemGrantedPermission(bundleName, permission_location);
    // user grant
    std::vector<std::string> permList_user;
    permList_user.push_back(permission_camera);
    permList_user.push_back(permission_microphone);
    PermissionKit::AddUserGrantedReqPermissions(bundleName, permList_user, 0);
    PermissionKit::GrantUserGrantedPermission(bundleName, permission_camera, 0);
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0200 end";
}
/**
 * @tc.number    : DPMS_BundleGrant_0300
 * @tc.name      : BundleGrant
 * @tc.desc      : THIRD_MORETHAN_MAXPERMISSION_HAP_NAME
 */
HWTEST_F(BundleGrant, DPMS_BundleGrant_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0300 start";
    STDistibutePermissionUtil::Install(THIRD_MORETHAN_MAXPERMISSION_HAP_NAME);
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0300 end";
}

/**
 * @tc.number    : DPMS_BundleGrant_0400
 * @tc.name      : BundleGrant
 * @tc.desc      : THIRD_EQ_MAXPERMISSION_HAP_NAME
 */
HWTEST_F(BundleGrant, DPMS_BundleGrant_0400, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0400 start";
    STDistibutePermissionUtil::Install(THIRD_EQ_MAXPERMISSION_HAP_NAME);
    GTEST_LOG_(INFO) << "BundleGrant DPMS_BundleGrant_0400 end";
}