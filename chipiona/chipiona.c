/*
    chipiona.c

    Chipiona Cipher

    (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Designed and written by Guillermo Amodeo Ojeda.

        - This is a very simple stream cipher where the actual data to encode is 
          used to generate the key to encode it. 
          
        - The cipher is not modelled after a mathematical function like Feistel 
          or Galois, instead uses the SHAX principle: Substitute, Hash And Xor              
            
        - Chipiona is small. It takes around 10 Kbytes in code, and around 30 Kbytes
          of memory. None of this numbers is big enough to discorage its use in small
          systems like routers or bridges.

        - DO NOT FORGET that you can build another SHAX cipher by using a different 
          pseudo-random function and hash algorithm e.g, Mersene-Twister and SHA256.
          
            
                                         --oO0Oo--
*/

#include "chipiona.h"

/* -------------------------------- *
   MOTHER pseudo-random function
 * -------------------------------- */

static unsigned int chipiona_mother_function(unsigned short int *mtop,unsigned short int *mbot)
{
    static const short int top_linear[8]={1941,1860,1812,1776,1492,1215,1066,12013};
    static const short int bot_linear[8]={1111,2222,3333,4444,5555,6666,7777,9272};
    unsigned int top,bot,tmp;
    int t,i;

    top=(unsigned int)mtop[0];
    bot=(unsigned int)mbot[0];

    memmove(mtop+2, mtop+1, 8*sizeof(unsigned short int));
    memmove(mbot+2, mbot+1, 8*sizeof(unsigned short int));

    for(i=0,t=2;t<10;t++,i++)
    {
        tmp=top_linear[i];
        tmp*=mtop[t];
        top+=tmp;
        tmp=bot_linear[i];
        tmp*=mbot[t];
        bot+=tmp;
    }

    mtop[0]=(unsigned short int)(top >> 16);
    mbot[0]=(unsigned short int)(bot >> 16);

    mtop[1]=(unsigned short int)(top & 0xFFFFU);
    mbot[1]=(unsigned short int)(bot & 0xFFFFU);

    return (unsigned int)(((top & 0xFFFFU) << 16) | (bot & 0xFFFFU));
}

/* -------------------------------- */

static void chipiona_mother_generate(unsigned int seed,unsigned int *sbox,int len)
{
    unsigned int dn,tn,t;
    unsigned short int sn,top[10],bot[10];  
    
    /* Let's init mother with that seed */

    dn=(unsigned int)(seed & 0x7FFFFFFFU);
    sn=(unsigned short int)(seed & 0xFFFFU);

    for(t=0;t<9;t++)
    {
        dn>>=16;
        tn=30903;
        tn*=sn;
        tn+=dn;
        sn=(unsigned short int)(tn & 0xFFFFU);
        top[t]=sn;
        dn=tn;
    }

    for(t=0;t<9;t++)
    {
        dn>>=16;
        tn=30903;
        tn*=sn;
        tn+=dn;
        sn=(unsigned short int)(tn & 0xFFFFU);
        bot[t]=sn;
        dn=tn;
    }

    top[0]&=0x7FFF;
    bot[0]&=0x7FFF;

    /* Now lets generate the SBOX  */

    for(t=0;t<len;t++)
    {   
        do {     
            sbox[t] = chipiona_mother_function(top,bot);
        } while(!sbox[t]);        
    }    

    memset(top,0,sizeof(top));
    memset(bot,0,sizeof(bot));
}

/* ----------------------------------------------------------- *
   MERSENNE TWISTER
 * ----------------------------------------------------------- */

#define MT_N     624
#define MT_M     397
#define MT_A     0x9908b0dfU
#define MT_U     0x80000000U
#define MT_L     0x7fffffffU

typedef struct
{
    unsigned int    x[MT_N];
    unsigned int    next;
} mt_rand_t;

/* -------------------------------- */

static unsigned int chipiona_mersenne_twister_function(mt_rand_t *rc)
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

/* -------------------------------- */

static void chipiona_mersenne_twister_generate(unsigned int seed,unsigned int *sbox,int len)
{
    mt_rand_t rc;
    int i;

    rc.next=0;

    rc.x[0] = (seed & 0xffffffff);

    for ( i = 1; i < MT_N; i++ )
    {
        rc.x[i] = ( 1812433253 * ( rc.x[i - 1] ^ ( rc.x[i - 1] >> 30 ) ) + i );
        rc.x[i] &= 0xffffffff;
    }

    /* Now lets generate the SBOX  */

    for(i=0;i<len;i++)
    {   
        do {     
            sbox[i] = chipiona_mersenne_twister_function(&rc);
        } while(!sbox[i]);        
    }   

    memset(&rc,0,sizeof(rc));
}

/* ----------------------------------------------------------- *
    XORSHIFT 128
 * ----------------------------------------------------------- */

typedef struct
{
    unsigned int x, y, z, w;
} gx_rand_t;

/* -------------------------------- */

static unsigned int chipiona_xorshift128_function(gx_rand_t *rc)
{
    unsigned int tmp;

    tmp = rc->x ^ (rc->x << 11);

    rc->x = rc->y;
    rc->y = rc->z;
    rc->z = rc->w;

    rc->w = rc->w ^ (rc->w >> 19) ^ tmp ^ (tmp >> 8);

    return rc->w;
}


/* -------------------------------- */

static void chipiona_xorshift128_generate(unsigned int seed,unsigned int *sbox,int len)
{
    gx_rand_t rc;
    int i;
    unsigned int tmp[4];

    /* Use mother for initial values */

    chipiona_mother_generate(seed,tmp,4);
    
    rc.x = tmp[0];
    rc.y = tmp[1];
    rc.z = tmp[2];
    rc.w = tmp[3];

    /* Now lets generate the SBOX  */

    for(i=0;i<len;i++)
    {   
        do {     
            sbox[i] = chipiona_xorshift128_function(&rc);
        } while(!sbox[i]);        
    }   

    memset(tmp,0,sizeof(tmp));
    memset(&rc,0,sizeof(rc));
}

/* -------------------------------- *
   Generate S-BOXES for the key
 * -------------------------------- */


static void chipiona_create_sboxes(chipiona_t *ctx,const unsigned char *key)
{
    unsigned int seed,t,i;
    
    for(i=0;i<ctx->snum;i++)
    {
        /* Take 4 bytes of key as seed */

        seed = *key++;
        for(t=1; t<4; t++)
        {
            seed <<= 8;
            seed |= *key++;
        }

        ctx->prg[i] = (seed % 3);        

        switch(ctx->prg[i])
        {

            case CHIPIONA_PRG_MERSENNE:
                chipiona_mersenne_twister_generate(seed,ctx->sboxes[i],256);
                break;
            case CHIPIONA_PRG_XORSHIFT:
                chipiona_xorshift128_generate(seed,ctx->sboxes[i],256);
                break;
            case CHIPIONA_PRG_MOTHER:
            default:
                chipiona_mother_generate(seed,ctx->sboxes[i],256);
                break;
        }
    }


}


/* -------------------------------- *
   MD5 Constants 
 * -------------------------------- */

#define S11      7
#define S12     12
#define S13     17
#define S14     22
#define S21      5
#define S22      9
#define S23     14
#define S24     20
#define S31      4
#define S32     11
#define S33     16
#define S34     23
#define S41      6
#define S42     10
#define S43     15
#define S44     21

/* -------------------------------- *
   MD5 Operations & Rounds
 * -------------------------------- */

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define R(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac) { (a) += F ((b), (c), (d)) + (x) + (unsigned int)(ac); (a) = R((a), (s)); (a) += (b); }
#define GG(a, b, c, d, x, s, ac) { (a) += G ((b), (c), (d)) + (x) + (unsigned int)(ac); (a) = R((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) { (a) += H ((b), (c), (d)) + (x) + (unsigned int)(ac); (a) = R((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) { (a) += I ((b), (c), (d)) + (x) + (unsigned int)(ac); (a) = R((a), (s)); (a) += (b); }

/* -------------------------------- *
   MD5 Transformation function 
 * -------------------------------- */

static void chipiona_md5_function(chipiona_t *ctx)
{
    unsigned int a,b,c,d,x[16];   
    int w,i;
    
    for(w=i=0;w<16;w++)
    {
        x[w] = ctx->sboxes[i][ ctx->data[w] ]; 
        if(++i==ctx->snum)
            i=0;
    }

    /* read status */

    a=ctx->state[0];
    b=ctx->state[1];
    c=ctx->state[2];
    d=ctx->state[3];

    /* Round 1 */

    FF (a, b, c, d, x[ 0], S11, 0xd76aa478);
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
    FF (c, d, a, b, x[ 2], S13, 0x242070db);
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
    FF (c, d, a, b, x[ 6], S13, 0xa8304613);
    FF (b, c, d, a, x[ 7], S14, 0xfd469501);
    FF (a, b, c, d, x[ 8], S11, 0x698098d8);
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
    FF (c, d, a, b, x[10], S13, 0xffff5bb1);
    FF (b, c, d, a, x[11], S14, 0x895cd7be);
    FF (a, b, c, d, x[12], S11, 0x6b901122);
    FF (d, a, b, c, x[13], S12, 0xfd987193);
    FF (c, d, a, b, x[14], S13, 0xa679438e);
    FF (b, c, d, a, x[15], S14, 0x49b40821);

    /* Round 2 */

    GG (a, b, c, d, x[ 1], S21, 0xf61e2562);
    GG (d, a, b, c, x[ 6], S22, 0xc040b340);
    GG (c, d, a, b, x[11], S23, 0x265e5a51);
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
    GG (d, a, b, c, x[10], S22,  0x2441453);
    GG (c, d, a, b, x[15], S23, 0xd8a1e681);
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
    GG (d, a, b, c, x[14], S22, 0xc33707d6);
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
    GG (a, b, c, d, x[13], S21, 0xa9e3e905);
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);

    /* Round 3 */

    HH (a, b, c, d, x[ 5], S31, 0xfffa3942);
    HH (d, a, b, c, x[ 8], S32, 0x8771f681);
    HH (c, d, a, b, x[11], S33, 0x6d9d6122);
    HH (b, c, d, a, x[14], S34, 0xfde5380c);
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
    HH (b, c, d, a, x[10], S34, 0xbebfbc70);
    HH (a, b, c, d, x[13], S31, 0x289b7ec6);
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
    HH (b, c, d, a, x[ 6], S34,  0x4881d05);
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
    HH (d, a, b, c, x[12], S32, 0xe6db99e5);
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665);

    /* Round 4 */

    II (a, b, c, d, x[ 0], S41, 0xf4292244);
    II (d, a, b, c, x[ 7], S42, 0x432aff97);
    II (c, d, a, b, x[14], S43, 0xab9423a7);
    II (b, c, d, a, x[ 5], S44, 0xfc93a039);
    II (a, b, c, d, x[12], S41, 0x655b59c3);
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
    II (c, d, a, b, x[10], S43, 0xffeff47d);
    II (b, c, d, a, x[ 1], S44, 0x85845dd1);
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II (c, d, a, b, x[ 6], S43, 0xa3014314);
    II (b, c, d, a, x[13], S44, 0x4e0811a1);
    II (a, b, c, d, x[ 4], S41, 0xf7537e82);
    II (d, a, b, c, x[11], S42, 0xbd3af235);
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
    II (b, c, d, a, x[ 9], S44, 0xeb86d391);

    /* update status */

    ctx->state[0]+=a;
    ctx->state[1]+=b;
    ctx->state[2]+=c;
    ctx->state[3]+=d;
}

/* ------------------------------- */

static void chipiona_block_function(chipiona_t *ctx,const void *buf, size_t tam)
{
	unsigned char *data = (unsigned char *)buf;
    size_t t, i, bits;

    /* Add data to the MD5 buffer */

    for(t=0;t<tam;t++)
    {
        ctx->data[ctx->pos++]=data[t];
        if(ctx->pos==16)
        {
            chipiona_md5_function(ctx);
            ctx->pos=0;
        }
        ctx->cbits += 32;
    }

    /* Now add the bit counter MD5 */    

    bits = ctx->cbits;
    while(bits)
    {
        ctx->data[ctx->pos++] = (unsigned char)(bits & 0xff);
        if(ctx->pos == 16)
        {
            chipiona_md5_function(ctx);
            ctx->pos=0;
        }
        bits >>= 8;
        ctx->cbits += 32;
    }

    /* Build the current key */

    data = ctx->curkey;
    for(t=0;t<4;t++)
    {
        bits = ctx->state[t];

        for(i=0;i<4;i++)
        {       
            data[i] = (unsigned char) (bits & 0xff);
            bits >>= 8;
        }
        data+=4;
    }    
}

/* ------------------------------- */
/* ------------------------------- */

int chipiona_init(chipiona_t *ctx, const void *key,size_t len)
{
    unsigned int ns;   

    if(ctx && key && len >= CHIPIONA_KEY_SIZE)
    {              
        memset(ctx,0,sizeof(chipiona_t));

        for(ns=0; ns < 16; ns++)
        {
            ctx->sboxes[ns] = (unsigned int *)calloc(sizeof(unsigned int),256);
            if(!ctx->sboxes[ns])
                break;            
        }

        if(ns == 16)
        {            
            /* Nothing-up-my-sleeve values */

            ctx->state[0] = 0x67452301;
	        ctx->state[1] = 0xefcdab89;
	        ctx->state[2] = 0x98badcfe;
	        ctx->state[3] = 0x10325476;

            ctx->snum = 16;   /* First block always uses the 16 s-boxes */
            
            chipiona_create_sboxes(ctx,(const unsigned char *)key); 
            chipiona_block_function(ctx,key,len); 

            return 0;
        }
        
        while(ns > 0)
        {
            if(ctx->sboxes[--ns])
            {
                free(ctx->sboxes[ns]);
                ctx->sboxes[ns] = NULL;
            }
        }            

        memset(ctx,0,sizeof(chipiona_t));

        return -2;
    }
    return -1;
}

/* ------------------------------- */

void chipiona_end(chipiona_t *ctx)
{
    unsigned int i;

    if(ctx)
    {
        for(i=0; i < 16; i++)
        {
            if(ctx->sboxes[i])
            {
                memset(ctx->sboxes[i],0,sizeof(unsigned int) * 256);
                free(ctx->sboxes[i]);                
            }
        }            
        memset(ctx,0,sizeof(chipiona_t));
    }
}

/* ------------------------------- */

void chipiona_encode(chipiona_t *ctx,void *dst,const void *src, size_t len)
{
    unsigned char *orig,*dest,tmp[20];
    size_t quedan,t;
    unsigned int snum[3] = {16,8,4};

    orig = (unsigned char *)src;
    dest = (unsigned char *)dst;

    while(len) 
    {
        quedan = (len > 16) ? 16 : len;
        
        for(t=0;t<quedan;t++)
            tmp[t] = (ctx->curkey[t] ^ orig[t]);
        
        chipiona_block_function(ctx,orig,quedan);

        ctx->snum = snum [ orig[ quedan - 1] % 3 ];  

        memcpy(dest,tmp,quedan);

        dest += quedan;
        orig += quedan;
        len  -= quedan;
    }
}

/* ------------------------------- */

void chipiona_decode(chipiona_t *ctx,void *dst,const void *src, size_t len)
{
	unsigned char *orig,*dest;
    size_t quedan,t;
    unsigned int snum[3] = {16,8,4};

    orig = (unsigned char *)src;
    dest = (unsigned char *)dst;

    while(len) 
    {
        quedan = (len > 16) ? 16 : len;
        
        for(t=0;t<quedan;t++)
            dest[t] = (ctx->curkey[t] ^ orig[t]);
        
        chipiona_block_function(ctx,dest,quedan);

        ctx->snum = snum [ dest[ quedan - 1] % 3 ];  
        
        
        dest += quedan;
        orig += quedan;
        len  -= quedan;
    }
}

/* ------------------------------- *
    Function to debug the cipher
 * ------------------------------- */

 const char *rand_name[3]={"Marsaglia's Mother","Mersenne Twister","Marsaglia's Xorshift128"};

void chipiona_dump_data(chipiona_t *ctx,FILE *fp,int show_sboxes)
{
    unsigned int i,t,w;

    fprintf(fp,"--- chipiona_dump_status() START ----\n");
    if(ctx)
    {
        fprintf(fp,"MEMORY USAGE: %lu bytes\n",(unsigned long) (((( sizeof(chipiona_t) + (256 * sizeof(unsigned int) * 16)) + 8191) * 4096) / 4096 ));
        if(show_sboxes)
        {            
            fprintf(fp,"sboxes[%u] =\n{\n",ctx->snum);
            for(i=0; i < ctx->snum; i++)
            {            
                fprintf(fp,"    {   /* Generated using %s */\n        ",rand_name[ctx->prg[i]]);
                for(t=w=0;t<256;t++)
                {
                    fprintf(fp,"0x%08x,",ctx->sboxes[i][t]);                
                    if(++w==16)
                    {
                        w=0;
                        fprintf(fp,"\n");
                        if(t < 255)
                            fprintf(fp,"        ");
                    }
                }
                fprintf(fp,"    },\n");            
            }            
            fprintf(fp,"}\n");
        }                
        fprintf(fp,"cbits = %lu\n",ctx->cbits);
        fprintf(fp,"data[%d] = [",ctx->pos);
        for(t=0;t<ctx->pos;t++)
            fprintf(fp,"%02x",ctx->data[t]);
        fprintf(fp,"]\n");
        fprintf(fp,"state[4] = [0x%08x,0x%08x,0x%08x,0x%08x]\n",ctx->state[0],ctx->state[1],ctx->state[2],ctx->state[3]);

        fprintf(fp,"curkey[16] = [");
        for(t=0;t<16;t++)
            fprintf(fp,"%02x",ctx->curkey[t]);
        fprintf(fp,"]\n");
    }
    else 
    {
        fprintf(fp,"ctx is NULL\n");
    }
    fprintf(fp,"--- chipiona_dump_status() END ----\n");
}