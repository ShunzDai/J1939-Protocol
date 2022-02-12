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

/* CAN buffer size */
#define J1939_SIZE_CAN_BUFFER               8
/* CAN port size */
#define J1939_SIZE_CAN_PORT                 8

#define J1939_PORT_SUSPEND                  0
#define J1939_PORT_VIRTUAL                  1
#define J1939_PORT_STM32                    2
#define J1939_PORT_TYPE                     J1939_PORT_SUSPEND
#define __J1939_Port(val)                   (J1939_PORT_TYPE == J1939_PORT_##val)

#if __J1939_Port(VIRTUAL)
#define J1939_SIZE_VIRTUAL_NODE             J1939_SIZE_CAN_PORT
#define J1939_SIZE_VIRTUAL_FIFO             3
#endif /* __J1939_Port() */

#define J1939_LOG_ENABLE                    1
#if J1939_LOG_ENABLE
#include <stdio.h>
#define J1939_LOG(format, ...)              printf(format, ##__VA_ARGS__)
#define J1939_LOG_INFO(format, ...)         J1939_LOG("[J1939][Info]" format "\r\n", ##__VA_ARGS__)
#define J1939_LOG_WARN(format, ...)         J1939_LOG("[J1939][Warn]" "func %s, file %s, line %d: " format "\r\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#define J1939_LOG_ERROR(format, ...)        J1939_LOG("[J1939][Error]" "func %s, file %s, line %d: " format "\r\n", __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define J1939_LOG_INFO(format, ...)
#define J1939_LOG_WARN(format, ...)
#define J1939_LOG_ERROR(format, ...)
#endif

#define J1939_ADDRESS_DIVIDE                0xF0/* DO NOT MODIFIED THIS PRAMETER */
#define J1939_ADDRESS_NULL                  0xFE/* DO NOT MODIFIED THIS PRAMETER */
#define J1939_ADDRESS_GLOBAL                0xFF/* DO NOT MODIFIED THIS PRAMETER */

/* Config J1939 transport Protocol, enabled by default */
#define J1939_ENABLE_TRANSPORT_PROTOCOL     1

#if J1939_ENABLE_TRANSPORT_PROTOCOL
/* Config response packets number of TP CTS */
#define J1939_TP_CM_CTS_RESPONSE            4

/* Reference https://elearning.vector.com/mod/page/view.php?id=422 */
/* Reference SAE J1939-81 */
/* Used for identification of an ECU and for detection of address conflicts */
#define J1939_PGN_ADDR_CLAIMED              0x00EE00
/* Reference SAE J1939-21 */
/* Other PGNs can be requested using this PGN, similarly as for the CAN Remote Frame. */
/* But note: J1939 does not support Remote Frames. The Request PGN is a CAN data frame. */
#define J1939_PGN_REQUEST                   0x00EA00
/* Reference SAE J1939-21 */
/* Transmits the payload data for the transport protocols */
#define J1939_PGN_TP_DT                     0x00EB00
/* Reference SAE J1939-21 */
/* Supplies the metadata (number of bytes, packets, etc.) for transport protocols */
#define J1939_PGN_TP_CM                     0x00EC00
/* Reference SAE J1939-21 */
/* Manufacturer-specific definable specific PGN */
#define J1939_PGN_PROPRIETARY_A             0x00EF00
/* Reference SAE J1939-21 */
/* Manufacturer-specific definable additional specific PGN */
#define J1939_PGN_PROPRIETARY_A1            0x01EF00
/* Reference SAE J1939-21 */
/* Used for acknowledgement of various network services. Can be positive or negative. */
/* The acknowledgement is referenced accordingly in the application layer. */
#define J1939_PGN_ACKNOWLEDGEMENT           0x00E800
#endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* J1939_CONFIG_H */
