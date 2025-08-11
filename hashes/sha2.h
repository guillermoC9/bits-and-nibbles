/*
    sha2.h

    Hashing SHA-2: SHA-224, SHA-256, SHA-384 y SHA-512.

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

#ifndef SHA2_DE_TOM_ST_DENIS
#define SHA2_DE_TOM_ST_DENIS

#include "sha1.h"

#define SHA224_SIZE          28
#define KEY_SIZE_SHA224     (SHA224_SIZE * 2)
#define BUF_SIZE_SHA224     (KEY_SIZE_SHA224 + 1)

#define SHA256_SIZE          32
#define KEY_SIZE_SHA256     (SHA256_SIZE * 2)
#define BUF_SIZE_SHA256     (KEY_SIZE_SHA256 + 1)

#define SHA384_SIZE          48
#define KEY_SIZE_SHA384     (SHA384_SIZE * 2)
#define BUF_SIZE_SHA384     (KEY_SIZE_SHA384 + 1)

#define SHA512_SIZE          64
#define KEY_SIZE_SHA512     (SHA512_SIZE * 2)
#define BUF_SIZE_SHA512     (KEY_SIZE_SHA512 + 1)

#define SHA512_224_SIZE      SHA224_SIZE
#define KEY_SIZE_SHA512_224  KEY_SIZE_SHA224
#define BUF_SIZE_SHA512_224  BUF_SIZE_SHA224

#define SHA512_256_SIZE      SHA256_SIZE
#define KEY_SIZE_SHA512_256  KEY_SIZE_SHA256
#define BUF_SIZE_SHA512_256  BUF_SIZE_SHA256

/** Contexto SHA-256 y SHA-224 */

typedef struct
{
    unsigned int state[8];
    unsigned char buffer[64];
    u64_t bitcount;
    int pos;
} sha224_t,sha256_t;

/** Contexto SHA-384, SHA-512, SHA-512_224, SHA-512_256 */

typedef struct
{
    u64_t state[8];
    unsigned char buffer[128];
    u64_t hi,lo;
    int pos;
} sha512_t,sha384_t;


/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/** SHA-224 */

void sha224_init(sha224_t *ctx);
void sha224_update(sha224_t *ctx,const void *buf, size_t tam);
void sha224_final(sha224_t *ctx,void *hash);

/** SHA-256 */

void sha256_init(sha256_t *ctx);
void sha256_update(sha256_t *ctx,const void *buf, size_t tam);
void sha256_final(sha256_t *ctx,void *hash);


/** SHA-384 */

void sha384_init(sha384_t *ctx);
void sha384_update(sha384_t *ctx,const void *buf, size_t tam);
void sha384_final(sha384_t *ctx,void *hash);


/** SHA-512 */

void sha512_init(sha512_t *ctx);
void sha512_update(sha512_t *ctx,const void *buf, size_t tam);
void sha512_final(sha512_t *ctx,void *hash);

/** SHA-512/224 */

void sha512_224_init(sha512_t *ctx);
void sha512_224_update(sha512_t *ctx,const void *buf, size_t tam);
void sha512_224_final(sha512_t *ctx,void *hash);

/** SHA-512/256 */

void sha512_256_init(sha512_t *ctx);
void sha512_256_update(sha512_t *ctx,const void *buf, size_t tam);
void sha512_256_final(sha512_t *ctx,void *hash);


/* -------------------------------------------------- *
   no-destructive version of shaX_final(), which
   allow get the current hash and keep going
 * -------------------------------------------------- */

void sha224_partial(sha224_t *ctx,void *hash);
void sha256_partial(sha256_t *ctx,void *hash);
void sha384_partial(sha384_t *ctx,void *hash);
void sha512_partial(sha512_t *ctx,void *hash);

void sha512_224_partial(sha512_t *ctx,void *hash);
void sha512_256_partial(sha512_t *ctx,void *hash);


#ifdef __cplusplus
}
#endif

#endif
