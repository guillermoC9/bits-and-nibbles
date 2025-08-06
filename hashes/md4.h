/*
    md4.h

    MD4 Hashing functions

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

        - Written by Guillermo Amodeo Ojeda using the info in RFC-1320 and wikipedia.

                                --oO0Oo--

*/

#ifndef GUILLE_MD4_STUFF
#define GUILLE_MD4_STUFF

#include "stuff.h"

#define MD4_SIZE         16
#define KEY_SIZE_MD4    (MD4_SIZE * 2)
#define BUF_SIZE_MD4    (KEY_SIZE_MD4 + 1)

/** Context MD4 */

typedef struct
{
    u64_t         count;        /** Counter of processed bits */
	unsigned int  state[4];     /** Current Status (hash) */
	unsigned char data[64];     /** Data buffer waiting to be processed */
    int           pos;          /** next free position in buffer  */
} md4_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/** MD4 */

void md4_init(md4_t *ctx);
void md4_update(md4_t *ctx,const void *buf, size_t tam);
void md4_final(md4_t *ctx,void *hash);

/** no-destructive version of md4_final(), which allow get the current hash and keep going */

void md4_partial(md4_t *ctx,void *hash);


#ifdef __cplusplus
}
#endif

#endif
