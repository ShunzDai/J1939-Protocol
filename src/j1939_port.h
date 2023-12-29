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
#ifndef J1939_PORT_H
#define J1939_PORT_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "j1939_types.h"

j1939_status_t j1939_port_transmit(j1939_port_t self, const j1939_static_message_t *msg, uint32_t timeout_ms);
j1939_status_t j1939_port_receive(j1939_port_t self, j1939_static_message_t *msg, uint32_t timeout_ms);
uint32_t j1939_port_get_tick(void);
void j1939_port_delay(uint32_t time_ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_PORT_H */