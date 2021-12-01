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

#ifndef AMS_ADAPTER_H
#define AMS_ADAPTER_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "constant.h"
#include "ability_manager_interface.h"
#include "if_system_ability_manager.h"

namespace OHOS {
namespace Security {
namespace Permission {
class AmsAdapter {
public:
    AmsAdapter() = default;
    virtual ~AmsAdapter() = default;
    sptr<AAFwk::IAbilityManager> GetAbilityManager();

private:
    OHOS::sptr<ISystemAbilityManager> saMgr_;
    std::mutex saMutex_;
    sptr<IRemoteObject> GetSystemAbility(const Constant::ServiceId systemAbilityId);
    sptr<AAFwk::IAbilityManager> iAbilityManager_;
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // AMS_ADAPTER_H
