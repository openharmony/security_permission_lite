// @ts-nocheck
import router from '@system.router';
//import bundle from '@ohos.bundle';
import permissionGroupConfig from '../../common/configs/permissionGroupConfigs.js';

export default {
    data: {
        loadingOn: true,
        permissionGroup: {},
        permissionGroupOrder: [],
        permissionList: [], //基础数据
        permissionAllList: [], //处理后加入标题的数据
        permissionAllDefList: [], //自定义的数据
        permissionListAllow: [],
        bundleName: "com.system.hiworld.uitest",
        permissionListForbid: [
            //                        {
            //                            "permissionIcon": "/res/image/ic_permissions_display.png",
            //                            "permissionTitle": "1",
            //                            "permissionAlias": "brightnessTab",
            //                            "permissionValue": "",
            //                            "permissionArrow": "/res/image/ic_permissions_arrow.png",
            //                            "permissionDefaultValue": false,
            //                            "permissionSummary": "11111111111111111111111111111111111",
            //                            "permissionShowDivider": true,
            //                            "permissionType": 1,
            //                            "permissionUri": "pages/index/index"
            //                        }
        ]
    },
    onInit() {
        this.permissionGroup = permissionGroupConfig.permissionGroup;
        this.permissionGroupOrder = permissionGroupConfig.appPermissionOrder;
        this.permissionSwitch = permissionGroupConfig.permissionSwitch;
        this.getAppBundleInfo();
        this.loadingOn = false;
    },
    getAppBundleInfo() {
//        bundle.getBundleInfo(this.bundleName, 0).then((data) => {
//            this.permissionList = data.reqPermissions;
//            let permissionAllDefList = ["permissions_def_1", "permissions_def_2"];
//            this.permissionAllDefList = permissionAllDefList;
//            this.handlerPermissionList(data.reqPermissions, permissionAllDefList);
//        });
        //        let permissionList = ["permissions_2", "permissions_3", "permissions_5", "permissions_6", "permissions_7", "permissions_12", "permissions_13", "permissions_14", "permissions_15", "permissions_16", "permissions_17", "permissions_18", "permissions_20"];
        //        let permissionAllDefList = ["permissions_def_1", "permissions_def_2"];
        //        this.handlerPermissionList(permissionList, permissionAllDefList);
    },
    handlerPermissionList(permissionList, permissionAllDefList) {
        let permissionTitleList = this.getOrderPermissionTitleList(permissionList);
        let permissionListAllowResult = [];
        let permissionListForbid = [];
        for (let i = 0;i < permissionTitleList.length; i++) {
            let permissionResult = {};
            let permissionName = permissionTitleList[i];
            //TODO是UserGrant,才显示。未来替换参数  现在怀疑只要是权限组标题，并且子项存在就显示，usergrant非判断依据
            //permissionResult.permissionIcon = "/res/image/ic_" + permissionName + ".png";
            //TODO 替换图片
            permissionResult.permissionIcon = "/res/image/ic_permissions_about.png";
            //            permissionResult.permissionTitle = this.$t('permissions.' + permissionName);
            permissionResult.permissionTitle = this.$t('permissions')[permissionName];
            permissionResult.permissionArrow = "/res/image/ic_permissions_arrow.png";
            permissionResult.permissionType = 1;
            permissionResult.permissionValue = "";
            permissionResult.permissionUri = "pages/appPermissionListAll/appPermissionListAll";
            permissionResult.permissionAlias = permissionName;
            //获取子权限内容
            let childrenPermissionArr = this.getChildrenPermissionArr(permissionName);
            let childrenPermissionStr = this.getChildrenPermissionStr(childrenPermissionArr);
            permissionResult.permissionSummary = childrenPermissionStr;
            if (this.checkPermission(childrenPermissionArr)) {
                permissionListAllowResult.push(permissionResult);
            } else {
                permissionListForbid.push(permissionResult);
            }
        }
        //处理自定义权限，只要有一个是允许，就允许
        if (permissionAllDefList.length > 0) {
            let permissionResult = {};
            permissionResult.permissionIcon = "/res/image/ic_permissions_about.png";
            permissionResult.permissionTitle = this.$t('permissionCommon.otherPermissions');
            permissionResult.permissionSummary = this.$t('permissionCommon.other') + permissionAllDefList.length + this.$t('permissionCommon.item');
            permissionResult.permissionArrow = "/res/image/ic_permissions_arrow.png";
            permissionResult.permissionType = 1;
            permissionResult.permissionValue = "";
            permissionResult.permissionAlias = "";
            //TODO 页面待定
            permissionResult.permissionUri = "pages/appPermissionListAll/appPermissionListAll";
            let flag = false;
            for (let i = 0;i < permissionAllDefList.length; i++) {
                if (this.checkPermission(i)) {
                    flag = true;
                    break;
                }
            }
            if (flag) {
                permissionListAllowResult.push(permissionResult);
            } else {
                permissionListForbid.push(permissionResult);
            }

        }
        this.permissionListAllow = permissionListAllowResult;
        this.permissionListForbid = permissionListForbid;
    },
    checkPermission(childrenPermissionArr) {
        for (let i = 0;i < childrenPermissionArr.length; i++) {
            if (childrenPermissionArr[i]) {
                return true;
            }
        }

        return false;
    },
    onclick(data) {
        if(data.detail.text.permissionAlias==""){
            router.push({
                uri: 'pages/appPermissionDef/appPermissionDef',
                params: {
                    title:this.appName,
                    bundleName: this.bundleName,
                    permissionDefList:this.permissionAllDefList
                }
            });
        }else{
            router.push({
                uri: 'pages/appGrant/appGrant',
                params: {
                    title: data.detail.text.permissionTitle,
                    bundleName: this.bundleName,
                    permission: data.detail.text.permissionAlias
                }
            });
        }

    },
    toShowAllPermission() {
        router.push({
            uri: "pages/appPermissionListAll/appPermissionListAll",
            params: {
                permissionAllList: this.permissionAllList,
                permissionAllDefList: this.permissionAllDefList,
                bundleName: this.bundleName,
                appName: this.appName
            }
        });
    },
    switchClick(data) {
        this.switchChangeValue = data.detail.text,
        this.switchDefaultChangeValue = data.detail.defaultText
    },
//获取bundle中，其中一个权限的，req列表中存在的子权限
    getChildrenPermissionArr(permissionName) {
        let childrenPermissionArr = this.permissionGroup[permissionName];
        let permissionList = this.permissionList;
        let result = [];
        if (childrenPermissionArr.length > 0) {
            let resultKey = 0;
            for (let i = 0;i < childrenPermissionArr.length; i++) {
                for (let j = 0;j < permissionList.length; j++) {
                    if (permissionList[j] == childrenPermissionArr[i]&&this.checkIsSwitch(permissionList[j])) {
                        result.push(permissionList[j]);
                    }
                }
            }
        }
        return result;
    },
    getChildrenPermissionStr(permissionArr) {
        let resultStr = "";
        let permissionsObj = this.$t('permissions');
        for (let i = 0;i < permissionArr.length; i++) {
            if (i != 0) {
                resultStr += permissionsObj.and;
            }
            resultStr += permissionsObj[permissionArr[i]];
        }
        return resultStr;
    },
    getOrderPermissionTitleList(permissionList) {
        let permissionAllTitle = this.permissionGroup;
        let titleSet = new Set();
        //如果权限组子项出现在req里了，讲这个权限组标题权限加入
        for (var key in permissionAllTitle) {
            let childrenPermissionArr = permissionAllTitle[key];
            for (let i = 0;i < childrenPermissionArr.length; i++) {
                for (let j = 0;j < permissionList.length; j++) {
                    if (permissionList[j] == childrenPermissionArr[i]) {
                        titleSet.add(key);
                    }
                }
            }
        }
        let appPermissionOrder = this.permissionGroupOrder;
        let orderPermissionTitle = [];
        for (let i = 0;i < appPermissionOrder.length; i++) {
            if (titleSet.has(appPermissionOrder[i])) {
                orderPermissionTitle.push(appPermissionOrder[i]);
            }
        }
        let permissionAllList = orderPermissionTitle.concat(permissionList);
        this.permissionAllList = permissionAllList;
        return orderPermissionTitle;
    },
    checkIsSwitch(permissionName){
        if (this.permissionSwitch.indexOf(permissionName) >= 0) {
            return true;
        }
        return false;
    }
}
