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
#include "gtest/gtest.h"

static auto recv_cb = +[](j1939_port_t port, const j1939_message_t *msg, void *arg) {
  printf("port [%02X] recv id [%08X] size [%d] data [", port, msg->id, msg->size);
  for (uint16_t idx = 0; idx < msg->size; ++idx) {
    printf("%02X%s", msg->data[idx], idx == msg->size - 1 ? "]\n" : " ");
  }
};

static auto timeout_cb = +[](j1939_port_t port, const j1939_message_t *msg, void *arg) {
  printf("port [%02X] timeout id [%08X] size [%d] data [", port, msg->id, msg->size);
  for (uint16_t idx = 0; idx < msg->size; ++idx) {
    printf("%02X%s", msg->data[idx], idx == msg->size - 1 ? "]\n" : " ");
  }
};

TEST(j1939, protocol) {
  j1939_config_t config[] = {
    {
      .self_address = 0x00,
      .recv_cb = recv_cb,
      .timeout_cb = timeout_cb,
      .port = 0,
      .arg = nullptr,
    },
    {
      .self_address = 0x01,
      .recv_cb = recv_cb,
      .timeout_cb = timeout_cb,
      .port = 1,
      .arg = nullptr,
    },
  };
  j1939_t *bus[] = {j1939_create(&config[0]), j1939_create(&config[1])};

  for (size_t idx = 0; idx < sizeof(bus) / sizeof(j1939_t *); ++idx)
    j1939_virtual_add_node();

  j1939_transmit(bus[0], j1939_message_create(0x18E00100U, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890", 62), -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_receive(bus[0], -1);
  j1939_tp_cm_transmit_manager(bus[0], -1);
  j1939_receive(bus[1], -1);
  j1939_tp_cm_transmit_manager(bus[1], -1);

  j1939_delete(bus[0]);
  j1939_delete(bus[1]);
}

/* valgrind --tool=memcheck --leak-check=full ./test/test */
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
