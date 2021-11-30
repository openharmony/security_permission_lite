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

#ifndef PERMISSION_RECORD_REPOSITORY_H
#define PERMISSION_RECORD_REPOSITORY_H

#include "query_permission_used_request.h"
#include "permission_record.h"
#include "permission_visitor.h"
#include "permission_record_dao.h"

namespace OHOS {
namespace Security {
namespace Permission {
class PermissionRecordRepository final {
public:
    static PermissionRecordRepository &GetInstance();

    PermissionRecordRepository();

    virtual ~PermissionRecordRepository();

    int32_t FindVisitorValues(GenericValues &visitorGenericValues, std::vector<GenericValues> &visitorValues);

    int32_t FindRecordValues(const GenericValues &recordAndGenericValues, const GenericValues &recordOrGenericValues,
        std::vector<GenericValues> &recordValues);

    bool AddVisitorValues(PermissionVisitor &permissionVisitor);

    bool AddRecordValues(const PermissionRecord &permissionRecord);

    bool RemoveVisitor(const GenericValues &visitorValue);

    bool RemoveRecord(const GenericValues &recordValue);
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif  // PERMISSION_RECORD_REPOSITORY_H
