/*
    curve448.c

    Curve 448 implementation

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

      -  Basic functions adapted by Guillermo Amodeo Ojeda from public domain
         files in Daniel Beer's implementation of Curve25519 at:

         https://www.dlbeer.co.nz/oss/c25519.html

         And the info from:

            * RFC 7748 at https://tools.ietf.org/html/rfc7748

                                --oO0Oo--
*/

#include "curve448.h"

#define C448_COUNT      CURVE_448_POINT_BYTES
#define C448_DOUBLE     (CURVE_448_POINT_BYTES * 2)
#define C448_HALF       (CURVE_448_POINT_BYTES / 2)
#define C448_LAST       (CURVE_448_POINT_BYTES - 1)
#define C448_BITS       (CURVE_448_POINT_BYTES * 8)
#define C448_HALF_BITS  (CURVE_448_POINT_BYTES * 4)

/* ------------------------------- */

#define curve448_set(_d,_s)    do { memcpy(_d,_s,C448_COUNT);} while(0)
#define curve448_set_0(_d)     do { memset(_d,0,C448_COUNT); } while(0)
#define curve448_set_1(_d)     do { memset(_d,0,C448_COUNT); _d[0]=1;} while(0)

/* ------------------------------- */

static void curve448_squeeze(unsigned char *value,int top)
{
    int i,carry;

    for (carry = top, i = 0; i < C448_COUNT; i++)
    {
        if (i == C448_HALF)
            carry += top;
        carry += value[i];
        value[i] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }
}

/* ------------------------------- */

static void curve448_add(unsigned char *result,const unsigned char *a, const unsigned char *b)
{
    int i,carry;

    for (carry = i = 0; i < C448_COUNT; i++)
    {
        carry += a[i];
        carry += b[i];
        result[i] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }

    curve448_squeeze(result,carry);
}

/* ------------------------------- */

static void curve448_sub(unsigned char *result, const unsigned char *a, const unsigned char *b)
{
    int i,carry;

    for (carry = i = 0; i < C448_COUNT; i++)
    {
        carry += (i == C448_HALF) ?  0x1fc : 0x1fe;
        carry += a[i];
        carry -= b[i];
        result[i] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }

    curve448_squeeze(result,carry);
}

/* ------------------------------- */

static void curve448_mula24(unsigned char *result, const unsigned char *value)
{
    int i,carry;

    for (carry = i = 0; i < C448_COUNT; i++)
    {
        carry += (int)value[i] * 39081;
        result[i] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }

    curve448_squeeze(result,carry);
}

/* ------------------------------- */

static void curve448_mul(unsigned char *result, const unsigned char* a, const unsigned char* b)
{
    int i,k,carry,first,last;
    unsigned char tmp[C448_DOUBLE];

    for (carry = k = 0; k < C448_DOUBLE - 1; k++)
    {
        first = ( k < C448_COUNT) ? 0 : k - C448_LAST;
        last  = (first) ? C448_LAST : k;

        for (i = first; i <= last; i++)
            carry += (int)a[i] * b[k - i];

        tmp[k] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }

    tmp[k] = (unsigned char)(carry & 0xff);

    for (carry = i = 0, k = C448_COUNT; i < C448_COUNT; i++, k++)
    {
        carry += tmp[i];
        carry += tmp[k];

        if (i < C448_HALF)
        {
            carry += tmp[k + C448_HALF];
        }
        else
        {
            carry += tmp[i + C448_HALF];
            carry += tmp[k];
        }

        result[i] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }

    curve448_squeeze(result,carry);
}

/* --------------------------------- *
 * Guille's 2 in 1 conditional swap
 * for x2,x3 and z2,z3 in one loop
 * --------------------------------- */

static void curve448_cswap(unsigned char *x2, unsigned char *x3, unsigned char *z2, unsigned char *z3, int swap)
{
    int i;
    unsigned char mask,tmp;

    mask = -(unsigned char)swap;

    for (i = 0; i < C448_COUNT; i++)
    {
        tmp = (x2[i] ^ x3[i]) & mask;

        x2[i] ^= tmp;
        x3[i] ^= tmp;

        tmp = (z2[i] ^ z3[i]) & mask;

        z2[i] ^= tmp;
        z3[i] ^= tmp;
    }
}

/* ------------------------------- *
   Mostly from Python example code
   in section 5 of RFC-7748
 * ------------------------------- */

void curve448_scalarmult(mp_int_t *point,mp_int_t *base,mp_int_t *resp)
{
    int i,swap,bit;

    unsigned char x1[C448_COUNT], x2[C448_COUNT], x3[C448_COUNT], z2[C448_COUNT],
                  z3[C448_COUNT],  a[C448_COUNT],  b[C448_COUNT],  c[C448_COUNT],
                   d[C448_COUNT],  e[C448_COUNT], aa[C448_COUNT], bb[C448_COUNT],
                  da[C448_COUNT], cb[C448_COUNT],  k[C448_COUNT];

    mp_copy_exact_bytes(point,k,C448_COUNT);

    k[0]  &= 252;
    k[C448_LAST] |= 128;

    mp_copy_exact_bytes(base,x1,C448_COUNT);

    curve448_set_1(x2);
    curve448_set(x3, x1);
    curve448_set_0(z2);
    curve448_set_1(z3);

    swap = 0;
    for (i = C448_BITS - 1; i >= 0; --i)
    {
        bit = (k[i >> 3] >> (i & 7)) & 1;

        /*
            (x_2, x_3) = cswap(swap, x_2, x_3)
            (z_2, z_3) = cswap(swap, z_2, z_3)
        */

        curve448_cswap(x2, x3, z2, z3,swap ^ bit);

        swap = bit;

        curve448_add(a, x2, z2);    /* A = x_2 + z_2 */
        curve448_mul(aa, a, a);     /* AA = A^2 */
        curve448_sub(b, x2, z2);    /* B = x_2 - z_2 */
        curve448_mul(bb, b, b);     /* BB = B^2 */
        curve448_sub(e, aa, bb);    /* E = AA - BB */
        curve448_add(c, x3, z3);    /* C = x_3 + z_3 */
        curve448_sub(d, x3, z3);    /* D = x_3 - z_3*/
        curve448_mul(da, d, a);     /* DA = D * A */
        curve448_mul(cb, c, b);     /* CB = C * B */

        /* x_3 = (DA + CB)^2 */

        curve448_add(x3, da, cb);
        curve448_mul(x3, x3, x3);

        /* z_3 = x_1 * (DA - CB)^2 */

        curve448_sub(z3, da, cb);
        curve448_mul(z3, z3, z3);
        curve448_mul(z3, x1, z3);

        /* x_2 = AA * BB */

        curve448_mul(x2, aa, bb);

        /* z_2 = E * (AA + a24 * E) */

        curve448_mula24(z2, e);
        curve448_add(z2, aa, z2);
        curve448_mul(z2, z2, e);
    }

    /*
        (x_2, x_3) = cswap(swap, x_2, x_3)
        (z_2, z_3) = cswap(swap, z_2, z_3)
    */

    curve448_cswap(x2, x3, z2, z3,swap);

    /* Return  x2 * (z2 ^ (p - 2)) */

    curve448_mul(b, z2, z2);
    curve448_mul(b, b, z2);

    for (i = 0; i < C448_BITS - 4; i++)
    {
        if(i == C448_HALF_BITS - 3 || i == C448_BITS - 5)
            curve448_mul(b, b, b);

        curve448_mul(b, b, b);
        curve448_mul(b, b, z2);
    }

    curve448_mul(x2, x2, b);

    mp_set_bytes(resp,x2,C448_COUNT);
}


/* ---------------------------- */
