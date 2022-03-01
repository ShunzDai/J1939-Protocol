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
#include "j1939.h"
#include "j1939_config.h"

J1939_WEAK J1939_Status_t J1939_AppSoftwareFilter(J1939_t Handle, J1939_Message_t Msg){
  /* Config software filter rules START */
  if (Msg->PDU.PDUFormat >= J1939_ADDRESS_DIVIDE)/* PDU2 */
    return J1939_OK;
  else if (Msg->PDU.PDUSpecific == J1939_GetSelfAddress(Handle))/* PDU1 */
    return J1939_OK;
  else if (Msg->PDU.PDUSpecific == J1939_ADDRESS_GLOBAL)/* Global address */
    return J1939_OK;
  else
    return J1939_ERROR;
  /* Config software filter rules END */
}

J1939_WEAK J1939_Status_t J1939_AppDecodePayload(J1939_t Handle, J1939_Message_t Msg){
  /* User define START */
  switch(J1939_GetPGN(Msg->ID)){

    default:
      J1939_LOG("[%s]RX 0x%08X ", J1939_GetPortName(Handle), Msg->ID);
      for (uint32_t i = 0; i < Msg->Length; i++)
        J1939_LOG("%02X ", Msg->Payload[i]);
      J1939_LOG("\r\n");
      J1939_LOG_WARN("[%s]fail to identify(0x%08X)", J1939_GetPortName(Handle), Msg->ID);
      return J1939_ERROR;
  }
  /* User define END */
}

/**
  * @brief This function handles message sending interrupt.
  */
J1939_WEAK J1939_Status_t J1939_AppSendingCallBack(J1939_t Handle, J1939_Message_t Msg){
  /* User define START */
  J1939_LOG("[%s]TX 0x%08X ", J1939_GetPortName(Handle), Msg->ID);
  for (uint32_t i = 0; i < Msg->Length; i++)
    J1939_LOG("%02X ", Msg->Payload[i]);
  J1939_LOG("\r\n");
  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message missing interrupt.
  */
J1939_WEAK J1939_Status_t J1939_AppMissingCallBack(J1939_t Handle, J1939_Message_t Msg){
  /* User define START */
  J1939_LOG_ERROR("[%s]TX Message missing(0x%08X)", J1939_GetPortName(Handle), Msg->ID);
  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message reading interrupt.
  */
J1939_WEAK J1939_Status_t J1939_AppReadingCallBack(J1939_t Handle, J1939_Message_t Msg){
  /* User define START */

  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message timeout interrupt.
  */
J1939_WEAK J1939_Status_t J1939_AppTimeoutCallBack(J1939_t Handle, J1939_Message_t Msg){
  /* User define START */
  J1939_LOG_ERROR("[%s]timeout(PGN 0x%08X)", J1939_GetPortName(Handle), Msg->ID);
  /* User define END */
  return J1939_OK;
};
