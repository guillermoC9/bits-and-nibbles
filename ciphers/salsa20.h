/*
    salsa20.h

    Salsa20 Stream Cypher

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

        - Written by Guillermo Amodeo Ojeda using the info and code from:

            * https://en.wikipedia.org/wiki/Salsa20
            * https://cr.yp.to/snuffle/salsa20/regs/salsa20.c
            * https://cr.yp.to/streamciphers/timings/estreambench/submissions/salsa20/chacha8/ref/chacha.c
            * RFC-7539 at https://tools.ietf.org/html/rfc7539

                                --oO0Oo--
*/

#ifndef DJB_SALSA20
#define DJB_SALSA20

#include "stuff.h"

/** Valid Rounds */

#define SALSA20_ROUNDS      20

/** Sizes */


#define SALSA20_128_KEY_SIZE    16
#define SALSA20_256_KEY_SIZE    32

#define SALSA20_IV_SIZE          8
#define SALSA20_BLOCK_SIZE      64

/** Salsa20 Context */

typedef struct
{
    unsigned int   input[16];
} salsa20_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------- *

   Initialize a Salsa20 context

    ctx   = Contextto initialize
    bits  = bits of the key (128 or 256)
    key   = Initialization key (at least 'bits'/8 bytes of size)
    iv    = optional 64 bits of IV

    Return 0 for OK or -1 error
 * -------------------------------------------------- */

int salsa20_start(salsa20_t *ctx,int bits,const void *key,const void *iv);

/* -------------------------------------------------- *
   Destroys a Salsa20 context
 * -------------------------------------------------- */

void salsa20_end(salsa20_t *ctx);

/* -------------------------------------------------- *
   This function allows the IV to be shuffled any
   time with 64 bits of data
* -------------------------------------------------- */

void salsa20_shuffle(salsa20_t *ctx,const void *iv);

/* -------------------------------------------------- *
    Encode/Decode using Salsa20

    NOTE: 'orig' and 'dest' are allowed to point to the same buffer
 * -------------------------------------------------- */

void salsa20(salsa20_t *ctx,void *dest,const void *orig,size_t tam);

#ifdef __cplusplus
};
#endif


#endif
