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
#ifndef J1939_SRC_J1939_H
#define J1939_SRC_J1939_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"

struct J1939;

typedef struct J1939 * J1939_t;

J1939_t J1939_HandleCreate(char *Name, uint8_t SelfAddress, uint32_t QueueSize);
J1939_Status_t J1939_HandleDelete(J1939_t *Handle);

char *J1939_GetPortName(J1939_t Handle);
uint8_t J1939_GetSelfAddress(J1939_t Handle);
J1939_Status_t J1939_SetSelfAddress(J1939_t Handle, uint8_t SelfAddress);
J1939_Status_t J1939_GetProtocolStatus(J1939_t Handle);

J1939_Status_t J1939_TaskHandler(void);
J1939_Status_t J1939_SendMessage(J1939_t Handle, const uint32_t ID, const uint16_t Length, const void *Payload);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_J1939_H */
