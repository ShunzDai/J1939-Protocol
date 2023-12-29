#include "j1939_virtual.h"
#include <queue>
#include <unordered_map>
#include <stdio.h>

struct Hash {
    size_t operator()(const j1939_port_t& self) const {
        return self;
    }
};

struct Pred {
    bool operator()(const j1939_port_t& l, const j1939_port_t& r) const {
        return l == r;
    }
};

using node_t = std::queue<j1939_static_message_t>;
using bus_t = std::unordered_map<j1939_port_t, node_t, Hash, Pred>;

static bus_t _bus{};

extern "C" j1939_status_t j1939_virtual_transmit(j1939_port_t self, const j1939_static_message_t *msg, uint32_t timeout_ms) {
  for (auto &[port, node] : _bus) {
    if (port == self)
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
  for (auto &[port, node] : _bus) {
    if (port != self)
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

void j1939_virtual_add_node(j1939_port_t self) {
  _bus[self] = {};
}
