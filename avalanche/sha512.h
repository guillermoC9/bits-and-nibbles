/*
    sha512.h

    Hashing SHA-512.

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

        - The transform functions were adapted by Guillermo Amodeo Ojeda
          from the pulic domain files LibSha256.c and LibSha512.c
          written by Tom St Denis and published by Water Juice.

                                --oO0Oo--
*/

#ifndef SHA512_DE_TOM_ST_DENIS
#define SHA512_DE_TOM_ST_DENIS

#include "stuff.h"

#define SHA512_SIZE          64
#define KEY_SIZE_SHA512     (SHA512_SIZE * 2)
#define BUF_SIZE_SHA512     (KEY_SIZE_SHA512 + 1)

/** Context */

typedef struct
{
    u64_t state[8];
    unsigned char buffer[128];
    u64_t hi,lo;
    int pos;
} sha512_t;


/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/** SHA-512 */

void sha512_init(sha512_t *ctx);
void sha512_update(sha512_t *ctx,const void *buf, size_t tam);
void sha512_final(sha512_t *ctx,void *hash);

/* -------------------------------------------------- *
   no-destructive version of shaX_final(), which
   allow get the current hash and keep going
 * -------------------------------------------------- */

void sha512_partial(sha512_t *ctx,void *hash);

#ifdef __cplusplus
}
#endif

#endif
