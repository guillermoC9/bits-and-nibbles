/*
    ecc.c

    Elliptic Curve Cryptography Protocols

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

        - Written by Guillermo Amodeo Ojeda using references from:

            * RFC-5480 (http://www.ietf.org/rfc/rfc5480.txt)
            * RFC-5208 (https://tools.ietf.org/html/rfc5208)
            * RFC-5915 (https://tools.ietf.org/html/rfc5915)
            * RFC-3279 (https://tools.ietf.org/html/rfc3279)
            * RFC-8032 (https://www.rfc-editor.org/rfc/rfc8032)
            * RFC-6979 (https://www.rfc-editor.org/rfc/rfc6979)
            * RFC-5758 (https://www.rfc-editor.org/rfc/rfc5758)

            https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm

                                --oO0Oo--
*/

#include "ecc.h"

/* --------------------------------------------- */

#define MAX_OID_ALG 11

static struct _ecc_sign_
{
    int alg,hash;
    char *oid;
    char *name;
    wchar_t *namew;
    unsigned char asn[MAX_OID_ALG];
}
ecc_signa[] =
{
    /* Hash algorithms used on the signatures (order must be the same as declared in ecc_proto.h) */

    {ECDSA_SHA1  , HASH_SHA1,  "1.2.840.10045.4.1"  ,"ecdsaWithSHA1"   , L"ecdsaWithSHA1"   , {0x06,0x07,0x2a,0x86,0x48,0xce,0x3d,0x04,0x01,0x00,0x00}},
    {ECDSA_SHA224, HASH_SHA224,"1.2.840.10045.4.3.1","ecdsaWithSHA224" , L"ecdsaWithSHA224" , {0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x01,0x00}},
    {ECDSA_SHA256, HASH_SHA256,"1.2.840.10045.4.3.2","ecdsaWithSHA256" , L"ecdsaWithSHA256" , {0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x02,0x00}},
    {ECDSA_SHA384, HASH_SHA384,"1.2.840.10045.4.3.3","ecdsaWithSHA384" , L"ecdsaWithSHA384" , {0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x03,0x00}},
    {ECDSA_SHA512, HASH_SHA512,"1.2.840.10045.4.3.4","ecdsaWithSHA512" , L"ecdsaWithSHA512" , {0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x04,0x03,0x04,0x00}},

    {ECDSA_SHA3_224, HASH_SHA3_224,"2.16.840.1.101.3.4.3.9" , "ecdsaWithSHA3-224", L"ecdsaWithSHA3-224", {0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x03,0x09}},
    {ECDSA_SHA3_256, HASH_SHA3_256,"2.16.840.1.101.3.4.3.10", "ecdsaWithSHA3-256", L"ecdsaWithSHA3-256", {0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x03,0x0a}},
    {ECDSA_SHA3_384, HASH_SHA3_384,"2.16.840.1.101.3.4.3.11", "ecdsaWithSHA3-384", L"ecdsaWithSHA3-384", {0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x03,0x0b}},
    {ECDSA_SHA3_512, HASH_SHA3_512,"2.16.840.1.101.3.4.3.12", "ecdsaWithSHA3-512", L"ecdsaWithSHA3-512", {0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x03,0x0c}},

    /* RFC-8410: https://datatracker.ietf.org/doc/rfc8410 */
    /* RFC-8032: https://datatracker.ietf.org/doc/rfc8032 */

    {EdDSA_25519,  HASH_SHA512, "1.3.101.112", "eddsaWithEd25519" ,L"eddsaWithEd25519", {0x06,0x03,0x2b,0x65,0x70,0x00,0x00,0x00,0x00,0x00,0x00}},
    {EdDSA_448, HASH_SHAKE_256, "1.3.101.113", "eddsaWithEd448"   ,L"eddsaWithEd448"  , {0x06,0x03,0x2b,0x65,0x71,0x00,0x00,0x00,0x00,0x00,0x00}},

    /* Add new ones here */

    {NPI_SIGN_ALG,HASH_NONE,NULL,NULL,NULL,{0}},
};

/*  
   WARNING ***  WARNING ** WARNING ***  WARNING 

   Make sure this is correct and points to EdDSA_25519 entry
   on the array above if you add new algorithms there, as code 
   downwards depends on it
*/

#define KEY_X25519_POS  9       

/* --------------------------------------------- */

int ecc_sign_alg(int alg)
{
    int t;

    for(t=0;ecc_signa[t].oid;t++)
    {
        if(ecc_signa[t].alg == alg)
            return alg;
    }

    return NPI_SIGN_ALG;
}

/* --------------------------------------------- */

int ecc_sign_algorithm(const char *oid)
{
    int t;

    if(oid)
    {
        for(t=0;ecc_signa[t].oid;t++)
        {
            if(!strcmp(oid,ecc_signa[t].oid))
                return ecc_signa[t].alg;
        }
    }
    return NPI_SIGN_ALG;
}

/* -------------------------- */

int ecc_sign_alg_from_namew(const wchar_t *name)
{
    int t;

    if(name)
    {
        for(t=0;ecc_signa[t].name;t++)
        {
            if(!wcscmp(name,ecc_signa[t].namew))
                return ecc_signa[t].alg;
        }
    }
    return NPI_SIGN_ALG;
}

/* -------------------------- */

int ecc_sign_alg_from_name(const char *name)
{
    int t;

    if(name)
    {
        for(t=0;ecc_signa[t].name;t++)
        {
            if(!strcmp(name,ecc_signa[t].name))
                return ecc_signa[t].alg;
        }
    }
    return NPI_SIGN_ALG;
}


/* -------------------------- */

char *ecc_sign_oid(int which)
{
    int t;

    for(t=0;ecc_signa[t].oid;t++)
    {
        if(which == ecc_signa[t].alg)
            return ecc_signa[t].oid;
    }
    return NULL;
}

/* -------------------------- */

char *ecc_sign_name(int which)
{
    int t;

    for(t=0;ecc_signa[t].oid;t++)
    {
        if(which == ecc_signa[t].alg)
            return ecc_signa[t].name;
    }
    return NULL;
}

/* -------------------------- */

wchar_t *ecc_sign_namew(int which)
{
    int t;

    for(t=0;ecc_signa[t].oid;t++)
    {
        if(which == ecc_signa[t].alg)
            return ecc_signa[t].namew;
    }
    return NULL;
}

/* -------------------------- */

int ecc_sign_oid_asn1(int which,unsigned char *dest,int max)
{
    int t,tmp;

    for(t=0;ecc_signa[t].oid;t++)
    {
        if(which == ecc_signa[t].alg)
        {
            tmp = ecc_signa[t].asn[1] + 2;
            if(max < tmp)
                return 0;
            if(dest)
                memcpy(dest,ecc_signa[t].asn,tmp);
            return tmp;
        }
    }
    return 0;
}

/* -------------------------- */

int ecc_sign_algorithm_asn1(unsigned char *oid,int tam)
{
    int t,tmp;

    for(t=0;ecc_signa[t].oid;t++)
    {
        tmp = ecc_signa[t].asn[1];
        if(tam == tmp && !memcmp(oid,ecc_signa[t].asn + 2,tam))
            return ecc_signa[t].alg;
    }
    return NPI_SIGN_ALG;
}

/* -------------------------- */

int ecc_signing_hash(int which)
{
    int t;

    for(t=0;ecc_signa[t].oid;t++)
    {
        if(which == ecc_signa[t].alg)   
            return ecc_signa[t].hash;        
    }
    return HASH_NONE;
}

/* --------------------------------------------- */
/*      OID for EC public key types              */
/* --------------------------------------------- */

typedef struct
{
    int                 val;
    const char         *oid;
    const unsigned char asn1[MAX_OID_ALG];
} eccOID_t;


static eccOID_t ecc_pub[ECC_PUBLIC_KEY_COUNT] =
{
    {ECC_PUBLIC_KEY,        "1.2.840.10045.2.1",{0x06,0x07,0x2a,0x86,0x48,0xce,0x3d,0x02,0x01,0x00,0x00}},
    {ECC_PUBLIC_DH_KEY,     "1.3.132.1.12",     {0x06,0x05,0x2b,0x81,0x04,0x01,0x0c,0x00,0x00,0x00,0x00}},
    {ECC_PUBLIC_MQV_KEY,    "1.3.132.1.13",     {0x06,0x05,0x2b,0x81,0x04,0x01,0x0d,0x00,0x00,0x00,0x00}},
    {ECC_PUBLIC_25519_KEY,  "1.3.101.110",      {0x06,0x03,0x2b,0x65,0x6e,0x00,0x00,0x00,0x00,0x00,0x00}},
    {ECC_PUBLIC_448_KEY,    "1.3.101.111",      {0x06,0x03,0x2b,0x65,0x6f,0x00,0x00,0x00,0x00,0x00,0x00}}
};
/*  
   WARNING ***  WARNING ** WARNING ***  WARNING 

   Make sure this is correct and points to ECC_PUBLIC_448_KEY 
   entry on the array above if you add new algorithms there, 
   as code downwards depend on it
*/

#define KEY_X448_POS   4

/* ------------------------ */

int ecc_public_key_type_from_asn1(const void *buf,int blen)
{
    int i;

    if(buf)
    {
        unsigned char len = (unsigned char)(blen & 0xff);

        for(i=0; i < ECC_PUBLIC_KEY_COUNT; i++)
        {
            if(len == ecc_pub[i].asn1[1] && !memcmp(buf,ecc_pub[i].asn1 + 2,blen))
                return ecc_pub[i].val;
        }
    }
    return ECC_PUBLIC_KEY_COUNT;
}

/* ------------------------ */

int ecc_public_key_type_to_asn1(int key, void *buf,int max)
{
    int i;

    if(buf)
    {
        for(i=0; i < ECC_PUBLIC_KEY_COUNT; i++)
        {
            if(key == ecc_pub[i].val)
            {
                int len = ecc_pub[i].asn1[1] + 2;

                if(len > max)
                    max = len;

                memcpy(buf,ecc_pub[i].asn1, len);
                return len;
            }
        }
    }
    return 0;
}

/* ------------------------ */
/* ------------------------ */

void ecc_free_keys(ecc_key_t *kc)
{
    if(kc)
    {
        mp_free(kc->pri);
        ecc_point_clear(&(kc->pub));
        memset(kc,0,sizeof(ecc_key_t));
        free(kc);
    }
}

/* ------------------------ */

int ecc_public_key_equal(ecc_key_t *a,ecc_key_t *b)
{
    if(a && b && ecc_key_size(a) == ecc_key_size(b))
    {
        if(ecc_point_is_equal(&(a->pub),&(b->pub)))
            return TRUE;
    }
    return FALSE;
}

/* ------------------------ */

int ecc_private_key_equal(ecc_key_t *a,ecc_key_t *b)
{
    if(a && b && ecc_key_size(a)== ecc_key_size(b))
    {
        if(mp_cmp(a->pri,b->pri) == MP_EQ)
            return TRUE;
    }
    return FALSE;
}

/* ------------------------ */

int ecc_private_key_copy(ecc_key_t *dst,ecc_key_t *src)
{
    if(dst && src && src->pri)
    {
        if(!dst->pri)
            dst->pri = mp_create();

        return (mp_copy(src->pri,dst->pri) == MP_OKAY) ? 0 : -1;
    }
    return -1;
}

/* ------------------------ */

static ecc_key_t *ecc_gen_keys(ecc_curve_t *ctx,rand_t *rc)
{
    ecc_key_t *kc;

    kc=(ecc_key_t *)calloc(sizeof(ecc_key_t),1);
    if(kc)
    {
        kc->ctx = ctx;
        kc->pri = NULL;
        ecc_point_init(&(kc->pub));

        /* Generate random keys from parameters if needed */

        if(rc)
        {
            kc->pri = mp_create();
            ecc_random_field(ctx,rc,kc->pri);
            ecc_point_mult(ctx,&(kc->pub),&(ctx->G),kc->pri);
        }
    }
    return kc;
}

/* ------------------------ */

ecc_key_t *ecc_generate_keys(int curve,rand_t *rc)
{
    ecc_key_t *kc=NULL;
    ecc_curve_t *ctx;
    rand_t   *orc=NULL;

    if(rc==NULL)
    {
        orc=rand_start(RAND_OS,RAND_TLS_SHA256);
        if(orc==NULL)
            return NULL;
        rc = orc;
    }
    ctx=ecc_get_curve(curve);
    if(ctx)
        kc = ecc_gen_keys(ctx,rc);

    rand_end(orc);
    return kc;
}

/* ------------------------ */

ecc_key_t *ecc_keys_from_bytes(int curve,const void *pri,size_t pvcnt,const void *pub,size_t pucnt)
{
    ecc_key_t *kc=NULL;
    ecc_curve_t *ctx;

    if(pri || pub)
    {
        ctx=ecc_get_curve(curve);
        if(ctx)
        {
            kc=ecc_gen_keys(ctx,NULL);
            if(kc)
            {
                if(pri)
                {
                    kc->pri = mp_create();
                    mp_set_bytes(kc->pri,pri,pvcnt);
                }

                if(pub)
                {
                    int ret = ecc_point_from_bytes(ctx,&(kc->pub),pub,pucnt);
                    if(ret)
                    {                       
                        ecc_free_keys(kc);
                        return NULL;
                    }
                }
                else if(kc->pri)
                {
                    ecc_point_mult(ctx,&(kc->pub),&(ctx->G),kc->pri);
                }
            }
        }
    }
    return kc;
}

/* ------------------------ */

ecc_key_t *ecc_keys_from_chars(int curve,const char *pri,const char *px,const char *py)
{
    ecc_key_t *kc=NULL;
    ecc_curve_t *ctx;

    if(pri)
    {
        ctx=ecc_get_curve(curve);
        if(ctx)
        {
            kc=ecc_gen_keys(ctx,NULL);
            if(kc)
            {
                if(pri)
                {
                    kc->pri = mp_create();
                    mp_set_string(kc->pri,pri,16);
                }

                if(px && py)
                {
                    mp_set_string(&(kc->pub.x),px,16);
                    mp_set_string(&(kc->pub.x),py,16);
                }
                else if (kc->pri)
                {
                    ecc_point_mult(ctx,&(kc->pub),&(ctx->G),kc->pri);
                }                
            }
        }
    }
    return kc;
}

/* ------------------------ */

int ecc_keys_from_seq_asn1(ecc_key_t **k,const void *buf,int blen)
{
    asn1_seq_t seq,pub;
    ecc_key_t *ecc = NULL;
    ecc_curve_t *ctx;    
    
   *k=NULL;

    if(!asn1_seq_read(&seq,buf,blen))
    {                  
        if (seq.num == 2 && !asn1_seq_read(&pub,seq.lst[0].data,seq.lst[0].len))   /* PEM_PUBLIC_KEY */
        {
           *k = ecc_public_key_from_asn1(&seq,&pub);

            asn1_seq_free(&seq);
            asn1_seq_free(&pub);

            return (*k) ? 0 : -2;
        }
        else if(seq.num > 2)
        {
            /* PEM_PRIVATE_KEY */

            if (seq.lst[0].type==ASN1_INT && seq.lst[1].type==ASN1_BYTES)
            {

                if(asn1_explicit(&(seq.lst[2])) != 0 || asn1_seq_read(&pub,seq.lst[2].data,seq.lst[2].len))
                {                    
                    asn1_seq_free(&seq);
                    return -1;
                }
            }
            else if(seq.lst[0].type==ASN1_INT && seq.lst[1].type==ASN1_SEQ && seq.lst[2].type==ASN1_BYTES)
            {
                if(asn1_seq_read(&pub,seq.lst[1].data,seq.lst[1].len))
                {                    
                    asn1_seq_free(&seq);
                    return -1;
                }
            }
            else
            {                
                asn1_seq_free(&seq);
                return -1;
            }

            if(pub.lst[0].type !=ASN1_OID)
            {                
                asn1_seq_free(&pub);
                asn1_seq_free(&seq);
                return -1;
            }

            ctx = ecc_get_curve_from_asn1(pub.lst[0].data,pub.lst[0].len);
            if(ctx == NULL)
                ctx = ecc_get_curve(ecc_sign_algorithm_asn1(pub.lst[0].data,pub.lst[0].len));            

            if(ctx == NULL)
            {
                wchar_t tmp[81];

                asn1_oidw(tmp,81,pub.lst[0].data,pub.lst[0].len);                

                asn1_seq_free(&pub);
                asn1_seq_free(&seq);

                return -1;
            }

            asn1_seq_free(&pub);

            ecc = ecc_gen_keys(ctx,NULL);
            if(ecc==NULL)
            {
                asn1_seq_free(&seq);
                return -2;
            }

            ecc->pri = mp_create();
            mp_set_bytes(ecc->pri,seq.lst[1].data,seq.lst[1].len);

            /* Generate our own public key */

            ecc_point_mult(ctx,&(ecc->pub),&(ctx->G),ecc->pri);

            /* If public key is present, check if same as calculated */

            if(seq.num > 3 && asn1_explicit(&(seq.lst[3]))==1)
            {
                ecc_point_t p;

                ecc_point_init(&p);

                if(asn1_seq_read(&pub,seq.lst[3].data,seq.lst[3].len) ||
                   ecc_point_from_bytes(ctx,&p,pub.lst[0].data + 1,pub.lst[0].len - 1) ||
                   !ecc_point_is_equal(&p,&(ecc->pub)))
                {                    
                    ecc_point_clear(&p);
                    ecc_free_keys(ecc);
                    asn1_seq_free(&seq);
                    return -3;
                }

                ecc_point_clear(&p);
            }
            /* verify point is in the curve */

            if(ecc_point_on_curve(ctx,&(ecc->pub)))
            {
                asn1_seq_free(&seq);
               *k = ecc;
                return 0;
            }

            ecc_free_keys(ecc);
        }

        asn1_seq_free(&seq);

        return -1;

    }
    return -1;
}

/* ------------------------ */

int ecc_keys_from_asn1(ecc_key_t **k,const void *bytes,int tam)
{
    asn1_t asn;

    /* Make sure it starts with a sequence */

    if(asn1_read(&asn,bytes,tam) < 1 || asn.type != ASN1_SEQ)
        return -1;

    return ecc_keys_from_seq_asn1(k,asn.data,asn.len);
}



/* -------------------------- */

ecc_key_t *ecc_public_key_from_asn1(asn1_seq_t *seq,asn1_seq_t *pub)
{
    ecc_curve_t *ctx = NULL;    
    ecc_key_t *key = NULL;    

    /* Make sure public key struct is right, 
       where outer is seq and inner is pub:

       SEQUENCE (seq) {
            SEQUENCE (pub) {
                OBJECT IDENTIFIER ...
                OPTIONAL OBJECT IDENTIFIER ...
            }
            BITS ...
        }
    */

    if(seq->lst[0].type==ASN1_SEQ && seq->lst[1].type==ASN1_BITS && pub->lst[0].type ==ASN1_OID)
    {
        switch(ecc_public_key_type_from_asn1(pub->lst[0].data,pub->lst[0].len))            
        {
            case ECC_PUBLIC_KEY:
                if(pub->num > 1 && pub->lst[1].type ==ASN1_OID)
                    ctx = ecc_get_curve_from_asn1(pub->lst[1].data,pub->lst[1].len);                
                break;
            case ECC_PUBLIC_25519_KEY:
                ctx = ecc_get_curve(ECC_CURVE_X25519);                
                break;
            case ECC_PUBLIC_448_KEY:
                ctx = ecc_get_curve(ECC_CURVE_X448);                
                break;
            default:                            
                break;
        }

        if(ctx)
        {               
            key=(ecc_key_t *)calloc(sizeof(ecc_key_t),1);
            if(key)
            {
                key->ctx = ctx;
                key->pri = NULL;
                ecc_point_init(&(key->pub));            
                if(ecc_point_from_bytes(ctx,&(key->pub),seq->lst[1].data + 1,seq->lst[1].len - 1))
                {
                    ecc_free_keys(key);
                    key= NULL;
                }
            }
        }
    }
    return key;
}


/* ------------------------ */

static int do_ecc_load_pem(pem_file_t *pem,int publica,const char *passcode,ecc_key_t **ecc)
{
    int ret = -1;
    pem_elem_t *elem=NULL;

    if(pem)
    {
        if(publica)
        {
            elem=pem_first_element(pem,PEM_PUBLIC_KEY);
        }
        else
        {
            elem=pem_first_element(pem,PEM_EC_PRIVATE_KEY);
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
        if(ret==0 && (elem->flags & PEM_F_ASN1))
        {
            ret=ecc_keys_from_asn1(ecc,elem->data,elem->size);
        }
    }
    return ret;
}

/* ------------------------ */

int ecc_load_pem(const char *file,int publica,const char *passcode,ecc_key_t **ecc)
{
    pem_file_t *pem;
    int ret = -1;

    if(ecc && file)
    {
        ret=pem_open(&pem,file);
        if(ret==0)
        {
            ret = do_ecc_load_pem(pem,publica,passcode,ecc);
            pem_close(pem);
        }
    }
    return ret;
}

/* ------------------------ */

int ecc_load_pemw(const wchar_t *file,int publica,const char *passcode,ecc_key_t **ecc)
{
    pem_file_t *pem;
    int ret = -1;

    if(ecc && file)
    {
        ret=pem_openw(&pem,file);
        if(ret==0)
        {
            ret = do_ecc_load_pem(pem,publica,passcode,ecc);
            pem_close(pem);
        }
    }
    return ret;
}

/* ------------------------------- */

int ecc_public_to_pem(ecc_key_t *key,FILE *fp)
{
    asn1_t *asn;
    int ret = -1;

    if(key && fp)
    {
        asn = ecc_public_to_asn1(key);
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

int ecc_private_to_pem(ecc_key_t *key,const char *passcode,int alg,FILE *fp)
{
    asn1_t *asn;
    int ret = -1;

    if(key && fp)
    {
        asn = ecc_private_to_asn1(key);
        if(asn)
        {
            ret=pem_save_element(fp,PEM_EC_PRIVATE_KEY,asn->start,asn->total,passcode,alg);
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

int ecc_save_pem(const char *file,ecc_key_t *key,int priv,const char *passcode,int alg)
{
    int ret = -1;
    FILE *fp;

    if(key && file)
    {        
        fp=fopen(file,"wb");
        if(fp)
        {
            if(priv)
                ret = ecc_private_to_pem(key,passcode,alg,fp);
            else
                ret = ecc_public_to_pem(key,fp);
            fflush(fp);
            fclose(fp);
        }
    }
    return ret;
}


/* ------------------------------- */

int ecc_save_pemw(const wchar_t *file,ecc_key_t *key,int priv,const char *passcode,int alg)
{
    int ret = -1;
    FILE *fp;

    if(key && file)
    {
        fp=fopenw(file,"wb");
        if(fp)
        {
            if(priv)
                ret = ecc_private_to_pem(key,passcode,alg,fp);
            else
                ret = ecc_public_to_pem(key,fp);
            fflush(fp);
            fclose(fp);
        }
    }
    return ret;
}


/* -------------------------- */

static asn1_t *ecc_private_edwards_to_asn1(ecc_key_t *key)
{
    asn1_t *asn=NULL,*pri=NULL,*seq = NULL;
    int len;
    unsigned char oid_dat[MAX_OID_ALG];
    int  oid_len=0;

    if(key->ctx->curve==ECC_CURVE_X25519)
    {
        memcpy(oid_dat,ecc_signa[KEY_X25519_POS].asn,5);
        oid_len=5;
    }    
    else if(key->ctx->curve==ECC_CURVE_X448)
    {
        memcpy(oid_dat,ecc_pub[KEY_X448_POS].asn1,5);
        oid_len=5;
    }

    if(oid_len==0)
        return NULL;

    pri = asn1_create(ASN1_BYTES,key->ctx->NUMBYTES + 2);
    if(pri)
    {            
        /* We know that Edwards curves are never bigger than 254 
           bytes long, so we do  the encapsulation manually */

        pri->data[0]=ASN1_BYTES;
        pri->data[1]=key->ctx->NUMBYTES;        
        mp_copy_exact_bytes(key->pri,pri->data + 2,key->ctx->NUMBYTES);

        seq = asn1_create(ASN1_SEQ,oid_len);
        if(seq) 
        {       
            memcpy(seq->data,oid_dat,oid_len);                
                
            len = 3 + pri->total + seq->total;
                
            asn = asn1_create(ASN1_SEQ,len);
            if(asn)
            {
                len = 0;

                /* Start with the version */

                asn->data[len++] = ASN1_INT;
                asn->data[len++] = 1;
                asn->data[len++] = 0;

                memcpy(asn->data + len,seq->start,seq->total);
                len+=seq->total;                

                memcpy(asn->data + len,pri->start,pri->total);                
            }
            asn1_free(seq);
        }
        asn1_free(pri);
    }
    return asn;
}


/* -------------------------- */

asn1_t *ecc_private_to_asn1(ecc_key_t *key)
{
    asn1_t *asn=NULL,*bits=NULL,*pri=NULL,*oid=NULL,*pub=NULL;
    int ret = 0;

    if(key && key->pri)
    {
        unsigned char oid_dat[MAX_OID_ALG],tmp[ECC_MAX_HEX_BUFFER];
        int  oid_len;

        if(key->ctx->curve==ECC_CURVE_X25519 || key->ctx->curve==ECC_CURVE_X448)
            return ecc_private_edwards_to_asn1(key);


        oid_len=ecc_get_curve_asn1(key->ctx->curve,oid_dat,MAX_OID_ALG);
        if(oid_len==0)
            return NULL;

        ret = ecc_point_to_bytes(key->ctx,&(key->pub),tmp,ECC_MAX_HEX_BUFFER,FALSE);
        if(ret < 1)
            return NULL;

        bits=asn1_create_bits(tmp,ret * 8);
        if(bits)
        {
            pub = asn1_create_explicit(1,bits->start,bits->total);
            asn1_free(bits);
        }

        oid = asn1_create_explicit(0,oid_dat,oid_len);
        if(oid)
        {
            pri = asn1_create(ASN1_BYTES,key->ctx->NUMBYTES);
            if(pri)
                mp_copy_exact_bytes(key->pri,pri->data,pri->len);
        }

        if(pri && oid)
        {
            int len = 3 + oid->total + pri->total;

            if(pub)
                len += pub->total;

            asn = asn1_create(ASN1_SEQ,len);
            if(asn)
            {
                int tam = 0;

                /* Start with the version */

                asn->data[tam++] = ASN1_INT;
                asn->data[tam++] = 1;
                asn->data[tam++] = 1;

                /* Now Private Key and EC parameters */

                memcpy(asn->data + tam,pri->start,pri->total);
                tam+=pri->total;

                memcpy(asn->data + tam,oid->start,oid->total);
                tam+=oid->total;

                if(pub)
                {
                    memcpy(asn->data + tam,pub->start,pub->total);
                    //tam+=pub->total;
                }

            }
        }

        asn1_free(pri);
        asn1_free(pub);
        asn1_free(oid);
    }
    return asn;
}

/* -------------------------- */

asn1_t *ecc_public_to_asn1(ecc_key_t *key)
{
    asn1_t *asn=NULL,*bits;
    int ret;

    if(key)
    {
        unsigned char oid1[MAX_OID_ALG],oid2[MAX_OID_ALG],tmp[ECC_MAX_HEX_BUFFER];
        int  len1,len2 = 0;

        if(is_curve25519(key->ctx))
        {
            len1 = ecc_public_key_type_to_asn1(ECC_PUBLIC_25519_KEY,oid1,MAX_OID_ALG);
            if(len1 == 0)
                return NULL;
        }
        else if(is_curve448(key->ctx))
        {
            len1 = ecc_public_key_type_to_asn1(ECC_PUBLIC_448_KEY,oid1,MAX_OID_ALG);
            if(len1 == 0)
                return NULL;
        }
        else
        {

            len1 = ecc_public_key_type_to_asn1(ECC_PUBLIC_KEY,oid1,MAX_OID_ALG);
            if(len1 == 0)
                return NULL;
    
            len2=ecc_get_curve_asn1(key->ctx->curve,oid2,MAX_OID_ALG);
            if(len2==0)
                return NULL;
        }

        ret = ecc_point_to_bytes(key->ctx,&(key->pub),tmp,ECC_MAX_HEX_BUFFER,FALSE);
        if(ret < 1)
            return NULL;

        bits=asn1_create_bits(tmp,ret * 8);
        if(bits)
        {
            asn = asn1_create(ASN1_SEQ, 2 + len1 + len2 + bits->total);
            if(asn)
            {
                int tam = 0;

                asn->data[tam++] = ASN1_SEQ;
                asn->data[tam++] = (unsigned char)(len1 + len2);

                memcpy(asn->data + tam,oid1,len1);
                tam+=len1;

                if(len2)
                {
                    memcpy(asn->data + tam,oid2,len2);
                    tam+=len2;
                }

                memcpy(asn->data + tam,bits->start,bits->total);
                //tam+=bits->total;

            }
        }
        asn1_free(bits);
    }
    return asn;
}

/* -------------------------- */
/* -------------------------- */

asn1_t *ecdsa_sign_to_asn1(ecdsa_sign_t *sign,int bits)
{
    asn1_t *asn=NULL,*tmpr,*tmps;

    if(sign)
    {
        tmpr=asn1_create_bignum(&(sign->r));
        tmps=asn1_create_bignum(&(sign->s));

        if(tmpr && tmps)
        {
            asn = asn1_create(ASN1_SEQ,tmpr->total + tmps->total);
            if(asn)
            {
                memcpy(asn->data,tmpr->start,tmpr->total);
                memcpy(asn->data + tmpr->total,tmps->start,tmps->total);

                if(bits)
                {
                    asn1_t *tmp = asn1_create_bits(asn->start,asn->total * 8);
                    asn1_free(asn);
                    asn = tmp;
                }
            }
        }

        asn1_free(tmpr);
        asn1_free(tmps);

    }
    return asn;
}


/* --------------------------------------------- */

int ecdsa_sign_from_asn1(ecdsa_sign_t *sign,const void *data,int tam)
{
    asn1_t asn;
    asn1_seq_t seq;
    int ret = -1;

    /* Read sequence */

    if(asn1_read(&asn,data,tam) > 0 && asn.type == ASN1_SEQ && !asn1_seq_read(&seq,asn.data,asn.len))
    {
        if(seq.num == 2 && seq.lst[0].type==ASN1_INT && seq.lst[1].type==ASN1_INT)
        {
            if(sign)
            {
                asn1_to_bignum(&(sign->r),&(seq.lst[0]));
                asn1_to_bignum(&(sign->s),&(seq.lst[1]));
                ret = 0;
            }
        }
        asn1_seq_free(&seq);
    }
    return ret;
}

/* -------------------------- */

void ecdsa_init_sign(ecdsa_sign_t *sign)
{
    if(sign)
    {
        mp_init(&(sign->r));
        mp_init(&(sign->s));
    }
}

/* -------------------------- */

void ecdsa_destroy_sign(ecdsa_sign_t *sign)
{
    if(sign)
    {
        mp_clear(&(sign->r));
        mp_clear(&(sign->s));
    }
}

/* -------------------------- */

int ecdsa_sign_hash(ecc_key_t *key, const void *hash, int hlen, ecdsa_sign_t *sign)
{
    hash_t ctx;
    unsigned char digest[SHA512_SIZE];
    mp_int_t k, tmp;
    ecc_point_t pt;

    if(hlen < MIN_HASH_SIZE || !key->pri)
        return -1;

    /* Check if we are given a K */

    if(key->k)
    {
        mp_init_set_string(&k,key->k,16);  /* Yup */
        //mp_show("Given K",&k);
    }
    else
    {
        /*  We build k using Simon Tatham 's deterministic method
           (see dsa_sign_hash() in dsa.c for details)
        */

        hash_init(&ctx,HASH_SHA512);
        hash_update(&ctx,"Simons's deterministic k generator", 34);
        mp_copy_exact_bytes(key->pri,digest,SHA512_SIZE);
        hash_update(&ctx,digest,SHA512_SIZE);
        hash_final(&ctx,digest);

        hash_init(&ctx,HASH_SHA512);
        hash_update(&ctx,digest,SHA512_SIZE);
        hash_update(&ctx,hash,hlen);
        hash_final(&ctx,digest);

        mp_init_set_bytes(&k,SHA512_SIZE,digest);        
    }

    /* Make sure k is in order of ECC's n */

    ecc_make_order(key->ctx,&k);

    /* Let's sign... */

    ecc_point_init(&pt);
    ecc_point_mult(key->ctx,&pt,&(key->ctx->G),&k);

    mp_mod(&(pt.x),&(key->ctx->n),&(sign->r));

    memset(digest, 0, sizeof(digest));

    mp_init_set_bytes(&tmp,(hlen > key->ctx->NUMBYTES) ? key->ctx->NUMBYTES : hlen, hash);

    mp_invmod(&k, &(key->ctx->n), &k);	              /* k^-1 mod N */
    mp_mul_add(key->pri,&(sign->r),&tmp,&(sign->s));  /* s = (hash + x*r) */
    mp_mod(&(sign->s), &(key->ctx->n), &tmp);         /* tmp = (hash + x*r)  mod n */
    mp_mulmod(&k,&tmp,&(key->ctx->n),&(sign->s));     /* s = k^-1 * (hash + x*r) mod N */

    ecc_point_set_zero(&pt);

    mp_clear(&tmp);
    mp_clear(&k);

    return 0;
}

/* -------------------------- */

int ecdsa_sign_data(ecc_key_t *key, int alg, const void *data, int dlen, ecdsa_sign_t *sign)
{
    hash_t ctx;
    unsigned char hash[MAX_HASH_SIZE];
    int ret,hlen,halg;

    if(!sign || !data || !key)
        return -1    ;

	/* Make sure the hash is supported and that is equal or smaller than
       the needed bytes to form a signature */

    halg = ecc_signing_hash(alg);

    hlen = hash_init(&ctx,halg);
    if(hlen < MIN_HASH_SIZE)
        return -1;
    
    /* Ok, build the hash */

    hash_update(&ctx,data,dlen);
    hash_final(&ctx,hash);

    ret = ecdsa_sign_hash(key, hash, hlen,sign);

    memset(hash, 0, sizeof(hash));

    return ret;
}

/* -------------------------- */

int ecdsa_verify_hash(ecc_key_t *key,const void *hash,int hlen,ecdsa_sign_t *sign)
{
    mp_int_t w, u1,u2, sha;
    ecc_point_t p1,p2;
    int ret;


    if(hlen < MIN_HASH_SIZE)
        return -1;

    mp_init(&w);
    mp_init(&u1);
    mp_init(&u2);

    /*
     * Step 1. w <- s^-1 mod n.
     */

    mp_invmod(&(sign->s),&(key->ctx->n),&w);

    /*
     * Step 2. u1 <- SHA(message) * w mod p.
     */

    mp_init_set_bytes(&sha,(hlen > key->ctx->NUMBYTES) ? key->ctx->NUMBYTES : hlen,hash);

    mp_mulmod(&sha,&w,&(key->ctx->n),&u1);

    mp_clear(&sha);

    /*
     * Step 3. u2 <- r * w mod n.
     */

    mp_mulmod(&(sign->r),&w,&(key->ctx->n),&u2);

    mp_clear(&w);

    ecc_point_init(&p1);
    ecc_point_init(&p2);

    ecc_point_mult(key->ctx,&p1,&(key->ctx->G),&u1);
    ecc_point_mult(key->ctx,&p2,&(key->pub),&u2);

    mp_clear(&u1);
    mp_clear(&u2);

    ecc_point_add(key->ctx,&p1,&p2);
    /*
     * Step 5. v should now be equal to r.
     */

    ret = (mp_cmp(&(p1.x), &(sign->r))==MP_EQ) ? 0 : -3;

    ecc_point_set_zero(&p1);
    ecc_point_set_zero(&p2);

    return ret;
}

/* -------------------------- */

int ecdsa_verify_sign(ecc_key_t *key,int alg,const void *data, int dlen,ecdsa_sign_t *sign)
{
    unsigned char hash[MAX_HASH_SIZE];
    int ret;
    hash_t ctx;

    if (!key || !data || !sign)
	    return -1;

	/* Make sure the hash is supported and that is equal or smaller than
       the needed bytes to form a signature */

    ret=hash_init(&ctx,ecc_signing_hash(alg));
    if(ret < MIN_HASH_SIZE)
        return -1;

    /* Ok, build the hash */

    hash_update(&ctx,data,dlen);
    hash_final(&ctx,hash);

    ret = ecdsa_verify_hash(key, hash,ret,sign);

    memset(hash, 0, sizeof(hash));

    return 0;
}

/* ------------------------ */
/* ------------------------ */

ecc_dh_t *ecc_dh_start(ecc_curve_t *ctx,const void *rpub,size_t rsz,rand_t *rc)
{
    ecc_dh_t *dh=NULL;

    if(ctx)
    {
        rand_t   *orc=NULL;

        if(rc==NULL)
        {
            orc=rand_start(RAND_OS,RAND_TLS_SHA256);
            if(orc==NULL)
                return NULL;
            rc = orc;
        }
        dh=(ecc_dh_t *)calloc(sizeof(ecc_dh_t),1);
        if(dh)
        {
            dh->ctx=ctx;

            ecc_point_init(&(dh->lpub));
            ecc_point_init(&(dh->rpub));

            ecc_random_field(ctx,rc,&(dh->lpri));

            ecc_point_mult(ctx,&(dh->lpub),&(ctx->G),&(dh->lpri));

            /* set remote key if any */

            if(rpub)
                ecc_point_from_bytes(dh->ctx,&(dh->rpub),rpub,rsz);
        }
        rand_end(orc);
    }
    return dh;
}


/* ------------------------ */

ecc_dh_t *ecc_dh_start_from_bytes(ecc_curve_t *ctx,const void *rpub,size_t rsz,const void *data,size_t cnt)
{
    ecc_dh_t *dh=NULL;

    if(ctx)
    {
        dh=(ecc_dh_t *)calloc(sizeof(ecc_dh_t),1);
        if(dh)
        {
            dh->ctx=ctx;

            ecc_point_init(&(dh->lpub));
            ecc_point_init(&(dh->rpub));

            mp_init_set_bytes(&(dh->lpri),cnt,data);
            ecc_make_order(ctx,&(dh->lpri));
            ecc_point_mult(ctx,&(dh->lpub),&(ctx->G),&(dh->lpri));

            /* set remote key if any */

            if(rpub)
                ecc_point_from_bytes(dh->ctx,&(dh->rpub),rpub,rsz);
        }
    }
    return dh;
}

/* ------------------------ */

void ecc_dh_end(ecc_dh_t *dh,const void *pub,size_t psz,void *shared)
{
    if(dh)
    {
        if(pub)
        {
            ecc_point_from_bytes(dh->ctx,&(dh->rpub),pub,psz);
        }
        if(shared)
        {
            ecc_point_t p;


            /* DH pass */

            ecc_point_init(&p);
            ecc_point_mult(dh->ctx,&p,&(dh->rpub),&(dh->lpri));

            mp_copy_exact_bytes(&(p.x),shared,dh->ctx->NUMBYTES);

            ecc_point_set_zero(&p);
        }

        /* Burn evidence and free memory */

        mp_clear(&(dh->lpri));
        ecc_point_set_zero(&(dh->lpub));
        ecc_point_set_zero(&(dh->rpub));
        memset(dh,0,sizeof(ecc_dh_t));
        free(dh);
    }
}

/* ------------------------ */

int ecc_dh_get_public(ecc_dh_t *dh,void *pub,size_t max,int compressed)
{
    if(dh && pub)
        return ecc_point_to_bytes(dh->ctx,&(dh->lpub),pub,max,compressed);
    return -1;
}


