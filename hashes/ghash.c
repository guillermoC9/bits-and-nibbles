/*
    ghash.c

    Galois Hashing

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

        - Galois Hash is a hashing algorithm based on the function for the
          multiplication of points on finite fields created by Evariste Galois,
          aka GF(x,y) and used on the GCM mode of AES.

        - Written using NIST info on AES-GCM

                                --oO0Oo--
*/

#include "ghash.h"

/* ------------------------------- */

void ghash_init(ghash_t *ctx,const void *iv)
{
    memset(ctx,0,sizeof(ghash_t));
    if(iv)
    {
        memcpy(ctx->iv,iv,GHASH_SIZE);
    }
}

/* ------------------------------- */

void ghash_update(ghash_t *ctx,const void *buf, size_t tam)
{
    unsigned char *datos = (unsigned char *)buf;
    size_t t;

    for(t=0;t<tam;t++)
    {
        ctx->buffer[ctx->pos++]=datos[t];
        ctx->bits+=8;

        if(ctx->pos == GHASH_SIZE)
        {
            mem_xor(ctx->estado,ctx->buffer,GHASH_SIZE);
            gcm_mult(ctx->estado,ctx->estado,ctx->iv,GHASH_SIZE);

            memset(ctx->buffer,0,GHASH_SIZE);
            ctx->pos=0;
        }
    }
}

/* ------------------------------- */

void ghash_final(ghash_t *ctx,void *hash)
{
    if(ctx->pos > 0)
    {
        mem_xor(ctx->estado,ctx->buffer,GHASH_SIZE);
        gcm_mult(ctx->estado,ctx->estado,ctx->iv,GHASH_SIZE);
    }

    memset(ctx->buffer,0,GHASH_SIZE);
    put_be64(ctx->buffer + (GHASH_SIZE - 8),ctx->bits);

    mem_xor(ctx->estado,ctx->buffer,GHASH_SIZE);
    gcm_mult(ctx->estado,ctx->estado,ctx->iv,GHASH_SIZE);

    memcpy(hash,ctx->estado,GHASH_SIZE);

	memset(ctx, 0,sizeof(ghash_t));
}

/* ------------------------------- */

void ghash_partial(ghash_t *ctx,void *hash)
{
    ghash_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    ghash_final(&tmp,hash);
}

