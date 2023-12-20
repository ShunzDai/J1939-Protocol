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
#ifndef J1939_CONFIG_H
#define J1939_CONFIG_H
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include <stdint.h>

#if defined J1939_MOCK
#define J1939_PORT_VIRTUAL
#elif defined ESP_PLATFORM
#define J1939_PORT_ESP32
#endif

#define J1939_SIZE_DATAFIELD            8

typedef enum j1939_status {
  J1939_ERROR = -10,
  J1939_TIMEOUT,
  J1939_OK = 0,
  J1939_BUSY,
  J1939_BLOCKED,
} j1939_status_t;

/* j1939 protocol data unit struct */
typedef struct j1939_pdu {
  /* reference SAE J1939-21 5.2 */
  uint32_t source_address : 8;
  uint32_t pdu_specific   : 8;
  uint32_t pdu_format     : 8;
  uint32_t data_page      : 1;
  uint32_t reserved       : 1;
  uint32_t priority       : 3;
  uint32_t err            : 1; /* error message frame */
  uint32_t rtr            : 1; /* remote transmission request flag */
  uint32_t eff            : 1; /* frame format flag */
} j1939_pdu_t;

/* j1939 message struct */
typedef struct j1939_message {
  union {
    j1939_pdu_t pdu;
    uint32_t id;
  };
  uint16_t size;
  uint8_t data[];
} j1939_message_t;

typedef struct j1939_static_message {
  union {
    j1939_pdu_t pdu;
    uint32_t id;
  };
  uint16_t size;
  uint8_t data[J1939_SIZE_DATAFIELD];
} j1939_static_message_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_CONFIG_H */
