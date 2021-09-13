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
#include <cstdio>
#include <fstream>
#include <sstream>
#include <gtest/gtest.h>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>
#include "permission_log.h"

#include "stress_test_api.h"
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using std::string;

class StressTest : public testing::Test {
public:
    const static int THREAD_COUNT = 15;
    const static int EXE_STRESS_TIME = 20;
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "StressTest"};
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void StressFountion();
    static void TestFountion(int i);
    static void ExeAPI();
};

void StressTest::SetUpTestCase(void)
{}

void StressTest::TearDownTestCase(void)
{}

void StressTest::SetUp(void)
{}

void StressTest::TearDown(void)
{}

void StressTest::TestFountion(int i)
{
    std::string str = "test  thread" + std::to_string(i);
    PERMISSION_LOG_INFO(LABEL, "%{public}s", str.c_str());
    while (true) {
        ExeAPI();
        int sleepTime = 2;
        sleep(sleepTime);
    }
}

void StressTest::ExeAPI()
{
    std::vector<FnPtr> vec;
    DistibutePermissionStressTest::GetTestFunction(vec);
    for (auto fn : vec) {
        EXPECT_TRUE(fn());
    }
}

void StressTest::StressFountion()
{
    for (int i = 0; i < StressTest::THREAD_COUNT; i++) {
        std::thread newThread(TestFountion, i);
        newThread.detach();
    }
    sleep(EXE_STRESS_TIME);
}

/**
 * @tc.number    : StressTest
 * @tc.name      : StressTest
 * @tc.desc      : StressTest
 */
HWTEST_F(StressTest, StressTest_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StressTestTest  start";
    StressTest::StressFountion();
    GTEST_LOG_(INFO) << "StressTestTest  end";
}
