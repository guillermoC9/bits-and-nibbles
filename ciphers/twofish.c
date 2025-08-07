/*
    twofish.c

    TWOFISH Encryption Routines

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

        - Written by Guillermo Amodeo Ojeda using info and code from 
          Bruce Schneier, Niels Ferguson and Dr B R Gladman's twofish.c, 
          which was part of the original submission  to the AES contest, 
          which can be found here: 

            https://embeddedsw.net/Cipher_Reference_Home.html#TWOFISH
            https://www.schneier.com/academic/twofish/

          But the link to Neils' work is now broken, although I found a 
          copy in the Debian sources at:

            https://sources.debian.org/src/twofish/0.3-5/twofish.c/        
            

                                --oO0Oo--
*/


/*  
    ORIGINAL COMMENT FROM Dr B R Gladman:
*/

/* This is an independent implementation of the encryption algorithm:   */
/*                                                                      */
/*         Twofish by Bruce Schneier and colleagues                     */
/*                                                                      */
/* which is a candidate algorithm in the Advanced Encryption Standard   */
/* programme of the US National Institute of Standards and Technology.  */
/*                                                                      */
/* Copyright in this implementation is held by Dr B R Gladman but I     */
/* hereby give permission for its free direct or derivative use subject */
/* to acknowledgment of its origin and compliance with any conditions   */
/* that the originators of the algorithm place on its exploitation.     */
/*                                                                      */
/* My thanks to Doug Whiting and Niels Ferguson for comments that led   */
/* to improvements in this implementation.                              */
/*                                                                      */
/* Dr Brian Gladman (gladman@seven77.demon.co.uk) 14th January 1999     */

/* 
    IMPORTANT * IMPORTANT * IMPORTANT * IMPORTANT 

    We do not use lookup tables for m and q but use the ad-hoc code
    from Dr Gladman's original submission. This is because these days
    is slower to access a lookup table than doing a small calculation,
    although we have not benchmarked the methods, so this is just a
    sensible guess. ;-)

    We also prefixed some macros with twf_ to avoid possible conflicts 
    with previously defined macros in some other header.
*/


#include "twofish.h"

#define twf_rotr(_x,n)   ((((_x) >> ((n))) & 0xffffffff) | (((_x) << (32 - (n))) & 0xffffffff))
#define twf_rotl(_x,n)   ((((_x) << ((n))) & 0xffffffff) | (((_x) >> (32 - (n))) & 0xffffffff))

/* Extract byte from a 32 bit quantity */ 

#define twf_byte(_x,n)   ((unsigned char)((_x) >> (8 * n)))

/* ORIGINAL COMMENT FROM Dr B R Gladman:                    */
/* finite field arithmetic for GF(2**8) with the modular    */
/* polynomial x^8 + x^6 + x^5 + x^3 + 1 (0x169)             */

#define G_M 0x0169

static const unsigned char  tab_5b[4] = { 0, G_M >> 2, G_M >> 1, (G_M >> 1) ^ (G_M >> 2) };
static const unsigned char  tab_ef[4] = { 0, (G_M >> 1) ^ (G_M >> 2), G_M >> 1, G_M >> 2 };

#define ffm_01(_x)    (_x)
#define ffm_5b(_x)   ((_x) ^ ((_x) >> 2) ^ tab_5b[(_x) & 3])
#define ffm_ef(_x)   ((_x) ^ ((_x) >> 1) ^ ((_x) >> 2) ^ tab_ef[(_x) & 3])

/* ---------------------------- */

static const unsigned char ror4[16] = { 0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15 };
static const unsigned char ashx[16] = { 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12, 5, 14, 7 };

static const unsigned char qt0[2][16] = 
{
    { 8, 1, 7, 13, 6, 15, 3, 2, 0, 11, 5, 9, 14, 12, 10, 4 },
    { 2, 8, 11, 13, 15, 7, 6, 14, 3, 1, 9, 4, 0, 10, 12, 5 }
};

static const unsigned char qt1[2][16] =
{
    { 14, 12, 11, 8, 1, 2, 3, 5, 15, 4, 10, 6, 7, 0, 9, 13 }, 
    { 1, 14, 2, 11, 4, 12, 3, 7, 6, 13, 10, 5, 15, 9, 0, 8 }
};

static const unsigned char qt2[2][16] = 
{
    { 11, 10, 5, 14, 6, 13, 9, 0, 12, 8, 15, 3, 2, 4, 7, 1 },
    { 4, 12, 7, 5, 1, 6, 9, 10, 0, 14, 13, 8, 2, 11, 3, 15 }
};

static const unsigned char qt3[2][16] = 
{
    { 13, 7, 15, 4, 1, 2, 6, 14, 9, 11, 3, 0, 8, 5, 12, 10 },
    { 11, 9, 5, 1, 12, 3, 13, 14, 6, 4, 7, 15, 2, 0, 8, 10 }
};

/* ---------------------------- */

static unsigned char q(const unsigned int n, const unsigned char x)
{
    unsigned char  a0, a1, a2, a3, a4, b0, b1, b2, b3, b4;

    a0 = x >> 4; b0 = x & 15;
    a1 = a0 ^ b0; b1 = ror4[b0] ^ ashx[a0];
    a2 = qt0[n][a1]; b2 = qt1[n][b1];
    a3 = a2 ^ b2; b3 = ror4[b2] ^ ashx[a2];
    a4 = qt2[n][a3]; b4 = qt3[n][b3];
    return (b4 << 4) | a4;
}

/* ---------------------------- */

#define fm_00   ffm_01
#define fm_10   ffm_5b
#define fm_20   ffm_ef
#define fm_30   ffm_ef

#define q_0(_x)  q(1,_x)

#define fm_01   ffm_ef
#define fm_11   ffm_ef
#define fm_21   ffm_5b
#define fm_31   ffm_01

#define q_1(_x)  q(0,_x)

#define fm_02   ffm_5b
#define fm_12   ffm_ef
#define fm_22   ffm_01
#define fm_32   ffm_ef

#define q_2(_x)  q(1,_x)

#define fm_03   ffm_5b
#define fm_13   ffm_01
#define fm_23   ffm_ef
#define fm_33   ffm_5b

#define q_3(_x)  q(0,_x)

#define f_0(n,_x)    ((unsigned int)fm_0##n(_x))
#define f_1(n,_x)    ((unsigned int)fm_1##n(_x) << 8)
#define f_2(n,_x)    ((unsigned int)fm_2##n(_x) << 16)
#define f_3(n,_x)    ((unsigned int)fm_3##n(_x) << 24)


#define mds(n,_x)    f_0(n,q_##n(_x)) ^ f_1(n,q_##n(_x)) ^ f_2(n,q_##n(_x)) ^ f_3(n,q_##n(_x))

/* ---------------------------- */

static unsigned int h_fun(twof_t *ctx,const unsigned int x, const unsigned int key[])
{   
    
    unsigned int  b0, b1, b2, b3;
    unsigned int  m5b_b0, m5b_b1, m5b_b2, m5b_b3;
    unsigned int  mef_b0, mef_b1, mef_b2, mef_b3;

    b0 = twf_byte(x, 0); 
    b1 = twf_byte(x, 1); 
    b2 = twf_byte(x, 2); 
    b3 = twf_byte(x, 3);

    switch(ctx->k_cycles)
    {

        case 4: b0 = q(1, b0) ^ twf_byte(key[3],0);
                b1 = q(0, b1) ^ twf_byte(key[3],1);
                b2 = q(0, b2) ^ twf_byte(key[3],2);
                b3 = q(1, b3) ^ twf_byte(key[3],3);

        case 3: b0 = q(1, b0) ^ twf_byte(key[2],0);
                b1 = q(1, b1) ^ twf_byte(key[2],1);
                b2 = q(0, b2) ^ twf_byte(key[2],2);
                b3 = q(0, b3) ^ twf_byte(key[2],3);

        case 2: b0 = q(0,q(0,b0) ^ twf_byte(key[1],0)) ^ twf_byte(key[0],0);
                b1 = q(0,q(1,b1) ^ twf_byte(key[1],1)) ^ twf_byte(key[0],1);
                b2 = q(1,q(0,b2) ^ twf_byte(key[1],2)) ^ twf_byte(key[0],2);
                b3 = q(1,q(1,b3) ^ twf_byte(key[1],3)) ^ twf_byte(key[0],3);
    }

    b0 = q(1, b0); b1 = q(0, b1); b2 = q(1, b2); b3 = q(0, b3);

    m5b_b0 = ffm_5b(b0); m5b_b1 = ffm_5b(b1); m5b_b2 = ffm_5b(b2); m5b_b3 = ffm_5b(b3);
    mef_b0 = ffm_ef(b0); mef_b1 = ffm_ef(b1); mef_b2 = ffm_ef(b2); mef_b3 = ffm_ef(b3);

    b0 ^= mef_b1 ^ m5b_b2 ^ m5b_b3; b3 ^= m5b_b0 ^ mef_b1 ^ mef_b2;
    b2 ^= mef_b0 ^ m5b_b1 ^ mef_b3; b1 ^= mef_b0 ^ mef_b2 ^ m5b_b3;

    return b0 | (b3 << 8) | (b2 << 16) | (b1 << 24);
};

/* ---------------------------- */

#define q20(_x)  q(0,q(0,_x) ^ twf_byte(ctx->e_key[1],0)) ^ twf_byte(ctx->e_key[0],0)
#define q21(_x)  q(0,q(1,_x) ^ twf_byte(ctx->e_key[1],1)) ^ twf_byte(ctx->e_key[0],1)
#define q22(_x)  q(1,q(0,_x) ^ twf_byte(ctx->e_key[1],2)) ^ twf_byte(ctx->e_key[0],2)
#define q23(_x)  q(1,q(1,_x) ^ twf_byte(ctx->e_key[1],3)) ^ twf_byte(ctx->e_key[0],3)

#define q30(_x)  q(0,q(0,q(1, _x) ^ twf_byte(ctx->e_key[2],0)) ^ twf_byte(ctx->e_key[1],0)) ^ twf_byte(ctx->e_key[0],0)
#define q31(_x)  q(0,q(1,q(1, _x) ^ twf_byte(ctx->e_key[2],1)) ^ twf_byte(ctx->e_key[1],1)) ^ twf_byte(ctx->e_key[0],1)
#define q32(_x)  q(1,q(0,q(0, _x) ^ twf_byte(ctx->e_key[2],2)) ^ twf_byte(ctx->e_key[1],2)) ^ twf_byte(ctx->e_key[0],2)
#define q33(_x)  q(1,q(1,q(0, _x) ^ twf_byte(ctx->e_key[2],3)) ^ twf_byte(ctx->e_key[1],3)) ^ twf_byte(ctx->e_key[0],3)

#define q40(_x)  q(0,q(0,q(1, q(1, _x) ^ twf_byte(ctx->e_key[3],0)) ^ twf_byte(ctx->e_key[2],0)) ^ twf_byte(ctx->e_key[1],0)) ^ twf_byte(ctx->e_key[0],0)
#define q41(_x)  q(0,q(1,q(1, q(0, _x) ^ twf_byte(ctx->e_key[3],1)) ^ twf_byte(ctx->e_key[2],1)) ^ twf_byte(ctx->e_key[1],1)) ^ twf_byte(ctx->e_key[0],1)
#define q42(_x)  q(1,q(0,q(0, q(0, _x) ^ twf_byte(ctx->e_key[3],2)) ^ twf_byte(ctx->e_key[2],2)) ^ twf_byte(ctx->e_key[1],2)) ^ twf_byte(ctx->e_key[0],2)
#define q43(_x)  q(1,q(1,q(0, q(1, _x) ^ twf_byte(ctx->e_key[3],3)) ^ twf_byte(ctx->e_key[2],3)) ^ twf_byte(ctx->e_key[1],3)) ^ twf_byte(ctx->e_key[0],3)

/* ---------------------------- */

static void gen_keyed_sboxes(twof_t *ctx)
{
    unsigned int  i;
    unsigned char  by;

    switch(ctx->k_cycles)
    {

        case 2: for(i = 0; i < 256; ++i)
                {
                    by = (unsigned char)i;

                    ctx->s_key[0][i] = mds(0, q20(by)); 
                    ctx->s_key[1][i] = mds(1, q21(by));
                    ctx->s_key[2][i] = mds(2, q22(by)); 
                    ctx->s_key[3][i] = mds(3, q23(by));
                }
                break;

        case 3: for(i = 0; i < 256; ++i)
                {
                    by = (unsigned char)i;

                    ctx->s_key[0][i] = mds(0, q30(by)); 
                    ctx->s_key[1][i] = mds(1, q31(by));
                    ctx->s_key[2][i] = mds(2, q32(by)); 
                    ctx->s_key[3][i] = mds(3, q33(by));
                }

                break;   

        case 4: for(i = 0; i < 256; ++i)
                {
                    by = (unsigned char)i;

                    ctx->s_key[0][i] = mds(0, q40(by));
                    ctx->s_key[1][i] = mds(1, q41(by));
                    ctx->s_key[2][i] = mds(2, q42(by));
                    ctx->s_key[3][i] = mds(3, q43(by));
                }
                break;
    }
};

/* ---------------------------- */

#define g0_fun(_x) ( ctx->s_key[0][twf_byte(_x,0)] ^ ctx->s_key[1][twf_byte(_x,1)] ^ ctx->s_key[2][twf_byte(_x,2)] ^ ctx->s_key[3][twf_byte(_x,3)] )
#define g1_fun(_x) ( ctx->s_key[0][twf_byte(_x,3)] ^ ctx->s_key[1][twf_byte(_x,0)] ^ ctx->s_key[2][twf_byte(_x,1)] ^ ctx->s_key[3][twf_byte(_x,2)] )

/* ORIGINAL COMMENT FROM Dr B R Gladman: 
  
The (12,8) Reed Soloman code has the generator polynomial
  g(x) = x^4 + (a + 1/a) * x^3 + a * x^2 + (a + 1/a) * x + 1
where the coefficients are in the finite field GF(2^8) with a
modular polynomial a^8 + a^6 + a^3 + a^2 + 1. To generate the
remainder we have to start with a 12th order polynomial with our
eight input bytes as the coefficients of the 4th to 11th terms. 

That is:
  m[7] * x^11 + m[6] * x^10 ... + m[0] * x^4 + 0 * x^3 +... + 0

We then multiply the generator polynomial by m[7] * x^7 and subtract
it - xor in GF(2^8) - from the above to eliminate the x^7 term (the 
artihmetic on the coefficients is done in GF(2^8). We then multiply 
the generator polynomial by x^6 * coeff(x^10) and use this to remove
the x^10 term. We carry on in this way until the x^4 term is removed
so that we are left with:

  r[3] * x^3 + r[2] * x^2 + r[1] 8 x^1 + r[0]

which give the resulting 4 bytes of the remainder. This is equivalent 
to the matrix multiplication in the Twofish description but much faster 
to implement.

*/

#define G_MOD   0x0000014d

/* Comments in this function are from Dr B R Gladman */

unsigned int mds_rem(unsigned int p0, unsigned int p1) {
    unsigned int  i, t, u;

    for(i = 0; i < 8; ++i)
    {

        t = p1 >> 24;                   /* get most significant coefficient */
        p1 = (p1 << 8) | (p0 >> 24); 
        p0 <<= 8;                       /* shift others up */

        /* multiply t by a (the primitive element - i.e. left shift) */

        u = (t << 1); 
        if(t & 0x80)            /* subtract modular polynomial on overflow */
            u ^= G_MOD; 

        p1 ^= t ^ (u << 16);    /* remove t * (a * x^2 + 1)  */
        u ^= (t >> 1);          /* form u = a * t + t / a = t * (a + 1 / a); */
     
        if(t & 0x01)            /* add the modular polynomial on underflow */
            u ^= G_MOD >> 1;

        p1 ^= (u << 24) | (u << 8); /* remove t * (a + 1/a) * (x^3 + x) */
    }

    return p1;

};

/* ---------------------------- */

static void twofish_keys_setup(twof_t *ctx,const unsigned char *in_key)
{   
    unsigned int  i, a, b, me_key[TWOFISH_NUM_IB], mo_key[TWOFISH_NUM_IB];

    for(i = 0; i < ctx->k_cycles; ++i)
    {
        a = get_le32(in_key + (i + i) * 4);     
        me_key[i] = a;
        b = get_le32(in_key + (i + i + 1) * 4);     
        mo_key[i] = b;
        ctx->e_key[ctx->k_cycles - i - 1] = mds_rem(a, b);
    }

    for(i = 0; i < TWOFISH_MAX_SCHED; i += 2)
    {
        a = 0x01010101 * i; 
        b = a + 0x01010101;

        a = h_fun(ctx,a, me_key);
        b = twf_rotl(h_fun(ctx,b, mo_key), 8);

        ctx->b_key[i] = a + b;
        ctx->b_key[i + 1] = twf_rotl(a + 2 * b, 9);
    }

    gen_keyed_sboxes(ctx);

}

/* ---------------------------- */

#define twf_e_rnd(i)                                        \
    t1 = g1_fun(b1); t0 = g0_fun(b0);                       \
    b2 = twf_rotr(b2 ^ (t0 + t1 + ctx->b_key[4 * (i) + 8]), 1);      \
    b3 = twf_rotl(b3, 1) ^ (t0 + 2 * t1 + ctx->b_key[4 * (i) + 9]);  \
    t1 = g1_fun(b3); t0 = g0_fun(b2);                       \
    b0 = twf_rotr(b0 ^ (t0 + t1 + ctx->b_key[4 * (i) + 10]), 1);     \
    b1 = twf_rotl(b1, 1) ^ (t0 + 2 * t1 + ctx->b_key[4 * (i) + 11])


/* ---------------------------- */

static void twofish_encrypt(twof_t *ctx,unsigned char *src,unsigned char *dst)
{      
    unsigned int  t0, t1, b0,b1,b2,b3;

    b0 = get_le32(src +  0) ^ ctx->b_key[0];
    b1 = get_le32(src +  4) ^ ctx->b_key[1];
    b2 = get_le32(src +  8) ^ ctx->b_key[2];
    b3 = get_le32(src + 12) ^ ctx->b_key[3];

    twf_e_rnd(0); twf_e_rnd(1); 
    twf_e_rnd(2); twf_e_rnd(3);
    twf_e_rnd(4); twf_e_rnd(5); 
    twf_e_rnd(6); twf_e_rnd(7);

    put_le32(dst +  0, b2 ^ ctx->b_key[4]);
    put_le32(dst +  4, b3 ^ ctx->b_key[5]);
    put_le32(dst +  8, b0 ^ ctx->b_key[6]);
    put_le32(dst + 12, b1 ^ ctx->b_key[7]); 
};

/* ---------------------------- */

#define twf_d_rnd(i)                                                        \
        t1 = g1_fun(b1); t0 = g0_fun(b0);                       \
        b2 = twf_rotl(b2, 1) ^ (t0 + t1 + ctx->b_key[4 * (i) + 10]);     \
        b3 = twf_rotr(b3 ^ (t0 + 2 * t1 + ctx->b_key[4 * (i) + 11]), 1); \
        t1 = g1_fun(b3); t0 = g0_fun(b2);                       \
        b0 = twf_rotl(b0, 1) ^ (t0 + t1 + ctx->b_key[4 * (i) +  8]);     \
        b1 = twf_rotr(b1 ^ (t0 + 2 * t1 + ctx->b_key[4 * (i) +  9]), 1)

/* ---------------------------- */

static void twofish_decrypt(twof_t *ctx,unsigned char *src,unsigned char *dst)
{
    unsigned int  t0, t1, b0,b1,b2,b3;

    b0 = get_le32(src +  0) ^ ctx->b_key[4];
    b1 = get_le32(src +  4) ^ ctx->b_key[5];
    b2 = get_le32(src +  8) ^ ctx->b_key[6];
    b3 = get_le32(src + 12) ^ ctx->b_key[7];

    twf_d_rnd(7); twf_d_rnd(6); 
    twf_d_rnd(5); twf_d_rnd(4);
    twf_d_rnd(3); twf_d_rnd(2); 
    twf_d_rnd(1); twf_d_rnd(0);

    put_le32(dst +  0, b2 ^ ctx->b_key[0]);
    put_le32(dst +  4, b3 ^ ctx->b_key[1]);
    put_le32(dst +  8, b0 ^ ctx->b_key[2]);
    put_le32(dst + 12, b1 ^ ctx->b_key[3]); 
}

/* ---------------------------- */
/* ---------------------------- */

void twofish_encode_block(twof_t *ctx,void *dest,const void *orig)
{    
    twofish_encrypt(ctx,(unsigned char *)orig,(unsigned char *)dest);    
}

/* ---------------------------- */

void twofish_decode_block(twof_t *ctx,void *dest,const void *orig)
{
    twofish_decrypt(ctx,(unsigned char *)orig,(unsigned char *)dest);
}

/* ---------------------------- */

int twofish_init(twof_t *ctx,int bits,const void *key)
{    
    if(ctx && key)
    {
        memset(ctx,0,sizeof(twof_t));

        switch(bits)
        {
            case 128:
                ctx->k_cycles = 2;
                break;            
            case 192:
                ctx->k_cycles = 3;
                break;
            case 256:
                ctx->k_cycles = 4;
                break;            
            default:
                return -1;
        }

        ctx->kbits = bits;

        twofish_keys_setup(ctx,(unsigned char *)key);

        return 0;
    }
    return -1;
}
