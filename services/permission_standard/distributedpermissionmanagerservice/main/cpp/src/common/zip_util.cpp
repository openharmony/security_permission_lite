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

#include "permission_log.h"
#include "zip_util.h"

namespace OHOS {
namespace Security {
namespace Permission {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_PERMISSION, "ZipUtil"};
}
bool ZipUtil::ZipCompress(const std::string &input, const unsigned long tlen, unsigned char *output, unsigned long &len)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called, input: %{public}s", __func__, input.c_str());

    if (output == NULL) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: no enough memory!", __func__);
        return false;
    }
    if (compress(output, &len, (unsigned char *)input.c_str(), tlen) != Z_OK) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: compress failed!", __func__);
        return false;
    }

    PERMISSION_LOG_INFO(LABEL, "%{public}s done, output: %{public}s", __func__, output);
    return true;
}

bool ZipUtil::ZipUnCompress(
    const unsigned char *input, const unsigned long tlen, std::string &output, unsigned long len)
{
    PERMISSION_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (len <= 0) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: compress length less than 0!", __func__);
        return false;
    }
    unsigned char *buf = (unsigned char *)malloc(len + 1);
    if (buf == NULL) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: no enough memory!", __func__);
        return false;
    }
    if (uncompress(buf, &len, input, tlen) != Z_OK) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: uncompress failed!", __func__);
        free(buf);
        return false;
    }
    buf[len] = '\0';
    output = (char *)buf;
    free(buf);
    PERMISSION_LOG_INFO(LABEL, "%{public}s done, output: %{public}s", __func__, output.c_str());
    return true;
}

bool ZipUtil::CompressCode(const std::string &input, unsigned long &codeLen, unsigned long &zipLen, std::string &output)
{
    if (codeLen <= 0) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: compress length less than 0!", __func__);
        return false;
    }
    unsigned char *buf = (unsigned char *)malloc(codeLen + 1);
    if (buf == NULL) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: malloc fail!", __func__);
        return false;
    }
    if (!ZipUtil::ZipCompress(input, zipLen, buf, codeLen)) {
        if (buf != NULL) {
            free(buf);
            buf = NULL;
        }
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: compress fail!", __func__);
        return false;
    }
    Base64Util::Encode(buf, codeLen, output);
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    return true;
}

bool ZipUtil::UnCompressDeCode(
    const std::string &input, unsigned long &codeLen, unsigned long &zipLen, std::string &output)
{
    if (codeLen <= 0) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: compress length less than 0!", __func__);
        return false;
    }
    unsigned char *pOut = (unsigned char *)malloc(codeLen + 1);
    if (pOut == NULL) {
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: malloc fail!", __func__);
        return false;
    }
    Base64Util::Decode(input, pOut, codeLen);
    if (!ZipUtil::ZipUnCompress(pOut, codeLen, output, zipLen)) {
        if (pOut != NULL) {
            free(pOut);
            pOut = NULL;
        }
        PERMISSION_LOG_ERROR(LABEL, "%{public}s: uncompress fail!", __func__);
        return false;
    }
    if (pOut != NULL) {
        free(pOut);
        pOut = NULL;
    }
    return true;
}
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS