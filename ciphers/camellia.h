/*
    camellia.h

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

        - The code in camellia.c only implements block's ECB encoding/decoding

        - We also support the orignal design of using 4 S-Boxes instead 1

                                --oO0Oo--
*/

#ifndef MITSUBISHI_CAMELLIA
#define MITSUBISHI_CAMELLIA

#include "stuff.h"

#define CAMELLIA_128_KEY_SIZE   16
#define CAMELLIA_192_KEY_SIZE   24
#define CAMELLIA_256_KEY_SIZE   32

#define CAMELLIA_MAX_KEY_SIZE   CAMELLIA_256_KEY_SIZE
#define CAMELLIA_BLOCK_SIZE     16
#define CAMELLIA_IV_SIZE        CAMELLIA_BLOCK_SIZE

#define CAMELLIA_KEY_SCHED      272

/** Camellia Context */

typedef struct _cam_ctx_
{
    unsigned char    ks[CAMELLIA_KEY_SCHED];
    unsigned char    iv[CAMELLIA_IV_SIZE];

    int     kbits;
} camellia_t;

/** Macros */

#define camellia_bits(_1)     ((_1)->kbits)
#define camellia_end(_1)      memset(_1,0,sizeof(camellia_t))

/* Funciones */

#ifdef __cplusplus
extern "C" {
#endif


/*
    Initialize a Camellia Context

    ctx  = Context
    bits = bits of the key (128, 192 or 256)    
    key  = Initialization Key (number of bytes for 'bits' eg. CAMELLIA_128_KEY_SIZE for 128)    

    Return 0 for OK or -1 error
*/

int camellia_init(camellia_t *ctx,int bits,const void *key);

/** These encode just one block of data (assumed CAMELLIA_BLOCK_SIZE bytes)
    and are intended to be used by routines that need to encrypt one
    block at the time (eg. crypto.c)

    'orig' and 'dest' can point to the same block of memory
*/

void camellia_encode_block(camellia_t *ctx,void *dest,const void *orig);
void camellia_decode_block(camellia_t *ctx,void *dest,const void *orig);

/** return the number of bytes that need to be added to make cnt a multiple of CAMELLIA_BLOCK_SIZE */

size_t camellia_pad(size_t tam);


#ifdef __cplusplus
};
#endif


#endif
