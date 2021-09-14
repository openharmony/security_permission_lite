import router from '@system.router';
export default {
    data: {
        title: "",
        array_permission: [],
    },

    onInit() {
        this.array_permission = [];
        this.permissionDefList.forEach(( data )=>{
                    var item = {
                        "permissionIcon": "/res/image/ic_permissions_about.png",
                        "permissionTitle": "",
                        "permissionValue": "",
                        "permissionArrow": "/res/image/ic_permissions_arrow.png",
                        "permissionDefaultValue": false,
                        "permissionSummary": "",
                        "permissionShowDivider": true,
                        "permissionType": 1,
                        "routerPath":""
                    };
                    item.permissionTitle = data;
                    this.array_permission.push(item);
                })
    },
    onclick(data){
        router.push({
            uri: 'pages/appGrant/appGrant',
            params: {
                title: data.detail.text.permissionTitle,
                bundleName: this.bundleName,
                permission: data.detail.text.permissionTitle
            }
        });
    }
}