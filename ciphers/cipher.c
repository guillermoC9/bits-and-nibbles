/*
    cipher.c

    Cryptographic Symmetric Ciphers

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

         - Written by Guillermo Amodeo Ojeda using code and info from:

            - RFC 2313 at https://www.rfc-editor.org/rfc/rfc2313.txt
            - RFC 4346 at https://www.rfc-editor.org/rfc/rfc4346.txt
            - RFC 2898 at https://www.ietf.org/rfc/rfc2898.txt
            - RFC 8269 at https://www.ietf.org/rfc/rfc8269.txt
            - RFC 3686 at https://www.ietf.org/rfc/rfc3586.txt
            - RFC 4106 at https://www.ietf.org/rfc/rfc4106.txt

        - GCM and CTR modes implemented by info from RFC-8269, RFC-3686 
          and RFC-4106

                                --oO0Oo--
*/

#include "cipher.h"

/* ------------------------------------ *
         Encryption Algorithms
 * ------------------------------------ */

static struct _alg_enc_
{
    int alg;    
    char *name;
    wchar_t *namew;    
}

alg_enc[CIPHER_COUNT + 1] =
{

    /* IMPORTANT: Make sure they are in teh same order (alphabetic) than in the enum of cipher.h */    

    {AES_128_ECB , "AES-128-ECB", L"AES-128-ECB"}, {AES_192_ECB , "AES-192-ECB", L"AES-192-ECB"}, {AES_256_ECB , "AES-256-ECB", L"AES-256-ECB"},
    {AES_128_CBC , "AES-128-CBC", L"AES-128-CBC"}, {AES_192_CBC , "AES-192-CBC", L"AES-192-CBC"}, {AES_256_CBC , "AES-256-CBC", L"AES-256-CBC"},
    {AES_128_CTR , "AES-128-CTR", L"AES-128-CTR"}, {AES_192_CTR , "AES-192-CTR", L"AES-192-CTR"}, {AES_256_CTR , "AES-256-CTR", L"AES-256-CTR"},
    {AES_128_GCM , "AES-128-GCM", L"AES-128-GCM"}, {AES_192_GCM , "AES-192-GCM", L"AES-192-GCM"}, {AES_256_GCM , "AES-256-GCM", L"AES-256-GCM"},
    
    {ARIA_128_ECB , "ARIA-128-ECB", L"ARIA-128-ECB"}, {ARIA_192_ECB , "ARIA-192-ECB", L"ARIA-192-ECB"}, {ARIA_256_ECB , "ARIA-256-ECB", L"ARIA-256-ECB"},
    {ARIA_128_CBC , "ARIA-128-CBC", L"ARIA-128-CBC"}, {ARIA_192_CBC , "ARIA-192-CBC", L"ARIA-192-CBC"}, {ARIA_256_CBC , "ARIA-256-CBC", L"ARIA-256-CBC"},
    {ARIA_128_CTR , "ARIA-128-CTR", L"ARIA-128-CTR"}, {ARIA_192_CTR , "ARIA-192-CTR", L"ARIA-192-CTR"}, {ARIA_256_CTR , "ARIA-256-CTR", L"ARIA-256-CTR"},
    {ARIA_128_GCM , "ARIA-128-GCM", L"ARIA-128-GCM"}, {ARIA_192_GCM , "ARIA-192-GCM", L"ARIA-192-GCM"}, {ARIA_256_GCM , "ARIA-256-GCM", L"ARIA-256-GCM"},

    {BLOWFISH_128_ECB, "BLOWF-128-ECB",L"BLOWF-128-ECB"}, {BLOWFISH_128_CBC, "BLOWF-128-CBC",L"BLOWF-128-CBC"},

    {CAMELLIA_128_ECB , "CAMELLIA-128-ECB", L"CAMELLIA-128-ECB"}, {CAMELLIA_192_ECB , "CAMELLIA-192-ECB", L"CAMELLIA-192-ECB"},
    {CAMELLIA_256_ECB , "CAMELLIA-256-ECB", L"CAMELLIA-256-ECB"}, {CAMELLIA_128_CBC , "CAMELLIA-128-CBC", L"CAMELLIA-128-CBC"},
    {CAMELLIA_192_CBC , "CAMELLIA-192-CBC", L"CAMELLIA-192-CBC"}, {CAMELLIA_256_CBC , "CAMELLIA-256-CBC", L"CAMELLIA-256-CBC"},
    {CAMELLIA_128_CTR , "CAMELLIA-128-CTR", L"CAMELLIA-128-CTR"}, {CAMELLIA_192_CTR , "CAMELLIA-192-CTR", L"CAMELLIA-192-CTR"},
    {CAMELLIA_256_CTR , "CAMELLIA-256-CTR", L"CAMELLIA-256-CTR"}, {CAMELLIA_128_GCM , "CAMELLIA-128-GCM", L"CAMELLIA-128-CBC"},
    {CAMELLIA_192_GCM , "CAMELLIA-192-GCM", L"CAMELLIA-192-CBC"}, {CAMELLIA_256_GCM , "CAMELLIA-256-GCM", L"CAMELLIA-256-CBC"},

    {CHACHA8_128  , "CHACHA8-128",  L"CHACHA8-128"},     {CHACHA8_256 ,  "CHACHA8-256",  L"CHACHA8-256"},
    {CHACHA12_128 , "CHACHA12-128", L"CHACHA12-128"},   {CHACHA12_256 , "CHACHA12-256", L"CHACHA12-256"},
    {CHACHA20_128 , "CHACHA20-128", L"CHACHA20-128"},   {CHACHA20_256 , "CHACHA20-256", L"CHACHA20-256"},
    
    {CHACHA20_IETF ,"CHACHA20-ITEF", L"CHACHA20-IETF"}, 
    {CHACHA20_POLY_1305 , "CHACHA20-POLY1305-AEAD", L"CHACHA20-POLY1305-AEAD"},

    {DES_ECB     , "DES-ECB",     L"DES-ECB"},  {DES_CBC     , "DES-CBC",     L"DES-CBC"},
    {DES_EDE3_ECB, "DES-EDE3"  ,  L"DES-EDE3"}, {DES_EDE3_CBC, "DES-EDE3-CBC",L"DES-EDE3-CBC"},

    {RC4_64,       "RC4-64",      L"RC4-64"},   {RC4_128,      "RC4-128",     L"RC4-128"},    

    {SALSA20_128 , "SALSA20-128", L"SALSA20-128"}, {SALSA20_256 , "SALSA20-256", L"SALSA20-256"},

    {TWOFISH_128_ECB , "TWOF-128-ECB", L"TWOF-128-ECB"}, {TWOFISH_192_ECB , "TWOF-192-ECB", L"TWOF-192-ECB"},
    {TWOFISH_256_ECB , "TWOF-256-ECB", L"TWOF-256-ECB"}, {TWOFISH_128_CBC , "TWOF-128-CBC", L"TWOF-128-CBC"},
    {TWOFISH_192_CBC , "TWOF-192-CBC", L"TWOF-192-CBC"}, {TWOFISH_256_CBC , "TWOF-256-CBC", L"TWOF-256-CBC"},
    {TWOFISH_128_CTR , "TWOF-128-CTR", L"TWOF-128-CTR"}, {TWOFISH_192_CTR , "TWOF-192-CTR", L"TWOF-192-CTR"},
    {TWOFISH_256_CTR , "TWOF-256-CTR", L"TWOF-256-CTR"}, {TWOFISH_128_GCM , "TWOF-128-GCM", L"TWOF-128-GCM"},
    {TWOFISH_192_GCM , "TWOF-192-GCM", L"TWOF-192-GCM"}, {TWOFISH_256_GCM , "TWOF-256-GCM", L"TWOF-256-GCM"},   

    {XTEA_128_ECB, "XTEA-128-EBC",L"XTEA-128-ECB"}, {XTEA_128_CBC, "XTEA-128-CBC",L"XTEA-128-CBC"},

    /* This always the last */

    {NULL_CIPHER,NULL,NULL},
};

/* -------------------------------------------- */

int cipher_alg_from_name(const char *name)
{
    int t;

    if(name)
    {
        for(t=0;alg_enc[t].name;t++)
        {
            if(!strcmp(name,alg_enc[t].name))
                return alg_enc[t].alg;
        }
    }
    return NULL_CIPHER;
}

/* -------------------------------------------- */

int cipher_alg_from_namew(const wchar_t *name)
{
    int t;

    if(name)
    {
        for(t=0;alg_enc[t].namew;t++)
        {
            if(!wcscmp(name,alg_enc[t].namew))
                return alg_enc[t].alg;
        }
    }
    return NULL_CIPHER;
}

/* -------------------------------------------- */

char *cipher_alg_name(int which)
{
    if(which > NULL_CIPHER && which < CIPHER_COUNT)
        return alg_enc[which].name;
    return NULL;
}

/* -------------------------------------------- */

wchar_t *cipher_alg_namew(int which)
{
    if(which > NULL_CIPHER && which < CIPHER_COUNT)
        return alg_enc[which].namew;
    return NULL;
}

/* -------------------------------------------- */

int cipher_alg_block_size(int alg)
{
    switch(alg)
    {        
        case DES_ECB:
        case DES_CBC:
        case DES_EDE3_ECB:
        case DES_EDE3_CBC:
            return DES_BLOCK_SIZE;

        case XTEA_128_ECB:
        case XTEA_128_CBC:
            return XTEA_BLOCK_SIZE;

        case BLOWFISH_128_ECB:
        case BLOWFISH_128_CBC:
            return BLOWF_BLOCK_SIZE;

        case AES_128_ECB:
        case AES_192_ECB:
        case AES_256_ECB:
        case AES_128_CBC:
        case AES_192_CBC:
        case AES_256_CBC:       
            return AES_BLOCK_SIZE;         

        case CAMELLIA_128_ECB:
        case CAMELLIA_192_ECB:
        case CAMELLIA_256_ECB:
        case CAMELLIA_128_CBC:
        case CAMELLIA_192_CBC:
        case CAMELLIA_256_CBC:        
            return CAMELLIA_BLOCK_SIZE;

        case ARIA_128_ECB:
        case ARIA_192_ECB:
        case ARIA_256_ECB:
        case ARIA_128_CBC:
        case ARIA_192_CBC:
        case ARIA_256_CBC:
            return ARIA_BLOCK_SIZE;        

        case TWOFISH_128_ECB:
        case TWOFISH_192_ECB:
        case TWOFISH_256_ECB:
        case TWOFISH_128_CBC:
        case TWOFISH_192_CBC:
        case TWOFISH_256_CBC:   
            return TWOFISH_BLOCK_SIZE;     
        default:
            break;
    }
    return 0;
}


/* -------------------------------------------- */

int cipher_alg_iv_size(int alg)
{
    switch(alg)
    {
        case DES_CBC:
        case DES_EDE3_CBC:
            return DES_IV_SIZE;

        case XTEA_128_CBC:
            return XTEA_IV_SIZE;

        case BLOWFISH_128_CBC:
            return BLOWF_IV_SIZE;

        case AES_128_CBC:
        case AES_192_CBC:
        case AES_256_CBC:
            return AES_IV_SIZE;

        case CAMELLIA_128_CBC:
        case CAMELLIA_192_CBC:
        case CAMELLIA_256_CBC:
            return CAMELLIA_IV_SIZE;        

        case CHACHA8_128:
        case CHACHA8_256:
        case CHACHA12_128:
        case CHACHA12_256:            
        case CHACHA20_128:
        case CHACHA20_256:
            return CHACHA_IV_SIZE;

        case CHACHA20_IETF:
            return CHACHA20_IV_SIZE;

        case ARIA_128_CBC:
        case ARIA_192_CBC:
        case ARIA_256_CBC:
            return ARIA_IV_SIZE;        

        case TWOFISH_128_CBC:
        case TWOFISH_192_CBC:
        case TWOFISH_256_CBC:
            return TWOFISH_IV_SIZE;        

        case AES_128_CTR:
        case AES_192_CTR:
        case AES_256_CTR:
        case CAMELLIA_128_CTR:
        case CAMELLIA_192_CTR:
        case CAMELLIA_256_CTR:            
        case ARIA_128_CTR:
        case ARIA_192_CTR:
        case ARIA_256_CTR:
            return CIPHER_CTR_IV_SIZE;  

        default:
            break;
    }
    return 0;
}

/* -------------------------------------------- */
/* -------------------------------------------- */

int cipher_init(cipher_t *ctx,int alg,const void *key,const void *iv,unsigned int counter)
{
    int bits=128,ret;    
    
    if(ctx == NULL)
        return -1;    

    memset(ctx,0,sizeof(cipher_t));
    ctx->alg = NULL_CIPHER;
    ctx->is_stream = FALSE;

    switch(alg)
    {
        case  AES_256_ECB:   bits += 64;
        case  AES_192_ECB:   bits += 64;        
        case  AES_128_ECB:

            ret=aes_init(&ctx->a.aes,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = AES_BLOCK_SIZE;
            ctx->kbits  = bits;

            ctx->encode_block = (block_cipher_t) aes_encode_block;
            ctx->decode_block = (block_cipher_t) aes_decode_block;

            break;

        case  AES_256_CBC:   bits += 64;
        case  AES_192_CBC:   bits += 64;
        case  AES_128_CBC:

            ret=aes_init(&ctx->a.aes,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = AES_IV_SIZE;
            ctx->blk_sz = AES_BLOCK_SIZE;
            ctx->kbits  = bits;

            if (iv)
                memcpy(ctx->iv,iv,ctx->iv_sz);

            ctx->encode_block = (block_cipher_t) aes_encode_block;
            ctx->decode_block = (block_cipher_t) aes_decode_block;

            break;

        case  AES_256_CTR:   bits += 64;
        case  AES_192_CTR:   bits += 64;
        case  AES_128_CTR:   
            
            ret=aes_init(&ctx->a.aes,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CTR;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = AES_BLOCK_SIZE;
            ctx->ctr_count = counter;
            ctx->ctr_iv = CIPHER_CTR_IV_SIZE;

            if (iv)
                memcpy(ctx->iv,iv,ctx->ctr_iv);            

            ctx->encode_block = (block_cipher_t) aes_encode_block;
            ctx->decode_block = (block_cipher_t) aes_decode_block;

            aes_encode_block(&(ctx->a.aes),ctx->gcm_h,ctx->gcm_h);
            break;

        case  AES_256_GCM:   bits += 64;
        case  AES_192_GCM:   bits += 64;
        case  AES_128_GCM:

            ret=aes_init(&ctx->a.aes,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_AEAD;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = AES_BLOCK_SIZE;

            ctx->encode_block = (block_cipher_t) aes_encode_block;
            ctx->decode_block = (block_cipher_t) aes_decode_block;

            aes_encode_block(&(ctx->a.aes),ctx->gcm_h,ctx->gcm_h);

            break;
            
        case  ARIA_256_ECB:  bits += 64;
        case  ARIA_192_ECB:  bits += 64;
        case  ARIA_128_ECB:

            ret=aria_init(&ctx->a.aria,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = ARIA_BLOCK_SIZE;
            ctx->kbits  = bits;

            ctx->encode_block = (block_cipher_t) aria_encode_block;
            ctx->decode_block = (block_cipher_t) aria_decode_block;

            break;

        case  ARIA_256_CBC:  bits += 64;
        case  ARIA_192_CBC:  bits += 64;
        case  ARIA_128_CBC:            

            ret=aria_init(&ctx->a.aria,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = ARIA_IV_SIZE;
            ctx->blk_sz = ARIA_BLOCK_SIZE;
            ctx->kbits  = bits;

            if (iv)
                memcpy(ctx->iv,iv,ARIA_IV_SIZE);

            ctx->encode_block = (block_cipher_t) aria_encode_block;
            ctx->decode_block = (block_cipher_t) aria_decode_block;

            break;

        case  ARIA_256_CTR:  bits += 64;
        case  ARIA_192_CTR:  bits += 64;
        case  ARIA_128_CTR:
            
            ret=aria_init(&ctx->a.aria,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CTR;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = ARIA_BLOCK_SIZE;
            ctx->ctr_count = counter;            
            ctx->ctr_iv = CIPHER_CTR_IV_SIZE;

            if (iv)
                memcpy(ctx->iv,iv,ctx->ctr_iv);            

            ctx->encode_block = (block_cipher_t) aria_encode_block;
            ctx->decode_block = (block_cipher_t) aria_decode_block;

            break;            

        case  ARIA_256_GCM:  bits += 64;
        case  ARIA_192_GCM:  bits += 64;
        case  ARIA_128_GCM:

            ret=aria_init(&ctx->a.aria,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_AEAD;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = ARIA_BLOCK_SIZE;

            ctx->encode_block = (block_cipher_t) aria_encode_block;
            ctx->decode_block = (block_cipher_t) aria_decode_block;

            aria_encode_block(&(ctx->a.aria),ctx->gcm_h,ctx->gcm_h);

            break;

        case  BLOWFISH_128_ECB:

            ret=blowf_init(&ctx->a.blf,key,16);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = BLOWF_BLOCK_SIZE;
            ctx->kbits  = 128;

            ctx->encode_block = (block_cipher_t) blowf_encode_block;
            ctx->decode_block = (block_cipher_t) blowf_decode_block;

            break;

        case  BLOWFISH_128_CBC:            

            ret=blowf_init(&ctx->a.blf,key,16);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = BLOWF_IV_SIZE;
            ctx->blk_sz = BLOWF_BLOCK_SIZE;
            ctx->kbits  = 128;

            if (iv)
                memcpy(ctx->iv,iv,ctx->iv_sz);

            ctx->encode_block = (block_cipher_t) blowf_encode_block;
            ctx->decode_block = (block_cipher_t) blowf_decode_block;

            break;
        
        case  CAMELLIA_256_ECB:  bits += 64;
        case  CAMELLIA_192_ECB:  bits += 64;
        case  CAMELLIA_128_ECB:

            ret=camellia_init(&ctx->a.cam,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = CAMELLIA_BLOCK_SIZE;
            ctx->kbits  = bits;

            ctx->encode_block = (block_cipher_t) camellia_encode_block;
            ctx->decode_block = (block_cipher_t) camellia_decode_block;

            break;

        case  CAMELLIA_256_CBC:  bits += 64;
        case  CAMELLIA_192_CBC:  bits += 64;
        case  CAMELLIA_128_CBC:

            ret=camellia_init(&ctx->a.cam,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = CAMELLIA_IV_SIZE;            
            ctx->blk_sz = CAMELLIA_BLOCK_SIZE;
            ctx->kbits  = bits;

            if (iv)
                memcpy(ctx->iv,iv,ctx->iv_sz);

            ctx->encode_block = (block_cipher_t) camellia_encode_block;
            ctx->decode_block = (block_cipher_t) camellia_decode_block;

            break;

        case  CAMELLIA_256_CTR:  bits += 64;
        case  CAMELLIA_192_CTR:  bits += 64;
        case  CAMELLIA_128_CTR:

            ret=camellia_init(&ctx->a.cam,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CTR;
            ctx->iv_sz  = 0;            
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = CAMELLIA_BLOCK_SIZE;
            ctx->ctr_count = counter;
            ctx->ctr_iv = CIPHER_CTR_IV_SIZE;

            if (iv)
                memcpy(ctx->iv,iv,ctx->ctr_iv);            

            ctx->encode_block = (block_cipher_t) camellia_encode_block;
            ctx->decode_block = (block_cipher_t) camellia_decode_block;            

            break;

        case  CAMELLIA_256_GCM:  bits += 64;
        case  CAMELLIA_192_GCM:  bits += 64;
        case  CAMELLIA_128_GCM:

            ret=camellia_init(&ctx->a.cam,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_AEAD;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = CAMELLIA_BLOCK_SIZE;

            ctx->encode_block = (block_cipher_t) camellia_encode_block;
            ctx->decode_block = (block_cipher_t) camellia_decode_block;

            camellia_encode_block(&(ctx->a.cam),ctx->gcm_h,ctx->gcm_h);

            break;

        case  CHACHA8_128:

            ret=chacha_start(&ctx->a.cha,128,key,8,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 128;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) chacha;
            ctx->decode_stream = (stream_cipher_t) chacha;

            break;

        case  CHACHA8_256:

            ret=chacha_start(&ctx->a.cha,256,key,8,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 256;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) chacha;
            ctx->decode_stream = (stream_cipher_t) chacha;
            
            break;

        case  CHACHA12_128:

            ret=chacha_start(&ctx->a.cha,128,key,12,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 128;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) chacha;
            ctx->decode_stream = (stream_cipher_t) chacha;

            break;

        case  CHACHA12_256:

            ret=chacha_start(&ctx->a.cha,256,key,12,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 256;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) chacha;
            ctx->decode_stream = (stream_cipher_t) chacha;

            break;

        case  CHACHA20_128:

            ret=chacha_start(&ctx->a.cha,128,key,20,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 128;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) chacha;
            ctx->decode_stream = (stream_cipher_t) chacha;

            break;

        case  CHACHA20_256:

            ret=chacha_start(&ctx->a.cha,256,key,20,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 256;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) chacha;
            ctx->decode_stream = (stream_cipher_t) chacha;

            break;

        case  CHACHA20_IETF:

            ret=chacha20_start(&ctx->a.cha,key,iv,counter);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 256;
            ctx->is_stream = TRUE;            

            ctx->encode_stream = (stream_cipher_t) chacha20;
            ctx->decode_stream = (stream_cipher_t) chacha20;

            break;

        case CHACHA20_POLY_1305:
            
            ret=chacha20_start(&ctx->a.cha,key,NULL,0);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_AEAD;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 256;
            ctx->is_stream = TRUE;

            break;

        case  DES_ECB:

            ret=des56_init(&ctx->a.des,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = DES_BLOCK_SIZE;
            ctx->kbits  = 56;

            if (iv)            
                memcpy(ctx->iv,iv,DES_IV_SIZE);

            ctx->encode_block = (block_cipher_t) des_encode_block;
            ctx->decode_block = (block_cipher_t) des_decode_block;

        case  DES_CBC:

            ret=des56_init(&ctx->a.des,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = DES_IV_SIZE;
            ctx->blk_sz = DES_BLOCK_SIZE;
            ctx->kbits  = 56;

            if (iv)            
                memcpy(ctx->iv,iv,DES_IV_SIZE);

            ctx->encode_block = (block_cipher_t) des_encode_block;
            ctx->decode_block = (block_cipher_t) des_decode_block;

            break;

        case  DES_EDE3_ECB:

            ret=des_init(&ctx->a.des,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = DES_BLOCK_SIZE;
            ctx->kbits  = 168;

            ctx->encode_block = (block_cipher_t) des_encode_block;
            ctx->decode_block = (block_cipher_t) des_decode_block;

            break;

        case  DES_EDE3_CBC:

            ret=des_init(&ctx->a.des,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = DES_IV_SIZE;
            ctx->blk_sz = DES_BLOCK_SIZE;
            ctx->kbits  = 168;

            if (iv)
                memcpy(ctx->iv,iv,DES_IV_SIZE);

            ctx->encode_block = (block_cipher_t) des_encode_block;
            ctx->decode_block = (block_cipher_t) des_decode_block;

            break;

        case  RC4_64:

            ret=rc4_init(&ctx->a.rc4,64,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 64;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) rc4;
            ctx->decode_stream = (stream_cipher_t) rc4;

            break;
        
        case  RC4_128:

            ret=rc4_init(&ctx->a.rc4,128,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 128;
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) rc4;
            ctx->decode_stream = (stream_cipher_t) rc4;

            break;

        case  SALSA20_128:

            ret=salsa20_start(&ctx->a.sal,128,key,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 128;            
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) salsa20;
            ctx->decode_stream = (stream_cipher_t) salsa20;

            break;

        case  SALSA20_256:

            ret=salsa20_start(&ctx->a.sal,256,key,iv);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_STREAM;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = 256;            
            ctx->is_stream = TRUE;

            ctx->encode_stream = (stream_cipher_t) salsa20;
            ctx->decode_stream = (stream_cipher_t) salsa20;

            break;

        case  TWOFISH_256_ECB:   bits += 64;
        case  TWOFISH_192_ECB:   bits += 64;
        case  TWOFISH_128_ECB:

            ret=twofish_init(&ctx->a.two,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = TWOFISH_BLOCK_SIZE;
            ctx->kbits  = bits;

            ctx->encode_block = (block_cipher_t) twofish_encode_block;
            ctx->decode_block = (block_cipher_t) twofish_decode_block;

            break;

        case  TWOFISH_256_CBC:   bits += 64;
        case  TWOFISH_192_CBC:   bits += 64;
        case  TWOFISH_128_CBC:
            
            ret=twofish_init(&ctx->a.two,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = TWOFISH_IV_SIZE;
            ctx->blk_sz = TWOFISH_BLOCK_SIZE;
            ctx->kbits  = bits;

            if (iv)
                memcpy(ctx->iv,iv,ctx->iv_sz);

            ctx->encode_block = (block_cipher_t) twofish_encode_block;
            ctx->decode_block = (block_cipher_t) twofish_decode_block;

            break;

        case  TWOFISH_256_CTR:   bits += 64;
        case  TWOFISH_192_CTR:   bits += 64;
        case  TWOFISH_128_CTR:

            ret=twofish_init(&ctx->a.two,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_CTR;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = TWOFISH_BLOCK_SIZE;
            ctx->ctr_count = counter;
            ctx->ctr_iv = CIPHER_CTR_IV_SIZE;

            if (iv)
                memcpy(ctx->iv,iv,ctx->ctr_iv);     

            ctx->encode_block = (block_cipher_t) twofish_encode_block;
            ctx->decode_block = (block_cipher_t) twofish_decode_block;

            break;

        case  TWOFISH_256_GCM:   bits += 64;
        case  TWOFISH_192_GCM:   bits += 64;
        case  TWOFISH_128_GCM:

            ret=twofish_init(&ctx->a.two,bits,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_AEAD;
            ctx->iv_sz  = 0;
            ctx->blk_sz = 0;
            ctx->kbits  = bits;
            ctx->is_stream = TRUE;
            ctx->gcm_blk = TWOFISH_BLOCK_SIZE;

            ctx->encode_block = (block_cipher_t) twofish_encode_block;
            ctx->decode_block = (block_cipher_t) twofish_decode_block;

            twofish_encode_block(&(ctx->a.two),ctx->gcm_h,ctx->gcm_h);

            break;

        case  XTEA_128_ECB:

            ret=xtea_init(&ctx->a.tea,XTEA_DEF_ROUNDS,key);
            if(ret)
                return ret;

            ctx->mode   = CIPHER_MODE_ECB;
            ctx->iv_sz  = 0;
            ctx->blk_sz = XTEA_BLOCK_SIZE;            
            ctx->kbits  = xtea_bits(&ctx->a.tea);

            ctx->encode_block = (block_cipher_t) xtea_encode_block;
            ctx->decode_block = (block_cipher_t) xtea_decode_block;

            break;

        case  XTEA_128_CBC:

            ret=xtea_init(&ctx->a.tea,XTEA_DEF_ROUNDS,key);
            if(ret)
                return ret;
            ctx->mode   = CIPHER_MODE_CBC;
            ctx->iv_sz  = XTEA_IV_SIZE;
            ctx->blk_sz = XTEA_BLOCK_SIZE;
            ctx->kbits  = 128;

            if (iv)
                memcpy(ctx->iv,iv,XTEA_IV_SIZE);

            ctx->encode_block = (block_cipher_t) xtea_encode_block;
            ctx->decode_block = (block_cipher_t) xtea_decode_block;

            break;

        default:
            return -1;
    }

    ctx->alg    = alg;
    ctx->pad    = PAD_NONE;
    ctx->namec  = alg_enc[alg].name;
    ctx->namew  = alg_enc[alg].namew;
    ctx->iv_len = ctx->blk_sz / sizeof(unsigned long);

    return 0;
}


/* -------------------------------------------- */

int cipher_set_padding(cipher_t *ctx,int mode)
{
    int old = PAD_NONE;

    if(ctx)
    {
        old = ctx->pad;

        if(mode > PAD_NONE && mode < PAD_COUNT)
        {
            switch(ctx->mode)
            {
                case CIPHER_MODE_ECB:
                case CIPHER_MODE_CBC:
                    ctx->pad = mode;
                    break;
                default:
                    break;
            }
        }
    }

    return old;
}

/* -------------------------------------------- */

int cipher_set_aead_params(cipher_t *ctx,const void *aead,size_t asz,const void *iv,size_t ivsz,int tagsz)
{
    if(ctx && ctx->mode == CIPHER_MODE_AEAD)
    {
        if(aead && iv)
        {
            ctx->dat_aead = aead;
            ctx->tam_aead = asz;
            ctx->dat_iv   = iv;
            ctx->tam_iv   = ivsz;
            ctx->tam_tag  = tagsz;
            return 0;
        }
    }
    return -1;
}

/* -------------------------------------------- */

int cipher_get_aead_tag(cipher_t *ctx,void *tag,int max)
{
    if(ctx && ctx->mode == CIPHER_MODE_AEAD)
    {
        if(max < ctx->tam_tag)
            return ERANGE;                    
        memcpy(tag,ctx->dat_tag,ctx->tam_tag);
        return 0;
    }
    return -1;
}

/* -------------------------------------------- */

int cipher_check_aead_tag(cipher_t *ctx,void *tag,int tam)
{
    if(ctx && (ctx->mode == CIPHER_MODE_AEAD) && tam)
    {
        if(tam == ctx->tam_tag)
            return memcmp(ctx->dat_tag,tag,tam) ? FALSE : TRUE;
    }
    return FALSE;
}

/* -------------------------------------------- */

void cipher_end(cipher_t *ctx)
{
    if(ctx)
        memset(ctx,0,sizeof(cipher_t));
}

/* -------------------------------------------- *
    Encoding/decoding for chaha20 + poly1305
 * -------------------------------------------- */

 static size_t cipher_chacha20poly1305(cipher_t *ctx,void *dest,const void *orig,size_t cnt,int decode)
{
    poly1305_t poly;
    size_t needed;
    unsigned char lenLE[8];
    unsigned char pad[POLY1305_SIZE]={0};

    memset(ctx->gcm_h,0,CHACHA_BLOCK_SIZE);
    chacha20_shuffle(&ctx->a.cha,ctx->dat_iv,0);
    chacha20(&ctx->a.cha,ctx->gcm_h,ctx->gcm_h,CHACHA_BLOCK_SIZE);

    if(!decode)
        chacha20(&ctx->a.cha,dest,orig,cnt);
    
    poly1305_init(&poly, ctx->gcm_h);
    memset(&pad, 0, sizeof(pad));

    poly1305_update(&poly, ctx->dat_aead, ctx->tam_aead);
    needed = ctx->tam_aead % POLY1305_SIZE;
    if (needed)
        poly1305_update(&poly, pad, POLY1305_SIZE  - needed);

    poly1305_update(&poly,(decode) ? orig : dest, cnt);
    needed = cnt % POLY1305_SIZE ;
    if (needed)
        poly1305_update(&poly, pad, POLY1305_SIZE  - needed);

    put_le64(lenLE,ctx->tam_aead);
    poly1305_update(&poly, lenLE, 8);

    put_le64(lenLE,cnt);
    poly1305_update(&poly, lenLE, 8);

    poly1305_final(&poly, ctx->dat_tag);

    if(decode)
        chacha20(&ctx->a.cha,dest,orig,cnt);

    return cnt;
}

/* -------------------------------------------- *
   Encoding/decoding in CTR mode for all cipher
   algorithms
 * -------------------------------------------- */

static size_t cipher_ctr_transform(cipher_t *ctx,void *dest,const void *orig, size_t tam)
{
    unsigned char tmp[CIPHER_MAX_BLOCK_SIZE], *dst,*org;    
    size_t cnt,quedan = tam;

    org = (unsigned char *) orig;
    dst = (unsigned char *) dest;

    while( quedan > 0 )
    {
        memcpy(tmp,ctx->iv,ctx->ctr_iv);
        put_be32(tmp + ctx->ctr_iv,ctx->ctr_count++);

        ctx->encode_block(&(ctx->a.gen),tmp,tmp);
        
        cnt = ( quedan < ctx->gcm_blk ) ? quedan : ctx->gcm_blk;

        if(org != dst)
            memcpy(dst,org,cnt);
        mem_xor(dst,tmp,cnt);

        org+=cnt;
        dst+=cnt;
        quedan-=cnt;
    }

    return tam;
}

/* -------------------------------------------- *
   Encoding decoding in GCM mode for all cipher
   algorithms
 * -------------------------------------------- */

static size_t cipher_gcm_transform(cipher_t *ctx,void *dest,const void *orig, size_t tam,int decode)
{
    unsigned char nonce[CIPHER_MAX_BLOCK_SIZE]={0},tmp[CIPHER_MAX_BLOCK_SIZE]={0};
    unsigned char hash[CIPHER_MAX_BLOCK_SIZE]={0};
    unsigned int non;
    size_t quedan,cnt;
    unsigned char *buffer,*final;

    if(ctx->tam_iv == CIPHER_AEAD_IV_SIZE)
    {
        memcpy(nonce,ctx->dat_iv,CIPHER_AEAD_IV_SIZE);
        put_be32(nonce + CIPHER_AEAD_IV_SIZE,1);
    }
    else
    {
        quedan = ctx->tam_iv;
        buffer = (unsigned char *)ctx->dat_iv;

        memset(nonce,0,ctx->gcm_blk);

        while( quedan > 0 )
        {
            cnt = ( quedan < ctx->gcm_blk ) ? quedan : ctx->gcm_blk;

            mem_xor(nonce,buffer,cnt);
            gcm_mult(nonce,nonce,ctx->gcm_h,ctx->gcm_blk);

            quedan-=cnt;
            buffer+=cnt;
        }
        
        put_be32(tmp + CIPHER_AEAD_IV_SIZE,ctx->tam_iv * 8);

        mem_xor(nonce,tmp,ctx->gcm_blk);
        gcm_mult(nonce,nonce,ctx->gcm_h,ctx->gcm_blk);
    }

    /* build tag */

    ctx->encode_block(&(ctx->a.gen),ctx->dat_tag,nonce);

    /* Authentication data */

    quedan = ctx->tam_aead;
    buffer = (unsigned char *)ctx->dat_aead;

    while(quedan > 0)
    {
        cnt = (quedan < ctx->gcm_blk ) ? quedan : ctx->gcm_blk;
        memcpy(tmp,buffer,cnt);
        mem_xor(hash, tmp,cnt);
        gcm_mult(hash,hash,ctx->gcm_h,ctx->gcm_blk);

        buffer+=cnt;
        quedan-=cnt;
    }

    /* lets cypher/decypher */

    buffer = (unsigned char *)orig;
    final  = (unsigned char *)dest;

    non = get_be32(nonce + CIPHER_AEAD_IV_SIZE);

    quedan=tam;
    while (quedan > 0)
    {

        put_be32(nonce + CIPHER_AEAD_IV_SIZE,++non);

        cnt = (quedan < ctx->gcm_blk ) ? quedan : ctx->gcm_blk;

        if(decode)
        {
            mem_xor(hash,buffer,cnt);
            gcm_mult(hash,hash,ctx->gcm_h,ctx->gcm_blk);
        }

        ctx->encode_block(&(ctx->a.gen),tmp,nonce);

        mem_xor(tmp,buffer,cnt);
        memcpy(final,tmp,cnt);

        if(!decode)
        {
            mem_xor(hash,tmp,cnt);
            gcm_mult(hash,hash,ctx->gcm_h,ctx->gcm_blk);
        }

        buffer+=cnt;
        final+=cnt;
        quedan-=cnt;
    }

    /* Add counters */

    put_be64(tmp,ctx->tam_aead * 8);
    put_be64(tmp + 8,tam * 8);

    mem_xor(tmp,hash,ctx->gcm_blk);
    gcm_mult(nonce,tmp,ctx->gcm_h,ctx->gcm_blk);

    /* finish the Tag */
    
    mem_xor(ctx->dat_tag,nonce, ctx->gcm_blk);    

    return tam;
}

/* -------------------------------------------- *
    The XORs part for encoding and decoding in 
    CBC mode is based on two facts:

    1.- All cipher block sizes are multiples of 
        both 4 and 8, which are the sizes that
        unsigned long will have depending on 
        the compilation mode.
    
    2.- like all data have the same endian,
        this will work independantly of the
        endian of the machine.
 * -------------------------------------------- */

size_t cipher_encode(cipher_t *ctx,void *dest,const void *orig,size_t tam)
{
    size_t topad,blk,rem,t,ret = 0;
    unsigned char *sblk,*dblk,pad,*pblk;
    unsigned char cbc[CIPHER_MAX_BLOCK_SIZE];
    unsigned long *xd,*xs,*xi;            

    if (ctx && dest && orig)
    {
        if (ctx->is_stream)
        {
            switch(ctx->mode)
            {
                case CIPHER_MODE_CTR:
                    return cipher_ctr_transform(ctx,dest,orig,tam);                        
                case CIPHER_MODE_AEAD:
                    if(ctx->alg == CHACHA20_POLY_1305)
                        return cipher_chacha20poly1305(ctx,dest,orig,tam,FALSE);  
                    else if(!ctx->encode_stream)
                        return cipher_gcm_transform(ctx,dest,orig,tam,FALSE);                                    
                default:
                    ctx->encode_stream(&(ctx->a.gen),dest,orig,tam);            
                    break;            
            }    
            return tam;         
        }

        sblk = (unsigned char *)orig;
        dblk = (unsigned char *)dest;

        blk = tam / ctx->blk_sz;
        rem = tam % ctx->blk_sz;

        if(ctx->mode == CIPHER_MODE_CBC)
        {
            xi = (unsigned long *) ctx->iv;            
            xd = (unsigned long *) cbc;
        }

        while(blk > 0)
        {             
            if(ctx->mode == CIPHER_MODE_CBC)
            {                
                xs = (unsigned long *) sblk;
    
                for(t = 0; t < ctx->iv_len; t++)
                    xd[t] = xs[t] ^ xi[t];    

                ctx->encode_block(&ctx->a.gen,dblk,cbc);

                memcpy(ctx->iv,dblk,ctx->blk_sz);
            }
            else
            {
                ctx->encode_block(&ctx->a.gen,dblk,sblk);
            }

            sblk += ctx->blk_sz;
            dblk += ctx->blk_sz;
            ret  += ctx->blk_sz;

            blk--;
        }

        if(ctx->pad != PAD_NONE)
        {                                                
            topad = (size_t) (ctx->blk_sz - rem);
            pad = (unsigned char) (topad & 0xff);

            pblk = dblk;

            if(rem > 0)
            {
                if(dblk != sblk)
                    memmove(dblk,sblk,rem);
                dblk += rem;
                ret  += rem;
            }
            
            ret += topad--;

            switch(ctx->pad)
            {
                case PAD_ZEROS:
                case PAD_ONES:
                    dblk[0] = pad;
                    memset(dblk + 1,(ctx->pad == PAD_ZEROS) ? 0x00 : 0xff,topad);
                    break;
                case PAD_SIZE:
                    dblk[topad] = pad;
                    memset(dblk,pad,topad);
                    break;
                default:
                    break;
            }

            if(ctx->mode == CIPHER_MODE_CBC)
            {                
                xs = (unsigned long *) pblk;
    
                for(t = 0; t < ctx->iv_len; t++)
                    xd[t] = xs[t] ^ xi[t];    

                ctx->encode_block(&ctx->a.gen,pblk,cbc);

                memcpy(ctx->iv,pblk,ctx->blk_sz);
            }
            else
            {
                ctx->encode_block(&ctx->a.gen,pblk,pblk);            
            }            
        }
    }
    return ret;
}

/* -------------------------------------------- */

size_t cipher_decode(cipher_t *ctx,void *dest,const void *orig,size_t tam)
{
    size_t blk,ret = 0, t;
    unsigned char *sblk,*dblk,*pblk=NULL;
    unsigned char cbc[CIPHER_MAX_BLOCK_SIZE];
    unsigned long *xd,*xi;            

    if (ctx && dest && orig)
    {
        if (ctx->is_stream)
        {
            switch(ctx->mode)
            {
                case CIPHER_MODE_CTR:
                    return cipher_ctr_transform(ctx,dest,orig,tam);                        
                case CIPHER_MODE_AEAD:
                    if(ctx->alg == CHACHA20_POLY_1305)
                        return cipher_chacha20poly1305(ctx,dest,orig,tam,TRUE);  
                    else if(!ctx->decode_stream)
                        return cipher_gcm_transform(ctx,dest,orig,tam,TRUE);                                                
                default:
                    ctx->decode_stream(&(ctx->a.gen),dest,orig,tam);            
                    break;
            }    
            return tam;
        }

        sblk = (unsigned char *)orig;
        dblk = (unsigned char *)dest;        

        blk = tam / ctx->blk_sz;        

        if(ctx->mode == CIPHER_MODE_CBC)
            xi = (unsigned long *) ctx->iv;            

        while(blk > 0)
        {
            if(ctx->mode == CIPHER_MODE_CBC)
            {                                
                memcpy(cbc,sblk,ctx->blk_sz);

                ctx->decode_block(&ctx->a.gen,dblk,cbc);

                xd = (unsigned long *) dblk;
    
                for(t = 0; t < ctx->iv_len; t++)
                    xd[t] ^= xi[t];    
                
                memcpy(ctx->iv,cbc,ctx->blk_sz);                
            }
            else
            {
                ctx->decode_block(&ctx->a.gen,dblk,sblk);
            }

            pblk = dblk;

            sblk += ctx->blk_sz;
            dblk += ctx->blk_sz;
            ret  += ctx->blk_sz;

            blk--;
        }

        if(ctx->pad != PAD_NONE && pblk)
        {
            int len,tmp;
            unsigned char pad;
            
            len = ctx->blk_sz;            

            switch(ctx->pad)
            {
                case PAD_ZEROS:
                    pad = 0;
                    while(pblk[--len] == 0x00 && len > 0)
                        pad++;
                    if(pblk[len] != pad)
                        return (size_t) -1;                        
                    ret-=pad;
                    break;
                case PAD_ONES:
                    pad = 0;
                    while(pblk[--len] == 0xff && len > 0)
                        pad++;                        
                    if(pblk[len] != pad)
                        return (size_t) -1;                        
                    ret-=pad;
                    break;
                case PAD_SIZE:                
                    tmp = pad = pblk[--len];            
                    while(--tmp > 0)
                    {
                        if(pblk[--len] != pad)
                            return (size_t) -1;                        
                    }                          
                    ret-=pad;              
                    break;
                default:
                    break;
            }
        }
    }

    return ret;
}

/* -------------------------------------------- */

size_t cipher_pad_needed(cipher_t *ctx,size_t tam)
{
    if(ctx && tam && ctx->pad != PAD_NONE)
    {
        size_t blk = (size_t) ctx->blk_sz;
        
        return blk - ((tam + 1) % blk);
    }
    return 0;
}

/* ------------------------------ */

