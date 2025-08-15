/*
    hash.c

    Cryptographic Hashing functions

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

#include "hash.h"

/* Algorithms name */

static char    *lst_hashc[HASH_NUM_HASHES]={"","MD2","MD4","MD5","SHA-1","SHA-224","SHA-256","SHA-384","SHA-512",
                                            "SHA-512/224","SHA-512/256","SHA3-224","SHA3-256","SHA3-384","SHA3-512",
                                            "KECCAK-224","KECCAK-256","KECCAK-384","KECCAK-512","SHAKE-128","SHAKE-256",
                                            "RIPEMD-128","RIPEMD-160","RIPEMD-256","RIPEMD-320","POLY1305"};

static wchar_t *lst_hashw[HASH_NUM_HASHES]={L"",L"MD2",L"MD4",L"MD5",L"SHA-1",L"SHA-224",L"SHA-256",L"SHA-384",L"SHA-512",
                                            L"SHA-512/224",L"SHA-512/256",L"SHA3-224",L"SHA-256",L"SHA-384",L"SHA-512",
                                            L"KECCAK-224",L"KECCAK-256",L"KECCAK-384",L"KECCAK-512",L"SHAKE-128",L"SHAKE-256",
                                            L"RIPEMD-128",L"RIPEMD-160",L"RIPEMD-256",L"RIPEMD-320",L"POLY1305"};

/** Nombres alternativos */

static char    *lst_hashC[HASH_NUM_HASHES]={"","MD-2","MD-4","MD-5","SHA1","SHA224","SHA256","SHA384","SHA512",
                                            "SHA512/224","SHA512/256","SHA3_224","SHA3_256","SHA3_384","SHA3_512",
                                            "KECCAK_224","KECCAK_256","KECCAK_384","KECCAK_512","SHAKE_128","SHAKE_256",
                                            "RIPEMD_128","RIPEMD_160","RIPEMD_256","RIPEMD_320","POLY-1305"};


static wchar_t *lst_hashW[HASH_NUM_HASHES]={L"",L"MD-2",L"MD-4",L"MD-5",L"SHA1",L"SHA224",L"SHA256",L"SHA384",L"SHA512",
                                            L"SHA512/224",L"SHA512/256",L"SHA3_224",L"SHA3_256",L"SHA3_384",L"SHA3_512",
                                            L"KECCAK_224",L"KECCAK_256",L"KECCAK_384",L"KECCAK_512",L"SHAKE_128",L"SHAKE_256",
                                            L"RIPEMD_128",L"RIPEMD_160",L"RIPEMD_256",L"RIPEMD_320",L"POLY-1305"};

/* ------------------------------ */

char *hash_name(int hash)
{
    if(hash > 0 && hash < HASH_NUM_HASHES)
        return lst_hashc[hash];
    return lst_hashc[0];
}

/* ------------------------------ */

wchar_t *hash_namew(int hash)
{
    if(hash > 0 && hash < HASH_NUM_HASHES)
        return lst_hashw[hash];
    return lst_hashw[0];
}

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
        case HASH_SHA3_224:
        case HASH_KECCAK_224:
            return SHA224_SIZE;
        case HASH_SHA256:
        case HASH_SHA3_256:
        case HASH_KECCAK_256:
            return SHA256_SIZE;
        case HASH_SHA384:
        case HASH_SHA3_384:
        case HASH_KECCAK_384:
            return SHA384_SIZE;
        case HASH_SHA512:
        case HASH_SHA3_512:
        case HASH_KECCAK_512:
            return SHA512_SIZE;
        case HASH_SHAKE_256:
            return SHAKE256_SIZE;            
        case HASH_SHAKE_128:            
            return SHAKE128_SIZE;
        case HASH_SHA512_224:
            return SHA512_224_SIZE;
        case HASH_SHA512_256:
            return SHA512_256_SIZE;
        case HASH_RIPE_128:
            return RIPE128_SIZE;
        case HASH_RIPE_160:
            return RIPE160_SIZE;
        case HASH_RIPE_256:
            return RIPE256_SIZE;
        case HASH_RIPE_320:
            return RIPE320_SIZE;
        case HASH_POLY1305:
            return POLY1305_SIZE;
        default:
            break;
    }
    return 0;
}


/* ------------------------------ */

int hash_alg(const char *hash)
{
    int ret;

    for(ret=HASH_MD2;ret<HASH_NUM_HASHES;ret++)
    {
        if(!strcasecmp(lst_hashc[ret],hash) || !strcasecmp(lst_hashC[ret],hash))
            return ret;
    }
    return HASH_NONE;
}

/* ------------------------------ */

int hash_algw(const wchar_t *hash)
{
    int ret;

    for(ret=HASH_MD2;ret<HASH_NUM_HASHES;ret++)
    {
        if(!wcscasecmp(lst_hashw[ret],hash) || !wcscasecmp(lst_hashW[ret],hash))
            return ret;
    }
    return HASH_NONE;
}

/* ------------------------------ */

int calc_hash(int alg,const void *data,int tam,void *hash)
{
    int ret;
    hash_t ctx;

    ret=hash_init(&ctx,alg,NULL,0);
    if(ret)
    {
        hash_update(&ctx,data,tam);
        hash_final(&ctx,hash);
    }
    return ret;
}


/* -------------------------------------------- */

int hash_init(hash_t *ctx,int alg,const void *key,int tam_key)
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
        case HASH_SHA512_224:
            sha512_224_init(&ctx->a.sha512);
            ret=SHA512_224_SIZE;
            break;
        case HASH_SHA512_256:
            sha512_256_init(&ctx->a.sha512);
            ret=SHA512_256_SIZE;
            break;
        case HASH_SHA3_224:
            sha3_224_init(&ctx->a.sha3);
            ret=SHA224_SIZE;
            break;
        case HASH_SHA3_256:
            sha3_256_init(&ctx->a.sha3);
            ret=SHA256_SIZE;
            break;
        case HASH_SHA3_384:
            sha3_384_init(&ctx->a.sha3);
            ret=SHA384_SIZE;
            break;
        case HASH_SHA3_512:
            sha3_512_init(&ctx->a.sha3);
            ret=SHA512_SIZE;
            break;
        case HASH_KECCAK_224:
            keccak_init(&ctx->a.sha3,224);
            ret=SHA224_SIZE;
            break;
        case HASH_KECCAK_256:
            keccak_init(&ctx->a.sha3,256);
            ret=SHA256_SIZE;
            break;
        case HASH_KECCAK_384:
            keccak_init(&ctx->a.sha3,384);
            ret=SHA384_SIZE;
            break;
        case HASH_KECCAK_512:
            keccak_init(&ctx->a.sha3,512);
            ret=SHA512_SIZE;
            break;
        case HASH_SHAKE_256:
            shake_init(&ctx->a.sha3,256,key,tam_key);
            ret=SHAKE256_SIZE;
            break;
        case HASH_SHAKE_128:
            shake_init(&ctx->a.sha3,128,key,tam_key);
            ret=SHAKE128_SIZE;
            break;
        case HASH_RIPE_128:
            ripe128_init(&ctx->a.ripe128);
            ret=RIPE128_SIZE;
            break;
        case HASH_RIPE_160:
            ripe160_init(&ctx->a.ripe160);
            ret=RIPE160_SIZE;
            break;
        case HASH_RIPE_256:
            ripe256_init(&ctx->a.ripe256);
            ret=RIPE256_SIZE;
            break;
        case HASH_RIPE_320:
            ripe320_init(&ctx->a.ripe320);
            ret=RIPE320_SIZE;
            break;
        case HASH_POLY1305:
            if(tam_key < POLY1305_KEY_SIZE)
                return -1;

            poly1305_init(&ctx->a.poly,key);

            ret=POLY1305_SIZE;
            break;
        default:
            return 0;
    }
    ctx->alg = alg;
    ctx->namec = lst_hashc[alg];
    ctx->namew = lst_hashw[alg];
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
        case HASH_SHA512_224:
            sha512_224_update(&ctx->a.sha512,data,tam);
            break;
        case HASH_SHA512_256:
            sha512_256_update(&ctx->a.sha512,data,tam);
            break;
        case HASH_SHA3_224:
        case HASH_SHA3_256:
        case HASH_SHA3_384:
        case HASH_SHA3_512:
        case HASH_KECCAK_224:
        case HASH_KECCAK_256:
        case HASH_KECCAK_384:
        case HASH_KECCAK_512:
            sha3_update(&ctx->a.sha3,data,tam);
            break;
        case HASH_SHAKE_128:
        case HASH_SHAKE_256:
            /* Ignore updates in these cases */
            break;
        case HASH_RIPE_128:
            ripe128_update(&ctx->a.ripe128,data,tam);
            break;
        case HASH_RIPE_160:
            ripe160_update(&ctx->a.ripe160,data,tam);
            break;
        case HASH_RIPE_256:
            ripe256_update(&ctx->a.ripe256,data,tam);
            break;
        case HASH_RIPE_320:
            ripe320_update(&ctx->a.ripe320,data,tam);
            break;
        case HASH_POLY1305:
            poly1305_update(&ctx->a.poly,data,tam);
            break;
        default:
            break;
    }
}

/* -------------------------------------------- */

void hash_update_str(hash_t *ctx,const char *str)
{
    if(str)
    {
        while(*str)
            hash_update(ctx,str++,sizeof(char));
    }
}

/* -------------------------------------------- */

void hash_update_wcs(hash_t *ctx,const wchar_t *str)
{
    if(str)
    {
        while(*str)
            hash_update(ctx,str++,sizeof(wchar_t));
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
        case HASH_SHA512_224:
            sha512_final(&ctx->a.sha512,hash);
            break;
        case HASH_SHA512_256:
            sha512_final(&ctx->a.sha512,hash);
            break;
        case HASH_SHA3_224:
        case HASH_SHA3_256:
        case HASH_SHA3_384:
        case HASH_SHA3_512:
        case HASH_KECCAK_224:
        case HASH_KECCAK_256:
        case HASH_KECCAK_384:
        case HASH_KECCAK_512:
            sha3_final(&ctx->a.sha3,hash);
            break;
        case HASH_SHAKE_128:
            shake_get(&ctx->a.sha3,hash,SHAKE128_SIZE);
            shake_clean(&ctx->a.sha3);
            break;
        case HASH_SHAKE_256:
            shake_get(&ctx->a.sha3,hash,SHAKE256_SIZE);
            shake_clean(&ctx->a.sha3);
            break;
        case HASH_RIPE_128:
            ripe128_final(&ctx->a.ripe128,hash);
            break;
        case HASH_RIPE_160:
            ripe160_final(&ctx->a.ripe160,hash);
            break;
        case HASH_RIPE_256:
            ripe256_final(&ctx->a.ripe256,hash);
            break;
        case HASH_RIPE_320:
            ripe320_final(&ctx->a.ripe320,hash);
            break;
        case HASH_POLY1305:
            poly1305_final(&ctx->a.poly,hash);
            break;
        default:
            break;
    }

    /* Limpiar evidencia */

    memset(ctx,0,sizeof(hash_t));
}


/* -------------------------------------------- */

void hash_partial(hash_t *ctx,void *hash)
{
    hash_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    hash_final(&tmp,hash);
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
            case HASH_POLY1305:
                ctx->passthru = TRUE;
                break;
            case HASH_KECCAK_224:
            case HASH_SHA3_224:
                ctx->tam_block = HMAC_SHA3_224_SIZE;
                break;
            case HASH_KECCAK_256:
            case HASH_SHA3_256:
                ctx->tam_block = HMAC_SHA3_256_SIZE;
                break;
            case HASH_KECCAK_384:
            case HASH_SHA3_384:
                ctx->tam_block = HMAC_SHA3_384_SIZE;
                break;
            case HASH_KECCAK_512:
            case HASH_SHA3_512:
                ctx->tam_block = HMAC_SHA3_512_SIZE;
                break;
            case HASH_SHA384:
            case HASH_SHA512:
                ctx->tam_block = HMAC_BIG_SIZE;
                break;
            default:
                ctx->tam_block = HMAC_SIZE;
                break;
        }

        if(ctx->passthru == TRUE)
        {
            ctx->tam_hash = hash_init(&ctx->h,alg,key,tam_key);
            if(ctx->tam_hash > 0)
                return ctx->tam_hash;
        }
        else
        {
            ctx->tam_hash = hash_init(&ctx->h,alg,NULL,0);
            if(ctx->tam_hash > 0)
            {
                unsigned char ipad[HMAC_MAX_BLOCK_SIZE];

                if(tam_key > ctx->tam_block)
                    calc_hash(alg,key,tam_key,ctx->hkey);
                else
                    memcpy(ctx->hkey,key,tam_key);

                memcpy(ipad,ctx->hkey,ctx->tam_block);
                memxor(ipad,0x36,ctx->tam_block);

                hash_update(&ctx->h,ipad,ctx->tam_block);

                memset(ipad,0,ctx->tam_block);

                return ctx->tam_hash;
            }
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

void hmac_update_str(hmac_t *ctx,const char *str)
{
    if(str)
    {
        while(*str)
            hmac_update(ctx,str++,sizeof(char));
    }
}

/* -------------------------------------------- */

void hmac_update_wcs(hmac_t *ctx,const wchar_t *str)
{
    if(str)
    {
        while(*str)
            hmac_update(ctx,str++,sizeof(wchar_t));
    }
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

        hash_init(&ctx->h,alg,NULL,0);
        hash_update(&ctx->h,opad,ctx->tam_block);
        hash_update(&ctx->h,ctx->hkey,ctx->tam_hash);

        memset(opad,0,ctx->tam_block);
    }

    hash_final(&ctx->h,hmac);

    memset(ctx,0,sizeof(hmac_t));
}

/* -------------------------------------------- */

void hmac_partial(hmac_t *ctx,void *hash)
{
    hmac_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    hmac_final(&tmp,hash);
}

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

static FILE *fopenw(const wchar_t *name,const char *perm)
{
    char tmp[1384];
    FILE *fp=NULL;

    if(name && wcstombs(tmp,name,1383) > 0)
        fp = fopen(tmp,perm);
    return fp;
}

/* --------------------------------------- */

#define F_H_NAMETOO     1
#define F_H_WCHAR       2

static int do_hash_file(int alg,const void *fich,void *hash,u64_t *tam,int flags)
{
    hash_t ctx;
    FILE *fp;
    int ret;

    ret=hash_init(&ctx,alg,NULL,0);
    if(ret)
    {
        if(flags & F_H_WCHAR)
            fp=fopenw((const wchar_t *)fich,"rb");
        else
            fp=fopen((const char *)fich,"rb");
        if(!fp)
            return -2;
        if(do_hash_file_content(&ctx,fp,tam))
            ret = -2;
        fclose(fp);
        if(ret > 0 && (flags & F_H_NAMETOO))
        {
            if(flags & F_H_WCHAR)
                hash_update_wcs(&ctx,fich);
            else
                hash_update_str(&ctx,fich);
        }
        hash_final(&ctx,hash);
    }
    return ret;
}

/* --------------------------------------- */

int calc_hash_file(int alg,const char *fich,void *hash,u64_t *tam)
{
    return do_hash_file(alg,fich,hash,tam,0);
}

/* --------------------------------------- */

int calc_hash_filew(int alg,const wchar_t *fich,void *hash,u64_t *tam)
{
    return do_hash_file(alg,fich,hash,tam,F_H_WCHAR);
}

/* --------------------------------------- */


int calc_hash_file_and_name(int alg,const char *fich,void *hash,u64_t *tam)
{
    return do_hash_file(alg,fich,hash,tam,F_H_NAMETOO);
}

/* --------------------------------------- */


int calc_hash_file_and_namew(int alg,const wchar_t *fich,void *hash,u64_t *tam)
{
    return do_hash_file(alg,fich,hash,tam,F_H_NAMETOO|F_H_WCHAR);
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

static int do_hmac_file(int alg,const void *clave,unsigned int tam_clave,const void *fich,void *hash,u64_t *tam,int flags)
{
    hmac_t ctx;
    FILE *fp;
    int ret;

    ret=hmac_init(&ctx,alg,clave,tam_clave);
    if(ret)
    {
        if(flags & F_H_WCHAR)
            fp=fopenw((const wchar_t *)fich,"rb");
        else
            fp=fopen((const char *)fich,"rb");
        if(!fp)
            return -2;
        if(do_hmac_file_content(&ctx,fp,tam))
            ret = -2;
        fclose(fp);
        if(ret > 0 && (flags & F_H_NAMETOO))
        {
            if(flags & F_H_WCHAR)
               hmac_update_wcs(&ctx,fich);
            else
               hmac_update_str(&ctx,fich);
        }
        hmac_final(&ctx,hash);
    }
    return ret;
}

/* --------------------------------------- */

int calc_hmac_file(int alg,const void *clave,unsigned int tam_clave,const char *fich,void *hash,u64_t *tam)
{
    return do_hmac_file(alg,clave,tam_clave,fich,hash,tam,0);
}

/* --------------------------------------- */

int calc_hmac_filew(int alg,const void *clave,unsigned int tam_clave,const wchar_t *fich,void *hash,u64_t *tam)
{
    return do_hmac_file(alg,clave,tam_clave,fich,hash,tam,F_H_WCHAR);
}

/* --------------------------------------- */

int calc_hmac_file_and_name(int alg,const void *clave,unsigned int tam_clave,const char *fich,void *hash,u64_t *tam)
{
    return do_hmac_file(alg,clave,tam_clave,fich,hash,tam,F_H_NAMETOO);
}

/* --------------------------------------- */


int calc_hmac_file_and_namew(int alg,const void *clave,unsigned int tam_clave,const wchar_t *fich,void *hash,u64_t *tam)
{
    return do_hmac_file(alg,clave,tam_clave,fich,hash,tam,F_H_NAMETOO|F_H_WCHAR);
}




