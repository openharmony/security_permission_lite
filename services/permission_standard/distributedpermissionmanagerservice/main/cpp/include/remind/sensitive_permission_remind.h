/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SENSITIVE_PERMISSION_REMIND_H
#define SENSITIVE_PERMISSION_REMIND_H

#include <sys/types.h>
#include <unistd.h>
#include <set>
#include <cstring>
#include "app_id_info.h"
#include "bundle_mgr_interface.h"
#include "bms_adapter.h"
#include "constant.h"
#include "ipc_skeleton.h"
#include "notification_helper.h"
#include "on_using_permission_reminder.h"
#include "pms_adapter.h"
#include "permission_log.h"
#include "permission_record_manager.h"
#include "permission_reminder_info.h"
#include "resource_manager.h"
#include "remind_info.h"
#include "subject_device_permission_manager.h"

namespace OHOS {
namespace Security {
namespace Permission {
class SensitivePermissionRemind {
public:
    SensitivePermissionRemind();

    ~SensitivePermissionRemind();

    static SensitivePermissionRemind &GetInstance();
    /**
     * Register using permission reminder to dpms at ohos side.
     *
     * @param callback instance of {@link ohos.security.permission.OnUsingPermissionReminder}.
     * @return result code indicates callback if register successfully.
     */
    int32_t RegisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback);

    /**
     * unregister using permission reminder to dpms at ohos side.
     *
     * @param callback instance of {@link ohos.security.permission.OnUsingPermissionReminder} has been registered.
     * @return result code indicates callback if unregister successfully.
     */
    int32_t UnregisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback);

    /**
     * Check permission and start permission using reminder if permission granted.
     *
     * @param permissionName permission name.
     * @param pid the pid from pid json string indicates app information
     * @param uid the uid from pid json string indicates app information
     * @param deviceId thd deviceId from pid json string indicates app information
     * @return Permission checked result, {@link #GRANTED} indicates permission granted, otherwise {@link #DENIED}.
     */
    int32_t CheckPermissionAndStartUsing(const std::string &permissionName, const std::string &deviceId,
        const int32_t &uid, const int32_t &pid);

    /**
     * Used to permission using remind when app start using permission continuously.
     *
     * @param permName the permission name which app start using.
     * @param pid the pid from pid json string indicates app information
     * @param uid the uid from pid json string indicates app information
     * @param deviceId thd deviceId from pid json string indicates app information
     */
    void StartUsingPermission(const std::string &permName, const std::string &deviceId, const int32_t &uid,
        const int32_t &pid);

    /**
     * Used to permission using remind when app stop using permission continuously.
     *
     * @param permName the permission name which app stop using.
     * @param pid the pid from pid json string indicates app information
     * @param uid the uid from pid json string indicates app information
     * @param deviceId thd deviceId from pid json string indicates app information
     */
    void StopUsingPermission(const std::string &permName, const std::string &deviceId, const int32_t &uid,
        const int32_t &pid);

private:
    sptr<AppExecFwk::IBundleMgr> iBundleManager_;
    sptr<Permission::IPermissionManager> iPermissionManager_;
    std::set<sptr<OnUsingPermissionReminder>> reminderSet_;
    void GetPermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid, const std::string &deviceId,
        PermissionReminderInfo &permReminderInfo);
    void InsertPermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
        const std::string &deviceId, PermissionReminderInfo &permReminderInfo) const;
    void RemovePermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
        const std::string &deviceId, PermissionReminderInfo &permReminderInfo) const;
    void CreateNewPermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
        const std::string &deviceId, PermissionReminderInfo &permReminderInfo) const;
    bool CheckPermissionExist(const std::vector<PermissionRemindInfo>::iterator item,
        const std::string permissionName) const;
    void ErasePermission(std::vector<PermissionRemindInfo>::iterator &item, const std::string permissionName) const;
    bool GetBundleInfo(AppExecFwk::BundleInfo &bundleInfo, int32_t uid);
    void CreatAns() const;
    void CloseAns() const;
    const char *g_resFilePath = "/data/test/resources.index";

    const std::string perm_using_remind_label_ = "系统";
    const std::string perm_using_remind_setting1_ = "跳转应用详情";
    const std::string perm_using_remind_setting2_ = "停止应用";
    const std::vector<std::string> perm_using_remind_texts_ = {
        "正在使用@",
        "正在使用@",
        "正在使用@和@",
        "正在使用@和@和@",
        "正在使用@和@和@和@",
        "正在使用@和@和@和@和@",
        "正在使用@和@和@和@和@和@",
        "正在使用@和@和@和@和@和@和@",
        "正在使用@和@和@和@和@和@和@和@",
        "正在使用@和@和@和@和@和@和@和@和@",
    };

    const std::map<std::string, std::string> perm_using_names_ = {
        {Constant::LOCATION, "位置"},
        {Constant::LOCATION_IN_BACKGROUND, "位置"},
        {Constant::CAMERA, "相机"},
        {Constant::MICROPHONE, "麦克风"},
        {Constant::READ_CALENDAR, "日历"},
        {Constant::WRITE_CALENDAR, "日历"},
        {Constant::ACTIVITY_MOTION, "健康运动"},
        {Constant::READ_HEALTH_DATA, "健康"},
        {Constant::DISTRIBUTED_DATASYNC, "分布式数据管理"},
        {Constant::DISTRIBUTED_DATA, "分布式数据管理"},
        {Constant::MEDIA_LOCATION, "媒体"},
        {Constant::READ_MEDIA, "媒体"},
        {Constant::WRITE_MEDIA, "媒体"},
    };
    enum {
        DENIED = -1,
        GRANTED = 0,
    };
};
} // namespace Permission
} // namespace Security
} // namespace OHOS

#endif // SENSITIVE_PERMISSION_REMIND_H
