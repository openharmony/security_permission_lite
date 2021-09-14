import router from '@system.router';
import PermissionManagerConfigs from '../../../../common/configs/PermissionManagerConfigs.js'

export default {
    data: {
        title: "",
        array_permission: [],
        group_lable_name:""
    },

    onInit() {
        this.group_lable_name = this.groupLableName;
        this.array_permission = [];
        PermissionManagerConfigs[this.group_lable_name].forEach(( data )=>{
            var item = {
                "permissionIcon": "",
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
            this.array_permission.push(item);
        })
    },

    onclick( data ) {
        router.push ({
            uri: data.detail.text.routerPath,
            params:{
                title:data.detail.text.permissionTitle,
                permission: data.detail.text.permissionName,
            }
        });
    },
}