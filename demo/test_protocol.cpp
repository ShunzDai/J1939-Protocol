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
#include "gtest/gtest.h"
#include "src/j1939.h"

/* 测试J1939控制句柄创建/释放功能 */
TEST(Handle, Test01){
  J1939_t Handle = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_HandleDelete(&Handle);
  EXPECT_EQ((uint64_t)Handle, (uint64_t)NULL);
}

/* 测试短报文发送/接收功能 */
TEST(Protocol, Test02){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_SendMessage(Handle1, 0x18F00400U, 8, "\x01\x02\x03\x04\x05\x06\x07\x08");
  J1939_SendMessage(Handle2, 0x18F00401U, 8, "\x01\x02\x03\x04\x05\x06\x07\x08");
  J1939_TaskHandler();
  J1939_TaskHandler();
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}

/* 测试TP BAM发送/接收功能 */
TEST(Protocol, TP_BAM01){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_SendMessage(Handle1, 0x18F00400U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_SendMessage(Handle2, 0x18F00400U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}

/* 测试TP BAM发送/接收1785字节用时 */
TEST(Protocol, TP_BAM02){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_SendMessage(Handle1, 0x18F00400U, 1785, NULL);
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_SendMessage(Handle2, 0x18F00401U, 1785, NULL);
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}

/* 测试TP CMDT发送/接收功能 */
TEST(Protocol, TP_CMDT01){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_SendMessage(Handle1, 0x18E00100U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_SendMessage(Handle2, 0x18E00001U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}

/* 测试TP CMDT发送/接收1785字节用时 */
TEST(Protocol, TP_CMDT02){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_SendMessage(Handle1, 0x18E00100U, 1785, NULL);
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_SendMessage(Handle2, 0x18E00001U, 1785, NULL);
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}