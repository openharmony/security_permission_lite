const PermissionGroupConfig = {
    "permissionGroup": {
        "CAMERA": [
                "ohos.permission.CAMERA"
        ],
        "MEDIA": [
                "ohos.permission.MEDIA_LOCATION",
                "ohos.permission.READ_MEDIA",
                "ohos.permission.WRITE_MEDIA"
        ],
        "PHONE": [
                "ohos.permission.PLACE_CALL"
        ],
        "CALENDAR": [
                "ohos.permission.READ_CALENDAR",
                "ohos.permission.WRITE_CALENDAR"
        ],
        "MICROPHONE": [
                "ohos.permission.MICROPHONE"
        ],
        "ACTIVITY_MOTION": [
                "ohos.permission.ACTIVITY_MOTION",
                "ohos.permission.ACCELEROMETER"
        ],
        "HEALTH_DATA": [
                "ohos.permission.READ_HEALTH_DATA"
        ],
        "LOCATION": [
                "ohos.permission.LOCATION",
                "ohos.permission.LOCATION_IN_BACKGROUND"
        ],
        "CONTACTS": [
                "ohos.permission.READ_CONTACTS",
                "ohos.permission.WRITE_CONTACTS"
        ],
        "MESSAGE": [
                "ohos.permission.RECEIVE_SMS",
                "ohos.permission.SEND_MESSAGES",
                "ohos.permission.READ_MESSAGES"
        ],
        "CALL_LOG": [
                "ohos.permission.READ_CALL_LOG",
                "ohos.permission.WRITE_CALL_LOG"
        ],
        "INSTALL_BUNDLE": [
                "ohos.permission.INSTALL_BUNDLE"
        ],
        "SYSTEM_FLOAT_WINDOW": [
                "ohos.permission.SYSTEM_FLOAT_WINDOW"
        ]
    },
    "appPermissionOrder": [
            "MEDIA",
            "PHONE",
            "LOCATION",
            "CAMERA",
            "MICROPHONE",
            "CONTACTS",
            "MESSAGE",
            "CALL_LOG",
            "CALENDAR",
            "HEALTH_DATA",
            "ACTIVITY_MOTION",
            "INSTALL_BUNDLE",
            "SYSTEM_FLOAT_WINDOW"
    ],
    "permissionSwitch": [
            "ohos.permission.READ_CONTACTS",
            "ohos.permission.WRITE_CONTACTS",
            "ohos.permission.READ_CALENDAR",
            "ohos.permission.WRITE_CALENDAR",
            "ohos.permission.RECEIVE_SMS",
            "ohos.permission.SEND_MESSAGES",
            "ohos.permission.READ_MESSAGES",
            "ohos.permission.READ_CALL_LOG",
            "ohos.permission.WRITE_CALL_LOG",
            "ohos.permission.PLACE_CALL",
            "ohos.permission.INSTALL_BUNDLE",
            "ohos.permission.SYSTEM_FLOAT_WINDOW"
    ]
}

export default PermissionGroupConfig;