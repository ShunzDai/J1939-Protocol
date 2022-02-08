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
#ifndef J1939_SRC_MESSAGE_H
#define J1939_SRC_MESSAGE_H
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"

/* Protocol Data Unit struct */
typedef struct J1939_PDU{
  /* Reference SAE J1939-21 5.2 */
  uint32_t SourceAddress                  : 8;
  uint32_t PDUSpecific                    : 8;
  uint32_t PDUFormat                      : 8;
  uint32_t DataPage                       : 1;
  uint32_t Reserved                       : 1;
  uint32_t Priority                       : 3;
  //uint32_t ERR                            : 1;/* error message frame */
  //uint32_t RTR                            : 1;/* remote transmission request flag */
  //uint32_t EFF                            : 1;/* frame format flag */
  uint32_t                                : 3;
} J1939_PDU_t;

/* Message struct */
typedef struct J1939_Message{
  union{
    J1939_PDU_t PDU;
    uint32_t ID;
  };
  uint16_t Length;
  uint8_t *Payload;
} * J1939_Message_t;

uint32_t J1939_GetPGN(uint32_t PDU);
void J1939_SetPGN(uint32_t *PDU, const uint32_t PGN);

J1939_Message_t J1939_MessageCreate(const uint32_t ID, const uint16_t Length, const void *Payload);
J1939_Status_t J1939_MessageDelete(J1939_Message_t *MsgPtr);

J1939_Message_t J1939_MessageCopy(J1939_Message_t Msg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_MESSAGE_H */
