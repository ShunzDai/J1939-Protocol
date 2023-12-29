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
#ifndef J1939_CONFIG_H
#define J1939_CONFIG_H
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdint.h>

#define J1939_MOCK 1

#if defined J1939_MOCK
#define J1939_PORT_VIRTUAL
#elif defined ESP_PLATFORM
#define J1939_PORT_ESP32
#endif

#define J1939_SIZE_DATAFIELD 8

#define J1939_LOGI
#define J1939_LOGW
#define J1939_LOGE

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_CONFIG_H */
