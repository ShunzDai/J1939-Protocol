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
#include "src/memory/j1939_memory.h"

struct J1939_Queue{
  J1939_Node_t Next;
};

J1939_Status_t J1939_NodeDelete(J1939_Node_t *Node){
  J1939_Node_t Temp = NULL;
  if (Node != NULL && *Node != NULL){
    Temp = (*Node)->Next;
    J1939_free(*Node);
    *Node = Temp;
  }
  return J1939_OK;
}

J1939_Queue_t J1939_QueueCreate(void){
  J1939_Queue_t Queue = (J1939_Queue_t)J1939_malloc(sizeof(struct J1939_Queue));
  Queue->Next = NULL;
  return Queue;
}

J1939_Status_t J1939_QueueDelete(J1939_Queue_t *Queue){
  if (Queue != NULL && *Queue != NULL){
    J1939_QueueClear(*Queue);
    J1939_free(*Queue);
    *Queue = NULL;
  }
  return J1939_OK;
}

J1939_Node_t *J1939_QueuePointer(J1939_Queue_t Queue, int32_t Pos){
  J1939_Assert(Queue != NULL, "");
  J1939_Node_t *Temp = &Queue->Next;
  J1939_Node_t *Next = NULL;
  while (Temp != NULL && Pos--){
    if (*Temp == NULL)
      return Temp;
    Next = J1939_NodeGetNext(*Temp);
    if (*Next == NULL)
      return (Pos < 0) ? Temp : NULL;
    Temp = Next;
  }
  return Temp;
}

uint32_t J1939_QueueCount(J1939_Queue_t Queue){
  J1939_Assert(Queue != NULL, "");
  J1939_Node_t *Temp = &Queue->Next;
  uint32_t Count = 0;
  while (Temp != NULL){
    if (*Temp == NULL)
      break;
    Temp = J1939_NodeGetNext(*Temp);
    Count++;
  }
  return Count;
}

int J1939_QueueEmpty(J1939_Queue_t Queue){
  J1939_Assert(Queue != NULL, "");
  return (Queue->Next == NULL);
}

int J1939_QueueClear(J1939_Queue_t Queue){
  J1939_Assert(Queue != NULL, "");
  while (!J1939_QueueEmpty(Queue))
    J1939_Dequeue(Queue, 0);
  return 1;
}

int J1939_Enqueue(J1939_Queue_t Queue, int32_t Pos, J1939_Node_t Node){
  J1939_Assert(Queue != NULL, "");
  J1939_Assert(Node != NULL, "");
  J1939_Node_t *Temp = J1939_QueuePointer(Queue, Pos);
  if (Temp == NULL)
    return 0;
  Temp = (Pos < 0 && *Temp != NULL) ? J1939_NodeGetNext(*Temp) : Temp;
  *J1939_NodeGetNext(Node) = *Temp;
  *Temp = Node;
  return 1;
}

int J1939_Dequeue(J1939_Queue_t Queue, int32_t Pos){
  J1939_Assert(Queue != NULL, "");
  J1939_Node_t *Temp = J1939_QueuePointer(Queue, Pos);
  if (Temp == NULL)
    return 0;
  J1939_NodeDelete(Temp);
  return 1;
}
