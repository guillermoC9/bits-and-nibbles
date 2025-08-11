/*
    xtea.h

    XTEA Cypher

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

        - Adapted by Guillermo Amodeo Ojeda from public domain code by
          David Wheeler and Roger Needham from:

          https://en.wikipedia.org/wiki/XTEA

                                --oO0Oo--

*/


#ifndef XTEA_ADAPTED_FROM_DW_RN
#define XTEA_ADAPTED_FROM_DW_RN

#include "stuff.h"

#define XTEA_KEY_SIZE     16
#define XTEA_BLOCK_SIZE    8
#define XTEA_IV_SIZE       8

/* Cipher Block Chaining */

/** Standard XTEA */

#define XTEA_DEF_ROUNDS     32

/** XTEA Context */

typedef struct
{
    int     kbits;    /** key size in bits */
    int     num_rounds;

    unsigned int   k[4];     /** key */
    unsigned int   iv[2];    /** IV */
} xtea_t;

/** Macros */

#define xtea_bits(_1)     ((_1)->kbits)
#define xtea_rounds(_1)   ((_1)->num_rounds)
#define xtea_end(_1)      memset(_1,0,sizeof(xtea_t))

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    Initialize a XTEA Context

    ctx    = Context
    rounds = Number of rounds (32 by default)
    key    = Initialization Key (min of XTEA_KEY_SIZE bytes)

    Return 0 for OK or -1 error
 * -------------------------------------------------- */

int xtea_init(xtea_t *ctx,int rounds,const void *key);

/* -------------------------------------------------- *
   These encode just one block of data (assumed
   XTEA_BLOCK_SIZE bytes) and are intended to be used
   by routines that need to encrypt one block at the
   time (eg. crypto.c)

    'orig' and 'dest' can point to the same block of memory
 * -------------------------------------------------- */

void xtea_encode_block(xtea_t *ctx,void *dest,const void *orig);
void xtea_decode_block(xtea_t *ctx,void *dest,const void *orig);

#ifdef __cplusplus
};
#endif

#endif
