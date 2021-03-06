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
#include "j1939_message.h"
#include "j1939_config.h"
#include "src/memory/j1939_memory.h"

/**
  * @brief  Calculate PGN with CAN ID
  * @param  PDU CAN ID
  * @retval PGN value
  */
uint32_t J1939_GetPGN(uint32_t PDU){
  /* Reference SAE J1939-21 5.1.2 */
  return ((((J1939_PDU_t *)&PDU)->Reserved << 17 | ((J1939_PDU_t *)&PDU)->DataPage << 16)| (((J1939_PDU_t *)&PDU)->PDUFormat < J1939_ADDRESS_DIVIDE)) ?
         (((J1939_PDU_t *)&PDU)->PDUFormat << 8) : (((J1939_PDU_t *)&PDU)->PDUFormat << 8 | ((J1939_PDU_t *)&PDU)->PDUSpecific);
}

/**
  * @brief  Put PGN into the ID(When the PDU is PDU1, the PS bits of the ID will not changed)
  * @param  PDU CAN id container printer
  * @param  PGN PGN value
  * @retval J1939 status
  */
J1939_Status_t J1939_SetPGN(uint32_t *PDU, const uint32_t PGN){
  J1939_Assert(PDU != NULL);
  /* Reference SAE J1939-21 5.1.2 */
  ((J1939_PDU_t *)PDU)->Reserved = (PGN >> 17) & 0x01;
  ((J1939_PDU_t *)PDU)->DataPage = (PGN >> 16) & 0x01;
  ((J1939_PDU_t *)PDU)->PDUFormat = (PGN >> 8) & 0xFF;
  if (((J1939_PDU_t *)PDU)->PDUFormat >= J1939_ADDRESS_DIVIDE)
    ((J1939_PDU_t *)PDU)->PDUSpecific = (PGN >> 0) & 0xFF;
  return J1939_OK;
}

/**
  * @brief  Create a J1939 message by id, payload's length, and payload
  * @param  ID CAN ID
  * @param  Length Message length
  * @param  Payload A pointer to a payload, can be a null printer(will send 0x00)
  * @retval J1939 message
  */
J1939_Message_t J1939_MessageCreate(const uint32_t ID, const uint16_t Length, const void *Payload){
  J1939_Message_t Msg = (J1939_Message_t)J1939_malloc(sizeof(struct J1939_Message) + Length);
  Msg->Next = NULL;
  Msg->ID = ID;
  Msg->Length = Length;
  (Payload == NULL) ? J1939_memset(Msg->Payload, 0, Length) : J1939_memcpy(Msg->Payload, (uint8_t *)Payload, Length);
  return Msg;
}

/**
  * @brief  Delete a message
  * @param  MsgPtr J1939 message printer
  * @retval J1939 status
  */
J1939_Status_t J1939_MessageDelete(J1939_Message_t *MsgPtr){
  J1939_Assert(MsgPtr != NULL);
  if (*MsgPtr == NULL){
    return J1939_OK;
  }
  J1939_free(*MsgPtr);
  *MsgPtr = NULL;
  return J1939_OK;
}

/**
  * @brief  Deep copy a J1939 message
  * @param  Msg J1939 message
  * @retval J1939 message's copy
  */
J1939_Message_t J1939_MessageCopy(J1939_Message_t Msg){
  J1939_Assert(Msg != NULL);
  J1939_Message_t Cpy = J1939_MessageCreate(Msg->ID, Msg->Length, Msg->Payload);
  return Cpy;
}
