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
#include "j1939_port.h"
#include "src/common/J1939_queue.h"
#include "src/message/j1939_message.h"
#include "src/port/j1939_memory.h"

/**
  * @brief  CAN port initialization
  * @param  Port CAN port printer
  * @retval J1939 status
  */
J1939_Status_t J1939_PortInit(J1939_Port_t *Port){
  #if __J1939_Port(SUSPEND)
  return J1939_OK;
  #elif __J1939_Port(VIRTUAL)
  *Port = J1939_VirtualNodeCreate();
  return J1939_OK;
  #elif __J1939_Port(STM32)
  CAN_FilterTypeDef Filter = {0};
  Filter.FilterActivation = CAN_FILTER_ENABLE;
  Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  Filter.FilterBank = 0;
  /* Config filter mode(CAN_FILTERMODE_IDMASK or CAN_FILTERMODE_IDLIST) */
  Filter.FilterMode = CAN_FILTERMODE_IDMASK;
  /* Config filter scale */
  Filter.FilterScale = CAN_FILTERSCALE_32BIT;
  Filter.FilterIdHigh = 0x0000;
  Filter.FilterIdLow = 0x0000;
  Filter.FilterMaskIdHigh = 0x0000;
  Filter.FilterMaskIdLow = 0x0000;
  if (HAL_CAN_ConfigFilter(Port, &Filter) == HAL_OK)
    return (J1939_Status_t)HAL_CAN_Start(Port);
  else
    return J1939_ERROR;
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */
}

/**
  * @brief  CAN port de-initialization
  * @param  Port CAN port printer
  * @retval J1939 status
  */
J1939_Status_t J1939_PortDeInit(J1939_Port_t *Port){
  #if __J1939_Port(SUSPEND)
  return J1939_OK;
  #elif __J1939_Port(VIRTUAL)
  return J1939_VirtualNodeDelete(Port);
  #elif __J1939_Port(STM32)
  return (J1939_Status_t)HAL_CAN_Stop(Port);
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */
}

/**
  * @brief  Get system tick
  * @param  void
  * @retval System tick
  */
uint64_t J1939_PortGetTick(void){
  #if __J1939_Port(SUSPEND)
  return 0;
  #elif __J1939_Port(VIRTUAL)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
  #elif __J1939_Port(STM32)
  return (uint64_t)HAL_GetTick();
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */

}

/**
  * @brief  Check transmit mailbox free level
  * @param  Port CAN port printer
  * @retval transmit mailbox free level
  */
uint32_t J1939_PortGetTxMailboxesFreeLevel(J1939_Port_t *Port){
  #if __J1939_Port(SUSPEND)
  return 1;
  #elif __J1939_Port(VIRTUAL)
  return J1939_VirtualGetTxMailboxesFreeLevel(*Port);
  #elif __J1939_Port(STM32)
  return HAL_CAN_GetTxMailboxesFreeLevel(Port);
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */

}

/**
  * @brief  Check receive mailbox fill level
  * @param  Port CAN port printer
  * @retval receive mailbox fill level
  */
uint32_t J1939_PortGetRxFifoFillLevel(J1939_Port_t *Port){
  #if __J1939_Port(SUSPEND)
  return 0;
  #elif __J1939_Port(VIRTUAL)
  return J1939_VirtualGetRxFifoFillLevel(*Port);
  #elif __J1939_Port(STM32)
  return HAL_CAN_GetRxFifoFillLevel(Port, CAN_RX_FIFO0);
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */

}

/**
  * @brief  Add message into transmit mailbox
  * @param  Port CAN port printer
  * @param  Msg J1939 message
  * @retval J1939 status
  */
J1939_Status_t J1939_PortAddTxMessage(J1939_Port_t *Port, J1939_Message_t Msg){
  #if __J1939_Port(SUSPEND)
  return J1939_OK;
  #elif __J1939_Port(VIRTUAL)
  return J1939_VirtualAddTxMessage(*Port, Msg);
  #elif __J1939_Port(STM32)
  uint32_t Mailbox = 0;
  CAN_TxHeaderTypeDef TxHeader;
  TxHeader.IDE = CAN_ID_EXT;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxHeader.DLC = Msg->Length;
  TxHeader.ExtId = Msg->ID;
  if (HAL_CAN_AddTxMessage(Port, &TxHeader, Msg->Payload, &Mailbox) == HAL_OK)
    return J1939_OK;
  return J1939_ERROR;
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */
}

/**
  * @brief  Get message from receive mailbox
  * @param  Port CAN port printer
  * @param  MsgPtr Message buffer printer
  * @retval J1939 status
  */
J1939_Status_t J1939_PortGetRxMessage(J1939_Port_t *Port, J1939_Message_t *MsgPtr){
  #if __J1939_Port(SUSPEND)
  return J1939_OK;
  #elif __J1939_Port(VIRTUAL)
  return J1939_VirtualGetRxMessage(*Port, MsgPtr);
  #elif __J1939_Port(STM32)
  CAN_RxHeaderTypeDef RxHeader;
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);
  if (HAL_CAN_GetRxMessage(Port, CAN_FILTER_FIFO0, &RxHeader, (*MsgPtr)->Payload) == HAL_OK){
    (*MsgPtr)->ID = RxHeader.ExtId;
    (*MsgPtr)->Length = RxHeader.DLC;
    return J1939_OK;
  }
  J1939_MessageDelete(MsgPtr);
  return J1939_ERROR;
  #else /* __J1939_Port() */
  #error "Incompatible port"
  #endif /* __J1939_Port() */
}
