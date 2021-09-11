import router from '@system.router';
import record from '@ohos.record';
import PinyinSort from '../../common/utils/PinyinSort.js';
import ResourceManager from '../../common/model/ResourceManager.js';
export default {
    data: {
        permissionList:[],
        applicationList:[],
        applicationShowList:[],
        bundlePermissionRecords:[],
        loadingOn: true,
        blankPageOn:false,
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
                permissionName:'媒体',
                permission:['ohos.permission.WRITE_MEDIA','ohos.permission.READ_MEDIA','ohos.permission.MEDIA_LOCATION']
            },
            {
                permissionName:'健身运动',
                permission:['ohos.permission.ACTIVITY_MOTION']
            },
            {
                permissionName:'健康',
                permission:['ohos.permission.READ_HEALTH_DATA']
            }
        ]
    },
    onInit() {
        console.info("init--------------------------------------------")
        record.getPermissionRecord({
            queryData: {
                deviceLabel: "",
                bundleName:"",
                permissionNames:[],
                beginTimeMillis: 1,
                endTimeMillis : 1234567890,
                flag : 2,
            }
        }).then((data) => {
            console.info('***********************getPermissionRecord Promise*******************************');
            var now_time =new Date();
            console.info("------------------------time = "+now_time);
            console.info("code:" + data.code);
            console.info("beginTimeMillis:" + data.beginTimeMillis);
            console.info("endTimeMillis:" + data.endTimeMillis);
            console.info('getPRecordsByUid Records length [' + data.bundlePermissionRecords.length + ']');

            for (var i = 0; i < data.bundlePermissionRecords.length; i++) {
                console.info("bundlePermissionRecords[" + i + "]deviceId:" + data.bundlePermissionRecords[i].deviceId);
                console.info("bundlePermissionRecords[" + i + "]deviceLabel:" + data.bundlePermissionRecords[i].deviceLabel);
                console.info("bundlePermissionRecords[" + i + "]bundleName:" + data.bundlePermissionRecords[i].bundleName);
                console.info("bundlePermissionRecords[" + i + "]bundleLabel:" + data.bundlePermissionRecords[i].bundleLabel);
                console.info("bundlePermissionRecords[" + i + "]iconId:" + data.bundlePermissionRecords[i].applicationIconId);
                console.info("bundlePermissionRecords[" + i + "]labelId:" + data.bundlePermissionRecords[i].labelId);
                console.info('permissionRecords length [' + data.bundlePermissionRecords[i].permissionRecords.length + ']');
                        for(var j = 0; j < data.bundlePermissionRecords[i].permissionRecords.length; j++){
                    console.info("["+i+"]permissionRecords[" + j + "]permissionName:" + data.bundlePermissionRecords[i].permissionRecords[j].permissionName);
                    console.info("["+i+"]permissionRecords[" + j + "]lastAccessTime:" + data.bundlePermissionRecords[i].permissionRecords[j].lastAccessTime);
                    console.info("["+i+"]permissionRecords[" + j + "]lastRejectTime:" + data.bundlePermissionRecords[i].permissionRecords[j].lastRejectTime);
                    console.info("["+i+"]permissionRecords[" + j + "]accessCountFg:" + data.bundlePermissionRecords[i].permissionRecords[j].accessCountFg);
                    console.info("["+i+"]permissionRecords[" + j + "]rejectCountFg:" + data.bundlePermissionRecords[i].permissionRecords[j].rejectCountFg);
                    console.info("["+i+"]permissionRecords[" + j + "]accessCountBg:" + data.bundlePermissionRecords[i].permissionRecords[j].accessCountBg);
                    console.info("["+i+"]permissionRecords[" + j + "]rejectCountBg:" + data.bundlePermissionRecords[i].permissionRecords[j].rejectCountBg);
                }
            }
            this.bundlePermissionRecords=data.bundlePermissionRecords;
            this.convertData();
        });
//        setTimeout(()=>{
//            this.convertData();
//        },2000);
    },
    onReady() {},
    onShow() {},
    async convertData() {
//        this.bundlePermissionRecords=[
//                {
//                    deviceId: '1001',
//                    deviceLabel: '华为pro30',
//                    bundleName: 'bundlname1',
//                    bundleLabel: '微信',
//                    applicationIconId:'001',
//                    permissionRecords:[
//                        {
//                            permissionName: 'ohos.permission.WRITE_CALL_LOG',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1611149048,
//                            lastRejectTime:9
//                        },
//                        {
//                            permissionName: 'ohos.permission.MEDIA_LOCATION',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1638643518,
//                            lastRejectTime:9
//                        },
//                        {
//                            permissionName: 'ohos.permission.PLACE_CALL',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1629642518,
//                            lastRejectTime:9
//                        }
//                    ]
//                },
//                {
//                    deviceId: '1002',
//                    deviceLabel: '华为mate 40',
//                    bundleName: 'bundlname1',
//                    bundleLabel: '华为音乐',
//                    applicationIconId:'001',
//                    permissionRecords:[
//                        {
//                            permissionName: 'ohos.permission.LOCATION',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1628543627,
//                            lastRejectTime:9
//                        },
//                        {
//                            permissionName: 'ohos.permission.PLACE_CALL',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1628672519,
//                            lastRejectTime:9
//                        },
//                        {
//                            permissionName: 'ohos.permission.MEDIA_LOCATION',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1628642018,
//                            lastRejectTime:9
//                        }
//                    ]
//                },
//                {
//                    deviceId: '1002',
//                    deviceLabel: '华为mate 40',
//                    bundleName: 'bundlname1',
//                    bundleLabel: '爱奇艺',
//                    applicationIconId:'001',
//                    permissionRecords:[
//                        {
//                            permissionName: 'ohos.permission.LOCATION',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1628643518,
//                            lastRejectTime:9
//                        },
//                        {
//                            permissionName: 'ohos.permission.MEDIA_LOCATION',
//                            accessCountFg:1264,
//                            rejectCountFg:2,
//                            accessCountBg:3,
//                            rejectCountBg:4,
//                            lastAccessTime:1628672518,
//                            lastRejectTime:9
//                        }
//                    ]
//                }
//            ]；
        console.info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>convertData>>>>>>>>>>>>>>>>>>>>>>>>>>");
        var bundlePermissionRecords = this.bundlePermissionRecords;
        console.info("bundlePermissionRecords.length:"+bundlePermissionRecords.length);
        //generate application list.
        for (var i = 0; i < bundlePermissionRecords.length; i++) {
            var appObject=new Object();
            var resourceManager = new ResourceManager();//Todo:调试时打开注释
            appObject.appName = await resourceManager.getAppName(bundlePermissionRecords[i].labelId, bundlePermissionRecords[i].bundleName, bundlePermissionRecords[i].bundleLabel);
            appObject.applicationIcon= await resourceManager.getAppIcon(bundlePermissionRecords[i].applicationIconId, bundlePermissionRecords[i].bundleName);
            appObject.permissionRecords=[];
            appObject.permissionIconList=[];
            var permissionInfo=bundlePermissionRecords[i].permissionRecords;
            for (var j = 0; j < permissionInfo.length; j++) {
                var permissionObject=new Object();
                permissionObject.permissionIcon="common/images/ic.png";//Todo: Accroding to permissionName to modify icon.
                appObject.permissionIconList.push(permissionObject.permissionIcon);
                permissionObject.permissionName=this.findPermission(permissionInfo[j].permissionName);
                permissionObject.accessCountFg=permissionInfo[j].accessCountFg;
                permissionObject.timeStamp=permissionInfo[j].lastAccessTime;
                permissionObject.lastAccessTime=this.formateDate(permissionObject.timeStamp);
                appObject.permissionRecords.push(permissionObject);
            }
            // sort by lastAccessTime
            appObject.permissionRecords.sort(function(a,b){
                return b.timeStamp - a.timeStamp;
            });
            this.applicationList.push(appObject);
        }

        console.info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>applicationList>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        //test applicationList
        for (var i =0;i<this.applicationList.length;i++) {
            console.info(this.applicationList[i].appName);
            console.info(this.applicationList[i].applicationIcon);//Todo:调试时打开注释
            console.info(this.applicationList[i].permissionIconList);
            for(var j=0;j<this.applicationList[i].permissionRecords.length;j++) {
                console.info(this.applicationList[i].permissionRecords[j].permissionIcon);
                console.info(this.applicationList[i].permissionRecords[j].permissionName);
                console.info(this.applicationList[i].permissionRecords[j].accessCountFg);
                console.info(this.applicationList[i].permissionRecords[j].lastAccessTime);
            }
        }

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
                            var appObject=new Object();
                            var resourceManager = new ResourceManager();//Todo:调试时打开注释
                            appObject.applicationIcon= await resourceManager.getAppIcon(bundlePermissionRecords[bi].applicationIconId, bundlePermissionRecords[bi].bundleName);
                            appObject.applicationName = await resourceManager.getAppName(bundlePermissionRecords[bi].labelId, bundlePermissionRecords[bi].bundleName, bundlePermissionRecords[bi].bundleLabel);
                            appObject.accessCountFg=permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount+=appObject.accessCountFg;
                            appObject.timeStamp=permissionRecords[bj].lastAccessTime;
                            appObject.lastAccessTime=this.formateDate(appObject.timeStamp);
                            permissionObject.applicationRecords.push(appObject);
                            // sort by lastAccessTime
                            permissionObject.applicationRecords.sort(function(a,b){
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName =permissionName;
                        }else if(bundlePermission == permission) {
                            permissionObject=new Object();
                            permissionObject.permissionIcon="common/images/ic.png";
                            permissionObject.permissionName=permissionName;
                            permissionObject.accessCount=0;
                            permissionObject.applicationRecords=[];
                            var appObject=new Object();
                            var resourceManager = new ResourceManager();//Todo:调试时打开注释
                            appObject.applicationIcon=await resourceManager.getAppIcon(bundlePermissionRecords[bi].applicationIconId, bundlePermissionRecords[bi].bundleName);
                            appObject.applicationName = await resourceManager.getAppName(bundlePermissionRecords[bi].labelId, bundlePermissionRecords[bi].bundleName, bundlePermissionRecords[bi].bundleLabel);
                            appObject.accessCountFg=permissionRecords[bj].accessCountFg;
                            permissionObject.accessCount+=appObject.accessCountFg;
                            appObject.timeStamp=permissionRecords[bj].lastAccessTime;
                            appObject.lastAccessTime=this.formateDate(appObject.timeStamp);
                            permissionObject.applicationRecords.push(appObject);
                            // sort by lastAccessTime
                            permissionObject.applicationRecords.sort(function(a,b){
                                return b.timeStamp - a.timeStamp;
                            });
                            permissionObjectName =permissionName;
                            this.permissionList.push(permissionObject);
                        }
                    }
                }
            }
        }
        console.info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>permissionList>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
        //test permissionList
        for (var i =0;i<this.permissionList.length;i++) {
            console.info(this.permissionList[i].permissionIcon);
            console.info(this.permissionList[i].permissionName);
            console.info(this.permissionList[i].accessCount);
            for(var j=0;j<this.permissionList[i].applicationRecords.length;j++) {
                console.info(this.permissionList[i].applicationRecords[j].applicationIcon);
                console.info(this.permissionList[i].applicationRecords[j].applicationName);
                console.info(this.permissionList[i].applicationRecords[j].accessCountFg);
                console.info(this.permissionList[i].applicationRecords[j].lastAccessTime);
            }
        }

        var pinyinSort = new PinyinSort();
        this.applicationList.sort(pinyinSort.sortByAppName.bind(pinyinSort));
        this.applicationShowList = this.applicationList.slice();
        this.permissionList=this.permissionList.slice();
        this.loadingOn = false;
        //whether show blank page.
        if (this.permissionList.length==0 || this.applicationList.length == 0){
            this.blankPageOn=true;
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
    onDestroy() {},
    onclick(data) {
        if (data.detail.type == 'expandPermission' && data.detail.flag) {
            this.$element('permissionlist').expandGroup({
                groupid: data.detail.text
            })
        } else if (data.detail.type == 'expandPermission' && !data.detail.flag) {
            this.$element('permissionlist').collapseGroup({
                groupid: data.detail.text
            })
        }
        if (data.detail.type == 'expandApp' && data.detail.flag) {
            console.info(1)
            this.$element('applicationlist').expandGroup({
                groupid: data.detail.text
            })
        } else if (data.detail.type == 'expandApp' && !data.detail.flag) {
            console.info(2)
            this.$element('applicationlist').collapseGroup({
                groupid: data.detail.text
            })
        }

        if (data.detail.type == 'routerPermission') {
            router.push({
                uri: "pages/permissionGrant/permissionGrant", //  指定要跳转的页面
            })
        }

        if (data.detail.type == 'routerApp') {
            router.push({
                uri: "pages/appPermissionList/appPermissionList", //  指定要跳转的页面
            })
        }
    }
}
