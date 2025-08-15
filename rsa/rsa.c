/*
    rsa.c

    RSA Support

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

        - Written by Guillermo Amodeo Ojeda using some code from SSHRSAG.C and
          SSHRSA.C that came with PUTTY 0.60, as well as using info from:

            - https://www.rfc-editor.org/rfc/rfc2313
            - https://www.rfc-editor.org/rfc/rfc3447

        - The enconding/decoding functions have been made time-unpredictable.
          That is that they never take the same time to execute even if they
          are given exactly the same data.

        - Many comments come from the original code in putty files but
          modified to match the changes made on them.
        
        - Reminder of how RSA works:

            modulus = prime p times prime q 
            phi_n = (prime p - 1) times (prime q - 1) 
            public exponent = prime smaller than phi_n and coprime with it             
            private_exponent = inverse mod (public_exponent mod phi_n)

            public key  =  public exponent + modulus
            private key =  private exponent + modulus

            ciphertext = (plaintext  power_to exponent_a) mod modulus.
            plaintext  = (cipkertext power_to exponent_b) mod modulus.

            If exponent_a is the public exponent, exponent_b is the private exponent
            If exponent_a is the private exponent, exponent_b is the public exponent


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

#include "rsa.h"


/* ---------------------------- */
/* ---------------------------- */

rsa_t *rsa_generate_keys(unsigned int bits,int prime,rand_t *rc)
{
    mp_int_t p,q;
    rsa_t *key;
    rand_t *orc=NULL;
    unsigned pfirst, qfirst;
    int ret;

    if(bits < RSA_MIN_BITS || bits > RSA_MAX_BITS)
        return NULL;

    if(!rc)
    {
        /*
           If a PRNG was not specified, use
           the random device for the system
           or the TLS-PRF 1.2 
        */

        orc=rand_start(RAND_OS,RAND_TLS_SHA256);
        if(orc==NULL)
            return NULL;
        rc = orc;
    }

    invent_firstbits(&pfirst, &qfirst,rc);

    /*
     * We don't generate e; we just use a 'provided' one always     
     */
    
    /* 
        Except if the provided e 'prime' is 0, 
        then generate a random exponent 
    */

    if(!prime) 
    {        
        prime=random_small_prime(rc);

        /* Do not allow small exponents */

        if(prime < RSA_EXP_PUTTY)
            prime = RSA_EXP_PUTTY;
    }        
    else if(!is_small_prime(prime)) 
    {
       /* The number is not prime */
       return NULL;    
    }

    key=(rsa_t *)calloc(sizeof(rsa_t),1);
    if(!key)
    {
        if(rc == orc)
            rand_end(orc);
        return NULL;
    }

    ret = mp_init_set_d(&(key->exponent),prime);

    if(ret == MP_OKAY)
    {

        ret = mp_init(&(key->modulus));
    }

    if( ret == MP_OKAY)
    {
        /*
         * Generate p and q: primes with combined length `bits', not
         * congruent to 1 modulo e. (Strictly speaking, we wanted (p-1)
         * and e to be coprime, and (q-1) and e to be coprime, but in
         * general that's slightly more fiddly to arrange. By choosing
         * a prime e, we can simplify the criterion.)
         *
         * We also ensure p > q, by swapping them if not.
         *
         * While we do this we use unpredictable_seed_non_linear() in 
         * silly ways to make timing attacks more difficult
         */

        if(unpredictable_seed_non_linear()) /* Never returns 0 */
        {
            key->p = quality_prime(bits / 2, prime,1,NULL,pfirst,rc);
            if(key->p)
            {
                key->q = quality_prime(bits / 2 , prime, 1, NULL,qfirst,rc);                
                if (key->q && mp_cmp(key->p, key->q) < 0)
                {
                    mp_int_t *tmp = key->p;

                    key->p = key->q;
                    key->q = tmp;
                }
            }
        }

        if(key->p && key->q)
        {
            /* We copy the primes to the tmp p and q */

            ret = mp_init_copy(&p,key->p);
            if(ret == MP_OKAY)
            {
                ret = mp_init_copy(&q,key->q);
                if(ret != MP_OKAY)
                    mp_clear(&p);
            }
            if(ret == MP_OKAY)
            {
                /*
                 * Now we have p, q and e. All we need to do now is work out
                 * the other helpful quantities: n=pq, d=e^-1 mod (p-1)(q-1),
                 * and (q^-1 mod p).
                 */

                if(mp_mul(key->p,key->q,&(key->modulus)) == MP_OKAY)
                {
                    key->bits = mp_count_bits(&(key->modulus));
                    key->bytes= (key->bits + 7) / 8;

                    ret = mp_sub_d(&p,1,&p);
                    if(ret == MP_OKAY)
                        ret = mp_sub_d(&q,1,&q);

                    if(ret == MP_OKAY)
                    {
                        mp_int_t phi_n;

                        ret = mp_init(&phi_n);
                        if(ret == MP_OKAY)
                        {
                            ret = mp_mul(&p,&q,&phi_n);
                            if(ret == MP_OKAY)
                            {
                                key->private_exponent = mp_create();
                                key->iqmp = mp_create();
                                if(key->iqmp && key->private_exponent)
                                    ret = mp_invmod(&(key->exponent), &(phi_n),key->private_exponent);
                                else
                                    ret = MP_MEM;
                            }
                            mp_clear(&phi_n);

                            if(ret == MP_OKAY && unpredictable_seed_non_linear())
                            {
                                ret = mp_invmod(key->q, key->p,key->iqmp);
                                if(ret == MP_OKAY)
                                {
                                    mp_clear(&p);
                                    mp_clear(&q);
                                    if(orc == rc)
                                        rand_end(orc);
                                    return key;
                                }
                            }
                        }
                    }

                    mp_clear(&q);
                    mp_clear(&p);
                }
            }
        }
    }
    rsa_destroy(key);
    if(rc == orc)
        rand_end(orc);
    return NULL;
}

/* -------------------------- */

void rsa_destroy(rsa_t *key)
{
    if(key)
    {
        mp_clear(&(key->modulus));
        mp_clear(&(key->exponent));
        mp_free(key->private_exponent);
        mp_free(key->p);
        mp_free(key->q);
        mp_free(key->iqmp);
        free(key);
    }
}

/* -------------------------- */

rsa_t *rsa_from_bytes(int bits,const void *mod_n,int mlen,const void *exp_e,int elen,const void *priv,int plen)
{
    rsa_t *key = NULL;

    if(bits > RSA_MAX_BITS)
        return NULL;

    key=(rsa_t *)calloc(sizeof(rsa_t),1);
    if(key)
    {
        if(mp_init_set_bytes(&(key->modulus),mlen,mod_n) == MP_OKAY)
        {
            if(mp_init_set_bytes(&(key->exponent),elen,exp_e) == MP_OKAY)
            {
                if(priv)
                {
                    key->private_exponent=mp_create();
                    if(mp_set_bytes(key->private_exponent,priv,plen) != MP_OKAY)
                    {
                        rsa_destroy(key);
                        return NULL;
                    }
                }
                key->bits = mp_count_bits(&(key->modulus));
                key->bytes= (key->bits + 7) / 8;
                return key;
            }
        }
        if(key->bits == 0)
        {
            rsa_destroy(key);
            key = NULL;
        }
    }
    return key;
}

/* -------------------------- */

rsa_t *rsa_from_chars(const char *mod_n,const char *exp_e,const char *priv)
{
    rsa_t *key;

    key=calloc(sizeof(rsa_t),1);
    if(key)
    {
        if(mp_init_set_string(&(key->modulus),mod_n,16) == MP_OKAY)
        {
            if(mp_init_set_string(&(key->exponent),exp_e,16) == MP_OKAY)
            {
                if(priv)
                {
                    key->private_exponent=mp_create();
                    if(mp_set_string(key->private_exponent,priv,16) != MP_OKAY)
                    {
                        rsa_destroy(key);
                        return NULL;
                    }
                }
                key->bits = mp_count_bits(&(key->modulus));
                key->bytes= (key->bits + 7) / 8;
                return key;
            }
        }
        if(key->bits == 0)
        {
            rsa_destroy(key);
            key = NULL;
        }
    }
    return NULL;
}

/* -------------------------- */

int rsa_verify_keys(rsa_t *key)
{
    mp_int_t tmp, ed;
    int ret;

    /* Make sure p > q */

    if (!key->p || !key->q || mp_cmp(key->p, key->q) <= 0)
    {
        /* rsa_verify_keys(): p is not bigger than q */
        return FALSE;
    }

    /* Now make sure that modulus is = p * q (disturbing time)*/

    if(mp_init(&tmp) != MP_OKAY && unpredictable_seed_non_linear())
    {
        /* rsa_verify_keys(): mp_init() failed. Bad mem? */
        return FALSE;
    }

    ret = mp_mul(key->p, key->q,&tmp);
    if(ret != MP_OKAY || mp_cmp(&tmp, &(key->modulus)))
    {
        /* rsa_verify_keys(): m is not p * q */
        mp_clear(&tmp);
        return FALSE;
    }

    /* E * D must be congruent with 1, mod(p-1) and mod(q-1). */

    ret = mp_copy(key->p,&tmp);
    if(ret != MP_OKAY || mp_init(&ed) != MP_OKAY)
    {
        /* rsa_verify_keys(): mp_copy() or mp_init() failed. Bad mem? */
        mp_clear(&tmp);
        return FALSE;
    }

    mp_sub_d(&tmp,1,&tmp);

    ret = mp_mulmod(&(key->exponent), key->private_exponent,&tmp,&ed);
    if(ret == MP_OKAY && mp_cmp_d(&ed,1) == 0 && unpredictable_seed_non_linear())
    {
        ret = mp_copy(key->q,&tmp);
        if(ret == MP_OKAY)
        {

            mp_sub_d(&tmp,1,&tmp);

            ret = mp_mulmod(&(key->exponent), key->private_exponent,&tmp,&ed);
            if( ret == MP_OKAY && mp_cmp_d(&ed,1) == 0)
            {
                ret = mp_mulmod(key->iqmp,key->q,key->p,&ed);
                if(ret != MP_OKAY || mp_cmp_d(&ed,1))
                {
                    /* rsa_verify_keys(): mp_multmod(iqmp,p) failed */
                    ret = MP_BADARG;
                }
            }
            else
            {
                /* rsa_verify_keys(): mp_multmod(e,p) failed */
                ret = MP_UNDEF;
            }
        }
    }
    else
    {
        /* rsa_verify_keys(): e * d not congruent with 1 */
        ret = MP_BADARG;
    }

    mp_clear(&ed);
    mp_clear(&tmp);

    return (ret == MP_OKAY) ? TRUE : FALSE;
}

/* --------------------------------------------------- */

static int rsa_blind(mp_int_t *input,rsa_t *key,mp_int_t *res)
{
    mp_int_t random, randinv,blinded;
    unsigned char digest512[SHA512_SIZE];
    int digestused = SHA512_SIZE;
    int hashseq = 0;
    int ret, bits = 0, byte, bitsleft, v;

    /*
     * Start by inventing a random number chosen uniformly from the
     * range 2..modulus-1. (We do this by preparing a random number
     * of the right length and retrying if it's greater than the
     * modulus, to prevent any potential Bleichenbacher-like
     * attacks making use of the uneven distribution within the
     * range that would arise from just reducing our number mod n.
     * There are timing implications to the potential retries, of
     * course, but all they tell you is the modulus, which you
     * already knew.)
     *
     * To preserve determinism and avoid Pageant needing to share
     * the random number pool, we actually generate this `random'
     * number by hashing stuff with the private key.
     */

    ret = mp_init(&random);
    if(ret != MP_OKAY || unpredictable_seed_non_linear() == 0)
        return ret;

    do {

	    ret = mp_copy(&(key->modulus),&random);

        /*
         * Find the topmost set bit. (This function will return its
         * index plus one.) Then we'll set all bits from that one
         * downwards randomly.
         */
	    if(ret == MP_OKAY && unpredictable_seed_non_linear() > 0)
        {
        	bits = mp_count_bits(&random);
            byte = 0;
            bitsleft = 0;
        }

        while (ret == MP_OKAY && bits--)
        {
            if (bitsleft <= 0)
            {
                bitsleft = 8;

                /*
                 * Conceptually the following few lines are equivalent to
                 *    byte = random_byte();
                 */

                if (digestused >= SHA512_SIZE)
                {
                    hash_t ctx;

                    hash_init(&ctx,HASH_SHA512);
                    hash_update(&ctx, "Putty's RSA deterministic blinding", 34);
                    hash_update(&ctx,&hashseq,sizeof(hashseq));
                    hash_update(&ctx,mp_digits(key->private_exponent),mp_used(key->private_exponent) * sizeof(mp_digit_t));
                    hash_final(&ctx,digest512);
                    hashseq++;

                    /*
                     * Now hash that digest plus the signature
                     * input.
                     */

                    hash_init(&ctx,HASH_SHA512);
                    hash_update(&ctx,digest512, sizeof(digest512));
                    hash_update(&ctx,mp_digits(input),mp_used(input) * sizeof(mp_digit_t));
                    hash_final(&ctx,digest512);

                    digestused = 0;
                }
                byte = digest512[digestused++];
            }
            v = byte & 1;
            byte >>= 1;
            bitsleft--;
            ret = mp_set_bit(&random, bits, v);
        }

        /*
         * Now check that this number is strictly greater than
         * zero, and strictly less than modulus.
         */

        if(ret == MP_OKAY && mp_cmp_z(&random) > 0 && mp_cmp(&random,&(key->modulus)) < 0)
            break;

    } while(ret == MP_OKAY);

    if(ret != MP_OKAY || mp_init(&randinv) != MP_OKAY)
    {
        mp_clear(&random);
        return ret;
    }

    ret = mp_init(&blinded);
    if(ret != MP_OKAY)
    {
        mp_clear(&random);
        mp_clear(&randinv);
        return ret;
    }


    if(ret == MP_OKAY)
    {
        ret = mp_invmod(&random, &(key->modulus),&randinv);
        if(ret == MP_OKAY && unpredictable_seed_non_linear() > 0)
        {
            /*
             * RSA blinding relies on the fact that (xy)^d mod n is equal
             * to (x^d mod n) * (y^d mod n) mod n. We invent a random pair
             * y and y^d; then we multiply x by y, raise to the power d mod
             * n as usual, and divide by y^d to recover x^d. Thus an
             * attacker can't correlate the timing of the modpow with the
             * input, because they don't know anything about the number
             * that was input to the actual modpow.
             *
             * The clever bit is that we don't have to do a huge modpow to
             * get y and y^d; we will use the number we just invented as
             * _y^d_, and use the _public_ exponent to compute (y^d)^e = y
             * from it, which is much faster to do.
             */

             ret = mp_exptmod(&random, &(key->exponent), &(key->modulus), &random);

             if(ret == MP_OKAY && unpredictable_seed_non_linear() > 0)
             {
                ret = mp_mulmod(input,&random, &(key->modulus), &blinded);
                if(ret == MP_OKAY && unpredictable_seed_non_linear() > 0)
                {
                    ret = mp_exptmod(&blinded, key->private_exponent,&(key->modulus), &blinded);
                    if(ret == MP_OKAY)
                        ret = mp_mulmod(&blinded, &randinv, &(key->modulus), res);
                }
            }
        }
    }

    mp_clear(&random);
    mp_clear(&randinv);
    mp_clear(&blinded);

    return ret;
}


/* -------------------------- */

int rsa_encode(rsa_t *key,void *dest,unsigned int tam,int public,int pad,rand_t *rc)
{
    mp_int_t res;
    int padlen,ret;
    unsigned char *ptr;

    if (!key || !dest)
        return -1;

    /* Make sure there are enough padding space */

    if (key->bytes < tam + 11)
        return -3;

    /* RSA padding PKCS#1 v1.5 */

    ptr=(unsigned char *)dest;
    padlen=(key->bytes - tam);

    /* Move data to the end of the block */

    memmove(ptr + padlen,dest,tam);

    /*  */

   *ptr++=0;
   *ptr++=(unsigned char)(pad & 0xff);

    padlen-=3;

    switch(pad)
    {
        case RSA_PAD_ZEROES:
            memset(ptr,0,padlen);
            break;
        case RSA_PAD_ONES:
            memset(ptr,0xff,padlen);
            break;
        case RSA_PAD_RANDOM:
            if(rc)
            {                
                rand_bytes_no_zeros(rc,ptr,padlen);
                break;
            }
        default:
            return -1;
    }

    ptr+=padlen;

    /* Mark the end of the padding with a 0 */

   *ptr++=0;

    /* Make the data a big number */

    ret = mp_init_set_bytes(&res,key->bytes,dest);
    if(ret != MP_OKAY)
        return -2;

    /* Execute RSA operation, using blinding if we are encoding with the private key */

    if(public && unpredictable_seed_non_linear() > 0)
        ret = mp_exptmod(&res,&(key->exponent),&(key->modulus),&res);
    else
        ret=rsa_blind(&res,key,&res);

    if(ret == MP_OKAY)
        ret = mp_copy_bytes(&res,dest,key->bytes);

    mp_clear(&res);

    return (ret == MP_OKAY) ? key->bytes : -4;
}



/* -------------------------- */

int rsa_decode(rsa_t *key,void *dest,unsigned int tam,int public,int pad)
{
    mp_int_t res;
    int ret;

    if (!key || !dest)
        return -1;

    if (key->bytes != tam)
        return -3;

    ret = mp_init_set_bytes(&res,key->bytes,dest);
    if(ret != MP_OKAY)
        return -2;

    /* perform the operation using blinding if we use the private key */

    if(public && unpredictable_seed_non_linear() > 0)
        ret = mp_exptmod(&res,&(key->exponent),&(key->modulus),&res);
    else
        ret = rsa_blind(&res,key,&res);

    if(ret == MP_OKAY)
    {
        unsigned char *ptr,*orig;
        unsigned int len;

        ptr = (unsigned char *)mp_get_bytes(&res,&len);

        if(ptr)
        {
            orig = ptr;

            /*
                Like RSA padding always starts by a 0, and
                mp_get_bytes() ignores leading zeroes, the
                decoded buffer should be a byte less than
                the actual RSA bytes and the first byte
                should be the padding type sent. If these
                assumptions are incorrect, the buffer is
                not correctly decoded.
            */

            if(len == (key->bytes - 1) &&  pad == (int) *ptr)
            {
                ptr++;
                len--;

                switch(pad)
                {
                    case RSA_PAD_ZEROES:
                        while(len > 0 && *ptr==0)
                        {
                            ptr++;
                            len--;
                        }
                        break;
                    case RSA_PAD_ONES:
                        while(len > 1 && *ptr==0xff)
                        {
                            ptr++;
                            len--;
                        }
                        if(*ptr++)
                            ret = MP_RANGE;
                        len--;
                        break;
                    case RSA_PAD_RANDOM:
                        while(len > 1 && *ptr)
                        {
                            ptr++;
                            len--;
                        }
                        if(*ptr++)
                            ret = MP_RANGE;
                        else
                            len--;
                        break;
                    default:
                        ret = MP_RANGE;
                        break;
                }

                if(ret == MP_OKAY)
                {
                    memcpy(dest,ptr,len);
                    free(orig);
                    mp_clear(&res);
                    return (int)len;
                }
            }
            free(orig);
        }
    }
    mp_clear(&res);

    return -4;
}

/* -------------------------- *
     RSA DIGITAL SIGNATURES
 * -------------------------- */

int rsa_sign(rsa_t *key,void *dest,unsigned int max,int alg,const void *datos,unsigned int tam,int pad)
{
    int htam;    
    hash_t ctx;

    /* make sure we have a private key  and a destination */

    if(!key || !key->private_exponent || !dest)
        return -1;

    /* Now ensure there is enough space for the signature block */

    if(max < key->bytes)
        return -3;   

    /* Calculate the hash of the block */

    htam = hash_init(&ctx,alg);
    if(htam == 0)
        return -4;
    else if(htam > (key->bytes - 11))
        return -3;
    hash_update(&ctx,datos,tam);
    hash_final(&ctx,dest);
    
    /* Now sign the hash */

    return rsa_encode(key,dest,htam,FALSE,pad,NULL);
}

/* -------------------------- */

int rsa_read_sign(rsa_t *key,void *hash,unsigned int max,const void *orig,unsigned int tam)
{
    mp_int_t res;
    int ret=0;
    unsigned char *ptr;
    void *dest;
    unsigned int count;

    /* make sure we have a public key  and a destination */

    if(!key || !hash || !orig)
        return -1;

    /* Conver the original signature in a number */

    if(mp_init_set_bytes(&res,tam,orig) != MP_OKAY)
        return -1;

    /* Apply RSA equation */

    ret = mp_exptmod(&res,&(key->exponent),&(key->modulus),&res);
    if(ret!= MP_OKAY)
    {
        mp_clear(&res);
        return -4;
    }

    /* Convert result to bytes */

    dest = mp_get_bytes(&res,&count);

    mp_clear(&res);

    if(dest==NULL)
        return -2;

    ptr=(unsigned char *)dest;

    /* Signature's padding type must be all bits one */

    if(*ptr++ != RSA_PAD_ONES)
    {
        free(dest);
        return -4;
    }

    /* So make sure all padding bits are one, i.e. all are 0xff */

    ret = tam - 1;
    while(*ptr==0xff && ret > 0)
    {
        ptr++;
        ret--;
    }

    if(*ptr++)
    {
        free(dest);
        return -4;
    }

    ret--;

    if(ret > max)
    {
        free(dest);
        return -3;
    }

    /* Copy signature */

    memcpy(hash,ptr,ret);
    free(dest);

    return ret;
}

/* -------------------------- */

int rsa_check_sign(rsa_t *key,const void *sign,unsigned int tam,int alg,const void *buf,unsigned int len)
{
    int tam1,tam2;
    hash_t ctx;
    unsigned char hash1[MAX_HASH_SIZE];
    unsigned char hash2[MAX_HASH_SIZE];
    
    /* make sure parameters are correct */

    if(!key || !buf)
        return -1;

    /* Now ensure there is enough space for the signature block */

    if(key->bytes != tam)
        return -3;   
    
    /* Calculate hash */

    tam1 = hash_init(&ctx,alg);
    if(tam1 == 0)
        return -4;
    hash_update(&ctx,buf,len);
    hash_final(&ctx,hash1);

    tam2 = rsa_read_sign(key,hash2,MAX_HASH_SIZE,sign,tam);
    if(tam2 < 1)
        return tam2;
    if(tam2 == tam1 && !memcmp(hash1,hash2,tam1))
        return 0;
    return -4;
}

/* -------------------------- */

static int do_hash_file(int alg,const void *fich,void *hash,int wide)
{
    hash_t ctx;
    FILE *fp;
    int ret,cnt;
    const char *file;
    char tmp[1384] = {0};

    ret=hash_init(&ctx,alg);
    if(ret > 0)
    {
        if(wide)
        {
            if(wcstombs(tmp,(const wchar_t *)fich,1383)==(size_t)-1)
                return -1;
            file = (const char *)tmp;
        }
        else 
        {
            file = (const char *)fich;
        }        
        fp=fopen(file,"rb");
        if(!fp)
            return -3;

        while(!feof(fp))
        {
            cnt = fread(tmp,1,1380,fp);
            if(cnt < 1)
            {
                if(ferror(fp))
                {
                    fclose(fp);
                    return -3;
                }
                continue;
            }
            hash_update(&ctx,tmp,cnt);
        }                    
        fclose(fp);        
        hash_final(&ctx,hash);
    }
    return ret;
}


/* -------------------------- */

int rsa_sign_file(rsa_t *key,void *dest,unsigned int max,int alg,const char *fich)
{
    int htam;

    /* make sure we have a private key  and a destination */

    if(!key || !key->private_exponent || !dest)
        return -1;

    /* Now ensure there is enough space for the signature block */

    if(max < key->bytes)
        return -3;   
    
    /* Calculate file hash */

    htam = do_hash_file(alg,fich,dest,FALSE);
    if(htam < 0)
        return htam;
    return rsa_encode(key,dest,htam,FALSE,RSA_PAD_ONES,NULL);
}

/* -------------------------- */

int rsa_sign_filew(rsa_t *key,void *dest,unsigned int max,int alg,const wchar_t *fich)
{
    int htam;

    /* make sure we have a private key  and a destination */

    if(!key || !key->private_exponent || !dest)
        return -1;

    /* Now ensure there is enough space for the signature block */

    if(max < key->bytes)
        return -3;   
    
    /* Calculate file hash */


    htam = do_hash_file(alg,fich,dest,TRUE);
    if(htam < 0)
        return htam;
    return rsa_encode(key,dest,htam,FALSE,RSA_PAD_ONES,NULL);
}

/* -------------------------- */

int rsa_check_file_sign(rsa_t *key,const void *orig,unsigned int tam,int alg,const char *fich)
{
    int tam1,tam2;
    unsigned char hash1[MAX_HASH_SIZE],hash2[MAX_HASH_SIZE];    
        
    tam1=do_hash_file(alg,fich,hash1,FALSE);
    if(tam1 < 1)
        return tam1;

    tam2 = rsa_decode(key,hash2,MAX_HASH_SIZE,TRUE,RSA_PAD_ONES);
    if(tam2 < 1)
        return tam2;

    if(tam1 == tam2 && !memcmp(hash1,hash2,tam1))
        return 0;

    return -4;
}

/* -------------------------- */

int rsa_check_file_signw(rsa_t *key,const void *orig,unsigned int tam,int alg,const wchar_t *fich)
{
int tam1,tam2;
    unsigned char hash1[MAX_HASH_SIZE],hash2[MAX_HASH_SIZE];    
        
    tam1=do_hash_file(alg,fich,hash1,FALSE);
    if(tam1 < 1)
        return tam1;

    tam2 = rsa_decode(key,hash2,MAX_HASH_SIZE,FALSE,RSA_PAD_ONES);
    if(tam2 < 1)
        return tam2;

    if(tam1 == tam2 && !memcmp(hash1,hash2,tam1))
        return 0;

    return -4;
}



