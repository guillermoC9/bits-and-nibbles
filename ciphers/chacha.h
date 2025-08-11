/*
    chacha.h

    Chacha Stream Cyphers

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
#ifndef DJB_CHACHA
#define DJB_CHACHA

#include "stuff.h"

/** Valid Rounds */

#define CHACHA8_ROUNDS       8
#define CHACHA12_ROUNDS     12
#define CHACHA20_ROUNDS     20

/** Sizes */

#define CHACHA_128_KEY_SIZE     16
#define CHACHA_256_KEY_SIZE     32

#define CHACHA_IV_SIZE           8
#define CHACHA20_IV_SIZE        12
#define CHACHA_BLOCK_SIZE       64

/** Chacha Context */

typedef struct
{
    int     rounds,kbits;
    unsigned int   input[16];
} chacha_t;

/** Macros */

#define chacha_bits(_1)              ((_1)->kbits)
#define chacha_end(_1)               memset((_1),0,sizeof(chacha_t))
#define chacha20_end(_1)             memset((_1),0,sizeof(chacha_t))

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------- *
   Initialize a Chacha context

    ctx    = Context to initialize
    bits   = bits of key (128 or 256)
    key    = Initialization key (at least 'bits'/8 bytes of size)
    rounds = Number of rounds (8, 12 or 20)
    iv     = optional 64 bits of IV

    Return 0 for OK or -1 for error
 * -------------------------------------------------- */

int chacha_start(chacha_t *ctx,int bits,const void *key,int rounds,const void *iv);

/* -------------------------------------------------- *
   This function allows the IV to be shuffled any 
   time with 64 bits of data (8 bytes)
 * -------------------------------------------------- */

void chacha_shuffle(chacha_t *ctx,const void *iv);

/* -------------------------------------------------- *
    Encode/Decode using chacha

    NOTE: 'orig' and 'dest' are allowed to point to
          the same buffer
 * -------------------------------------------------- */

void chacha(chacha_t *ctx,void *dest,const void *orig,size_t tam);

/* -------------------------------------------------- *
   IETF version of Chacha20, which uses a 256 bits
   key, 20 rounds, a 96 bits IV and a 32 bits counter.
 * -------------------------------------------------- */

int  chacha20_start(chacha_t *ctx,const void *key,const void *iv,unsigned int cnt);
void chacha20_shuffle(chacha_t *ctx,const void *iv,unsigned int cnt);
void chacha20(chacha_t *ctx,void *dest,const void *orig,size_t tam);

#ifdef __cplusplus
};
#endif


#endif
