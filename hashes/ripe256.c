/*
    ripe256.c

    RIPEMD-256 Hashing functions

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
          public domsin implementation.

        - RIPEMD was designed by Hans Dobbertin, Antoon Bosselaers and Bart Preneel
          as an improvement to Ron Rivest's MD (MD4, MD5)
        
                                --oO0Oo--
*/

#include "ripe256.h"

/* Operations */

#define F(x, y, z)    (x ^ y ^ z) 
#define G(x, y, z)    (z ^ (x & (y^z)))
#define H(x, y, z)    (z ^ (x | ~y))
#define I(x, y, z)    (y ^ (z & (x^y)))
#define J(x, y, z)    (x ^ (y | ~z))

#define k0 0
#define k1 0x5a827999
#define k2 0x6ed9eba1
#define k3 0x8f1bbcdc
#define k4 0xa953fd4e
#define k5 0x50a28be6
#define k6 0x5c4dd124
#define k7 0x6d703ef3
#define k8 0x7a6d76e9
#define k9 0

/* Rounds */

#define R(_x1, _n1)     (((_x1) << (_n1)) | ((_x1) >> (32-(_n1))))

#define Subround(_f, _a, _b, _c, _d, _x, _s, _k)  { \
	_a += _f(_b, _c, _d) + _x + _k;                 \
	_a = R(_a, _s);                                 \
}

/* RIPE256 */

static void ripe256_function(ripe256_t *ctx)
{
    unsigned int a1,a2,b1,b2,c1,c2,d1,d2,X[16];
    unsigned char *ptr;
    int t;

    /* pack buffer in little endian */

    ptr=ctx->datos;
    for(t=0;t<16;t++)
    {
        X[t]=get_le32(ptr);
        ptr+=4;
    }

    /* read status */

    a1=ctx->estado[0];
    b1=ctx->estado[1];
    c1=ctx->estado[2];
    d1=ctx->estado[3];
	a2=ctx->estado[4];
    b2=ctx->estado[5];
    c2=ctx->estado[6];
    d2=ctx->estado[7];

    /* Round 1 */

    Subround(F, a1, b1, c1, d1, X[ 0], 11, k0);
	Subround(F, d1, a1, b1, c1, X[ 1], 14, k0);
	Subround(F, c1, d1, a1, b1, X[ 2], 15, k0);
	Subround(F, b1, c1, d1, a1, X[ 3], 12, k0);
	Subround(F, a1, b1, c1, d1, X[ 4],  5, k0);
	Subround(F, d1, a1, b1, c1, X[ 5],  8, k0);
	Subround(F, c1, d1, a1, b1, X[ 6],  7, k0);
	Subround(F, b1, c1, d1, a1, X[ 7],  9, k0);
	Subround(F, a1, b1, c1, d1, X[ 8], 11, k0);
	Subround(F, d1, a1, b1, c1, X[ 9], 13, k0);
	Subround(F, c1, d1, a1, b1, X[10], 14, k0);
	Subround(F, b1, c1, d1, a1, X[11], 15, k0);
	Subround(F, a1, b1, c1, d1, X[12],  6, k0);
	Subround(F, d1, a1, b1, c1, X[13],  7, k0);
	Subround(F, c1, d1, a1, b1, X[14],  9, k0);
	Subround(F, b1, c1, d1, a1, X[15],  8, k0);

    /* Round 2 */

    Subround(I, a2, b2, c2, d2, X[ 5],  8, k5);
	Subround(I, d2, a2, b2, c2, X[14],  9, k5);
	Subround(I, c2, d2, a2, b2, X[ 7],  9, k5);
	Subround(I, b2, c2, d2, a2, X[ 0], 11, k5);
	Subround(I, a2, b2, c2, d2, X[ 9], 13, k5);
	Subround(I, d2, a2, b2, c2, X[ 2], 15, k5);
	Subround(I, c2, d2, a2, b2, X[11], 15, k5);
	Subround(I, b2, c2, d2, a2, X[ 4],  5, k5);
	Subround(I, a2, b2, c2, d2, X[13],  7, k5);
	Subround(I, d2, a2, b2, c2, X[ 6],  7, k5);
	Subround(I, c2, d2, a2, b2, X[15],  8, k5);
	Subround(I, b2, c2, d2, a2, X[ 8], 11, k5);
	Subround(I, a2, b2, c2, d2, X[ 1], 14, k5);
	Subround(I, d2, a2, b2, c2, X[10], 14, k5);
	Subround(I, c2, d2, a2, b2, X[ 3], 12, k5);
	Subround(I, b2, c2, d2, a2, X[12],  6, k5);

	t = a1; a1 = a2; a2 = t;

    /* Round 3 */

    Subround(G, a1, b1, c1, d1, X[ 7],  7, k1);
	Subround(G, d1, a1, b1, c1, X[ 4],  6, k1);
	Subround(G, c1, d1, a1, b1, X[13],  8, k1);
	Subround(G, b1, c1, d1, a1, X[ 1], 13, k1);
	Subround(G, a1, b1, c1, d1, X[10], 11, k1);
	Subround(G, d1, a1, b1, c1, X[ 6],  9, k1);
	Subround(G, c1, d1, a1, b1, X[15],  7, k1);
	Subround(G, b1, c1, d1, a1, X[ 3], 15, k1);
	Subround(G, a1, b1, c1, d1, X[12],  7, k1);
	Subround(G, d1, a1, b1, c1, X[ 0], 12, k1);
	Subround(G, c1, d1, a1, b1, X[ 9], 15, k1);
	Subround(G, b1, c1, d1, a1, X[ 5],  9, k1);
	Subround(G, a1, b1, c1, d1, X[ 2], 11, k1);
	Subround(G, d1, a1, b1, c1, X[14],  7, k1);
	Subround(G, c1, d1, a1, b1, X[11], 13, k1);
	Subround(G, b1, c1, d1, a1, X[ 8], 12, k1);

    /* Round 4 */

    Subround(H, a2, b2, c2, d2, X[ 6],  9, k6);
	Subround(H, d2, a2, b2, c2, X[11], 13, k6);
	Subround(H, c2, d2, a2, b2, X[ 3], 15, k6);
	Subround(H, b2, c2, d2, a2, X[ 7],  7, k6);
	Subround(H, a2, b2, c2, d2, X[ 0], 12, k6);
	Subround(H, d2, a2, b2, c2, X[13],  8, k6);
	Subround(H, c2, d2, a2, b2, X[ 5],  9, k6);
	Subround(H, b2, c2, d2, a2, X[10], 11, k6);
	Subround(H, a2, b2, c2, d2, X[14],  7, k6);
	Subround(H, d2, a2, b2, c2, X[15],  7, k6);
	Subround(H, c2, d2, a2, b2, X[ 8], 12, k6);
	Subround(H, b2, c2, d2, a2, X[12],  7, k6);
	Subround(H, a2, b2, c2, d2, X[ 4],  6, k6);
	Subround(H, d2, a2, b2, c2, X[ 9], 15, k6);
	Subround(H, c2, d2, a2, b2, X[ 1], 13, k6);
	Subround(H, b2, c2, d2, a2, X[ 2], 11, k6);

	t = b1; b1 = b2; b2 = t;

    /* Parallel Round 1 */

	Subround(H, a1, b1, c1, d1, X[ 3], 11, k2);
	Subround(H, d1, a1, b1, c1, X[10], 13, k2);
	Subround(H, c1, d1, a1, b1, X[14],  6, k2);
	Subround(H, b1, c1, d1, a1, X[ 4],  7, k2);
	Subround(H, a1, b1, c1, d1, X[ 9], 14, k2);
	Subround(H, d1, a1, b1, c1, X[15],  9, k2);
	Subround(H, c1, d1, a1, b1, X[ 8], 13, k2);
	Subround(H, b1, c1, d1, a1, X[ 1], 15, k2);
	Subround(H, a1, b1, c1, d1, X[ 2], 14, k2);
	Subround(H, d1, a1, b1, c1, X[ 7],  8, k2);
	Subround(H, c1, d1, a1, b1, X[ 0], 13, k2);
	Subround(H, b1, c1, d1, a1, X[ 6],  6, k2);
	Subround(H, a1, b1, c1, d1, X[13],  5, k2);
	Subround(H, d1, a1, b1, c1, X[11], 12, k2);
	Subround(H, c1, d1, a1, b1, X[ 5],  7, k2);
	Subround(H, b1, c1, d1, a1, X[12],  5, k2);    

    /* Parallel Round 2 */

	Subround(G, a2, b2, c2, d2, X[15],  9, k7);
	Subround(G, d2, a2, b2, c2, X[ 5],  7, k7);
	Subround(G, c2, d2, a2, b2, X[ 1], 15, k7);
	Subround(G, b2, c2, d2, a2, X[ 3], 11, k7);
	Subround(G, a2, b2, c2, d2, X[ 7],  8, k7);
	Subround(G, d2, a2, b2, c2, X[14],  6, k7);
	Subround(G, c2, d2, a2, b2, X[ 6],  6, k7);
	Subround(G, b2, c2, d2, a2, X[ 9], 14, k7);
	Subround(G, a2, b2, c2, d2, X[11], 12, k7);
	Subround(G, d2, a2, b2, c2, X[ 8], 13, k7);
	Subround(G, c2, d2, a2, b2, X[12],  5, k7);
	Subround(G, b2, c2, d2, a2, X[ 2], 14, k7);
	Subround(G, a2, b2, c2, d2, X[10], 13, k7);
	Subround(G, d2, a2, b2, c2, X[ 0], 13, k7);
	Subround(G, c2, d2, a2, b2, X[ 4],  7, k7);
	Subround(G, b2, c2, d2, a2, X[13],  5, k7);

	t = c1; c1 = c2; c2 = t;
    
    /* Parallel Round 3 */

    Subround(I, a1, b1, c1, d1, X[ 1], 11, k3);
	Subround(I, d1, a1, b1, c1, X[ 9], 12, k3);
	Subround(I, c1, d1, a1, b1, X[11], 14, k3);
	Subround(I, b1, c1, d1, a1, X[10], 15, k3);
	Subround(I, a1, b1, c1, d1, X[ 0], 14, k3);
	Subround(I, d1, a1, b1, c1, X[ 8], 15, k3);
	Subround(I, c1, d1, a1, b1, X[12],  9, k3);
	Subround(I, b1, c1, d1, a1, X[ 4],  8, k3);
	Subround(I, a1, b1, c1, d1, X[13],  9, k3);
	Subround(I, d1, a1, b1, c1, X[ 3], 14, k3);
	Subround(I, c1, d1, a1, b1, X[ 7],  5, k3);
	Subround(I, b1, c1, d1, a1, X[15],  6, k3);
	Subround(I, a1, b1, c1, d1, X[14],  8, k3);
	Subround(I, d1, a1, b1, c1, X[ 5],  6, k3);
	Subround(I, c1, d1, a1, b1, X[ 6],  5, k3);
	Subround(I, b1, c1, d1, a1, X[ 2], 12, k3);

    /* Parallel Round 4 */

    Subround(F, a2, b2, c2, d2, X[ 8], 15, k9);
	Subround(F, d2, a2, b2, c2, X[ 6],  5, k9);
	Subround(F, c2, d2, a2, b2, X[ 4],  8, k9);
	Subround(F, b2, c2, d2, a2, X[ 1], 11, k9);
	Subround(F, a2, b2, c2, d2, X[ 3], 14, k9);
	Subround(F, d2, a2, b2, c2, X[11], 14, k9);
	Subround(F, c2, d2, a2, b2, X[15],  6, k9);
	Subround(F, b2, c2, d2, a2, X[ 0], 14, k9);
	Subround(F, a2, b2, c2, d2, X[ 5],  6, k9);
	Subround(F, d2, a2, b2, c2, X[12],  9, k9);
	Subround(F, c2, d2, a2, b2, X[ 2], 12, k9);
	Subround(F, b2, c2, d2, a2, X[13],  9, k9);
	Subround(F, a2, b2, c2, d2, X[ 9], 12, k9);
	Subround(F, d2, a2, b2, c2, X[ 7],  5, k9);
	Subround(F, c2, d2, a2, b2, X[10], 15, k9);
	Subround(F, b2, c2, d2, a2, X[14],  8, k9);

    /* update status */

    t = d1; d1 = d2; d2 = t;

	ctx->estado[0] += a1;
	ctx->estado[1] += b1;
	ctx->estado[2] += c1;
	ctx->estado[3] += d1;
	ctx->estado[4] += a2;
	ctx->estado[5] += b2;
	ctx->estado[6] += c2;
	ctx->estado[7] += d2;
}


/* ------------------------------- */

void ripe256_init(ripe256_t *ctx)
{
    memset(ctx,0,sizeof(ripe256_t));

	ctx->estado[0] = 0x67452301;
	ctx->estado[1] = 0xefcdab89;
	ctx->estado[2] = 0x98badcfe;
	ctx->estado[3] = 0x10325476;
	ctx->estado[4] = 0x76543210;
	ctx->estado[5] = 0xfedcba98;
	ctx->estado[6] = 0x89abcdef;
	ctx->estado[7] = 0x01234567;
}

/* ------------------------------- */

void ripe256_update(ripe256_t *ctx,const void *buf, size_t tam)
{
	unsigned char *datos = (unsigned char *)buf;
    size_t t;

    for(t=0;t<tam;t++)
    {
        ctx->datos[ctx->pos++]=datos[t];
        if(ctx->pos==64)
        {
            ripe256_function(ctx);
            ctx->pos=0;
        }

        ctx->cont+=8;
    }
}

/* ------------------------------- */

void ripe256_final(ripe256_t *ctx,void *h)
{
    int quedan;
    unsigned char *hash=(unsigned char *)h;

    ctx->datos[ctx->pos++]=0x80;

    quedan = 64 - ctx->pos;
    if(quedan < 8)
    {
        memset(ctx->datos + ctx->pos,0,quedan);
        ripe256_function(ctx);
        ctx->pos=0;
        quedan = 64;
    }

    memset(ctx->datos + ctx->pos,0,quedan - 8);

    put_le64(ctx->datos + 56,ctx->cont);

    ripe256_function(ctx);

    put_le32(hash,ctx->estado[0]);
    put_le32(hash + 4,ctx->estado[1]);
    put_le32(hash + 8,ctx->estado[2]);
    put_le32(hash + 12,ctx->estado[3]);
	put_le32(hash + 16,ctx->estado[4]);
    put_le32(hash + 20,ctx->estado[5]);
	put_le32(hash + 24,ctx->estado[6]);
    put_le32(hash + 28,ctx->estado[7]);	

	memset(ctx, 0,sizeof(ripe256_t));
}

/* ------------------------------- */

void ripe256_partial(ripe256_t *ctx,void *hash)
{
    ripe256_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    ripe256_final(&tmp,hash);
}







