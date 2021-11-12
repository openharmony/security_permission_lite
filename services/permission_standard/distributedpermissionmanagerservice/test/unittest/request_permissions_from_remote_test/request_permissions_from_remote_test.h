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

#ifndef REQUEST_PERMISSIONS_FROM_REMOTE_TEST_H
#define REQUEST_PERMISSIONS_FROM_REMOTE_TEST_H
#include <gtest/gtest.h>
#define private public
#include <iostream>
#include "distributed_permission_manager_service.h"
#include "on_request_permissions_result_stub.h"
#include "mock_bundle_mgr.h"
#include "mock_permission_mgr.h"
#include "base_remote_command.h"
#include "get_uid_permission_command.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ability_manager_interface.h"
#include "permission_log.h"
#include "request_remote_permission.h"
namespace OHOS {
namespace Security {
namespace Permission {
class RequestPermissionsFromRemoteTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

public:
    class TestCallbackA : public OnRequestPermissionsResultStub {
    public:
        TestCallbackA() = default;
        virtual ~TestCallbackA() = default;

        void OnResult(const std::string nodeId, std::vector<std::string> permissions, std::vector<int32_t> grantResults)
        {
            onResult_--;
            std::cout << "nodeId:" << nodeId << ",permissions:" << permissions.size() << ",grantResults"
                      << grantResults.size() << std::endl;
            std::cout << "RequestPermissionsFromRemoteTest :OnResult  is run!!!";
        }

        void OnCancel(const std::string nodeId, std::vector<std::string> permissions)
        {
            onCancel_--;
            std::cout << "RequestPermissionsFromRemoteTest :OnCancel  is run!!!";
        }
        void OnTimeOut(const std::string nodeId, std::vector<std::string> permissions)
        {
            onTimeOut_--;
            std::cout << "RequestPermissionsFromRemoteTest :OnTimeOut  is run!!!";
        }
        int onResult_ = 100;
        int onCancel_ = 100;
        int onTimeOut_ = 100;
    };
    std::shared_ptr<DistributedPermissionManagerService> service;
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // REQUEST_PERMISSIONS_FROM_REMOTE_TEST_H
