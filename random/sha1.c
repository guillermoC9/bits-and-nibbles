/*
    sha1.c

    SHA-1 Hashing functions

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

        - Written by Guillermo Amodeo Ojeda using some code from SSHSHA.C and
          SSH.H that came with PUTTY 0.60

                             PUTTY's LICENSE

        PuTTY is copyright 1997-2007 Simon Tatham.

        Portions copyright Robert de Bath, Joris van Rantwijk, Delian
        Delchev, Andreas Schultz, Jeroen Massar, Wez Furlong, Nicolas Barry,
        Justin Bradford, Ben Harris, Malcolm Smith, Ahmad Khalifa, Markus
        Kuhn, Colin Watson, Christopher Staite, and CORE SDI S.A.

        Permission is hereby granted, free of charge, to any person
        obtaining a copy of this software and associated documentation files
        (the "Software"), to deal in the Software without restriction,
        including without limitation the rights to use, copy, modify, merge,
        publish, distribute, sublicense, and/or sell copies of the Software,
        and to permit persons to whom the Software is furnished to do so,
        subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
        EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
        MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
        NONINFRINGEMENT.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
        FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
        CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
        WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

                                --oO0Oo--
*/


#include "sha1.h"

#define rol(x,y) ( ((x) << (y)) | (((unsigned int)x) >> (32-y)) )

void SHA1_Transform(sha1_t *ctx)
{
    unsigned int w[80];
    unsigned int a, b, c, d, e,tmp;
    int t;
    unsigned char *ptr;

    /* Empaquetar el buffer en big endian */

    ptr=ctx->buffer;
    for(t=0;t<16;t++)
    {
        w[t]=get_be32(ptr);
        ptr+=4;
    }

    for (t = 16; t < 80; t++)
    {
	    tmp = w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16];
    	w[t] = rol(tmp, 1);
    }

    a = ctx->estado[0];
    b = ctx->estado[1];
    c = ctx->estado[2];
    d = ctx->estado[3];
    e = ctx->estado[4];

    for (t = 0; t < 20; t++)
    {
	    tmp =rol(a, 5) + ((b & c) | (d & ~b)) + e + w[t] + 0x5a827999;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }

    for (t = 20; t < 40; t++)
    {
	    tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0x6ed9eba1;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }

    for (t = 40; t < 60; t++)
    {
	     tmp = rol(a,5) + ((b & c) | (b & d) | (c & d)) + e + w[t] + 0x8f1bbcdc;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }

    for (t = 60; t < 80; t++)
    {
	     tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0xca62c1d6;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }

    ctx->estado[0] += a;
    ctx->estado[1] += b;
    ctx->estado[2] += c;
    ctx->estado[3] += d;
    ctx->estado[4] += e;
}


/* ------------------------------- */

void sha1_init(sha1_t *ctx)
{
    memset(ctx,0,sizeof(sha1_t));

    ctx->estado[0] = 0x67452301;
    ctx->estado[1] = 0xEFCDAB89;
    ctx->estado[2] = 0x98BADCFE;
    ctx->estado[3] = 0x10325476;
    ctx->estado[4] = 0xC3D2E1F0;
}

/* ------------------------------- */

void sha1_update(sha1_t *ctx,const void *buf, size_t tam)
{
    unsigned char *datos = (unsigned char *)buf;
    size_t t;

    /* Actualizar buffer y ejecutar la función si es necesario */

    for(t=0;t<tam;t++)
    {
        ctx->buffer[ctx->pos++]=datos[t];
        if(ctx->pos==64)
        {
            SHA1_Transform(ctx);
            ctx->pos=0;
        }

        /* Actualizar el contador de bits */

        ctx->bitcount+=8;
    }
}


/* ------------------------------- */

void sha1_final(sha1_t *ctx,void *h)
{
    int quedan;
    unsigned char *hash=(unsigned char *)h;

    /* Primero añadimos el padding */

    ctx->buffer[ctx->pos++]=0x80;

    quedan = 64 - ctx->pos;
    if(quedan < 8)
    {
        memset(ctx->buffer + ctx->pos,0,quedan);
        SHA1_Transform(ctx);
        ctx->pos=0;
        quedan = 64;
    }

    memset(ctx->buffer + ctx->pos,0,quedan - 8);

    /* Ahora el contador de bits */

    put_be64(ctx->buffer + 56,ctx->bitcount);

    /* Lo pasamos por la Turmix */

    SHA1_Transform(ctx);

    /* Copiamos el hash */

    put_be32(hash,ctx->estado[0]);
    put_be32(hash + 4,ctx->estado[1]);
    put_be32(hash + 8,ctx->estado[2]);
    put_be32(hash + 12,ctx->estado[3]);
    put_be32(hash + 16,ctx->estado[4]);

    /* Y borramos la evidencia */

	memset(ctx, 0,sizeof(sha1_t));
}

/* ------------------------------- */

void sha1_partial(sha1_t *ctx,void *hash)
{
    sha1_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha1_final(&tmp,hash);
}





