#include "j1939_virtual.h"
#include <queue>
#include <stdio.h>

using node_t = std::queue<j1939_static_message_t>;
using bus_t = std::vector<node_t>;

static bus_t _bus{};

extern "C" j1939_status_t j1939_virtual_transmit(j1939_port_t self, const j1939_static_message_t *msg, uint32_t timeout_ms) {
  for (auto &node : _bus) {
    if (&_bus[self] == &node)
      continue;
    node.push(*msg);
  }
  printf("port [%02X] tx id [%08X] size [%d] data [", self, msg->id, msg->size);
  for (uint16_t idx = 0; idx < msg->size; ++idx) {
    printf("%02X%s", msg->data[idx], idx == msg->size - 1 ? "]\n" : " ");
  }
  return J1939_OK;
}

extern "C" j1939_status_t j1939_virtual_receive(j1939_port_t self, j1939_static_message_t *msg, uint32_t timeout_ms) {
  for (auto &node : _bus) {
    if (&_bus[self] != &node)
      continue;
    if (node.size() == 0)
      return J1939_TIMEOUT;
    *msg = node.front();
    node.pop();
  }
  printf("port [%02X] rx id [%08X] size [%d] data [", self, msg->id, msg->size);
  for (uint16_t idx = 0; idx < msg->size; ++idx) {
    printf("%02X%s", msg->data[idx], idx == msg->size - 1 ? "]\n" : " ");
  }
  return J1939_OK;
}

extern "C" uint32_t j1939_virtual_get_tick(void) {
  static uint32_t count = 0;
  return count++;
}

void j1939_virtual_add_node(void) {
  _bus.push_back(node_t{});
}
