/*
    pem.c

    Basic PEM file support

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

            - RFC 1421 at https://www.rfc-editor.org/rfc/rfc1421.txt
            - RFC 2898 at https://www.ietf.org/rfc/rfc2898.txt
            - RFC 5208 at https://tools.ietf.org/html/rfc5208

                                --oO0Oo--
*/

#include "pem.h"


static struct _pem_ele_
{
    int elem;
    const char *name;
}

pem_elems[] =
{
    {PEM_PRIVATE_KEY,     "PRIVATE KEY"},
    {PEM_ENC_PRIVATE_KEY, "ENCRYPTED PRIVATE KEY"},
    {PEM_PUBLIC_KEY,      "PUBLIC KEY"},
    {PEM_TRUSTED_CERT,    "TRUSTED CERTIFICATE"},
    {PEM_CERTIFICATE,     "CERTIFICATE"},
    {PEM_X509_CERT,       "X509 CERTIFICATE"},
    {PEM_X509_CRL,        "X509 CRL"},
    {PEM_CERT_REQUEST,    "CERTIFICATE REQUEST"},
    {PEM_DSA_PRIVATE_KEY, "DSA PRIVATE KEY"},
    {PEM_RSA_PRIVATE_KEY, "RSA PRIVATE KEY"},
    {PEM_EC_PRIVATE_KEY,  "EC PRIVATE KEY"},
    {PEM_EC_PARAMS,       "EC PARAMETERS"},
    {PEM_DH_PARAMS,       "DH PARAMETERS"},
    {PEM_DSA_PARAMS,      "DSA PARAMETERS"},
    {PEM_PKCS7,           "PKCS7"},
    {PEM_MESSAGE,         "PRIVACY-ENHANCED MESSAGE"},
    {PEM_SSL_PARAMS,      "SSL SESSION PARAMETERS"},

    /* Always last */

    {PEM_ELEM_COUNT,NULL}
};

/* ----------------------------------------- */

const char *pem_element_name(int element)
{
    int pos=0;

    while(pem_elems[pos].name)
    {
        if(pem_elems[pos].elem==element)
            return pem_elems[pos].name;
        pos++;
    }
    return NULL;
}

/* ----------------------------------------- */

int pem_element_type(const char *element)
{
    int pos=0;

    while(pem_elems[pos].name)
    {
        if(!strcmp(pem_elems[pos].name,element))
            return pem_elems[pos].elem;
        pos++;
    }
    return PEM_ELEM_NONE;
}


/* ----------------------------------------- */

static int pem_read_element_body(pem_file_t *pem,int type)
{
    char *ptr,*next,*end;    
    size_t left;
        
    pem->dat_len = 0;
    pem->elem.flags = 0;                 
    pem->elem.elem = type;
    pem->elem.data = NULL;
    pem->elem.size = 0;

    while(!feof(pem->fp))
    {   
        if(ferror(pem->fp))
            return -3;
        left = (pem->dat_max - pem->dat_len);
        if(left < 4096)
        {
            ptr = (char *)realloc(pem->dat_buf,pem->dat_max + 4096);
            if(!ptr)
            {
                /* Not enough memory to grow the data buffer */
                return -2;
            }
            pem->dat_buf  = ptr;
            pem->dat_max += 4096;   
            left = 4096;
        }
        next = (pem->dat_buf + pem->dat_len);       
        ptr = fgets(next,left,pem->fp);
        if(ptr == NULL)
            continue;            
        while(isspace(*ptr))
        {
            ptr++;            
            pem->dat_len++;
        }
        if(!strncmp(ptr,"-----END ",9))
        {
           *ptr='\0';            
            ptr+=9;
            end = strstr(ptr,"-----");
            if(end && (end[5]=='\n' || end[5]=='\r'))
            {
                *end = 0;                
                if(type != pem_element_type(ptr))
                {
                    /* Found ending mark for another label instead of the current */
                    return -4;                    
                }         
                pem->elem.data = pem->dat_buf;
                pem->elem.size = pem->dat_len;
                return 1; /* Read 1 element */
            }
            /* IGNORING this section as it has the incorrect label format */
            return -4;
        }        
        if(!pem->elem.flags && strstr((const char *)ptr,"Proc-Type: 4,ENCRYPTED"))
            pem->elem.flags = PEM_F_CRYPT;
        pem->dat_len += strlen(ptr);
    }
    pem->eof = TRUE;
    return 0;    /* No more data to read */
}

/* ----------------------------------------- */

static int pem_read_next_element(pem_file_t *pem)
{
    int ret,elem;    
    char *ptr, *lst,name[132]={0};
    
    while(!feof(pem->fp))
    {                           
        ptr = fgets(name,128,pem->fp);       
        if(ptr == NULL) 
        {
            if(ferror(pem->fp))
                return -3;
            continue;
        }       
        while(isspace(*ptr))
            ptr++;
        if(strncmp(ptr,"-----BEGIN ",11))
            continue;
        ptr+=11;
        lst = strstr(ptr,"-----");
        if(lst && (lst[5]=='\n' || lst[5]=='\r'))
        {            
           *lst=0;
            elem = pem_element_type(ptr);
            if(elem == PEM_ELEM_NONE)
            {
                /* IGNORING UNKNOWN ELEMENT */
                continue;
            }               
            ret = pem_read_element_body(pem,elem);
            if(ret != -4)
                return ret;            
        }
        if(ferror(pem->fp))
            return -3;      
    }
    /* Signal de EOF */
    pem->eof=TRUE; 
    return 0;  /* No more data to read */
}

/* ----------------------------------------- */

void pem_close(pem_file_t  *pem)
{
    if(pem)
    {
        if(pem->fp)
            fclose(pem->fp);

        if(pem->dat_buf)
        {
            memset(pem->dat_buf,0,pem->dat_len);
            free(pem->dat_buf);
        }
        
        memset(pem,0,sizeof(pem_file_t));
        free(pem);
    }
}

/* ----------------------------------------- */

static int do_pem_open(pem_file_t **rpem,const void *file,int wc)
{
    pem_file_t *pem;
    int ret;

    if(file && rpem)
    {        
       *rpem = NULL; 
        pem=(pem_file_t *)calloc(sizeof(pem_file_t),1);
        if(pem)
        {
            if(wc)
                pem->fp = fopenw((const wchar_t *)file,"r");
            else
                pem->fp = fopen((const char *)file,"r");       
            if(pem->fp)
            {
                /* Load first ignoring wrong sections and unknwon ones */
                do {
                    ret = pem_read_next_element(pem);
                } while( ret == -4 );
                if(ret < 0)
                {
                    pem_close(pem);
                    return ret;
                }
               *rpem=pem; 
                return 0;
            }
            free(pem);
            return -3;
        }
        return -2;
    }
    return -1;
}

/* ---------------------- */

int pem_open(pem_file_t **ret,const char *file)
{
    return do_pem_open(ret,file,FALSE);
}

/* ---------------------- */

int pem_openw(pem_file_t **ret,const wchar_t *file)
{
    return do_pem_open(ret,file,TRUE);
}

/* ---------------------- */

static pem_elem_t *do_find_element(pem_file_t *pem,int type)
{
    int ret = 0;

    while(pem->eof == FALSE && ret != -3)
    {
        ret = pem_read_next_element(pem);
        if(ret == 1 && (type == pem->elem.elem || type == -1))
            return &(pem->elem);
    }
    return NULL;
}

/* ---------------------- */

pem_elem_t *pem_first_element(pem_file_t *pem,int type)
{
    if(pem && pem->fp)
    {
        rewind(pem->fp);
        pem->eof = FALSE;
        return do_find_element(pem,type);
    }
    return NULL;
}

/* ---------------------- */

pem_elem_t *pem_next_element(pem_file_t *pem,pem_elem_t *prev)
{    
    if(pem && pem->fp && prev)
        return do_find_element(pem,prev->elem);
    return NULL;
}


/* ---------------------- */

pem_elem_t *pem_get_first_element(pem_file_t *pem)
{
    if(pem && pem->fp)
    {
        rewind(pem->fp);
        pem->eof = FALSE;
        return do_find_element(pem,-1);
    }
    return NULL;
}

/* ---------------------- */

pem_elem_t *pem_get_next_element(pem_file_t *pem)
{    
    if(pem && pem->fp)
        return do_find_element(pem,-1);
    return NULL;
}

/* ---------------------- */
/* ---------------------- */

/* PKCS-5 stuff for decoding private keys in PKCS-8 format */

enum
{
    PBE_MD2_DES_CBC=0,
    PBE_MD5_DES_CBC,
    PBE_SHA1_DES_CBC,
    PBKDF2,
    PBES2,
    /* Always last */
    PBE_ULT
};

typedef struct
{
    int which;
    char *name;
    int hash,alg;
    char *oid;
    unsigned char asn[11];
} pkcs5_t;

/*  Note that we do not support any RC2 PBEs because is too old (1987) and its
    very unlikely that we will find key files encrypted with them. Actually, we
    support DES only because OpenSSL still uses it as default algorithm when
    the parameter -topk8 is used without specifying an algorithm, thus files
    with it can show up anytime.

    Its important to highlight that these PBEs use DES (56 bits), not Triple DES
    (168 bits), so its a bit upsetting that OpenSSL still uses it as default when
    better options like PBES2 and PBKDF2 exist.
*/

static pkcs5_t pkcs5_lst[] =
{
    {
        PBE_MD2_DES_CBC,"pbeWithMD2AndDES-CBC" , HASH_MD2,DES_EDE3_CBC,
        "1.2.840.113549.1.5.1",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x05,0x01}
    },
    {
        PBE_MD5_DES_CBC,"pbeWithMD5AndDES-CBC" , HASH_MD5,DES_EDE3_CBC,
        "1.2.840.113549.1.5.3",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x05,0x03}
    },
    {
        PBE_SHA1_DES_CBC,"pbeWithSHA1AndDES-CBC",HASH_SHA1,DES_EDE3_CBC,
        "1.2.840.113549.1.5.10",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x05,0x0A}
    },
    {
        PBKDF2,"PBKDF2",0,0,
        "1.2.840.113549.1.5.12",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x05,0x0C}
    },
    {
        PBES2,"PBES2",0,0,
        "1.2.840.113549.1.5.13",{0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x05,0x0D}
    },

    {PBE_ULT,NULL,0,0,NULL,{0}},
};

/* ---------------------------- */

static pkcs5_t *pkcs5_find(const unsigned char *asn1,size_t tam)
{
    pkcs5_t *ptr = (pkcs5_t *)pkcs5_lst;

    while(ptr->name)
    {
        if(tam == 9 && !memcmp(ptr->asn + 2,asn1,tam))
            return ptr;
        ptr++;
    }
    return NULL;
}

/* ---------------------------- */

static int pem_decode_pkcs8(pem_elem_t *elem,const char *password)
{
    char *prox=NULL;
    size_t tam;
    asn1_t asn;
    asn1_seq_t seq,tmp,parm1,parm2,parm3;
    unsigned char hval[MAX_HASH_SIZE],sval[64]={0};
    int htam=0,stam=0,alg,ret;
    unsigned int iterations=0;
    hash_t ctx;
    pkcs5_t *p5;

    /* Make sure it starts with a sequence */

    if(asn1_read(&asn,elem->data,elem->size) < 1 || asn.type != ASN1_SEQ)
        return -3;

    /* Now we read it */

    ret=asn1_seq_read(&seq,asn.data,asn.len);
    if(ret)
        return ret;

    if(seq.num != 2 || seq.lst[0].type!=ASN1_SEQ)
    {
        asn1_seq_free(&seq);
        return -3;
    }

    prox=NULL;
    tam = 0;

    ret=asn1_seq_read(&tmp,seq.lst[0].data,seq.lst[0].len);
    if(ret)
    {
        asn1_seq_free(&seq);
        return ret;
    }

    if(tmp.lst[0].type!=ASN1_OID)
    {
        asn1_seq_free(&seq);
        asn1_seq_free(&tmp);
        return -3;
    }


    p5 = pkcs5_find(tmp.lst[0].data,tmp.lst[0].len);
    if(p5)
    {
        switch(p5->which)
        {
            case PBE_MD2_DES_CBC:
            case PBE_MD5_DES_CBC:
            case PBE_SHA1_DES_CBC:

                /* PBES1: read parameters as per section A.3 of RFC-2898 */

                alg = p5->alg;

                if(tmp.lst[1].type != ASN1_SEQ || asn1_seq_read(&parm1,tmp.lst[1].data,tmp.lst[1].len))
                {
                    asn1_seq_free(&seq);
                    asn1_seq_free(&tmp);
                    return -3;
                }

                if(parm1.num > 1)
                {
                    if(parm1.lst[0].type == ASN1_BYTES && parm1.lst[0].len > 7)
                    {
                        stam = 8;
                        memcpy(sval,parm1.lst[0].data,8);
                    }

                    asn1_to_uint(parm1.lst + 1,&iterations);
                }

                asn1_seq_free(&parm1);

                /* Calculate PBKDF1 of PBES1 as per section 5.1 of RFC-2898 */

                htam = hash_init(&ctx,alg);
                hash_update(&ctx,password,strlen(password));
                hash_update(&ctx,sval,stam);
                hash_final(&ctx,hval);

                while(iterations > 1)
                {
                    hash_init(&ctx,alg);
                    hash_update(&ctx,hval,htam);
                    hash_final(&ctx,hval);

                    iterations--;
                }

                /* Decrypt as per section 6.1.2 of RFC-2898 */

                if(htam > 15)
                {
                    cipher_t ctx;

                    if(cipher_init(&ctx,DES_ECB,hval + 8,NULL,0)==0)
                    {
                        prox = (char *)seq.lst[1].data;
                        tam = seq.lst[1].len;
                        cipher_decode(&ctx,prox,prox,tam);
                        cipher_end(&ctx);
                    }
                }
                break;

            case PBES2:

                /* PBES2: lread parameters as per section A.4 of RFC-2898 */

                if(tmp.lst[1].type != ASN1_SEQ ||
                   asn1_seq_read(&parm1,tmp.lst[1].data,tmp.lst[1].len))
                {
                    asn1_seq_free(&tmp);
                    asn1_seq_free(&seq);
                    return -3;
                }

                /* Check they contain keyDerivationFunc and encryptionScheme */

                if(parm1.num < 2 || parm1.lst[0].type != ASN1_SEQ ||
                   parm1.lst[1].type != ASN1_SEQ)
                {
                    asn1_seq_free(&parm1);
                    asn1_seq_free(&tmp);
                    asn1_seq_free(&seq);
                    return -3;
                }

                /* Read KeyDerivationFunc */

                if(!asn1_seq_read(&parm2,parm1.lst[0].data,parm1.lst[0].len) &&
                   parm2.num > 1 && parm2.lst[0].type==ASN1_OID)
                {
                    /* make sure it is PBKDF2 */

                    p5 = pkcs5_find(parm2.lst[0].data,parm2.lst[0].len);
                    if(p5 && p5->which==PBKDF2 && parm2.lst[1].type == ASN1_SEQ)
                    {
                        /* Read parameters */

                        if(!asn1_seq_read(&parm3,parm2.lst[1].data,parm2.lst[1].len) && parm3.num > 1)
                        {
                            if(parm3.lst[0].type == ASN1_BYTES)
                            {
                                stam = parm3.lst[0].len;
                                if(stam > 64)
                                    stam = 64;
                                memcpy(sval,parm3.lst[0].data,stam);
                            }

                            asn1_to_uint(parm3.lst + 1,&iterations);
                        }
                        asn1_seq_free(&parm3);
                    }
                }

                asn1_seq_free(&parm2);
                asn1_seq_free(&parm1);

                break;
            default:
                asn1_seq_free(&tmp);
                asn1_seq_free(&seq);
                return -3;
        }
    }

    asn1_seq_free(&tmp);
    asn1_seq_free(&seq);

    if(prox)
    {
        elem->elem = PEM_PRIVATE_KEY;
        elem->data = prox;
        elem->size = tam;
        return 0;
    }

    return -4;
}


/* ---------------------- */

int pem_decode_element(pem_elem_t *elem,const char *password)
{
    if(!elem)
        return EINVAL;
    if(elem->flags & PEM_F_ASN1)
        return 0;
    if(elem->flags & PEM_F_CRYPT)
    {
        unsigned char keyiv[32];
        md5_t md5;
        unsigned char dat_iv[32];
        int t,max,len_iv=0,v;
        int alg,ret;
        size_t tam;
        char *ptr,*prox;
        cipher_t ctx;

        if(password == NULL)
            return -1;

        ptr=strstr(elem->data,"DEK-Info: ");
        if(ptr==NULL)
            return -1;

        ptr+=10;

        prox=strchr(ptr,',');
        if(!prox)
            return -3;
       *prox++ = '\0'; 

        /* Read the algorithm */
        strtrim(ptr);
        alg = cipher_alg_from_name( ptr );        
        switch(alg)
        {
            case AES_128_CBC:
            case AES_192_CBC:
            case AES_256_CBC:            
            case DES_EDE3_CBC:
            case DES_CBC:
                max = cipher_alg_iv_size(alg);
                break;
            default:
                return -1;
        }

        /* Read the IV */

        ptr = prox;
        while(*ptr && !isxdigit(*ptr))
            ptr++;

        for(t=0;t < max;t++)
        {
            if (sscanf(ptr,"%2x", &v)!=1)
                break;
            dat_iv[len_iv++] = (unsigned char)v;
            ptr += 2;
        }
        if(max != len_iv)
            return -3;

        tam=base64_decode(ptr,strlen(ptr),ptr,0,TRUE);

        md5_init(&md5);
        md5_update(&md5,password,strlen(password));
        md5_update(&md5,dat_iv,8);
        md5_final(&md5,keyiv);

        md5_init(&md5);
        md5_update(&md5,keyiv,16);
        md5_update(&md5,password,strlen(password));
        md5_update(&md5,dat_iv,8);
        md5_final(&md5,keyiv + 16);

        ret = cipher_init(&ctx,alg,keyiv,dat_iv,0);
        if(ret)
            return ret;
        cipher_set_padding(&ctx,PAD_SIZE);
        elem->data = ptr;
        elem->size = cipher_decode(&ctx,ptr,ptr,tam);
        elem->flags &= ~PEM_F_CRYPT;
    }
    else
    {
        elem->size=base64_decode(elem->data,elem->size,elem->data,0,TRUE);
    }

    elem->flags |= PEM_F_ASN1;

    if(elem->elem==PEM_ENC_PRIVATE_KEY)
    {
        int ret;
        ret = pem_decode_pkcs8(elem,password);
        if(ret)
        {
            /* CANNOT DECODE PKCS8 */
            return ret;
        }
    }
    return 0;
}


/* ---------------------- */
/* ---------------------- */

typedef struct
{
    unsigned char dat_iv[32];
    int  len_iv;

    unsigned char   *dat_enc;
    size_t  len_enc;

    char  alg[32];
} pem_encode_t;


static int pem_encode_data(pem_encode_t *cr,const unsigned char *dat,size_t total,const char *passcode,int algoritmo)
{
    unsigned char keyiv[32];
    md5_t md5;
    cipher_t ctx;
    int ret;

    switch(algoritmo)
    {
        case DES_CBC:
        case DES_EDE3_CBC:
            break;
        case AES_128_CBC:
        case AES_192_CBC:
        case AES_256_CBC:        
            break;
        default:
            algoritmo = AES_128_CBC;
            break;
    }

    cr->dat_enc = (unsigned char *)malloc(total + 256);
    if(cr->dat_enc == NULL)
        return ENOMEM;

    memcpy(cr->dat_enc,dat,total);

    hash_get_entropy(HASH_SHA256,cr->dat_iv,32);   

    md5_init(&md5);
    md5_update(&md5,passcode,strlen(passcode));
    md5_update(&md5,cr->dat_iv,8);
    md5_final(&md5,keyiv);

    md5_init(&md5);
    md5_update(&md5,keyiv,16);
    md5_update(&md5,passcode,strlen(passcode));
    md5_update(&md5,cr->dat_iv,8);
    md5_final(&md5,keyiv + 16);

    /* encodiiiiiiiiing... */

    ret = cipher_init(&ctx,algoritmo,keyiv,cr->dat_iv,0);
    if(ret == 0)
    {
        cipher_set_padding(&ctx,PAD_SIZE);
        strcpy(cr->alg,ctx.namec);
        cr->len_iv = ctx.iv_sz;        
        cr->len_enc = cipher_encode(&ctx,cr->dat_enc,cr->dat_enc,total);
    }
    else
    {
        free(cr->dat_enc);
    }
    cipher_end(&ctx);
    return ret;
}

/* ---------------------- */

int pem_save_element(FILE *fp,int elem,const void *data,size_t tam,const char *passcode,int alg)
{
    const unsigned char *ptr;
    const char *key;

    ptr = (const unsigned char *)data;

    key = pem_element_name(elem);
    if(key && data && fp)
    {
        /* Save the key */

        fprintf(fp,"-----BEGIN %s-----\n",key);

        if(passcode)
        {
            pem_encode_t cr;
            int ret,t;

            ret=pem_encode_data(&cr,ptr,tam,passcode,alg);
            if(ret)
                return ret;

            fprintf(fp,"Proc-Type: 4,ENCRYPTED\n");
            fprintf(fp,"DEK-Info: %s,",cr.alg);
            for(t=0;t<cr.len_iv;t++)
                fprintf(fp,"%02x",cr.dat_iv[t]);
            fprintf(fp,"\n\n");
            base64_save(fp,64,cr.dat_enc,cr.len_enc,FALSE);
            free(cr.dat_enc);
        }
        else
        {
            base64_save(fp,64,ptr,tam,FALSE);
        }

        fprintf(fp,"-----END %s-----\n",key);

        return 0;
    }
    return -1;
}

