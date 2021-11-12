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

#include "sensitive_permission_remind.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION,
    "SensitivePermissionReminder"};
}

SensitivePermissionRemind::SensitivePermissionRemind()
{
    PERMISSION_LOG_INFO(LABEL, "enter");
}

SensitivePermissionRemind::~SensitivePermissionRemind()
{
    PERMISSION_LOG_INFO(LABEL, "exit");
}

SensitivePermissionRemind &SensitivePermissionRemind::GetInstance()
{
    static SensitivePermissionRemind instance;
    return instance;
}

/**
 * Register using permission reminder to dpms at ohos side.
 *
 * @param callback instance of {@link ohos.security.permission.OnUsingPermissionReminder}.
 * @return result code indicates callback if register successfully.
 */
int32_t SensitivePermissionRemind::RegisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    if (callback == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: callback is nullptr", __func__);
        return Constant::FAILURE_DPMS;
    }

    if (reminderSet_.find(callback) == reminderSet_.end()) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: regist callback", __func__);
        reminderSet_.insert(callback);
    }
    return Constant::SUCCESS;
}

/**
 * unregister using permission reminder to dpms at ohos side.
 *
 * @param callback instance of {@link ohos.security.permission.OnUsingPermissionReminder} has been registered.
 * @return result code indicates callback if unregister successfully.
 */
int32_t SensitivePermissionRemind::UnregisterUsingPermissionReminder(const sptr<OnUsingPermissionReminder> &callback)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    if (callback == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: callback is nullptr", __func__);
        return Constant::FAILURE_DPMS;
    }

    if (reminderSet_.find(callback) != reminderSet_.end()) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: remove callback", __func__);
        reminderSet_.erase(callback);
    }
    return Constant::SUCCESS;
}

/**
 * Check permission and start permission using reminder if permission granted.
 *
 * @param permissionName permission name.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 * @return Permission checked result, {@link #GRANTED} indicates permission granted, otherwise {@link #DENIED}.
 */
int32_t SensitivePermissionRemind::CheckPermissionAndStartUsing(const std::string &permissionName,
    const std::string &deviceId, const int32_t &uid, const int32_t &pid)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    int32_t iGranted = Constant::FAILURE;
    char localDeviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, Constant::DEVICE_UUID_LENGTH);

    if (deviceId == localDeviceId) {
        std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
        if (bmsAdapter == nullptr) {
            PERMISSION_LOG_INFO(LABEL, "%{public}s: The permissionName is not sevsitive.", __func__);
            return Constant::FAILURE;
        }
        iBundleManager_ = bmsAdapter->GetBundleManager();
        if (iBundleManager_ == nullptr) {
            PERMISSION_LOG_INFO(LABEL, "%{public}s: iBundleManager_ is nullptr.", __func__);
            return Constant::FAILURE;
        }
        std::string bundleName;
        bool result = iBundleManager_->GetBundleNameForUid(uid, bundleName);
        if (!result) {
            PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundle name by uid %{public}d", __func__, uid);
            return Constant::FAILURE;
        }
        std::unique_ptr<PmsAdapter> pmsAdapter = std::make_unique<PmsAdapter>();
        iPermissionManager_ = pmsAdapter->GetPermissionManager();
        if (iPermissionManager_ == nullptr) {
            PERMISSION_LOG_INFO(LABEL, "%{public}s: iBundleManager_ is nullptr.", __func__);
            return Constant::PERMISSION_DENIED;
        }

        iGranted = iPermissionManager_->VerifyPermission(bundleName, permissionName, Constant::DEFAULT_USERID);
        PERMISSION_LOG_INFO(LABEL, "iGranted : %{public}d", iGranted);
    } else {
        int32_t distributedUid = SubjectDevicePermissionManager::GetInstance().AllocateDistributedUid(deviceId, uid);
        iGranted =
            SubjectDevicePermissionManager::GetInstance().CheckDistributedPermission(distributedUid, permissionName);
    }

    bool isRootOrSystem = DistributedDataValidator::IsSpecRuid(uid);
    if (GRANTED == iGranted && !isRootOrSystem) {
        StartUsingPermission(permissionName, deviceId, uid, pid);
    }

    if (GRANTED != iGranted) {
        PermissionRecordManager::GetInstance().AddPermissionsRecord(permissionName, deviceId, uid, 0, 1);
    }

    return iGranted;
}

/**
 * Used to permission using remind when app start using permission continuously.
 *
 * @param permName the permission name which app start using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 */
void SensitivePermissionRemind::StartUsingPermission(const std::string &permName, const std::string &deviceId,
    const int32_t &uid, const int32_t &pid)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    PermissionReminderInfo permReminderInfo;

    permReminderInfo.Init();

    GetPermissionReminderInfo(permName, pid, uid, deviceId, permReminderInfo);

    CloseAns();

    InsertPermissionReminderInfo(permName, pid, uid, deviceId, permReminderInfo);

    CreatAns();

    PermissionRecordManager::GetInstance().AddPermissionsRecord(permName, deviceId, uid, 1, 0);
    return;
}

/**
 * Used to permission using remind when app stop using permission continuously.
 *
 * @param permName the permission name which app stop using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 */
void SensitivePermissionRemind::StopUsingPermission(const std::string &permName, const std::string &deviceId,
    const int32_t &uid, const int32_t &pid)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    PermissionReminderInfo permReminderInfo;

    permReminderInfo.Init();

    GetPermissionReminderInfo(permName, pid, uid, deviceId, permReminderInfo);

    CloseAns();

    RemovePermissionReminderInfo(permName, pid, uid, deviceId, permReminderInfo);

    CreatAns();

    PermissionRecordManager::GetInstance().AddPermissionsRecord(permName, deviceId, uid, 1, 0);

    return;
}

/**
 * Get permission reminder infomation for the callback
 *
 * @param permName the permission name which app stop using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 * @param permReminderInfo permmission reminder infomation
 */
void SensitivePermissionRemind::GetPermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
    const std::string &deviceId, PermissionReminderInfo &permReminderInfo)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    // set deviceId
    permReminderInfo.SetDeviceId(deviceId);

    // set deviceLabel
    DeviceInfo deviceInfo;
    if (!deviceId.empty() &&
        DeviceInfoManager::GetInstance().GetDeviceInfo(deviceId, DeviceIdType::UNKNOWN, deviceInfo) &&
        !deviceInfo.deviceName.empty()) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: set deviceLabel.", __func__);
        permReminderInfo.SetDeviceLabel(deviceInfo.deviceName);
    }

    // get bundleInfo
    AppExecFwk::BundleInfo bundleInfo;
    if (!GetBundleInfo(bundleInfo, uid)) {
        return;
    }
    // set bundleName
    permReminderInfo.SetBundleName(bundleInfo.name);

    // set bundleLabel
    int indexS = bundleInfo.label.find("$");
    int indexE = bundleInfo.label.find(":");
    if (indexS == 0 && indexE > 0) {
        bundleInfo.label = bundleInfo.label.erase(indexS, ++indexE);
    }
    permReminderInfo.SetBundleLabel(bundleInfo.label);

    // set permName
    permReminderInfo.SetPermName(permName);

    // set appName
    std::unique_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: create resourceManager failed!", __func__);
        return;
    }

    std::string appName = "";
    for (auto resPath = bundleInfo.moduleResPaths.begin(); resPath != bundleInfo.moduleResPaths.end(); resPath++) {
        if (resPath->empty()) {
            continue;
        }

        if (!resourceManager->AddResource(resPath->c_str())) {
            continue;
        }

        if (Constant::SUCCESS != resourceManager->GetStringByName(bundleInfo.label.c_str(), appName)) {
            continue;
        }

        if (appName.empty()) {
            continue;
        }
        break;
    }
    permReminderInfo.appName = appName;
}

/**
 * Get bundle info by uid
 *
 * @param bundleInfo the bundle info of app.
 * @param uid the uid from app.
 * @return result true if get successfully.
 */
bool SensitivePermissionRemind::GetBundleInfo(AppExecFwk::BundleInfo &bundleInfo, int32_t uid)
{
    std::unique_ptr<BmsAdapter> bmsAdapter = std::make_unique<BmsAdapter>();
    iBundleManager_ = bmsAdapter->GetBundleManager();
    std::string bundleName;
    bool result = iBundleManager_->GetBundleNameForUid(uid, bundleName);
    if (!result) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundle name by uid %{public}d", __func__, uid);
        return false;
    }
    result = iBundleManager_->GetBundleInfo(bundleName, AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo);
    if (!result) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s cannot get bundleInfo by bundleName %{public}s", __func__,
            bundleName.c_str());
        return false;
    }
    return true;
}

/**
 * Insert permission reminder infomation for Ans
 *
 * @param permName the permission name which app stop using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 * @param permReminderInfo permmission reminder infomation
 */
void SensitivePermissionRemind::InsertPermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
    const std::string &deviceId, PermissionReminderInfo &permReminderInfo) const
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    bool isSevsitive = BaseRemoteCommand::IsSensitiveResource(permName);
    if (!isSevsitive) {
        PERMISSION_LOG_INFO(LABEL, "The permName is not sevsitive.");
        return;
    }

    char localDeviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, Constant::DEVICE_UUID_LENGTH);
    PERMISSION_LOG_INFO(LABEL, "localDeviceId :%{public}s", localDeviceId);

    std::vector<PermissionRemindInfo>::iterator item;
    bool permission_is_exist = false;
    for (item = RemindInfo::GetInstance().permRemindInfos_.begin();
         item != RemindInfo::GetInstance().permRemindInfos_.end(); item++) {
        if (!(deviceId != localDeviceId && item->deviceId == deviceId) &&
            !(deviceId == localDeviceId && item->pid == pid && item->uid == uid)) {
            continue;
        }

        permission_is_exist = permission_is_exist || CheckPermissionExist(item, permName);
        if (permName == Constant::LOCATION || permName == Constant::LOCATION_IN_BACKGROUND) {
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::LOCATION);
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::LOCATION_IN_BACKGROUND);
        }

        if (permName == Constant::READ_CALENDAR || permName == Constant::WRITE_CALENDAR) {
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::READ_CALENDAR);
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::WRITE_CALENDAR);
        }

        if (permName == Constant::DISTRIBUTED_DATASYNC || permName == Constant::DISTRIBUTED_DATA) {
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::DISTRIBUTED_DATASYNC);
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::DISTRIBUTED_DATA);
        }

        if (permName == Constant::MEDIA_LOCATION || permName == Constant::READ_MEDIA ||
            permName == Constant::WRITE_MEDIA) {
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::MEDIA_LOCATION);
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::READ_MEDIA);
            permission_is_exist = permission_is_exist || CheckPermissionExist(item, Constant::WRITE_MEDIA);
        }
        if (!permission_is_exist) {
            item->permissions.push_back(permName);
        }
        return;
    }
    CreateNewPermissionReminderInfo(permName, pid, uid, deviceId, permReminderInfo);
}

/**
 * check permisson is exist
 *
 * @param item permission info vector
 * @param permissionName  permission name.
 * @return result true if check successfully.
 */
bool SensitivePermissionRemind::CheckPermissionExist(const std::vector<PermissionRemindInfo>::iterator item,
    const std::string permissionName) const
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::vector<std::string>::iterator it = find(item->permissions.begin(), item->permissions.end(), permissionName);
    if (it != item->permissions.end()) {
        PERMISSION_LOG_INFO(LABEL, "The permission is haved. permssionName :%{public}s", permissionName.c_str());
        return true;
    }
    return false;
}

/**
 * create new permission reminder info
 *
 * @param permName the permission name which app stop using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 * @param permReminderInfo permmission reminder infomation
 */
void SensitivePermissionRemind::CreateNewPermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
    const std::string &deviceId, PermissionReminderInfo &permReminderInfo) const
{
    PermissionRemindInfo permItem;
    permItem.permissions.push_back(permName);
    permItem.pid = pid;
    permItem.uid = uid;
    permItem.deviceId = deviceId;
    permItem.notificationId = uid;
    permItem.bundleName = Str16ToStr8(permReminderInfo.bundleName);
    permItem.bundleLabel = Str16ToStr8(permReminderInfo.bundleLabel);
    permItem.deviceName = Str16ToStr8(permReminderInfo.deviceLabel);
    permItem.appName = permReminderInfo.appName;
    RemindInfo::GetInstance().permRemindInfos_.push_back(permItem);
}

/**
 * Remove permission reminder infomation for Ans
 *
 * @param permName the permission name which app stop using.
 * @param pid the pid from pid json string indicates app information
 * @param uid the uid from pid json string indicates app information
 * @param deviceId thd deviceId from pid json string indicates app information
 * @param permReminderInfo permmission reminder infomation
 */
void SensitivePermissionRemind::RemovePermissionReminderInfo(const std::string &permName, int32_t pid, int32_t uid,
    const std::string &deviceId, PermissionReminderInfo &permReminderInfo) const
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    char localDeviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, Constant::DEVICE_UUID_LENGTH);
    PERMISSION_LOG_INFO(LABEL, "localDeviceId :%{public}s", localDeviceId);

    std::vector<PermissionRemindInfo>::iterator item;
    for (item = RemindInfo::GetInstance().permRemindInfos_.begin();
         item != RemindInfo::GetInstance().permRemindInfos_.end(); item++) {
        if (!(deviceId != localDeviceId && item->deviceId == deviceId) &&
            !(deviceId == localDeviceId && item->pid == pid && item->uid == uid)) {
            continue;
        }

        ErasePermission(item, permName);
        if (permName == Constant::LOCATION || permName == Constant::LOCATION_IN_BACKGROUND) {
            ErasePermission(item, Constant::LOCATION);
            ErasePermission(item, Constant::LOCATION_IN_BACKGROUND);
        }

        if (permName == Constant::READ_CALENDAR || permName == Constant::WRITE_CALENDAR) {
            ErasePermission(item, Constant::READ_CALENDAR);
            ErasePermission(item, Constant::WRITE_CALENDAR);
        }

        if (permName == Constant::DISTRIBUTED_DATASYNC || permName == Constant::DISTRIBUTED_DATA) {
            ErasePermission(item, Constant::DISTRIBUTED_DATASYNC);
            ErasePermission(item, Constant::DISTRIBUTED_DATA);
        }

        if (permName == Constant::MEDIA_LOCATION || permName == Constant::READ_MEDIA ||
            permName == Constant::WRITE_MEDIA) {
            ErasePermission(item, Constant::MEDIA_LOCATION);
            ErasePermission(item, Constant::READ_MEDIA);
            ErasePermission(item, Constant::WRITE_MEDIA);
        }
        if (item->permissions.size() <= 0) {
            RemindInfo::GetInstance().permRemindInfos_.erase(item);
        }
        return;
    }
}

/**
 * delete permission
 *
 * @param item permission info vector
 * @param permissionName  permission name.
 */
void SensitivePermissionRemind::ErasePermission(std::vector<PermissionRemindInfo>::iterator &item,
    const std::string permissionName) const
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::vector<std::string>::iterator it = find(item->permissions.begin(), item->permissions.end(), permissionName);
    if (it != item->permissions.end()) {
        item->permissions.erase(it);
    }
}

/**
 * creat ans, get resource manager
 */
void SensitivePermissionRemind::CreatAns() const
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);
    std::unique_ptr<Global::Resource::ResourceManager> resourceManager(Global::Resource::CreateResourceManager());
    if (resourceManager == nullptr) {
        PERMISSION_LOG_INFO(LABEL, "%{public}s: create resourceManager failed!", __func__);
        return;
    }

    if (RemindInfo::GetInstance().permRemindInfos_.size() <= 0) {
        PERMISSION_LOG_INFO(LABEL, "RemindInfo::GetInstance().permRemindInfos_ is nullptr.");
        return;
    }
    bool ret = true;
    ret = resourceManager->AddResource(g_resFilePath);
    if (!ret) {
        PERMISSION_LOG_INFO(LABEL, "failed to AddResource");
    }

    int permCount = 0;
    std::string label = "";
    std::string settingtext = "";
    std::string settingtextDistributed = "";
    std::string permName = "";
    std::string permUsing = "";
    char localDeviceId[Constant::DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(localDeviceId, Constant::DEVICE_UUID_LENGTH);

    if (ret) {
        resourceManager->GetStringByName("perm_using_remind_label", label);
        resourceManager->GetStringByName("perm_using_remind_setting1", settingtext);
        resourceManager->GetStringByName("perm_using_remind_setting2", settingtextDistributed);
    }

    if (label.empty() || settingtext.empty() || settingtextDistributed.empty()) {
        ret = false;
        label = perm_using_remind_label_;
        settingtext = perm_using_remind_setting1_;
        settingtextDistributed = perm_using_remind_setting2_;
    }

    std::vector<PermissionRemindInfo>::iterator item;
    std::vector<std::string>::iterator perm;

    for (item = RemindInfo::GetInstance().permRemindInfos_.begin();
         item != RemindInfo::GetInstance().permRemindInfos_.end(); item++) {
        if (item->permissions.size() <= 0) {
            continue;
        }

        item->label = label;

        Notification::NotificationRequest notification(item->notificationId);

        notification.SetLabel(item->label);
        std::shared_ptr<Notification::NotificationLongTextContent> longTextContent =
            std::make_shared<Notification::NotificationLongTextContent>();

        // The using permName
        permCount = item->permissions.size();
        if (ret) {
            resourceManager->GetPluralStringByName("perm_using_remind_text", permCount, permUsing);
            if (permUsing.empty()) {
                permUsing = perm_using_remind_texts_.at(permCount);
            }
        } else {
            permUsing = perm_using_remind_texts_.at(permCount);
        }

        for (perm = item->permissions.begin(); perm != item->permissions.end(); perm++) {
            if (ret) {
                resourceManager->GetStringByName(perm->c_str(), permName);
                if (permName.empty()) {
                    permName = perm->c_str();
                    permName = perm_using_names_.find(permName)->second;
                }
            } else {
                permName = perm->c_str();
                permName = perm_using_names_.find(permName)->second;
            }

            permUsing.replace(permUsing.find("@", 0), 1, permName.c_str());
        }

        longTextContent->SetLongText(permUsing);
        std::shared_ptr<Notification::NotificationContent> content =
            std::make_shared<Notification::NotificationContent>(longTextContent);
        notification.SetCreatorPid(item->pid);
        notification.SetCreatorUid(item->uid);
        notification.SetContent(content);

        if (item->deviceId.empty() || item->deviceId == localDeviceId) {
            if (item->appName.empty()) {
                longTextContent->SetExpandedTitle(item->bundleName);
            } else {
                longTextContent->SetExpandedTitle(item->appName);
            }
            notification.SetSettingsText(settingtext);
            PERMISSION_LOG_INFO(LABEL, "label     : %{public}s", label.c_str());
            PERMISSION_LOG_INFO(LABEL, "appName   : %{public}s", longTextContent->GetExpandedTitle().c_str());
            PERMISSION_LOG_INFO(LABEL, "permUsing : %{public}s", permUsing.c_str());
            PERMISSION_LOG_INFO(LABEL, "setting   : %{public}s", settingtext.c_str());
            PERMISSION_LOG_INFO(LABEL, "checkresult : CreatAns Success");
        } else {
            longTextContent->SetExpandedTitle(item->deviceName);
            notification.SetSettingsText(settingtextDistributed);
            PERMISSION_LOG_INFO(LABEL, "label     : %{public}s", label.c_str());
            PERMISSION_LOG_INFO(LABEL, "appName   : %{public}s", item->deviceName.c_str());
            PERMISSION_LOG_INFO(LABEL, "permUsing : %{public}s", permUsing.c_str());
            PERMISSION_LOG_INFO(LABEL, "setting   : %{public}s", settingtextDistributed.c_str());
            PERMISSION_LOG_INFO(LABEL, "checkresult : CreatAns Success");
        }
        Notification::NotificationHelper::PublishNotification(item->label, notification);
    }
}

/**
 * Close the Ans
 */
void SensitivePermissionRemind::CloseAns() const
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s: called!", __func__);

    std::vector<PermissionRemindInfo>::iterator item;

    for (item = RemindInfo::GetInstance().permRemindInfos_.begin();
         item != RemindInfo::GetInstance().permRemindInfos_.end(); item++) {
        Notification::NotificationHelper::CancelNotification(item->label, item->notificationId);
    }
}

} // namespace Permission
} // namespace Security
} // namespace OHOS