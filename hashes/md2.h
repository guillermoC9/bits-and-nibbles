/*
    md2.h

    MD2 Hashing functions

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

        - Written by Guillermo Amodeo Ojeda using the info in RFC-1319 and wikipedia.

                                --oO0Oo--
*/

#ifndef GUILLE_MD2_STUFF
#define GUILLE_MD2_STUFF

#include "stuff.h"

#define MD2_SIZE         16
#define KEY_SIZE_MD2     (MD2_SIZE * 2)
#define BUF_SIZE_MD2     (KEY_SIZE_MD2 + 1)

/** Context*/

typedef struct
{
	unsigned char state[48];
	unsigned char data[16];
	unsigned char checksum[16];
	unsigned int  count;
} md2_t;

/** Funcs */

#ifdef __cplusplus
extern "C" {
#endif

/** MD2 */

void md2_init(md2_t *ctx);
void md2_update(md2_t *ctx,const void *buf, size_t tam);
void md2_final(md2_t *ctx,void *hash);

/** no-destructive version of md2_final(), which allow get the current hash and keep going */

void md2_partial(md2_t *ctx,void *hash);


#ifdef __cplusplus
}
#endif

#endif
