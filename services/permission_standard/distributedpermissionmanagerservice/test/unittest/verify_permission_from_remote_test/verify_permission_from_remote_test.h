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

#ifndef VERIFY_PERMISSION_FROM_REMOTE_TEST_H
#define VERIFY_PERMISSION_FROM_REMOTE_TEST_H
#include <gtest/gtest.h>
#define private public
#include "mock_bundle_mgr.h"
#include "mock_permission_mgr.h"
#include "distributed_permission_kit.h"
#include "distributed_permission_manager_service.h"
#include "base_remote_command.h"
#include "get_uid_permission_command.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ability_manager_interface.h"
namespace OHOS {
namespace Security {
namespace Permission {
class VerifyPermissionFromRemoteTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};
} // namespace Permission
} // namespace Security
} // namespace OHOS
#endif // VERIFY_PERMISSION_FROM_REMOTE_TEST_H
