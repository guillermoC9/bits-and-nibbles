/*
    md5.h

    MD5 Hashing functions

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

        - Written by Guillermo Amodeo Ojeda using the info in RFC-1321 and wikipedia.

                                --oO0Oo--
*/

#ifndef HASH_OF_MD5
#define HASH_OF_MD5

#include "stuff.h"

/** Sizes */

#define MD5_SIZE         16
#define KEY_SIZE_MD5    (MD5_SIZE * 2)
#define BUF_SIZE_MD5    (KEY_SIZE_MD5 + 1)

/** Context MD5 */

typedef struct
{
    u64_t cont;
	unsigned int state[4];
	unsigned char data[64];
    int pos;
} md5_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/** MD5 */

void md5_init(md5_t *ctx);
void md5_update(md5_t *ctx,const void *buf, size_t tam);
void md5_final(md5_t *ctx,void *hash);

/** no-destructive version of md5_final(), which allow get the current hash and keep going */

void md5_partial(md5_t *ctx,void *hash);


#ifdef __cplusplus
}
#endif

#endif
