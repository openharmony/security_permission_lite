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

#include <thread>
#include "check_self_permission_ability_a1.h"
#include "app_log_wrapper.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "distributed_permission_kit.h"

using namespace OHOS::EventFwk;
using namespace OHOS::Security::Permission;
using MAP_STR_STR = std::map<std::string, std::string>;
namespace OHOS {
namespace AppExecFwk {
using AbilityConnectionProxy = OHOS::AAFwk::AbilityConnectionProxy;
int CheckSelfPermissionAbilityA1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
int hapRet = -1;
std::map<std::string, CheckSelfPermissionAbilityA1::func> CheckSelfPermissionAbilityA1::funcMap_ = {
    {"DPMS_CheckSelfPermission_0100", &CheckSelfPermissionAbilityA1::CheckSelfPermission0100},
    {"DPMS_CheckSelfPermission_0400", &CheckSelfPermissionAbilityA1::CheckSelfPermission0400},
    {"DPMS_CheckCallingPermission_0600", &CheckSelfPermissionAbilityA1::CheckCallingPermission0600},
    {"DPMS_CheckCallingOrSelfPermission_0300", &CheckSelfPermissionAbilityA1::CheckCallingOrSelfPermission0300},
    {"DPMS_CheckCallingOrSelfPermission_0400", &CheckSelfPermissionAbilityA1::CheckCallingOrSelfPermission0400},
    {"DPMS_CheckCallerPermission_0100", &CheckSelfPermissionAbilityA1::CheckCallerPermission0100},
    {"StopSelfAbility", &CheckSelfPermissionAbilityA1::StopSelfAbility},
};

CheckSelfPermissionAbilityA1::~CheckSelfPermissionAbilityA1()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

std::vector<std::string> CheckSelfPermissionAbilityA1::Split(std::string str, const std::string &token)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::Split");

    std::vector<std::string> splitString;
    while (str.size()) {
        size_t index = str.find(token);
        if (index != std::string::npos) {
            splitString.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0) {
                splitString.push_back(str);
            }
        } else {
            splitString.push_back(str);
            str = "";
        }
    }
    return splitString;
}

void CheckSelfPermissionAbilityA1::StopSelfAbility()
{
    APP_LOGI("CheckSelfPermissionAbilityA1::StopSelfAbility");

    TerminateAbility();
}

void CheckSelfPermissionAbilityA1::OnStart(const Want &want)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);
    PublishEvent(APP_A1_RESP_EVENT_NAME, 0, "OnStart");
    SubscribeEvent();
}
void CheckSelfPermissionAbilityA1::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
}
void CheckSelfPermissionAbilityA1::OnNewWant(const Want &want)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void CheckSelfPermissionAbilityA1::OnStop()
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnStop");

    Ability::OnStop();
}
void CheckSelfPermissionAbilityA1::OnActive()
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnActive");

    Ability::OnActive();
}
void CheckSelfPermissionAbilityA1::OnInactive()
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnInactive");

    Ability::OnInactive();
}
void CheckSelfPermissionAbilityA1::OnBackground()
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnBackground");

    Ability::OnBackground();
}

void CheckSelfPermissionAbilityA1::Clear()
{
    permName = "";
}
void CheckSelfPermissionAbilityA1::GetWantInfo(const Want &want)
{
    Want mWant(want);
    permName = mWant.GetStringParam("permName");
    CheckSelfPermissionAbilityA1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
bool CheckSelfPermissionAbilityA1::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::PublishEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        code,
        data.c_str());

    Want want;
    want.SetAction(eventName);
    want.SetParam("ret", hapRet);

    CommonEventData commonData;
    commonData.SetWant(want);
    commonData.SetCode(code);
    commonData.SetData(data);
    return CommonEventManager::PublishCommonEvent(commonData);
}
sptr<IRemoteObject> CheckSelfPermissionAbilityA1::OnConnect(const Want &want)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    return ret;
}
void CheckSelfPermissionAbilityA1::OnDisconnect(const Want &want)
{
    APP_LOGI("CheckSelfPermissionAbilityA1::OnDisconnect");

    Ability::OnDisconnect(want);
}
void CheckSelfPermissionAbilityA1::CheckSelfPermission0100()
{
    APP_LOGI("app func CheckSelfPermission0100 start");
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permissionName = "ohos.permission.GET_NETWORK_INFO";
    int32_t result = service->CheckSelfPermission(permissionName);
    PublishEvent(APP_A1_RESP_EVENT_NAME, result, "CheckSelfPermission0100");
    APP_LOGI("app func CheckSelfPermission0100 end");
}
void CheckSelfPermissionAbilityA1::CheckSelfPermission0400()
{
    APP_LOGI("app func CheckSelfPermission0400 start");
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permissionName = "ohos.permission.GET_NETWORK_INFO";
    auto startTime = std::chrono::high_resolution_clock::now();
    service->CheckSelfPermission(permissionName);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    int32_t result = -1;
    if (fp_ms.count() <= MAX_ELAPSED) {
        result = 0;
    }
    APP_LOGI("timeCount:%{public}f", fp_ms.count());
    PublishEvent(APP_A1_RESP_EVENT_NAME, result, "CheckSelfPermission0400");
    APP_LOGI("app func CheckSelfPermission0400 end");
}
void CheckSelfPermissionAbilityA1::CheckCallingPermission0600()
{
    APP_LOGI("app func CheckCallingPermission0600 start");
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permissionName = "ohos.permission.GET_NETWORK_INFO";
    auto startTime = std::chrono::high_resolution_clock::now();
    service->CheckCallingPermission(permissionName);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    int32_t result = -1;
    if (fp_ms.count() <= MAX_ELAPSED) {
        result = 0;
    }
    APP_LOGI("timeCount:%{public}f", fp_ms.count());
    PublishEvent(APP_A1_RESP_EVENT_NAME, result, "CheckCallingPermission0600");
    APP_LOGI("app func CheckCallingPermission0600 end");
}
void CheckSelfPermissionAbilityA1::CheckCallingOrSelfPermission0300()
{
    APP_LOGI("app func CheckCallingOrSelfPermission0300 start");
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permissionName = "ohos.permission.INTERNET";
    int32_t result = service->CheckCallingOrSelfPermission(permissionName);
    PublishEvent(APP_A1_RESP_EVENT_NAME, result, "CheckCallingOrSelfPermission0300");
    APP_LOGI("app func CheckCallingOrSelfPermission0300 end");
}
void CheckSelfPermissionAbilityA1::CheckCallingOrSelfPermission0400()
{
    APP_LOGI("app func CheckCallingOrSelfPermission0400 start");
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permissionName = "ohos.permission.GET_NETWORK_INFO";
    auto startTime = std::chrono::high_resolution_clock::now();
    service->CheckCallingOrSelfPermission(permissionName);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    int32_t result = -1;
    if (fp_ms.count() <= MAX_ELAPSED) {
        result = 0;
    }
    APP_LOGI("timeCount:%{public}f", fp_ms.count());
    PublishEvent(APP_A1_RESP_EVENT_NAME, result, "CheckCallingOrSelfPermission0400");
    APP_LOGI("app func CheckCallingOrSelfPermission0400 end");
}
void CheckSelfPermissionAbilityA1::CheckCallerPermission0100()
{
    APP_LOGI("app func CheckCallerPermission0100 start");
    std::unique_ptr<DistributedPermissionKit> service = std::make_unique<DistributedPermissionKit>();
    std::string permissionName = "ohos.permission.INTERNET";
    auto startTime = std::chrono::high_resolution_clock::now();
    service->CheckCallerPermission(permissionName);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = endTime - startTime;
    int32_t result = -1;
    if (fp_ms.count() <= MAX_ELAPSED) {
        result = 0;
    }
    APP_LOGI("timeCount:%{public}f", fp_ms.count());
    PublishEvent(APP_A1_RESP_EVENT_NAME, result, "CheckCallerPermission0100");
    APP_LOGI("app func CheckCallerPermission0100 end");
}
bool CheckSelfPermissionAbilityA1::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_A1_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void CheckSelfPermissionAbilityA1::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI(
        "CheckSelfPermissionAbilityA1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (APP_A1_REQ_EVENT_NAME.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI("CheckSelfPermissionAbilityA1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = "
                     "%{public}s",
                eventName.c_str(),
                data.GetCode(),
                dataContent.c_str());
        } else {
            if (mainAbility_ != nullptr) {
                (mainAbility_->*funcMap_[dataContent])();
            }
        }
    }
}
REGISTER_AA(CheckSelfPermissionAbilityA1);
}  // namespace AppExecFwk
}  // namespace OHOS