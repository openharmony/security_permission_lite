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

#include "is_duid_test.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS::Security::Permission;

void IsDuidTest::SetUpTestCase()
{}
void IsDuidTest::TearDownTestCase()
{}
void IsDuidTest::SetUp()
{}
void IsDuidTest::TearDown()
{}

/**
 * @tc.number: IsDuid_0001
 * @tc.name: IsDuid
 * @tc.desc: IsDuid is valid.
 */
HWTEST_F(IsDuidTest, IsDuid_0001, Function | MediumTest | Level1)
{
    bool flag = DistributedPermissionKit::IsDuid(12610001);
    GTEST_LOG_(INFO) << flag;
    EXPECT_TRUE(flag);
}

/**
 * @tc.number: IsDuid_0002
 * @tc.name: IsDuid
 * @tc.desc: IsDuid is invalid.
 */
HWTEST_F(IsDuidTest, IsDuid_0002, Function | MediumTest | Level1)
{
    bool flag = DistributedPermissionKit::IsDuid(1);
    GTEST_LOG_(INFO) << flag;
    EXPECT_FALSE(flag);
}
