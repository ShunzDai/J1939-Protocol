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
#ifndef J1939_PORT_H
#define J1939_PORT_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"
#include "src/message/j1939_message.h"
#include "j1939_config.h"
#if __J1939_Port(SUSPEND)
#include "j1939_virtual.h"
typedef J1939_VirtualNode_t J1939_Port_t;
#elif __J1939_Port(VIRTUAL)
#include "j1939_virtual.h"
#include <sys/time.h>
#include <time.h>
typedef J1939_VirtualNode_t J1939_Port_t;
#elif __J1939_Port(STM32)
#include "can.h"
typedef CAN_HandleTypeDef J1939_Port_t;
#else /* __J1939_Port() */
#error "Incompatible port"
#endif /* __J1939_Port() */

J1939_Status_t J1939_PortInit(J1939_Port_t *Port);
J1939_Status_t J1939_PortDeInit(J1939_Port_t *Port);
uint32_t J1939_PortGetTxMailboxesFreeLevel(J1939_Port_t *Port);
uint32_t J1939_PortGetRxFifoFillLevel(J1939_Port_t *Port);
J1939_Status_t J1939_PortAddTxMessage(J1939_Port_t *Port, J1939_Message_t Msg);
J1939_Status_t J1939_PortGetRxMessage(J1939_Port_t *Port, J1939_Message_t *MsgPtr);
uint64_t J1939_PortGetTick(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_PORT_H */
