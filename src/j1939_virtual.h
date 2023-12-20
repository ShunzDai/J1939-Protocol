#pragma once
#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "j1939_config.h"

typedef uint8_t j1939_port_t;

j1939_status_t j1939_virtual_transmit(j1939_port_t self, const j1939_static_message_t *msg, uint32_t timeout_ms);
j1939_status_t j1939_virtual_receive(j1939_port_t self, j1939_static_message_t *msg, uint32_t timeout_ms);
uint32_t j1939_virtual_get_tick(void);
void j1939_virtual_add_node(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
