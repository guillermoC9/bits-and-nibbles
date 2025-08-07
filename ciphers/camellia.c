/*
    camellia.c

    Camellia Encryption routines

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

        - Written by Guillermo Amodeo Ojeda using the basic functions published
          by Mitsubishi/NTTC as camellia.c, which can be downloaded from:

            https://embeddedsw.net/Cipher_Reference_Home.html

        - Camellia is pantented by Mitsubishi & NTTC LICENSE but can be used
          as it was published under a royalty-free scheme in 2006.

        - The code here is adapted from the original submission to NESSIE
          process (camellia.c), which had no license but just the following 
          copyright header:

                    *******************************************
                    *						                  *
                    *	Camellia Block Encryption Algorithm	  *
                    *	in ANSI-C Language : Camellia.c	      *
                    *						                  *
                    *	  Version M1.02 September 24 2001	  *
                    *   Copyright Mitsubishi Corp 2000-2001   *
                    *						                  *
                    *******************************************        

        - We also support the orignal design of using 4 S-Boxes instead 1

                                --oO0Oo--
*/

#include "camellia.h"

/* -------------------------------------- *
   We use just SBOX_1 at the moment.
   Comment this definition to use all 4 
 * -------------------------------------- */

#define USE_S_BOX_1_ONLY    1    

static const unsigned char SBOX_1[256] =
{
    112, 130,  44, 236, 179,  39, 192, 229, 228, 133,  87,  53, 234,  12, 174,  65,
     35, 239, 107, 147,  69,  25, 165,  33, 237,  14,  79,  78,  29, 101, 146, 189,
    134, 184, 175, 143, 124, 235,  31, 206,  62,  48, 220,  95,  94, 197,  11,  26,
    166, 225,  57, 202, 213,  71,  93,  61, 217,   1,  90, 214,  81,  86, 108,  77,
    139,  13, 154, 102, 251, 204, 176,  45, 116,  18,  43,  32, 240, 177, 132, 153,
    223,  76, 203, 194,  52, 126, 118,   5, 109, 183, 169,  49, 209,  23,   4, 215,
     20,  88,  58,  97, 222,  27,  17,  28,  50,  15, 156,  22,  83,  24, 242,  34,
    254,  68, 207, 178, 195, 181, 122, 145,  36,   8, 232, 168,  96, 252, 105,  80,
    170, 208, 160, 125, 161, 137,  98, 151,  84,  91,  30, 149, 224, 255, 100, 210,
     16, 196,   0,  72, 163, 247, 117, 219, 138,   3, 230, 218,   9,  63, 221, 148,
    135,  92, 131,   2, 205,  74, 144,  51, 115, 103, 246, 243, 157, 127, 191, 226,
     82, 155, 216,  38, 200,  55, 198,  59, 129, 150, 111,  75,  19, 190,  99,  46,
    233, 121, 167, 140, 159, 110, 188, 142,  41, 245, 249, 182,  47, 253, 180,  89,
    120, 152,   6, 106, 231,  70, 113, 186, 212,  37, 171,  66, 136, 162, 141, 250,
    114,   7, 185,  85, 248, 238, 172,  10,  54,  73,  42, 104,  60,  56, 241, 164,
     64,  40, 211, 123, 187, 201,  67, 193,  21, 227, 173, 244, 119, 199, 128, 158
};

#ifdef USE_S_BOX_1_ONLY

#define SBOX1(n)    SBOX_1[(n)]
#define SBOX2(n)    (unsigned char)( ((SBOX_1[(n)] >> 7) ^ (SBOX_1[(n)] << 1)) & 0xff )
#define SBOX3(n)    (unsigned char)( ((SBOX_1[(n)] >> 1) ^ (SBOX_1[(n)] << 7)) & 0xff )
#define SBOX4(n)    SBOX_1[ (((n) << 1) ^ ((n) >> 7 )) & 0xff ]

#else

static const unsigned char SBOX_2[256] =
{
    224,   5,  88, 217, 103,  78, 129, 203, 201,  11, 174, 106, 213,  24,  93, 130,
     70, 223, 214,  39, 138,  50,  75,  66, 219,  28, 158, 156,  58, 202,  37, 123,
     13, 113,  95,  31, 248, 215,  62, 157, 124,  96, 185, 190, 188, 139,  22,  52,
     77, 195, 114, 149, 171, 142, 186, 122, 179,   2, 180, 173, 162, 172, 216, 154,
     23,  26,  53, 204, 247, 153,  97,  90, 232,  36,  86,  64, 225,  99,   9,  51,
    191, 152, 151, 133, 104, 252, 236,  10, 218, 111,  83,  98, 163,  46,   8, 175,
     40, 176, 116, 194, 189,  54,  34,  56, 100,  30,  57,  44, 166,  48, 229,  68,
    253, 136, 159, 101, 135, 107, 244,  35,  72,  16, 209,  81, 192, 249, 210, 160,
     85, 161,  65, 250,  67,  19, 196,  47, 168, 182,  60,  43, 193, 255, 200, 165, 
     32, 137,   0, 144,  71, 239, 234, 183,  21,   6, 205, 181,  18, 126, 187,  41,
     15, 184,   7,   4, 155, 148,  33, 102, 230, 206, 237, 231,  59, 254, 127, 197,
    164, 55,  177,  76, 145, 110, 141, 118,   3,  45, 222, 150,  38, 125, 198,  92,
    211, 242,  79,  25,  63, 220, 121,  29,  82, 235, 243, 109,  94, 251, 105, 178,
    240,  49,  12, 212, 207, 140, 226, 117, 169,  74,  87, 132,  17,  69,  27, 245,
    228,  14, 115, 170, 241, 221,  89,  20, 108, 146,  84, 208, 120, 112, 227,  73,
    128,  80, 167, 246, 119, 147, 134, 131,  42, 199,  91, 233, 238, 143,   1,  61
};


static const unsigned char SBOX_3[256] =
{
     56,  65,  22, 118, 217, 147,  96, 242, 114, 194, 171, 154, 117,   6,  87, 160,
    145, 247, 181, 201, 162, 140, 210, 144, 246,   7, 167,  39, 142, 178,  73, 222,
     67,  92, 215, 199,  62, 245, 143, 103,  31,  24, 110, 175,  47, 226, 133,  13,
     83, 240, 156, 101, 234, 163, 174, 158, 236, 128,  45, 107, 168,  43,  54, 166,
    197, 134,  77,  51, 253, 102,  88, 150,  58,   9, 149,  16, 120, 216,  66, 204,
    239,  38, 229,  97,  26,  63,  59, 130, 182, 219, 212, 152, 232, 139,   2, 235,
     10,  44,  29, 176, 111, 141, 136,  14,  25, 135,  78,  11, 169,  12, 121,  17,
    127,  34, 231,  89, 225, 218,  61, 200,  18,   4, 116,  84,  48, 126, 180,  40,
     85, 104,  80, 190, 208, 196,  49, 203,  42, 173,  15, 202, 112, 255,  50, 105,
      8,  98,   0,  36, 209, 251, 186, 237,  69, 129, 115, 109, 132, 159, 238,  74,
    195,  46, 193,   1, 230,  37,  72, 153, 185, 179, 123, 249, 206, 191, 223, 113,
     41, 205, 108,  19, 100, 155,  99, 157, 192,  75, 183, 165, 137,  95, 177,  23,
    244, 188, 211,  70, 207,  55,  94,  71, 148, 250, 252,  91, 151, 254,  90, 172,
     60,  76,   3,  53, 243,  35, 184,  93, 106, 146, 213,  33,  68,  81, 198, 125,
     57, 131, 220, 170, 124, 119,  86,   5,  27, 164,  21,  52,  30,  28, 248,  82,
     32,  20, 233, 189, 221, 228, 161, 224, 138, 241, 214, 122, 187, 227,  64,  79
};


static const unsigned char SBOX_4[256] =
{
    112,  44, 179, 192, 228,  87, 234, 174,  35, 107,  69, 165, 237,  79,  29, 146,
    134, 175, 124,  31,  62, 220,  94,  11, 166,  57, 213,  93, 217,  90,  81, 108,
    139, 154, 251, 176, 116,  43, 240, 132, 223, 203,  52, 118, 109, 169, 209,   4,
     20,  58, 222,  17,  50, 156,  83, 242, 254, 207, 195, 122,  36, 232,  96, 105,
    170, 160, 161,  98,  84,  30, 224, 100,  16,   0, 163, 117, 138, 230,   9, 221,
    135, 131, 205, 144, 115, 246, 157, 191,  82, 216, 200, 198, 129, 111,  19, 99,
    233, 167, 159, 188,  41, 249,  47, 180, 120,   6, 231, 113, 212, 171, 136, 141,
    114, 185, 248, 172,  54,  42,  60, 241,  64, 211, 187,  67,  21, 173, 119, 128,
    130, 236,  39, 229, 133,  53,  12,  65, 239, 147,  25,  33,  14,  78, 101, 189,
    184, 143, 235, 206,  48,  95, 197,  26, 225, 202,  71,  61,   1, 214,  86,  77,
     13, 102, 204,  45,  18,  32, 177, 153,  76, 194, 126,   5, 183,  49,  23, 215,
     88,  97,  27,  28,  15,  22,  24,  34,  68, 178, 181, 145,   8, 168, 252,  80,
    208, 125, 137, 151,  91, 149, 255, 210, 196,  72, 247, 219,   3, 218,  63, 148,
     92,   2,  74,  51, 103, 243, 127, 226, 155,  38,  55,  59, 150,  75, 190,  46,
    121, 140, 110, 142, 245, 182, 253,  89, 152, 106,  70, 186,  37,  66, 162, 250,
      7,  85, 238,  10,  73, 104,  56, 164,  40, 123, 201, 193, 227, 244, 199, 158
};

#define SBOX1(n)    SBOX_1[(n)]
#define SBOX2(n)    SBOX_2[(n)]
#define SBOX3(n)    SBOX_3[(n)]
#define SBOX4(n)    SBOX_4[(n)]

#endif

/* ------------------------------ */

static void camellia_feistel( const unsigned char *x, const unsigned char *k, unsigned char *y )
{
	unsigned char t[8];

	t[0] = SBOX1(x[0]^k[0]);
	t[1] = SBOX2(x[1]^k[1]);
	t[2] = SBOX3(x[2]^k[2]);
	t[3] = SBOX4(x[3]^k[3]);
	t[4] = SBOX2(x[4]^k[4]);
	t[5] = SBOX3(x[5]^k[5]);
	t[6] = SBOX4(x[6]^k[6]);
	t[7] = SBOX1(x[7]^k[7]);

	y[0] ^= t[0]^t[2]^t[3]^t[5]^t[6]^t[7];
	y[1] ^= t[0]^t[1]^t[3]^t[4]^t[6]^t[7];
	y[2] ^= t[0]^t[1]^t[2]^t[4]^t[5]^t[7];
	y[3] ^= t[1]^t[2]^t[3]^t[4]^t[5]^t[6];
	y[4] ^= t[0]^t[1]^t[5]^t[6]^t[7];
	y[5] ^= t[1]^t[2]^t[4]^t[6]^t[7];
	y[6] ^= t[2]^t[3]^t[4]^t[5]^t[7];
	y[7] ^= t[0]^t[3]^t[4]^t[5]^t[6];
}

/* ------------------------------ */

static const int KIDX[60] =
{
    0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x04,0x00,0x04,
    0x04,0x00,0x00,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x0c,0x0c,
    0x08,0x08,0x04,0x04,0x08,0x08,0x0c,0x0c,0x00,0x00,0x04,0x04,0x00,0x00,0x08,
    0x08,0x0c,0x0c,0x00,0x00,0x04,0x04,0x08,0x08,0x04,0x04,0x00,0x00,0x0c,0x0c
};

static const int KSFT[60] =
{
    0x00,0x40,0x00,0x40,0x0f,0x4f,0x0f,0x4f,0x1e,0x5e,0x2d,0x6d,0x2d,0x7c,0x3c,
    0x7c,0x4d,0x0d,0x5e,0x1e,0x5e,0x1e,0x6f,0x2f,0x6f,0x2f,0x00,0x40,0x00,0x40,
    0x0f,0x4f,0x0f,0x4f,0x1e,0x5e,0x1e,0x5e,0x2d,0x6d,0x2d,0x6d,0x3c,0x7c,0x3c,
    0x7c,0x3c,0x7c,0x4d,0x0d,0x4d,0x0d,0x5e,0x1e,0x5e,0x1e,0x6f,0x2f,0x6f,0x2f
};

/* ------------------------------ */

static void camellia_rotate( const unsigned int *x, const int one, const int i, unsigned int *y )
{
	int r,n;


    x += KIDX[i + (one * 26)];
    n  = KSFT[i + (one * 26)];

    r  = (n & 31);

	if( r )
	{
		y[0] = x[((n >> 5) + 0) & 3] << r ^ x[((n>>5) + 1 ) & 3] >> (32 - r);
		y[1] = x[((n >> 5) + 1) & 3] << r ^ x[((n>>5) + 2 ) & 3] >> (32 - r);
	}
	else
	{
		y[0] = x[((n >> 5) + 0) & 3];
		y[1] = x[((n >> 5) + 1) & 3];
	}
}

/* ------------------------------ */

static void camellia_bytes_to_block( const unsigned char *x, unsigned int *y )
{
    y[0] = get_be32(x + 0);
    y[1] = get_be32(x + 4);
    y[2] = get_be32(x + 8);
    y[3] = get_be32(x + 12);
}

/* ------------------------------ */

static void camellia_block_to_bytes( const unsigned int *x, unsigned char *y )
{
	put_be32(y +  0,x[0]);
	put_be32(y +  4,x[1]);
	put_be32(y +  8,x[2]);
	put_be32(y + 12,x[3]);
}

/* ------------------------------ */

static void camellia_fl_layer( unsigned char *x, const unsigned char *kl, const unsigned char *kr )
{
	unsigned int t[4],u[4],v[4];

	camellia_bytes_to_block( x, t );
	camellia_bytes_to_block( kl, u );
	camellia_bytes_to_block( kr, v );

	t[1] ^= (t[0]&u[0])<<1^(t[0]&u[0])>>31;
	t[0] ^= t[1]|u[1];
	t[2] ^= t[3]|v[1];
	t[3] ^= (t[2]&v[0])<<1^(t[2]&v[0])>>31;

	camellia_block_to_bytes( t, x );
}

/* ------------------------------ */

static void camellia_swap_xor( unsigned char *x, const unsigned char *y )
{
	unsigned char tmp[8];

    memcpy(tmp,x+8,8);
    memcpy(x+8,x,8);
    memcpy(x,tmp,8);
    mem_xor(x,y,16);
}

/* ------------------------------ */

static void camellia_encrypt( const int n, const unsigned char *p, const unsigned char *e, unsigned char *c )
{
	int i;

    memcpy(c,p,16);
    mem_xor(c,e,16);

	for( i=0; i<3; i++ )
	{
		camellia_feistel( c+0, e+16+(i<<4), c+8 );
		camellia_feistel( c+8, e+24+(i<<4), c+0 );
	}

	camellia_fl_layer( c, e+64, e+72 );

	for( i=0; i<3; i++ )
	{
		camellia_feistel( c+0, e+80+(i<<4), c+8 );
		camellia_feistel( c+8, e+88+(i<<4), c+0 );
	}

	camellia_fl_layer( c, e+128, e+136 );

	for( i=0; i<3; i++ )
	{
		camellia_feistel( c+0, e+144+(i<<4), c+8 );
		camellia_feistel( c+8, e+152+(i<<4), c+0 );
	}

	if( n == 128 )
	{
		camellia_swap_xor( c , e+192);
	}
	else
	{
		camellia_fl_layer( c, e+192, e+200 );

		for( i=0; i<3; i++ )
		{
			camellia_feistel( c+0, e+208+(i<<4), c+8 );
			camellia_feistel( c+8, e+216+(i<<4), c+0 );
		}

		camellia_swap_xor( c, e+256);
	}
}

/* ------------------------------ */

static void camellia_decrypt( const int n, const unsigned char *c, const unsigned char *e, unsigned char *p )
{
	int i;

    memcpy(p,c,16);
	if( n == 128 )
	{
        mem_xor(p,e + 192,16);
	}
	else
	{
		mem_xor(p,e + 256,16);

		for( i=2; i>=0; i-- )
		{
			camellia_feistel( p+0, e+216+(i<<4), p+8 );
			camellia_feistel( p+8, e+208+(i<<4), p+0 );
		}

		camellia_fl_layer( p, e+200, e+192 );
	}

	for( i=2; i>=0; i-- )
	{
		camellia_feistel( p+0, e+152+(i<<4), p+8 );
		camellia_feistel( p+8, e+144+(i<<4), p+0 );
	}

	camellia_fl_layer( p, e+136, e+128 );

	for( i=2; i>=0; i-- )
	{
		camellia_feistel( p+0, e+88+(i<<4), p+8 );
		camellia_feistel( p+8, e+80+(i<<4), p+0 );
	}

	camellia_fl_layer( p, e+72, e+64 );

	for( i=2; i>=0; i-- )
	{
		camellia_feistel( p+0, e+24+(i<<4), p+8 );
		camellia_feistel( p+8, e+16+(i<<4), p+0 );
	}

	camellia_swap_xor( p, e+0);
}

/* ------------------------------ */

static void camellia_keygen( const int n, const unsigned char *key, unsigned char *res )
{
	unsigned int wrd[20];
	int  i;
	unsigned char tmp[64]   = {0};
	unsigned char SIGMA[48] =
    {
        0xa0,0x9e,0x66,0x7f,0x3b,0xcc,0x90,0x8b,
        0xb6,0x7a,0xe8,0x58,0x4c,0xaa,0x73,0xb2,
        0xc6,0xef,0x37,0x2f,0xe9,0x4f,0x82,0xbe,
        0x54,0xff,0x53,0xa5,0xf1,0xd3,0x6f,0x1c,
        0x10,0xe5,0x27,0xfa,0xde,0x68,0x2d,0x1d,
        0xb0,0x56,0x88,0xc2,0xb3,0xe6,0xc1,0xfd
    };


    if(n == 192)
    {
        memcpy(tmp,key,24);
        memcpy(tmp + 24, key + 16, 8);
        memxor(tmp + 24, 0xff, 8);
    }
    else
    {
        memcpy(tmp,key,(n / 8));
    }

    memcpy(tmp + 32, key,16);
    mem_xor(tmp + 32, tmp + 16,16);

	camellia_feistel( tmp + 32, SIGMA + 0, tmp + 40 );
	camellia_feistel( tmp + 40, SIGMA + 8, tmp + 32 );

    mem_xor(tmp + 32 ,tmp + 0, 16);


	camellia_feistel( tmp + 32, SIGMA + 16, tmp + 40 );
	camellia_feistel( tmp + 40, SIGMA + 24, tmp + 32 );

	camellia_bytes_to_block( tmp + 0,  wrd +0 );
	camellia_bytes_to_block( tmp + 32, wrd +4 );

	if( n == 128 )
	{
		for(i=0; i<26; i++)
		{
            camellia_rotate( wrd, 0,  i + 0 , wrd + 16 );
            camellia_rotate( wrd, 0,  i + 1 , wrd + 18 );

			camellia_block_to_bytes( wrd + 16, res + i * 8 );
		}
	}
	else
	{
        memcpy(tmp + 48, tmp + 32,16);
        mem_xor(tmp + 48, tmp + 16,16);

		camellia_feistel( tmp + 48, SIGMA + 32, tmp + 56 );
		camellia_feistel( tmp + 56, SIGMA + 40, tmp + 48 );

		camellia_bytes_to_block( tmp + 16, wrd + 8  );
		camellia_bytes_to_block( tmp + 48, wrd + 12 );

		for( i=0; i<34; i+=2 )
		{
    		camellia_rotate( wrd, 1,  i + 0 , wrd + 16 );
            camellia_rotate( wrd, 1,  i + 1 , wrd + 18 );

			camellia_block_to_bytes( wrd + 16, res + (i<<3) );
		}
	}

}



/* ---------------------------- */

int camellia_init(camellia_t *ctx,int bits,const void *key)
{
    if(ctx && key)
    {
        memset(ctx,0,sizeof(camellia_t));

        switch(bits)
        {
            case 128:
                ctx->kbits=bits;
                break;
            case 192:
                ctx->kbits=bits;
                break;
            case 256:
                ctx->kbits=bits;
                break;
            default:
                return -1;
        }

        camellia_keygen(bits,(unsigned char *)key,ctx->ks);

        return 0;
    }
    return -1;
}

/* ---------------------------- */

void camellia_encode_block(camellia_t *ctx,void *dest,const void *orig)
{
    unsigned char tmp[CAMELLIA_BLOCK_SIZE];

    camellia_encrypt(ctx->kbits,orig,ctx->ks,tmp);
    memcpy(dest,tmp,CAMELLIA_BLOCK_SIZE);
}

/* ---------------------------- */

void camellia_decode_block(camellia_t *ctx,void *dest,const void *orig)
{
    unsigned char  tmp[CAMELLIA_BLOCK_SIZE];

    camellia_decrypt(ctx->kbits,orig,ctx->ks,tmp);
    memcpy(dest,tmp,CAMELLIA_BLOCK_SIZE);
}







