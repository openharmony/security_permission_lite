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
#ifndef PERMISSION_REMIND_INNER_TEST_H
#define PERMISSION_REMIND_INNER_TEST_H

#include <thread>
#include <functional>
#include <iostream>
#include "gtest/gtest.h"
#include "distributed_permission_kit.h"
#include "distributed_permission_manager_client.h"
#include "distributed_permission_death_recipient.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "on_using_permission_reminder_stub.h"
#include "permission_log.h"
#include "system_ability_definition.h"

class PermissionRemindInnerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    int CheckProxy();

    class TestCallback : public OHOS::Security::Permission::OnUsingPermissionReminderStub {
    public:
        TestCallback() = default;
        virtual ~TestCallback() = default;
        std::string permName = "ohos.permission.CAMERA";
        void StartUsingPermission(const OHOS::Security::Permission::PermissionReminderInfo &permReminderInfo);
        void StopUsingPermission(const OHOS::Security::Permission::PermissionReminderInfo &permReminderInfo);
    };
};
#endif  // PERMISSION_REMIND_INNER_TEST_H
