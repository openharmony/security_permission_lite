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

#ifndef GET_PERMISSION_USED_RECORD_ASYNCH_TEST_H
#define GET_PERMISSION_USED_RECORD_ASYNCH_TEST_H

#include <gtest/gtest.h>

#include "sqlite_storage.h"
#include "distributed_permission_manager_service.h"
#include "on_permission_used_record_stub.h"

namespace OHOS {
namespace Security {
namespace Permission {
class GetPermissionUsedRecordsAsynchTest : public testing::Test {
public:
    DistributedPermissionManagerService managerService;
    std::shared_ptr<DistributedPermissionManagerService> service;
    static void InitVisitorData();
    static void InitRecordData(int64_t timestamp);

    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

    class TestCallback : public OnPermissionUsedRecordStub {
    public:
        TestCallback() = default;
        virtual ~TestCallback() = default;

        void OnQueried(ErrCode code, QueryPermissionUsedResult &result)
        {
            int successCode = 200;
            EXPECT_EQ(code, successCode);
        }
    };
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // GET_PERMISSION_USED_RECORD_ASYNCH_TEST_H
