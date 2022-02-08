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
#ifndef J1939_SRC_QUEUE_H
#define J1939_SRC_QUEUE_H
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "src/common/j1939_typedef.h"

struct J1939_Queue;

typedef struct J1939_Queue * J1939_Queue_t;

J1939_Queue_t J1939_QueueCreate(char *Name, uint32_t QueueSize, void *CreateMethod, void *DeleteMethod);
J1939_Status_t J1939_QueueDelete(J1939_Queue_t *Queue);
void *J1939_QueueHead(J1939_Queue_t Queue);
void *J1939_QueueTail(J1939_Queue_t Queue);
void *J1939_QueueAmong(J1939_Queue_t Queue, const uint32_t Serial);
J1939_Status_t J1939_Enqueue(J1939_Queue_t Queue, void *Obj);
J1939_Status_t J1939_Dequeue(J1939_Queue_t Queue, const uint32_t Serial);
J1939_Status_t J1939_QueueClear(J1939_Queue_t Queue);
uint32_t J1939_QueueCount(J1939_Queue_t Queue);
uint32_t J1939_QueueGetSize(J1939_Queue_t Queue);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_QUEUE_H */
