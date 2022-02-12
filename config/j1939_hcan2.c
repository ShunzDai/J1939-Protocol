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
#include "src/common/j1939_typedef.h"
#include "src/port/j1939_memory.h"
#include "j1939_config.h"
#include "j1939.h"

__weak J1939_EXTERN_FUNCTION_DEF(hcan2, SoftwareFilter){
  typedef struct J1939_Filter{
    uint8_t SelfAddress;
    J1939_Message_t Msg;
  } * J1939_Filter_t;
  J1939_Filter_t Pack = (J1939_Filter_t)argument;
  /* Config software filter rules START */
  if (Pack->Msg->PDU.PDUFormat >= J1939_ADDRESS_DIVIDE)/* PDU2 */
    return J1939_OK;
  else if (Pack->Msg->PDU.PDUSpecific == Pack->SelfAddress)/* PDU1 */
    return J1939_OK;
  else if (Pack->Msg->PDU.PDUSpecific == J1939_ADDRESS_GLOBAL)/* Global address */
    return J1939_OK;
  else
    return J1939_ERROR;
  /* Config software filter rules END */
}

__weak J1939_EXTERN_FUNCTION_DEF(hcan2, DecodePayload){
  J1939_Message_t Msg = (J1939_Message_t)argument;
  /* User define START */
  switch(J1939_GetPGN(Msg->ID)){

    default:
      J1939_LOG("[hcan2]RX 0x%08X ", Msg->ID);
      for (uint32_t i = 0; i < Msg->Length; i++)
        J1939_LOG("%02X ", Msg->Payload[i]);
      J1939_LOG("\r\n");
      J1939_LOG_WARN("[hcan2]fail to identify(0x%08X)", Msg->ID);
      return J1939_ERROR;
  }
  /* User define END */
}

/**
  * @brief This function handles message sending interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan2, SendingCallBack){
  J1939_Message_t Msg = (J1939_Message_t)argument;
  /* User define START */
  J1939_LOG("[hcan2]TX 0x%08X ", Msg->ID);
  for (uint32_t i = 0; i < Msg->Length; i++)
    J1939_LOG("%02X ", Msg->Payload[i]);
  J1939_LOG("\r\n");
  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message missing interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan2, MissingCallBack){
  J1939_Message_t Msg = (J1939_Message_t)argument;
  /* User define START */
  J1939_LOG_ERROR("[hcan2]TX Message missing(0x%08X)", Msg->ID);
  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message reading interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan2, ReadingCallBack){
  J1939_Message_t Msg = (J1939_Message_t)argument;
  /* User define START */

  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message timeout interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan2, TimeoutCallBack){
  J1939_Message_t Msg = (J1939_Message_t)argument;
  /* User define START */
  J1939_LOG_ERROR("[hcan2]timeout(PGN 0x%08X)", Msg->ID);
  /* User define END */
  return J1939_OK;
};