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

#ifndef DPMS_ST_SERVICE_DISTRIBUTED_THIRD_A_
#define DPMS_ST_SERVICE_DISTRIBUTED_THIRD_A_
#include <string>
#include <map>
#include "ability_connect_callback_stub.h"
#include "ability_connect_callback_proxy.h"
#include "ability_loader.h"
#include "common_event.h"
#include "common_event_manager.h"
namespace OHOS {
namespace AppExecFwk {
using AbilityConnectionStub = OHOS::AAFwk::AbilityConnectionStub;
using AbilityConnectionProxy = OHOS::AAFwk::AbilityConnectionProxy;
const std::string RESP_EVENT_NAME_THIRD_A = "resp_com_ohos_dpmsst_service_distributed_third_a";
const std::string REQ_EVENT_NAME_THIRD_A = "req_com_ohos_dpmsst_service_distributed_third_a";
class DpmsStServiceDistributedThirdA : public Ability {
public:
    ~DpmsStServiceDistributedThirdA();

protected:
    virtual void OnStart(const Want &want) override;
    virtual void OnStop() override;
    virtual void OnActive() override;
    virtual void OnInactive() override;
    virtual void OnBackground() override;
    virtual void OnNewWant(const Want &want) override;
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;
    virtual sptr<IRemoteObject> OnConnect(const Want &want) override;
    virtual void OnDisconnect(const Want &want) override;

private:
    void Clear();
    void GetWantInfo(const Want &want);
    std::vector<std::string> Split(std::string str, const std::string &token);
    bool SubscribeEvent();
    static bool PublishEvent(const std::string &eventName, const int &code, const std::string &data);
    void StopSelfAbility();
    void VerifyPermissionFromRemote();
    void VerifySelfPermissionFromRemote();
    void RequestPermissionsFromRemote();
    void CanRequestPermissionFromRemote();
    string GetAppIdInfo(int32_t pid, int32_t uid);
    std::string permName = {};
    typedef void (DpmsStServiceDistributedThirdA::*func)();
    static std::map<std::string, func> funcMap_;
    class AbilityConnectCallback;
    sptr<AbilityConnectCallback> stub_ = {};
    sptr<AbilityConnectionProxy> connCallback_ = {};
    class AppEventSubscriber;
    std::shared_ptr<AppEventSubscriber> subscriber_ = {};
    std::string permission_ = "";
    std::string nodeId_ = "";
    std::string bundleName_ = "";
    int32_t reasonResId_ = 1;
    class AbilityConnectCallback : public AbilityConnectionStub {
    public:
        sptr<IRemoteObject> AsObject() override
        {
            return nullptr;
        }
        /**
         * OnAbilityConnectDone, AbilityMs notify caller ability the result of connect.
         *
         * @param element,.service ability's ElementName.
         * @param remoteObject,.the session proxy of service ability.
         * @param resultCode, ERR_OK on success, others on failure.
         */
        void OnAbilityConnectDone(
            const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override
        {
            if (resultCode == 0) {
                onAbilityConnectDoneCount++;
                PublishEvent(RESP_EVENT_NAME_THIRD_A, onAbilityConnectDoneCount, "OnAbilityConnectDone");
            }
        }

        /**
         * OnAbilityDisconnectDone, AbilityMs notify caller ability the result of disconnect.
         *
         * @param element,.service ability's ElementName.
         * @param resultCode, ERR_OK on success, others on failure.
         */
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override
        {
            if (resultCode == 0) {
                onAbilityConnectDoneCount--;
                PublishEvent(RESP_EVENT_NAME_THIRD_A, onAbilityConnectDoneCount, "OnAbilityDisconnectDone");
            }
        }

        static int onAbilityConnectDoneCount;
    };
    class AppEventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        AppEventSubscriber(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp){};
        ~AppEventSubscriber() = default;
        virtual void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

        DpmsStServiceDistributedThirdA *mainAbility_ = nullptr;
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // DPMS_ST_SERVICE_DISTRIBUTED_THIRD_A_