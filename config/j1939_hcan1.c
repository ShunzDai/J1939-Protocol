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

__weak J1939_EXTERN_FUNCTION_DEF(hcan1, SoftwareFilter){
  typedef struct Filter{
    uint8_t SelfAddress;
    J1939_Message_t Msg;
  } Filter_t;
  /* Config software filter rules START */
  if (((Filter_t *)argument)->Msg->PDU.PDUFormat >= J1939_ADDRESS_DIVIDE)/* PDU2 */
    return J1939_OK;
  else if (((Filter_t *)argument)->Msg->PDU.PDUSpecific == ((Filter_t *)argument)->SelfAddress)/* PDU1 */
    return J1939_OK;
  else if (((Filter_t *)argument)->Msg->PDU.PDUSpecific == J1939_ADDRESS_GLOBAL)/* Global address */
    return J1939_OK;
  else
    return J1939_ERROR;
  /* Config software filter rules END */
}

__weak J1939_EXTERN_FUNCTION_DEF(hcan1, DecodePayload){
  uint32_t PGN = J1939_GetPGN(((J1939_Message_t)argument)->ID);
  void *p = NULL;
  switch(PGN){
    /* User define START */

    /* User define END */
    default:
      J1939_LOG_WARN("[Decode]fail to identify(0x%06X)", PGN);
      return J1939_ERROR;
  }

  if (p != NULL){
    J1939_memcpy(p, ((J1939_Message_t)argument)->Payload, ((J1939_Message_t)argument)->Length);
    return J1939_OK;
  }
  J1939_LOG_ERROR("A null pointer appears");
  return J1939_ERROR;
}

/**
  * @brief This function handles message sending interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan1, SendingCallBack){
  /* User define START */
  J1939_LOG_INFO("'hcan1' Message sending PGN 0x%06X", J1939_GetPGN(((J1939_Message_t)argument)->ID));
  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message missing interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan1, MissingCallBack){
  /* User define START */
  J1939_LOG_ERROR("'hcan1' Message missing(PGN 0x%06X)", J1939_GetPGN(((J1939_Message_t)argument)->ID));

  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message reading interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan1, ReadingCallBack){
  /* User define START */

  /* User define END */
  return J1939_OK;
};

/**
  * @brief This function handles message timeout interrupt.
  */
__weak J1939_EXTERN_FUNCTION_DEF(hcan1, TimeoutCallBack){
  /* User define START */
  J1939_LOG_ERROR("'hcan1' timeout(PGN 0x%06X)", J1939_GetPGN(((J1939_Message_t)argument)->ID));
  /* User define END */
  return J1939_OK;
};
