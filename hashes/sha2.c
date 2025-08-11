/*
    sha2.c

    Hashing SHA-2: SHA-224, SHA-256, SHA-384 y SHA-512.

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

        - The transform functions were adapted by Guillermo Amodeo Ojeda
          from the pulic domain files LibSha256.c and LibSha512.c
          written by Tom St Denis and published by Water Juice.

                                --oO0Oo--
*/

#include "sha2.h"


/* Transformations  */

#define Ch( x, y, z )     (z ^ (x & (y ^ z)))
#define Maj( x, y, z )    (((x | y) & z) | (x & y))

#define R(x,n) 		((x) >> (n))

/* SHA-256 */

#define S32(x,n)	(((x) >> (n)) | ((x) << (32 - (n))))

#define Sigma0_256( x )     (S32(x,  2) ^ S32(x, 13) ^ S32(x, 22))
#define Sigma1_256( x )     (S32(x,  6) ^ S32(x, 11) ^ S32(x, 25))
#define Gamma0_256( x )     (S32(x,  7) ^ S32(x, 18) ^ R(x,  3))
#define Gamma1_256( x )     (S32(x, 17) ^ S32(x, 19) ^ R(x, 10))

#define Sha256Round( a, b, c, d, e, f, g, h, i )            \
     t0 = h + Sigma1_256(e) + Ch(e, f, g) + K256[i] + W[i]; \
     t1 = Sigma0_256(a) + Maj(a, b, c);                     \
     d += t0;                                               \
     h  = t0 + t1;

/* SHA-384 and SHA-512 */

#define S64(x,n)	(((x) >> (n)) | ((x) << (64 - (n))))


#define Sigma0_512( x )     (S64(x, 28) ^ S64(x, 34) ^ S64(x, 39))
#define Sigma1_512( x )     (S64(x, 14) ^ S64(x, 18) ^ S64(x, 41))
#define Gamma0_512( x )     (S64(x,  1) ^ S64(x,  8) ^ R(x, 7))
#define Gamma1_512( x )     (S64(x, 19) ^ S64(x, 61) ^ R(x, 6))

#define Sha512Round( a, b, c, d, e, f, g, h, i )            \
     t0 = h + Sigma1_512(e) + Ch(e, f, g) + K512[i] + W[i]; \
     t1 = Sigma0_512(a) + Maj(a, b, c);                     \
     d += t0;                                               \
     h  = t0 + t1;


/* K for SHA-256 */

const static unsigned int K256[64] =
{
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


/* K for SHA-384 and SHA-512 */

const static u64_t K512[80] =
{
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
	0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
	0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
	0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
	0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
	0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
	0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
	0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
	0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
	0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
	0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
	0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
	0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
	0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
	0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
	0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
	0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
	0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
	0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
	0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
	0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
	0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
	0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
	0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
	0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
	0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
	0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
	0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
	0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
	0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
	0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
	0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
	0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
	0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

/* ------------------------------- */
/* ------------------------------- */

static void SHA256_Transform(sha256_t *ctx)
{
    unsigned int    S[8],W[64];
    unsigned int    t0,t1,t,i;

    unsigned char *data = ctx->buffer;

	// Copy state into S
    for( i=0; i<8; i++ )
    {
         S[i] = ctx->state[i];
    }

    // Copy the state into 512-bits into W[0..15]

    for( i=0; i<16; i++ )
    {
        W[i]=get_be32(data);
   	    data+=4;
    }


    // Fill W[16..63]

    for( i=16; i<64; i++ )
    {
        W[i] = Gamma1_256( W[i-2]) + W[i-7] + Gamma0_256( W[i-15] ) + W[i-16];
    }

    // Compress

    for( i=0; i<64; i++ )
    {
        Sha256Round( S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i );
        t = S[7];
        S[7] = S[6];
        S[6] = S[5];
        S[5] = S[4];
        S[4] = S[3];
        S[3] = S[2];
        S[2] = S[1];
        S[1] = S[0];
        S[0] = t;
    }

    // Feedback

    for( i=0; i<8; i++ )
    {
        ctx->state[i] = ctx->state[i] + S[i];
    }

}

/* ------------------------------- */

static void SHA512_Transform(sha512_t *ctx)
{
	unsigned char *data=ctx->buffer;
    u64_t    S[8],W[80];
    u64_t    t0,t1,i;

    // Copy state into S

    for( i=0; i<8; i++ )
    {
        S[i] = ctx->state[i];
    }

    // Copy the state into 1024-bits into W[0..15]

    for( i=0; i<16; i++ )
    {
        W[i]=get_be64(data);
        data+=8;

    }

    // Fill W[16..79]

    for( i=16; i<80; i++ )
    {
        W[i] = Gamma1_512(W[i - 2]) + W[i - 7] + Gamma0_512(W[i - 15]) + W[i - 16];
    }

    // Compress

     for( i=0; i<80; i+=8 )
     {
         Sha512Round(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0);
         Sha512Round(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1);
         Sha512Round(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2);
         Sha512Round(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3);
         Sha512Round(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4);
         Sha512Round(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5);
         Sha512Round(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6);
         Sha512Round(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7);
     }

    // Feedback

    for( i=0; i<8; i++ )
    {
         ctx->state[i] = ctx->state[i] + S[i];
    }
}


/* ------------------------------- */
/* ------------------------------- */

void sha256_init(sha256_t *ctx)
{
    memset(ctx,0,sizeof(sha256_t));

    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

/* ------------------------------- */

void sha256_update(sha256_t *ctx,const void *buf, size_t tam)
{
    unsigned char *datos = (unsigned char *)buf;
    size_t t;

    for(t=0;t<tam;t++)
    {
        ctx->buffer[ctx->pos++]=datos[t];
        if(ctx->pos==64)
        {
            SHA256_Transform(ctx);
            ctx->pos=0;
        }

        ctx->bitcount+=8;
    }
}

/* ------------------------------- */

void sha256_final(sha256_t *ctx,void *h)
{
    int quedan;
    unsigned char *hash=(unsigned char *)h;

    ctx->buffer[ctx->pos++]=0x80;

    quedan = 64 - ctx->pos;
    if(quedan < 8)
    {
        memset(ctx->buffer + ctx->pos,0,quedan);
        SHA256_Transform(ctx);
        ctx->pos=0;
        quedan = 64;
    }

    memset(ctx->buffer + ctx->pos,0,quedan - 8);

    put_be64(ctx->buffer + 56,ctx->bitcount);

    SHA256_Transform(ctx);

    put_be32(hash,ctx->state[0]);
    put_be32(hash + 4,ctx->state[1]);
    put_be32(hash + 8,ctx->state[2]);
    put_be32(hash + 12,ctx->state[3]);
    put_be32(hash + 16,ctx->state[4]);
    put_be32(hash + 20,ctx->state[5]);
    put_be32(hash + 24,ctx->state[6]);
    put_be32(hash + 28,ctx->state[7]);

	memset(ctx, 0,sizeof(sha256_t));
}

/* ------------------------------- */
/* ------------------------------- */

void sha512_init(sha512_t *ctx)
{
    memset(ctx,0,sizeof(sha512_t));

    ctx->state[0] = 0x6a09e667f3bcc908ULL;
    ctx->state[1] = 0xbb67ae8584caa73bULL;
    ctx->state[2] = 0x3c6ef372fe94f82bULL;
    ctx->state[3] = 0xa54ff53a5f1d36f1ULL;
    ctx->state[4] = 0x510e527fade682d1ULL;
    ctx->state[5] = 0x9b05688c2b3e6c1fULL;
    ctx->state[6] = 0x1f83d9abfb41bd6bULL;
    ctx->state[7] = 0x5be0cd19137e2179ULL;
}

/* ------------------------------- */

void sha512_update(sha512_t *ctx,const void *buf, size_t tam)
{
    unsigned char *datos = (unsigned char *)buf;
    size_t t;

    /* Actualizar buffer y ejecutar la función si es necesario */

    for(t=0;t<tam;t++)
    {
        ctx->buffer[ctx->pos++]=datos[t];
        if(ctx->pos==128)
        {
            SHA512_Transform(ctx);
            ctx->pos=0;
        }

        /* Actualizar el contador de bits */

        ctx->lo+=8;
        if(ctx->lo < 8)
	    	ctx->hi++;
    }
}

/* ------------------------------- */

void sha512_final(sha512_t *ctx,void *h)
{
    int quedan;
    unsigned char *hash=(unsigned char *)h;

    /* Primero añadimos el padding */

    ctx->buffer[ctx->pos++]=0x80;

    quedan = 128 - ctx->pos;
    if(quedan < 16)
    {
        memset(ctx->buffer + ctx->pos,0,quedan);
        SHA512_Transform(ctx);
        ctx->pos=0;
        quedan = 128;
    }

    memset(ctx->buffer + ctx->pos,0,quedan - 16);

    /* Ahora el contador de bits */

    put_be64(ctx->buffer + 112,ctx->hi);
    put_be64(ctx->buffer + 120,ctx->lo);

    /* Lo pasamos por la Turmix */

    SHA512_Transform(ctx);

    /* Copiamos el hash */

    put_be64(hash,ctx->state[0]);
    put_be64(hash + 8,ctx->state[1]);
    put_be64(hash + 16,ctx->state[2]);
    put_be64(hash + 24,ctx->state[3]);
    put_be64(hash + 32,ctx->state[4]);
    put_be64(hash + 40,ctx->state[5]);
    put_be64(hash + 48,ctx->state[6]);
    put_be64(hash + 56,ctx->state[7]);

    /* Y borramos la evidencia */

	memset(ctx, 0,sizeof(sha512_t));
}


/* ------------------------------- */
/* ------------------------------- */

void sha224_init(sha224_t *ctx)
{
    memset(ctx,0,sizeof(sha224_t));

    ctx->state[0] = 0xc1059ed8;
    ctx->state[1] = 0x367cd507;
    ctx->state[2] = 0x3070dd17;
    ctx->state[3] = 0xf70e5939;
    ctx->state[4] = 0xffc00b31;
    ctx->state[5] = 0x68581511;
    ctx->state[6] = 0x64f98fa7;
    ctx->state[7] = 0xbefa4fa4;
}

/* ------------------------------- */

void sha224_update(sha224_t *ctx,const void *buf, size_t tam)
{
    sha256_update(ctx,buf,tam);
}

/* ------------------------------- */

void sha224_final(sha224_t *ctx,void *h)
{
    unsigned char tmp[SHA256_SIZE];

    sha256_final(ctx,tmp);
    memcpy(h,tmp,SHA224_SIZE);
}


/* ------------------------------- */
/* ------------------------------- */

void sha384_init(sha384_t *ctx)
{
    memset(ctx,0,sizeof(sha384_t));

    ctx->state[0] = 0xcbbb9d5dc1059ed8ULL;
    ctx->state[1] = 0x629a292a367cd507ULL;
    ctx->state[2] = 0x9159015a3070dd17ULL;
    ctx->state[3] = 0x152fecd8f70e5939ULL;
    ctx->state[4] = 0x67332667ffc00b31ULL;
    ctx->state[5] = 0x8eb44a8768581511ULL;
    ctx->state[6] = 0xdb0c2e0d64f98fa7ULL;
    ctx->state[7] = 0x47b5481dbefa4fa4ULL;
}

/* ------------------------------- */

void sha384_update(sha384_t *ctx,const void *buf, size_t tam)
{
    sha512_update(ctx,buf,tam);
}

/* ------------------------------- */

void sha384_final(sha384_t *ctx,void *h)
{
    unsigned char tmp[SHA512_SIZE];

    sha512_final(ctx,tmp);
    memcpy(h,tmp,SHA384_SIZE);
}


/* ------------------------------- */
/* ------------------------------- */

void sha512_224_init(sha512_t *ctx)
{
    memset(ctx,0,sizeof(sha512_t));

    ctx->state[0] = 0x8C3D37C819544DA2ULL;
    ctx->state[1] = 0x73E1996689DCD4D6ULL;
    ctx->state[2] = 0x1DFAB7AE32FF9C82ULL;
    ctx->state[3] = 0x679DD514582F9FCFULL;
    ctx->state[4] = 0x0F6D2B697BD44DA8ULL;
    ctx->state[5] = 0x77E36F7304C48942ULL;
    ctx->state[6] = 0x3F9D85A86A1D36C8ULL;
    ctx->state[7] = 0x1112E6AD91D692A1ULL;
}

/* ------------------------------- */

void sha512_224_update(sha512_t *ctx,const void *buf, size_t tam)
{
    sha512_update(ctx,buf,tam);
}

/* ------------------------------- */

void sha512_224_final(sha512_t *ctx,void *h)
{
    unsigned char tmp[SHA512_SIZE];

    sha512_final(ctx,tmp);
    memcpy(h,tmp,SHA512_224_SIZE);
}


/* ------------------------------- */
/* ------------------------------- */

void sha512_256_init(sha512_t *ctx)
{
    memset(ctx,0,sizeof(sha512_t));

    ctx->state[0] = 0x22312194FC2BF72CULL;
    ctx->state[1] = 0x9F555FA3C84C64C2ULL;
    ctx->state[2] = 0x2393B86B6F53B151ULL;
    ctx->state[3] = 0x963877195940EABDULL;
    ctx->state[4] = 0x96283EE2A88EFFE3ULL;
    ctx->state[5] = 0xBE5E1E2553863992ULL;
    ctx->state[6] = 0x2B0199FC2C85B8AAULL;
    ctx->state[7] = 0x0EB72DDC81C52CA2ULL;
}

/* ------------------------------- */

void sha512_256_update(sha512_t *ctx,const void *buf, size_t tam)
{
    sha512_update(ctx,buf,tam);
}

/* ------------------------------- */

void sha512_256_final(sha512_t *ctx,void *h)
{
    unsigned char tmp[SHA512_SIZE];

    sha512_final(ctx,tmp);
    memcpy(h,tmp,SHA512_256_SIZE);
}



/* ------------------------------- */
/* ------------------------------- */

void sha224_partial(sha224_t *ctx,void *hash)
{
    sha224_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha224_final(&tmp,hash);
}

/* ------------------------------- */

void sha256_partial(sha256_t *ctx,void *hash)
{
    sha256_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha256_final(&tmp,hash);
}


/* ------------------------------- */

void sha384_partial(sha384_t *ctx,void *hash)
{
    sha384_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha384_final(&tmp,hash);
}

/* ------------------------------- */

void sha512_partial(sha512_t *ctx,void *hash)
{
    sha512_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha512_final(&tmp,hash);
}


/* ------------------------------- */

void sha512_224_partial(sha512_t *ctx,void *hash)
{
    sha512_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha512_224_final(&tmp,hash);
}

/* ------------------------------- */

void sha512_256_partial(sha512_t *ctx,void *hash)
{
    sha512_t tmp;

    memcpy(&tmp,ctx,sizeof(tmp));
    sha512_256_final(&tmp,hash);
}


