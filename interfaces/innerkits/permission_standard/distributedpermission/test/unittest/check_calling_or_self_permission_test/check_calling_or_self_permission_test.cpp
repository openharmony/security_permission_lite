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

#include "check_calling_or_self_permission_test.h"

using namespace std;
using namespace OHOS::Security::Permission;
using namespace testing::ext;

namespace OHOS {
namespace Security {
namespace Permission {
void CheckCallingOrSelfPermissionTest::SetUpTestCase()
{
    cout << "SetUpTestCase()" << endl;
}
void CheckCallingOrSelfPermissionTest::TearDownTestCase()
{
    cout << "TearDownTestCase()" << endl;
}
void CheckCallingOrSelfPermissionTest::SetUp()
{
    cout << "SetUp() is running" << endl;
}
void CheckCallingOrSelfPermissionTest::TearDown()
{
    cout << "TearDown()" << endl;
}

HWTEST_F(CheckCallingOrSelfPermissionTest, CheckCallingOrSelfPermission01, Function | MediumTest | Level1)
{
    string permissionName = "";
    int result = Constant::PERMISSION_DENIED;
    int ret = DistributedPermissionKit::CheckCallingOrSelfPermission(permissionName);
    EXPECT_EQ(result, ret);
}
HWTEST_F(CheckCallingOrSelfPermissionTest, CheckCallingOrSelfPermission02, Function | MediumTest | Level1)
{
    string permissionName = "ohos.permission.INTERNET";
    int result = Constant::PERMISSION_GRANTED;
    int ret = DistributedPermissionKit::CheckCallingOrSelfPermission(permissionName);
    EXPECT_EQ(result, ret);
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS