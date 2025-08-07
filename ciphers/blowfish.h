/*
    blowfish.h

    Blowfish Encryption Routines

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

    	- Adapted by Guillermo Amodeo Ojeda from Bruce Schneier's Public Domain
    	  implementation published in its website:

    	  https://www.schneier.com/academic/blowfish/download.html

                                --oO0Oo--
*/

#ifndef BF_FROM_BRUCE_SCHNEIER
#define BF_FROM_BRUCE_SCHNEIER

#include "stuff.h"

/** Tamaños de cosas */

#define BLOWF_BLOCK_SIZE    8
#define BLOWF_IV_SIZE       8

#define BLOWF_MIN_KEY_SIZE  4
#define BLOWF_MAX_KEY_SIZE  56

/** BlowFish Context */

typedef struct
{
    unsigned int   P[18];
    unsigned int   S[4][256];
} blowf_t;

/** Macros */

#define blowf_end(_1)    memset(_1,0,sizeof(blowf_t))

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *

    Initialize a Blowfish Context

    ctx = Context
    keyword = Initialization Key
    ksize = size in bytes of keyword
    Return 0 for OK or -1 error
 * -------------------------------------------------- */

int blowf_init(blowf_t *ctx,const void *keyword,size_t ksize);

/* ------------------------------------------------------------------------ *
    These encode just one block of data (assumed BLOWF_BLOCK_SIZE bytes)
    and are intended to be used by routines that need to encrypt one
    block at the time (eg. crypto.c)

    'orig' and 'dest' can point to the same block of memory
 * ------------------------------------------------------------------------ */

void blowf_encode_block(blowf_t *ctx,void *dest,const void *orig);
void blowf_decode_block(blowf_t *ctx,void *dest,const void *orig);

#ifdef __cplusplus
};
#endif

#endif
