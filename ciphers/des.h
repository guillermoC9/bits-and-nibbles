/*
    des.h

    Data Encryption Standard Routines

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

        - Adapted by Guillermo Amodeo Ojeda from Richard Outerbridge's
          public domain implementation of DES, which now we know is
          based on the first implementation by IBM and published as
          public domain in a file called LUCIFER.C, which is the original
          name of the algorithm.

                                --oO0Oo--
*/

#ifndef DES_FROM_RICHARD_OUTERBRIDGE
#define DES_FROM_RICHARD_OUTERBRIDGE

#include <string.h>

/** Tamaños de cosas */

#define DES_KEY_SIZE    24      /** key size (Triple DES) */
#define DES_BLOCK_SIZE  8
#define DES_IV_SIZE     8

/** DES Context */

typedef struct
{
    int     kbits;    /** Key size in bits */

    unsigned int   KL[32];   /** Left Key (encode) */
    unsigned int   KM[32];   /** Middle Key (encode) */
    unsigned int   KR[32];   /** Right Key (encode) */
    unsigned int   KLd[32];  /** Left Key (decode) */
    unsigned int   KMd[32];  /** Middle Key (decode) */
    unsigned int   KRd[32];  /** Right Key (decode) */

} des_t;

/** Macros */

#define des_bits(_1)     ((_1)->kbits)
#define des_end(_1)      memset(_1,0,sizeof(des_t))

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Initialize a Triple DES Context

    ctx = Context
    key = Initialization Key (min of DES_KEY_SIZE bytes)

    Return 0 for OK or -1 error
 * -------------------------------------------------- */

int des_init(des_t *ctx,const void *key);

/* -------------------------------------------------- *
    this is Simple DES (56 bits).

   OBSOLETE but supported here because PKCS-8 can
   use it to encrypt private keys, and in fact OpenSSL
   uses it to encrypt then by default.

   'key' is DES_IV_SIZE (8 bytes = 64 bits).
 * -------------------------------------------------- */

int des56_init(des_t *ctx,const void *key);

/* -------------------------------------------------- *
   These encode just one block of data (assumed
   DES_BLOCK_SIZE bytes) and are intended to be used
   by routines that need to encrypt one block at the
   time (eg. crypto.c)

    'orig' and 'dest' can point to the same block
    of memory
 * -------------------------------------------------- */

void des_encode_block(des_t *ctx,void *dest,const void *orig);
void des_decode_block(des_t *ctx,void *dest,const void *orig);

#ifdef __cplusplus
};
#endif

#endif
