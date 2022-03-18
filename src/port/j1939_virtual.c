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
#include "j1939_virtual.h"
#include "j1939_config.h"
#include "src/memory/j1939_memory.h"

/* Virtual bus struct */
typedef J1939_Queue_t J1939_VirtualBus_t;

/* Virtual node struct */
struct J1939_VirtualNode{
  J1939_Node_t Next;
  J1939_Queue_t RxFIFO;
};

#define J1939_PORT_VIRTUAL_CREATE
#include "j1939_port.inc"

#if __J1939_Port(VIRTUAL)

static J1939_VirtualBus_t VirtualBus = NULL;

/**
  * @brief  Set a virtual node online
  * @param  Virtual Virtual node
  * @retval J1939 status
  */
static J1939_Status_t J1939_Online(J1939_VirtualNode_t Virtual){
  if (Virtual == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    return J1939_ERROR;
  }
  else if (VirtualBus == NULL)
    VirtualBus = J1939_QueueCreate();

  return J1939_Enqueue(VirtualBus, -1, (J1939_Node_t)Virtual);
}

/**
  * @brief  Set a virtual node offline
  * @param  Virtual Virtual node
  * @retval J1939 status
  */
static J1939_Status_t J1939_Offine(J1939_VirtualNode_t Virtual){
  uint32_t Count = J1939_QueueCount(VirtualBus);
  J1939_VirtualNode_t *Node = (J1939_VirtualNode_t *)J1939_QueuePointer(VirtualBus, 0);
  for (uint32_t i = 0; i < Count; i++){
    if (Node == NULL)
      return J1939_ERROR;
    else if (*Node == Virtual)
      J1939_Dequeue(VirtualBus, i);
    else
      Node = (J1939_VirtualNode_t *)J1939_NodeGetNext(*Node);
  }
  return (J1939_QueueCount(VirtualBus)) ? J1939_OK : J1939_QueueDelete(&VirtualBus);
}

/**
  * @brief  Create a virtual node
  * @param  void
  * @retval Virtual node
  */
J1939_VirtualNode_t J1939_VirtualNodeCreate(void){
  J1939_VirtualNode_t Virtual = (J1939_VirtualNode_t)J1939_malloc(sizeof(struct J1939_VirtualNode));
  if (Virtual == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    return NULL;
  }

  Virtual->RxFIFO = J1939_QueueCreate();
  if (Virtual->RxFIFO == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    J1939_free(Virtual);
    return NULL;
  }

  J1939_Online(Virtual);

  J1939_LOG_INFO("[Virtual]vnode has been created");
  return Virtual;
}

/**
  * @brief  Delete a virtual node
  * @param  Virtual Virtual node printer
  * @retval J1939 status
  */
J1939_Status_t J1939_VirtualNodeDelete(J1939_VirtualNode_t *Virtual){
  if (Virtual == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    return J1939_ERROR;
  }
  else if (*Virtual == NULL){
    return J1939_OK;
  }
  else{
    J1939_QueueDelete(&(*Virtual)->RxFIFO);
    J1939_Offine(*Virtual);
    *Virtual = NULL;
    return J1939_OK;
  }
}

/**
  * @brief  Check virtual transmit mailbox free level
  * @param  Virtual Virtual Node
  * @retval Transmit mailbox free level
  */
uint32_t J1939_VirtualGetTxMailboxesFreeLevel(J1939_VirtualNode_t Virtual){
  if (Virtual == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    return 0;
  }

  return J1939_SIZE_VIRTUAL_FIFO - J1939_QueueCount(Virtual->RxFIFO);
}

/**
  * @brief  Check virtual receive mailbox fill level
  * @param  Virtual Virtual Node
  * @retval Receive mailbox fill level
  */
uint32_t J1939_VirtualGetRxFifoFillLevel(J1939_VirtualNode_t Virtual){
  if (Virtual == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    return 0;
  }
  return J1939_QueueCount(Virtual->RxFIFO);
}

/**
  * @brief  Add message into other virtual node's receive mailbox
  * @param  Virtual Virtual Node
  * @param  Msg J1939 message
  * @retval J1939 status
  */
J1939_Status_t J1939_VirtualAddTxMessage(J1939_VirtualNode_t Virtual, J1939_Message_t Msg){
  uint32_t Count = J1939_QueueCount(VirtualBus);
  J1939_VirtualNode_t *Node = (J1939_VirtualNode_t *)J1939_QueuePointer(VirtualBus, 0);
  for (uint32_t i = 0; i < Count; i++){
    if (Node == NULL)
      return J1939_ERROR;
    else if (*Node != Virtual)
      J1939_Enqueue((*Node)->RxFIFO, -1, (J1939_Node_t)J1939_MessageCopy(Msg));
    Node = (J1939_VirtualNode_t *)J1939_NodeGetNext(*Node);
  }
  return J1939_OK;
}

/**
  * @brief  Get message from virtual receive mailbox
  * @param  Virtual Virtual Node
  * @param  MsgPtr Message buffer printer
  * @retval J1939 status
  */
J1939_Status_t J1939_VirtualGetRxMessage(J1939_VirtualNode_t Virtual, J1939_Message_t *MsgPtr){
  if (Virtual == NULL || MsgPtr == NULL){
    J1939_LOG_ERROR("[Virtual]A null pointer appears");
    return J1939_ERROR;
  }

  if (J1939_QueueCount(Virtual->RxFIFO)){
    *MsgPtr = *(J1939_Message_t *)J1939_QueuePointer(Virtual->RxFIFO, 0);
    ((J1939_Node_t)Virtual->RxFIFO)->Next = ((J1939_Node_t)Virtual->RxFIFO)->Next->Next;
  }

  return J1939_OK;
}

#endif /* __J1939_Port() */
