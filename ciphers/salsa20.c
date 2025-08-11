/*
    salsa20.c

    Salsa20 Stream Cypher

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

        - Written by Guillermo Amodeo Ojeda using the info and code from:

            * https://en.wikipedia.org/wiki/Salsa20
            * https://cr.yp.to/snuffle/salsa20/regs/salsa20.c
            * https://cr.yp.to/streamciphers/timings/estreambench/submissions/salsa20/chacha8/ref/chacha.c
            * RFC-7539 at https://tools.ietf.org/html/rfc7539

                                --oO0Oo--
*/


#include "salsa20.h"


#define ROTL(_a,_b)     (((_a) << (_b)) | ((_a) >> (32 - (_b))))

/* ---------------------------- */

/* Constants already converted to little endian

    sigma = "expand 32-byte k"
      tau = "expand 16-byte k"
*/

#define SIGMA_0     0x61707865   /* 'expa' */
#define SIGMA_1     0x3320646e   /* 'nd 3' */
#define SIGMA_2     0x79622d32   /* '2-by' */
#define SIGMA_4     0x6b206574   /* 'te k' */

#define TAU_1       0x3120646e   /* 'nd 1' */
#define TAU_2       0x79622d36   /* '6-by' */


/* ---------------------------- */


#define Salsa_QR(a, b, c, d)    \
    b ^= ROTL(a + d, 7);        \
    c ^= ROTL(b + a, 9);        \
    d ^= ROTL(c + b,13);        \
    a ^= ROTL(d + c,18);

/* ---------------------------- */

static void salsa20_function(unsigned char *out, unsigned int *in)
{
	int i;
	unsigned int x[16];

	for (i = 0; i < 16; ++i)
		x[i] = in[i];

	// 10 loops × 2 rounds/loop = 20 rounds

	for (i = 0; i < SALSA20_ROUNDS; i += 2)
	{
		// Odd round

		Salsa_QR(x[ 0], x[ 4], x[ 8], x[12]);	// column 1
		Salsa_QR(x[ 5], x[ 9], x[13], x[ 1]);	// column 2
		Salsa_QR(x[10], x[14], x[ 2], x[ 6]);	// column 3
		Salsa_QR(x[15], x[ 3], x[ 7], x[11]);	// column 4

		// Even round

		Salsa_QR(x[ 0], x[ 1], x[ 2], x[ 3]);	// row 1
		Salsa_QR(x[ 5], x[ 6], x[ 7], x[ 4]);	// row 2
		Salsa_QR(x[10], x[11], x[ 8], x[ 9]);	// row 3
		Salsa_QR(x[15], x[12], x[13], x[14]);	// row 4
	}

	for (i = 0; i < 16; ++i)
	{
		put_le32(out,x[i] + in[i]);
		out+=4;
	}
}

/* ---------------------------- */

int salsa20_start(salsa20_t *ctx,int bits,const void *key,const void *iv)
{
    if(ctx && key && (bits == 128 || bits == 256))
    {
        const unsigned char *k = (const unsigned char *)key;

        memset(ctx,0,sizeof(salsa20_t));

        ctx->input[ 0] = SIGMA_0;
        ctx->input[ 1] = get_le32(k);
        ctx->input[ 2] = get_le32(k + 4);
        ctx->input[ 3] = get_le32(k + 8);
        ctx->input[ 4] = get_le32(k + 12);

        if (bits == 256)
        {
            ctx->input[ 5] = SIGMA_1;
            ctx->input[10] = SIGMA_2;
            k += 16;
        }
        else /* kbits == 128 */
        {
            ctx->input[ 5] = TAU_1;
            ctx->input[10] = TAU_2;
        }

        if(iv)
        {
            const unsigned char *i = (const unsigned char *)iv;

            ctx->input[6] = get_le32(i);
            ctx->input[7] = get_le32(i + 4);
        }

        ctx->input[11] = get_le32(k + 0);
        ctx->input[12] = get_le32(k + 4);
        ctx->input[13] = get_le32(k + 8);
        ctx->input[14] = get_le32(k + 12);

        ctx->input[15] = SIGMA_4;

        return 0;
    }
    return -1;
}


/* ---------------------------- */

void salsa20_end(salsa20_t *ctx)
{
    if(ctx)
        memset(ctx,0,sizeof(salsa20_t));
}

/* ---------------------------- */

void salsa20_shuffle(salsa20_t *ctx,const void *iv)
{
    if(ctx && iv)
    {
        const unsigned char *i = (const unsigned char *)iv;

        ctx->input[6] = get_le32(i);
        ctx->input[7] = get_le32(i + 4);
        ctx->input[8] = 0;
        ctx->input[9] = 0;
    }
}

/* ---------------------------- */

void salsa20(salsa20_t *ctx,void *dest,const void *orig,size_t tam)
{
    unsigned char output[SALSA20_BLOCK_SIZE];
    size_t i,blk;
    unsigned char *d;
    const unsigned char *s;

    d = (unsigned char *)dest;
    s = (const unsigned char *)orig;


    while(tam > 0)
    {
        salsa20_function(output,ctx->input);

        ctx->input[8] = ctx->input[8] + 1;
        if (ctx->input[8] == 0)
            ctx->input[9] = ctx->input[9] + 1;

        blk = (tam < SALSA20_BLOCK_SIZE) ? tam : SALSA20_BLOCK_SIZE;

        for (i=0; i < blk; i++)
            d[i] = (s[i] ^ output[i]);

        tam -= blk;

        s += blk;
        d += blk;
    }
}

