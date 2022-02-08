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
#ifndef J1939_SRC_J1939_H
#define J1939_SRC_J1939_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//#pragma anon_unions

#include "src/common/j1939_typedef.h"
#include "src/message/j1939_message.h"

#define J1939_EXTERN_FUNCTION_DEF(Key, Type) J1939_Status_t J1939_##Key##_##Type(void *argument)

struct J1939;

typedef struct J1939 * J1939_t;

J1939_t J1939_HandleCreate(char *Name, uint8_t SelfAddress, uint32_t QueueSize);
J1939_Status_t J1939_HandleDelete(J1939_t *Handle);

J1939_Status_t J1939_TaskHandler(void);
J1939_Status_t J1939_SendMessage(J1939_t Handle, J1939_Message_t Msg);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_J1939_H */
