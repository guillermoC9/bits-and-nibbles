/*
    chacha.c

    Chacha Stream Cyphers

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

#include "chacha.h"

#define ROTL(_a, _b) (((_a) << (_b)) | ((_a) >> (32 - (_b))))

/* ---------------------------- */

/* Constants already converted to little endian

    sigma = "expand 32-byte k"
      tau = "expand 16-byte k"
*/

#define SIGMA_0     0x61707865     /* 'expa' */
#define SIGMA_1     0x3320646e     /* 'nd 3' */
#define SIGMA_2     0x79622d32     /* '2-by' */
#define SIGMA_4     0x6b206574     /* 'te k' */

#define TAU_1       0x3120646e     /* 'nd 1' */
#define TAU_2       0x79622d36     /* '6-by' */

/* ---------------------------- */

#define Chacha_QR(a, b, c, d) \
    a += b;                   \
    d ^= a;                   \
    d = ROTL(d, 16);          \
    c += d;                   \
    b ^= c;                   \
    b = ROTL(b, 12);          \
    a += b;                   \
    d ^= a;                   \
    d = ROTL(d, 8);           \
    c += d;                   \
    b ^= c;                   \
    b = ROTL(b, 7);

/* ---------------------------- */

static void chacha_function(int rounds, unsigned char *out, unsigned int *in)
{
    int i;
    unsigned int x[16];

    for (i = 0; i < 16; ++i)
        x[i] = in[i];

    // 10 loops × 2 rounds/loop = 20 rounds

    for (i = 0; i < rounds; i += 2)
    {
        // Odd round

        Chacha_QR(x[0], x[4], x[8], x[12]);  // column 0
        Chacha_QR(x[1], x[5], x[9], x[13]);  // column 1
        Chacha_QR(x[2], x[6], x[10], x[14]); // column 2
        Chacha_QR(x[3], x[7], x[11], x[15]); // column 3

        // Even round

        Chacha_QR(x[0], x[5], x[10], x[15]); // diagonal 1 (main diagonal)
        Chacha_QR(x[1], x[6], x[11], x[12]); // diagonal 2
        Chacha_QR(x[2], x[7], x[8], x[13]);  // diagonal 3
        Chacha_QR(x[3], x[4], x[9], x[14]);  // diagonal 4
    }

    for (i = 0; i < 16; ++i)
    {
        put_le32(out, x[i] + in[i]);
        out += 4;
    }
}

/* ---------------------------- */

int chacha_start(chacha_t *ctx, int bits, const void *key, int rounds, const void *iv)
{
    if (ctx && key && (bits == 128 || bits == 256) && (rounds == 8 || rounds == 12 || rounds == 20))
    {
        const unsigned char *k = (const unsigned char *)key;

        memset(ctx, 0, sizeof(chacha_t));

        ctx->rounds = rounds;

        ctx->input[0] = SIGMA_0;
        ctx->input[3] = SIGMA_4;

        ctx->input[4] = get_le32(k);
        ctx->input[5] = get_le32(k + 4);
        ctx->input[6] = get_le32(k + 8);
        ctx->input[7] = get_le32(k + 12);

        if (bits == 256)
        {
            ctx->kbits = 256;

            ctx->input[1] = SIGMA_1;
            ctx->input[2] = SIGMA_2;
            k += 16;
        }
        else /* kbits == 128 */
        {
            ctx->kbits = 128;

            ctx->input[1] = TAU_1;
            ctx->input[2] = TAU_2;            
        }

        ctx->input[8] = get_le32(k + 0);
        ctx->input[9] = get_le32(k + 4);
        ctx->input[10] = get_le32(k + 8);
        ctx->input[11] = get_le32(k + 12);

        if (iv)
        {
            const unsigned char *i = (const unsigned char *)iv;

            ctx->input[14] = get_le32(i);
            ctx->input[15] = get_le32(i + 4);
        }
        return 0;
    }
    return -1;
}

/* ---------------------------- */

void chacha_shuffle(chacha_t *ctx, const void *iv)
{
    if (ctx && iv)
    {
        const unsigned char *i = (const unsigned char *)iv;        
            
        ctx->input[12] = 0;
        ctx->input[13] = 0;
        ctx->input[14] = get_le32(i);
        ctx->input[15] = get_le32(i + 4);
    }
}

/* ---------------------------- */

void chacha(chacha_t *ctx, void *dest, const void *orig, size_t tam)
{
    unsigned char output[CHACHA_BLOCK_SIZE];
    size_t i, blk;
    unsigned char *d;
    const unsigned char *s;

    d = (unsigned char *)dest;
    s = (const unsigned char *)orig;

    while (tam > 0)
    {
        chacha_function(ctx->rounds, output, ctx->input);

        ctx->input[12] = ctx->input[12] + 1;
        if (ctx->input[12] == 0)
            ctx->input[13] = ctx->input[13] + 1;

        blk = (tam < CHACHA_BLOCK_SIZE) ? tam : CHACHA_BLOCK_SIZE;

        for (i = 0; i < blk; i++)
            d[i] = (s[i] ^ output[i]);

        tam -= blk;

        s += blk;
        d += blk;
    }
}

/* ---------------------------- */
/* ---------------------------- */

int chacha20_start(chacha_t *ctx, const void *key, const void *iv, unsigned int cnt)
{
    if (ctx && key)
    {
        const unsigned char *k = (const unsigned char *)key;

        memset(ctx, 0, sizeof(chacha_t));

        ctx->rounds = CHACHA20_ROUNDS;
        ctx->kbits = 256;

        ctx->input[0] = SIGMA_0;
        ctx->input[1] = SIGMA_1;
        ctx->input[2] = SIGMA_2;
        ctx->input[3] = SIGMA_4;

        ctx->input[4] = get_le32(k);
        ctx->input[5] = get_le32(k + 4);
        ctx->input[6] = get_le32(k + 8);
        ctx->input[7] = get_le32(k + 12);

        ctx->input[8] = get_le32(k + 16);
        ctx->input[9] = get_le32(k + 20);
        ctx->input[10] = get_le32(k + 24);
        ctx->input[11] = get_le32(k + 28);

        ctx->input[12] = cnt;

        if (iv)
        {
            const unsigned char *i = (const unsigned char *)iv;

            ctx->input[13] = get_le32(i);
            ctx->input[14] = get_le32(i + 4);
            ctx->input[15] = get_le32(i + 8);
        }
        return 0;
    }
    return -1;
}

/* ---------------------------- */

void chacha20_shuffle(chacha_t *ctx, const void *iv, unsigned int cnt)
{
    if (ctx && iv)
    {
        const unsigned char *i = (const unsigned char *)iv;

        ctx->input[12] = cnt;
        ctx->input[13] = get_le32(i);
        ctx->input[14] = get_le32(i + 4);
        ctx->input[15] = get_le32(i + 8);
    }
}

/* ---------------------------- */

void chacha20(chacha_t *ctx, void *dest, const void *orig, size_t tam)
{
    unsigned char output[CHACHA_BLOCK_SIZE];
    size_t i, blk;
    unsigned char *d;
    const unsigned char *s;

    d = (unsigned char *)dest;
    s = (const unsigned char *)orig;

    while (tam > 0)
    {
        chacha_function(CHACHA20_ROUNDS, output, ctx->input);

        ctx->input[12] = ctx->input[12] + 1;

        blk = (tam < CHACHA_BLOCK_SIZE) ? tam : CHACHA_BLOCK_SIZE;

        for (i = 0; i < blk; i++)
            d[i] = (s[i] ^ output[i]);

        tam -= blk;

        s += blk;
        d += blk;
    }
}

