/*
    rc4.h

    RC4 Stream Cypher

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

        - Written by Guillermo Amodeo Ojeda using the info from:

            http://en.wikipedia.org/wiki/RC4.

                                --oO0Oo--
*/

#ifndef RC4_GAO
#define RC4_GAO

#include <string.h>

/** Sizes */

#define RC4_40_KEY_SIZE     5
#define RC4_128_KEY_SIZE    16

/** Context RC-4 */

typedef struct
{
    unsigned char S[256];
    int j,i;
} rc4_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------- *
   Initialize an RC4 context

    ctx   = Context
    bits  = bits of key (40, 128, ...)
    key   = Initialization key (at least 'bits'/8 bytes of size)

    Return S9_OK (0) or negative error
 * -------------------------------------------------- */

int rc4_init(rc4_t *ctx,int bits,const void *key);

/* -------------------------------------------------- *
    Encode/Decode using RC4

    NOTE: 'orig' and 'dest' can point to the same buffer
 * -------------------------------------------------- */

void rc4(rc4_t *ctx,void *dest,const void *orig,size_t tam);


#ifdef __cplusplus
};
#endif


#endif
