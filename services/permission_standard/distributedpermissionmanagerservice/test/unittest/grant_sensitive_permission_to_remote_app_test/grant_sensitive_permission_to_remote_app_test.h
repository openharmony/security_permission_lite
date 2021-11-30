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

#ifndef GRANT_SENSITIVE_PERMISSION_TO_REMOTE_APP_TEST
#define GRANT_SENSITIVE_PERMISSION_TO_REMOTE_APP_TEST
#include <gtest/gtest.h>
#define private public
#include "distributed_permission_manager_service.h"
namespace OHOS {
namespace Security {
namespace Permission {
class GrantSensitivePermissionToRemoteAppTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // GRANT_SENSITIVE_PERMISSION_TO_REMOTE_APP_TEST
