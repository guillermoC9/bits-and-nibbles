/*
    random.c

    Random and pseudo-ramdom numbers generation

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

        - Written by Guillermo Amodeo Ojeda, there is also code from:

            - George Marsaglia, Florida University (Mother and Xorshift)
            - Julienne Walker, Eternallyconfuzzled.com (Mersenne Twister)

                                --oO0Oo--
*/

#include "random.h"

/** Needed for OSSYS generator */

#ifndef FOR_WIN

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_CLOEXEC
    #define O_CLOEXEC 0
#endif

#endif

/* ----------------------------------------------------------- *
    MOTHER
 * ----------------------------------------------------------- */

static short int top_linear[8]={1941,1860,1812,1776,1492,1215,1066,12013};
static short int bot_linear[8]={1111,2222,3333,4444,5555,6666,7777,9272};

/* Mother Context */

typedef struct
{
    unsigned int  value;

    unsigned short int     top[10];  /* Top half */
    unsigned short int     bot[10];  /* Bottom half */

} gm_rand_t;

/* -------------------------------- */

static unsigned int mother_function(gm_rand_t *rc)
{
    unsigned int top,bot,tmp;
    int t,i;

    top=(unsigned int)rc->top[0];
    bot=(unsigned int)rc->bot[0];

    memmove(rc->top+2,rc->top+1,8*sizeof(unsigned short int));
    memmove(rc->bot+2,rc->bot+1,8*sizeof(unsigned short int));

    for(i=0,t=2;t<10;t++,i++)
    {
        tmp=top_linear[i];
        tmp*=rc->top[t];
        top+=tmp;
        tmp=bot_linear[i];
        tmp*=rc->bot[t];
        bot+=tmp;
    }

    rc->top[0]=(unsigned short int)(top >> 16);
    rc->bot[0]=(unsigned short int)(bot >> 16);

    rc->top[1]=(unsigned short int)(top & 0xFFFFU);
    rc->bot[1]=(unsigned short int)(bot & 0xFFFFU);

    return (unsigned int)(((top & 0xFFFFU) << 16) | (bot & 0xFFFFU));
}

/* -------------------------- */

static void mother_bytes(gm_rand_t *rc,void *data,size_t max)
{
    size_t num = 0,i;
    unsigned char *ptr=(unsigned char *)data;
    unsigned int tmp;

    while(num < max)
    {
        tmp=mother_function(rc);
        for(i=0;i<sizeof(unsigned int) && num < max;i++)
        {
            ptr[num++]=(unsigned char) (tmp & 0xFFU);
            tmp>>=8;
        }
    }
}

/* -------------------------------- */

static void mother_init(gm_rand_t *rc,unsigned int seed)
{
    unsigned int dn,tn;
    unsigned short int sn;
    int t;

    if(!seed)
        seed=unpredictable_seed();

    dn=(unsigned int)(seed & 0x7FFFFFFFU);
    sn=(unsigned short int)(seed & 0xFFFFU);

    for(t=0;t<9;t++)
    {
        dn>>=16;
        tn=30903;
        tn*=sn;
        tn+=dn;
        sn=(unsigned short int)(tn & 0xFFFFU);
        rc->top[t]=sn;
        dn=tn;
    }

    for(t=0;t<9;t++)
    {
        dn>>=16;
        tn=30903;
        tn*=sn;
        tn+=dn;
        sn=(unsigned short int)(tn & 0xFFFFU);
        rc->bot[t]=sn;
        dn=tn;
    }

    rc->top[0]&=0x7FFF;
    rc->bot[0]&=0x7FFF;
}

/* ----------------------------------------------------------- *
   MERSENNE TWISTER
 * ----------------------------------------------------------- */

#define MT_N     624
#define MT_M     397
#define MT_A     0x9908b0dfU
#define MT_U     0x80000000U
#define MT_L     0x7fffffffU


/* Mersenne Twister Context*/

typedef struct
{
    unsigned int    x[MT_N];
    unsigned int    next;
} mt_rand_t;

/* -------------------------------- */

static unsigned int mersenne_twister_function(mt_rand_t *rc)
{
    unsigned int y, a;
    int i;

    if (rc->next == MT_N )
    {
        rc->next = 0;

        for ( i = 0; i < MT_N - 1; i++ )
        {
            y = ( rc->x[i] & MT_U ) | (rc->x[i + 1] & MT_L);
            a = ( y & 0x1U ) ? MT_A : 0x0U;
            rc->x[i] = rc->x[( i + MT_M ) % MT_N] ^ ( y >> 1 ) ^ a;
        }

        y = ( rc->x[MT_N - 1] & MT_U ) | (rc->x[0] & MT_L);

        a = ( y & 0x1U ) ? MT_A : 0x0U;

        rc->x[MT_N - 1] = rc->x[MT_M - 1] ^ ( y >> 1 ) ^ a;
    }

    y = rc->x[rc->next++];

    /* Improve distribution */

    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680U;
    y ^= (y << 15) & 0xefc60000U;
    y ^= (y >> 18);

    return y;
}

/* -------------------------- */

static void mersenne_twister_bytes(mt_rand_t *rc,void *data,size_t max)
{
    size_t num = 0,i;
    unsigned char *ptr=(unsigned char *)data;
    unsigned int tmp;

    while(num < max)
    {
        tmp=mersenne_twister_function(rc);
        for(i=0;i<sizeof(unsigned int) && num < max;i++)
        {
            ptr[num++]=(unsigned char) (tmp & 0xffU);
            tmp>>=8;
        }
    }
}

/* -------------------------------- */

static void mersenne_twister_init(mt_rand_t *rc,unsigned int seed)
{
    int i;

    if(!seed)
        seed=unpredictable_seed();

    rc->next=0;

    rc->x[0] = (seed & 0xffffffff);

    for ( i = 1; i < MT_N; i++ )
    {
        rc->x[i] = ( 1812433253 * ( rc->x[i - 1] ^ ( rc->x[i - 1] >> 30 ) ) + i );
        rc->x[i] &= 0xffffffff;
    }
}

/* ----------------------------------------------------------- *
    XORSHIFT 128
 * ----------------------------------------------------------- */

/* Xorshift 128 Context */


typedef struct
{
    unsigned int x, y, z, w;
} gx_rand_t;

/* -------------------------------- */

static unsigned int xorshift128_function(gx_rand_t *rc)
{
    unsigned int tmp;

    tmp = rc->x ^ (rc->x << 11);

    rc->x = rc->y;
    rc->y = rc->z;
    rc->z = rc->w;

    rc->w = rc->w ^ (rc->w >> 19) ^ tmp ^ (tmp >> 8);

    return rc->w;
}

/* -------------------------- */

static void xorshift128_bytes(gx_rand_t *rc,void *data,size_t max)
{
    size_t num = 0,i;
    unsigned char *ptr=(unsigned char *)data;
    unsigned int tmp;

    while(num < max)
    {
        tmp = xorshift128_function(rc);
        for(i=0; i<sizeof(tmp) && num < max;i++)
        {
            ptr[num++]=(unsigned char) (tmp & 0xff);
            tmp>>=8;
        }
    }
}

/* -------------------------------- */

static void xorshift128_init(gx_rand_t *rc,unsigned int seed)
{

    gm_rand_t m;

    /* Use mother for initial values */

    mother_init(&m,seed);

    rc->x = mother_function(&m);
    rc->y = mother_function(&m);
    rc->z = mother_function(&m);
    rc->w = mother_function(&m);
}

/* ----------------------------------------------------------- *
    TLS-PRF

    P_HASH() is the heart of the TLS Pseudo-Ramdom Function
 * ----------------------------------------------------------- */

static void p_hash(int alg,int xor,const void *sec,int tam_sec,const void *s1,int tam_s1,
                   const void *s2 ,int tam_s2, const void *s3,int tam_s3, void *ret,size_t num)
{
    unsigned char *dest,a0[MAX_HASH_SIZE],ai[MAX_HASH_SIZE];
    size_t remain;
    int cnt,hash_size;
    hmac_t ctx;

    /* A(0) = p_hash(sec,s1,s2,s3) */

    hash_size=hmac_init(&ctx,alg,sec,tam_sec);
    hmac_update(&ctx,s1,tam_s1);

    if(s2)
        hmac_update(&ctx,s2,tam_s2);

    if(s3)
        hmac_update(&ctx,s3,tam_s3);
    hmac_final(&ctx,a0);


    remain=num;
    dest = (unsigned char *)ret;

    while(remain > 0)
    {
        /* Calc A(i) = p_hash(sec,A(i-1),s1,s2) */

        hmac_init(&ctx,alg,sec,tam_sec);
        hmac_update(&ctx,a0,hash_size);
        hmac_update(&ctx,s1,tam_s1);

        if(s2)
            hmac_update(&ctx,s2,tam_s2);

        if(s3)
            hmac_update(&ctx,s3,tam_s3);

        hmac_final(&ctx,ai);

        cnt = (remain > hash_size) ? hash_size : remain;

        if(xor)
            mem_xor(dest,ai,cnt);
        else
            memcpy(dest,ai,cnt);

        dest+=cnt;
        remain-=cnt;

        /* Prepare next round A(i-1) = p_hash(sec,A(i)) */

        hmac_init(&ctx,alg,sec,tam_sec);
        hmac_update(&ctx,a0,hash_size);
        hmac_final(&ctx,a0);
    }
}

/* -------------------------------------------- *

    This function generates random bytes using
    the specified 'prf' function.

 * -------------------------------------------- */

static void tls_prf(int prf,const void *sec,int tam_sec,const char *label,const void *s1,
                    int tam_s1, const void *s2,int tam_s2, void *ret,size_t num)
{
    int sec_tam,lbl_size;
    unsigned char *sec2,*sec1;

    lbl_size = strlen(label);

    switch(prf)
    {
        case RAND_TLS_MD5_SHA1:   /* P_HASH() of TLS v1.0 and v1.1 is P_HASH(MD5) ^ P_HASH(SHA1) */

            /* Prepare both halves of the secret */

            sec_tam = (tam_sec / 2);
            sec1 = (unsigned char *)sec;
            sec2 = sec1 + sec_tam;
            if(tam_sec & 1)
                sec_tam ++;

            /* Create MD5 part */

            p_hash(HASH_MD5,FALSE,sec1,sec_tam,label,lbl_size,s1,tam_s1,s2,tam_s2,ret,num);

            /* Now create the SHA-1 part and make P_HASH() XOR it with the MD5 part */

            p_hash(HASH_SHA1,TRUE,sec2,sec_tam,label,lbl_size,s1,tam_s1,s2,tam_s2,ret,num);

            break;        

        case RAND_TLS_SHA384: /* P_HASH() SHA-384 (from RFC-5288) */

            p_hash(HASH_SHA384,FALSE,sec,tam_sec,label,lbl_size,s1,tam_s1,s2,tam_s2,ret,num);
            break;

        case RAND_TLS_SHA256:  /* SHA-256 P_HASH() from TLS v1.2 */
        default:
            p_hash(HASH_SHA256,FALSE,sec,tam_sec,label,lbl_size,s1,tam_s1,s2,tam_s2,ret,num);
            break;
    }
}

/* ---------------------------------- *
    Random Functions and context to
    be used by TLS implementations.
 * ---------------------------------- */

 typedef struct
 {
    int prf;
    unsigned int x,y,w,z;
 } tls_rnd_t;

/* ---------------------------------- */

static void tls_rand8(tls_rnd_t *ctx,void *data,size_t num)
{
    u64_t p1,p2;
    unsigned int tmp;

    /* Execute Marsaglia's Xorshift 128 */

    tmp = ctx->x ^ (ctx->x << 11);

    ctx->x = ctx->y;
    ctx->y = ctx->z;
    ctx->z = ctx->w;
    ctx->w = ctx->w ^ (ctx->w >> 19) ^ tmp ^ (tmp >> 8);
    
    /* Build PRF's parameters */

    p1 =  ctx->x;
    p2 =  ctx->w;

    p1 <<= 32;
    p2 <<= 32;

    p1 |= ctx->y;
    p2 |= ctx->z;


    tls_prf(ctx->prf,&p1,sizeof(u64_t),"tls(£$@^&*)",&p2,sizeof(u64_t),NULL,0,data,num);
}

/* ---------------------------------- */

static unsigned int tls_rand32(tls_rnd_t *ctx)
{
    unsigned int value;

    tls_rand8(ctx,&value,sizeof(unsigned int));

    return value;
}

/* ---------------------------------- */

static void tls_init(rand_t *rc,int prf,unsigned int seed)
{
    tls_rnd_t *ctx = (tls_rnd_t *)rc->ctx;

    rand_t *rnd = rand_start(RAND_GM,seed);
    if(rnd)
    {
        ctx->x = rand_unsigned(rnd);
        ctx->y = rand_unsigned(rnd);
        ctx->w = rand_unsigned(rnd);
        ctx->z = rand_unsigned(rnd);
        rand_end(rnd);
    }

    switch(prf)
    {
        case RAND_TLS_MD5_SHA1:
            rc->namec = "TLS-PRF(MD5-SHA1)";
            rc->namew = L"TLS-PRF(MD5-SHA1)";
            ctx->prf = prf;            
            break;

        case RAND_TLS_SHA384:
            rc->namec = "TLS-PRF(SHA384)";
            rc->namew = L"TLS-PRF(SHA384)";
            ctx->prf = prf;            
            break;

        case RAND_TLS_SHA256:
        default:
            rc->namec = "TLS-PRF(SHA256)";
            rc->namew = L"TLS-PRF(SHA256)";
            ctx->prf = RAND_TLS_SHA256;
            break;
    }
}

/* ---------------------------------- */

void rand_tls_prf(int prf,void *sec,int tam_sec,const char *label,void *s1,int tam_s1,void *s2,int tam_s2,void *ret,size_t num)
{
    if(sec && s1)
        tls_prf(prf,sec,tam_sec,label,s1,tam_s1,s2,tam_s2,ret,num);    
}

/* ----------------------------------------------------------- *
    OSSYS
 * ----------------------------------------------------------- */

/* OSSYS Context */

#define SIZE_BUF_OSSYS   512

typedef struct
{
    int              which;
    unsigned char    buf[SIZE_BUF_OSSYS + 4];
    size_t           left;
    unsigned char   *next;
} ossys_rand_t;

/* ---------------------------------------------------------------------------------------------- *

    ossys_function() is based on info from several sources depending on the implementation:

    - Linux:

            https://en.wikipedia.org/wiki/dev/random

    - Windows:

        Implementation used to be based on RtlGenRandom(), but one day I read the page:

        https://learn.microsoft.com/en-us/windows/win32/api/ntsecapi/nf-ntsecapi-rtlgenrandom

        There says that this function should not be used and instead one must use CryptGenRandom,
        but in the page:

        https://learn.microsoft.com/en-us/windows/win32/api/wincrypt/nf-wincrypt-cryptgenrandom

        Curiously says that this interface is now deprecated and the Cryptography API: Next Generation
        (CNG) should be used instead.

        Last but not least, on the page https://en.wikipedia.org/wiki/CryptGenRandom, sais the same,
        BUT it also says that all Windows interfaces use a top CSPRNG, therefore there is a bit of
        a mess on what to use in Windows for system's PRNG data.

        I decided to also support BCryptGenRandom() -which is part of the CNG- so its tried first,
        but only if the system is Windows 7 onwards, as the way we used it is not supported until
        Windows Vista with SP2.

        https://learn.microsoft.com/en-gb/windows/win32/api/bcrypt/nf-bcrypt-bcryptgenrandom

 * ---------------------------------------------------------------------------------------------- */


#ifdef FOR_WIN

#include <windows.h>

typedef NTSTATUS (WINAPI *BCryptGenRandom_func_t) (BCRYPT_ALG_HANDLE hAlgorithm,PUCHAR pbBuffer,ULONG cbBuffer,ULONG dwFlags);
typedef BOOLEAN  (WINAPI *RtlGenRandom_func_t) (PVOID RandomBuffer,ULONG RandomBufferLength);

static HMODULE crypt_dll = NULL;

static RtlGenRandom_func_t     rtl_gen_random = NULL;
static BCryptGenRandom_func_t  bcrypt_gen_random = NULL;

static int                     is_windows_7 = FALSE;

/* ----------------------------------------------------------- */

#define OSSYS_RTL_RAND      0
#define OSSYS_BCRYPT_RAND   1

static void ossys_generate(ossys_rand_t *rc)
{
    if(crypt_dll == NULL)
    {
        OSVERSIONINFOEX vi;

        vi.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);

        if(GetVersionEx((OSVERSIONINFO *)&vi))
        {
            if(vi.dwMajorVersion > 6 || (vi.dwMajorVersion == 6 && vi.dwMinorVersion > 1))
                is_windows_7 = TRUE;

        }
    }

    if(rc->which == -1)
    {
        if(crypt_dll == NULL &&  is_windows_7)
        {
            crypt_dll = LoadLibraryA("bcrypt.dll");
            if(crypt_dll)
            {
                bcrypt_gen_random = (BCryptGenRandom_func_t) GetProcAddress(crypt_dll,"BCryptGenRandom");
                if(bcrypt_gen_random == NULL)
                {
                    FreeLibrary(crypt_dll);
                    crypt_dll = NULL;
                }
            }
        }

        if(crypt_dll == NULL)
        {
            crypt_dll = LoadLibraryA("advapi32.dll");
            if(crypt_dll)
            {
                rtl_gen_random = (RtlGenRandom_func_t) GetProcAddress(crypt_dll,"RtlGenRandom");
                if(rtl_gen_random == NULL)
                {
                    FreeLibrary(crypt_dll);
                    crypt_dll = NULL;
                }
            }
        }

        if(bcrypt_gen_random)
        {
            rc->which = OSSYS_BCRYPT_RAND;            
        }
        else if(rtl_gen_random)
        {
            rc->which = OSSYS_RTL_RAND;            
        }
    }

    if(rc->which == OSSYS_RTL_RAND)
        rtl_gen_random((PVOID)rc->buf,SIZE_BUF_OSSYS);

    else if(rc->which == OSSYS_BCRYPT_RAND);
        bcrypt_gen_random(NULL,(PUCHAR)rc->buf,SIZE_BUF_OSSYS,0x00000002);
}

#else

/* ----------------------------------------------------------- */

#define OSSYS_URANDOM    0
#define OSSYS_RANDOM     1

static void ossys_generate(ossys_rand_t *rc)
{
    int rnd = 1,cnt;
    unsigned char *ptr;
    size_t lft;

    if(rc->which == -1 || rc->which == OSSYS_URANDOM)
    {
        rnd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
        if(rnd != -1)
            rc->which=OSSYS_URANDOM;
    }

    if(rnd == -1)
    {
        rnd = open("/dev/random", O_RDONLY | O_CLOEXEC);
        if(rnd != -1)
        {
            rc->which=OSSYS_RANDOM;
        }
        else
        {
            rc->which = -1;
            return;
        }
    }

    ptr  = (unsigned char *)rc->buf;
    lft = SIZE_BUF_OSSYS;
    cnt = 0;

    while(lft > 0)
    {
        cnt = read(rnd,ptr,lft);
        if(cnt < 1)
        {
            rc->which=-1;
            break;
        }
        lft -= cnt;
        ptr += cnt;
    }
    close(rnd);
}

#endif

/* -------------------------- */

static void ossys_bytes(ossys_rand_t *rc,void *data,size_t num)
{
    unsigned char *dest;
    size_t remain;
    int cnt;

    remain=num;
    dest = (unsigned char *)data;
    while(remain > 0)
    {
        if(rc->left == 0)
        {
            ossys_generate(rc);
            /*
               Although this would happen extremely rarely, 
               ossys_generate() may fail temporary if the
               system is shutting down or the user is 
               logging off, as the system random engine 
               may refuse connections.

               On these exceptional cases we will generate
               a block of unpredictable data using entropy
               while the condition goes away or our program
               its ended.
            */
            if(rc->which==-1)
                get_entropy(rc->buf,SIZE_BUF_OSSYS);
            rc->left = SIZE_BUF_OSSYS;
            rc->next = rc->buf;
        }
        cnt = (remain > rc->left) ? rc->left : (int) remain;
        memcpy(dest,rc->next,cnt);
        dest += cnt;
        rc->next += cnt;
        rc->left -= cnt;
        remain-=cnt;
    }
}

/* -------------------------- */

static unsigned int ossys_function(ossys_rand_t *rc)
{
    unsigned int ret;

    ossys_bytes(rc,&ret,sizeof(unsigned int));
    return ret;
}

/* -------------------------------- */

static int ossys_init(ossys_rand_t *rc)
{
    rc->which = -1;
    ossys_generate(rc);
    rc->left = SIZE_BUF_OSSYS;
    rc->next = rc->buf;
    return (rc->which == -1);
}

/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

/* rand test Context */

typedef struct
{
    unsigned char   *buf;
    size_t  cnt;

    size_t  left;
    unsigned char   *next;

} rand_test_t;

/* -------------------------- */

static void randtest_bytes(rand_test_t *rc,void *data,size_t num)
{
    unsigned char *dest;
    size_t remain;
    int cnt;

    remain=num;
    dest = (unsigned char *)data;
    while(remain > 0)
    {
        if(rc->left == 0)
        {
            rc->left = rc->cnt;
            rc->next = rc->buf;
        }

        cnt = (remain > rc->left) ? rc->left : (int) remain;
        memcpy(dest,rc->buf,cnt);
        dest += cnt;
        rc->next += cnt;
        rc->left -= cnt;
        remain-=cnt;
    }
}

/* -------------------------- */

static unsigned int randtest_function(rand_test_t *rc)
{
    unsigned int ret;

    if(rc->left < 4)
    {
        rc->left = rc->cnt;
        rc->next = rc->buf;
    }

    ret=(unsigned int)get_be32(rc->next);

    rc->next+=4;
    rc->left-=4;

    return ret;
}


/* -------------------------- */
/* -------------------------- */

rand_t *rand_tester(void *buf,size_t len)
{
    rand_t *rc=NULL;

    if(buf && len > 0)
    {
        rc=(rand_t *)calloc(sizeof(rand_t) + sizeof(rand_test_t),1);
        if(rc)
        {
            rand_test_t *ctx;

            rc->namec = "RandTEST";
            rc->namew = L"RandTEST";
            rc->ctx=rc->dont_touch;
            rc->tam=sizeof(rand_test_t);
            rc->func32=(rand32_func_t)randtest_function;
            rc->func8=(rand8_func_t)randtest_bytes;

            ctx =(rand_test_t *)rc->dont_touch;

            ctx->left = ctx->cnt = len;
            ctx->next = ctx->buf =(unsigned char *)buf;
        }
    }
    return rc;
}

/* -------------------------- */
/* -------------------------- */

rand_t *rand_create(size_t tam,rand32_func_t fun32,rand8_func_t fun8,rand_save_func_t fsave,rand_load_func_t fload)
{
    rand_t *rc=NULL;

    if(fun32 && fun8)
    {
        rc = (rand_t *)calloc(sizeof(rand_t) + tam,1);
        if(rc)
        {
            rc->ctx=rc->dont_touch;
            rc->tam=tam;
            rc->func32=fun32;
            rc->func8=fun8;
            rc->save=fsave;
            rc->load=fload;
        }
    }
    return rc;
}

/* -------------------------- */
/* -------------------------- */

rand_t *rand_start(int cual,unsigned int param)
{
    rand_t *rc = NULL;

    switch(cual)
    {
        case RAND_GX:
            rc = rand_create(sizeof(gx_rand_t),(rand32_func_t)xorshift128_function,(rand8_func_t)xorshift128_bytes,NULL,NULL);
            if(rc)
            {
                rc->namec = "XorShift128";
                rc->namew = L"XorShift128";

                xorshift128_init((gx_rand_t *)rc->ctx,param);
            }
            break;
        case RAND_MT:
            rc = rand_create(sizeof(mt_rand_t),(rand32_func_t)mersenne_twister_function,(rand8_func_t)mersenne_twister_bytes,NULL,NULL);
            if(rc)
            {
                rc->namec = "Mersenne-Twister";
                rc->namew = L"Mersenne-Twister";

                mersenne_twister_init((mt_rand_t *)rc->ctx,param);
            }
            break;
        case RAND_GM:
            rc = rand_create(sizeof(gm_rand_t),(rand32_func_t)mother_function,(rand8_func_t)mother_bytes,NULL,NULL);
            if(rc)
            {
                rc->namec = "Mother";
                rc->namew = L"Mother";

                mother_init((gm_rand_t *)rc->ctx,param);
            }
            break;
        case RAND_TLS_MD5_SHA1:
        case RAND_TLS_SHA256:
        case RAND_TLS_SHA384:
            rc=rand_create(sizeof(tls_rnd_t),(rand32_func_t)tls_rand32,(rand8_func_t)tls_rand8,NULL,NULL);            
            if(rc)
                tls_init(rc,cual,param);            
            break;
         case RAND_OS:
            rc = rand_create(sizeof(ossys_rand_t),(rand32_func_t)ossys_function,(rand8_func_t)ossys_bytes,NULL,NULL);
            if(rc)
            {
                rc->namec = "OSSYS";
                rc->namew = L"OSSYS";

                if(ossys_init((ossys_rand_t *)rc->ctx))
                {
                    memset(rc->ctx,0,sizeof(ossys_rand_t));
                    free(rc);
                    /* Avoid an endless loop */
                    rc = (param == RAND_OS) ? NULL : rand_start(param,0);
                }
            }
            break;
        default:
            break;
    }
    return rc;
}

/* -------------------------- */

void rand_end(rand_t *rc)
{
    if(rc)
    {
        memset(rc->ctx,0,rc->tam);
        free(rc);
    }
}

/* -------------------------- */

int rand_load_status(rand_t *rand,FILE *fp)
{
    if(rand && fp)
    {
        if(rand->load)
            return rand->load(rand->ctx,fp);
    }
    return -1;
}

/* -------------------------- */

int rand_save_status(rand_t *rand,FILE *fp)
{
    if(rand && fp)
    {
        if(rand->save)
            return rand->save(rand->ctx,fp);        
    }
    return -1;
}

/* -------------------------- */

int rand_integer(rand_t *rc)
{
    return (int)rc->func32(rc->ctx);
}

/* -------------------------- */

unsigned int rand_unsigned(rand_t *rc)
{
    return rc->func32(rc->ctx);
}

/* -------------------------- */

unsigned char rand_byte(rand_t *rc)
{
    int val=0;
    unsigned char ret;

    do {
        if(val==0)
            val = rc->func32(rc->ctx);
        ret = (unsigned char)(val & 0xff);
        val >>=8;
    } while(ret==0 || ret==0xff);

    return ret;
}

/* -------------------------- */

double rand_double(rand_t *rc)
{
    double val,dec;

    val = (double)rc->func32(rc->ctx);
    dec =(double) rc->func32(rc->ctx);
    dec/= 4294967295.0;

    return (val + dec);
}

/* -------------------------- */

double rand_decimal(rand_t *rc)
{
    double value;

    value =(double) rc->func32(rc->ctx);
    value/= 4294967295.0;
    return value;
}

/* -------------------------- */

void rand_bytes(rand_t *rc,void *data,size_t max)
{
    rc->func8(rc->ctx,data,max);
}

/* -------------------------- */

void rand_bytes_no_zeros(rand_t *rc,void *data,size_t max)
{
    size_t t;
    unsigned char *ptr=(unsigned char *)data;

    rc->func8(rc->ctx,data,max);
    for(t=0;t<max;t++)
    {
        while(!ptr[t])
            rc->func8(rc->ctx,&(ptr[t]),1);
    }
}

/* -------------------------- */

u64_t rand_u64(rand_t *rc)
{
    unsigned char tmp[8];

    rc->func8(rc->ctx,tmp,8);
    return get_be64(tmp);
}

/* -------------------------- */

void rand_bits(rand_t *rc,void *ret,int bits)
{
    unsigned char *buf = (unsigned char *)ret;

    if (buf && bits > 0)    
    {
        int cnt = (bits + 7) / 8;
        
        rc->func8(rc->ctx,buf,cnt);

        cnt *= 8;
        cnt -= bits;

       *buf |= (0x80 >> cnt);                 
       *buf &= (0xff >> cnt);
    }
}

/* ------------------------------------------------------------------------------------------- *
    https://github.com/firebase/firebase-js-sdk/blob/main/packages/firestore/src/util/misc.ts
 * ------------------------------------------------------------------------------------------- */

static void math_dot_random(char *buf)
{
    double value;
    int t;
    unsigned int pos;
    static gx_rand_t ctx = {0,0,0,0};
    char chars[63]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    if(!ctx.w)
        xorshift128_init(&ctx,unpredictable_seed_non_linear());

    for(t=0;t<20;t++)
    {
        value =(double) xorshift128_function(&ctx);
        value/= 4294967295.0;

        pos = (unsigned int)(value * 62.0);

       *buf++ = chars[pos % 62];
    }
}

/* -------------------------------- */

static void build_firestore_auto_id(void *buf,size_t max,int wide)
{
    char id[20];
    size_t pos=0,cnt=20;
    char *ch;
    wchar_t *wc;

    if(max > 0)
    {
        max--;
        ch = (char *)buf;
        wc = (wchar_t *)buf;
        while(pos < max)
        {
            if(cnt == 20)
            {
                math_dot_random(id);
                cnt = 0;
            }
            if(wide)
                wc[pos++]=id[cnt++];
            else
                ch[pos++]=id[cnt++];
        }
        if(wide)
            wc[pos]=0;
        else
            ch[pos]=0;
    }
}

/* -------------------------------- */

char *firestore_auto_id(char *string,size_t max)
{
    if(string)
        build_firestore_auto_id(string,max,FALSE);
    return string;
}

/* -------------------------------- */

wchar_t *firestore_auto_idw(wchar_t *string,size_t max)
{
    if(string)
        build_firestore_auto_id(string,max,TRUE);
    return string;
}

/* ------------------------------- */
/* ------------------------------- */

void hash_get_entropy(int hash,void *dest,size_t num)
{
    unsigned char *buf,tmp[MAX_HASH_SIZE];
    size_t cnt = 0;
    int left = 0;
    hash_t ctx;
    
    if(hash > HASH_NONE && hash < HASH_NUM_HASHES)
    {
        buf  = (unsigned char *)dest;        
        while(cnt < num)
        {
            if(left == 0)
            {                
                get_entropy(tmp,MAX_HASH_SIZE);
                left = hash_init(&ctx,hash);                
                hash_update(&ctx,tmp,MAX_HASH_SIZE);
                hash_final(&ctx,tmp);                
            }
            buf[cnt++] = tmp[--left];
        }
    }
}
