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

#include "dpms_st_service_distributed_system_signed.h"
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
int DpmsStServiceDistributedSystemSigned::AbilityConnectCallback::onAbilityConnectDoneCount = 0;
int hapRet = -1;
std::map<std::string, DpmsStServiceDistributedSystemSigned::func> DpmsStServiceDistributedSystemSigned::funcMap_ = {
    {"StopSelfAbility", &DpmsStServiceDistributedSystemSigned::StopSelfAbility},
    {"VerifyPermissionFromRemote", &DpmsStServiceDistributedSystemSigned::VerifyPermissionFromRemote},
    {"VerifySelfPermissionFromRemote", &DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote},
    {"RequestPermissionsFromRemote", &DpmsStServiceDistributedSystemSigned::RequestPermissionsFromRemote},
    {"CanRequestPermissionFromRemote", &DpmsStServiceDistributedSystemSigned::CanRequestPermissionFromRemote},
};

DpmsStServiceDistributedSystemSigned::~DpmsStServiceDistributedSystemSigned()
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
        APP_LOGI("DpmsStServiceDistributedSystemSigned  TestCallback :OnResult  is run!!!");
    }

    void OnCancel(const std::string nodeId, std::vector<std::string> permissions)
    {
        APP_LOGI("DpmsStServiceDistributedSystemSigned  TestCallback :OnCancel  is run!!!");
    }
    void OnTimeOut(const std::string nodeId, std::vector<std::string> permissions)
    {
        APP_LOGI("DpmsStServiceDistributedSystemSigned  TestCallback :OnTimeOut  is run!!! nodeId = %{public}s",
            nodeId.c_str());
    }
};

std::vector<std::string> DpmsStServiceDistributedSystemSigned::Split(std::string str, const std::string &token)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::Split");

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

void DpmsStServiceDistributedSystemSigned::StopSelfAbility()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::StopSelfAbility");

    TerminateAbility();
}

string DpmsStServiceDistributedSystemSigned::GetAppIdInfo(int32_t pid, int32_t uid)
{
    return DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(pid, uid);
}

void DpmsStServiceDistributedSystemSigned::VerifyPermissionFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifyPermissionFromRemote");

    std::string permission = permission_;
    std::string nodeId = nodeId_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();
    std::string appInfo = GetAppIdInfo(pid, uid);

    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote  pid = %{public}d, uid = "
             "%{public}d, permission= %{public}s,nodeId= %{public}s ",
        pid,
        uid,
        permission.c_str(),
        nodeId.c_str());

    int result = DistributedPermissionKit::VerifyPermissionFromRemote(permission, nodeId, appInfo);

    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifyPermissionFromRemote  result = %{public}d, ", result);

    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, result, "VerifyPermissionFromRemote");
    Clear();
}

void DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote");

    std::string permission = permission_;
    std::string nodeId = nodeId_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();

    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote  pid = %{public}d, uid = "
             "%{public}d, permission= %{public}s,nodeId= %{public}s ",
        pid,
        uid,
        permission.c_str(),
        nodeId.c_str());

    int result = DistributedPermissionKit::VerifySelfPermissionFromRemote(permission, nodeId);

    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote  result = %{public}d, ", result);

    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, result, "VerifySelfPermissionFromRemote");
    Clear();
}

void DpmsStServiceDistributedSystemSigned::CanRequestPermissionFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::CanRequestPermissionFromRemote");
    std::string permission = permission_;
    std::string nodeId = nodeId_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();

    APP_LOGI("DpmsStServiceDistributedSystemSigned::VerifySelfPermissionFromRemote  pid = %{public}d, uid = "
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
    APP_LOGI("DpmsStServiceDistributedSystemSigned::CanRequestPermissionFromRemote result = %{public}d, ", result);
    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, result, "CanRequestPermissionFromRemote");
    Clear();
}

void DpmsStServiceDistributedSystemSigned::RequestPermissionsFromRemote()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::RequestPermissionsFromRemote");
    int32_t reasonResId = 1;
    std::string permission = permission_;
    std::string nodeId = nodeId_;
    std::string bundleName = bundleName_;
    const int32_t pid = IPCSkeleton::GetCallingPid();
    const int32_t uid = IPCSkeleton::GetCallingUid();
    OHOS::sptr<TestCallback> callback(new TestCallback());

    APP_LOGI("DpmsStServiceDistributedSystemSigned::RequestPermissionsFromRemote  pid = %{public}d, uid = "
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
    APP_LOGI("DpmsStServiceDistributedSystemSigned::RequestPermissionsFromRemote  callback->result = %{public}d, ",
        callback->result);
    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, callback->result, "RequestPermissionsFromRemote");
    Clear();
}

void DpmsStServiceDistributedSystemSigned::OnStart(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnStart");

    GetWantInfo(want);
    Ability::OnStart(want);

    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, AbilityLifecycleExecutor::LifecycleState::INACTIVE, "OnStart");
    SubscribeEvent();
}
void DpmsStServiceDistributedSystemSigned::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnCommand");

    Ability::OnCommand(want, restart, startId);
}
void DpmsStServiceDistributedSystemSigned::OnNewWant(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnNewWant");

    GetWantInfo(want);
    Ability::OnNewWant(want);
}
void DpmsStServiceDistributedSystemSigned::OnStop()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnStop");

    Ability::OnStop();
    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, AbilityLifecycleExecutor::LifecycleState::INITIAL, "OnStop");
}
void DpmsStServiceDistributedSystemSigned::OnActive()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnActive");

    Ability::OnActive();
}
void DpmsStServiceDistributedSystemSigned::OnInactive()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnInactive");

    Ability::OnInactive();
}
void DpmsStServiceDistributedSystemSigned::OnBackground()
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnBackground");

    Ability::OnBackground();
}

void DpmsStServiceDistributedSystemSigned::Clear()
{
    permission_ = "";
    nodeId_ = "";
    bundleName_ = "";
}
void DpmsStServiceDistributedSystemSigned::GetWantInfo(const Want &want)
{
    Want mWant(want);
    permission_ = mWant.GetStringParam("permission");
    nodeId_ = mWant.GetStringParam("nodeId");
    bundleName_ = mWant.GetStringParam("bundleName");
}
bool DpmsStServiceDistributedSystemSigned::PublishEvent(
    const std::string &eventName, const int &code, const std::string &data)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::PublishEvent eventName = %{public}s, code = %{public}d, data = "
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
sptr<IRemoteObject> DpmsStServiceDistributedSystemSigned::OnConnect(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnConnect");

    sptr<IRemoteObject> ret = Ability::OnConnect(want);
    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, AbilityLifecycleExecutor::LifecycleState::ACTIVE, "OnConnect");
    return ret;
}
void DpmsStServiceDistributedSystemSigned::OnDisconnect(const Want &want)
{
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnDisconnect");

    Ability::OnDisconnect(want);
    PublishEvent(RESP_EVENT_NAME_SYSTEM_SIGNED, AbilityLifecycleExecutor::LifecycleState::BACKGROUND, "OnDisconnect");
}
bool DpmsStServiceDistributedSystemSigned::SubscribeEvent()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(REQ_EVENT_NAME_SYSTEM_SIGNED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber_ = std::make_shared<AppEventSubscriber>(subscribeInfo);
    subscriber_->mainAbility_ = this;
    return CommonEventManager::SubscribeCommonEvent(subscriber_);
}
void DpmsStServiceDistributedSystemSigned::AppEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    auto eventName = data.GetWant().GetAction();
    auto dataContent = data.GetData();
    APP_LOGI("DpmsStServiceDistributedSystemSigned::OnReceiveEvent eventName = %{public}s, code = %{public}d, data "
             "= %{public}s",
        eventName.c_str(),
        data.GetCode(),
        dataContent.c_str());
    if (REQ_EVENT_NAME_SYSTEM_SIGNED.compare(eventName) == 0) {
        if (funcMap_.find(dataContent) == funcMap_.end()) {
            APP_LOGI("DpmsStServiceDistributedSystemSigned::OnReceiveEvent eventName = %{public}s, code = "
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
REGISTER_AA(DpmsStServiceDistributedSystemSigned);
}  // namespace AppExecFwk
}  // namespace OHOS