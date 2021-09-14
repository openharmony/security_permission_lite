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
import router from "@system.router";
import permissionRecordsConfigs from "../../common/configs/permissionRecordsConfigs.js";
import PinyinSort from "../../common/utils/PinyinSort.js";

export default {
    data: {
        permissionList: [],
        applicationList: [],
        applicationShowList: [],
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
        //generate application list.
        for (let i = 0; i < bundlePermissionRecords.length; i++) {
            let appObject = new Object();
            appObject.appName = bundlePermissionRecords[i].bundleLabel;
            appObject.bundleName = bundlePermissionRecords[i].bundleName;
            appObject.applicationIcon = "common/images/ic.png";
            appObject.permissionRecords = [];
            appObject.permissionIconList = [];
            let permissionInfo = bundlePermissionRecords[i].permissionRecords;
            for (let j = 0; j < permissionInfo.length; j++) {
                let permissionObject = new Object();
                permissionObject.permissionIcon = "common/images/ic.png"; //Todo: Accroding to permissionName to modify icon.
                appObject.permissionIconList.push(permissionObject.permissionIcon);
                permissionObject.permissionName = this.findPermission(permissionInfo[j].permissionName);
                permissionObject.accessCountFg = permissionInfo[j].accessCountFg;
                permissionObject.timeStamp = permissionInfo[j].lastAccessTime;
                permissionObject.lastAccessTime = this.formateDate(permissionObject.timeStamp);
                appObject.permissionRecords.push(permissionObject);
            }
            // sort by lastAccessTime
            appObject.permissionRecords.sort(function (a, b) {
                return b.timeStamp - a.timeStamp;
            });
            this.applicationList.push(appObject);
        }
        //generate permission list.
        // permission name list
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
                            let appObject = new Object();
                            appObject.applicationIcon = "common/images/ic.png";
                            appObject.applicationName = bundlePermissionRecords[bi].bundleLabel;
                            appObject.accessCountFg = permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount += appObject.accessCountFg;
                            appObject.timeStamp = permissionRecords[bj].lastAccessTime;
                            appObject.lastAccessTime = this.formateDate(appObject.timeStamp);
                            permissionObject.applicationRecords.push(appObject);
                            // sort by lastAccessTime
                            permissionObject.applicationRecords.sort(function (a, b) {
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName = permissionName;
                        } else if (bundlePermission == permission) {
                            permissionObject = new Object();
                            permissionObject.permissionIcon = "common/images/ic.png"; //Todo: Accroding to permissionName to modify icon.
                            permissionObject.permissionName = permissionName;
                            permissionObject.permission = permission;
                            permissionObject.accessCount = 0;
                            permissionObject.applicationRecords = [];
                            let appObject = new Object();
                            appObject.applicationIcon = "common/images/ic.png";
                            appObject.applicationName = bundlePermissionRecords[bi].bundleLabel;
                            appObject.accessCountFg = permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount += appObject.accessCountFg;
                            appObject.timeStamp = permissionRecords[bj].lastAccessTime;
                            appObject.lastAccessTime = this.formateDate(appObject.timeStamp);
                            permissionObject.applicationRecords.push(appObject);
                            // sort by lastAccessTime
                            permissionObject.applicationRecords.sort(function (a, b) {
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName = permissionName;
                            this.permissionList.push(permissionObject);
                        }
                    }
                }
            }
        }
        var pinyinSort = new PinyinSort();
        this.applicationList.sort(pinyinSort.sortByAppName.bind(pinyinSort));
        this.applicationShowList = this.applicationList.slice();
        this.permissionList = this.permissionList.slice();
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
            this.$element("permissionList").expandGroup({
                groupId: data.detail.text.permissionName
            })
        } else if (data.detail.type == "expandPermission" && !data.detail.flag) {
            this.$element("permissionList").collapseGroup({
                groupId: data.detail.text.permissionName
            })
        }

        if (data.detail.type == "expandApp" && data.detail.flag) {
            this.$element("applicationList").expandGroup({
                groupId: data.detail.text.appName
            })
        } else if (data.detail.type == "expandApp" && !data.detail.flag) {
            this.$element("applicationList").collapseGroup({
                groupId: data.detail.text.appName
            })
        }
        if (data.detail.type == "routerPermission") {
            router.push({
                uri: "pages/permissionGrant/permissionGrant", // link to permission grant page.
                params: {
                    title: data.detail.text.permissionName,
                    permission: data.detail.text.permission,
                }
            })
        }

        if (data.detail.type == "routerApp") {
            router.push({
                uri: "pages/appPermissionList/appPermissionList", //  link to app permission list page.
                params: {
                    appName: data.detail.text.appName,
                    bundleName: data.detail.text.bundleName,
                }
            })
        }
    },
}