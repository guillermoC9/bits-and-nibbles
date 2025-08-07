/*
    twofish.h

    TWOFISH Encryption Routines

    (CC) Creative Commons 2018-2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Written by Guillermo Amodeo Ojeda using info and code from 
          Bruce Schneier, Niels Ferguson and Dr B R Gladman's twofish.c, 
          which was part of the original submission  to the AES contest, 
          which can be found here: 

            https://embeddedsw.net/Cipher_Reference_Home.html#TWOFISH
            https://www.schneier.com/academic/twofish/

          But the link to Neils' work is now broken, although I found a 
          copy in the Debian sources at:

            https://sources.debian.org/src/twofish/0.3-5/twofish.c/                                 

                                --oO0Oo--
*/

#ifndef _TWOFISH_CIPHER_FROM_NEILS_
#define _TWOFISH_CIPHER_FROM_NEILS_

#include "stuff.h"

#define TWOFISH_128_KEY_SIZE   16
#define TWOFISH_192_KEY_SIZE   24
#define TWOFISH_256_KEY_SIZE   32

#define TWOFISH_MAX_KEY_SIZE   32
#define TWOFISH_BLOCK_SIZE     16
#define TWOFISH_IV_SIZE        16
#define TWOFISH_CTR_IV_SIZE    12

#define TWOFISH_NUM_IB          4   /* num integers per block (128 bits) */
#define TWOFISH_MAX_SCHED      40   /* Max bytes per key schedule */

/** Context */

typedef struct _twofish_ctx_
{
    unsigned int   b_key[TWOFISH_MAX_SCHED];        /* Byte key */
    unsigned int   e_key[TWOFISH_NUM_IB];           /* Expanded key */
    unsigned int   s_key[TWOFISH_NUM_IB][256];      /* Keyed S-boxes */
   
    int     k_cycles,kbits;
} twof_t;

/** Macros */

#define twofish_bits(_1)     ((_1)->kbits)
#define twofish_end(_1)      memset(_1,0,sizeof(twof_t))

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------- *

    Initialize a TWOFISH Context

    ctx  = Context
    bits = bits of the key (128, 192 or 256)
    key  = Initialization Key (min of 'bits' size)

    Return 0 for OK or -1 error

 * ---------------------------------------------------------------------- */

int twofish_init(twof_t *ctx,int bits,const void *key);

/* ------------------------------------------------------------------ *
   These encode just one block of data (assumed of TWOFISH_BLOCK_SIZE
   bytes long) and are intended to be used by routines that need
   to encrypt one block at the time (eg. cipher.c)

   'orig' and 'dest' can point to the same block of memory
 * ------------------------------------------------------------------ */

void twofish_encode_block(twof_t *ctx,void *dest,const void *orig);
void twofish_decode_block(twof_t *ctx,void *dest,const void *orig);

#ifdef __cplusplus
};
#endif


#endif
