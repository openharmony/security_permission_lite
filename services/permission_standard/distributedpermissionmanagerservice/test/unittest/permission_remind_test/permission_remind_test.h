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

#ifndef PERMISSION_REMIND_TEST_H
#define PERMISSION_REMIND_TEST_H

#include <cstdio>
#include <fstream>
#include <sstream>
#include <gtest/gtest.h>
#include "mock_bundle_mgr.h"
#include "mock_permission_mgr.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ability_manager_interface.h"
#include "distributed_permission_kit.h"
#include "distributed_permission_manager_service.h"
#include "on_using_permission_reminder_stub.h"
#include "on_using_permission_reminder_proxy.h"
#include "permission_reminder_info.h"

class PermissionRemindTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static const int32_t uid_normal_1_ = 12612345;
    static const int32_t uid_normal_2_ = 12512345;
    static const int32_t uid_normal_3_ = 12600000;
    static const int32_t uid_normal_4_ = 12500000;
    int32_t pid = 101;
    int32_t uid = uid_normal_1_;
    std::string deviceId = "ohos.deviceId.test";
    std::string deviceName = "ohos.deviceLabel.test";
    std::shared_ptr<OHOS::Security::Permission::DistributedPermissionManagerService> dpms_;

    class TestCallback : public OHOS::Security::Permission::OnUsingPermissionReminderStub {
    public:
        TestCallback() = default;
        virtual ~TestCallback() = default;
        std::string permName = "ohos.permission.CAMERA";
        void StartUsingPermission(const OHOS::Security::Permission::PermissionReminderInfo &permReminderInfo);
        void StopUsingPermission(const OHOS::Security::Permission::PermissionReminderInfo &permReminderInfo);
    };
};

#endif // PERMISSION_REMIND_TEST_H