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


/* ------------------------------ */

int hash_size(int alg)
{
    switch(alg)
    {
        case HASH_MD2:
            return MD2_SIZE;
        case HASH_MD4:
            return MD4_SIZE;
        case HASH_MD5:
            return MD5_SIZE;
        case HASH_SHA1:
            return SHA1_SIZE;
        case HASH_SHA224:        
            return SHA224_SIZE;
        case HASH_SHA256:        
            return SHA256_SIZE;
        case HASH_SHA384:        
            return SHA384_SIZE;
        case HASH_SHA512:        
            return SHA512_SIZE;        
        default:
            break;
    }
    return 0;
}

/* ------------------------------ */

int calc_hash(int alg,const void *data,int tam,void *hash)
{
    int ret;
    hash_t ctx;

    ret=hash_init(&ctx,alg);
    if(ret)
    {
        hash_update(&ctx,data,tam);
        hash_final(&ctx,hash);
    }
    return ret;
}




/* -------------------------------------------- */

int hash_init(hash_t *ctx,int alg)
{
    int ret=0;

    if(!ctx)
        return -1;

    memset(ctx,0,sizeof(hash_t));

    switch(alg)
    {
        case HASH_MD2:
            md2_init(&ctx->a.md2);
            ret=MD2_SIZE;
            break;
        case HASH_MD4:
            md4_init(&ctx->a.md4);
            ret=MD4_SIZE;
            break;
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
        case HASH_MD2:
            md2_update(&ctx->a.md2,data,tam);
            break;
        case HASH_MD4:
            md4_update(&ctx->a.md4,data,tam);
            break;
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
        case HASH_MD2:
            md2_final(&ctx->a.md2,hash);
            break;
        case HASH_MD4:
            md4_final(&ctx->a.md4,hash);
            break;
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

/* ------------------------------ */

int calc_hmac(int alg,const void *key,int tam_key,const void *data,int tam,void *hash)
{
    int ret;
    hmac_t ctx;

    ret=hmac_init(&ctx,alg,key,tam_key);
    if(ret)
    {
        hmac_update(&ctx,data,tam);
        hmac_final(&ctx,hash);
    }
    return ret;
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


/* --------------------------------------- */
/* --------------------------------------- */

static int do_hash_file_content(hash_t *ctx,FILE *fp,u64_t *tam)
{
    u64_t tamf = 0;
    unsigned char buf[512];
    size_t readed;

    while(!feof(fp))
    {
        readed = fread(buf,1,512,fp);
        if(readed < 1)
        {
            if(ferror(fp))
            {
                fclose(fp);
                return -2;
            }

            break;
        }
        hash_update(ctx,buf,readed);
        tamf+=readed;
    }
    if(tam)
      *tam = tamf;
    return 0;
}

/* --------------------------------------- */

static int do_hash_file(int alg,const void *fich,void *hash,u64_t *tam,int wide)
{
    hash_t ctx;
    FILE *fp;
    int ret;

    ret=hash_init(&ctx,alg);
    if(ret)
    {
        if(wide)
            fp=fopenw((const wchar_t *)fich,"rb");
        else
            fp=fopen((const char *)fich,"rb");
        if(!fp)
            return -2;
        if(do_hash_file_content(&ctx,fp,tam))
            ret = -2;
        fclose(fp);
        hash_final(&ctx,hash);
    }
    return ret;
}

/* --------------------------------------- */

int calc_hash_file(int alg,const char *fich,void *hash,u64_t *tam)
{
    return do_hash_file(alg,fich,hash,tam,FALSE);
}

/* --------------------------------------- */

int calc_hash_filew(int alg,const wchar_t *fich,void *hash,u64_t *tam)
{
    return do_hash_file(alg,fich,hash,tam,TRUE);
}

/* --------------------------------------- */
/* --------------------------------------- */

static int do_hmac_file_content(hmac_t *ctx,FILE *fp,u64_t *tam)
{
    u64_t tamf = 0;
    unsigned char buf[512];
    size_t readed;

    while(!feof(fp))
    {
        readed = fread(buf,1,512,fp);
        if(readed < 1)
        {
            if(ferror(fp))
            {
                fclose(fp);
                return -2;
            }

            break;
        }
        hmac_update(ctx,buf,readed);
        tamf+=readed;
    }
    if(tam)
      *tam = tamf;
    return 0;
}
/* --------------------------------------- */

static int do_hmac_file(int alg,const void *clave,unsigned int tam_clave,const void *fich,void *hash,u64_t *tam,int wide)
{
    hmac_t ctx;
    FILE *fp;
    int ret;

    ret=hmac_init(&ctx,alg,clave,tam_clave);
    if(ret)
    {
        if(wide)
            fp=fopenw((const wchar_t *)fich,"rb");
        else
            fp=fopen((const char *)fich,"rb");
        if(!fp)
            return -2;
        if(do_hmac_file_content(&ctx,fp,tam))
            ret = -2;
        fclose(fp);
        hmac_final(&ctx,hash);
    }
    return ret;
}

/* --------------------------------------- */

int calc_hmac_file(int alg,const void *clave,unsigned int tam_clave,const char *fich,void *hash,u64_t *tam)
{
    return do_hmac_file(alg,clave,tam_clave,fich,hash,tam,FALSE);
}

/* --------------------------------------- */

int calc_hmac_filew(int alg,const void *clave,unsigned int tam_clave,const wchar_t *fich,void *hash,u64_t *tam)
{
    return do_hmac_file(alg,clave,tam_clave,fich,hash,tam,TRUE);
}
