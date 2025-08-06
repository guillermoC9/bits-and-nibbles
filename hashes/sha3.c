/*
    sha3.c

    Hashing SHA-3: SHA3-224, SHA3-256, SHA3-384 y SHA3-512.

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

        - Adapted by Guillermo Amodeo Ojeda from code at:

          https://github.com/brainhub/SHA3IUF/blob/master/sha3.c

          written by Andrey Jivsov, who donated it to the
          public domain.

                                --oO0Oo--
*/

#include "sha3.h"


#define ROTL64(_x,_n)	(((_x) << (_n)) | ((_x) >> (64 - (_n))))

/* SHA3 (Keccak) constants for 24 rounds */

static u64_t keccakf_rndc[24] =
{
	0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808AULL, 0x8000000080008000ULL,
	0x000000000000808BULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL,
	0x000000000000008AULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000AULL,
	0x000000008000808BULL, 0x800000000000008BULL, 0x8000000000008089ULL, 0x8000000000008003ULL,
	0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800AULL, 0x800000008000000AULL,
	0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL,
};

static const unsigned keccakf_rotc[24] =
{
    1,  3,  6, 10, 15, 21, 28, 36, 45, 55,  2, 14,
   27, 41, 56,  8, 25, 43, 62, 18, 39, 61, 20, 44
};

static const unsigned keccakf_piln[24] =
{
    10,  7, 11, 17, 18, 3,  5, 16,  8, 21, 24, 4,
    15, 23, 19, 13, 12, 2, 20, 14, 22,  9,  6, 1
};

/* ------------------------------- */

static void SHA3_Transform(sha3_t *ctx)
{
	int i,j,round;
    u64_t t, bc[5];
    unsigned char *ptr;

    /* Load values from ctx->buf to ctx->hash */

    ptr = ctx->buf;

    for (i = 0; i < 25; i++)
    {
        for(j=7; j > -1; j--)
        {
            ctx->hash[i]<<=8;
            ctx->hash[i] |= ptr[j];
        }
        ptr += 8;
    }

    /* Process ctx->hash */

    for(round = 0; round < 24; round++)
    {
        /* Theta */

        for(i = 0; i < 5; i++)
        {
            bc[i] = ctx->hash[i] ^ ctx->hash[i + 5] ^ ctx->hash[i + 10] ^ ctx->hash[i + 15] ^ ctx->hash[i + 20];
        }


        for(i = 0; i < 5; i++)
        {
            t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
            for(j = 0; j < 25; j += 5)
            {
                ctx->hash[j + i] ^= t;
            }
        }

        /* Rho Pi */

        t = ctx->hash[1];

        for(i = 0; i < 24; i++)
        {
            j = keccakf_piln[i];
            bc[0] = ctx->hash[j];
            ctx->hash[j] = ROTL64(t, keccakf_rotc[i]);
            t = bc[0];
        }

        /* Chi */

        for(j = 0; j < 25; j += 5)
        {
            for(i = 0; i < 5; i++)
                bc[i] = ctx->hash[j + i];
            for(i = 0; i < 5; i++)
                ctx->hash[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
        }

        /* Iota */

        ctx->hash[0] ^= keccakf_rndc[round];
    }

    /* Save values from ctx->hash to ctx->buf */

    ptr = ctx->buf;

    for (i = 0; i < 25; i++)
    {
        for(j=0; j < 8; j++)
        {
            ptr[j] = (unsigned char)(ctx->hash[i] & 0xff);
            ctx->hash[i]>>=8;
        }
        ptr+=8;
    }

}

/* ------------------------------- */
/* ------------------------------- */

void sha3_224_init(sha3_t *ctx)
{
    sha3_init(ctx,224);
}

/* ------------------------------- */

void sha3_256_init(sha3_t *ctx)
{
    sha3_init(ctx,256);
}

/* ------------------------------- */

void sha3_384_init(sha3_t *ctx)
{
    sha3_init(ctx,384);
}

/* ------------------------------- */

void sha3_512_init(sha3_t *ctx)
{
    sha3_init(ctx,512);
}

/* ------------------------------- */

int sha3_init(sha3_t *ctx,int bits)
{
    memset(ctx, 0, sizeof(sha3_t));
    switch(bits)
    {
        case 224:
            ctx->tam_digest = SHA224_SIZE;
           	break;
        case 256:
        	ctx->tam_digest = SHA256_SIZE;
            break;
        case 384:
        	ctx->tam_digest = SHA384_SIZE;
            break;
        case 512:
        	ctx->tam_digest = SHA512_SIZE;
            break;
        default:
            return FALSE;
    }
	ctx->num = 200 - (2 * ctx->tam_digest);
    return TRUE;
}

/* ------------------------------- */

int keccak_init(sha3_t *ctx,int bits)
{
    if(!sha3_init(ctx,bits))
        return FALSE;
    ctx->pure_keccak = 1;
    return TRUE;
}

/* ------------------------------- */

void sha3_update(sha3_t *ctx,const void *buf, size_t tam)
{
    size_t i;
    const unsigned char *datos = (unsigned char *)buf;

    for(i = 0; i < tam; i++)
    {
        ctx->buf[ctx->idx++] ^= datos[i];
        if(ctx->idx == ctx->num)
        {
            SHA3_Transform(ctx);
            ctx->idx = 0;
        }
    }
}

/* ------------------------------- */

void sha3_final(sha3_t *ctx,void *h)
{
    if(ctx->pure_keccak)
        ctx->buf[ctx->idx] ^=  0x01;
    else
        ctx->buf[ctx->idx] ^=  0x06;

    ctx->buf[ctx->num - 1] ^= 0x80;

    SHA3_Transform(ctx);

	memcpy(h,ctx->buf,ctx->tam_digest);
	memset(ctx, 0,sizeof(sha3_t));
}


/* ------------------------------- */

void sha3_partial(sha3_t *ctx,void *hash)
{
    sha3_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha3_final(&tmp,hash);
}

/* ------------------------------- */

int shake_init(sha3_t *ctx,int bits,const void *key,size_t len)
{
    memset(ctx, 0, sizeof(sha3_t));

    switch(bits)
    {
        case 128:
            ctx->tam_digest = SHAKE128_SIZE;
           	break;
        case 256:
        	ctx->tam_digest = SHAKE256_SIZE;
            break;
        default:
            return FALSE;
    }

	ctx->num = 200 - (2 * ctx->tam_digest);

	if(key && len > 0)
	{
	    const unsigned char *datos = (unsigned char *)key;
        size_t i;
        for(i = 0; i < len; i++)
        {
    	    ctx->buf[ctx->idx++] ^= datos[i];
            if(ctx->idx == ctx->num)
            {
                SHA3_Transform(ctx);
                ctx->idx = 0;
            }
        }
    }

    ctx->buf[ctx->idx] ^=  0x1f;
    ctx->buf[ctx->num - 1] ^= 0x80;
    SHA3_Transform(ctx);
    ctx->idx = 0;
    return TRUE;
}

/* ------------------------------- */

void shake_get(sha3_t *ctx,void *buf,size_t len)
{
    size_t i;
    unsigned char *datos = (unsigned char *)buf;

    for(i = 0; i < len; i++)
    {
        if(ctx->idx == ctx->num)
        {
            SHA3_Transform(ctx);
            ctx->idx = 0;
        }

        datos[i] = ctx->buf[ctx->idx++];
    }
}

/* ------------------------------- */

void shake_clean(sha3_t *ctx)
{
    if(ctx)
        memset(ctx,0,sizeof(sha3_t));
}






