/*
    aes.h

    Advanced Encryption Standard Routines

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

        - Written by Guillermo Amodeo Ojeda using code from SSHAES.C that
          came with PUTTY 0.60.

        - Many comments come from the original code in putty files but
          modified to match the changes made on them.

                                --oO0Oo--
*/

#ifndef AES_FROM_PUTTY
#define AES_FROM_PUTTY

#include "stuff.h"

#define AES_MAX_NR         14		/* max number of rounds */
#define AES_MAX_NK          8		/* max number of integers in the input */
#define AES_MAX_NB          8		/* max number of integers per block (256 bits) */
#define AES_NUM_IB          4   /* num integers per block (128 bits) */

#define AES_MAX_SCHED       ((AES_MAX_NR + 1) * AES_MAX_NB)

#define AES_128_KEY_SIZE   16
#define AES_192_KEY_SIZE   24
#define AES_256_KEY_SIZE   32

#define AES_MAX_KEY_SIZE   32
#define AES_BLOCK_SIZE     16
#define AES_IV_SIZE        16

/** Context AES */

typedef struct _aes_ctx_
{
    unsigned int   keysched[AES_MAX_SCHED];
    unsigned int   invkeysched[AES_MAX_SCHED];

    int     Nb, Nr,kbits;

    void (*encode_func) (struct _aes_ctx_ *ctx,unsigned int * block);
    void (*decode_func) (struct _aes_ctx_ *ctx,unsigned int * block);

} aes_t;

/** Macros */

#define aes_bits(_1)     ((_1)->kbits)
#define aes_end(_1)      memset(_1,0,sizeof(aes_t))

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------- *

    Initialize an AES Context

    ctx  = Context
    bits = bits of the key (128, 192 or 256)
    key  = Initialization Key (min of 'bits' size)

    Return 0 for OK or -1 error

 * ----------------------------------------------------------------- */

int aes_init(aes_t *ctx,int bits,const void *key);

/* ------------------------------------------------------------------ *
   These encode just one block of data (assumed of AES_BLOCK_SIZE
   bytes long) and are intended to be used by routines that need
   to encrypt one block at the time (eg. crypto.c)

   'orig' and 'dest' can point to the same block of memory
 * ------------------------------------------------------------------ */

void aes_encode_block(aes_t *ctx,void *dest,const void *orig);
void aes_decode_block(aes_t *ctx,void *dest,const void *orig);

#ifdef __cplusplus
};
#endif


#endif
