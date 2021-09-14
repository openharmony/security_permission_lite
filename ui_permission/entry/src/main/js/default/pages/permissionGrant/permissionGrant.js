//import bundle from '@ohos.bundle';
import PinyinSort from '../../common/utils/PinyinSort.js';
export default {
    data: {
        bundleInfoManager:'',
        loading_on: true,
        title: "",
        selectRow: 0,
        permission: "",
        description: "meiyou数据",
        arrayShowData: [],
        sourceList: [],
        permissionGrantList: [
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": false,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "58tongcheng",
                "appName": "58同城"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": true,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "57tongcheng",
                "appName": "57同城"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": true,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "beiwanglu",
                "appName": "备忘录"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": false,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "baidu",
                "appName": "百度"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": false,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "beifen",
                "appName": "备份"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": true,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "11111",
                "appName": "11111"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": true,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "changshou",
                "appName": "长寿"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionDefaultValue": false,
                "permissionSwitchValue": true,
                "permissionShowDivider": true,
                "permissionType": 2,
                "bundleName": "",
                "appName_pinyin": "ahahaa",
                "appName": "啊哈哈a"
            }
        ],
    },
    onInit() {
        //        this.title = "存储";
//        this.permission = "";
        console.info('sssssssssssssssssssssssssssssssssssssssss1' + this.title);
        console.info('sssssssssssssssssssssssssssssssssssssssss' + this.permission);

//        this.bundleInfoManager = this.$app.$def.data.bundleInfoManager;
//        this.sourceList = this.bundleInfoManager.getBundleInfoList.slice();
//        if(this.sourceList.length == 0){
//            this.bundleInfoManager.updateBundleInfoList(this.updateBundleInfoListCallback.bind(this));
//        }
//        this.convertData();

        //***********************删除
        var pinyinSort = new PinyinSort();
        this.permissionGrantList.sort(pinyinSort.sortByAppName.bind(pinyinSort));
        setTimeout(()=>{
            this.loading_on = false;
        },5000 );
        //************************删除

        if(this.permissionGrantList.length > 0){
            this.permissionGrantList[this.permissionGrantList.length -1].permissionShowDivider = false;
        }
        this.arrayShowData = this.permissionGrantList.slice();
        console.info('permission permissionGrant onInit');
    },

    updateBundleInfoListCallback(list){
        this.sourceList = list.slice();
    },


//    convertData(){
//        this.permissionGrantList = [];
//        for( var index = 0; index < this.sourceList.length; index++ ){
//            let item = this.sourceList[index];
//            if(item.reqPermissions.includes(this.permission)){
//                //checkPermission接口
//                bundle.checkPermission(item.name, this.permission).then((result) => {
//                    console.info('**************************checkPermission start********************************');
//                    console.info(result);
//                    item.permissionDefaultValue = (result == 0 ? true : false);
//                    console.info('**************************checkPermission end********************************');
//                })
//                item.permissionSwitchValue = true;
//                item.permissionType = 2;
//                this.permissionGrantList.push(item);
//            }
//        }
//        this.loading_on = false;
//    },

    clickRow(index){
        this.selectRow = index;
    },
    switchClick(data){
        console.info("111111111"  + data.detail.text);
        console.info("111111111"  + data.detail.defaultText);
        console.info("22222223333   " + this.selectRow);
//        if(data.detail.text == data.detail.defaultText){
//
//        }
        //调用授权接口 TODO
    },
    search(data){
        data.detail.searchList.forEach((item)=>{
            console.info("callBackList : " + item.appName);
        })
        this.arrayShowData = data.detail.searchList.slice();
    },
    onCreate() {
        console.info('permission permissionGrant onCreate')
    },
    onReady() {
        console.info('permission permissionGrant onReady')
    },
    onShow() {
        console.info('permission permissionGrant onShow')
    },
    onHide() {
        console.info('permission permissionGrant onHide')
    },
    onDestroy() {
        console.info('permission permissionGrant onDestroy')
    }
}
