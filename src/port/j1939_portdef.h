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
#undef J1939_PORT

#if defined(J1939_PORT_HANDLE_CREATE)
#define J1939_PORT(Key) if (J1939_strcmp(Name, #Key) == 0) do {Handle->Port = &Key;} while(0);
#undef J1939_PORT_HANDLE_CREATE
#endif /* J1939_PORT_HANDLE_CREATE */

#if defined(J1939_PORT_VIRTUAL_CREATE)
#define J1939_PORT(Key) J1939_VirtualNode_t Key = NULL;
#undef J1939_PORT_VIRTUAL_CREATE
#endif /* J1939_PORT_VIRTUAL_CREATE */

#if defined(J1939_PORT_VIRTUAL_EXTERN)
#define J1939_PORT(Key) extern J1939_VirtualNode_t Key;
#undef J1939_PORT_VIRTUAL_EXTERN
#endif /* J1939_PORT_VIRTUAL_EXTERN */
