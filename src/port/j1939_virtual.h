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
#ifndef J1939_SRC_VIRTUAL_H
#define J1939_SRC_VIRTUAL_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"
#include "src/common/J1939_queue.h"
#include "src/message/j1939_message.h"

struct J1939_VirtualNode;

typedef struct J1939_VirtualNode * J1939_VirtualNode_t;

#define J1939_PORT_VIRTUAL_EXTERN
#include "j1939_port.inc"

J1939_VirtualNode_t J1939_VirtualNodeCreate(void);
J1939_Status_t J1939_VirtualNodeDelete(J1939_VirtualNode_t *Virtual);

uint32_t J1939_VirtualGetTxMailboxesFreeLevel(J1939_VirtualNode_t Virtual);
uint32_t J1939_VirtualGetRxFifoFillLevel(J1939_VirtualNode_t Virtual);
J1939_Status_t J1939_VirtualAddTxMessage(J1939_VirtualNode_t Virtual, J1939_Message_t Msg);
J1939_Status_t J1939_VirtualGetRxMessage(J1939_VirtualNode_t Virtual, J1939_Message_t *MsgPtr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_VIRTUAL_H */
