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

#include "dpms_st_service_ability_a1.h"
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
int DpmsStServiceAbilityA1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
int hapRet = -1;
std::map<std::string, DpmsStServiceAbilityA1::func> DpmsStServiceAbilityA1::funcMap_ = {
    {"StopSelfAbility", &DpmsStServiceAbilityA1::StopSelfAbility},
};

DpmsStServiceAbilityA1::~DpmsStServiceAbilityA1()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

std::vector<std::string> DpmsStServiceAbilityA1::Split(std::string str, const std::string &token)
{
    APP_LOGI("DpmsStServiceAbilityA1::Split");

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

void DpmsStServiceAbilityA1::StopSelfAbility()
{
    APP_LOGI("DpmsStServiceAbilityA1::StopSelfAbility");

    TerminateAbility();
}

void DpmsStServiceAbilityA1::OnStart(const Want &want)
{
    APP_LOGI("DpmsStServiceAbilityA1::OnStart");
    GetWantInfo(want);
    Ability::OnStart(want);

    string permName = "ohos.permission.CAMERA";
    hapRet = DistributedPermissionKit::CheckCallerPermissionAndStartUsing(permName);

    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();
}
void DpmsStServiceAbilityA1::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("DpmsStServiceAbilityA1::OnCommand");

    GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void DpmsStServiceAbilityA1::OnNewWant(const Want &want)
{
    APP_LOGI("DpmsStServiceAbilityA1::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void DpmsStServiceAbilityA1::OnStop()
{
    APP_LOGI("DpmsStServiceAbilityA1::OnStop");

    Ability::OnStop();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void DpmsStServiceAbilityA1::OnActive()
{
    APP_LOGI("DpmsStServiceAbilityA1::OnActive");

    Ability::OnActive();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void DpmsStServiceAbilityA1::OnInactive()
{
    APP_LOGI("DpmsStServiceAbilityA1::OnInactive");

    Ability::OnInactive();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void DpmsStServiceAbilityA1::OnBackground()
{
    APP_LOGI("DpmsStServiceAbilityA1::OnBackground");

    Ability::OnBackground();
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnBackground");
}

void DpmsStServiceAbilityA1::Clear()
{
    permName = "";
}
void DpmsStServiceAbilityA1::GetWantInfo(const Want &want)
{
    Want mWant(want);
    permName = mWant.GetStringParam("permName");
    DpmsStServiceAbilityA1::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
}
bool DpmsStServiceAbilityA1::PublishEvent(const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("DpmsStServiceAbilityA1::PublishEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
sptr<IRemoteObject> DpmsStServiceAbilityA1::OnConnect(const Want &want)
{
    APP_LOGI("DpmsStServiceAbilityA1::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}
void DpmsStServiceAbilityA1::OnDisconnect(const Want &want)
{
    APP_LOGI("DpmsStServiceAbilityA1::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(APP_A1_RESP_EVENT_NAME, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}
bool DpmsStServiceAbilityA1::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(APP_A1_REQ_EVENT_NAME);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void DpmsStServiceAbilityA1::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("DpmsStServiceAbilityA1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (APP_A1_REQ_EVENT_NAME.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI(
                "DpmsStServiceAbilityA1::OnReceiveEvent eventName = %{public}s, code = %{public}d, data = %{public}s",
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
REGISTER_AA(DpmsStServiceAbilityA1);
}  // namespace AppExecFwk
}  // namespace OHOS