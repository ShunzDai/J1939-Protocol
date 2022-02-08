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
#include <unistd.h>
#include <mcheck.h>
#include <stdlib.h>

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else /* __cplusplus */
#define EXTERN_C
#endif /* __cplusplus */

EXTERN_C uint64_t J1939_PortGetTick(void);

J1939_t Handle1 = NULL;
J1939_t Handle2 = NULL;

void thread_5ms(void){
  static uint64_t tick;
  if (J1939_PortGetTick() - tick >= 5){
    J1939_TaskHandler();
    tick = J1939_PortGetTick();
  }
}

void thread_100ms(void){
  static uint64_t tick;
  if (J1939_PortGetTick() - tick >= 1000){
    J1939_Message_t Msg = J1939_MessageCreate(0x18F00400, 8, "12345678");
    J1939_SendMessage(Handle1, Msg);
    J1939_SendMessage(Handle2, Msg);
    J1939_MessageDelete(&Msg);
    tick = J1939_PortGetTick();
  }
}

int main(void){
  setenv("MALLOC_TRACE", "output", 1);
  mtrace();

  for (uint32_t i = 0; i <= 3; i++){
    Handle1 = J1939_HandleCreate((char *)"hcan1", 0x01, 32);
    Handle2 = J1939_HandleCreate((char *)"hcan2", 0x02, 32);

    uint64_t tick = J1939_PortGetTick();
    while(J1939_PortGetTick() - tick <= 3000){
      thread_5ms();
      thread_100ms();
    }

    J1939_HandleDelete(&Handle1);
    J1939_HandleDelete(&Handle2);

    sleep(1);
  }

  return 0;
}
