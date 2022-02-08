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
#include "src/common/J1939_queue.h"
#include "src/port/j1939_memory.h"
#include "src/port/j1939_port.h"
#include "src/protocol/j1939_protocol.h"

#define J1939_EXTERN_FUNCTION(Key, FuncPtr)\
do{\
  J1939_EXTERN_FUNCTION_DEF(Key, FuncPtr);\
  Handle->FuncPtr = &J1939_##Key##_##FuncPtr;\
}while(0)

/* J1939 register struct */
typedef J1939_Queue_t * J1939_Register_t;

/* J1939 handle struct */
struct J1939{
  J1939_Port_t *Port;
  char *Name;
  uint8_t SelfAddress;
  J1939_Queue_t TxFIFO;
  J1939_FuncPtr_t SoftwareFilter;
  J1939_FuncPtr_t DecodePayload;
  J1939_FuncPtr_t SendingCallBack;
  J1939_FuncPtr_t MissingCallBack;
  J1939_FuncPtr_t ReadingCallBack;
  J1939_FuncPtr_t TimeoutCallBack;
  #if J1939_ENABLE_TRANSPORT_PROTOCOL
  J1939_Protocol_t Protocol;
  #endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */
};

static J1939_Status_t J1939_Transmit(J1939_t Handle);
static J1939_Status_t J1939_Receive(J1939_t Handle);
static J1939_Status_t J1939_TransmitSplit(J1939_t Handle, J1939_Message_t Msg);
static J1939_Status_t J1939_ReceiveSplit(J1939_t Handle, J1939_Message_t Msg);

/**
  * @brief  Check the software FIFO and transmit the message
  * @param  Handle J1939 handle
  * @retval J1939 status
  */
static J1939_Status_t J1939_Transmit(J1939_t Handle){
  /* Check TxFIFO */
  if (J1939_QueueCount(Handle->TxFIFO) == 0)
    return J1939_OK;
  /* Local variables init */
  uint32_t FreeLevel = J1939_PortGetTxMailboxesFreeLevel(Handle->Port);
  /* Make sure there are empty mailboxes */
  if (FreeLevel == 0)
    return J1939_BUSY;
  /* Fill all mailboxes untill hardware or software FIFO becomes empty  */
  while(FreeLevel--){
    J1939_Message_t Msg = J1939_QueueHead(Handle->TxFIFO);

    if (Msg == NULL){
      J1939_LOG_ERROR("[Send]A null pointer appears");
    }
    else if (Msg->Payload == NULL){
      J1939_LOG_ERROR("[Send]A null pointer appears");
    }

    if (J1939_PortAddTxMessage(Handle->Port, Msg) == J1939_OK)
      Handle->SendingCallBack(Msg);
    else
      Handle->MissingCallBack(Msg);

    J1939_Dequeue(Handle->TxFIFO, 1);

    if (J1939_QueueCount(Handle->TxFIFO) == 0)
      return J1939_OK;
  }
  /* Unreachable */
  return J1939_ERROR;
}

/**
  * @brief  Check the hardware FIFO and receive the message
  * @param  Handle J1939 handle
  * @retval J1939 status
  */
static J1939_Status_t J1939_Receive(J1939_t Handle){
  typedef struct Filter{
    uint8_t SelfAddress;
    J1939_Message_t Msg;
  } Filter_t;
  /* Check mailboxes */
  uint32_t FillLevel = J1939_PortGetRxFifoFillLevel(Handle->Port);
  if (FillLevel == 0)
    return J1939_OK;
  /* Create a mail buffer */
  J1939_Message_t Msg = NULL;
  /* Get all mails untill hardware or software FIFO becomes empty */
  while (FillLevel--){
    if (J1939_PortGetRxMessage(Handle->Port, &Msg) == J1939_OK){
      Handle->ReadingCallBack(Msg);
      Filter_t Pack = {Handle->SelfAddress, Msg};
      if (Handle->SoftwareFilter(&Pack) == J1939_OK)
        J1939_ReceiveSplit(Handle, Msg);
      J1939_MessageDelete(&Msg);
    }
    else{
      J1939_LOG_ERROR("Failed to get mail");
      if (Msg != NULL)
        J1939_MessageDelete(&Msg);
      return J1939_ERROR;
    }
  }
  return J1939_OK;
}

/**
  * @brief  Transmit split
  * @param  Handle J1939 handle
  * @param  Msg J1939 message
  * @retval J1939 status
  */
static J1939_Status_t J1939_TransmitSplit(J1939_t Handle, J1939_Message_t Msg){
  if (Msg->Length <= J1939_SIZE_CAN_BUFFER)
    return J1939_Enqueue(Handle->TxFIFO, Msg);
  #if J1939_ENABLE_TRANSPORT_PROTOCOL
  return J1939_ProtocolTransmitManager(Handle->Protocol, Msg);
  #else /* J1939_ENABLE_TRANSPORT_PROTOCOL */
  return J1939_ERROR;
  #endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */
}

/**
  * @brief  Receive split
  * @param  Handle J1939 handle
  * @param  Msg J1939 message
  * @retval J1939 status
  */
static J1939_Status_t J1939_ReceiveSplit(J1939_t Handle, J1939_Message_t Msg){
  #if J1939_ENABLE_TRANSPORT_PROTOCOL
  if (J1939_ProtocolReceiveManager(Handle->Protocol, Msg) == J1939_OK)
    return J1939_OK;
  #endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */
  return Handle->DecodePayload(Msg);
}

/**
  * @brief  Return J1939 handle register
  * @param  void
  * @retval Handle register
  */
static J1939_Register_t Register(void){
  static J1939_Queue_t Register = NULL;
  if (Register == NULL)
    Register = J1939_QueueCreate("register", J1939_SIZE_CAN_PORT, NULL, NULL);
  return &Register;
}

/**
  * @brief  Put a J1939 handle into register
  * @param  Handle J1939 handle
  * @retval J1939 status
  */
static J1939_Status_t J1939_Enregister(J1939_t Handle){
  if (Handle == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    return J1939_ERROR;
  }
  return J1939_Enqueue(*Register(), Handle);
}

/**
  * @brief  Remove a J1939 handle from register
  * @param  Handle J1939 handle
  * @retval J1939 status
  */
static J1939_Status_t J1939_Deregister(J1939_t Handle){
  if (Handle == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    return J1939_ERROR;
  }
  for (uint32_t i = J1939_QueueCount(*Register()); i >= 1; i--){
    J1939_t Node = J1939_QueueAmong(*Register(), i);
    if (Node == NULL){
      J1939_LOG_ERROR("[Handle]A null pointer appears");
      return J1939_ERROR;
    }
    else if (Node == Handle){
      J1939_Dequeue(*Register(), i);
      return J1939_OK;
    }
  }

  J1939_LOG_ERROR("[Handle]'%s' has not been registered", Handle->Name);
  return J1939_ERROR;
}

/**
  * @brief  Create a J1939 handle
  * @param  Name HAL CAN handle's name
  * @param  SelfAddress self address, base on J1939 protocol
  * @param  QueueSize Maximum size of transmit queue
  * @retval J1939 handle
  */
J1939_t J1939_HandleCreate(char *Name, uint8_t SelfAddress, uint32_t QueueSize){
  if (Name == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    return NULL;
  }

  J1939_t Handle = (J1939_t)J1939_malloc(sizeof(struct J1939));
  if (Handle == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    return NULL;
  }

  Handle->Name = Name;
  Handle->SelfAddress = SelfAddress;

  #ifndef J1939_REGISTER
  #undef J1939_REGISTER
  #endif /* J1939_REGISTER */

  #define J1939_REGISTER(Key)\
  do{\
    if (strcmp(Name, #Key) == 0){\
      Handle->Port = &Key;\
      J1939_EXTERN_FUNCTION(Key, SoftwareFilter);\
      J1939_EXTERN_FUNCTION(Key, DecodePayload);\
      J1939_EXTERN_FUNCTION(Key, SendingCallBack);\
      J1939_EXTERN_FUNCTION(Key, MissingCallBack);\
      J1939_EXTERN_FUNCTION(Key, ReadingCallBack);\
      J1939_EXTERN_FUNCTION(Key, TimeoutCallBack);\
    }\
  }while(0)
  #include "j1939_register.inc"
  #undef J1939_REGISTER

  if (Handle->Port == NULL){
    J1939_LOG_ERROR("[Handle]'%s' does not exist, create failed", Name);
    J1939_free(Handle);
    Handle = NULL;
    return NULL;
  }

  if (J1939_PortInit(Handle->Port) != J1939_OK){
    J1939_LOG_ERROR("[Handle]'%s' init failed", Name);
    J1939_free(Handle);
    Handle = NULL;
    return NULL;
  }

  Handle->TxFIFO = J1939_QueueCreate(Name, QueueSize, J1939_MessageCopy, J1939_MessageDelete);
  if (Handle->TxFIFO == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    J1939_PortDeInit(Handle->Port);
    J1939_free(Handle);
    Handle = NULL;
    return NULL;
  }

  #if J1939_ENABLE_TRANSPORT_PROTOCOL
  Handle->Protocol = J1939_ProtocolCreate();
  if (Handle->Protocol == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    J1939_PortDeInit(Handle->Port);
    J1939_QueueDelete(&Handle->TxFIFO);
    J1939_free(Handle);
    Handle = NULL;
    return NULL;
  }
  #endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */

  J1939_Enregister(Handle);

  J1939_LOG_INFO("[Handle]'%s' has been created", Name);
  return Handle;
}

/**
  * @brief  Delete the J1939 handle
  * @param  Handle J1939 handle printer
  * @retval J1939 status
  */
J1939_Status_t J1939_HandleDelete(J1939_t *Handle){
  if (Handle == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    return J1939_ERROR;
  }
  else if (*Handle == NULL){
    return J1939_OK;
  }
  else{
    J1939_LOG_INFO("[Handle]'%s' has been deleted", (*Handle)->Name);
    #if J1939_ENABLE_TRANSPORT_PROTOCOL
    J1939_ProtocolDelete(&(*Handle)->Protocol);
    #endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */
    J1939_QueueDelete(&(*Handle)->TxFIFO);
    J1939_PortDeInit((*Handle)->Port);
    J1939_Deregister(*Handle);
    if (J1939_QueueCount(*Register()) == 0){
      J1939_QueueDelete(Register());
    }
    J1939_free(*Handle);
    *Handle = NULL;
    return J1939_OK;
  }

}

/**
  * @brief  J1939 task handler, put it in a timer with a period of 5 milliseconds
  * @param  void
  * @retval J1939 status(reserved)
  */
J1939_Status_t J1939_TaskHandler(void){
  for (uint32_t i = J1939_QueueCount(*Register()); i >= 1; i--){
    J1939_t Handle = J1939_QueueAmong(*Register(), i);
    #if J1939_ENABLE_TRANSPORT_PROTOCOL
    J1939_Message_t Msg = NULL;
    switch (J1939_ProtocolTaskHandler(Handle->Protocol, &Msg)){
      case J1939_TRANSMIT:
        J1939_Enqueue(Handle->TxFIFO, Msg);
        break;
      case J1939_RECEIVED:
        Handle->DecodePayload(Msg);
        break;
      case J1939_TIMEOUT:
        Handle->TimeoutCallBack(Msg);
        break;
      default:
        break;
    }
    J1939_MessageDelete(&Msg);
    #endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */
    J1939_Transmit(Handle);
    J1939_Receive(Handle);
  }
  return J1939_OK;
}

/**
  * @brief  Send a J1939 message from a J1939 handle
  * @param  Handle J1939 handle
  * @param  Msg J1939 message
  * @retval J1939 status
  */
J1939_Status_t J1939_SendMessage(J1939_t Handle, J1939_Message_t Msg){
  if (Handle == NULL || Msg == NULL){
    J1939_LOG_ERROR("[Handle]A null pointer appears");
    return J1939_ERROR;
  }
  /* Check source address */
  if (Msg->PDU.SourceAddress != Handle->SelfAddress)
    Msg->PDU.SourceAddress = Handle->SelfAddress;

  return J1939_TransmitSplit(Handle, Msg);
}
