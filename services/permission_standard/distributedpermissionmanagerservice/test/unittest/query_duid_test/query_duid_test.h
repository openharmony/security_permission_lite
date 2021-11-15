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
#ifndef QUERY_DUID_TEST_H
#define QUERY_DUID_TEST_H

#include "gtest/gtest.h"
#define private public
#include "distributed_permission_manager_service.h"

namespace OHOS {
pid_t IPCSkeleton::pid_ = 1;
pid_t IPCSkeleton::uid_ = 1;
std::string IPCSkeleton::localDeviceId_ = "1004";
std::string IPCSkeleton::deviceId_ = "";
namespace Security {
namespace Permission {
class QueryDuidTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

    std::shared_ptr<DistributedPermissionManagerService> GetInstance();

    int32_t validRuid_ = 1024;
    int32_t resultCode_ = -2;
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // QUERY_DUID_TEST_H