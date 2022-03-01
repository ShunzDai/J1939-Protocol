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
#ifndef J1939_SRC_TYPEDEF_H
#define J1939_SRC_TYPEDEF_H
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdint.h>
#include <string.h>

#if defined(_MSC_FULL_VER)
#define J1939_WEAK __weak
#elif defined(__ARMCC_VERSION)
#define J1939_WEAK   __attribute__((weak))
#elif defined(__GNUC__)
#define J1939_WEAK   __attribute__((weak))
#else
#define J1939_WEAK
#endif

/* J1939 common status */
typedef enum{
  J1939_OK                                  = 0x00U,
  J1939_ERROR                               = 0x01U,
  J1939_BUSY                                = 0x02U,
  J1939_TIMEOUT                             = 0x03U,
  J1939_TRANSMIT                            = 0x04U,
  J1939_RECEIVED                            = 0x05U,
}J1939_Status_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_TYPEDEF_H */
