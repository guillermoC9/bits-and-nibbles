/*
    aria.h

    ARIA Encryption Routines

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

        - Written by Guillermo Amodeo Ojeda using info and code from RFC-5794
          and the reference files which comes in ARIA.ZIP from KISA, original 
          developer of the algorithm. You can found them here: 

          * RFF-5794    https://www.rfc-editor.org/rfc/rfc5794.html
          * ARIA.ZIP    https://seed.kisa.or.kr/kisa/Board/19/detailView.do

                                --oO0Oo--
*/

#ifndef KISA_ARIA_CIPHER
#define KISA_ARIA_CIPHER

#include "stuff.h"

#define ARIA_128_KEY_SIZE   16
#define ARIA_192_KEY_SIZE   24
#define ARIA_256_KEY_SIZE   32

#define ARIA_MAX_KEY_SIZE   32
#define ARIA_BLOCK_SIZE     16
#define ARIA_IV_SIZE        16
#define ARIA_CTR_IV_SIZE    12

#define ARIA_NUM_IB          4   /* Num integers per block (128 bits) */
#define ARIA_MAX_IB         16   /* Max integers per block */
#define ARIA_KEY_IB         17   /* Num of blocks per scheduled key */

/** Context */

typedef struct _aria_ctx_
{
    unsigned int  enckeysched[ARIA_KEY_IB][ARIA_NUM_IB];    /* Key schedules */
    unsigned int  deckeysched[ARIA_KEY_IB][ARIA_NUM_IB];    /* Key schedules */

    unsigned char  iv[ARIA_IV_SIZE];

    int     nr,kbits;      /* Number of rounds, bits of the key, mode of operation */

    const unsigned int *kr0,*kr1,*kr2;    /* Key rounds */    
} aria_t;

/** Macros */

#define aria_bits(_1)     ((_1)->kbits)
#define aria_end(_1)      memset(_1,0,sizeof(aria_t))

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------- *

    Initialize an ARIA Context

    ctx  = Context
    bits = bits of the key (128, 192 or 256)
    key  = Initialization Key (min of 'bits' size)

    Return 0 for OK or -1 error

 * ----------------------------------------------------------------- */

int aria_init(aria_t *ctx,int bits,const void *key);

/* ------------------------------------------------------------------ *
   These encode just one block of data (assumed of ARIA_BLOCK_SIZE
   bytes long) and are intended to be used by routines that need
   to encrypt one block at the time (eg. crypto.c)

   'orig' and 'dest' can point to the same block of memory
 * ------------------------------------------------------------------ */

void aria_encode_block(aria_t *ctx,void *dest,const void *orig);
void aria_decode_block(aria_t *ctx,void *dest,const void *orig);

#ifdef __cplusplus
};
#endif


#endif
