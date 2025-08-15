/*
    hmac.h

    Hash-derived Message Authentication Code functions

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

        - Originally written by Guillermo Amodeo Ojeda

                                --oO0Oo--
*/

#ifndef CRYTOGRAPHIC_HMAC
#define CRYTOGRAPHIC_HMAC

#include "md2.h"
#include "md5.h"
#include "sha1.h"
#include "sha2.h"

/** Hash Algorithms IDs */

enum
{
    HASH_NONE=0,

    /* MD */

    HASH_MD2,
    HASH_MD5,

    /* SHA */

    HASH_SHA1,
    HASH_SHA224,
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512,

    /* New algorithms go over this comment */

    HASH_NUM_HASHES
};

/** Biggest hash produced (SHA-512 at the moment) */

#define MIN_HASH_SIZE     MD2_SIZE
#define MAX_HASH_SIZE     SHA512_SIZE

/* ----------------------------------------------------- *
    Hash calculation with support for few algorithms
 * ----------------------------------------------------- */

typedef struct
{
    int       alg;
    char     *namec;
    wchar_t  *namew;

    union
    {
        md2_t     md2;
        md5_t     md5;
        
        sha1_t    sha1;
        sha224_t  sha224;
        sha256_t  sha256;
        sha384_t  sha384;
        sha512_t  sha512;
    } a;
} hash_t;

/* ------------------------------------------------ *
   Get information from the context
 * ------------------------------------------------ */

#define hash_alg_code(_ctx)   ((_ctx)->alg)
#define hash_alg_name(_ctx)   ((_ctx)->namec)
#define hash_alg_namew(_ctx)  ((_ctx)->namew)


/* ------------------------------------------------ *
   HMAC calculation from RFC-2104, RFC-6234 and
   RFC-4868 with support for multiple algorithms.

   It supports algorithms not covered by those RFC
   by basically using the same method and blocksizes.
 * ------------------------------------------------ */

/* Block size for normal HMAC as RFCs */

#define HMAC_SIZE         64

/* Blocksize for HMAC-SHA384 and HMAC-SHA512 as RFCs */

#define HMAC_BIG_SIZE        128
#define HMAC_MAX_BLOCK_SIZE  150

typedef struct
{
    int tam_block,tam_hash,passthru;

    hash_t h;

    unsigned char hkey[HMAC_MAX_BLOCK_SIZE];

} hmac_t;

/* ------------------------------------------------ *
   Functions
 * ------------------------------------------------ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------ *
   The classical way of calculating hashes by
   initializing a context and update it with
   a series to call to the update function.

   returns the size of the hash, 0 if do not
   recognize it or -1 in parameter errors
 * ------------------------------------------------ */

int  hash_init(hash_t *ctx,int alg);
void hash_update(hash_t *ctx,const void *datos,size_t tam);
void hash_final(hash_t *ctx,void *hash);

/* ------------------------------------------------ *
   Calculate the HMAC (RFC-2104 ,RFC-6234 and
   RFC-4868)
 * ------------------------------------------------ */

int  hmac_init(hmac_t *ctx,int alg,const void *key,size_t tam_key);
void hmac_update(hmac_t *ctx,const void *datos,size_t tam);
void hmac_final(hmac_t *ctx,void *hash);

/* -------------------------------------------- *
    This function generate an unpredictable 
    amount of data hashed with the specified
    hash algorithm. 
    
                 *** WARNING ***

    You may be tempted to use it as random data 
    generator, but I discorage it if you are 
    going to do something serious with the 
    generated data. 
    
    Unless of course you have measured the risk,
    or doing something like generating a random
    IV.
    
    Random data generation is not a trivial 
    business, specially for use in criptography.
 * -------------------------------------------- */


void hash_get_entropy(int hash,void *dest,size_t num);

#ifdef __cplusplus
}
#endif

#endif
