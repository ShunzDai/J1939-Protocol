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
#include "J1939_queue.h"
#include "j1939_config.h"
#include "src/port/j1939_memory.h"

/* Object's method */
typedef void *(*J1939_Method_t)(void *Obj);

/* Node struct */
typedef struct J1939_Node{
  struct J1939_Node *Next;
  void *Obj;
} * J1939_Node_t;

/* Queue struct */
struct J1939_Queue{
  uint32_t Size;
  uint32_t Count;
  J1939_Node_t List;
  J1939_Method_t CreateMethod;
  J1939_Method_t DeleteMethod;
};

/**
  * @brief  Package objects into nodes in create method
  * @param  Obj The packaged object
  * @param  CreateMethod Create method, format must be 
  * 'object *method(object *obj)', can be a null printer
  * @retval Node package
  */
static J1939_Node_t J1939_NodeCreate(void *Obj, J1939_Method_t CreateMethod){
  J1939_Node_t Node = (J1939_Node_t)J1939_malloc(sizeof(struct J1939_Node));
  if (Node == NULL){
    J1939_LOG_ERROR("[Node]A null pointer appears");
    return Node;
  }
  Node->Next = NULL;
  Node->Obj = (Obj != NULL && CreateMethod != NULL) ? CreateMethod(Obj) : Obj;
  return Node;
}

/**
  * @brief  Delete objects from nodes in delete method
  * @param  Node A pointer to the node where the object is located
  * @param  DeleteMethod Delete method, format must be 
  * 'whatever method(object **obj)', the double printer is used to
  *  assign a null value to *obj. can be a null printer
  * @retval J1939 status
  */
static J1939_Status_t J1939_NodeDelete(J1939_Node_t *Node, J1939_Method_t DeleteMethod){
  if (Node == NULL || *Node == NULL){
    J1939_LOG_ERROR("[Node]A null pointer appears");
    return J1939_ERROR;
  }/*
  else if (Node->Next != NULL){
    J1939_LOG_WARN("[Node]Node->Next is not released");
  }*/
  else if ((*Node)->Obj != NULL && DeleteMethod != NULL){
    DeleteMethod(&(*Node)->Obj);
  }
  J1939_Node_t Temp = *Node;
  *Node = (*Node)->Next;
  J1939_free(Temp);
  Temp = NULL;
  return J1939_OK;
}

/**
  * @brief  Return a pointer to the specified node in the queue
  * @param  Queue Queue
  * @param  Serial Node position
  * @retval A pointer to the specified node
  */
static J1939_Node_t *J1939_QueuePointer(J1939_Queue_t Queue, uint32_t Serial){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return NULL;
  }
  else if (Serial > Queue->Count){
    J1939_LOG_ERROR("[Queue]Serial value(%d) exceeds count value(%d)", Serial, Queue->Count);
    return NULL;
  }
  J1939_Node_t *Temp = &Queue->List;
  while (Serial--){
    if (*Temp == NULL){
      J1939_LOG_ERROR("[Queue]Linked list was broken");
      return NULL;
    }
    Temp = &(*Temp)->Next;
  }
  return Temp;
}

/**
  * @brief  Create a queue
  * @param  Name Queue name
  * @param  QueueSize Queue maximum size
  * @param  CreateMethod Create method of objects in nodes
  * @param  DeleteMethod Delete method of objects in nodes
  * @retval Queue handle
  */
J1939_Queue_t J1939_QueueCreate(char *Name, uint32_t QueueSize, void *CreateMethod, void *DeleteMethod){
  if (CreateMethod == NULL)
    J1939_LOG_WARN("[Queue]'%s' non create method", Name);
  if (DeleteMethod == NULL)
    J1939_LOG_WARN("[Queue]'%s' non delete method", Name);

  J1939_Queue_t Queue = (J1939_Queue_t)J1939_malloc(sizeof(struct J1939_Queue));
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return NULL;
  }

  Queue->List = J1939_NodeCreate(Name, NULL);
  if (Queue->List == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    J1939_free(Queue);
    Queue = NULL;
    return NULL;
  }
  Queue->Size = QueueSize;
  Queue->Count = 0;
  Queue->CreateMethod = (J1939_Method_t)CreateMethod;
  Queue->DeleteMethod = (J1939_Method_t)DeleteMethod;

  J1939_LOG_INFO("[Queue]'%s' has been created, size %d", Name, QueueSize);
  return Queue;
}

/**
  * @brief  Delete a queue
  * @param  Queue Queue handle printer
  * @retval J1939 status
  */
J1939_Status_t J1939_QueueDelete(J1939_Queue_t *Queue){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return J1939_ERROR;
  }
  else if (*Queue == NULL){
    return J1939_OK;
  }
  char *Name = (char *)J1939_QueueAmong(*Queue, 0);
  if (J1939_QueueClear(*Queue) != J1939_OK){
    J1939_LOG_ERROR("[Queue]Clear failed");
    return J1939_ERROR;
  }
  else if (J1939_NodeDelete(&(*Queue)->List, NULL) != J1939_OK){
    J1939_LOG_ERROR("[Queue]'%s' delete failed", Name);
    return J1939_ERROR;
  }
  else{
    J1939_LOG_INFO("[Queue]'%s' has been deleted", Name);
    J1939_free(*Queue);
    *Queue = NULL;
    return J1939_OK;
  }
}

/**
  * @brief  Return the object in the first node of the queue
  * @param  Queue Queue handle
  * @retval Object
  */
void *J1939_QueueHead(J1939_Queue_t Queue){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return NULL;
  }
  J1939_Node_t *Node = J1939_QueuePointer(Queue, 1);
  return (*Node == NULL) ? NULL : (*Node)->Obj;
}

/**
  * @brief  Return the object in the last node of the queue
  * @param  Queue Queue handle
  * @retval Object
  */
void *J1939_QueueTail(J1939_Queue_t Queue){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return NULL;
  }
  J1939_Node_t *Node = J1939_QueuePointer(Queue, Queue->Count);
  return (*Node == NULL) ? NULL : (*Node)->Obj;
}

/**
  * @brief  Return the object of the specified node of the queue
  * @param  Queue Queue handle
  * @param  Serial Node position
  * @retval Object
  */
void *J1939_QueueAmong(J1939_Queue_t Queue, const uint32_t Serial){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return NULL;
  }
  else if (Serial > Queue->Count){
    J1939_LOG_ERROR("[Queue]Serial value(%d) exceeds maximum value(%d)", Serial, Queue->Size);
    return NULL;
  }
  J1939_Node_t *Node = J1939_QueuePointer(Queue, Serial);
  return (*Node == NULL) ? NULL : (*Node)->Obj;
}

/**
  * @brief  Package an object as a node and put it at the end of the queue
  * @param  Queue Queue handle
  * @param  Obj Object
  * @retval J1939 status
  */
J1939_Status_t J1939_Enqueue(J1939_Queue_t Queue, void *Obj){
  if (Queue == NULL || Obj == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return J1939_ERROR;
  }
  else if (Queue->Count >= Queue->Size){
    J1939_LOG_WARN("[Queue]Queue is full");
    return J1939_ERROR;
  }
  J1939_Node_t Temp = *J1939_QueuePointer(Queue, Queue->Count);
  J1939_Node_t New = J1939_NodeCreate(Obj, Queue->CreateMethod);
  New->Next = Temp->Next;
  Temp->Next = New;
  Queue->Count++;
  return J1939_OK;
}

/**
  * @brief  Delete the specified node from the queue
  * @param  Queue Queue handle
  * @param  Serial Node position
  * @retval J1939 status
  */
J1939_Status_t J1939_Dequeue(J1939_Queue_t Queue, uint32_t Serial){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return J1939_ERROR;
  }
  else if (Serial > Queue->Count){
    J1939_LOG_ERROR("[Queue]Serial value(%d) exceeds maximum value(%d)", Serial, Queue->Size);
    return J1939_ERROR;
  }
  else if (Serial == 0){
    J1939_LOG_ERROR("[Queue]Serial value should be non-zero");
    return J1939_ERROR;
  }
  J1939_Node_t *Del = J1939_QueuePointer(Queue, Serial);
  J1939_NodeDelete(Del, Queue->DeleteMethod);
  Queue->Count--;
  return J1939_OK;
}

/**
  * @brief  Clear the queue
  * @param  Queue Queue handle
  * @retval J1939 status
  */
J1939_Status_t J1939_QueueClear(J1939_Queue_t Queue){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return J1939_ERROR;
  }
  
  while (Queue->Count){
    if (J1939_Dequeue(Queue, 1) != J1939_OK){
      return J1939_ERROR;
    }
  }
  
  return J1939_OK;
}

/**
  * @brief  Return the number of nodes in the queue
  * @param  Queue Queue handle
  * @retval Count value
  */
uint32_t J1939_QueueCount(J1939_Queue_t Queue){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return 0;
  }
  return Queue->Count;
}

/**
  * @brief  Return the size of the queue
  * @param  Queue Queue handle
  * @retval Size of the queue
  */
uint32_t J1939_QueueGetSize(J1939_Queue_t Queue){
  if (Queue == NULL){
    J1939_LOG_ERROR("[Queue]A null pointer appears");
    return 0;
  }
  return Queue->Size;
}
