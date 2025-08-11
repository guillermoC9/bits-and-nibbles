/*
    sha3.h

    Hashing SHA-3: SHA3-224, SHA3-256, SHA3-384 y SHA3-512.

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

#ifndef SHA3_BASADO_EN_EL_DE_ANDREY_JIVSOV
#define SHA3_BASADO_EN_EL_DE_ANDREY_JIVSOV

#include "sha2.h"

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

void sha3_224_init(sha3_t *ctx);
void sha3_256_init(sha3_t *ctx);
void sha3_384_init(sha3_t *ctx);
void sha3_512_init(sha3_t *ctx);

int  sha3_init(sha3_t *ctx,int bits);

void sha3_update(sha3_t *ctx,const void *buf, size_t tam);
void sha3_final(sha3_t *ctx,void *hash);

/* -------------------------------------------------- *
   no-destructive version of sha3_final(), which
   allow get the current hash and keep going
 * -------------------------------------------------- */

void sha3_partial(sha3_t *ctx,void *hash);

/* Keccak variant */

typedef sha3_t           keccak_t;

/* Returns TRUE if bits is 224,256,384 or 512 or FALSE if not */

int  keccak_init(sha3_t *ctx,int bits);

#define keccak_update     sha3_update
#define keccak_final      sha3_final
#define keccak_partial    sha3_partial

/* SHAKE */

#define SHAKE128_SIZE   16
#define SHAKE256_SIZE   32

typedef sha3_t           shake_t;

/* -------------------------------------------------- *
   SHAKE criptographic entropy generator.

   bits = 128 or 256
   key  = key of the shake
   len  = lenght of key
 * -------------------------------------------------- */

int  shake_init(sha3_t *ctx,int bits,const void *key,size_t len);

/* -------------------------------------------------- *
   Gets 'len' bytes of SHAKE criptographic entropy.
 * -------------------------------------------------- */

void shake_get(sha3_t *ctx,void *buf, size_t tam);

/* -------------------------------------------------- *
   Cleans up the SHAKE context
 * -------------------------------------------------- */

void shake_clean(sha3_t *ctx);



#ifdef __cplusplus
}
#endif

#endif
