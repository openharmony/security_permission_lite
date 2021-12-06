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
#include <securec.h>
#include "session.h"
#include "constant.h"
#include "soft_bus_manager.h"
#include "permission_log.h"
#include "soft_bus_session_listener.h"
#include "soft_bus_channel.h"
using namespace OHOS::Security::Permission;
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "SoftBusSenssionMock"};
static const int SESSION_COUNT_LIMIT = 20;
static const int SERVER_COUNT_LIMIT = 10;
} // namespace

#define MIN_(x, y) ((x) < (y)) ? (x) : (y)

static int serverCount_ = -1;
bool IsServerCountOK()
{
    return serverCount_ >= 0 && serverCount_ < SERVER_COUNT_LIMIT;
}

static ISessionListener *listener_ = nullptr;
int CreateSessionServer(const char *pkgName, const char *sessionName, const ISessionListener *listener)
{
    PERMISSION_LOG_DEBUG(LABEL, "pkg name: %{public}s", pkgName);
    PERMISSION_LOG_DEBUG(LABEL, "sessionName: %{public}s", sessionName);
    serverCount_++;
    if (IsServerCountOK()) {
        listener_ = const_cast<ISessionListener *>(listener);
        PERMISSION_LOG_DEBUG(LABEL, "success, server count: %{public}d", serverCount_);
        return Constant::SUCCESS;
    }

    PERMISSION_LOG_DEBUG(LABEL, "failure, server count: %{public}d", serverCount_);
    return Constant::FAILURE;
}
int RemoveSessionServer(const char *pkgName, const char *sessionName)
{
    PERMISSION_LOG_DEBUG(LABEL, "pkg name: %{public}s", pkgName);
    PERMISSION_LOG_DEBUG(LABEL, "sessionName: %{public}s", sessionName);
    if (IsServerCountOK()) {
        serverCount_--;
        listener_ = nullptr;
        PERMISSION_LOG_DEBUG(LABEL, "success, server count: %{public}d", serverCount_);
        return Constant::SUCCESS;
    }

    if (serverCount_ >= 0) {
        serverCount_--;
    }
    PERMISSION_LOG_DEBUG(LABEL, "failure, server count: %{public}d", serverCount_);
    return Constant::FAILURE;
}

static int sessionCount_ = -1;
bool IsSessionCountOK()
{
    return sessionCount_ >= 0 && sessionCount_ < SESSION_COUNT_LIMIT;
    PERMISSION_LOG_DEBUG(LABEL, "SESSION_COUNT_LIMIT: %{public}d", SESSION_COUNT_LIMIT);
}

int OpenSession(const char *mySessionName, const char *peerSessionName, const char *peerDeviceId, const char *groupId,
    const SessionAttribute *attr)
{
    PERMISSION_LOG_DEBUG(LABEL, "mySessionName: %{public}s", mySessionName);
    PERMISSION_LOG_DEBUG(LABEL, "peerSessionName: %{public}s", peerSessionName);
    PERMISSION_LOG_DEBUG(LABEL, "peerDeviceId: %{public}s", peerDeviceId);
    PERMISSION_LOG_DEBUG(LABEL, "groupId: %{public}s", groupId);
    if (serverCount_ == -1) {
        PERMISSION_LOG_DEBUG(LABEL, "failure, server count: %{public}d", serverCount_);
        return Constant::INVALID_SESSION;
    }

    sessionCount_++;
    if (IsSessionCountOK()) {
        SoftBusSessionListener::OnSessionOpened(1, Constant::SUCCESS);
        PERMISSION_LOG_DEBUG(LABEL, "success, session count: %{public}d", sessionCount_);
        return 1;
    }

    PERMISSION_LOG_DEBUG(LABEL, "failure, session count: %{public}d", sessionCount_);
    return Constant::INVALID_SESSION;
}
void CloseSession(int sessionId)
{
    PERMISSION_LOG_DEBUG(LABEL, "sessionId: %{public}d", sessionId);
    if (sessionCount_ >= 0) {
        sessionCount_--;
        PERMISSION_LOG_DEBUG(LABEL, "success, session count: %{public}d", sessionCount_);
    }
}

int SendBytes(int sessionId, const void *data, unsigned int len)
{
    PERMISSION_LOG_DEBUG(LABEL, "len: %{public}d", len);
    if (sessionId == Constant::INVALID_SESSION) {
        return Constant::FAILURE;
    }
    DecompressMock((unsigned char *) data, len);
    return Constant::SUCCESS;
}

int GetPeerSessionName(int sessionId, char *sessionName, unsigned int len)
{
    if (sessionId == Constant::INVALID_SESSION) {
        return Constant::FAILURE;
    }
    std::string x;
    if (sessionId < SERVER_COUNT_LIMIT) {
        x = SoftBusManager::SESSION_NAME;
    } else {
        x = "sessionid-" + std::to_string(sessionId);
    }
    if (len < x.length()) {
        return Constant::FAILURE;
    }
    if (memcpy_s(sessionName, x.length(), x.c_str(), x.length()) != EOK) {
        return Constant::FAILURE;
    }
    sessionName[x.length()] = '\0';
    PERMISSION_LOG_DEBUG(LABEL, "success, session name: %{public}s", sessionName);

    return 0;
}

int GetPeerDeviceId(int sessionId, char *devId, unsigned int len)
{
    if (sessionId == Constant::INVALID_SESSION) {
        return Constant::FAILURE;
    }

    std::string x = "deviceid-" + std::to_string(sessionId);
    if (len < x.length()) {
        return Constant::FAILURE;
    }

    if (memcpy_s(devId, x.length(), x.c_str(), x.length()) != EOK) {
        return Constant::FAILURE;
    }
    devId[x.length()] = '\0';
    return 0;
}

static std::string uuid = "";
void DecompressMock(const unsigned char *bytes, const int length)
{
    PERMISSION_LOG_DEBUG(LABEL, "input length: %{public}d", length);
    uLong len = 1048576;
    unsigned char *buf = (unsigned char *) malloc(len + 1);
    if (buf == nullptr) {
        PERMISSION_LOG_ERROR(LABEL, "no enough memory!");
        return;
    }
    memset_s(buf, len + 1, 0, len + 1);
    int result = uncompress(buf, &len, (unsigned char *) bytes, length);
    if (result != Z_OK) {
        PERMISSION_LOG_ERROR(LABEL,
            "uncompress failed, error code: %{public}d, bound length: %{public}d, buffer length: %{public}d", result,
            (int) len, length);
        free(buf);
        return;
    }
    buf[len] = '\0';
    std::string str((char *) buf);
    free(buf);
    PERMISSION_LOG_DEBUG(LABEL, "done, output: %{public}s", str.c_str());

    int id_post = str.find("\"id\":");

    std::string id_string = str.substr(id_post + 6, 9);
    uuid = id_string;
    PERMISSION_LOG_DEBUG(LABEL, "id_string: %{public}s", id_string.c_str());
    return;
}

std::string GetUuidMock()
{
    PERMISSION_LOG_DEBUG(LABEL, "GetUuidMock called uuid: %{public}s", uuid.c_str());
    return uuid;
}