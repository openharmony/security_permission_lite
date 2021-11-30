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

#ifndef SENSITIVE_SWITCH_TEST_H
#define SENSITIVE_SWITCH_TEST_H
#include <gtest/gtest.h>
#include "constant.h"
#define private public
#include "distributed_permission_manager_service.h"
#include "resource_switch.h"
#include "resource_switch_cache.h"
#include "resource_switch_local.h"
#include "resource_switch_remote.h"
#include "sensitive_resource_switch_setting.h"
#include "ipc_skeleton.h"
namespace OHOS {
namespace Security {
namespace Permission {
class SensitiveSwitchTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // SENSITIVE_SWITCH_TEST_H
