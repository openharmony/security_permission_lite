/**
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
import permissionRecordsConfigs from "../../common/configs/permissionRecordsConfigs.js";

export default {
    data: {
        permissionList: [],
        bundlePermissionRecords: [],
        permissionNameList: [],
        loadingOn: true,
        blankPageOn: false,
    },
    onInit() {
        this.permissionNameList = permissionRecordsConfigs.permissionNameList;
        this.bundlePermissionRecords = [
            {
                deviceId: '1001',
                deviceLabel: '华为pro30',
                bundleName: 'bundlname1',
                bundleLabel: '微信',
                applicationIconId:'001',
                permissionRecords:[
                    {
                        permissionName: 'ohos.permission.WRITE_CALL_LOG',
                        accessCountFg:1264,
                        rejectCountFg:2,
                        accessCountBg:3,
                        rejectCountBg:4,
                        lastAccessTime:1628055610,
                        lastRejectTime:9
                    },
                    {
                        permissionName: 'ohos.permission.MEDIA_LOCATION',
                        accessCountFg:1264,
                        rejectCountFg:2,
                        accessCountBg:3,
                        rejectCountBg:4,
                        lastAccessTime:1628055620,
                        lastRejectTime:9
                    },
                    {
                        permissionName: 'ohos.permission.PLACE_CALL',
                        accessCountFg:1264,
                        rejectCountFg:2,
                        accessCountBg:3,
                        rejectCountBg:4,
                        lastAccessTime:1628055613,
                        lastRejectTime:9
                    }
                ]
            },
            {
                deviceId: '1002',
                deviceLabel: '华为mate 40',
                bundleName: 'bundlname1',
                bundleLabel: '华为音乐',
                applicationIconId:'001',
                permissionRecords:[
                    {
                        permissionName: 'ohos.permission.LOCATION',
                        accessCountFg:1264,
                        rejectCountFg:2,
                        accessCountBg:3,
                        rejectCountBg:4,
                        lastAccessTime:1628055613,
                        lastRejectTime:9
                    },
                    {
                        permissionName: 'ohos.permission.PLACE_CALL',
                        accessCountFg:1264,
                        rejectCountFg:2,
                        accessCountBg:3,
                        rejectCountBg:4,
                        lastAccessTime:1628055613,
                        lastRejectTime:9
                    },
                    {
                        permissionName: 'ohos.permission.MEDIA_LOCATION',
                        accessCountFg:1264,
                        rejectCountFg:2,
                        accessCountBg:3,
                        rejectCountBg:4,
                        lastAccessTime:1628055620,
                        lastRejectTime:9
                    }
                ]
            }
        ];
        if (this.bundlePermissionRecords.length == 0) {
            this.loadingOn = false;
            this.blankPageOn = true;
        }
        else {
            this.convertData();
        }
    },
    convertData() {
        var bundlePermissionRecords = this.bundlePermissionRecords;
        //generate permission list.
        // permissionNamelist
        var permissionObjectName = "";
        var permissionObject = new Object();
        for (let i = 0;i < this.permissionNameList.length; i++) {
            for (let j = 0;j < this.permissionNameList[i].permission.length; j++) {
                let permission = this.permissionNameList[i].permission[j];
                let permissionName = this.permissionNameList[i].permissionName;
                //match the bundlePermissionRecords
                for (let bi = 0; bi < bundlePermissionRecords.length; bi++) {
                    let permissionRecords = bundlePermissionRecords[bi].permissionRecords;
                    for (let bj = 0; bj < permissionRecords.length; bj++) {
                        let bundlePermission = permissionRecords[bj].permissionName;
                        if ((bundlePermission == permission) && permissionObjectName == permissionName) {
                            let deviceObject = new Object();
                            deviceObject.deviceName = bundlePermissionRecords[bi].deviceLabel;
                            deviceObject.accessCountFg = permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount += deviceObject.accessCountFg;
                            deviceObject.timeStamp = permissionRecords[bj].lastAccessTime;
                            deviceObject.lastAccessTime = this.formateDate(deviceObject.timeStamp);
                            permissionObject.deviceRecords.push(deviceObject);
                            // sort by lastAccessTime
                            permissionObject.deviceRecords.sort(function (a, b) {
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName = permissionName;
                        } else if (bundlePermission == permission) {
                            permissionObject = new Object();
                            permissionObject.permissionIcon = "common/images/ic.png"; //Todo: Accroding to permissionName to modify icon.
                            permissionObject.permissionName = permissionName;
                            permissionObject.accessCount = 0;
                            permissionObject.deviceRecords = [];
                            let deviceObject = new Object();
                            deviceObject.deviceName = bundlePermissionRecords[bi].deviceLabel;
                            ;
                            deviceObject.accessCountFg = permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount += deviceObject.accessCountFg;
                            deviceObject.timeStamp = permissionRecords[bj].lastAccessTime;
                            deviceObject.lastAccessTime = this.formateDate(deviceObject.timeStamp);
                            permissionObject.deviceRecords.push(deviceObject);
                            // sort by lastAccessTime
                            permissionObject.deviceRecords.sort(function (a, b) {
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName = permissionName;
                            this.permissionList.push(permissionObject);
                        }
                    }
                }
            }
        }
        this.loadingOn = false;
    },
// permission name match
    findPermission(permissionName) {
        for (let i = 0;i < this.permissionNameList.length; i++) {
            for (let j = 0;j < this.permissionNameList[i].permission.length; j++) {
                let permission = this.permissionNameList[i].permission[j];
                if (permissionName == permission) {
                    return this.permissionNameList[i].permissionName;
                }
            }
        }
    },
    formateDate(timeStamp) {
        timeStamp = timeStamp * 1000 + 8 * 60 * 60 * 1000;
        let date = new Date(timeStamp);
        let month = (date.getMonth() + 1 < 10 ? "" + (date.getMonth() + 1) : date.getMonth() + 1) + this.$t("records.month");
        let day = (date.getDate() < 10 ? "" + date.getDate() : date.getDate()) + this.$t("records.date");
        let hour = (date.getHours() < 10 ? "" + date.getHours() : date.getHours()) + ":";
        let minute = (date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes());
        if ((parseInt(hour)) > 12) {
            hour = this.$t("records.afternoon") + (parseInt(hour) - 12).toString() + ":";
        } else {
            hour = this.$t("records.morning") + hour;
        }
        let strDate = month + day + hour + minute;
        return strDate;
    },
    onclick(data) {
        if (data.detail.type == "expandPermission" && data.detail.flag) {
            this.$element("deviceList").expandGroup({
                groupId: data.detail.text.permissionName
            });
        } else if (data.detail.type == "expandPermission" && !data.detail.flag) {
            this.$element("deviceList").collapseGroup({
                groupId: data.detail.text.permissionName
            });
        }
    }
}