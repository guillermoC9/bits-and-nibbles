/*
    curve25519.c

    Dj Bernstein's Curve 25519 implementation

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

      -  Most code adapted by Guillermo Amodeo Ojeda from public domain
         files in Daniel Beer's implementation of Curve25519 at:

         https://www.dlbeer.co.nz/oss/c25519.html

         And the info from:

            * RFC 7748 at https://tools.ietf.org/html/rfc7748

                                --oO0Oo--
*/

#include "curve25519.h"

#define C25519_COUNT      CURVE_25519_POINT_BYTES
#define C25519_DOUBLE     (CURVE_25519_POINT_BYTES * 2)
#define C25519_HALF       (CURVE_25519_POINT_BYTES / 2)
#define C25519_LAST       (CURVE_25519_POINT_BYTES - 1)
#define C25519_BITS       (CURVE_25519_POINT_BYTES * 8)
#define C25519_HALF_BITS  (CURVE_25519_POINT_BYTES * 4)

/* ------------------------------- */

#define curve25519_set(_d,_s)    do { memcpy(_d,_s,C25519_COUNT);} while(0)
#define curve25519_set_0(_d)     do { memset(_d,0,C25519_COUNT); } while(0)
#define curve25519_set_1(_d)     do { memset(_d,0,C25519_COUNT); _d[0]=1;} while(0)

/* ------------------------------- */

static void curve25519_squeeze(unsigned char *value,int top)
{
    int i,carry;

    value[C25519_LAST] &= 127;

    carry = (top >> 7) * 19;

    for (i = 0; i < C25519_COUNT; i++)
    {
        carry += value[i];
        value[i] = (unsigned char)(carry & 0xff);
        carry >>= 8;
    }
}

/* ------------------------------- */

void curve25519_add(unsigned char *result, const unsigned char *a, const unsigned char *b)
{
	int i,carry;

	/* Add */
	for (carry = i = 0; i < C25519_COUNT; i++)
	{
		carry >>= 8;
		carry += a[i];
        carry += b[i];
        result[i] = (unsigned char)(carry & 0xff);
	}

	/* Reduce with 2^255 = 19 mod p */

    curve25519_squeeze(result,carry);
}

/* ------------------------------- */

void curve25519_sub(unsigned char *result, const unsigned char *a, const unsigned char *b)
{
	int i,carry;

	/* Calculate a + 2p - b, to avoid underflow */

	carry = 218;
	for (i = 0; i < C25519_LAST; i++)
	{
		carry += 65280;
		carry += a[i];
        carry -= b[i];
        result[i] = (unsigned char)(carry & 0xff);
		carry >>= 8;
	}

    carry += a[i];
    carry -= b[i];
    result[i] = (unsigned char)(carry & 0x7f);

    curve25519_squeeze(result,carry);
}

/* ------------------------------- */

void curve25519_mul(unsigned char *result, const unsigned char *a, const unsigned char *b)
{
	int carry,j,i;
	unsigned char tmp[C25519_COUNT];

	for (carry = i = 0; i < C25519_COUNT; i++)
	{
		carry >>= 8;
		for (j = 0; j <= i; j++)
		{
			carry += ((int)a[j]) * ((int)b[i - j]);
		}

		for (; j < C25519_COUNT; j++)
		{
			carry += ((int)a[j]) * ((int)b[i + C25519_COUNT - j]) * 38;
		}
        tmp[i] = (unsigned char)(carry & 0xff);
	}

    curve25519_set(result,tmp);
    curve25519_squeeze(result,carry);
}

/* ------------------------------- */

void curve25519_mul_int(unsigned char *result, const unsigned char *a, int b)
{
	int i,carry;

	for (carry = i = 0; i < C25519_COUNT; i++)
	{
		carry >>= 8;
		carry += b * ((int)a[i]);
        result[i] = (unsigned char)(carry & 0xff);
	}

    curve25519_squeeze(result,carry);
}

/* --------------------------------- *
 * Guille's 2 in 1 conditional swap
 * for x2,x3 and z2,z3 in one loop
 * --------------------------------- */

static void curve25519_cswap(unsigned char *x2, unsigned char *x3, unsigned char *z2, unsigned char *z3, int swap)
{
    int i;
    unsigned char mask,tmp;

    mask = -(unsigned char)swap;

    for (i = 0; i < C25519_COUNT; i++)
    {
        tmp = (x2[i] ^ x3[i]) & mask;

        x2[i] ^= tmp;
        x3[i] ^= tmp;

        tmp = (z2[i] ^ z3[i]) & mask;

        z2[i] ^= tmp;
        z3[i] ^= tmp;
    }
}

/* --------------------------------- *
 * Differential addition
 * --------------------------------- */

static void xc_diffadd(unsigned char *x5, unsigned char *z5,const unsigned char *x1, const unsigned char *z1,
		       const unsigned char *x2, const unsigned char *z2,const unsigned char *x3, const unsigned char *z3)
{
	/* Explicit formulas database: dbl-1987-m3
	 *
	 * source 1987 Montgomery "Speeding the Pollard and elliptic curve
	 *   methods of factorization", page 261, fifth display, plus
	 *   common-subexpression elimination
	 * compute A = X2+Z2
	 * compute B = X2-Z2
	 * compute C = X3+Z3
	 * compute D = X3-Z3
	 * compute DA = D A
	 * compute CB = C B
	 * compute X5 = Z1(DA+CB)^2
	 * compute Z5 = X1(DA-CB)^2
	 */
	unsigned char da[C25519_COUNT];
	unsigned char cb[C25519_COUNT];
	unsigned char a[C25519_COUNT];
	unsigned char b[C25519_COUNT];

	curve25519_add(a, x2, z2);
	curve25519_sub(b, x3, z3); /* D */
	curve25519_mul(da, a, b);

	curve25519_sub(b, x2, z2);
	curve25519_add(a, x3, z3); /* C */
	curve25519_mul(cb, a, b);

	curve25519_add(a, da, cb);
	curve25519_mul(b, a, a);
	curve25519_mul(x5, z1, b);

	curve25519_sub(a, da, cb);
	curve25519_mul(b, a, a);
	curve25519_mul(z5, x1, b);
}

/* ------------------------------- */

void curve25519_scalarmult(mp_int_t *point,mp_int_t *base,mp_int_t *resp)
{
    int i,bit;

    unsigned char xm[C25519_COUNT] , xm1[C25519_COUNT], zm[C25519_COUNT] ,
                 zm1[C25519_COUNT], xms[C25519_COUNT], zms[C25519_COUNT],
                 one[C25519_COUNT], k[C25519_COUNT]  , b[C25519_COUNT],
                 tmp[C25519_COUNT], xz[C25519_COUNT];

    mp_copy_exact_bytes(point,k,C25519_COUNT);

    k[0]  &= 248;
    k[C25519_LAST] &= 127;
    k[C25519_LAST] |= 64;

    mp_copy_exact_bytes(base,b,C25519_COUNT);

    b[C25519_LAST] &= 127;

    curve25519_set(xm, b);
    curve25519_set_1(zm);
    curve25519_set_0(zm1);
    curve25519_set_1(xm1);
    curve25519_set_1(one);

    for (i = C25519_BITS - 3; i >= 0; i--)
    {
        bit = (k[i >> 3] >> (i & 7)) & 1;

        /* From P_m and P_(m-1), compute P_(2m) and P_(2m-1) */

        xc_diffadd(xm1, zm1, b, one, xm, zm, xm1, zm1);

        /* Doubling
         *
         * Explicit formulas database: dbl-1987-m
         *
         * source 1987 Montgomery "Speeding the Pollard and elliptic
         *   curve methods of factorization", page 261, fourth display
         * compute X3 = (X1^2-Z1^2)^2
         * compute Z3 = 4 X1 Z1 (X1^2 + a X1 Z1 + Z1^2)
         */

        curve25519_mul(xms, xm, xm);
        curve25519_mul(zms, zm, zm);
        curve25519_mul(xz, xm, zm);

        curve25519_sub(tmp, xms, zms);
        curve25519_mul(xm, tmp, tmp);

        curve25519_mul_int(tmp, xz, 486662);
        curve25519_add(tmp, xms, tmp);
        curve25519_add(tmp, zms, tmp);
        curve25519_mul(xms, xz, tmp);
        curve25519_mul_int(zm, xms, 4);

        /* Compute P_(2m+1) */

		xc_diffadd(xms, zms, xm1, zm1, xm, zm, b, one);

		/* Select:
		 *   bit = 1 --> (P_(2m+1), P_(2m))
		 *   bit = 0 --> (P_(2m), P_(2m-1))
		 */

		curve25519_cswap(xm1,xm,zm1,zm,bit);
		curve25519_cswap(xm,xms,zm,zms,bit);

    }

    /* Return  xm * (zm ^ (p - 2)) */

	curve25519_mul(b, zm, zm);
	curve25519_mul(zm1, b, zm);

	for (i = 0; i < C25519_BITS - 5; i++)
	{
	    if(i == C25519_BITS - 8)
	    {
            curve25519_mul(b, zm1, zm1);
            curve25519_mul(zm1, b, b);
            curve25519_mul(b,zm1,zm);
            curve25519_mul(zm1, b, b);
	    }
	    else
	    {
            curve25519_mul(b, zm1, zm1);
    	    curve25519_mul(zm1, b, zm);
    	}
	}

    curve25519_mul(b, zm1, xm);

    mp_set_bytes(resp,b,C25519_COUNT);
}
