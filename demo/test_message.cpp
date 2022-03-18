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
#include "src/message/j1939_message.h"

/* 验证PDU结构体功能 */
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

/* 测试J1939消息创建/释放功能 */
TEST(Message, Test02){
  J1939_Message_t Msg = J1939_MessageCreate(0x18F00400U, 8, (char *)"\x01\x02\x03\x04\x05\x06\x07\x00");
  EXPECT_EQ(Msg->ID, 0x18F00400U);
  EXPECT_EQ(Msg->Length, 8);
  EXPECT_STREQ((char *)Msg->Payload, (char *)"\x01\x02\x03\x04\x05\x06\x07\x00");
  J1939_MessageDelete(&Msg);
  EXPECT_EQ((uint64_t)Msg, (uint64_t)NULL);
}