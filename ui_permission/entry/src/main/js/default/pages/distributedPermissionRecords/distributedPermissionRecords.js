export default {
    data: {
        loading_on:true,
        blankpage_on:false,
        description: '无访问记录',
        permissionList:[],
        permissionNameList:[
            {
                permissionName:'通话记录',
                permission:['ohos.permission.READ_CALL_LOG','ohos.permission.WRITE_CALL_LOG']
            },
            {
                permissionName:'电话',
                permission:['ohos.permission.PLACE_CALL']
            },
            {
                permissionName:'位置',
                permission:['ohos.permission.LOCATION','ohos.permission.LOCATION_IN_BACKGROUND']
            },
            {
                permissionName:'麦克风',
                permission:['ohos.permission.MICROPHONE']
            },
            {
                permissionName:'相机',
                permission:['ohos.permission.CAMERA']
            },
            {
                permissionName:'日历',
                permission:['ohos.permission.READ_CALENDAR','ohos.permission.WRITE_CALENDAR']
            },
            {
                permissionName:'媒体和文件',
                permission:['ohos.permission.WRITE_MEDIA','ohos.permission.READ_MEDIA','ohos.permission.MEDIA_LOCATION']
            },
            {
                permissionName:'健身运动',
                permission:['ohos.permission.ACTIVITY_MOTION']
            },
            {
                permissionName:'身体传感器',
                permission:['ohos.permission.READ_HEALTH_DATA']
            }
        ],
//        permission_list: [
//            {
//                permission_icon: "common/images/ic.png",
//                permission_name: '日历',
//                count: '6',
//                device:[
//                    {
//                        device_name:'Mate 30 Pro',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'智慧屏',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 40',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 20',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    }
//                ]
//            },
//            {
//                permission_icon: "common/images/ic.png",
//                permission_name: '通话记录',
//                count: '6',
//                device:[
//                    {
//                        device_name:'Mate 30 Pro',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'智慧屏',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 40',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 20',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    }
//                ]
//            },
//            {
//                permission_icon: "common/images/ic.png",
//                permission_name: '相机',
//                count: '6',
//                device:[
//                    {
//                        device_name:'Mate 30 Pro',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'智慧屏',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 40',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 20',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    }
//                ]
//            },
//            {
//                permission_icon: "common/images/ic.png",
//                permission_name: '读取联系人',
//                count: '6',
//                device:[
//                    {
//                        device_name:'Mate 30 Pro',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'智慧屏',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 40',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 20',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    }
//                ]
//            },
//            {
//                permission_icon: "common/images/ic.png",
//                permission_name: '位置信息',
//                count: '6',
//                device:[
//                    {
//                        device_name:'Mate 30 Pro',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'智慧屏',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 40',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 20',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    }
//                ]
//            },
//            {
//                permission_icon: "common/images/ic.png",
//                permission_name: '存储',
//                count: '6',
//                device:[
//                    {
//                        device_name:'Mate 30 Pro',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'智慧屏',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 40',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    },
//                    {
//                        device_name:'Mate 20',
//                        visit_count:'6',
//                        last_visit:'7月9日 下午2:27'
//                    }
//                ]
//            }
//        ],
        images_resource: {
            "image_icon": "common/images/ic.png",
            "image_back": "common/images/ic_back.png"
        }
    },
    onInit() {
        this.convertData();
        this.loading_on=false;
//        if(this.permissionList.length == 0) {
//            this.isData = false;
//            this.noData = true;
//        }
    },
    convertData() {
        var appRecordsList=[
            {
                code: 1,
                beginTimeMillis: 1628055613,
                endTimeMillis: 1628228414,
                bundlePermissionRecords: [
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
                                lastAccessTime:1628054620,
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
                                lastAccessTime:1628055673,
                                lastRejectTime:9
                            },
                            {
                                permissionName: 'ohos.permission.PLACE_CALL',
                                accessCountFg:1264,
                                rejectCountFg:2,
                                accessCountBg:3,
                                rejectCountBg:4,
                                lastAccessTime:1628055603,
                                lastRejectTime:9
                            },
                            {
                                permissionName: 'ohos.permission.MEDIA_LOCATION',
                                accessCountFg:1264,
                                rejectCountFg:2,
                                accessCountBg:3,
                                rejectCountBg:4,
                                lastAccessTime:1628056620,
                                lastRejectTime:9
                            }
                        ]
                    },
                    {
                        deviceId: '1002',
                        deviceLabel: '华为mate 40',
                        bundleName: 'bundlname1',
                        bundleLabel: '爱奇艺',
                        applicationIconId:'001',
                        permissionRecords:[
                            {
                                permissionName: 'ohos.permission.LOCATION',
                                accessCountFg:1264,
                                rejectCountFg:2,
                                accessCountBg:3,
                                rejectCountBg:4,
                                lastAccessTime:1628057613,
                                lastRejectTime:9
                            },
                            {
                                permissionName: 'ohos.permission.MEDIA_LOCATION',
                                accessCountFg:1264,
                                rejectCountFg:2,
                                accessCountBg:3,
                                rejectCountBg:4,
                                lastAccessTime:1628058620,
                                lastRejectTime:9
                            }
                        ]
                    }
                ]
            }];

        var bundlePermissionRecords = appRecordsList[0].bundlePermissionRecords;
        //generate permission list.
        // permissionNamelist
        var permissionObjectName='';
        var permissionObject=new Object();
        for(var i=0;i<this.permissionNameList.length;i++) {
            for(var j=0;j<this.permissionNameList[i].permission.length;j++) {
                var permission=this.permissionNameList[i].permission[j];
                var permissionName=this.permissionNameList[i].permissionName;
                //match the bundlePermissionRecords
                for (var bi = 0; bi < bundlePermissionRecords.length; bi++) {
                    var permissionRecords=bundlePermissionRecords[bi].permissionRecords;
                    for (var bj = 0; bj < permissionRecords.length; bj++) {
                        var bundlePermission=permissionRecords[bj].permissionName;
                        if((bundlePermission == permission) && permissionObjectName==permissionName) {
                            var deviceObject=new Object();
                            deviceObject.deviceName=bundlePermissionRecords[bi].deviceLabel;
                            deviceObject.accessCountFg=permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount+=deviceObject.accessCountFg;
                            deviceObject.timeStamp=permissionRecords[bj].lastAccessTime;
                            deviceObject.lastAccessTime=this.formateDate(deviceObject.timeStamp);
                            permissionObject.deviceRecords.push(deviceObject);
                            // sort by lastAccessTime
                            permissionObject.deviceRecords.sort(function(a,b){
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName =permissionName;
                        }else if(bundlePermission == permission) {
                            permissionObject=new Object();
                            permissionObject.permissionIcon="common/images/ic.png";
                            permissionObject.permissionName=permissionName;
                            permissionObject.accessCount=0;
                            permissionObject.deviceRecords=[];
                            var deviceObject=new Object();
                            deviceObject.deviceName=bundlePermissionRecords[bi].deviceLabel;
                            deviceObject.accessCountFg=permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount+=deviceObject.accessCountFg;
                            deviceObject.timeStamp=permissionRecords[bj].lastAccessTime;
                            deviceObject.lastAccessTime=this.formateDate(deviceObject.timeStamp);
                            permissionObject.deviceRecords.push(deviceObject);
                            // sort by lastAccessTime
                            permissionObject.deviceRecords.sort(function(a,b){
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName =permissionName;
                            this.permissionList.push(permissionObject);
                        }
                    }
                }
            }
        }

        //test permissionList
        for (var i =0;i<this.permissionList.length;i++) {
            console.info(this.permissionList[i].permissionIcon);
            console.info(this.permissionList[i].permissionName);
            console.info(this.permissionList[i].accessCount);
            for(var j=0;j<this.permissionList[i].deviceRecords.length;j++) {
                console.info(this.permissionList[i].deviceRecords[j].deviceName);
                console.info(this.permissionList[i].deviceRecords[j].accessCountFg);
                console.info(this.permissionList[i].deviceRecords[j].lastAccessTime);
            }
        }

        //whether show blank page.
        if(this.permissionList.length == 0) {
            this.blankpage_on = true;
        }
    },
// permission name match
    findPermission(permissionName) {
        for(var i=0;i<this.permissionNameList.length;i++) {
            for(var j=0;j<this.permissionNameList[i].permission.length;j++) {
                var permission=this.permissionNameList[i].permission[j];
                if(permissionName == permission) {
                    return this.permissionNameList[i].permissionName;
                }
            }
        }
    },
    formateDate(timeStamp) {
        if(timeStamp>1000000000000) {
            timeStamp=timeStamp+8*60*60*1000;
        }else {
            timeStamp=timeStamp*1000+8*60*60*1000;
        }

        var date = new Date(timeStamp);
        var month = (date.getMonth()+1 < 10 ? '0'+(date.getMonth()+1) : date.getMonth()+1)+'月';
        var day = (date.getDate() < 10 ? '0'+date.getDate() : date.getDate()) + '日  ';
        var hour = (date.getHours() < 10 ? '0'+date.getHours() : date.getHours()) + ':';
        var minute = (date.getMinutes() < 10 ? '0'+date.getMinutes() : date.getMinutes());

        if(month.substring(0,1)=='0') {
            month = month.substr(1,month.length);
        }
        if(day.substring(0,1)=='0') {
            day = day.substr(1,day.length);
        }
        if(hour.substring(0,1)=='0') {
            hour = hour.substr(1,hour.length);
        }
        if((parseInt(hour))>12) {
            console.info(hour);
            hour='下午'+(parseInt(hour)-12).toString()+':';
        }else {
            hour='上午'+hour;
        }
        let strDate = month+day+hour+minute;
        return strDate;
    },
    onReady()
    {

    },
    onShow()
    {
    },
    onDestroy() {

    },
    onclick(data) {
        if (data.detail.type == 'expandPermission' && data.detail.flag) {
            this.$element('deviceList').expandGroup({
                groupid: data.detail.text
            });
        } else if (data.detail.type == 'expandPermission' && !data.detail.flag) {
            this.$element('deviceList').collapseGroup({
                groupid: data.detail.text
            });
        }
    }
}