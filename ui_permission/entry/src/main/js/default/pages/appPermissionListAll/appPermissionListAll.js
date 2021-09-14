// @ts-nocheck
import router from '@system.router';
//import bundle from '@ohos.bundle';
import permissionGroupConfig from '../../common/configs/permissionGroupConfigs.js';

export default {
    data: {
        loadingOn:true,
        permissionGroup: {},
        permissionSwitch: [],
        handlerPermissionAllList: [],
        permissionAllChildrenList: [],
        dialogTitleText: "",
        dialogMsgText: "",
        dialogConfirmText: ""
    },
    onInit() {
        this.permissionGroup = permissionGroupConfig.permissionGroup;
        this.permissionSwitch = permissionGroupConfig.permissionSwitch;
        this.handlerPermissionList();
        this.getPermissionAllChildrenList();

    },
    async handlerPermissionList() {
        let permissionAllList = this.permissionAllList;
        let handlerPermissionAllListResult = [];
        let otherPermissionList = [];
        for (let i = 0;i < permissionAllList.length; i++) {
            let permissionName = permissionAllList[i];
            let permissionType = this.checkPermissionGroup(permissionName);
            //1是标题处理
            if (permissionType == 1) {
                //处理标题后，要循环处理子项，userHandler的给
                let permissionResult = {};
//                permissionResult.permissionTitle = this.$t('permissions.' + permissionName);
                permissionResult.permissionTitle = this.$t('permissions')[permissionName] ;
                //大标题传-1
                permissionResult.permissionType = -1;
                handlerPermissionAllListResult.push(permissionResult);
                handlerPermissionAllListResult = handlerPermissionAllListResult.concat(this.getChildrenPermission(permissionName));
            }
            //如果未收录，暂存在数组里，最后统一放进其他
            if (permissionType == 0) {
                otherPermissionList.push(permissionName);
            }
        }
        //处理其他项目
        let permissionResultOther = {};
        let permissionsObj = this.$t('permissions');
        permissionResultOther.permissionTitle = permissionsObj['other'];
        permissionResultOther.permissionType = -1;
        handlerPermissionAllListResult.push(permissionResultOther);
        for (let i = 0;i < otherPermissionList.length; i++) {
            let permissionName = otherPermissionList[i];
            let permissionResult = {};
            permissionResult.permissionTitle = (permissionsObj[permissionName] == undefined || permissionsObj[permissionName] == "") ? permissionName : permissionsObj[permissionName];
            //userGrant是switch
            if (this.checkIsSwitch(permissionName)) {
                permissionResult.permissionValue = "";
                permissionResult.permissionType = 2;
                permissionResult.permissionDefaultValue = this.checkPermission(i);
            } else {
                permissionResult.permissionValue = "";
                permissionResult.permissionType = 1;
                permissionResult.permissionArrow = "/res/image/ic_permissions_arrow.png";
            }
            handlerPermissionAllListResult.push(permissionResult);
        }
        //0805req权限中包含了def权限？去掉
        //        //自定义权限也算其他
        //        let permissionAllDefList = this.permissionAllDefList;
        //        for (let i = 0;i < permissionAllDefList.length; i++) {
        //            let permissionName = permissionAllDefList[i];
        //            let permissionResult = {};
        //            permissionResult.permissionTitle = permissionName;
        //            //userGrant是switch
        //            permissionResult.permissionValue = "";
        //            permissionResult.permissionType = 1;
        //            //TODO 弹窗内容获取
        //            permissionResult.permissionArrow = "/res/image/ic_permissions_arrow.png";
        //            permissionResult.permissionAlias = this.permissionAlias;
        //            handlerPermissionAllListResult.push(permissionResult);
        //
        //        }
        this.handlerPermissionAllList = handlerPermissionAllListResult;
        this.loadingOn=false;

    },
    async getPermissionDef(permissionName) {
        let result = {};
//        await bundle.getPermissionDef(permissionName).then((data) => {
//            result = data;
//        });
        return result;
    },
    checkPermission(index) {
        if (index % 2 == 1) {
            return true;
        }
        return false;
    },
    async onclick(data) {
        let itmeJson = data.detail.text;
        if (itmeJson.permissionType == "1") {
            let permissionName = itmeJson.permissionTitle;
            this.dialogTitleText = permissionName;
            //TODO 应取得discription 然后去getResource，但是现在没这字段
            this.dialogMsgText = await this.getPermissionDef(permissionName)["name"];
            this.dialogConfirmText = this.$t('permissionCommon.confirm');
            this.$child('permissionDialog').showDialog();
        }
    },
    switchClick(data) {
        this.switchChangeValue = data.detail.text;
        this.switchDefaultChangeValue = data.detail.defaultText;
    },
    getChildrenPermission(permissionName) {
        let childrenPermissionArr = this.permissionGroup[permissionName];
        let permissionsObj = this.$t('permissions');
        let resultArr = [];
        let permissionAllList = this.permissionAllList;
        if (childrenPermissionArr.length > 0) {
            let resultKey = 0;
            for (let i = 0;i < childrenPermissionArr.length; i++) {
                let permissionResult = {};
                for (let j = 0;j < permissionAllList.length; j++) {
                    //每循环到一个列表，应该作为其子项校验userGrant确认开关和弹窗后，加入
                    if (permissionAllList[j] == childrenPermissionArr[i]) {
                        permissionResult.permissionTitle = permissionsObj[permissionAllList[j]];
                        //userGrant是switch
                        if (this.checkIsSwitch(permissionAllList[j])) {
                            permissionResult.permissionValue = "";
                            permissionResult.permissionType = 2;
                            permissionResult.permissionDefaultValue = this.checkPermission(i);
                        } else {
                            permissionResult.permissionType = 1;
                            permissionResult.permissionValue = "";
                            permissionResult.permissionArrow = "/res/image/ic_permissions_arrow.png";
                        }
                        resultArr.push(permissionResult);
                        resultKey++;
                    }
                }
            }
        }
        return resultArr;

    },
    checkIsSwitch(permissionName) {
        if (this.permissionSwitch.indexOf(permissionName) >= 0) {
            return true;
        }
        return false;
    },
//如果0，代表未收录，是标题，无子权限 ；1标题，有子权限；2不是标题
    checkPermissionGroup(permissionName) {
        let childrenPermissionArr = this.permissionGroup[permissionName] == undefined ? [] : this.permissionGroup[permissionName]
        let allChildrenPermissionsArr = this.getPermissionAllChildrenList();
        //是子权限，肯定不是标题
        if (allChildrenPermissionsArr.indexOf(permissionName) != -1) {
            return 2;
        }
        //如果是未收录，那么是其他里的
        if (childrenPermissionArr.length == 0) {
            return 0;
        }
        //其他情况，是key，是那种title的标题
        return 1;

    },
//获取权限group所有子类权限并缓存
    getPermissionAllChildrenList() {
        if (this.permissionAllChildrenList.length > 0) {
            return this.permissionAllChildrenList;
        }
        let resultArr = [];
        let permissionJson = this.permissionGroup;
        for (let permission in permissionJson) {
            let onePermissionChildren = permissionJson[permission];
            resultArr = resultArr.concat(onePermissionChildren);
        }
        this.permissionAllChildrenList = resultArr;
        return resultArr;
    }
}
