/**
  * Copyright 2022 ShunzDai
  *
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
#ifndef J1939_SRC_MEMORY_H
#define J1939_SRC_MEMORY_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"

void *J1939_malloc(size_t _Size);
void J1939_free(void *_Block);
void *J1939_memset(void *_Dst, uint8_t _Val, size_t _Size);
void *J1939_memcpy(void *_Dst, const void *_Src, size_t _Size);
int J1939_strcmp(const char *Str1,const char *Str2);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_MEMORY_H */
