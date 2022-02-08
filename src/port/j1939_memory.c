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
#include "j1939_memory.h"
#include <stdlib.h>
#include <string.h>

/**
  * @brief  malloc
  * @param  _Size Malloc size
  * @retval Void printer 
  */
void *J1939_malloc(size_t _Size){
  return malloc(_Size);
}

/**
  * @brief  free
  * @param  _Block Object printer
  * @retval Void 
  */
void J1939_free(void *_Block){
  free(_Block);
}

/**
  * @brief  memset
  * @param  _Dst Destination
  * @param  _Val Value
  * @param  _Size Destination size byte(s)
  * @retval A pointer to the _Dst 
  */
void *J1939_memset(void *_Dst, int _Val, size_t _Size){
  return memset(_Dst, _Val, _Size);
}

/**
  * @brief  memcpy
  * @param  _Dst Destination
  * @param  _Src Source
  * @param  _Size Destination size byte(s)
  * @retval A pointer to the _Dst 
  */
void *J1939_memcpy(void *_Dst, const void *_Src, size_t _Size){
  return memcpy(_Dst, _Src, _Size);
}
