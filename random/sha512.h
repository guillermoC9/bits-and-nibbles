/*
    sha512.h

    Hashing SHA3-512.

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

        - Adapted by Guillermo Amodeo Ojeda from code at:

          https://github.com/brainhub/SHA3IUF/blob/master/sha3.c

          written by Andrey Jivsov, who donated it to the
          public domain.

                                --oO0Oo--

*/

#ifndef SHA512_BASADO_EN_EL_DE_ANDREY_JIVSOV
#define SHA512_BASADO_EN_EL_DE_ANDREY_JIVSOV

#include "stuff.h"


#define SHA512_SIZE          64
#define KEY_SIZE_SHA512     (SHA512_SIZE * 2)
#define BUF_SIZE_SHA512     (KEY_SIZE_SHA512 + 1)

/** Contexto SHA3 **/

typedef struct sha3_ctx
{
    u64_t          hash[25];   /* Words */
    unsigned char  buf[200];   /* Bytes */
    int            idx;
    int            num;
    int            tam_digest;
    int            pure_keccak;
} sha3_t;

/** Macros **/

/* -------------------------------------------------- *
   SHA-3 is Keccak
 * -------------------------------------------------- */

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

void sha512_init(sha3_t *ctx);
void sha512_update(sha3_t *ctx,const void *buf, size_t tam);
void sha512_final(sha3_t *ctx,void *hash);

#ifdef __cplusplus
}
#endif

#endif
