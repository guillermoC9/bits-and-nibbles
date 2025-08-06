/*
    sha1.h

    SHA-1 Hashing functions

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

        - Written by Guillermo Amodeo Ojeda using some code from SSHSHA.C and
          SSH.H that came with PUTTY 0.60

                                --oO0Oo--
*/


#ifndef SHA1_ADAPTADAS_DE_PUTTY
#define SHA1_ADAPTADAS_DE_PUTTY

#include "stuff.h"

/** Sizes */

#define SHA1_SIZE        20
#define KEY_SIZE_SHA1   (SHA1_SIZE * 2)
#define BUF_SIZE_SHA1   (KEY_SIZE_SHA1 + 1)

/** Context SHA-1 */

typedef struct
{
    unsigned int    estado[5];
    unsigned char   buffer[64];
    int             pos;
    u64_t           bitcount;
} sha1_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/** SHA-1 */

void sha1_init(sha1_t *ctx);
void sha1_update(sha1_t *ctx,const void *buf, size_t tam);
void sha1_final(sha1_t *ctx,void *hash);

/* -------------------------------------------------- *
   no-destructive version of sha1_final(), which
   allow get the current hash and keep going
 * -------------------------------------------------- */

void sha1_partial(sha1_t *ctx,void *hash);

#ifdef __cplusplus
}
#endif

#endif
