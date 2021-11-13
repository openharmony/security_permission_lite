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

#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <cstring>
#include <string>
#include <zlib.h>

#include "base64_util.h"

namespace OHOS {
namespace Security {
namespace Permission {
class ZipUtil {
public:
    static bool ZipCompress(
        const std::string &input, const unsigned long tlen, unsigned char *output, unsigned long &len);

    static bool ZipUnCompress(
        const unsigned char *input, const unsigned long tlen, std::string &output, unsigned long len);

    static bool CompressCode(
        const std::string &input, unsigned long &codeLen, unsigned long &zipLen, std::string &output);

    static bool UnCompressDeCode(
        const std::string &input, unsigned long &codeLen, unsigned long &zipLen, std::string &output);
};
}  // namespace Permission
}  // namespace Security
}  // namespace OHOS
#endif
