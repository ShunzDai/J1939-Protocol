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

#ifdef J1939_ARCH_64
    #define MEM_UNIT         uint64_t
    #define ALIGN_MASK       0x7
#else /* J1939_ARCH_64 */
    #define MEM_UNIT         uint32_t
    #define ALIGN_MASK       0x3
#endif /* J1939_ARCH_64 */

#define COPY32 *d32 = *s32; d32++; s32++;
#define COPY8 *d8 = *s8; d8++; s8++;
#define SET32(x) *d32 = x; d32++;
#define SET8(x) *d8 = x; d8++;
#define REPEAT8(expr) expr expr expr expr expr expr expr expr

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
void *J1939_memset(void *_Dst, uint8_t _Val, size_t _Size){
  uint8_t *d8 = (uint8_t *)_Dst;

  uintptr_t d_align = (uintptr_t) d8 & ALIGN_MASK;

  /*Make the address aligned*/
  if(d_align) {
    d_align = ALIGN_MASK + 1 - d_align;
    while(d_align && _Size){
      SET8(_Val);
      _Size--;
      d_align--;
    }
  }

  uint32_t v32 = (uint32_t)_Val + ((uint32_t)_Val << 8) + ((uint32_t)_Val << 16) + ((uint32_t)_Val << 24);

  uint32_t *d32 = (uint32_t *)d8;

  while(_Size > 32){
    REPEAT8(SET32(v32));
    _Size -= 32;
  }

  while(_Size > 4){
    SET32(v32);
    _Size -= 4;
  }

  d8 = (uint8_t *)d32;
  while(_Size) {
    SET8(_Val);
    _Size--;
  }

  return _Dst;
}

/**
  * @brief  memcpy
  * @param  _Dst Destination
  * @param  _Src Source
  * @param  _Size Destination size byte(s)
  * @retval A pointer to the _Dst
  */
void *J1939_memcpy(void *_Dst, const void *_Src, size_t _Size){
  uint8_t *d8 = _Dst;
  const uint8_t *s8 = _Src;

  uintptr_t d_align = (uintptr_t)d8 & ALIGN_MASK;
  uintptr_t s_align = (uintptr_t)s8 & ALIGN_MASK;

  /* Byte copy for unaligned memories */
  if(s_align != d_align){
    while(_Size > 32){
      REPEAT8(COPY8);
      REPEAT8(COPY8);
      REPEAT8(COPY8);
      REPEAT8(COPY8);
      _Size -= 32;
    }
    while(_Size){
      COPY8
      _Size--;
    }
    return _Dst;
  }

  /* Make the memories aligned */
  if(d_align){
    d_align = ALIGN_MASK + 1 - d_align;
    while(d_align && _Size){
      COPY8;
      d_align--;
      _Size--;
    }
  }

  uint32_t *d32 = (uint32_t *)d8;
  const uint32_t *s32 = (uint32_t *)s8;
  while(_Size > 32){
    REPEAT8(COPY32)
    _Size -= 32;
  }

  while(_Size > 4){
    COPY32;
    _Size -= 4;
  }

  d8 = (uint8_t *)d32;
  s8 = (const uint8_t *)s32;
  while(_Size){
    COPY8
    _Size--;
  }

  return _Dst;
}

int J1939_strcmp(const char *Str1,const char *Str2){
  while ((*Str1) && (*Str1 == *Str2)){
    Str1++;
    Str2++;
  }
  if (*(unsigned char*)Str1 > *(unsigned char*)Str2){
    return 1;
  }
  else if (*(unsigned char*)Str1 < *(unsigned char*)Str2){
    return -1;
  }
  else{
    return 0;
  }
}
