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
        case HASH_MD2:
            md2_init(&ctx->a.md2);
            ret=MD2_SIZE;
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

/* --------------------------------- *
   Include specific headers for each
   operating system.
 * --------------------------------- */

#ifdef FOR_WIN

#include <windows.h>

#else 

#include <sys/time.h>

#ifdef FOR_MAC

#include <mach/mach_time.h>

#endif

#endif

/* ------------------------------------------------------------- *
    Return the high resolution tick counter of the CPU.

    In linux/unix it has a 10 microsecond resolution, but is 
    enough for its use here.
 * ------------------------------------------------------------- */

static u64_t cpu_ticks(void)
{
    u64_t ret;

#if defined(FOR_WIN)
	LARGE_INTEGER cont;

    QueryPerformanceCounter(&cont);
	ret = (u64_t) cont.QuadPart;

#elif defined(FOR_MAC)

    ret = (u64_t)mach_absolute_time();

#else

    struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	ret = (u64_t)ts.tv_sec;
	ret*= 1000000000;
	ret+= ts.tv_nsec;

#endif

    return ret;
}

/* -------------------------------------------- */

unsigned int unpredictable_seed(void)
{
    unsigned int seed,tam,t;
    u64_t a[5];
    static u64_t sclk = 0;
    char *b;

    /*
        Get the current count of cpu time 
        using a system-dependant call so any 
        try to reproduce this would be really 
        difficult.
    */

    a[0]=cpu_ticks();

     /*
        Now we allocate up to 1023 bytes of
        memory in order to hash the memory
        address and its content more or
        less quickly using DJB. The way in
        which we do it should facilitate
        for the function to take different 
        execution time and thus affect the 
        values returned by cpu_ticks(), as 
        well as to give different addresses 
        and content if somethign happens 
        between calls to this function.
    */

    tam = (unsigned int)(a[0] & 0xff) | 0x300;
    b = (char *)malloc(tam);
    if(b)
    {
        a[1] = 5381;
        for(t = 0;t < tam ; t++)
        {
            a[1] *= 33;
            a[1] += b[t];
        }
        a[1]<<=32;
        a[1] |= (u64_t)((size_t)b);
        a[1] |= tam;
        free(b);        
    }
    else
    {
        a[1] = ~a[0];
    }

    /*  
        We initialize the sequence number
        (the counter) if this is the 
        first call. 

        We do it here to make sure that
        time is a bit more disturbed than 
        if this was done at the start of 
        the function.
    */

    if(!sclk)
        sclk = cpu_ticks();
    
    /*
        Feed the array with an amalgamation
        of everything so we have some more
        data to feed to the DJB hash.
    */

    a[2]=~sclk;
    a[3]=++sclk;
    a[4]=(a[0] ^ a[1] ^ a[2] ^ a[3]);

    /* 
        Hash everything using DJB in order
        to build the final seed.
    */

    b = (char *)a;

    for(seed=5381,t=0;t < sizeof(a);t++)
        seed = (seed << 5) + seed + b[t];

    return seed;
}

/* ------------------------------- */

void hash_get_entropy(int hash,void *dest,size_t num)
{
    unsigned char *buf,tmp[MAX_HASH_SIZE];
    size_t cnt = 0;
    unsigned int left = 0, *seed;
    hash_t ctx;
    
    if(hash > HASH_NONE && hash < HASH_NUM_HASHES)
    {
        buf  = (unsigned char *)dest;
        seed = (unsigned int *)tmp;
        while(cnt < num)
        {
            if(left == 0)
            {                
                left = hash_init(&ctx,hash);

                seed[0] = unpredictable_seed();
                seed[1] = unpredictable_seed();
                seed[2] = unpredictable_seed();
                seed[3] = unpredictable_seed();
                seed[4] = unpredictable_seed();
                seed[5] = unpredictable_seed();
                seed[6] = unpredictable_seed();
                seed[7] = unpredictable_seed();

                hash_update(&ctx,tmp,32);
                hash_final(&ctx,tmp);                
            }
            buf[cnt++] = tmp[--left];
        }
    }
}

