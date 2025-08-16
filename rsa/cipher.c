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
    
    {DES_ECB     , "DES-ECB",     L"DES-ECB"},  {DES_CBC     , "DES-CBC",     L"DES-CBC"},
    {DES_EDE3_ECB, "DES-EDE3"  ,  L"DES-EDE3"}, {DES_EDE3_CBC, "DES-EDE3-CBC",L"DES-EDE3-CBC"},
    
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

        case AES_128_ECB:
        case AES_192_ECB:
        case AES_256_ECB:
        case AES_128_CBC:
        case AES_192_CBC:
        case AES_256_CBC:       
            return AES_BLOCK_SIZE;         

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

        case AES_128_CBC:
        case AES_192_CBC:
        case AES_256_CBC:
            return AES_IV_SIZE;

        case AES_128_CTR:
        case AES_192_CTR:
        case AES_256_CTR:
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
                    if(!ctx->encode_stream)
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
                    if(!ctx->decode_stream)
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

