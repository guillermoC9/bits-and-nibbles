/*
    md4.c

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

        - Written by Guillermo Amodeo Ojeda using the info in RFC-1320 and wikipedia.

                                --oO0Oo--

*/

#include "md4.h"

/* Constants */

#define S11     3
#define S12     7
#define S13    11
#define S14    19
#define S21     3
#define S22     5
#define S23     9
#define S24    13
#define S31     3
#define S32     9
#define S33    11
#define S34    15

/* Operations */

#define F(x,y,z) (((x)&(y))|((~(x))&(z)))
#define G(x,y,z) (((x)&(y))|((x)&(z))|((y)&(z)))
#define H(x,y,z) ((x)^(y)^(z))
#define R(x,n)   (((x)<<(n))|((x)>>(32-(n))))

/* Rounds */

#define FF(a, b, c, d, x, s) { (a)+=F((b),(c),(d))+(x); (a) = R((a),(s)); }
#define GG(a, b, c, d, x, s) { (a)+=G((b),(c),(d))+(x)+((unsigned int)0x5a827999); (a)=R((a),(s)); }
#define HH(a, b, c, d, x, s) { (a)+=H((b),(c),(d))+(x)+((unsigned int)0x6ed9eba1); (a)=R((a),(s)); }

/* MD4 function */

static void md4_function(md4_t *ctx)
{
    unsigned int a,b,c,d,x[16];
    unsigned char *ptr;
    int t;

    /* pack buffer in little endian */

    ptr=ctx->data;
    for(t=0;t<16;t++)
    {
        x[t]=get_le32(ptr);
        ptr+=4;
    }

    /* read status */

    a=ctx->state[0];
    b=ctx->state[1];
    c=ctx->state[2];
    d=ctx->state[3];

    /* Round 1 */

    FF (a, b, c, d, x[ 0], S11);
    FF (d, a, b, c, x[ 1], S12);
    FF (c, d, a, b, x[ 2], S13);
    FF (b, c, d, a, x[ 3], S14);
    FF (a, b, c, d, x[ 4], S11);
    FF (d, a, b, c, x[ 5], S12);
    FF (c, d, a, b, x[ 6], S13);
    FF (b, c, d, a, x[ 7], S14);
    FF (a, b, c, d, x[ 8], S11);
    FF (d, a, b, c, x[ 9], S12);
    FF (c, d, a, b, x[10], S13);
    FF (b, c, d, a, x[11], S14);
    FF (a, b, c, d, x[12], S11);
    FF (d, a, b, c, x[13], S12);
    FF (c, d, a, b, x[14], S13);
    FF (b, c, d, a, x[15], S14);

    /* Round 2 */

    GG (a, b, c, d, x[ 0], S21);
    GG (d, a, b, c, x[ 4], S22);
    GG (c, d, a, b, x[ 8], S23);
    GG (b, c, d, a, x[12], S24);
    GG (a, b, c, d, x[ 1], S21);
    GG (d, a, b, c, x[ 5], S22);
    GG (c, d, a, b, x[ 9], S23);
    GG (b, c, d, a, x[13], S24);
    GG (a, b, c, d, x[ 2], S21);
    GG (d, a, b, c, x[ 6], S22);
    GG (c, d, a, b, x[10], S23);
    GG (b, c, d, a, x[14], S24);
    GG (a, b, c, d, x[ 3], S21);
    GG (d, a, b, c, x[ 7], S22);
    GG (c, d, a, b, x[11], S23);
    GG (b, c, d, a, x[15], S24);

    /* Round 3 */

    HH (a, b, c, d, x[ 0], S31);
    HH (d, a, b, c, x[ 8], S32);
    HH (c, d, a, b, x[ 4], S33);
    HH (b, c, d, a, x[12], S34);
    HH (a, b, c, d, x[ 2], S31);
    HH (d, a, b, c, x[10], S32);
    HH (c, d, a, b, x[ 6], S33);
    HH (b, c, d, a, x[14], S34);
    HH (a, b, c, d, x[ 1], S31);
    HH (d, a, b, c, x[ 9], S32);
    HH (c, d, a, b, x[ 5], S33);
    HH (b, c, d, a, x[13], S34);
    HH (a, b, c, d, x[ 3], S31);
    HH (d, a, b, c, x[11], S32);
    HH (c, d, a, b, x[ 7], S33);
    HH (b, c, d, a, x[15], S34);

    /* Update status */

    ctx->state[0]+=a;
    ctx->state[1]+=b;
    ctx->state[2]+=c;
    ctx->state[3]+=d;
}




/* ------------------------------- */

void md4_init(md4_t *ctx)
{
    memset(ctx,0,sizeof(md4_t));

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
}

/* ------------------------------- */

void md4_update(md4_t *ctx,const void *buf, size_t tam)
{
    unsigned char *data = (unsigned char *)buf;
    size_t t;

    for(t=0;t<tam;t++)
    {
        ctx->data[ctx->pos++]=data[t];
        if(ctx->pos==64)
        {
            md4_function(ctx);
            ctx->pos=0;
        }

        ctx->count+=8;
    }
}

/* ------------------------------- */

void md4_final(md4_t *ctx,void *h)
{
    int remain;
    unsigned char *hash=(unsigned char *)h;

    /* Add padding */

    ctx->data[ctx->pos++]=0x80;

    remain = 64 - ctx->pos;
    if(remain < 8)
    {
        /* If padding is smaller than 8 bytes
           we first add 0, then process the buffer
           and add a full padding buffer
        */
        memset(ctx->data + ctx->pos,0,remain);
        md4_function(ctx);
        ctx->pos=0;
        remain = 64;
    }

    memset(ctx->data + ctx->pos,0,remain - 8);

    /* bits counter */

    put_le64(ctx->data + 56,ctx->count);

    /* process */

    md4_function(ctx);

    /* hash */

    put_le32(hash,ctx->state[0]);
    put_le32(hash + 4,ctx->state[1]);
    put_le32(hash + 8,ctx->state[2]);
    put_le32(hash + 12,ctx->state[3]);

    /* cleanup */

	memset(ctx, 0,sizeof(md4_t));
}

/* ------------------------------- */

void md4_partial(md4_t *ctx,void *hash)
{
    md4_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    md4_final(&tmp,hash);
}







