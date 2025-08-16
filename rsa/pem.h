/*
    pem.h

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
            - RFC 7468 at https://datatracker.ietf.org/doc/html/rfc7468

                                --oO0Oo--

*/


#ifndef PEM_FILE_SUPPORT
#define PEM_FILE_SUPPORT

#include "random.h"
#include "base64.h"
#include "cipher.h"
#include "hmac.h"
#include "asn1.h"

/** Supported PEM Elements */

enum
{
    PEM_ELEM_NONE=-1,    
    PEM_PRIVATE_KEY,
    PEM_ENC_PRIVATE_KEY,
    PEM_PUBLIC_KEY,
    PEM_TRUSTED_CERT,
    PEM_CERTIFICATE,
    PEM_X509_CERT,
    PEM_X509_CRL,
    PEM_CERT_REQUEST,
    PEM_RSA_PRIVATE_KEY,
    PEM_DSA_PRIVATE_KEY,
    PEM_EC_PRIVATE_KEY,
    PEM_EC_PARAMS,
    PEM_DH_PARAMS,
    PEM_DSA_PARAMS,
    PEM_PKCS7,
    PEM_MESSAGE,
    PEM_SSL_PARAMS,

    PEM_ELEM_COUNT,
};

/* PEM structure */

typedef struct
{
    int      flags;    /* Flags */
    int      elem;     /* Type of element */
    void    *data;     /* Data buffer */
    size_t   size;     /* Size of element's data */    
} pem_elem_t;

/* PEM flags */

#define PEM_F_CRYPT     0x0001  /* It is encrypted */
#define PEM_F_ASN1      0x0002  /* Converted to ASN1 */

/* PEM File */

typedef struct
{
    pem_elem_t  elem;

    char   *dat_buf;   /* Data buffer of the PEM file */
    size_t  dat_len;   /* size of data on the buffer */
    size_t  dat_max;   /* Max size of buffer (biggest read element) */   

    FILE       *fp;    /* File desriptor */
    int         eof;   /* TRUE if ALL file was read) */    
} pem_file_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------ *
   Return the name of an element given its ID and
   viceversa
 * ------------------------------------------------------ */

const char *pem_element_name(int element);
int pem_element_type(const char *element);


/* ------------------------------------------------------ *
   Open PEM 'file' and read first element, returns NULL 
   if the file cannot be opened or
 * ------------------------------------------------------ */

int pem_open(pem_file_t **pem,const char *file);
int pem_openw(pem_file_t **pem,const wchar_t *file);

/* ------------------------------------------------------ *
   Free resources from a loaded PEM
 * ------------------------------------------------------ */

void pem_close(pem_file_t  *pem);

/* ------------------------------------------------------ *
  return the first element of a given type or NULL if
  none can be found
 * ------------------------------------------------------ */

pem_elem_t *pem_first_element(pem_file_t *pem,int type);

/* ------------------------------------------------------ *
   return the next element of teh given type or NULL
   if not more can be found
 * ------------------------------------------------------ */

pem_elem_t *pem_next_element(pem_file_t *pem,pem_elem_t *prev);

/* ------------------------------------------------------ *
   Same as previous two but gets any element regardless
   its type
 * ------------------------------------------------------ */

pem_elem_t *pem_get_first_element(pem_file_t *pem);
pem_elem_t *pem_get_next_element(pem_file_t *pem);


/* ------------------------------------------------------ *
   Decode an element from base64 to ASN.1 if not already
   decoded. 'passcode' is the passcode to decrypt the
   element if needed
 * ------------------------------------------------------ */

int pem_decode_element(pem_elem_t *elem,const char *passcode);

/* ------------------------------------------------------ *
   Save a element of type 'elem' with 'tam' bytes of 
   'data' to a PEM file.
   If 'passcode' is not NULL, the data is encrypted
   using algorithm 'alg', which -if not recognized-
   defaults to AES-128-CBC
 * ------------------------------------------------------ */

int pem_save_element(FILE *fp,int elem,const void *data,size_t tam,const char *passcode,int alg);

#ifdef __cplusplus
};
#endif


#endif
