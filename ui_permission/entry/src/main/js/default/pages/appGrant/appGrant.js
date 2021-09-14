//import bundle from "@ohos.bundle"
//import ResourceManager from '../../../common/model/ResourceManager.js';
export default {
    data: {
        title: "",
        icon: "",
        appName: "",
        versionName: "",
        bundleName: "",
        permission: "",
        reason: "",
        radioList: [
            {
                "title": "允许",
                "isChecked": true,
                "showDivider": true
            },
            {
                "title": "禁止",
                "isChecked": false,
                "showDivider": false
            }
        ]
    },
    onInit() {
        //temp
        this.appName = "58同城";
        this.icon = "/res/image/ic_permissions_display.png";
        this.reason = "58同城使用麦克风reason"
        this.versionName = "8.22.8";

        this.title = "麦克风";
        this.bundleName = "";//TODO
        this.permission = "";//TODO
//        bundle.getBundleInfo(this.bundleName, 1).then((data) => {
//            var resourceManager = new ResourceManager();
//            console.info('**************************get start********************************');
//            this.reason = data.reqPermissionDetails.reason;
//            this.versionName = data.versionName;
//            this.icon = resourceManager.getAppIcon(data.appInfo.iconId, data.name);
//            this.appName = resourceManager.getAppName(data.appInfo.labelId, data.name, data.appInfo.label);
//            console.info('**************************get end********************************');
//        });
//        bundle.checkPermission(this.bundleName, this.permission).then((result) => {
//            console.info('**************************checkPermission start********************************');
//            console.info(result);
//            result
//            if(result == 0){
//                this.radioList[0].isChecked = true;
//                this.radioList[1].isChecked = false;
//            }else{
//                this.radioList[0].isChecked = false;
//                this.radioList[1].isChecked = true;
//            }
//            console.info('**************************checkPermission end********************************');
//        });

    },
    /**
     *
     */
    clickRadio(index) {
        if(this.radioList[index].isChecked == false){
            for(var i = 0; i < this.radioList.length; i++){
                this.radioList[i].isChecked = false
            }
            this.radioList[index].isChecked = true;
            //调用授权接口 TODO
        }
    },
    onCreate() {
        console.info('permission appGrant onCreate')
    },
    onReady() {
        console.info('permission appGrant onReady')
    },
    onShow() {
        console.info('permission appGrant onShow')
    },
    onHide() {
        console.info('permission appGrant onHide')
    },
    onDestroy() {
        console.info('permission appGrant onDestroy')
    }
}
