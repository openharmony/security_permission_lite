/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
const PermissionRecordsConfigs = {
    permissionNameList: [
        {
            permissionName: "通话记录",
            permission: ["ohos.permission.READ_CALL_LOG", "ohos.permission.WRITE_CALL_LOG"]
        },
        {
            permissionName: "电话",
            permission: ["ohos.permission.PLACE_CALL"]
        },
        {
            permissionName: "位置",
            permission: ["ohos.permission.LOCATION", "ohos.permission.LOCATION_IN_BACKGROUND"]
        },
        {
            permissionName: "麦克风",
            permission: ["ohos.permission.MICROPHONE"]
        },
        {
            permissionName: "相机",
            permission: ["ohos.permission.CAMERA"]
        },
        {
            permissionName: "日历",
            permission: ["ohos.permission.READ_CALENDAR", "ohos.permission.WRITE_CALENDAR"]
        },
        {
            permissionName: "媒体",
            permission: ["ohos.permission.WRITE_MEDIA", "ohos.permission.READ_MEDIA", "ohos.permission.MEDIA_LOCATION"]
        },
        {
            permissionName: "健身运动",
            permission: ["ohos.permission.ACTIVITY_MOTION"]
        },
        {
            permissionName: "健康",
            permission: ["ohos.permission.READ_HEALTH_DATA"]
        }
    ]
}

export default PermissionRecordsConfigs;