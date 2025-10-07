/*
    rand.c

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


