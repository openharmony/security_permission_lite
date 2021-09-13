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
#define private public
#define protected public

#include <unistd.h>
#include <csignal>
#include "permission_log.h"
#include "../include/getparam.h"
#include "../include/fuzz_test_manager.h"
#include "../include/fuzz_config_parser.h"
#include "distributed_permission_kit.h"
#include "on_permission_used_record_stub.h"

#undef private
#undef protected
namespace OHOS {
namespace Security {
namespace Permission {
class OnPermissionUsedRecordCallback : public OnPermissionUsedRecordStub {
public:
    OnPermissionUsedRecordCallback() = default;
    virtual ~OnPermissionUsedRecordCallback() = default;
    void OnQueried(ErrCode code, QueryPermissionUsedResult &result)
    {}
};

std::shared_ptr<FuzzTestManager> FuzzTestManager::instance_ = nullptr;
const std::string distributedPermissionKitName_ = "DistributedPermissionKit";
const std::string distributedPermissionManager_ = "DistributedPermissionManager";

void FuzzTestManager::fuzzTestManagerTest()
{
    callFunctionMap_["DistributedPermissionKitFuzzTestMethodintstring"] = []() {
        std::cout << "DistributedPermissionKitFuzzTestMethodintstring" << std::endl;
    };
}

void FuzzTestManager::DistributedPermissionKitFuzzTest()
{
    callFunctionMap_[distributedPermissionKitName_ + "AllocateDuid"] = []() {
        DistributedPermissionKit::AllocateDuid(GetStringParam(), GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "QueryDuid"] = []() {
        DistributedPermissionKit::QueryDuid(GetStringParam(), GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "IsDuid"] = []() {
        DistributedPermissionKit::IsDuid(GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CreateAppIdInfo1"] = []() {
        DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(GetS32Param(), GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CreateAppIdInfo2"] = []() {
        DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(GetS32Param(), GetS32Param(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CreateAppIdInfo3"] = []() {
        DistributedPermissionKit::AppIdInfoHelper::CreateAppIdInfo(
            GetS32Param(), GetS32Param(), GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "ParseAppIdInfo"] = []() {
        AppIdInfo appIdInfo;
        appIdInfo.deviceId = GetStringParam();
        appIdInfo.pid = GetIntParam();
        appIdInfo.uid = GetIntParam();
        DistributedPermissionKit::AppIdInfoHelper::ParseAppIdInfo(GetStringParam(), appIdInfo);
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckDPermission"] = []() {
        DistributedPermissionKit::CheckDPermission(GetS32Param(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckPermission2"] = []() {
        DistributedPermissionKit::CheckPermission(GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckSelfPermission"] = []() {
        DistributedPermissionKit::CheckSelfPermission(GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckCallingPermission"] = []() {
        DistributedPermissionKit::CheckCallingPermission(GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckCallingOrSelfPermission"] = []() {
        DistributedPermissionKit::CheckCallingOrSelfPermission(GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckCallerPermission"] = []() {
        DistributedPermissionKit::CheckCallerPermission(GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "IsRestrictedPermission"] = []() {
        DistributedPermissionKit::IsRestrictedPermission(GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "VerifyPermissionFromRemote"] = []() {
        DistributedPermissionKit::VerifyPermissionFromRemote(GetStringParam(), GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "VerifySelfPermissionFromRemote"] = []() {
        DistributedPermissionKit::VerifySelfPermissionFromRemote(GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CanRequestPermissionFromRemote"] = []() {
        DistributedPermissionKit::CanRequestPermissionFromRemote(GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "RequestPermissionsFromRemote"] = []() {
        DistributedPermissionKit::RequestPermissionsFromRemote(GetStringVectorParam(),
            GetOnRequestPermissionsResultCallBack(),
            GetStringParam(),
            GetStringParam(),
            GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "GrantSensitivePermissionToRemoteApp"] = []() {
        DistributedPermissionKit::GrantSensitivePermissionToRemoteApp(
            GetStringParam(), GetStringParam(), GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "RegisterUsingPermissionReminder"] = []() {
        DistributedPermissionKit::RegisterUsingPermissionReminder(GetOnUsingPermissionReminderSptrParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "UnregisterUsingPermissionReminder"] = []() {
        DistributedPermissionKit::UnregisterUsingPermissionReminder(GetOnUsingPermissionReminderSptrParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckPermissionAndStartUsing"] = []() {
        DistributedPermissionKit::CheckPermissionAndStartUsing(GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "CheckCallerPermissionAndStartUsing"] = []() {
        DistributedPermissionKit::CheckCallerPermissionAndStartUsing(GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "StartUsingPermission"] = []() {
        DistributedPermissionKit::StartUsingPermission(GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "StopUsingPermission"] = []() {
        DistributedPermissionKit::StopUsingPermission(GetStringParam(), GetStringParam());
    };
    callFunctionMap_[distributedPermissionKitName_ + "AddPermissionUsedRecord"] = []() {
        DistributedPermissionKit::AddPermissionUsedRecord(
            GetStringParam(), GetStringParam(), GetS32Param(), GetS32Param());
    };
    callFunctionMap_[distributedPermissionKitName_ + "GetPermissionUsedRecords1"] = []() {
        QueryPermissionUsedResult result;
        DistributedPermissionKit::GetPermissionUsedRecords(GetQueryPermissionUsedRequestParam(), result);
    };
    callFunctionMap_[distributedPermissionKitName_ + "GetPermissionUsedRecords2"] = []() {
        OHOS::sptr<OnPermissionUsedRecordCallback> callback(new OnPermissionUsedRecordCallback());
        DistributedPermissionKit::GetPermissionUsedRecords(GetQueryPermissionUsedRequestParam(), callback);
    };
}

FuzzTestManager::FuzzTestManager()
{
    fuzzTestManagerTest();
    DistributedPermissionKitFuzzTest();
}

void FuzzTestManager::SetJsonFunction(std::string functionName)
{
    remainderMap_.emplace(functionName, cycle_);
}

void FuzzTestManager::SetCycle(uint16_t cycle)
{
    cycle_ = cycle;
    for_each(remainderMap_.begin(), remainderMap_.end(), [cycle](std::unordered_map<std::string, int>::reference temp) {
        temp.second = cycle;
    });
}

int GetRandomInt(int minNum, int maxNum)
{
    return GetU16Param() % (maxNum - minNum + 1) + minNum;
}

void action(int a)
{
    std::cout << "Interrupt signal (" << a << ") received.\n";
}

void FuzzTestManager::StartFuzzTest()
{
    std::cout << __func__ << std::endl;
    OHOS::FuzzConfigParser jsonParser;
    OHOS::FuzzTestData tempData;

    std::cout << "parseFromFile start" << std::endl;
    jsonParser.ParseFromFile4FuzzTest("/data/config.json", tempData);
    std::cout << "flag :" << tempData.mainLoopFlag << std::endl;
    for_each(tempData.methodVec.begin(), tempData.methodVec.end(), [this](std::vector<std::string>::reference s) {
        SetJsonFunction(s);
    });
    SetCycle(tempData.mainLoopFlag);

    std::vector<std::string> index;
    std::unordered_map<std::string, int>::iterator it = remainderMap_.begin();
    while (it != remainderMap_.end()) {
        if (it->second <= 0) {
            it = remainderMap_.erase(it);
        } else {
            index.push_back(it->first);
            it++;
        }
    }

    std::cout << remainderMap_.size() << "--------fuzz test start--------" << callFunctionMap_.size() << std::endl;
    for (; remainderMap_.size() > 0;) {
        std::string functionName;
        int offset = GetRandomInt(0, index.size() - 1);
        functionName = index[offset];
        if (callFunctionMap_.find(functionName) != callFunctionMap_.end()) {
            std::cout << "call function : " << functionName << std::endl;
            callFunctionMap_[functionName]();
            std::cout << "function end  :" << functionName << std::endl;
        } else {
            std::cout << "can't find function : " << functionName << std::endl;
        }
        remainderMap_[functionName]--;
        if (remainderMap_[functionName] <= 0) {
            remainderMap_.erase(functionName);
            index.erase(index.begin() + offset);
        };
    }
    std::cout << remainderMap_.size() << "--------fuzz test end--------" << std::endl;
}

}  // namespace Permission
}  // namespace Security
}  // namespace OHOS