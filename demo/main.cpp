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
#include <mcheck.h>

TEST(Message, Test01){
  uint32_t ID = 0x18F00400;
  EXPECT_EQ(((J1939_PDU_t *)&ID)->SourceAddress, 0x00U);
  EXPECT_EQ(((J1939_PDU_t *)&ID)->PDUSpecific, 0x04U);
  EXPECT_EQ(((J1939_PDU_t *)&ID)->PDUFormat, 0xF0U);
  EXPECT_EQ(((J1939_PDU_t *)&ID)->DataPage, 0U);
  EXPECT_EQ(((J1939_PDU_t *)&ID)->Reserved, 0U);
  EXPECT_EQ(((J1939_PDU_t *)&ID)->Priority, 6U);
  EXPECT_EQ(J1939_GetPGN(ID), 0xF004U);
  J1939_SetPGN(&ID, 0xE000U);
  EXPECT_EQ(ID, 0x18E00400U);
  EXPECT_EQ(J1939_GetPGN(ID), 0xE000U);
}

TEST(Message, Test02){
  J1939_Message_t Msg = J1939_MessageCreate(0x18F00400U, 8, "\x01\x02\x03\x04\x05\x06\x07\x08");
  EXPECT_EQ(Msg->ID, 0x18F00400U);
  EXPECT_EQ(Msg->Length, 8);
  EXPECT_STREQ((char *)Msg->Payload, "\x01\x02\x03\x04\x05\x06\x07\x08");
  J1939_MessageDelete(&Msg);
  EXPECT_EQ((uint64_t)Msg, (uint64_t)NULL);
}

TEST(Handle, Test01){
  J1939_t Handle = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_HandleDelete(&Handle);
  EXPECT_EQ((uint64_t)Handle, (uint64_t)NULL);
}

TEST(Handle, Test02){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_Message_t Msg = J1939_MessageCreate(0x18F00400U, 8, "\x01\x02\x03\x04\x05\x06\x07\x08");
  J1939_SendMessage(Handle1, Msg);
  //J1939_SendMessage(Handle1, Msg);
  //J1939_SendMessage(Handle1, Msg);
  J1939_TaskHandler();
  J1939_TaskHandler();
  J1939_MessageDelete(&Msg);
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Msg, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}

TEST(Handle, Test03){
  J1939_t Handle1 = J1939_HandleCreate((char *)"hcan1", 0x00, 4);
  J1939_t Handle2 = J1939_HandleCreate((char *)"hcan2", 0x01, 4);
  J1939_Send(Handle1, 0x18F00400U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_Send(Handle2, 0x18F00400U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_Send(Handle1, 0x18E00100U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_Send(Handle2, 0x18E00000U, 16, "\x01\x02\x03\x04\x05\x06\x07\x08\x01\x02\x03\x04\x05\x06\x07\x08");
  while (J1939_GetProtocolStatus(Handle1) != J1939_OK || J1939_GetProtocolStatus(Handle2) != J1939_OK)
    J1939_TaskHandler();
  J1939_HandleDelete(&Handle1);
  J1939_HandleDelete(&Handle2);
  EXPECT_EQ((uint64_t)Handle1, (uint64_t)NULL);
  EXPECT_EQ((uint64_t)Handle2, (uint64_t)NULL);
}

int main(int argc, char** argv){
  setenv("MALLOC_TRACE", "output", 1);
  mtrace();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
