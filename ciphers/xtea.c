/*
    xtea.c

    XTEA Cypher

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

        - Adapted by Guillermo Amodeo Ojeda from public domain code by
          David Wheeler and Roger Needham from:

          https://en.wikipedia.org/wiki/XTEA

                                --oO0Oo--
*/

#include "xtea.h"

/* ------------------------------- */

int xtea_init(xtea_t *ctx,int rondas,const void *key)
{
    if(ctx && key)
    {
        unsigned char *k;

        memset(ctx,0,sizeof(xtea_t));
        ctx->kbits=(XTEA_KEY_SIZE * 8);
        ctx->num_rounds = (rondas < XTEA_DEF_ROUNDS) ? XTEA_DEF_ROUNDS : rondas;

        k=(unsigned char *)key;

        ctx->k[0] = get_be32(k + 0);
        ctx->k[1] = get_be32(k + 4);
        ctx->k[2] = get_be32(k + 8);
        ctx->k[3] = get_be32(k + 12);

        return 0;
    }
    return -1;
}

#define XTEA_DELTA          0x9E3779B9

/* ---------------------------- */

void xtea_encode_block(xtea_t *ctx,void *dest,const void *orig)
{
    unsigned char *sblk,*dblk;
    unsigned int v0,v1,sum;
    int i;

    sblk=(unsigned char *)orig;
    dblk=(unsigned char *)dest;

    v0=get_be32(sblk + 0);
    v1=get_be32(sblk + 4);

    for (sum=0,i=0; i < ctx->num_rounds; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + ctx->k[sum & 3]);
        sum += XTEA_DELTA ;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + ctx->k[(sum>>11) & 3]);
    }

    put_be32(dblk + 0,v0);
    put_be32(dblk + 4,v1);
}

/* ---------------------------- */

void xtea_decode_block(xtea_t *ctx,void *dest,const void *orig)
{
    unsigned char *sblk,*dblk;
    unsigned int v0,v1,sum;
    int i;

    sblk=(unsigned char *)orig;
    dblk=(unsigned char *)dest;

    v0=get_be32(sblk + 0);
    v1=get_be32(sblk + 4);

    sum=XTEA_DELTA  * ctx->num_rounds;
    for (i=0; i < ctx->num_rounds; i++)
    {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + ctx->k[(sum>>11) & 3]);
        sum -= XTEA_DELTA ;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + ctx->k[sum & 3]);
    }

    put_be32(dblk + 0,v0);
    put_be32(dblk + 4,v1);

}

/* Validation sets (32 rounds):
 *
 * Key    : 00010203 04050607 08090a0b 0c0d0e0f
 * Plain  : 41424344 45464748
 * Cipher : 497df3d0 72612cb5
 *
 * Key    : 00010203 04050607 08090a0b 0c0d0e0f
 * Plain  : 41414141 41414141
 * Cipher : e78f2d13 744341d8
 *
 * Key    : 00010203 04050607 08090a0b 0c0d0e0f
 * Plain  : 5a5b6e27 8948d77f
 * Cipher : 41414141 41414141
 *
 **********************************************************************/





