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
#ifndef WAIT_DUID_READY_TEST_H
#define WAIT_DUID_READY_TEST_H

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
class WaitDuidReadyTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

    std::unique_ptr<DistributedPermissionManagerService> GetInstance();

    int32_t validRuid_ = 1024;
    int32_t validTime_ = 5;
    int32_t INVALID_DEVICE_ID_ = Constant::INVALID_DEVICE_ID;
    int32_t INVALID_RUID_ = Constant::INVALID_RUID;
    int32_t FAILURE_ = Constant::FAILURE;
    int32_t INVALID_DISTRIBUTED_UID_ = Constant::INVALID_DISTRIBUTED_UID;
    int32_t WAIT_DISTRIBUTED_UID_TIME_OUT_ = Constant::WAIT_DISTRIBUTED_UID_TIME_OUT;
    int32_t CANNOT_GET_PACKAGE_FOR_UID_ = Constant::CANNOT_GET_PACKAGE_FOR_UID;
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // WAIT_DUID_READY_TEST_H
