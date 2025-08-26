/*

    curves.h

    Elliptic Curve Cryptography Curves

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

    - Originally written by Guillermo Amodeo Ojeda using info from:

        * https://en.wikipedia.org/wiki/Elliptic-curve_cryptography

        * Hankerson, Menezes & Vanstone's "Guide to Elliptic Curve Cryptography"
          Springer (2004)

        * Michael Rosing's Book "Implementing Elliptic Curve Cryptography".
          Manning Publications Co (1998).

        * Phrack's "All Hackers Need To Know About Elliptic Curve Cryptography".
          http://www.phrack.org/issues/63/3.html (2005)

        * B. Poettering's articles at http://point-at-infinity.org/ecc/

        * RFC 5915 at https://tools.ietf.org/html/rfc5915

        * RFC 7748 at https://tools.ietf.org/html/rfc7748

        * RFC 7748 at https://tools.ietf.org/html/rfc8032

        * https://www.cs.miami.edu/home/burt/learning/Csc609.142/ecdsa-cert.pdf

        * http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf

        * SEC 2: Recommended Elliptic Curve Domain Parameters
          http://www.secg.org/sec2-v2.pdf

       * Handbook of Applied Cryptography - https://cacr.uwaterloo.ca/hac/

       * https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.204.9073&rep=rep1&type=pdf

    - The functions have been made time-unpredictable. Some are time-constant and some never
      take the same time to execute even if they are given exactly the same data.

    - We include Curve25519 and Curve448 in our homogeneus management of Elliptic Curves,
      which is why we consider points on those curves, even if the Y coordinate is ignored
      when used and -thus- always set to 0.


                                --oO0Oo--
*/

#ifndef ELLIPTIC_CURVES
#define ELLIPTIC_CURVES

#include "curve25519.h"
#include "curve448.h"
#include "mpint.h"
#include "random.h"

#define ECC_MIN_BITS    192                         /* Min size in bits of a field (secp192k1) */
#define ECC_MAX_BITS    521                         /* Max size in bits of a field (secp521r1) */
#define ECC_MAX_BYTES   ((ECC_MAX_BITS +  7) / 8)   /* Max size in bytes of a field */

#define ECC_MAX_WORDS   ((ECC_MAX_BITS + MP_DIGIT_LOG) / MP_DIGIT_BITS)

#define ECC_MSB         0x80000000
#define ECC_FULL        0xffffffff

/* ECC Point */

typedef struct
{
    mp_int_t x;
    mp_int_t y;
} ecc_point_t;

/** ECC Curve  */

typedef struct
{
    int     curve;       /* What curve is this (see enum below ) */

    const char **alias;  /* Curve aliases list */

    int     NUMBITS;     /* Bits of the curve */
    int     NUMBYTES;    /* Size in bytes of the curve */
    int     NUMWORDS;    /* Size in words of the fields (for GF2m arithmetic) */

    mp_int_t     p;      /* Polynomial P */
    mp_int_t     a;      /* Element A */
    mp_int_t     b;      /* Element B */
    mp_int_t     n;      /* Order N */

    ecc_point_t G;      /* Base point (Generator)*/

    int         h;      /* Cofactor H */

    /* Stuff for  Binary Arithmetics */

    mp_digit_t  UPRBIT;
    mp_digit_t  UPRMASK;

    int     FULLWORDS;
    int     LOG2;

} ecc_curve_t;

/* -------------------------- * 
      Supported Curve List 
 * -------------------------- */

enum
{
    ECC_CURVE_NONE = 0,

    ECC_CURVE_192k1,        /* Koblitz secp192k1 / ansip192k1 */
    ECC_CURVE_192r1,        /* Random secp192r1 / prime192v1 / nistp192 */

    ECC_CURVE_256r1,        /* Random  secp256r1 / prime256v1 / nistp256 */
    ECC_CURVE_256k1,        /* Koblitz  secp256k1 / ansip256k1 */

    ECC_CURVE_384r1,        /* Random secp384r1 / prime384v1 / nistp384 */
    ECC_CURVE_521r1,        /* Koblitz secp521r1 / prime521v1 / nistp521 */

    ECC_CURVE_BRAIN_192,    /* Brainpool 192r1 */
    ECC_CURVE_BRAIN_224,    /* Brainpool 224r1 */
    ECC_CURVE_BRAIN_256,    /* Brainpool 256r1 */
    ECC_CURVE_BRAIN_320,    /* Brainpool 320r1 */
    ECC_CURVE_BRAIN_384,    /* Brainpool 384r1 */
    ECC_CURVE_BRAIN_512,    /* Brainpool 512r1 */

    ECC_CURVE_X25519,       /* Edwards curve25519 */
    ECC_CURVE_X448,         /* Edwards curve448 */

    ECC_NUM_CURVES          /* Always last */
};

/* Macros */

#define ecc_curve_bits(_c)   ((_c)->NUMBITS)
#define ecc_curve_bytes(_c)  ((_c)->NUMBYTES)
#define ecc_curve_name(_c)   ((_c)->alias[0])
#define ecc_curve_id(_c)     ((_c)->curve)

#define is_curve25519(_c)    ((_c)->curve == ECC_CURVE_X25519)
#define is_curve448(_c)      ((_c)->curve == ECC_CURVE_X448)

/* -------------------------- * 
          Functions 
 * -------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Returns an Elliptic Curve with the given curve type:

    Eg: curve = ecc_get_curve(ECC_CURVE_224r1);

    The curve struct is static and doesn't need 
    to be freed
* -------------------------------------------------- */

ecc_curve_t *ecc_get_curve(int curve);

/* -------------------------------------------------- *
   Returns an Elliptic Curve with the given curve 
   name:

    Eg: curve = ecc_get_named_curve("prime256v1");

   The curve is static and doesn't need to be freed
 * -------------------------------------------------- */

ecc_curve_t *ecc_get_named_curve(const char *curve);

/* -------------------------------------------------- *
   Returns an Elliptic Curve with the given curve OID:

    Eg:
        curve = ecc_get_curve_from_oid("1.3.132.0.10");

    is equivalent to:

        curve = ecc_get_curve(ECC_CURVE_256k1);

    and:

        curve = ecc_get_named_curve("secp256k1");

    The curve is static and doesn't need to be freed
* -------------------------------------------------- */

ecc_curve_t *ecc_get_curve_from_oid(const char *curve);

/* -------------------------------------------------- *
   Returns an Elliptic Curve with the given curve OID
   in asn1 format

    The curve is static and doesn't need to be freed
 * -------------------------------------------------- */

ecc_curve_t *ecc_get_curve_from_asn1(const void *asn,int len);

/* -------------------------------------------------- *
   copy to 'buf' the OID of a curve encoded in ASN.1
   and return its length or 0 if not found
 * -------------------------------------------------- */

int ecc_get_curve_asn1(int curve,void *buf,int max);

/* -------------------------------------------------- *
    Returns the OID of a curve or NULL if not found
 * -------------------------------------------------- */

const char *ecc_get_curve_oid(int curve);
const char *ecc_get_curve_name(int curve);

/* -------------------------------------------------- *
   Returns the size of a public key in bytes after
   conversion.

   Size includes the type/prefix byte for all curves except
   for x25519 and x448.
 * -------------------------------------------------- */

int ecc_curve_pub_size(ecc_curve_t *ctx,int compressed);

/* -------------------------------------------------- *
   Convert points to bytes.

   For curves not x25519 nor x448, adds a type/prefix
   byte with the folling meaning:

        0x04 = Uncompressed (both X and Y)
        0x02 = Compressed (only X) where Y is even
        0x03 = Compressed (only X) where Y is odd

   compress = TRUE to get a compressed curve.

   Note that 'compress' is ignored for curves x25519,
   x448 and for binary curves, which are always
   uncompressed.
 * -------------------------------------------------- */

int ecc_point_to_bytes(ecc_curve_t *ctx,ecc_point_t *p,void *dest,int max,int compress);

/* -------------------------------------------------- *
   Convert bytes to a point. It expects a type/prefix
   byte (0x02, 0x03 or 0x04) at the start for
   curves other than x25519 or x448, and it will
   return S9_EINCORRECT if not there.

   Returns error or S9_OK (0) if done
 * -------------------------------------------------- */

int ecc_point_from_bytes(ecc_curve_t *ctx,ecc_point_t *p,const void *source,int len);

/* -------------------------------------------------- *
   Make a field bn to be in the magnitude of N and
   and multiple of the cofactor
 * -------------------------------------------------- */

void ecc_make_order(ecc_curve_t *ctx,mp_int_t *bn);

/* -------------------------------------------------- *
   Generate random fields and points

   IMPORTANT!! bn and p should NOT be initialized
 * -------------------------------------------------- */

void ecc_random_field(ecc_curve_t *ctx,mp_int_t  *bn,rand_t *rc);
void ecc_random_point(ecc_curve_t *ctx,ecc_point_t *p,rand_t *rc);

/* -------------------------------------------------- *
   Show a point in the screen, 'decimal' is TRUE or
   FALSE show as decimal. FALSE show hexa
 * -------------------------------------------------- */

void ecc_point_show(char *prefix,ecc_point_t *p,int decimal);

/* -------------------------------------------------- *
   IMPORTANT!! p should NOT be initialized
 * -------------------------------------------------- */

void ecc_point_init(ecc_point_t *p);
void ecc_point_init_copy(ecc_point_t *p,ecc_point_t *q);
void ecc_point_init_string(ecc_point_t *p,const char *x,const char *y,int radix);

/* -------------------------------------------------- *
   The values of x,y are copied so they can be freed
   without affecting the point.
 * -------------------------------------------------- */

void ecc_point_init_set(ecc_point_t *p,mp_int_t *x,mp_int_t *y);

/* -------------------------------------------------- *
   Point clean up
 * -------------------------------------------------- */

void ecc_point_clear(ecc_point_t *p);

/* -------------------------------------------------- *
   IMPORTANT!! p should BE initialized as current
   values would be freed
 * -------------------------------------------------- */

void ecc_point_set_zero(ecc_point_t *p);
void ecc_point_copy(ecc_point_t *p,ecc_point_t *from);

/* -------------------------------------------------- *
   IMPORTANT!! This function is to set values to an
   existing point so p should BE initialized becasue
   the current x,y of p will be freed, and the alues
   of x,y would be copied so they are independent
   of the originals
   .
 * -------------------------------------------------- */

void ecc_point_set(ecc_point_t *p,mp_int_t *x,mp_int_t *y);

/* -------------------------------------------------- *
   Point arithmetic
 * -------------------------------------------------- */

int ecc_point_on_curve(ecc_curve_t *ctx,ecc_point_t *p);
int ecc_point_is_zero(ecc_point_t *p);
int ecc_point_is_equal(ecc_point_t *p1,ecc_point_t *p2);

/* -------------------------------------------------- *
   r(x,y) = p(x,-y) mod m. Note that r should be
   initialized
 * -------------------------------------------------- */

void ecc_point_inverse(ecc_point_t *r,ecc_point_t *p,mp_int_t *m);

/* -------------------------------------------------- *
   Point doubling, addition and multiplication

        r = r + r;
        r = r + p
        r = p * k
 * -------------------------------------------------- */

void ecc_point_double(ecc_curve_t *ctx,ecc_point_t *r);
void ecc_point_add(ecc_curve_t *ctx,ecc_point_t *r,ecc_point_t *p);
void ecc_point_mult(ecc_curve_t *ctx,ecc_point_t *r,ecc_point_t *p, mp_int_t *k);


#ifdef __cplusplus
};
#endif


#endif
