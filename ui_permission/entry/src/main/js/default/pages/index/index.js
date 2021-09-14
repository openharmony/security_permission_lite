import router from '@system.router';

export default {
    launch() {
        router.push ({
            uri:'pages/appGrant/appGrant', //  指定要跳转的页面
        })
    },
    launch2() {
        router.push ({
            uri:'pages/permissionGrant/permissionGrant', //  指定要跳转的页面
        })
    },
    launch3() {
        router.push ({
            uri:"pages/appPermissionList/appPermissionList", //  指定要跳转的页面
        })
    },
    launch4() {
        router.push ({
            uri:"pages/distributedPermissionRecords/distributedPermissionRecords", //  指定要跳转的页面
        })
    },
    launch5() {
        router.push ({
            uri:"pages/permissionManager/permissionManager", //  指定要跳转的页面
        })
    },
    launch6() {
        router.push ({
            uri:"pages/permissionRecords/permissionRecords", //  指定要跳转的页面
        })
    }
}


