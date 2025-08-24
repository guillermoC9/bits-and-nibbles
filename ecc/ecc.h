/*
    ecc.h

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

#ifndef ECC_PROTOCOLS_H
#define ECC_PROTOCOLS_H

#include "curves.h"
#include "pem.h"

/* Limits */

#define ECDSA_MIN_BITS          ECC_MIN_BITS
#define ECDSA_MAX_BITS          ECC_MAX_BITS
#define ECDSA_MAX_BYTES         ECC_MAX_BYTES

#define ECC_MAX_PRIVATE_DH      (ECDSA_MAX_BYTES)
#define ECC_MAX_PUBLIC_DH       (ECDSA_MAX_BYTES * 2 + 1)
#define ECC_MAX_SHARED_DH       (ECDSA_MAX_BYTES)

#define ECC_MAX_HEX_BUFFER      (ECC_MAX_PUBLIC_DH * 2 + 1)

/* ECC Keys context */

typedef struct
{
    ecc_curve_t  *ctx;      /* Private copy of key parameters */

    mp_int_t       *pri;    /* Private key (optional) */
    ecc_point_t  pub;       /* Public key */

    const char     *k;      /* Only used for test stuff */
} ecc_key_t;


/* Useful macros */

#define ecc_bits(_r)         ((_r)->ctx->NUMBITS)
#define ecc_key_size(_r)     ((_r)->ctx->NUMBYTES)
#define ecc_bufsize(_r)      ((_r)->ctx->NUMBYTES)
#define ecc_has_private(_r)  ((_r)->pri)

/* Elliptic Curve Diffie-Hellman key exchange */

typedef struct
{
    ecc_curve_t *ctx;      /* Curve Parameters Context */

    mp_int_t     lpri;      /* Local Private key */
    ecc_point_t  lpub;      /* Local Public key */
    ecc_point_t  rpub;      /* Remote Public key */
} ecc_dh_t;

/* ECC Public Key Types List */

enum
{
    ECC_PUBLIC_KEY=0,       /* id-ecPublicKey (eg. ECDSA) */
    ECC_PUBLIC_DH_KEY,      /* id-ecDH (eg. ECDH) */
    ECC_PUBLIC_MQV_KEY,     /* id-ecMQV (eg. ECMQV) */
    ECC_PUBLIC_25519_KEY,   /* id-ecDH-25519 (eg. ECDH 25519) */
    ECC_PUBLIC_448_KEY,     /* id-ecDH-448 (eg. ECDH 448) */

    ECC_PUBLIC_KEY_COUNT    /* LEAVE ALWAYS LAST */
};

/* Elliptic curve Digital Signature Algorithms */

enum
{
    NPI_SIGN_ALG=0,         /* Non Probably Implemented SIGNature ALGgorithm */

    ECDSA_SHA1,             /* ecdsaWithSHA1*/
    ECDSA_SHA224,           /* ecdsaWithSHA224 */
    ECDSA_SHA256,           /* ecdsaWithSHA256 */
    ECDSA_SHA384,           /* ecdsaWithSHA384 */    
    ECDSA_SHA512,           /* ecdsaWithSHA512 */

    ECDSA_SHA3_224,         /* ecdsaWithSHA3-224 */
    ECDSA_SHA3_256,         /* ecdsaWithSHA3-256 */
    ECDSA_SHA3_384,         /* ecdsaWithSHA3-384 */
    ECDSA_SHA3_512,         /* ecdsaWithSHA3-512 */
    
    EdDSA_25519,            /* ecdsaWithEd25519 */
    EdDSA_448,              /* ecdsaWithEd448 */

    LAST_ECC_SIGN_ALG /* LEAVE ALWAYS LAST */
};

/* Signature */

typedef struct
{
    mp_int_t r;
    mp_int_t s;
} ecdsa_sign_t;

/* Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    This is to check if  'alg' is a valid ECC sign
    algorithm. If it is it will return 'alg' otherwise
    it will return NPI_SIGN_ALG
 * -------------------------------------------------- */

int ecc_sign_alg(int alg);

/* -------------------------------------------------- *
    Returns the signature algorithm from an OID
 * -------------------------------------------------- */

int ecc_sign_algorithm(const char *oid);

/* -------------------------------------------------- *
    Returns the signature algorithm from an OID but
    encoded in ASN.1/DER
 * -------------------------------------------------- */

int ecc_sign_algorithm_asn1(unsigned char *oid,int tam);

/* -------------------------------------------------- *
    Returns the OID from signature algorithm
 * -------------------------------------------------- */

char *ecc_sign_oid(int cual);

/* -------------------------------------------------- *
    Returns the name of a signature algorithm
 * -------------------------------------------------- */

char    *ecc_sign_name(int cual);
wchar_t *ecc_sign_namew(int cual);

/* -------------------------------------------------- *
    Returns the signature algorithm  from its name
 * -------------------------------------------------- */

int ecc_sign_alg_from_name(const char *nombre);
int ecc_sign_alg_from_namew(const wchar_t *nombre);

/* -------------------------------------------------- *

    Copies the OID -in ASN.1/DER format- of a given
    signature algorithm.

    It returns the number of bytes written or 0 if
    doesn't recognize the given algorithm.

    'des' can be NULL if you only want to know the
    length of a given signature in ASN.1/DER

 * -------------------------------------------------- */

int ecc_sign_oid_asn1(int cual,unsigned char *dest,int max);

/* -------------------------------------------------- *
    Returns the hash used by a given signature
    algorithm
 * -------------------------------------------------- */

int ecc_signing_hash(int cual);

/* -------------------------------------------------- *
   Destroy keys created with the key generation/load
   functions
 * -------------------------------------------------- */

void ecc_free_keys(ecc_key_t *kc);

/* -------------------------------------------------- *
    check if both keys are the same
 * -------------------------------------------------- */

int ecc_public_key_equal(ecc_key_t *a,ecc_key_t *b);
int ecc_private_key_equal(ecc_key_t *a,ecc_key_t *b);

/* -------------------------------------------------- *
   Copy the private key in src into dst
 * -------------------------------------------------- */

int ecc_private_key_copy(ecc_key_t *dst,ecc_key_t *src);

/* -------------------------------------------------- *
    Generate an ECC key context from a given curve
    and a random generator context.

    if 'rc' is not provided -i.e. is NULL- then a
    generator is choosen, trying first the system
    RAND_OS and if not present then it uses
    RAND_TLS_SHA256.
 * -------------------------------------------------- */

ecc_key_t *ecc_generate_keys(int curve,rand_t *rc);

/* -------------------------------------------------- *
    Generate an ECC key context from a given curve
    and given raw data bytes.

    If cnt is smaller than the number of bytes needed
    it returns NULL.

    Extra bytes will be truncated.
 * -------------------------------------------------- */

ecc_key_t *ecc_keys_from_bytes(int curve,const void *pri,size_t pvcnt,const void *pub,size_t pucnt);
ecc_key_t *ecc_keys_from_chars(int curve,const char *pri,const char *px,const char *py);

/* ------------------------------------------------------------ *
    Builds an ECC key context from a byte buffer
    in ASN.1/DER

    It accepts these formats:

    - PublicKeyInfo (PKCS-8 public key)

        PublicKeyInfo ::= SEQUENCE
        {
            algorithm       AlgorithmIdentifier,
            PublicKey       BIT STRING
        }

        AlgorithmIdentifier ::= SEQUENCE
        {
            algorithm       OBJECT IDENTIFIER,
            parameters      ANY DEFINED BY algorithm OPTIONAL
        }

        ECParameters ::= CHOICE
        {
            NamedCurve      OBJECT IDENTIFIER,
        }


    - PrivateKeyInfo (Clave Privada en formato PKCS-8)

        PrivateKeyInfo ::= SEQUENCE
        {
            version         Version
            algorithm       AlgorithmIdentifier,
            PrivateKey      BIT STRING
        }

        AlgorithmIdentifier ::= SEQUENCE
        {
            algorithm       OBJECT IDENTIFIER,
            parameters      ANY DEFINED BY algorithm OPTIONAL
        }

    - ECPrivateKeyInfo

        ECPrivateKeyInfo ::= SEQUENCE
        {
            version         Version
            PrivateKey      OCTECT STRING
            parameters [0]  ECParameters OPTIONAL,
            PublicKey  [1]  BIT STRING OPTIONAL
        }

        Although marked as OPTIONAL, section 3 of RFC-5915
        says that conforming implementations  MUST include
        the 'parameters' field.

        'PublicKey' SHOULD be present, but if it was ommitted
        it can be calculated from the private key and the
        parameters (which is why it should always be present).

 * ------------------------------------------------------------ */


int ecc_keys_from_asn1(ecc_key_t **k,const void *bytes,int tam);

/* -------------------------------------------------- *
   Creates a ECC context from an ASN.1/DER sequence,
   like ecc_keys_from_asn1() does, but in this case
   the header of the sequence must not be there.
 * -------------------------------------------------- */

int ecc_keys_from_seq_asn1(ecc_key_t **k,const void *bytes,int tam);

/* -------------------------------------------------- *
   Builds a public ECC key context from 2 buffers 
   encoded in ASN.1/DER with the public key format

        - PublicKeyInfo (PKCS-8 public key)

            PublicKeyInfo ::= SEQUENCE
            {
                algorithm       AlgorithmIdentifier,
                PublicKey       BIT STRING
            }
            
            AlgorithmIdentifier ::= SEQUENCE
            {
                algorithm       OBJECT IDENTIFIER,
                parameters      ECParameters IF REQUIRED BY algorithm OPTIONAL
            }

            ECParameters ::= CHOICE
            {
                NamedCurve      OBJECT IDENTIFIER
            }

        Basically 'seq' is PublicKeyInfo and 'pub' is
        AlgorithmIdentifier

*/

ecc_key_t *ecc_public_key_from_asn1(asn1_seq_t *seq,asn1_seq_t *pub);

/* -------------------------------------------------- *
   Return a public key type from a OID block in ASN.1   
   or converts such a block to the public key type
 * -------------------------------------------------- */

int ecc_public_key_type_from_asn1(const void *buf,int blen);
int ecc_public_key_type_to_asn1(int key, void *buf,int max);


/* -------------------------------------------------- *

    Loads a DSA key from a PEM 'file' in PKCS-8
    format.

    If 'public' is TRUE, it will look for a ECC
    public key in PKCS-8 formta inside the file.

    If it is FALSE it will look for the private key.

    The PKCS-8 DSA private key can be encrypted using
    AES-256-CBC, AES-192-CBC, AES-128-CBC or DES=EDE3-CBC
    using 'passcode' as the encription key.

    Returns:

        OK         - Loaded
        EINVAL     - 'cer' is NULL.
        EOPEN      - Cannot load the file (it may
                        not exist).
        EINCORRECT - The file format of the key is
                        incorrect.
        ENOIMPL    - an algorithm or curve used is
                        not supported
        ECRYPTED   - The key is encrypted but 'passcode'
                        was not provided (NULL)
        EPRIV      - They key failed the verification.
                        It might wrong or be corrupt.
 * -------------------------------------------------- */

int ecc_load_pem(const char *file,int publica,const char *passcode,ecc_key_t **ecc);
int ecc_load_pemw(const wchar_t *file,int publica,const char *passcode,ecc_key_t **ecc);

/* ----------------------------------------------------- *
    Encodes the public key int a ECC context to PKCS-8
    format in ASN.1/DER and returns the buffer.

    Format:

        PublicKeyInfo (look ecc_keys_from_asn1())

    Use asn1_free() to free the returned buffer
    once you do not need it anymore.
 * ----------------------------------------------------- */

asn1_t *ecc_public_to_asn1(ecc_key_t *key);


/* ----------------------------------------------------- *
    Same as the previous but encodes the private key

    Format:

        ECPrivateKeyInfo (look ecc_keys_from_asn1())

    Use asn1_free() to free the returned buffer
    once you do not need it anymore.

 * ----------------------------------------------------- */

asn1_t *ecc_private_to_asn1(ecc_key_t *key);

/* -------------------------------------------------- *
    Saves a public key to a PEM file in PKCS-8 format
    compatible with OpenSSL and other products.
 * -------------------------------------------------- */
int ecc_public_to_pem(ecc_key_t *key,FILE *fp);

/* -------------------------------------------------- *
   Same as previous but it saves the private key in
   PKCS-1 format instead.

   If 'passcode' is not NULL, it will encrypt the key
   using it with the provided 'alg' algorithm, which
   would be ignored if 'passcode' is NULL
 * -------------------------------------------------- */

int ecc_private_to_pem(ecc_key_t *key,const char *passcode,int alg,FILE *fp);


/* -------------------------------------------------- *
    Saves a ECC key context to a PEM file.

    If 'priv' is TRUE it saves as a private key
    format, encrypting it using 'passcode' if
    especified with 'alg as the cypher. Default is
    AES-128-CBC.

    If 'priv' is FALSE it will save it as a
    PKCS-8 public key format
 * -------------------------------------------------- */

int ecc_save_pem(const char *file,ecc_key_t *key,int priv,const char *passcode,int alg);
int ecc_save_pemw(const wchar_t *file,ecc_key_t *key,int priv,const char *passcode,int alg);

/* -------------------------------------------------- *

   ECDSA Stuff  (includes EdDSA-25519 & EdDSA-448)

 * -------------------------------------------------- */

/* -------------------------------------------------- *
    Convert a ECDSA signature to a ASN.1 sequence of
    bytes, or optionally to a bit string if 'bits' is
    TRUE

    free with asn1_free()
 * -------------------------------------------------- */

 asn1_t *ecdsa_sign_to_asn1(ecdsa_sign_t *sign,int bits);

 /* -------------------------------------------------- *
    Builds a DSA Signature result from a byte buffer
    encoded in ASN.1/DER

    It accepts this format:

        dsaSignatureValue:: = SEQUENCE
        {
            modulus r       INTEGER
            modulus s       INTEGER
        }
 * -------------------------------------------------- */

int ecdsa_sign_from_asn1(ecdsa_sign_t *sign,const void *data,int tam);


 /* -------------------------------------------------- *
     initialize the data from a signature result
  * -------------------------------------------------- */
 
 void ecdsa_init_sign(ecdsa_sign_t *sign);
 
 /* -------------------------------------------------- *
     Destroy the data from a signature result
  * -------------------------------------------------- */
 
 void ecdsa_destroy_sign(ecdsa_sign_t *sign);

/* -------------------------------------------------- *
   Sign a buffer of dlen bytes and return the
   signature in 'sign'
 * -------------------------------------------------- */

int ecdsa_sign_data(ecc_key_t *key,int alg, const void *data, int dlen, ecdsa_sign_t *sign);

/* -------------------------------------------------- *
    Sign a buffer that contains a hash of hlen bytes
    and return the signature in 'sign'
 * -------------------------------------------------- */

int ecdsa_sign_hash(ecc_key_t *key, const void *hash, int hlen, ecdsa_sign_t *sign);

/* -------------------------------------------------- *
    Verify if signature in 'sign' corresponds to the
    signature of 'data'
 * -------------------------------------------------- */

int ecdsa_verify_sign(ecc_key_t *key, int alg, const void *data, int dlen, ecdsa_sign_t *sign);

/* -------------------------------------------------- *
    Verify if signature in 'sign' corresponds to the
    signature of the hash value in 'hash'
 * -------------------------------------------------- */

int ecdsa_verify_hash(ecc_key_t *key,const void *hash,int hlen,ecdsa_sign_t *sign);

/* ----------------------------------------------------- *
    ECDH - Elliptic Curve Diffe-Hellman stuff    
 * ----------------------------------------------------- */

/* ----------------------------------------------------- *
   Creates an Elliptic Curve Diffie-Hellman key
   exchange context using a random number generator
   to generate the one-use keys.

   If the generator is NOT given, a default RAND_OS
   is used or RAND_TLS_SHA256 if not present.

  'rpub' is assumed to be at least ctx->NUMBYTES long
   and is only needed by the client side of the
   exchange to set the server key.

   Eg:
        dh = ecc_dh_start(ctx,NULL,rnd);
 * ----------------------------------------------------- */

ecc_dh_t *ecc_dh_start(ecc_curve_t *ctx,const void *rpub,size_t rsz,rand_t *rc);


/* -------------------------------------------------- *
    Same as previous but using a raw data buffer of
    'cnt' bytes, which cannot be smaller than
    ctx->NUMBYTES.
 * -------------------------------------------------- */

ecc_dh_t *ecc_dh_start_from_bytes(ecc_curve_t *ctx,const void *rpub,size_t rsz,const void *data,size_t cnt);

/* -------------------------------------------------- *
    Finishes an Elliptic Curve Diffie-Hellman key
    exchange generating the shared secret.

    'pub' is assumed to be at least ctx->NUMBYTES long
    and is only needed by the server side of the
    exchange to set the client key.

    'shared' is assumed to be at least ctx->NUMBYTES long
    and it receives the shared secret.

    Eg:

        ecc_dh_end(dh,NULL,0,shared);

    IMPORTANT: this frees 'dh'

 * -------------------------------------------------- */

void ecc_dh_end(ecc_dh_t *dh,const void *pub,size_t psz,void *shared);

/* -------------------------------------------------- *
    Read the local public key of an Elliptic Curve
    Diffie-Hellman key exchange context.

    Eg:

        ret = ecc_dh_get_public(dh,public,256,TRUE);
 * -------------------------------------------------- */

int ecc_dh_get_public(ecc_dh_t *dh,void *pub,size_t max,int compressed);


#ifdef __cplusplus
};
#endif


#endif
