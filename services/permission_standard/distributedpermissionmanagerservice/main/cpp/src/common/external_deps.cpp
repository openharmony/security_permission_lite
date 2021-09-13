
#include "external_deps.h"

#include "permission_log.h"
#include "ipc_skeleton.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "ExternalDeps"};
}
sptr<AppExecFwk::IBundleMgr> ExternalDeps::GetBundleManager(sptr<AppExecFwk::IBundleMgr> iBundleManager_)
{
    if (iBundleManager_ == nullptr) {
        auto bundleObj = GetSystemAbility(Constant::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (bundleObj == nullptr) {
            PERMISSION_LOG_ERROR(LABEL, "failed to get bundle manager service.");
            return nullptr;
        }
        iBundleManager_ = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    }

    return iBundleManager_;
}

sptr<Permission::IPermissionManager> ExternalDeps::GetPermissionManager(
    sptr<Permission::IPermissionManager> iPermissionManager_)
{
    if (iPermissionManager_ == nullptr) {
        auto permissionObj = GetSystemAbility(Constant::SUBSYS_SECURITY_PERMISSION_SYS_SERVICE_ID);
        if (permissionObj == nullptr) {
            PERMISSION_LOG_ERROR(LABEL, "failed to get permission manager service.");
            return nullptr;
        }
        iPermissionManager_ = iface_cast<Permission::IPermissionManager>(permissionObj);
    }

    return iPermissionManager_;
}

sptr<AAFwk::IAbilityManager> ExternalDeps::GetAbilityManager(sptr<AAFwk::IAbilityManager> iAbilityManager_)
{
    if (iAbilityManager_ == nullptr) {
        auto abilityObj = GetSystemAbility(Constant::ABILITY_MST_SERVICE_ID);
        if (abilityObj == nullptr) {
            PERMISSION_LOG_ERROR(LABEL, "failed to get ability manager service.");
            return nullptr;
        }
        iAbilityManager_ = iface_cast<AAFwk::IAbilityManager>(abilityObj);
    }

    return iAbilityManager_;
}

sptr<IRemoteObject> ExternalDeps::GetSystemAbility(const Constant::ServiceId systemAbilityId)
{
    if (saMgr_ == nullptr) {
        std::lock_guard<std::mutex> lock(saMutex_);
        if (saMgr_ == nullptr) {
            saMgr_ = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (saMgr_ == nullptr) {
                PERMISSION_LOG_ERROR(LABEL, "fail to get Registry.");
                return nullptr;
            }
        }
    }
    return saMgr_->GetSystemAbility(systemAbilityId);
}

}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
