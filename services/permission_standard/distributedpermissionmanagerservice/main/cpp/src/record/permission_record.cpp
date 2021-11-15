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

#include "permission_record.h"

namespace OHOS {
namespace Security {
namespace Permission {
int PermissionRecord::TranslationIntoPermissionRecord(
    const GenericValues &inGenericValues, PermissionRecord &outPermissionRecord)
{
    outPermissionRecord.timestamp = inGenericValues.GetInt(FIELD_TIMESTAMP);
    outPermissionRecord.visitorId = inGenericValues.GetInt(FIELD_VISITOR_ID);
    outPermissionRecord.opCode = inGenericValues.GetInt(FIELD_OP_CODE);
    outPermissionRecord.isForeground = inGenericValues.GetInt(FIELD_IS_FOREGROUND);
    outPermissionRecord.accessCount = inGenericValues.GetInt(FIELD_ACCESS_COUNT);
    outPermissionRecord.rejectCount = inGenericValues.GetInt(FIELD_REJECT_COUNT);
    return Constant::SUCCESS;
}

int PermissionRecord::TranslationIntoGenericValues(
    const PermissionRecord &inPermissionRecord, GenericValues &outGenericValues)
{
    outGenericValues.Put(FIELD_TIMESTAMP, inPermissionRecord.timestamp);
    outGenericValues.Put(FIELD_VISITOR_ID, inPermissionRecord.visitorId);
    outGenericValues.Put(FIELD_OP_CODE, inPermissionRecord.opCode);
    outGenericValues.Put(FIELD_IS_FOREGROUND, inPermissionRecord.isForeground);
    outGenericValues.Put(FIELD_ACCESS_COUNT, inPermissionRecord.accessCount);
    outGenericValues.Put(FIELD_REJECT_COUNT, inPermissionRecord.rejectCount);
    return Constant::SUCCESS;
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
