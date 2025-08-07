/*
    rc4.c

    RC4 Stream Cypher

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

        - Written by Guillermo Amodeo Ojeda using the info from:

            http://en.wikipedia.org/wiki/RC4.

                                --oO0Oo--

*/


#include "rc4.h"

/* ---------------------------- *

    Wikipedia pseudo-code for the initialization

        for i = 0 ... 255
            S[i] = i

        for i = 0 ... 255
            j = (j + S[i] + key[i mod key_length]) mod 256
            swap (S[i],S[j])

        i = 0
        j = 0

 * ---------------------------- */


int rc4_init(rc4_t *ctx,int bits,const void *key)
{
    int i,j,l;
    unsigned char *k,T;

    if(ctx && key && bits > 8)
    {
        ctx->i = 0;
        ctx->j = 0;

        l = (bits / 8);
        k = (unsigned char *)key;

        for (i=0;i<256;i++)
            ctx->S[i]=i;

        for (j=i=0;i<256;i++)
        {
            j=(j + ctx->S[i] + k[ i % l]) % 256;
            T = ctx->S[i];
            ctx->S[i] = ctx->S[j];
            ctx->S[j] = T;
        }
        return 0;
    }
    return -1; 
}

/* ---------------------------- *

    Wikipedia pseudo-code for
    the function


    loop until the entire message is encrypted/decrypted
        i = (i + 1) mod 256
        j = (j + S[i]) mod 256
        swap(S[i],S[j])
        k = S[(S[i] + S[j]) mod 256]
        output the XOR of k with the next byte of input

 * ---------------------------- */

void rc4(rc4_t *ctx,void *dest,const void *orig,size_t tam)
{
    unsigned char *o,*d,k,T;
    size_t t;
    register int i,j;

    i = ctx->i;
    j = ctx->j;

    d = (unsigned char *)dest;
    o = (unsigned char *)orig;

    for(t=0;t<tam;t++)
    {
        i=( i + 1 ) % 256;
        j=( j + ctx->S[i]) % 256;

        T = ctx->S[i];
        ctx->S[i] = ctx->S[j];
        ctx->S[j] = T;

        k=ctx->S[(ctx->S[i] + ctx->S[j]) % 256];
        d[t]=(k ^ o[t]);
    }

    ctx->i=i;
    ctx->j=j;
}

