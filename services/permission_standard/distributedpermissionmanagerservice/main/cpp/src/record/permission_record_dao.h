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
#ifndef PERMISSION_RECORD_DAO_H
#define PERMISSION_RECORD_DAO_H
#include <vector>

#include "constant.h"
#include "permission_log.h"
#include "sqlite_storage.h"

namespace OHOS {
namespace Security {
namespace Permission {
class PermissionRecordDao final {
public:
    static PermissionRecordDao &GetInstance();

    PermissionRecordDao();

    virtual ~PermissionRecordDao();

    bool FindVisitor(const GenericValues &visitorGenericValues, std::vector<GenericValues> &visitorValues);

    bool FindRecord(const GenericValues &recordAndGenericValues, const GenericValues &recordOrGenericValues,
        std::vector<GenericValues> &recordValues);

    bool AddVisitor(const std::vector<GenericValues> &visitorValues);

    bool AddRecord(const std::vector<GenericValues> &recordValues);

    bool RemoveVisitor(const GenericValues &visitorValue);

    bool RemoveRecord(const GenericValues &recordValue);
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // PERMISSION_RECORD_DAO_H
