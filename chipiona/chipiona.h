/*
    chipiona.h

    Chipiona Cipher

    (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Designed and written by Guillermo Amodeo Ojeda.

        - This is a very simple stream cipher where the actual data to encode is 
          used to generate the key to encode it. 
          
        - The cipher is not modelled after a mathematical function like Feistel 
          or Galois, instead uses the SHAX principle: Substitute, Hash And Xor              
            
        - Chipiona is small. It takes around 10 Kbytes in code, and around 30 Kbytes
          of memory. None of this numbers is big enough to discorage its use in small
          systems like routers or bridges.

        - DO NOT FORGET that you can build another SHAX cipher by using a different 
          pseudo-random function and hash algorithm e.g, Mersene-Twister and SHA256.          

                                         --oO0Oo--
*/

#ifndef CHIPIONA_CIPHER
#define CHIPIONA_CIPHER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHIPIONA_KEY_SIZE  64

typedef struct
{    
    unsigned int      *sboxes[16];    /* S-Boxes for the cipher */
    unsigned int       snum;          /* Number of s-boxes */
    unsigned long      cbits;         /* Counter of processed bits (MD5) */
    unsigned int       state[4];      /* MD5 state */
	  unsigned char      data[16];      /* MD5 buffer */
    unsigned int       pos;           /* Position in the buffer */ 
    unsigned char      curkey[16];    /* Current Key (MD5 hash) */
} chipiona_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------ *
  Initialize a chipiona context, allocating 
  the memory and creating the s-boxes using 
  the key material, which has to be at least
  64 bytes long.

  it  returns 0 if correct, -1 if a parameter 
  is wrong (eg len is less than 64 or key is
  NULL) or -2 if it fals to allocate  memory 
  for the s-boxes
 * ------------------------------------------ */

int  chipiona_init(chipiona_t *ctx,const void *key,size_t len);

/* ------------------------------------------ *
   This will clear the context and free the
   memory used by it.
 * ------------------------------------------ */

 void chipiona_end(chipiona_t *ctx);

/* ------------------------------------------ *
   This will encode 'len' bytes fron 'src' 
   and put them in 'dst'. Both can point to
   the same buffer. 
 * ------------------------------------------ */

void chipiona_encode(chipiona_t *ctx,void *dst,const void *src, size_t len);

/* ------------------------------------------ *
   This will decode 'len' bytes fron 'src' 
   and put them in 'dst'. Both can point to
   the same buffer.
 * ------------------------------------------ */

void chipiona_decode(chipiona_t *ctx,void *dst,const void *src, size_t len);

/* ------------------------------------------ *
   This will dump the current state of the
   context 'ctx' through the stream 'fp'.
   'show_sboxes' controls if the s-boxes 
   should be dumped as well.
 * ------------------------------------------ */

void chipiona_dump_data(chipiona_t *ctx,FILE *fp,int show_sboxes);

#ifdef __cplusplus
}
#endif

#endif
