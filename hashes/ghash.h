/*
    ghash.h

    Galois Hashing

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

        - Originally written by Guillermo Amodeo Ojeda

        - Galois Hash is a hashing algorithm based on the function for the
          multiplication of points on finite fields created by Evariste Galois,
          aka GF(x,y) and used on the GCM mode of AES.

        - Written using NIST info on AES-GCM

                                --oO0Oo--
*/


#ifndef GAO_GALOIS_HASH
#define GAO_GALOIS_HASH

#include "stuff.h"

/** Sizes */

#define GHASH_SIZE    16

/** Context GHASH */

typedef struct
{
	unsigned char    iv[GHASH_SIZE];
	unsigned char    estado[GHASH_SIZE];
	unsigned char    buffer[GHASH_SIZE];
	size_t           pos;
    u64_t            bits;
} ghash_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/** Initialzes the GHASH context 'ctx' with 'iv',
    which is assumed to be GHASH_SIZE bytes
    if not NULL.
*/

void ghash_init(ghash_t *ctx,const void *iv);

void ghash_update(ghash_t *ctx,const void *buf, size_t tam);
void ghash_final(ghash_t *ctx,void *hash);

/** no-destructive version of ghash_final(), which allow get the current hash and keep going */

void ghash_partial(ghash_t *ctx,void *hash);

#ifdef __cplusplus
}
#endif

#endif
