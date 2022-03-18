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
#include "src/common/j1939_queue.h"

TEST(Queue, Test01){
  J1939_Queue_t Queue = J1939_QueueCreate();
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  J1939_Enqueue(Queue, -1, (J1939_Node_t)malloc(0x100));
  EXPECT_EQ(J1939_QueueCount(Queue), 7);
  J1939_Dequeue(Queue, -1);
  EXPECT_EQ(J1939_QueueCount(Queue), 6);
  J1939_Dequeue(Queue, 0);
  EXPECT_EQ(J1939_QueueCount(Queue), 5);
  J1939_Dequeue(Queue, 2);
  EXPECT_EQ(J1939_QueueCount(Queue), 4);
  J1939_QueueDelete(&Queue);
  EXPECT_EQ((uint64_t)Queue, (uint64_t)NULL);
}
