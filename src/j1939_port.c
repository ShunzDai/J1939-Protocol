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
#include <stdio.h>

#if defined J1939_PORT_VIRTUAL

j1939_status_t j1939_port_transmit(j1939_port_t self, const j1939_static_message_t *msg, uint32_t timeout_ms) {
  return j1939_virtual_transmit(self, msg, timeout_ms);
}

j1939_status_t j1939_port_receive(j1939_port_t self, j1939_static_message_t *msg, uint32_t timeout_ms) {
  return j1939_virtual_receive(self, msg, timeout_ms);
}

uint32_t j1939_port_get_tick() {
  return j1939_virtual_get_tick();
}

void j1939_port_delay(uint32_t time_ms) {

}

#elif defined J1939_PORT_ESP32
#include "driver/twai.h"
#include <string.h>

j1939_status_t j1939_port_transmit(j1939_port_t *self, const j1939_static_message_t *msg, uint32_t timeout_ms) {
  twai_message_t buff = { { { .extd = 1, }, }, .identifier = msg->id, .data_length_code = msg->size, };
  memcpy(buff.data, msg->data, msg->size);
  return twai_transmit(&buff, pdMS_TO_TICKS(timeout_ms)) == ESP_OK ? J1939_OK : J1939_ERROR;
}

j1939_status_t j1939_port_receive(j1939_port_t *self, j1939_static_message_t *msg, uint32_t timeout_ms) {
  twai_message_t buff = {0};
  return twai_receive(&buff, pdMS_TO_TICKS(timeout_ms)) == ESP_OK ? msg->id = buff.identifier, msg->size = buff.data_length_code, memcpy(msg->data, buff.data, buff.data_length_code), J1939_OK : J1939_TIMEOUT;
}

uint32_t j1939_port_get_tick() {
  return 0;
}

#endif /* J1939_PORT */
