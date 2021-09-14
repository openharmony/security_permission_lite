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
#include "permission_log.h"
#include "fuzz_test_manager.h"
#include "test_config_parser.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Security::Permission;
using std::string;

class DSFuzzTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static StressTestLevel stLevel_;
};

StressTestLevel DSFuzzTest::stLevel_{};
void DSFuzzTest::SetUpTestCase(void)
{
    TestConfigParser tcp;
    tcp.ParseFromFile4StressTest(STRESS_TEST_CONFIG_FILE_PATH, stLevel_);
    std::cout << "stress test level : "
              << "DPMS : " << stLevel_.DPMSLevel << std::endl;
}

void DSFuzzTest::TearDownTestCase(void)
{}

void DSFuzzTest::SetUp(void)
{}

void DSFuzzTest::TearDown(void)
{}

/**
 * @tc.number    : FuzzTest_0100
 * @tc.name      : DSFuzzTest
 * @tc.desc      : DSFuzzTest
 */
HWTEST_F(DSFuzzTest, FuzzTest_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "StartFuzzTest  start";
    FuzzTestManager::GetInstance()->StartFuzzTest();
    GTEST_LOG_(INFO) << "StartFuzzTest  end";
}
