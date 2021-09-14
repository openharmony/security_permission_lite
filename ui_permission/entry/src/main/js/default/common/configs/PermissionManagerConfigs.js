const PermissionManagerConfigs = {
    permissionLable : [
        //位置
        { "lable":"location",
          "routerPath":"pages/permissionManager/permissionManager_Permission/permission_group/permission_group" ,
          "permissionName":"",
          "groupLableName":"permissionGroupLocation"
        },
        //相机
        { "lable":"camera",
          "routerPath":"pages/permissionGrant/permissionGrant",
          "permissionName":"ohos.permission.CAMERA",
          "groupLableName":""
        },
        //麦克风
        { "lable":"microphone",
          "routerPath":"pages/permissionGrant/permissionGrant" ,
          "permissionName":"ohos.permission.MICROPHONE",
          "groupLableName":""
        },
        //电话
        { "lable":"phone",
          "routerPath":"pages/permissionGrant/permissionGrant",
          "permissionName":"ohos.permission.PLACE_CALL",
          "groupLableName":""
        },
        //信息
        { "lable":"messaging",
          "routerPath":"pages/permissionManager/permissionManager_Permission/permission_group/permission_group" ,
          "permissionName":"",
          "groupLableName":"permissionGroupMessaging"
        },
        //通讯录
        { "lable":"contacts",
          "routerPath":"pages/permissionManager/permissionManager_Permission/permission_group/permission_group",
          "permissionName":"",
          "groupLableName":"permissionGroupContacts"
        },
        //通话记录
        { "lable":"call_logs",
          "routerPath":"pages/permissionManager/permissionManager_Permission/permission_group/permission_group",
          "permissionName":"",
          "groupLableName":"permissionGroupCalllogs"
        },
        //媒体与文件
        { "lable":"media",
          "routerPath":"pages/permissionManager/permissionManager_Permission/permission_group/permission_group",
          "permissionName":"",
          "groupLableName":"permissionGroupMedia"
        },
        //日历
        { "lable":"calendar",
          "routerPath":"pages/permissionManager/permissionManager_Permission/permission_group/permission_group",
          "permissionName":"",
          "groupLableName":"permissionGroupCalendar"
        },
        //健身运动
        { "lable":"physical_activity",
          "routerPath":"pages/permissionGrant/permissionGrant",
          "permissionName":"ohos.permission.ACTIVITY_MOTION",
          "groupLableName":""
        },
        //健康
        { "lable":"health",
            "routerPath":"pages/permissionGrant/permissionGrant",
            "permissionName":"ohos.permission.READ_HEALTH_DATA",
            "groupLableName":""
        },
        //应用内安装其他应用
        { "lable":"in_app_installations",
          "routerPath":"pages/permissionGrant/permissionGrant",
          "permissionName":"ohos.permission.INSTALL_BUNDLE",
          "groupLableName":""
        },
        //悬浮窗
        { "lable":"dropzone",
          "routerPath":"pages/permissionGrant/permissionGrant",
          "permissionName":"ohos.permission.SYSTEM_FLOAT_WINDOW",
          "groupLableName":""
        },

    ],

    permissionGroupLocation : [
        { "lable":"read_call_status_and_mobile_network_information" },
        { "lable":"directly_call_phone_numbers" },
        { "lable":"access_call_forwarding" }
    ],

    permissionGroupMedia : [
        { "lable":"read_call_status_and_mobile_network_information" },
        { "lable":"directly_call_phone_numbers" },
        { "lable":"access_call_forwarding" }
    ],

    permissionGroupMessaging : [
        { "lable":"read_messages" },
        { "lable":"send_messages" },
        { "lable":"receive_messages" },
        { "lable":"send_mms" }
    ],

    permissionGroupContacts : [
        { "lable":"read_contacts" },
        { "lable":"modify_contacts" }
    ],

    permissionGroupCalllogs : [
        { "lable":"read_call_logs" },
        { "lable":"modify_call_logs" }
    ],

    permissionGroupCalendar : [
        { "lable":"read_calendar" },
        { "lable":"modify_calendar" }
    ]
}
export default PermissionManagerConfigs;