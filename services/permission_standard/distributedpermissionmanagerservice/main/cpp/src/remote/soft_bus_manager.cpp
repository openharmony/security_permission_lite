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

#include "soft_bus_manager.h"
#include "parameter.h"
#include "device_info_manager.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "SoftBusManager"};
}
namespace {
static const std::string TASK_NAME_CREATE_SESSION = "dpms_taskname_create_session_server";
static const std::string TASK_NAME_FULFILL_DEVICE_INFO = "dpms_taskname_fulfill_device_info";
static const std::string SESSION_GROUP_ID = "dpms_dsoftbus_session_group_id";
static const SessionAttribute SESSION_ATTR = {.dataType = TYPE_BYTES};

// TODO need REASON_EXIST?
static const int REASON_EXIST = -3;
// static const int REGISTER_RETRY_TIMES = 5;
// static const int REGISTER_RETRY_INTERVAL_MS = 500;
// static const std::string PACKAGE_NAME = "dpms";
static const int OPENSESSION_RETRY_TIMES = 10 * 60;
static const int OPENSESSION_RETRY_INTERVAL_MS = 100;
}  // namespace

const std::string SoftBusManager::DPMS_PACKAGE_NAME = "ohos.security.distributed_permission";
const std::string SoftBusManager::SESSION_NAME = "com.huawei.systemserver_CHANNEL_DPMS";

SoftBusManager::SoftBusManager() : isSoftBusServiceBindSuccess_(false), inited_(false), mutex_(), fulfillMutex_()
{
    PERMISSION_LOG_DEBUG(LABEL, "SoftBusManager()");
}

SoftBusManager::~SoftBusManager()
{
    PERMISSION_LOG_DEBUG(LABEL, "~SoftBusManager()");
}

SoftBusManager &SoftBusManager::GetInstance()
{
    static SoftBusManager instance;
    return instance;
}

void SoftBusManager::Initialize()
{
    // TODO old code
    // if (!ptrDeviceManager_) {
    //     // TODO serviceConnectionListener
    //     ptrDeviceManager_ = std::make_shared<DeviceManager>();
    //     // maybe failed
    //     // ptrDeviceManager_->QueryOnlineDevice();
    // }

    bool inited = false;
    // cas failed means already inited.
    if (!inited_.compare_exchange_strong(inited, true)) {
        PERMISSION_LOG_DEBUG(LABEL, "already initialized, skip");
        return;
    }

    std::function<void()> runner = [&]() {
        // TODO device manager
        // deviceManager.bindDevMonitor();

        std::unique_lock<std::mutex> lock(mutex_);

        // register device state change listener
        // ref: distributeddataservice/adapter/communicator/src/softbus_adapter_standard.cpp
        std::string packageName = DPMS_PACKAGE_NAME;
        std::shared_ptr<MyDmInitCallback> ptrDmInitCallback = std::make_shared<MyDmInitCallback>();
        int ret = DistributedHardware::DeviceManager::GetInstance().InitDeviceManager(packageName, ptrDmInitCallback);
        if (ret != ERR_OK) {
            PERMISSION_LOG_ERROR(LABEL, "Initialize: InitDeviceManager error, result: %{public}d", ret);
            inited_.store(false);
            return;
        }

        std::string extra = "";
        std::shared_ptr<SoftBusDeviceConnectionListener> ptrDeviceStateCallback =
            std::make_shared<SoftBusDeviceConnectionListener>();
        ret = DistributedHardware::DeviceManager::GetInstance().RegisterDevStateCallback(
            packageName, extra, ptrDeviceStateCallback);
        if (ret != ERR_OK) {
            PERMISSION_LOG_ERROR(LABEL, "Initialize: RegisterDevStateCallback error, result: %{public}d", ret);
            inited_.store(false);
            return;
        }

        // register session listener
        ISessionListener sessionListener;
        sessionListener.OnSessionOpened = SoftBusSessionListener::OnSessionOpened;
        sessionListener.OnSessionClosed = SoftBusSessionListener::OnSessionClosed;
        sessionListener.OnBytesReceived = SoftBusSessionListener::OnBytesReceived;
        sessionListener.OnMessageReceived = SoftBusSessionListener::OnMessageReceived;

        ret = ::CreateSessionServer(DPMS_PACKAGE_NAME.c_str(), SESSION_NAME.c_str(), &sessionListener);
        PERMISSION_LOG_INFO(LABEL, "Initialize: createSessionServer, result: %{public}d", ret);
        // TODO REASON_EXIST?
        if ((ret != Constant::SUCCESS) && (ret != REASON_EXIST)) {
            PERMISSION_LOG_ERROR(LABEL, "Initialize: CreateSessionServer error, result: %{public}d", ret);
            // init failed.
            Destroy();
            inited_.store(false);
            return;
        }

        isSoftBusServiceBindSuccess_ = true;
        this->FulfillLocalDeviceInfo();
    };

    std::thread initThread(runner);
    initThread.detach();
    PERMISSION_LOG_DEBUG(LABEL, "Initialize thread started");
}

void SoftBusManager::Destroy()
{
    PERMISSION_LOG_DEBUG(LABEL,
        "destroy, init: %{public}d, isSoftBusServiceBindSuccess: %{public}d",
        inited_.load(),
        isSoftBusServiceBindSuccess_);

    if (inited_.load() == false) {
        PERMISSION_LOG_DEBUG(LABEL, "not inited, skip");
        return;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    if (inited_.load() == false) {
        PERMISSION_LOG_DEBUG(LABEL, "not inited, skip");
        return;
    }

    if (isSoftBusServiceBindSuccess_) {
        int32_t ret = ::RemoveSessionServer(DPMS_PACKAGE_NAME.c_str(), SESSION_NAME.c_str());
        PERMISSION_LOG_DEBUG(LABEL, "destroy, RemoveSessionServer: %{public}d", ret);
        isSoftBusServiceBindSuccess_ = false;
    }

    // deviceManager.unbindDevMonitor();
    std::string packageName = DPMS_PACKAGE_NAME;
    int ret = DistributedHardware::DeviceManager::GetInstance().UnRegisterDevStateCallback(packageName);
    if (ret != ERR_OK) {
        PERMISSION_LOG_ERROR(LABEL, "UnRegisterDevStateCallback failed, code: %{public}d", ret);
    }
    ret = DistributedHardware::DeviceManager::GetInstance().UnInitDeviceManager(packageName);

    if (ret != ERR_OK) {
        PERMISSION_LOG_ERROR(LABEL, "UnInitDeviceManager failed, code: %{public}d", ret);
    }

    inited_.store(false);

    PERMISSION_LOG_DEBUG(LABEL, "destroy, done");
}

int32_t SoftBusManager::OpenSession(const std::string &deviceId)
{
    DeviceInfo info;
    bool result = DeviceInfoManager::GetInstance().GetDeviceInfo(deviceId, DeviceIdType::UNKNOWN, info);
    if (result == false) {
        PERMISSION_LOG_WARN(LABEL, "device info notfound for deviceId %{public}s", deviceId.c_str());
        return Constant::FAILURE;
    }
    std::string networkId = info.deviceId.networkId;
    PERMISSION_LOG_INFO(LABEL, "openSession, networkId: %{public}s", networkId.c_str());

    // async open session, should waitting for OnSessionOpened event.
    int sessionId = ::OpenSession(
        SESSION_NAME.c_str(), SESSION_NAME.c_str(), networkId.c_str(), SESSION_GROUP_ID.c_str(), &SESSION_ATTR);

    PERMISSION_LOG_DEBUG(LABEL,
        "session info: sessionId: %{public}d, uuid: %{public}s, udid: %{public}s",
        sessionId,
        info.deviceId.universallyUniqueId.c_str(),
        info.deviceId.uniqueDisabilityId.c_str());

    // wait session opening
    int retryTimes = 0;
    int logSpan = 10;
    auto sleepTime = std::chrono::milliseconds(OPENSESSION_RETRY_INTERVAL_MS);
    while (retryTimes++ < OPENSESSION_RETRY_TIMES) {
        if (SoftBusSessionListener::GetSessionState(sessionId) < 0) {
            std::this_thread::sleep_for(sleepTime);
            if (retryTimes % logSpan == 0) {
                PERMISSION_LOG_INFO(
                    LABEL, "openSession, waitting for: %{public}d ms", retryTimes * OPENSESSION_RETRY_INTERVAL_MS);
            }
            continue;
        }
        break;
    }
    int64_t state = SoftBusSessionListener::GetSessionState(sessionId);
    if (state < 0) {
        PERMISSION_LOG_ERROR(LABEL, "openSession, timeout, session: %{public}lld", state);
        return Constant::FAILURE;
    }

    PERMISSION_LOG_DEBUG(LABEL, "openSession, succeed, session: %{public}lld", state);
    return sessionId;
}

int SoftBusManager::CloseSession(int sessionId)
{
    if (sessionId < 0) {
        PERMISSION_LOG_INFO(LABEL, "closeSession: session is invalid");
        return Constant::FAILURE;
    }

    ::CloseSession(sessionId);
    PERMISSION_LOG_INFO(LABEL, "closeSession ");
    return Constant::SUCCESS;
}

std::string SoftBusManager::GetUniversallyUniqueIdByNodeId(const std::string &nodeId)
{
    if (!DistributedDataValidator::IsDeviceIdValid(nodeId)) {
        PERMISSION_LOG_ERROR(LABEL, "invalid nodeId: %{public}s", nodeId.c_str());
        return "";
    }

    std::string uuid = GetUuidByNodeId(nodeId);
    if (uuid.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "softbus return null or empty string [%{public}s]", uuid.c_str());
        return "";
    }

    DeviceInfo info;
    bool result = DeviceInfoManager::GetInstance().GetDeviceInfo(uuid, DeviceIdType::UNIVERSALLY_UNIQUE_ID, info);
    if (result == false) {
        PERMISSION_LOG_DEBUG(LABEL, "local device info not found for uuid %{public}s", uuid.c_str());
    } else {
        std::string dimUuid = info.deviceId.universallyUniqueId;
        if (uuid == dimUuid) {
            // refresh cache
            // std::shared_ptr<DistributedPermissionEventHandler> handler =
            //     DelayedSingleton<DistributedPermissionManagerService>::GetInstance()->GetEventHandler();
            // if (handler == nullptr) {
            //     PERMISSION_LOG_ERROR(LABEL,"fail to get EventHandler");
            //     return "";
            // }
            std::function<void()> fulfillDeviceInfo = std::bind(&SoftBusManager::FulfillLocalDeviceInfo, this);
            // handler->PostTask(fulfillDeviceInfo, TASK_NAME_FULFILL_DEVICE_INFO);
            std::thread fulfill(fulfillDeviceInfo);
            fulfill.detach();
        }
    }

    return uuid;
}

std::string SoftBusManager::GetUniqueDisabilityIdByNodeId(const std::string &nodeId)
{
    if (!DistributedDataValidator::IsDeviceIdValid(nodeId)) {
        PERMISSION_LOG_ERROR(LABEL, "invalid nodeId: %{public}s", nodeId.c_str());
        return "";
    }
    std::string udid = GetUdidByNodeId(nodeId);
    if (udid.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "softbus return null or empty string: %{public}s", udid.c_str());
        return "";
    }
    char localUdid[Constant::DEVICE_UUID_LENGTH] = {0};
    // int result = ::GetDevUdid(localUdid, Constant::DEVICE_UUID_LENGTH);
    ::GetDevUdid(localUdid, Constant::DEVICE_UUID_LENGTH);
    if (udid == localUdid) {
        // refresh cache
        // std::shared_ptr<DistributedPermissionEventHandler> handler =
        //     DelayedSingleton<DistributedPermissionManagerService>::GetInstance()->GetEventHandler();
        // if (handler == nullptr) {
        //     PERMISSION_LOG_ERROR(LABEL,"fail to get EventHandler");
        //     return "";
        // }
        std::function<void()> fulfillDeviceInfo = std::bind(&SoftBusManager::FulfillLocalDeviceInfo, this);
        // handler->PostTask(fulfillDeviceInfo, TASK_NAME_FULFILL_DEVICE_INFO);
        std::thread fulfill(fulfillDeviceInfo);
        fulfill.detach();
    }
    return udid;

    // TODO old code
    // auto idnet = DnetworkAdapter::GetInstance();
    //     if (idnet == nullptr) {
    //         PERMISSION_LOG_ERROR(LABEL, "GetDnetService error");
    //         return "";
    //     }
    //     return idnet->GetUdidByNodeId(nodeId);
}

std::string SoftBusManager::GetUuidByNodeId(const std::string &nodeId) const
{
    // TODO device manager
    //  long identity = Binder.clearCallingIdentity();
    // try {
    //     return Optional.ofNullable(deviceManager.getUuidByNodeId(nodeId));
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }

    // TODO old code
    //     std::shared_ptr<Communication::DnetworkAdapter> adapter = Communication::DnetworkAdapter::GetInstance();
    // std::string udid = adapter->GetUdidByNodeId(networkId);
    // return udid;

    // TODO udid/uuid max length?
    int len = 128;
    uint8_t *info = (uint8_t *)malloc(len + 1);
    memset(info, 0, len + 1);
    if (info == nullptr) {
        PERMISSION_LOG_ERROR(LABEL, "no enough memory: %{public}d", len);
        return "";
    }
    int32_t ret =
        ::GetNodeKeyInfo(DPMS_PACKAGE_NAME.c_str(), nodeId.c_str(), NodeDeivceInfoKey::NODE_KEY_UUID, info, len);
    if (ret != Constant::SUCCESS) {
        free(info);
        PERMISSION_LOG_WARN(LABEL, "GetNodeKeyInfo error, return code: %{public}d", ret);
        return "";
    }
    std::string uuid((char *)info);
    free(info);
    PERMISSION_LOG_DEBUG(
        LABEL, "call softbus finished. nodeId(in): %{public}s, uuid: %{public}s", nodeId.c_str(), uuid.c_str());
    return uuid;
}

std::string SoftBusManager::GetUdidByNodeId(const std::string &nodeId) const
{
    // TODO device manager
    //  long identity = Binder.clearCallingIdentity();
    //     try {
    //         return Optional.ofNullable(deviceManager.getUdidByNodeId(nodeId));
    //     } finally {
    //         Binder.restoreCallingIdentity(identity);
    //     }

    // TODO old source?
    // auto adapter = Communication::DnetworkAdapter::GetInstance();
    // std::string udid = adapter->GetUdidByNodeId(networkId);
    // return udid;

    int len = 128;
    uint8_t *info = (uint8_t *)malloc(len + 1);
    memset(info, 0, len + 1);
    if (info == nullptr) {
        PERMISSION_LOG_ERROR(LABEL, "no enough memory: %{public}d", len);
        return "";
    }
    int32_t ret =
        ::GetNodeKeyInfo(DPMS_PACKAGE_NAME.c_str(), nodeId.c_str(), NodeDeivceInfoKey::NODE_KEY_UDID, info, len);
    if (ret != Constant::SUCCESS) {
        free(info);
        PERMISSION_LOG_WARN(LABEL, "GetNodeKeyInfo error, code: %{public}d", ret);
        return "";
    }
    std::string udid((char *)info);
    free(info);
    PERMISSION_LOG_DEBUG(
        LABEL, "call softbus finished: nodeId(in): %{public}s, udid: %{public}s", nodeId.c_str(), udid.c_str());
    return udid;
}

int SoftBusManager::FulfillLocalDeviceInfo()
{
    // TODO L3 code
    // Optional<NodeBasicInfo> localInfoOpt = getLocalBasicInfo();
    // if (!localInfoOpt.isPresent()) {
    //     HiLog.error(LABEL, "fulfillLocalDeviceInfo: localInfo is null from dnetwork, abort.");
    //     return;
    // }
    // NodeBasicInfo localInfo = localInfoOpt.get();
    // Optional<String> uuidOpt = getUuidByNodeId(localInfo.getNodeId());
    // Optional<String> udidOpt = getUdidByNodeId(localInfo.getNodeId());
    // if (!uuidOpt.isPresent() || !udidOpt.isPresent()) {
    //     HiLog.error(LABEL, "fulfillLocalDeviceInfo: uuidOpt or udidOpt is not present, abort.");
    //     return;
    // }
    // DeviceInfoManager.getInstance()
    //     .addDeviceInfo(localInfo.getNodeId(), uuidOpt.get(), udidOpt.get(), localInfo.getDeviceName(),
    //         Objects.toString(localInfo.getDeviceType()));

    //     // TODO old code
    //     std::shared_ptr<Communication::DnetworkAdapter> adapter = Communication::DnetworkAdapter::GetInstance();
    //     std::shared_ptr<Communication::NodeBasicInfo> localInfoOpt = adapter->GetLocalBasicInfo();
    //     if (localDevice == nullptr) {
    //         PERMISSION_LOG_ERROR(LABEL, "fulfillLocalDeviceInfo: localInfo is null from dnetwork, abort.");
    //         return;
    //     }
    //     std::string uuidOpt = getUuidByNodeId(localInfoOpt.getNodeId());
    //     std::string udidOpt = getUdidByNodeId(localInfoOpt.getNodeId());
    //     if (uuidOpt == nullptr || udidOpt == nullptr) {
    //         PERMISSION_LOG_ERROR(LABEL, "fulfillLocalDeviceInfo: uuidOpt or udidOpt is not present, abort.");
    //         return;
    //     }
    //     DeviceInfoManager.getInstance().addDeviceInfo(
    //         localDevice->GetNodeId(), uuidOpt, uuidOpt, localInfo.getDeviceName(), localInfo.getDeviceType());

    // repeated task will just skip
    if (!fulfillMutex_.try_lock()) {
        PERMISSION_LOG_INFO(LABEL, "FulfillLocalDeviceInfo already running, skip.");
        return Constant::SUCCESS;
    }

    NodeBasicInfo info;
    int32_t ret = ::GetLocalNodeDeviceInfo(DPMS_PACKAGE_NAME.c_str(), &info);
    if (ret != Constant::SUCCESS) {
        PERMISSION_LOG_ERROR(LABEL, "GetLocalNodeDeviceInfo error");
        fulfillMutex_.unlock();
        return Constant::FAILURE;
    }

    PERMISSION_LOG_DEBUG(LABEL,
        "call softbus finished, networkId:%{public}s, name:%{public}s, type:%{public}d",
        info.networkId,
        info.deviceName,
        info.deviceTypeId);

    std::string uuid = GetUuidByNodeId(info.networkId);
    std::string udid = GetUdidByNodeId(info.networkId);
    if (uuid.empty() || udid.empty()) {
        PERMISSION_LOG_ERROR(LABEL, "FulfillLocalDeviceInfo: uuid or udid is empty, abort.");
        fulfillMutex_.unlock();
        return Constant::FAILURE;
    }

    DeviceInfoManager::GetInstance().AddDeviceInfo(
        info.networkId, uuid, udid, info.deviceName, std::to_string(info.deviceTypeId));
    PERMISSION_LOG_DEBUG(LABEL,
        "AddDeviceInfo finished, networkId:%{public}s, uuid:%{public}s, udid:%{public}s",
        info.networkId,
        uuid.c_str(),
        udid.c_str());

    fulfillMutex_.unlock();
    return Constant::SUCCESS;
}

}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
