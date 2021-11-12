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
#include "request_permissions_from_remote_test.h"
using namespace testing::ext;
namespace OHOS {
pid_t IPCSkeleton::pid_ = 1;
pid_t IPCSkeleton::uid_ = 1;
std::string IPCSkeleton::localDeviceId_ = "1004";
std::string IPCSkeleton::deviceId_ = "";
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "PermissionBmsManager"};
}
namespace {}  // namespace
void RequestPermissionsFromRemoteTest::SetUpTestCase(void)
{}
void RequestPermissionsFromRemoteTest::TearDownTestCase(void)
{}
void RequestPermissionsFromRemoteTest::SetUp()
{
    OHOS::sptr<OHOS::IRemoteObject> bundleObject = new OHOS::AppExecFwk::BundleMgrService();
    OHOS::sptr<OHOS::IRemoteObject> permissionObject = new PermissionManagerService();
    auto sysMgr = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysMgr == NULL) {
        GTEST_LOG_(ERROR) << "fail to get ISystemAbilityManager";
        return;
    }
    sysMgr->AddSystemAbility(Constant::ServiceId::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, bundleObject);
    sysMgr->AddSystemAbility(Constant::ServiceId::SUBSYS_SECURITY_PERMISSION_SYS_SERVICE_ID, permissionObject);
    service = DelayedSingleton<DistributedPermissionManagerService>::GetInstance();
}
void RequestPermissionsFromRemoteTest::TearDown()
{}
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_001";
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions(0);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("");
    std::string deviceId("deviceId");
    int32_t reasonResId = -1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    // case NeedStopProcess fail
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);
    EXPECT_EQ(callback->onResult_, 100);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_002";
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("");
    std::string deviceId("deviceId");
    int32_t reasonResId = -1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    // requestingDevices_.count(request->deviceId_) != 0
    RequestRemotePermission::GetInstance().requestingDevices_.insert(
        std::pair<std::string, std::string>(deviceId, deviceId));
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);
    EXPECT_EQ(callback->onResult_, 99);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
// 3and 4same ，deviceid can not empty
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0300, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_003";
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("");
    std::string deviceId("deviceId");
    int32_t reasonResId = -1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    RequestRemotePermission::GetInstance().requestingDevices_.erase(deviceId);
    // case   RequestPermissionsInfo::CanRequestPermissions IsDeviceIdValid fail
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);

    EXPECT_EQ(callback->onResult_, 99);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0500, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_005";
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("");
    std::string deviceId("deviceId");
    int32_t reasonResId = -1;
    // case  InitObjectLocaleReason bundleName=""
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);

    EXPECT_EQ(callback->onResult_, 99);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0600, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_006";
    auto timeFast = std::chrono::milliseconds(200);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("bundleName");
    std::string deviceId("deviceId");
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    // case  InitObjectLocaleReason reasonResId_ < 0)
    int32_t reasonResId = -1;
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);
    EXPECT_EQ(callback->onResult_, 99);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}

HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0700, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_007";
    auto timeFast = std::chrono::milliseconds(2000);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("bundleName");
    std::string deviceId("deviceId");
    int32_t reasonResId = -1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    //  case now  InitObjectLocaleReason reasonString_ is  not empty  ,will be request command
    reasonResId = 1;
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);

    EXPECT_EQ(callback->onResult_, 100);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
// timeout run,but  already handle
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0800, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_008";
    auto timeFast = std::chrono::milliseconds(64000);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("bundleName");
    std::string deviceId("deviceId");
    int32_t reasonResId = 1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    //  case now  InitObjectLocaleReason reasonString_ is  not empty  ,will be request command
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);

    EXPECT_EQ(callback->onResult_, 99);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
// timeout
HWTEST_F(RequestPermissionsFromRemoteTest, request_permissionsservice_from_remote_test_0900, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_permissionsservice_from_remote_test_009";
    auto timeFast = std::chrono::milliseconds(64000);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("bundleName");
    std::string deviceId("deviceIdTimeout");
    int32_t reasonResId = 1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    PERMISSION_LOG_INFO(LABEL, "onResult_ : %{public}d,", callback->onResult_);

    EXPECT_EQ(callback->onResult_, 100);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 99);
}
// requestResultForCancel requestId  is not exist and have been processed
HWTEST_F(RequestPermissionsFromRemoteTest, request_result_for_cancel_test_0100, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_result_for_cancel_test_001";
    auto timeFast = std::chrono::milliseconds(2000);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("bundleName");
    std::string deviceId("deviceIdCancelExit");
    int32_t reasonResId = 1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    // will sleep 60s
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    std::string requestId = "requestId";
    RequestRemotePermission::GetInstance().RequestResultForCancel(requestId);

    EXPECT_EQ(callback->onResult_, 100);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
// requestResultForCancel
HWTEST_F(RequestPermissionsFromRemoteTest, request_result_for_cancel_test_0200, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "request_result_for_cancel_test_002";
    auto timeFast = std::chrono::milliseconds(2000);
    std::vector<std::string> permissions;
    permissions.push_back(Constant::LOCATION);
    OHOS::sptr<TestCallbackA> callback(new TestCallbackA());
    std::string nodeId("networkId");
    std::string bundleName("bundleName");
    std::string deviceId("deviceIdCancel ");
    int32_t reasonResId = 1;
    DeviceInfoRepository::GetInstance().SaveDeviceInfo(
        nodeId, "universallyUniqueId", deviceId, "deviceName", "deviceType");
    // will sleep 60s
    service->RequestPermissionsFromRemote(permissions, callback, nodeId, bundleName, reasonResId);
    std::this_thread::sleep_for(timeFast);
    // cancel task
    RequestRemotePermission::GetInstance().RequestResultForCancel(deviceId);
    std::this_thread::sleep_for(timeFast);

    EXPECT_EQ(callback->onResult_, 100);
    EXPECT_EQ(callback->onCancel_, 100);
    EXPECT_EQ(callback->onTimeOut_, 100);
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS