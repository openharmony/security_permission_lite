/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef PARSE_PERM_FLAGS_INT_H
#define PARSE_PERM_FLAGS_INT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Parse a full decimal int string (persist JSON permission flags).
 * Reject empty, junk, partial, overflow. Valid decimal ints are unchanged vs atoi.
 */
bool ParsePermFlagsInt(const char *text, int *out);

#ifdef __cplusplus
}
#endif

#endif /* PARSE_PERM_FLAGS_INT_H */
