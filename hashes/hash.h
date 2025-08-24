/*
    hash.h

    Cryptographic Hashing functions

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

#ifndef CRYTOGRAPHIC_HASH
#define CRYTOGRAPHIC_HASH

#include "md2.h"
#include "md4.h"
#include "md5.h"
#include "sha1.h"
#include "sha2.h"
#include "sha3.h"
#include "ripe128.h"
#include "ripe256.h"
#include "ripe160.h"
#include "ripe320.h"
#include "poly1305.h"

/** Hash Algorithms IDs */

enum
{
    HASH_NONE=0,

    /* MD */

    HASH_MD2,
    HASH_MD4,
    HASH_MD5,

    /* SHA */

    HASH_SHA1,
    HASH_SHA224,
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512,
    HASH_SHA512_224,
    HASH_SHA512_256,

    /* SHA-3 */

    HASH_SHA3_224,
    HASH_SHA3_256,
    HASH_SHA3_384,
    HASH_SHA3_512,

    /* Keccak */

    HASH_KECCAK_224,
    HASH_KECCAK_256,
    HASH_KECCAK_384,
    HASH_KECCAK_512,

    /* Shake */

    HASH_SHAKE_128,
    HASH_SHAKE_256,

    /* RIPEMD */

    HASH_RIPE_128,
    HASH_RIPE_160,
    HASH_RIPE_256,
    HASH_RIPE_320,

    /* POLY1305 */

    HASH_POLY1305,

    /* New algorithms go over this comment */

    HASH_NUM_HASHES
};

#define FIRST_HASH   (HASH_NONE + 1)
#define LAST_HASH    (HASH_NUM_HASHES - 1)

/** Biggest hash produced (SHA-512 at the moment) */

#define MIN_HASH_SIZE     MD2_SIZE
#define MAX_HASH_SIZE     SHA512_SIZE

/* ----------------------------------------------------- *
    Hash calculation with support for all algorithms

    IMPORTANT: 
    
      Poly1305 is not really a hash algorithm but 
      a HMAC Function, so calls to hash_init() must 
      include a key of POLY1305_SIZE bytes (16). 
      Thus the result from has_final() is actually 
      an HMAC rather than a hash.

      Shake128/256 are neither hash algorithms,
      they are HMAC Key Derived Functions, so
      calls to hash_init() may include a key (not 
      actually compulsory like when using POLY1305) 
      to save a call to  hash_update(), and the
      hash resulting from hash_final() would
      be a 16/32 bytes key rather than a hash.

      If you need a bigger key, you can call 
      hash_final_hkdf() instead, which allows
      an amount of bytes to be returned instead
      the 16/32 of the digest size ;-)

      It may seem stupid to allow these two
      algorithms to be used in here if they are
      not pure hash algorithms, but allowing them
      here ease to use them from anywhere
      else like hmac_...() functions, or for
      ed448 signature algorithms, as the caller 
      know how to handle the differences.

 * ----------------------------------------------------- */

typedef struct
{
    int       alg;
    char     *namec;
    wchar_t  *namew;

    union
    {
        md2_t     md2;
        md4_t     md4;
        md5_t     md5;

        sha1_t    sha1;
        sha224_t  sha224;
        sha256_t  sha256;
        sha384_t  sha384;
        sha512_t  sha512;
        sha3_t    sha3;

        ripe128_t ripe128;
        ripe160_t ripe160;
        ripe256_t ripe256;
        ripe320_t ripe320;

        poly1305_t poly;
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
   by basically using the same method and blocksizes,
   although in the case of SHA3 the blocksizes are
   standarized by NIST.

   In the case of Poly1305 from D J Bernstein the
   code DOESN'T follow the RFCs with ipad and opad,
   as this algorithm already produces an HMAC, so
   the interface here is a mere passthrough to
   allow a caller to use this interface as a way to
   to solve all their HMAC needs.

   Shake128/256 works kind of like the RFCs but not
   quite exactly so it should be avoided. As stated
   before, it was added for ease handling of the
   algorithms from anywhere else.
 * ------------------------------------------------ */

/** Block size for normal HMAC as RFCs */

#define HMAC_SIZE         64

/** Blocksize for HMAC-SHA384 and HMAC-SHA512 as RFCs */

#define HMAC_BIG_SIZE    128

/** Blocksizes for HMAC-SHA3_... as per NIST tables */

#define HMAC_SHA3_224_SIZE   144  /* 1152 bits */
#define HMAC_SHA3_256_SIZE   136  /* 1088 bits */
#define HMAC_SHA3_384_SIZE   104  /*  832 bits */
#define HMAC_SHA3_512_SIZE    72  /*  576 bits */

#define HMAC_MAX_BLOCK_SIZE  150

typedef struct
{
    int tam_block,tam_hash,passthru;

    hash_t h;

    unsigned char hkey[HMAC_MAX_BLOCK_SIZE];

} hmac_t;


/* ------------------------------------------------ *
   Get information from the context
 * ------------------------------------------------ */

#define hmac_alg_code(_ctx)   ((_ctx)->h.alg)
#define hmac_alg_name(_ctx)   ((_ctx)->h,namec)
#define hmac_alg_namew(_ctx)  ((_ctx)->h.namew)


#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------ *
   Given a hash algorithm id returns its name
 * ------------------------------------------------ */

char *hash_name(int hash);

/* ------------------------------------------------ *
   Same as before but it returns a wchar_t *
 * ------------------------------------------------ */

wchar_t *hash_namew(int hash);

/* ------------------------------------------------ *
   Given a hash name it retursn its algorithm id
 * ------------------------------------------------ */

int hash_alg(const char *hash);

/* ------------------------------------------------ *
   Same as before but it returns a wchar_t *
 * ------------------------------------------------ */

int hash_algw(const wchar_t *hash);

/* ------------------------------------------------ *
   Return the result size of a hash algorithm
 * ------------------------------------------------ */

int hash_size(int alg);

/* ------------------------------------------------ *
   Calculates the hash of 'tam' bytes from 'data'
   and copy the result in 'hash' which is suppossed
   to have enough space to hold it.

   Returns the size of the result or 0 if the
   algorithm  'alg' is not recognized.
 * ------------------------------------------------ */

int calc_hash(int alg,const void *data,int tam,void *hash);

/* ------------------------------------------------ *
   Calculates the hash of the file 'fich' and copy
   the result in 'hash' which is suppossed
   to have enough space to hold it.

   Optionally you can obtain the size of the file
   in 'tam'.

   Returns the size of the result or 0 if the
   algorithm  'alg' is not recognized.

   return negative if cannot read the file
 * ------------------------------------------------ */

int calc_hash_file(int alg,const char *fich,void *hash,u64_t *tam);
int calc_hash_filew(int alg,const wchar_t *fich,void *hash,u64_t *tam);

/* ------------------------------------------------ *
   Same as the previous but it also adds the name
   of the file in the hash ('fich')
 * ------------------------------------------------ */

int calc_hash_file_and_name(int alg,const char *fich,void *hash,u64_t *tam);
int calc_hash_file_and_name(int alg,const char *fich,void *hash,u64_t *tam);

/* ------------------------------------------------ *
   The classical way of calculating hashes by
   initializing a context and update it with
   a series to call to the update function.

   'key' is only used for those algorithms
   that take a parameter like poly1305

   returns the size of the hash, 0 if do not
   recognize it or -1 in parameter errors
 * ------------------------------------------------ */

int  hash_init(hash_t *ctx,int alg,const void *key,int tam_key);
void hash_update(hash_t *ctx,const void *datos,size_t tam);
void hash_final(hash_t *ctx,void *hash);

/* Use this final for HKDF liek shake-128 or shake-256 */

void hash_final_hkdf(hash_t *ctx,void *hash,size_t len);

/* ------------------------------------------------ *
    Functiosn ot update using strings
 * ------------------------------------------------ */

void hash_update_str(hash_t *ctx,const char *str);
void hash_update_wcs(hash_t *ctx,const wchar_t *str);


/* ------------------------------------------------ *
   no-destructive version of hash_final(),
   which allow get the current hash and keep going
 * ------------------------------------------------ */

void hash_partial(hash_t *ctx,void *hash);

/* ------------------------------------------------ *
   Calculate the HMAC (RFC-2104 ,RFC-6234 and
   RFC-4868) of 'tam' bytes from 'data' and copies
   the result onto 'hash', which is suppossed to
   have enough space to hold it.

   Returns the size of the result or 0 if there
   was an error like the algorithm  'alg' is not
   recognized or a parameter is wrong.

   As explained before, if you use poly1305 the
   code doesn't follow the RFCs but uses the
   algorithm directly.
 * ------------------------------------------------ */

int calc_hmac(int alg,const void *key,int tam_key,const void *datos,int tam,void *hash);

/* ------------------------------------------------ *
   The classical way of calculating an HMAC by
   initialazing a context and update it with
   a series of calls to the update function.
 * ------------------------------------------------ */

int  hmac_init(hmac_t *ctx,int alg,const void *key,size_t tam_key);
void hmac_update(hmac_t *ctx,const void *datos,size_t tam);
void hmac_final(hmac_t *ctx,void *hash);

void hmac_update_str(hmac_t *ctx,const char *str);
void hmac_update_wcs(hmac_t *ctx,const wchar_t *str);

/** no-destructive version of hmac_final(), which allow get the current hash and keep going */

void hmac_partial(hmac_t *ctx,void *hash);

/* ------------------------------------------------ *
   Calculates the hmac of the file 'fich' using
   'key' for the hmac and copies the result in
   'hash', which is suppossed to have enough space
   to hold it.

   Optionally you can obtain the size of the file
   in 'tam'.

   Returns the size of the result or 0 if the
   algorithm  'alg' is not recognized.

   return negative if cannot read the file
 * ------------------------------------------------ */


int calc_hmac_file(int alg,const void *key,unsigned int tam_key,const char *fich,void *hash,u64_t *tam);
int calc_hmac_filew(int alg,const void *key,unsigned int tam_key,const wchar_t *fich,void *hash,u64_t *tam);

/* ------------------------------------------------ *
   Same as the previous but it also adds the name
   of the file in the hash ('fich')
 * ------------------------------------------------ */

int calc_hmac_file_and_name(int alg,const void *key,unsigned int tam_key,const char *fich,void *hash,u64_t *tam);
int calc_hmac_file_and_namew(int alg,const void *key,unsigned int tam_key,const wchar_t *fich,void *hash,u64_t *tam);

#ifdef __cplusplus
}
#endif

#endif
