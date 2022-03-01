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
#include "src/common/j1939_queue.h"
#include "j1939_config.h"
#include <stdlib.h>

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
#define LBLOCKSIZE      (sizeof(long))
#define UNALIGNED(X)    ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

  unsigned int i;
  char *m = (char *)_Dst;
  unsigned long buffer;
  unsigned long *aligned_addr;
  unsigned int d = _Val & 0xff;  /* To avoid sign extension, copy C to an unsigned variable.  */

  if (!TOO_SMALL(_Size) && !UNALIGNED(_Dst)){
    /* If we get this far, we know that count is large and s is word-aligned. */
    aligned_addr = (unsigned long *)_Dst;

    /* Store d into each char sized location in buffer so that we can set large blocks quickly. */
    if (LBLOCKSIZE == 4){
      buffer = (d << 8) | d;
      buffer |= (buffer << 16);
    }
    else{
      buffer = 0;
      for (i = 0; i < LBLOCKSIZE; i ++)
        buffer = (buffer << 8) | d;
    }

    while (_Size >= LBLOCKSIZE * 4){
      *aligned_addr++ = buffer;
      *aligned_addr++ = buffer;
      *aligned_addr++ = buffer;
      *aligned_addr++ = buffer;
      _Size -= 4 * LBLOCKSIZE;
    }

    while (_Size >= LBLOCKSIZE){
      *aligned_addr++ = buffer;
      _Size -= LBLOCKSIZE;
    }

    /* Pick up the remainder with a bytewise loop. */
    m = (char *)aligned_addr;
  }

  while (_Size--){
    *m++ = (char)d;
  }

  return _Dst;

#undef LBLOCKSIZE
#undef UNALIGNED
#undef TOO_SMALL
}

/**
  * @brief  memcpy
  * @param  _Dst Destination
  * @param  _Src Source
  * @param  _Size Destination size byte(s)
  * @retval A pointer to the _Dst
  */
void *J1939_memcpy(void *_Dst, const void *_Src, size_t _Size){
#define UNALIGNED(X, Y) (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#define BIGBLOCKSIZE    (sizeof (long) << 2)
#define LITTLEBLOCKSIZE (sizeof (long))
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

  char *dst_ptr = (char *)_Dst;
  char *src_ptr = (char *)_Src;
  long *aligned_dst;
  long *aligned_src;
  int len = _Size;

  /* If the size is small, or either SRC or DST is unaligned, then punt into the byte copy loop.  This should be rare. */
  if (!TOO_SMALL(len) && !UNALIGNED(src_ptr, dst_ptr)){
    aligned_dst = (long *)dst_ptr;
    aligned_src = (long *)src_ptr;

    /* Copy 4X long words at a time if possible. */
    while (len >= BIGBLOCKSIZE){
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      *aligned_dst++ = *aligned_src++;
      len -= BIGBLOCKSIZE;
    }

    /* Copy one long word at a time if possible. */
    while (len >= LITTLEBLOCKSIZE){
      *aligned_dst++ = *aligned_src++;
      len -= LITTLEBLOCKSIZE;
    }

    /* Pick up any residual with a byte copier. */
    dst_ptr = (char *)aligned_dst;
    src_ptr = (char *)aligned_src;
  }

  while (len--)
    *dst_ptr++ = *src_ptr++;

  return _Dst;
#undef UNALIGNED
#undef BIGBLOCKSIZE
#undef LITTLEBLOCKSIZE
#undef TOO_SMALL
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
