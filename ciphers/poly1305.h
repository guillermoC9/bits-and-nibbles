/*
    poly1305.h

    Poly1305 HMAC functions

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

        - Adapted by Guillermo Amodeo Ojeda from Andrew Moon's public domain code
          for poly1305-donna on:

          https://github.com/floodyberry/poly1305-donna.


                                --oO0Oo--
*/

#ifndef __POLY1305_AM__
#define __POLY1305_AM__

#include "stuff.h"

#define POLY1305_SIZE        16
#define POLY1305_KEY_SIZE    32
#define POLY1305_BLOCK_SIZE  16
#define BUF_SIZE_POLY1305   ((POLY1305_SIZE * 2) + 1)

/** Context*/

typedef struct
{

	size_t leftover;

    unsigned int r[5];
    unsigned int h[5];
    unsigned int pad[4];

	unsigned char buffer[POLY1305_BLOCK_SIZE];
	unsigned char final;

} poly1305_t;


/** Funcs */

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------- *
   Initializes a Poly1305 context using 'key' which
   is assumed to have at least POLY1305_KEY_SIZE (32)
    bytes of unpredictable data.
 * -------------------------------------------------- */

void poly1305_init(poly1305_t *st, const void *key);

void poly1305_update(poly1305_t *st, const void *buf, size_t tam);
void poly1305_final(poly1305_t *st, void *hash);

void poly1305_partial(poly1305_t *st, void *hash);


#ifdef __cplusplus
}
#endif

#endif
