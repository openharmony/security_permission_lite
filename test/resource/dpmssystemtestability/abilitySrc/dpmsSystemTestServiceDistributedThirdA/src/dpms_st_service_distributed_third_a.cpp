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

#include "dpms_st_service_distributed_third_a.h"
#include "on_request_permissions_result_stub.h"
#include "app_log_wrapper.h"
#include "common_event.h"
#include "common_event_manager.h"
#include "distributed_permission_kit.h"
#include "ipc_skeleton.h"

using namespace OHOS::EventFwk;
using namespace OHOS::Security::Permission;
using MAP_STR_STR = std::map<std::string, std::string>;
namespace OHOS {
namespace AppExecFwk {
using AbilityConnectionProxy = OHOS::AAFwk::AbilityConnectionProxy;
int DpmsStServiceDistributedThirdA::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
int hapRet = -1;
std::map<std::string, DpmsStServiceDistributedThirdA::func> DpmsStServiceDistributedThirdA::funcMap_ = {
    {"StopSelfAbility", &DpmsStServiceDistributedThirdA::StopSelfAbility},
    {"VerifyPermissionFromRemote", &DpmsStServiceDistributedThirdA::VerifyPermissionFromRemote},
    {"VerifySelfPermissionFromRemote", &DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote},
    {"RequestPermissionsFromRemote", &DpmsStServiceDistributedThirdA::RequestPermissionsFromRemote},
    {"CanRequestPermissionFromRemote", &DpmsStServiceDistributedThirdA::CanRequestPermissionFromRemote},

};

DpmsStServiceDistributedThirdA::~DpmsStServiceDistributedThirdA()
{
    CommonEventManager::UnSubscribeCommonEvent(subscriber_);
}

class TestCallback : public OnRequestPermissionsResultStub {
public:
    TestCallback() = default;
    virtual ~TestCallback() = default;

    int32_t result = -1;

    void OnResult(const std::string nodeId, std::vector<std::string> permissions, std::vector<int32_t> grantResults)
    {
        // Default only one permission, so only one grantResult
        if (grantResults[0] == Constant::PERMISSION_GRANTED) {
            result = 0;
        }
        APP_LOGI("DpmsStServiceDistributedThirdA  TestCallback :OnResult  is run!!!");
    }

    void OnCancel(const std::string nodeId, std::vector<std::string> permissions)
    {
        APP_LOGI("DpmsStServiceDistributedThirdA  TestCallback :OnCancel  is run!!!");
    }
    void OnTimeOut(const std::string nodeId, std::vector<std::string> permissions)
    {
        APP_LOGI(
            "DpmsStServiceDistributedThirdA  TestCallback :OnTimeOut  is run!!! nodeId = %{public}s", nodeId.c_str());
    }
};

std::vector<std::string> DpmsStServiceDistributedThirdA::Split(std::string str, const std::string &token)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::Split");

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

void DpmsStServiceDistributedThirdA::StopSelfAbility()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::StopSelfAbility");

    TerminateAbility();
}

//生成appinfo
string DpmsStServiceDistributedThirdA::GetAppIdInfo(int32_t pid, int32_t uid)
{
    return DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid, uid);
}

void DpmsStServiceDistributedThirdA::VerifyPermissionFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::VerifyPermissionFromRemote");

    std::string permission = permission_;
    std::string nodeId = nodeId_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();
    std::string appInfo = GetAppIdInfo(pid, uid);

    APP_LOGI("DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote  pid = %{public}d, uid = "
             "%{public}d, permission= %{public}s,nodeId= %{public}s ",
        pid,
        uid,
        permission.c_str(),
        nodeId.c_str());

    int result = DistributedPermissionKit::VerifyPermissionFromRemote(permission, nodeId, appInfo);

    APP_LOGI("DpmsStServiceDistributedThirdA::VerifyPermissionFromRemote  result = %{public}d, ", result);

    PublishEvent(RESP_EVENT_NAME_THIRD_A, result, "VerifyPermissionFromRemote");
}

void DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote");

    std::string permission = permission_;
    std::string nodeId = nodeId_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();

    APP_LOGI("DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote  pid = %{public}d, uid = "
             "%{public}d, permission= %{public}s,nodeId= %{public}s ",
        pid,
        uid,
        permission.c_str(),
        nodeId.c_str());

    int result = DistributedPermissionKit::VerifySelfPermissionFromRemote(permission, nodeId);

    APP_LOGI("DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote  result = %{public}d, ", result);

    PublishEvent(RESP_EVENT_NAME_THIRD_A, result, "VerifySelfPermissionFromRemote");
}

void DpmsStServiceDistributedThirdA::CanRequestPermissionFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::CanRequestPermissionFromRemote");
    std::string permission = permission_;
    std::string nodeId = nodeId_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();

    APP_LOGI("DpmsStServiceDistributedThirdA::VerifySelfPermissionFromRemote  pid = %{public}d, uid = "
             "%{public}d, permission= %{public}s,nodeId= %{public}s ",
        pid,
        uid,
        permission.c_str(),
        nodeId.c_str());
    bool res = DistributedPermissionKit::CanRequestPermissionFromRemote(permission, nodeId);

    int result = 0;
    if (res) {
        result = 1;
    } else {
        result = 0;
    }
    APP_LOGI("DpmsStServiceDistributedThirdA::CanRequestPermissionFromRemote result = %{public}d, ", result);
    PublishEvent(RESP_EVENT_NAME_THIRD_A, result, "CanRequestPermissionFromRemote");
}

void DpmsStServiceDistributedThirdA::RequestPermissionsFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::RequestPermissionsFromRemote");
    int32_t reasonResId = reasonResId_;
    std::string permission = permission_;
    std::string nodeId = nodeId_;
    std::string bundleName = bundleName_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();
    OHOS::sptr<TestCallback> callback(new TestCallback());

    APP_LOGI("DpmsStServiceDistributedThirdA::RequestPermissionsFromRemote  pid = %{public}d, uid = "
             "%{public}d, permission= %{public}s,nodeId= %{public}s, bundleName=%{public}s ",
        pid,
        uid,
        permission.c_str(),
        nodeId.c_str(),
        bundleName.c_str());

    std::vector<std::string> permissions;
    permissions.push_back(permission);
    DistributedPermissionKit::RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    sleep(1);

    APP_LOGI("DpmsStServiceDistributedThirdA::RequestPermissionsFromRemote  callback->result = %{public}d, ",
        callback->result);
    PublishEvent(RESP_EVENT_NAME_THIRD_A, callback->result, "RequestPermissionsFromRemote");
}

void DpmsStServiceDistributedThirdA::OnStart(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);

    PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();
}
void DpmsStServiceDistributedThirdA::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnCommand");

    // GetWantInfo(want);
    Ability::OnCommand(want, restart, startId);

    // PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnCommand");
}
void DpmsStServiceDistributedThirdA::OnNewWant(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void DpmsStServiceDistributedThirdA::OnStop()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnStop");

    Ability::OnStop();
    PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
    Clear();
}
void DpmsStServiceDistributedThirdA::OnActive()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnActive");

    Ability::OnActive();
    // PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnActive");
}
void DpmsStServiceDistributedThirdA::OnInactive()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnInactive");

    Ability::OnInactive();
    // PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnInactive");
}
void DpmsStServiceDistributedThirdA::OnBackground()
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnBackground");

    Ability::OnBackground();
    // PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::BACKGROUND,
    // "OnBackground");
}

void DpmsStServiceDistributedThirdA::Clear()
{
    permission_ = "";
    nodeId_ = "";
    bundleName_ = "";
    reasonResId_ = 1;
}
void DpmsStServiceDistributedThirdA::GetWantInfo(const Want &want)
{
    Want mWant(want);
    permission_ = mWant.GetStringParam("permission");
    nodeId_ = mWant.GetStringParam("nodeId");
    bundleName_ = mWant.GetStringParam("bundleName");
    std::string temp_reasonResId = mWant.GetStringParam("reasonResId");
    if (temp_reasonResId == "-1") {
        reasonResId_ = -1;
    }
}
bool DpmsStServiceDistributedThirdA::PublishEvent(
    const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::PublishEvent eventName = %{public}s, code = %{public}d, data = "
             "%{public}s",
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
sptr<IRemoteObject> DpmsStServiceDistributedThirdA::OnConnect(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}
void DpmsStServiceDistributedThirdA::OnDisconnect(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedThirdA::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(RESP_EVENT_NAME_THIRD_A, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}
bool DpmsStServiceDistributedThirdA::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(REQ_EVENT_NAME_THIRD_A);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void DpmsStServiceDistributedThirdA::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("DpmsStServiceDistributedThirdA::OnReceiveEvent eventName = %{public}s, code = %{public}d, data "
             "= %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (REQ_EVENT_NAME_THIRD_A.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI("DpmsStServiceDistributedThirdA::OnReceiveEvent eventName = %{public}s, code = "
                     "%{public}d, data = %{public}s",
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
REGISTER_AA(DpmsStServiceDistributedThirdA);
}  // namespace AppExecFwk
}  // namespace OHOS