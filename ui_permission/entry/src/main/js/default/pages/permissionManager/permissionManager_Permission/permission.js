import router from '@system.router';
import PermissionManagerConfigs from '../../../common/configs/PermissionManagerConfigs.js'
export default {
    data: {
        array_permission:[]
    },

    onInit() {
        this.array_permission = [];
        PermissionManagerConfigs.permissionLable.forEach(( data )=>{
            var item = {
                "permissionIcon": "/res/image/ic_permissions_display.png",
                "permissionTitle": "",
                "permissionValue": "",
                "permissionArrow": "/res/image/ic_permissions_arrow.png",
                "permissionDefaultValue": false,
                "permissionSummary": "",
                "permissionShowDivider": true,
                "permissionType": 1,
                "routerPath":"",
                "permissionName":""
            };

            item.permissionTitle = this.$t('permissionManager.'+ data.lable);
            item.routerPath = data.routerPath;
            item.permissionName = data.permissionName;
            item.groupLableName = data.groupLableName;
            this.array_permission.push(item);
        })
    },

    onclick( data ) {
        router.push ({
            uri: data.detail.text.routerPath,
            params:{
                title:data.detail.text.permissionTitle,
                permission: data.detail.text.permissionName,
                groupLableName:data.detail.text.groupLableName
            }
        });
    },

}