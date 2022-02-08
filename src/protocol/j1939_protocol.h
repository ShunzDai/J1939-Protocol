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
#ifndef J1939_SRC_PROTOCOL_H
#define J1939_SRC_PROTOCOL_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"
#include "src/j1939.h"

struct J1939_Protocol;

typedef struct J1939_Protocol * J1939_Protocol_t;

J1939_Protocol_t J1939_ProtocolCreate(void);
J1939_Status_t J1939_ProtocolDelete(J1939_Protocol_t *Protocol);

J1939_Status_t J1939_ProtocolTransmitManager(J1939_Protocol_t Protocol, J1939_Message_t Msg);
J1939_Status_t J1939_ProtocolReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg);
J1939_Status_t J1939_ProtocolTaskHandler(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_PROTOCOL_H */
