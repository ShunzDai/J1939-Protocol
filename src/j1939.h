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
#ifndef J1939_SRC_H
#define J1939_SRC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "j1939_port.h"
#include "j1939_config.h"
#include <stdint.h>
#include <stdbool.h>

typedef void (*j1939_cb_t)(j1939_port_t port, const j1939_message_t *msg, void *arg);

typedef struct j1939_config {
  uint8_t self_address;
  j1939_cb_t recv_cb;
  j1939_cb_t timeout_cb;
  j1939_port_t port;
  void *arg;
} j1939_config_t;

typedef struct j1939 j1939_t;

j1939_message_t *j1939_message_create(uint32_t id, const void *data, uint16_t size);
void j1939_message_delete(j1939_message_t *msg);

j1939_t *j1939_create(j1939_config_t *config);
j1939_status_t j1939_delete(j1939_t *self);

j1939_status_t j1939_status(j1939_t *self);

j1939_status_t j1939_transmit(j1939_t *self, const j1939_message_t *msg, uint32_t timeout_ms);

j1939_status_t j1939_receive(j1939_t *self, uint32_t timeout_ms);

j1939_status_t j1939_tp_cm_transmit_manager(j1939_t *self, uint32_t timeout_ms);

static inline j1939_status_t j1939_transmit_static(j1939_t *self, const j1939_static_message_t *msg, uint32_t timeout_ms) {
  return j1939_transmit(self, (const j1939_message_t *)msg, timeout_ms);
}

// static inline j1939_status_t j1939_receive_static(j1939_t *self, j1939_static_message_t *msg, uint32_t timeout_ms) {
//   return j1939_receive(self, (j1939_message_t **)&msg, timeout_ms);
// }

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_SRC_H */
