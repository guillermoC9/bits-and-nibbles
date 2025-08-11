/*
    ripe320.h

    RIPEMD-320 Hashing functions

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

        - Written using information and source code by Wei Day and Kevin Springle's
          public domain implementation.

        - RIPEMD was designed by Hans Dobbertin, Antoon Bosselaers and Bart Preneel
          as an improvement to Ron Rivest's MD (MD4, MD5)

                                --oO0Oo--
*/

#ifndef RUTINAS_DE_RIPEMD_320
#define RUTINAS_DE_RIPEMD_320

#include "stuff.h"

/** Sizes */

#define RIPE320_SIZE         40
#define KEY_SIZE_RIPE320    (RIPE320_SIZE * 2)
#define BUF_SIZE_RIPE320    (KEY_SIZE_RIPE320 + 1)

/** Context RIPE320 */

typedef struct
{
    u64_t          cont;
	  unsigned int   estado[10];
	  unsigned char  datos[64];
    int pos;
} ripe320_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/** RIPE320 */

void ripe320_init(ripe320_t *ctx);
void ripe320_update(ripe320_t *ctx,const void *buf, size_t tam);
void ripe320_final(ripe320_t *ctx,void *hash);

/* -------------------------------------------------- *
   no-destructive version of ripe320_final(),
   which allow get the current hash and keep going
 * -------------------------------------------------- */

void ripe320_partial(ripe320_t *ctx,void *hash);


#ifdef __cplusplus
}
#endif

#endif
