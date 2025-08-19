/*
    hmac.c

    Hash-derived Message Authentication Code functions

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

#include "hmac.h"

/* -------------------------------------------- */

int hash_init(hash_t *ctx,int alg)
{
    int ret=0;

    if(!ctx)
        return -1;

    memset(ctx,0,sizeof(hash_t));

    switch(alg)
    {
        case HASH_MD5:
            md5_init(&ctx->a.md5);
            ret=MD5_SIZE;
            break;
        case HASH_SHA1:
            sha1_init(&ctx->a.sha1);
            ret=SHA1_SIZE;
            break;
        case HASH_SHA224:
            sha224_init(&ctx->a.sha224);
            ret=SHA224_SIZE;
            break;
        case HASH_SHA256:
            sha256_init(&ctx->a.sha256);
            ret=SHA256_SIZE;
            break;
        case HASH_SHA384:
            sha384_init(&ctx->a.sha384);
            ret=SHA384_SIZE;
            break;
        case HASH_SHA512:
            sha512_init(&ctx->a.sha512);
            ret=SHA512_SIZE;
            break;
        default:
            return 0;
    }
    ctx->alg = alg;    
    return ret;
}

/* -------------------------------------------- */

void hash_update(hash_t *ctx,const void *data,size_t tam)
{
    switch(ctx->alg)
    {
        case HASH_MD5:
            md5_update(&ctx->a.md5,data,tam);
            break;
        case HASH_SHA1:
            sha1_update(&ctx->a.sha1,data,tam);
            break;
        case HASH_SHA224:
            sha224_update(&ctx->a.sha224,data,tam);
            break;
        case HASH_SHA256:
            sha256_update(&ctx->a.sha256,data,tam);
            break;
        case HASH_SHA384:
            sha384_update(&ctx->a.sha384,data,tam);
            break;
        case HASH_SHA512:
            sha512_update(&ctx->a.sha512,data,tam);
            break;        
        default:
            break;
    }
}

/* -------------------------------------------- */

void hash_final(hash_t *ctx,void *hash)
{
    switch(ctx->alg)
    {
        case HASH_MD5:
            md5_final(&ctx->a.md5,hash);
            break;
        case HASH_SHA1:
            sha1_final(&ctx->a.sha1,hash);
            break;
        case HASH_SHA224:
            sha224_final(&ctx->a.sha224,hash);
            break;
        case HASH_SHA256:
            sha256_final(&ctx->a.sha256,hash);
            break;
        case HASH_SHA384:
            sha384_final(&ctx->a.sha384,hash);
            break;
        case HASH_SHA512:
            sha512_final(&ctx->a.sha512,hash);
            break;
        default:
            break;
    }

    /* Limpiar evidencia */

    memset(ctx,0,sizeof(hash_t));
}

/* -------------------------------------------- */

int hmac_init(hmac_t *ctx,int alg,const void *key,size_t tam_key)
{
    if(ctx)
    {
        memset(ctx,0,sizeof(hmac_t));

        /* Determine the blocksize of the HMAC and passthru */

        switch(alg)
        {
            case HASH_SHA384:
            case HASH_SHA512:
                ctx->tam_block = HMAC_BIG_SIZE;
                break;
            default:
                ctx->tam_block = HMAC_SIZE;
                break;
        }

        ctx->tam_hash = hash_init(&ctx->h,alg);
        if(ctx->tam_hash > 0)
        {
            unsigned char ipad[HMAC_MAX_BLOCK_SIZE];
            
            if(tam_key > ctx->tam_block)
            {
                hash_t tmp;

                hash_init(&tmp,alg);
                hash_update(&tmp,key,tam_key);
                hash_final(&tmp,ctx->hkey);
            }
            else
            {
                memcpy(ctx->hkey,key,tam_key);
            }

            memcpy(ipad,ctx->hkey,ctx->tam_block);
            memxor(ipad,0x36,ctx->tam_block);

            hash_update(&ctx->h,ipad,ctx->tam_block);

            memset(ipad,0,ctx->tam_block);

            return ctx->tam_hash;
        }
        memset(ctx,0,sizeof(hmac_t));
    }
    return 0;
}

/* -------------------------------------------- */

void hmac_update(hmac_t *ctx,const void *data,size_t tam)
{
    hash_update(&ctx->h,data,tam);
}

/* -------------------------------------------- */

void hmac_final(hmac_t *ctx,void *hmac)
{
    unsigned char opad[HMAC_MAX_BLOCK_SIZE];
    int alg = ctx->h.alg;

    if(ctx->passthru != TRUE)
    {
        memcpy(opad,ctx->hkey,ctx->tam_block);
        memxor(opad,0x5c,ctx->tam_block);

        hash_final(&ctx->h,ctx->hkey);

        hash_init(&ctx->h,alg);
        hash_update(&ctx->h,opad,ctx->tam_block);
        hash_update(&ctx->h,ctx->hkey,ctx->tam_hash);

        memset(opad,0,ctx->tam_block);
    }

    hash_final(&ctx->h,hmac);

    memset(ctx,0,sizeof(hmac_t));
}

