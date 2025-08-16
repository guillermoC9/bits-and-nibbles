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
           or the TLS-PRF variation by GAO
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
         * While we do this we use unpredictable_seed_non_linear() in silly
         * ways to make timing attacks more difficult
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

/* RSAEncryption struct encoded in ASN.1/DER :

 30 0d -- Sequence of 13 bytes (0x0d)
    06 09 -- OID of 9 bytes
       2a 86 48 86 f7 0d 01 01 01 --  (1.2.840.113549.1.1.1)
    05 00 -- NULL (no params)
*/

static unsigned char rsa_oid[15]=
{
    0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x01,0x05,0x00
};

/* -------------------------- */

asn1_t *rsa_public_to_asn1(rsa_t *key)
{
    int tam_ints,tam_bits,tam_seq,tam;
    asn1_t *asn =NULL;

    if(key)
    {
        asn1_t *mod_n,*exp_e;

        mod_n = asn1_create_bignum(&(key->modulus));
        exp_e = asn1_create_bignum(&(key->exponent));

        if(mod_n && exp_e)
        {
            tam_ints=(mod_n->total + exp_e->total);

            tam_seq=asn1_calc_len(ASN1_SEQ,tam_ints,NULL) + 1;

            /* We use ASN1_BYTES count here instead
               ASN1_BITS because they are going to
               be a multiple of 8 and therefore there
               is no need to do the conversion from bits
               to bytes. ;-)
            */

            tam_bits=asn1_calc_len(ASN1_BYTES,tam_seq,NULL);

            /* Add the RSAEncryption's  OID and seq */

            asn = asn1_create(ASN1_SEQ,tam_bits + 15);

            if(asn)
            {
                memcpy(asn->data,rsa_oid,15);
                tam=15;

                asn->data[tam++]=ASN1_BITS;
                tam+=asn1_put_len(tam_seq,asn->data+tam);
                asn->data[tam++]=0x00;

                /* Sequence  */

                asn->data[tam++]=ASN1_SEQ;
                tam+=asn1_put_len(tam_ints,asn->data + tam);

                /* Modulus N */

                memcpy(asn->data + tam,mod_n->start,mod_n->total);
                tam+=mod_n->total;

                /* Exponent E */

                memcpy(asn->data + tam,exp_e->start,exp_e->total);
                tam+=exp_e->total;
            }
        }

        asn1_free(mod_n);
        asn1_free(exp_e);

    }
    return asn;
}

/* -------------------------- */

asn1_t *rsa_private_to_asn1(rsa_t *key,int pkcs8)
{
    int num,tam_ints=0,tam_bits=0,tam_seq=0,tam=0,t;
    asn1_t *asn=NULL,*tmp[8];

    if(key)
    {
        mp_int_t p1,q1,dmp1,dmq1;

        /*  PKCS-1 requires RSA private
            key structures to have the values
            'd mod p-1' and 'd mod q-1'
            populated, so we do calculate
            them only for storage, as we do
            not use them at all.
        */

        if(mp_init_copy(&p1,key->p) != MP_OKAY)
            return NULL;

        mp_sub_d(&p1,1,&p1);

        if(mp_init_copy(&q1,key->q) != MP_OKAY)
        {
            mp_clear(&p1);
            return NULL;
        }

        mp_sub_d(&q1,1,&q1);

        if(mp_init(&dmp1) != MP_OKAY)
        {
            mp_clear(&p1);
            mp_clear(&q1);
            return NULL;
        }

        if(mp_init(&dmq1) != MP_OKAY)
        {
            mp_clear(&dmp1);
            mp_clear(&p1);
            mp_clear(&q1);
            return NULL;
        }

        if(mp_mod(key->private_exponent,&p1,&dmp1) != MP_OKAY ||
           mp_mod(key->private_exponent,&q1,&dmq1) != MP_OKAY)
        {
            mp_clear(&dmp1);
            mp_clear(&dmq1);
            mp_clear(&p1);
            mp_clear(&q1);
            return NULL;
        }

        mp_clear(&p1);
        mp_clear(&q1);

        /* Go on and save the key */

        tmp[0]=asn1_create_bignum(&(key->modulus));
        tmp[1]=asn1_create_bignum(&(key->exponent));
        tmp[2]=asn1_create_bignum(key->private_exponent);
        tmp[3]=asn1_create_bignum(key->p);
        tmp[4]=asn1_create_bignum(key->q);
        tmp[5]=asn1_create_bignum(&dmp1);
        tmp[6]=asn1_create_bignum(&dmq1);
        tmp[7]=asn1_create_bignum(key->iqmp);

        mp_clear(&dmp1);
        mp_clear(&dmq1);

        /* Calculate size */

        for(num=t=0;t<8;t++)
        {
            if(tmp[t])
            {
                num++;
                tam_ints+=tmp[t]->total;
            }
        }

        /* Make sure they are all there */

        if(num == 8)
        {
            /* Add version's size */

            tam_ints += 3;

            if(pkcs8)
            {
                /* Make sure we are compliant with PKCS-8 if needed */

                tam_seq=asn1_calc_len(ASN1_SEQ,tam_ints,NULL) + 1;
                tam_bits=asn1_calc_len(ASN1_BYTES,tam_seq,NULL);
                tam = tam_bits + 18;
            }
            else
            {
                tam = tam_ints;
            }

            asn = asn1_create(ASN1_SEQ,tam);
            if(asn)
            {
                tam = 0;
                if(pkcs8)
                {
                    asn->data[tam++] = ASN1_INT;
                    asn->data[tam++] = 1;
                    asn->data[tam++] = 0;

                    memcpy(asn->data + tam,rsa_oid,15);
                    tam+=15;

                    asn->data[tam++]=ASN1_BITS;
                    tam+=asn1_put_len(tam_seq,asn->data+tam);
                    asn->data[tam++]=0x00;

                    asn->data[tam++]=ASN1_SEQ;
                    tam+=asn1_put_len(tam_ints,asn->data + tam);
                }

                asn->data[tam++] = ASN1_INT;
                asn->data[tam++] = 1;
                asn->data[tam++] = 0;

                for(t=0;t<8;t++)
                {
                    memcpy(asn->data + tam,tmp[t]->start,tmp[t]->total);
                    tam+=tmp[t]->total;
                }
            }
        }

        for(t=0;t<8;t++)
            asn1_free(tmp[t]);
    }
    return asn;
}

/* ---------------------- */

static int do_rsa_load_pem(pem_file_t *pem,int public,const char *passcode,rsa_t **rsa)
{
    int ret = -3;
    pem_elem_t *elem=NULL;

    rsa_t *tmp;

    if(pem)
    {
        if(public)
        {
            elem=pem_first_element(pem,PEM_PUBLIC_KEY);
        }
        else
        {
            elem=pem_first_element(pem,PEM_RSA_PRIVATE_KEY);
            if(!elem)
            {
                elem=pem_first_element(pem,PEM_PRIVATE_KEY);
                if(!elem)
                    elem=pem_first_element(pem,PEM_ENC_PRIVATE_KEY);
            }
        }
    }

    if(elem)
    {
        ret = pem_decode_element(elem,passcode);

        if(ret == 0 && (elem->flags & PEM_F_ASN1))
        {
            tmp=rsa_from_asn1(elem->data,elem->size);
            if(tmp)
            {
                /* If the key does not verify we destroy it and fail */

                if(rsa_verify_keys(tmp))
                {
                    *rsa=tmp;
                     ret = 0;
                }
                else
                {
                    rsa_destroy(tmp);
                    ret= -6;
                }
            }
        }
    }

    return ret;
}


/* ---------------------- */

int rsa_load_pem(const char *file,int public,const char *passcode,rsa_t **rsa)
{
    pem_file_t *pem;
    int ret = -1;

    if(rsa && file)
    {
       *rsa = NULL;

        ret = pem_open(&pem,file);
        if(ret == 0)
        {
            ret = do_rsa_load_pem(pem,public,passcode,rsa);
            pem_close(pem);
        }
    }
    return ret;
}

/* ---------------------- */

int rsa_load_pemw(const wchar_t *file,int public,const char *passcode,rsa_t **rsa)
{
    pem_file_t *pem;
    int ret = -1;

    if(rsa && file)
    {
       *rsa = NULL;

        ret = pem_openw(&pem,file);
        if(ret == 0)
        {
            ret = do_rsa_load_pem(pem,public,passcode,rsa);
            pem_close(pem);
        }
    }
    return ret;
}

/* ------------------------------- */

int rsa_public_to_pem(rsa_t *key,FILE *fp)
{
    asn1_t *asn;
    int ret = -1;

    if(key && fp)
    {
        asn = rsa_public_to_asn1(key);
        if(asn)
        {
            ret=pem_save_element(fp,PEM_PUBLIC_KEY,asn->start,asn->total,NULL,0);
            asn1_free(asn);
        }
        else
        {
            ret = -2;
        }
    }
    return ret;
}

/* ------------------------------- */

int rsa_private_to_pem(rsa_t *key,const char *passcode,int alg,FILE *fp)
{
    asn1_t *asn;
    int ret = -1;

    if(key && fp)
    {
        asn = rsa_private_to_asn1(key,FALSE);
        if(asn)
        {
            ret=pem_save_element(fp,PEM_RSA_PRIVATE_KEY,asn->start,asn->total,passcode,alg);
            asn1_free(asn);
        }
        else
        {
            ret = -2;
        }
    }
    return ret;
}

/* ------------------------------- */

int rsa_save_pem(const char *file,rsa_t *key,int priv,const char *passcode,int alg)
{
    int ret = -1;
    FILE *fp;

    if(key && file)
    {
        ret=-3;
        fp=fopen(file,"wb");
        if(fp)
        {
            if(priv)
                ret = rsa_private_to_pem(key,passcode,alg,fp);
            else
                ret = rsa_public_to_pem(key,fp);
            fflush(fp);
            fclose(fp);
        }
    }
    return ret;
}

/* ------------------------------- */

int rsa_save_pemw(const wchar_t *file,rsa_t *key,int priv,const char *passcode,int alg)
{
    int ret = -1;
    FILE *fp;

    if(key && file)
    {
        ret=-3;
        fp=fopenw(file,"wb");
        if(fp)
        {
            if(priv)
                ret = rsa_private_to_pem(key,passcode,alg,fp);
            else
                ret = rsa_public_to_pem(key,fp);
            fflush(fp);
            fclose(fp);
        }
    }
    return ret;
}

/* -------------------------- */

rsa_t *rsa_from_seq_asn1(const void *bytes,int tam)
{
    asn1_seq_t seq,pub,pri;
    asn1_t asn;
    rsa_t *ret = NULL;

    if(!asn1_seq_read(&seq,bytes,tam))
    {
        if(seq.num == 2 && seq.lst[0].type == ASN1_SEQ && seq.lst[1].type==ASN1_BITS)
        {
            /* Public RSA PKCS-8 */
            
            if(!asn1_seq_read(&pub,seq.lst[0].data,seq.lst[0].len))
            {
                ret = rsa_public_key_from_asn1(&seq,&pub);
                asn1_seq_free(&pub);                
            }
        }
        else if(seq.num == 3 && seq.lst[2].type == ASN1_INT && seq.lst[1].type == ASN1_SEQ && seq.lst[2].type == ASN1_BITS)
        {
            /* Private RSA PKCS-8 */

            if(asn1_read(&asn,seq.lst[2].data,seq.lst[2].len) > 0 && asn.type == ASN1_SEQ)
            {
                if(!asn1_seq_read(&pri,asn.data,asn.len) && pri.num > 8)
                {
                    ret = rsa_private_key_from_asn1(&pri);
                    asn1_seq_free(&pri);
                }
                asn1_free(&asn);
            }
        }
        else if(seq.num > 8)
        {
            /* Clave Privada RSA PKCS-1 */

            ret = rsa_private_key_from_asn1(&seq);
        }
        asn1_seq_free(&seq);
    }
    return ret;
}

/* -------------------------- */

rsa_t *rsa_from_asn1(const void *bytes,int tam)
{
    asn1_t asn;

    if(asn1_read(&asn,bytes,tam) < 1 || asn.type != ASN1_SEQ)
        return NULL;
    return rsa_from_seq_asn1(asn.data,asn.len);
}

/* -------------------------- */

/* Clave Privada RSA PKCS-1 */
            
rsa_t *rsa_private_key_from_asn1(asn1_seq_t *seq)
{    
    rsa_t *ret = NULL;
    int t, total = 0;

    /* Make sure private key struct is right, 
       where outer is seq and inner is pub:

       SEQUENCE (seq) {
            INTEGER ... (version)
            INTEGER ... (n)
            INTEGER ... (e)
            INTEGER ... (d)
            INTEGER ... (p)
            INTEGER ... (q)
            INTEGER ... (d mod p-1)            
            INTEGER ... (d mod q-1)            
            INTEGER ... (inverse of q mod p)
        }
    */

    for(t=0;t<seq->num;t++)
    {
        if(seq->lst[t].type==ASN1_INT)
            total++;
    }

    if(total > 8) 
    {
        ret=(rsa_t *)calloc(sizeof(rsa_t),1);
        if(ret)
        {
            /* We ignore the version */

            mp_init(&(ret->modulus));
            mp_init(&(ret->exponent));
            ret->private_exponent = mp_create();
            ret->p = mp_create();
            ret->q = mp_create();
            ret->iqmp = mp_create();            

            asn1_to_bignum(&(ret->modulus),&(seq->lst[1]));
            asn1_to_bignum(&(ret->exponent),&(seq->lst[2]));
            asn1_to_bignum(ret->private_exponent,&(seq->lst[3]));
            asn1_to_bignum(ret->p,&(seq->lst[4]));
            asn1_to_bignum(ret->q,&(seq->lst[5]));
            asn1_to_bignum(ret->iqmp,&(seq->lst[8]));

            ret->bits = mp_count_bits(&(ret->modulus));
            ret->bytes= (ret->bits + 7) / 8;

            if(!rsa_verify_keys(ret))
            {
                rsa_destroy(ret);
                ret = NULL;
            }
        }
    }
    return ret;
}

/* -------------------------- */

rsa_t *rsa_public_key_from_asn1(asn1_seq_t *seq,asn1_seq_t *pub)
{
    asn1_seq_t param;
    asn1_t asn;
    rsa_t *key = NULL;

    /* Make sure public key struct is right, 
       where outer is seq and inner is pub:

       SEQUENCE (seq) {
            SEQUENCE (pub) {
                OBJECT IDENTIFIER 1.2.840.10040.4.1
                NULL
            }
            BITS ...
        }
    */

    if(seq->lst[0].type==ASN1_SEQ && seq->lst[1].type==ASN1_BITS && pub->lst[0].type ==ASN1_OID)
    {
        if(pub->lst[0].len > 8 && !memcmp(pub->lst[0].data,rsa_oid + 4,9))
        {
            if(asn1_read(&asn,seq->lst[1].data + 1,seq->lst[1].len - 1) > 0 && asn.type == ASN1_SEQ)
            {
                if(!asn1_seq_read(&param,asn.data,asn.len) && param.num > 1)
                {
                    key = (rsa_t *)calloc(sizeof(rsa_t),1);
                    if(key)
                    {
                        if(mp_init_set_bytes(&(key->modulus),param.lst[0].len,param.lst[0].data) == MP_OKAY)
                        {
                            if(mp_init_set_bytes(&(key->exponent),param.lst[1].len,param.lst[1].data) == MP_OKAY)
                            {
                                key->bits = mp_count_bits(&(key->modulus));
                                key->bytes= (key->bits + 7) / 8;

                                asn1_seq_free(&param);                        
                                asn1_free(&asn);    
                                return key;
                            }
                        }
                        rsa_destroy(key);
                        key = NULL;
                    }
                    asn1_seq_free(&param);                        
                }
                asn1_free(&asn);             
            }
        }
    }
    return key;
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
        return FALSE;
    
    /* Now make sure that modulus is = p * q */

    if(mp_init(&tmp) != MP_OKAY && unpredictable_seed_non_linear())
        return FALSE;
    

    ret = mp_mul(key->p, key->q,&tmp);
    if(ret != MP_OKAY || mp_cmp(&tmp, &(key->modulus)))
    {
        /* m is not p * q */
        mp_clear(&tmp);
        return FALSE;
    }

    /* E * D must be congruent with 1, mod(p-1) and mod(q-1). */

    ret = mp_copy(key->p,&tmp);
    if(ret != MP_OKAY || mp_init(&ed) != MP_OKAY)
    {
        /* mp_copy() or mp_init() failed. Bad mem? */
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
                    /* mp_multmod(iqmp,p) failed  */
                    ret = MP_BADARG;
                }
            }
            else
            {
                /* mp_multmod(e,p) failed  */
                ret = MP_UNDEF;
            }
        }
    }
    else
    {
        /* e * d not congruent with 1  */
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
            break;
        default:
            return -1;
    }

    ptr+=padlen;

    /* Mark the end of the padding with a 0 */

   *ptr++=0;

    /* Make the data a big number */

    ret = mp_init_set_bytes(&res,key->bytes,dest);

    if(ret == MP_OKAY)
    {
        /* Execute RSA operation, using blinding if we are encoding with the private key */

        if(public && unpredictable_seed_non_linear() > 0)
            ret = mp_exptmod(&res,&(key->exponent),&(key->modulus),&res);
        else
            ret=rsa_blind(&res,key,&res);

        if(ret == MP_OKAY)
            ret = mp_copy_bytes(&res,dest,key->bytes);

        mp_clear(&res);
    }

    return (ret == MP_OKAY) ? key->bytes : -1;
}



/* -------------------------- */

int rsa_decode(rsa_t *key,void *dest,unsigned int tam,int public,int pad)
{
    mp_int_t res;
    int ret;

    if (key->bytes != tam)
        return -3;

    ret = mp_init_set_bytes(&res,key->bytes,dest);

    if(ret == MP_OKAY)
    {
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
                    decded buffer should be a byte less than
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
                                ret = -4;
                            len--;
                            break;
                        case RSA_PAD_RANDOM:
                            while(len > 1 && *ptr)
                            {
                                ptr++;
                                len--;
                            }
                            if(*ptr++)
                                ret = -4;
                            else
                                len--;
                            break;
                        default:
                            ret = -4;
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
    }

    return -1;
}

/* -------------------------- *
     RSA DIGITAL SIGNATURES
 * -------------------------- */

static struct _firma_
{
    int alg,hash;
    char *oid;
    char *name;
    wchar_t *namew;
    unsigned char asn[11];
}
alg_firma[] =
{
    /* Algoritmos de Hash usados en las firmas */

    {MD2_RSA_DIGEST,HASH_MD2,"1.2.840.113549.2.2", "md2Digest",L"md2Digest",{0x06,0x08,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x02,0x02,0x00}},
    {MD4_RSA_DIGEST,HASH_MD4,"1.2.840.113549.2.4", "md4Digest",L"md4Digest",{0x06,0x08,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x02,0x04,0x00}},
    {MD5_RSA_DIGEST,HASH_MD5,"1.2.840.113549.2.5", "md5Digest",L"md5Digest",{0x06,0x08,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x02,0x05,0x00}},
    {SHA1_RSA_DIGEST,HASH_SHA1,"1.3.14.3.2.26", "sha1Digest",L"sha1Digest",{0x06,0x05,0x2B,0x0E,0x03,0x02,0x1A,0x00,0x00,0x00,0x00}},

    {SHA224_RSA_DIGEST,HASH_SHA224,"2.16.840.1.101.3.4.2.4","sha224Digest",L"sha224Digest",{0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x04}},
    {SHA256_RSA_DIGEST,HASH_SHA256,"2.16.840.1.101.3.4.2.1","sha256Digest",L"sha256Digest",{0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01}},
    {SHA384_RSA_DIGEST,HASH_SHA384,"2.16.840.1.101.3.4.2.2","sha384Digest",L"sha384Digest",{0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02}},
    {SHA512_RSA_DIGEST,HASH_SHA512,"2.16.840.1.101.3.4.2.3","sha512Digest",L"sha512Digest",{0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03}},

    {MD2_RSA_ENC,HASH_MD2,"1.2.840.113549.1.1.2", "md2WithRSAEncryption",L"md2WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x02}},
    {MD4_RSA_ENC,HASH_MD4,"1.2.840.113549.1.1.3", "md4WithRSAEncryption",L"md4WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x03}},
    {MD5_RSA_ENC,HASH_MD5,"1.2.840.113549.1.1.4", "md5WithRSAEncryption",L"md5WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x04}},
    {SHA1_RSA_ENC,HASH_SHA1,"1.2.840.113549.1.1.5", "sha1WithRSAEncryption",L"sha1WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x05}},

    {SHA224_RSA_ENC,HASH_SHA224,"1.2.840.113549.1.1.14", "sha224WithRSAEncryption",L"sha224WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x0e}},
    {SHA256_RSA_ENC,HASH_SHA256,"1.2.840.113549.1.1.11", "sha256WithRSAEncryption",L"sha256WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x0b}},
    {SHA384_RSA_ENC,HASH_SHA384,"1.2.840.113549.1.1.12", "sha384WithRSAEncryption",L"sha384WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x0c}},
    {SHA512_RSA_ENC,HASH_SHA512,"1.2.840.113549.1.1.13", "sha512WithRSAEncryption",L"sha512WithRSAEncryption",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x0d}},

    /* Otros identificadores de los mismos algoritmos */

    {MD4_RSA,HASH_MD4,"1.3.14.3.2.2","md4WithRSA",L"md4WithRSA",{0x06,0x05,0x2B,0x0E,0x03,0x02,0x02,0x00,0x00,0x00,0x00}},
    {MD5_RSA,HASH_MD5,"1.3.14.3.2.3","md5WithRSA",L"md5WithRSA",{0x06,0x05,0x2B,0x0E,0x03,0x02,0x03,0x00,0x00,0x00,0x00}},
    
    {MD2_RSA_SIGN,HASH_MD2,"1.3.14.3.2.24",  "md2WithRSASignature",L"md2WithRSASignature",{0x06,0x05,0x2B,0x0E,0x03,0x02,0x18,0x00,0x00,0x00,0x00}},
    {MD5_RSA_SIGN,HASH_MD5,"1.3.14.3.2.25",  "md5WithRSASignature",L"md5WithRSASignature",{0x06,0x05,0x2B,0x0E,0x03,0x02,0x19,0x00,0x00,0x00,0x00}},
    {SHA1_RSA_SIGN,HASH_SHA1,"1.3.14.3.2.29", "sha1WithRSASignature",L"sha1WithRSASignature",{0x06,0x05,0x2B,0x0E,0x03,0x02,0x1D,0x00,0x00,0x00,0x00}},

    /* Este siempre último */

    {NPI_SIGN_ALG,HASH_NONE,NULL,NULL,NULL,{0}},
};


/* --------------------------------------------- */

int rsa_sign_alg(int alg)
{
    int t;

    for(t=0;alg_firma[t].oid;t++)
    {
        if(alg_firma[t].alg == alg)
            return alg;
    }

    return NPI_SIGN_ALG;
}

/* --------------------------------------------- */

int rsa_sign_algorithm(const char *oid)
{
    int t;

    if(oid)
    {
        for(t=0;alg_firma[t].oid;t++)
        {
            if(!strcmp(oid,alg_firma[t].oid))
                return alg_firma[t].alg;
        }
    }
    return NPI_SIGN_ALG;
}

/* -------------------------- */

int rsa_sign_alg_from_namew(const wchar_t *name)
{
    int t;

    if(name)
    {
        for(t=0;alg_firma[t].name;t++)
        {
            if(!wcscmp(name,alg_firma[t].namew))
                return alg_firma[t].alg;
        }
    }
    return NPI_SIGN_ALG;
}

/* -------------------------- */

int rsa_sign_alg_from_name(const char *name)
{
    int t;

    if(name)
    {
        for(t=0;alg_firma[t].name;t++)
        {
            if(!strcmp(name,alg_firma[t].name))
                return alg_firma[t].alg;
        }
    }
    return NPI_SIGN_ALG;
}


/* -------------------------- */

char *rsa_sign_oid(int which)
{
    int t;

    for(t=0;alg_firma[t].oid;t++)
    {
        if(which == alg_firma[t].alg)
            return alg_firma[t].oid;
    }
    return NULL;
}

/* -------------------------- */

char *rsa_sign_name(int which)
{
    int t;

    for(t=0;alg_firma[t].oid;t++)
    {
        if(which == alg_firma[t].alg)
            return alg_firma[t].name;
    }
    return NULL;
}

/* -------------------------- */

wchar_t *rsa_sign_namew(int which)
{
    int t;

    for(t=0;alg_firma[t].oid;t++)
    {
        if(which == alg_firma[t].alg)
            return alg_firma[t].namew;
    }
    return NULL;
}

/* -------------------------- */

int rsa_sign_oid_asn1(int which,void *dest,unsigned int max)
{
    unsigned int t,tmp;

    for(t=0;alg_firma[t].oid;t++)
    {
        if(which == alg_firma[t].alg)
        {
            tmp = alg_firma[t].asn[1] + 2;
            if(max < tmp)
                return 0;
            if(dest)
                memcpy(dest,alg_firma[t].asn,tmp);
            return tmp;
        }
    }
    return 0;
}

/* -------------------------- */

int rsa_sign_algorithm_asn1(const void *oid,unsigned int tam)
{
    unsigned int t,tmp;

    for(t=0;alg_firma[t].oid;t++)
    {
        tmp = alg_firma[t].asn[1];
        if(tam == tmp && !memcmp(oid,alg_firma[t].asn + 2,tam))
            return alg_firma[t].alg;
    }
    return NPI_SIGN_ALG;
}

/* -------------------------- */

int rsa_signing_hash(int which)
{
    int t;

    for(t=0;alg_firma[t].oid;t++)
    {
        if(which == alg_firma[t].alg)
            return alg_firma[t].hash;
    }
    return HASH_NONE;
}

/* -------------------------- */

/*
    Una firma digital no es otra cosa que el hash de unos datos, precedido
    por una cabecera en ASN.1/DER que describe el algoritmo usado para
    calcular el hash y el algoritmo de key pública posteriormente usado
    para encriptar la firma con la key privada usando el sistema de padding
    de PKCS-1 versión 2.0.

    Ejemplos de secuencias de firma digital antes de ser encriptadas con
    la key privada:

    MD5_RSA_DIGEST:

        01  -- Bloque PKCS-1 tipo 1

        ff
        ...     -- Numero arbitrario de bits a 1 (depende de la longitud de bloque)
        ff

        00  -- Final del padding

        30 20 -- Secuencia de 32 bytes (0x20)

          30 0c -- Sub-secuencia de 12 bytes (0x0c)

             06 08 -- OID de 8 bytes de largo

                2a 86 48 86 f7 0d 02 05 --  (1.2.840.113549.2.5)

             05 00 -- NULL

          04 10 -- Una cadena de 16 bytes (0x10)

             16 bytes del hash MD5


    SHA1_RSA_DIGEST:

        01  -- Bloque PKCS-1 tipo 1

        ff
        ...     -- Numero arbitrario de bits a 1 (depende de la longitud de bloque)
        ff

        00  -- Final del padding

        30 21 -- Secuencia de 33 bytes

          30 09 -- Sub-Secuencia de 9 bytes

             06 05 -- OID de 5 bytes de largo

               2B 0E 03 02 1A -- (1.3.14.3.2.26)

             05 00 -- NULL

          04 14 -- Una cadena de 20 bytes (0x14)

             20 bytes del hash SHA1
*/

int rsa_sign(rsa_t *key,void *dest,unsigned int max,int alg,const void *datos,unsigned int tam)
{
    int atam,htam,total,que_hash,digest;
    unsigned char *ptr,hash[MAX_HASH_SIZE],oid[25];

    /* Nos aseguramos de que hay sitio para el bloque una vez encriptado */

    if(max < key->bytes)
        return -3;

    /* También de que la key privada esta disponible */

    if(!key->private_exponent)
        return -3;

    /* Obtenemos el algoritmo de hash */

    que_hash = rsa_signing_hash(alg);

    switch(que_hash)
    {
        case HASH_MD2:
            digest=MD2_RSA_DIGEST;
            break;
        case HASH_MD4:
            digest=MD4_RSA_DIGEST;
            break;
        case HASH_MD5:
            digest=MD5_RSA_DIGEST;
            break;
        case HASH_SHA1:
            digest=SHA1_RSA_DIGEST;
            break;
        case HASH_SHA224:
            digest=SHA224_RSA_DIGEST;
            break;
        case HASH_SHA256:
            digest=SHA256_RSA_DIGEST;
            break;
        case HASH_SHA384:
            digest=SHA384_RSA_DIGEST;
            break;
        case HASH_SHA512:
            digest=SHA512_RSA_DIGEST;
            break;
        default:
            return -4;
    }

    /* Calculamos el hash del bloque de datos y guardamos su tamaño */

    htam = calc_hash(que_hash,datos,tam,hash);

    /* Ahora Obtenemos el OID de algoritmo de hash */

    atam=rsa_sign_oid_asn1(digest,oid,25);

    /* Ahora calculamos el tamaño de todo */

    total = 4 + atam + 2 + htam + 2;

    /* Secuencias */

    ptr = (unsigned char *)dest;

   *ptr++=ASN1_SEQ;
   *ptr++=(unsigned char) total - 2;


   *ptr++=ASN1_SEQ;
   *ptr++=(unsigned char) atam + 2;

    /* OID */

    memcpy(ptr,oid,atam);
    ptr+=atam;

    /* NULL */

   *ptr++=ASN1_NULL;
   *ptr++=0;

    /* Hash */

   *ptr++=ASN1_BYTES;
   *ptr++=(unsigned char) htam;

    memcpy(ptr,hash,htam);

    return rsa_encode(key,dest,total,FALSE,RSA_PAD_ONES,NULL);
}

/* -------------------------- */

int rsa_read_sign(rsa_t *key,void *hash,unsigned int max,const void *orig,unsigned int tam)
{
    mp_int_t res;
    int ret=0;
    unsigned char *ptr;
    void *dest;
    asn1_seq_t seq;
    asn1_t asn;
    unsigned int count;

    /* Desencriptar los datos convirtiendolos a BN */

    if(mp_init_set_bytes(&res,tam,orig) != MP_OKAY)
        return -1;

    /* Aplicamos la ecuación RSA */

    ret = mp_exptmod(&res,&(key->exponent),&(key->modulus),&res);
    if(ret!= MP_OKAY)
    {
        mp_clear(&res);
        return ret;
    }

    /* Convert result to bytes */

    dest = mp_get_bytes(&res,&count);

    mp_clear(&res);

    if(dest==NULL)
        return -2;

    ptr=(unsigned char *)dest;

    /* Signature's padding type is all bits one */

    if(*ptr++ != RSA_PAD_ONES)
    {
        free(dest);
        return -4;
    }

    /* So make sure all padding bits are one, i.e. all are 0xff */

    ret=tam - 1;
    while(*ptr==0xff && ret > 0)
    {
        ptr++;
        ret--;
    }

    if(*ptr++)
    {
        free(dest);
        return -3;
    }

    ret--;

    /* Comprobamos la firma */

    if(asn1_read(&asn,ptr,ret) < 1 || asn.type != ASN1_SEQ)
    {
        free(dest);
        return -3;
    }

    /* Leemos la secuencia que contiene el algoritmo y la firma */

    if(asn1_seq_read(&seq,asn.data,asn.len) || seq.lst[0].type!=ASN1_SEQ || seq.lst[1].type!=ASN1_BYTES)
    {
        asn1_seq_free(&seq);
        free(dest);
        return -3;
    }

    /* Buscamos el algoritmo */

    if(asn1_read(&asn,seq.lst[0].data,seq.lst[0].len) < 1 || asn.type != ASN1_OID)
    {
        asn1_seq_free(&seq);
        free(dest);
        return -3;
    }

    ret = -3;

    switch(rsa_sign_algorithm_asn1(asn.data,asn.len))
    {
        case MD2_RSA_ENC:
        case MD2_RSA_DIGEST:
        case MD2_RSA_SIGN:
            if(seq.lst[1].len==MD2_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < MD2_SIZE) ? max : MD2_SIZE);
                ret=HASH_MD2;
            }
            break;
        case MD4_RSA:
        case MD4_RSA_ENC:
        case MD4_RSA_DIGEST:
            if(seq.lst[1].len==MD4_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < MD4_SIZE) ? max : MD4_SIZE);
                ret=HASH_MD4;
            }
            break;
        case MD5_RSA_ENC:
        case MD5_RSA_DIGEST:
        case MD5_RSA:
        case MD5_RSA_SIGN:
            if(seq.lst[1].len==MD5_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < MD5_SIZE) ? max : MD5_SIZE);
                ret=HASH_MD5;
            }
            break;
        case SHA1_RSA_DIGEST:
        case SHA1_RSA_SIGN:
        case SHA1_RSA_ENC:
            if(seq.lst[1].len==SHA1_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < SHA1_SIZE) ? max : SHA1_SIZE);
                ret=HASH_SHA1;
            }
            break;
        case SHA224_RSA_DIGEST:        
            if(seq.lst[1].len==SHA224_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < SHA224_SIZE) ? max : SHA224_SIZE);
                ret=HASH_SHA256;
            }
            break;
        case SHA256_RSA_ENC:
        case SHA256_RSA_DIGEST:        
            if(seq.lst[1].len==SHA256_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < SHA256_SIZE) ? max : SHA256_SIZE);
                ret=HASH_SHA256;
            }
            break;
        case SHA384_RSA_ENC:
        case SHA384_RSA_DIGEST:        
            if(seq.lst[1].len==SHA384_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < SHA384_SIZE) ? max : SHA384_SIZE);
                ret=HASH_SHA384;
            }
            break;
        case SHA512_RSA_ENC:
        case SHA512_RSA_DIGEST:        
            if(seq.lst[1].len==SHA512_SIZE)
            {
                memcpy(hash,seq.lst[1].data,(max < SHA512_SIZE) ? max : SHA512_SIZE);
                ret=HASH_SHA512;
            }
            break;
        default:    /* Esto no deberia pasar */
            ret = -777;
            break;
    }
    asn1_seq_free(&seq);
    free(dest);
    return ret;
}

/* -------------------------- */

int rsa_calc_file_digest(rsa_t *key,void *dest,unsigned int max,int alg,const wchar_t *fich)
{
    int htam;

    /* Nos aseguramos de que hay sitio para el bloque una vez encriptado */

    if(max < key->bytes)
        return -3;

    /* También de que la key privada esta disponible */

    if(!key->private_exponent)
        return -3;

    /* Calculamos el hash del file */

    htam = calc_hash_filew(alg,fich,dest,NULL);
    if(htam < 0)
        return htam;
    return rsa_encode(key,dest,htam,FALSE,1,NULL);
}

/* -------------------------- */

int rsa_check_file_digest(rsa_t *key,const void *orig,unsigned int tam,int alg,const wchar_t *fich)
{
    mp_int_t res;
    int ret=0,tamh;
    unsigned char *ptr,hash[MAX_HASH_SIZE];
    void *dest;
    unsigned int count;

    /* Asegurarnos que el hash es valido */

    tamh=hash_size(alg);
    if(!tamh)
        return -5;

    /* Desencriptar los datos convirtiendolos a BN */

    if(mp_init_set_bytes(&res,tam,orig) != MP_OKAY)
        return -1;

    /* Aplicamos la ecuación RSA */

    if( mp_exptmod(&res,&(key->exponent),&(key->modulus),&res) == MP_OKAY)
    {
        /* Convertimos el resultado a bytes */

        dest = mp_get_bytes(&res,&count);

        mp_clear(&res);

        if(dest==NULL)
            return -2;

        ptr=(unsigned char *)dest;

        /* El tipo de bloque en la firma es 1 */

        if(*ptr++!=0x01)
        {
            free(dest);
            return -3;
        }

        /* Ahora nos aseguramos de que el padding está compuesto de 0xff */

        ret=tam - 1;
        while(*ptr==0xff && ret > 0)
        {
            ptr++;
            ret--;
        }
        if(*ptr++)
        {
            free(dest);
            return -3;
        }

        ret--;

        /* Comprobamos la firma */

        if(ret != tamh)
        {
            free(dest);
            return -3;
        }

        ret = calc_hash_filew(alg,fich,hash,NULL);
        if(ret==tamh)
        {
            if(memcmp(hash,ptr,tamh))
                ret = -4;
            else
                ret = 0;
        }
        else if( ret > 0)
        {
            ret = -4;
        }
        free(dest);
        return ret;
    }
    else
    {
        mp_clear(&res);
    }
    return -4;
}



