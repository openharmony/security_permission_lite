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

#include "permission_kit_test.h"

#include "test_const.h"
#include "permission_kit.h"

using namespace testing::ext;
using namespace OHOS::Security::Permission;

void PermissionKitTest::SetUpTestCase()
{}

void PermissionKitTest::TearDownTestCase()
{}

void PermissionKitTest::SetUp()
{}

void PermissionKitTest::TearDown()
{}

/**
 * @tc.name: AddDefPermissions001
 * @tc.desc: Get permission definition info successfully after AddDefPermissions function has been invoked
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, AddDefPermissions001, TestSize.Level0)
{}

/**
 * @tc.name: RemoveDefPermissions001
 * @tc.desc: Cannot get permission definition info after RemoveDefPermissions has been invoked
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, RemoveDefPermissions001, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermission001
 * @tc.desc: Verify user granted permission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermission001, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermission002
 * @tc.desc: Verify system granted permission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermission002, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermission003
 * @tc.desc: Verify permission that has not been defined.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermission003, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermissionErrorUserGrant001
 * @tc.desc: Verify permission error that user granted but request system granted.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermissionErrorUserGrant001, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermissionErrorUserGrant002
 * @tc.desc: Verify permission error that user granted but grant system granted.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermissionErrorUserGrant002, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermissionErrorSystemGrant001
 * @tc.desc: Verify permission error that system granted but request user granted.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermissionErrorSystemGrant001, TestSize.Level0)
{}

/**
 * @tc.name: VerifyPermissionErrorSystemGrant002
 * @tc.desc: Verify permission error that system granted but grant user granted.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, VerifyPermissionErrorSystemGrant002, TestSize.Level0)
{}

/**
 * @tc.name: CanRequestPermission001
 * @tc.desc: For user granted permission and permission is granted, can request permission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, CanRequestPermission001, TestSize.Level0)
{}

/**
 * @tc.name: CanRequestPermission001
 * @tc.desc: For non user granted permission, can not request permission
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PermissionKitTest, CanRequestPermission002, TestSize.Level0)
{}