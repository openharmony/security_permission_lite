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

#ifndef OHOS_SYSTEM_TEST_DISTRIBUTED_PERMISSION_STRESS_TEST_H
#define OHOS_SYSTEM_TEST_DISTRIBUTED_PERMISSION_STRESS_TEST_H
#include <string>
#include <unistd.h>
#include <vector>
#include "permission_log.h"
#include "system_test_distributed_permission_util.h"
#include "distributed_permission_kit.h"
#include "on_request_permissions_result_stub.h"
#include "sensitive_resource_switch_setting.h"
#include "resource_switch_cache.h"
#include "permission_kit.h"
#include "ability_manager_service.h"
#include "ability_manager_errors.h"
#include "app_mgr_service.h"
#include "module_test_dump_util.h"
#include "system_test_ability_util.h"
#include "sa_mgr_client.h"
#include "system_ability_definition.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "ability_lifecycle_executor.h"
#include "device_info_repository.h"
using std::string;
#include "sqlite_storage.h"
#include "time_util.h"
#include "on_permission_used_record_stub.h"
namespace OHOS {
namespace Security {
namespace Permission {

typedef bool (*FnPtr)();
class DistibutePermissionStressTest {
public:
    class AppEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
    public:
        explicit AppEventSubscriber(const OHOS::EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data) override;
    };
    static bool CheckDPermissionTest_01();
    static bool CheckDPermissionTest_02();

    static string GetAppIdInfo(int32_t pid, int32_t uid);
    static bool SubscribeEvent();
    static std::string remotePermissionPrepare();
    void OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data);
    static bool DPMS_VerifyPermissionFromRemote_2200();
    static bool DPMS_VerifySelfPermissionFromRemote_2200();
    static bool DPMS_CanRequestPermissionFromRemote_2100();
    static bool DPMS_GrantSensitivePermissionToRemoteApp_1300();

    static bool AddPermissionUsedRecordTest();
    static bool GetPermissionUsedRecordTest();
    static bool DeletePermissionUsedRecordTest();

    static bool AddPermissionUsedRecordTest_01();
    static bool AddPermissionUsedRecordTest_02();
    static bool AddPermissionUsedRecordTest_03();
    static bool AddPermissionUsedRecordTest_04();
    static bool AddPermissionUsedRecordTest_05();
    static bool AddPermissionUsedRecordTest_06();
    static bool GetPermissionUsedRecordTest_01();
    static bool GetPermissionUsedRecordTest_02();
    static bool GetPermissionUsedRecordTest_03();
    static bool GetPermissionUsedRecordTest_04();
    static bool GetPermissionUsedRecordTest_05();
    static bool GetPermissionUsedRecordTest_06();
    static bool GetPermissionUsedRecordTest_07();
    static bool GetPermissionUsedRecordTest_08();
    static bool GetPermissionUsedRecordTest_09();
    static bool GetPermissionUsedRecordTest_10();
    static bool GetPermissionUsedRecordTest_11();
    static bool GetPermissionUsedRecordTest_12();
    static bool GetPermissionUsedRecordTest_13();
    static bool GetPermissionUsedRecordTest_14();
    static bool GetPermissionUsedRecordAsyncTest();
    static void GetTestFunction(std::vector<FnPtr> &vector);
    static std::vector<std::string> eventList_;
    static STtools::Event event_;
    static sptr<OHOS::AppExecFwk::IAppMgr> appMs_;
    static sptr<OHOS::AAFwk::IAbilityManager> abilityMs_;
    static std::shared_ptr<AppEventSubscriber> subscriber_;

private:
};  // namespace DistributedHardware
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // OHOS_SYSTEM_TEST_DISTRIBUTED_PERMISSION_STRESS_TEST_H