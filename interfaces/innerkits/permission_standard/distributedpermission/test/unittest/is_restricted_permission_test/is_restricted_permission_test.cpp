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

#include "is_restricted_permission_test.h"
using namespace testing::ext;
using namespace OHOS::Security::Permission;
using namespace std;
namespace OHOS {
}  // namespace OHOS
void IsRestrictedPermissionTest::SetUpTestCase()
{
    cout << "SetUpTestCase()" << endl;
}

void IsRestrictedPermissionTest::TearDownTestCase()
{
    cout << "TearDownTestCase()" << endl;
}
void IsRestrictedPermissionTest::SetUp()
{
    cout << "SetUp() is running" << endl;
}
void IsRestrictedPermissionTest::TearDown()
{
    cout << "TearDown()" << endl;
}

HWTEST_F(IsRestrictedPermissionTest, IsRestrictedPermissionTest02, Function | MediumTest | Level1)
{
    string ohosPermissionName = "ohos.permission.INTERNET";
    bool ret = DistributedPermissionKit::IsRestrictedPermission(ohosPermissionName);
    EXPECT_FALSE(ret);
}

HWTEST_F(IsRestrictedPermissionTest, IsRestrictedPermissionTest03, Function | MediumTest | Level1)
{
    string ohosPermissionName = "";
    bool ret = DistributedPermissionKit::IsRestrictedPermission(ohosPermissionName);
    EXPECT_FALSE(ret);
}
