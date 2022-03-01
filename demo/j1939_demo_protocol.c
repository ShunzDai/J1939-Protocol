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

void thread_1000ms(void){
  static uint64_t tick;
  static uint8_t type;
  if (J1939_PortGetTick() - tick >= 100){
    if (J1939_GetProtocolStatus(Handle1) == J1939_OK){
      type = !type;
      J1939_Send(Handle1, type ? 0x18E00201 : 0x18F00201, 32,
        "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"
        "\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20"
      );
    }
    tick = J1939_PortGetTick();
  }
}

int main(void){
  setenv("MALLOC_TRACE", "output", 1);
  mtrace();

  Handle1 = J1939_HandleCreate((char *)"hcan1", 0x01, 32);
  Handle2 = J1939_HandleCreate((char *)"hcan2", 0x02, 32);

  uint64_t tick = J1939_PortGetTick();
  while(J1939_PortGetTick() - tick <= 5000){
    thread_5ms();
    thread_1000ms();
  }

  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);

  return 0;
}
