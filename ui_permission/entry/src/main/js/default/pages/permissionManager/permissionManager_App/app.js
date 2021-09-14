import router from '@system.router';
import PinyinSort from '../../../common/utils/PinyinSort.js';
export default {
    data: {
        loading_on: true,
        blankpage_on:true,
        bundleInfoManager:'',
        array_show_data:[],
        array_source_data: [
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "大众点评",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"dazhongdianping"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "AR测量",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"arceliang"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "百度",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"baidu"
            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "抖音",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"douyin"

            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "34同城",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"34tongcheng"

            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "畅联",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"changlian"

            },

            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "zune",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"zune"

            },


            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "华为商城抖",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"huaweishangchengdou"

            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "高德地图",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "appName_pinyin":"gaodeditu"

            },
            {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "appName": "58同城",
                "permissionValue": "6项权限",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "bundleName":"testbundlename",
                "appName_pinyin":"58tongcheng"

            },

        ],
    },

    onInit(){
        this.loading_on = true;
        console.log("======= this.array_source_data.legth =======" + this.array_source_data.length);
//        this.bundleInfoManager = this.$app.$def.data.bundleInfoManager;
//        this.bundleInfoManager.updateBundleInfoList( this.updateBundleInfoListCallback.bind(this) );

        var pinyinSort = new PinyinSort();
        this.array_source_data.sort(pinyinSort.sortByAppName.bind(pinyinSort));
        this.array_show_data.length = 0;
        this.array_show_data = this.array_source_data.slice();
        setTimeout(()=>{
            this.loading_on = false;
            this.blankpage_on = false;
        },5000 );

      //  this.loading_on = false;

    },

    updateBundleInfoListCallback( list ){
        this.array_source_data = list.slice();
        this.convertData();
    },

    convertData(){
        this.array_source_data.forEach(( item )=>{
            item.permissionValue = item.reqPermissions.length + this.$t('permissionManager.permission_num');
            item.permissionDefaultValue = false;
            item.permissionType = 1;
        })
        this.createShowDataList( this.array_source_data );
        this.loading_on = false;
    },

    getBundleInfoList(){
        this.array_source_data = this.bundleInfoManager.getBundleInfoList.slice();
    },

    onclick( data ) {
        console.log("======= data.detail.text.reqPermissions.length =======" + data.detail.text.reqPermissions.length);

        if( data.detail.text.reqPermissions.length == 0){
            router.push ({
                uri: "pages/blankPage/blankPage",
                params:{
                    appName: data.detail.text.appName,
                }
            });
        } else {
            router.push ({
                uri: "pages/appPermissionList/appPermissionList",
                params:{
                    bundleName: data.detail.text.bundleName,
                    appName: data.detail.text.appName,
                }
            });
        }
    },

    search(data){
        data.detail.searchList.forEach((item)=>{
            console.info("callBackList : " + item.appName);
        })
        this.createShowDataList( data.detail.searchList );
    },

    createShowDataList( data_list ){
        this.array_show_data.length = 0;
        this.array_show_data = data_list.slice();
        if( this.array_show_data.length == 0){
            this.blankpage_on = true;
        } else {
            this.array_show_data[this.array_show_data.length-1].permissionShowDivider = false;
            this.blankpage_on = false;
        }
    }

}