/*
    ripe128.h

    RIPEMD-128 Hashing functions

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

        - Initially written using information and source code by A. Bosselaers 
          on its personal webpage:
         
              https://homes.esat.kuleuven.be/~bosselae/ripemd128.html

        - Later changed to also use Wei Day and Kevin Springle's public domain 
          implementation.

        - RIPEMD was designed by Hans Dobbertin, Antoon Bosselaers and Bart Preneel
          as an improvement to Ron Rivest's MD (MD4, MD5)

                                --oO0Oo--
*/

#ifndef RIPEMD_128_HASH
#define RIPEMD_128_HASH

#include "stuff.h"

/** Sizes */

#define RIPE128_SIZE         16
#define KEY_SIZE_RIPE128     (RIPE128_SIZE * 2)
#define BUF_SIZE_RIPE128     (KEY_SIZE_RIPE128 + 1)

/** Context RIPE128 */

typedef struct
{
    u64_t cont;
	  unsigned int estado[4];
	  unsigned char  datos[64];
    int pos;
} ripe128_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/** RIPE160 */

void ripe128_init(ripe128_t *ctx);
void ripe128_update(ripe128_t *ctx,const void *buf, size_t tam);
void ripe128_final(ripe128_t *ctx,void *hash);

/* -------------------------------------------------- *
   no-destructive version of ripe128_final(),
   which allow get the current hash and keep going
 * -------------------------------------------------- */

void ripe128_partial(ripe128_t *ctx,void *hash);


#ifdef __cplusplus
}
#endif

#endif
