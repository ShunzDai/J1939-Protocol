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
#include "j1939_config.h"
#include "gtest/gtest.h"
#include <stdlib.h>
#include <mcheck.h>

#if !__J1939_Port(VIRTUAL)
#error "Must select virtual port"
#endif /* __J1939_Port() */

int main(int argc, char** argv){
  setenv("MALLOC_TRACE", "output", 1);
  mtrace();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
