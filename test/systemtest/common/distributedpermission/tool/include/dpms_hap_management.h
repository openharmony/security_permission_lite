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

#ifndef OHOS_SYSTEM_TEST_DPMS_HAP_MANAGEMENT_H
#define OHOS_SYSTEM_TEST_DPMS_HAP_MANAGEMENT_H
#include <memory>
namespace OHOS {
namespace STPermissionUtil {
namespace {
//包名常量（推荐使用dpms开头的包，权限信息多）
// bundules（uninstall时候使用该常量。此外获取包信息也是。与下面的hap包名对应 两个一组）
// const std::string SYSTEM_BUNDLE_NAME = "com.system.hiworld.examples1";
// const std::string SYSTEM_HAP_NAME = "DPSystemBundle";  // hap包名称（install时候使用该常量）

// const std::string THIRD_HAP_NAME = "DPThirdBundle";
// const std::string THIRD_BUNDLE_NAME = "com.third.hiworld.example1";

// //只有一个 SystemGrant 授权方式的系统包
// const std::string System_OnlySystemGrant_HAP_NAME = "dpmsSystemBundleOnlySystemGrant";
// const std::string System_OnlySystemGrant_BUNDLE_NAME = "com.system.hiworld.dpms_system_grant";

// //大于1024个权限
// const std::string THIRD_MORETHAN_MAXPERMISSION_HAP_NAME = "dpmsThirddMaxPermissionExceeded";
// const std::string THIRD_MORETHAN_MAXPERMISSION_BUNDLE_NAME = "com.third.hiworld.dpmsThirddMaxPermissionExceeded";

// //等于1024个权限
// const std::string THIRD_EQ_MAXPERMISSION_HAP_NAME = "dpmsThirddEqMaxPermission";
// const std::string THIRD_EQ_MAXPERMISSION_BUNDLE_NAME = "com.third.hiworld.dpmsThirddEqMaxPermission";

// //权限信息更多的第三方包
// const std::string THIRD_HAP_NAME2 = "dpmsThirdBundle";
// const std::string THIRD_BUNDLE_NAME2 = "com.third.hiworld.dpmsExample";

// //权限信息更多的系统包
// const std::string SYSTEM_HAP_NAME2 = "dpmsSystemBundle";
// const std::string SYSTEM_BUNDLE_NAME2 = "com.system.hiworld.dpms_system_bundle";


// //包含本地受限权   系统包
// const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_HAP_NAME = "dpmsSystemIncludeUseByLocalApp";
// const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME = "com.system.hiworld.include_use_by_local_app";

// //包含本地受限权   系统包（更新用）
// const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1 = "dpmsSystemIncludeUseByLocalAppAddV1";
// const std::string SYSTEM_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1 = "com.system.hiworld.include_use_by_local_app";

// //包含本地受限权   三方包
// const std::string THIRD_INCLUDE_USE_BY_lOCAL_HAP_NAME = "dpmsThirdIncludeUseByLocalApp";
// const std::string THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME = "com.third.hiworld.include_use_by_local_app";

// //包含本地受限权   三方包（更新用）
// const std::string THIRD_INCLUDE_USE_BY_lOCAL_HAP_NAME_ADD_V1 = "dpmsThirdIncludeUseByLocalAppAddV1";
// const std::string THIRD_INCLUDE_USE_BY_lOCAL_BUNDLE_NAME_ADD_V1 = "com.third.hiworld.include_use_by_local_app";

}  // namespace

}  // namespace STPermissionUtil
}  // namespace OHOS
#endif  // OHOS_SYSTEM_TEST_ABILITY_UTIL_H