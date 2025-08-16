/*
    rsa.h

    RSA support

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


                                --oO0Oo--

*/

#ifndef RSA_BY_GAO_AND_FROM_PUTTY
#define RSA_BY_GAO_AND_FROM_PUTTY

#include "primes.h"
#include "pem.h"

/* pre-defined exponents for rsa_generate_keys() */

#define RSA_EXP_PUTTY   37      /** PUTTY's exponent */
#define RSA_EXP_CERT    65537   /** X509 Certificates exponent  */

/* Limits */

#define RSA_MIN_BITS    64      /* This allows for experiments */
#define RSA_MAX_BITS    32768   /* This allows for big security */

#define RSA_MAX_BYTES   ((RSA_MAX_BITS+7) / 8)

/* RSA Context */

typedef struct
{
    unsigned int bits;
    unsigned int bytes;
    mp_int_t     modulus;
    mp_int_t     exponent;
    mp_int_t    *private_exponent;
    mp_int_t    *iqmp,*p,*q;
} rsa_t;

/* Useful macros */

#define rsa_bits(_r)         ((_r)->bits)
#define rsa_key_size(_r)     ((_r)->bytes)
#define rsa_block_size(_r)   ((_r)->bytes)
#define rsa_has_private(_r)  ((_r)->private_exponent)

/* -------------------------------------------------- *
   Section 8 (Encryption process) of RFC-2313
   (PKCS#1 v1.5) says that the data should never
   be bigger than key->bytes - 11 so there is always
   8 or more bytes of padding.
 * -------------------------------------------------- */

#define rsa_max_data(_r)    ((_r)->bytes - 11)

/* Digital Signature Algorithms based on RSA (use different OID) */

enum
{
    NPI_SIGN_ALG=0,         /* Non Probably Implemented SIGNature ALGgorithm */

    MD2_RSA_DIGEST,         /* md2Digest */
    MD4_RSA_DIGEST,         /* md4Digest */
    MD5_RSA_DIGEST,         /* md5Digest */

    SHA1_RSA_DIGEST,        /* sha1Digest */
    SHA224_RSA_DIGEST,      /* sha224Digest */
    SHA256_RSA_DIGEST,      /* sha256Digest */
    SHA384_RSA_DIGEST,      /* sha384Digest */
    SHA512_RSA_DIGEST,      /* sha512Digest */

    MD2_RSA_ENC,            /* md2WithRSAEncryption */
    MD4_RSA_ENC,            /* md4WithRSAEncryption */
    MD5_RSA_ENC,            /* md5WithRSAEncryption */

    SHA1_RSA_ENC,           /* sha1WithRSAEncryption */
    SHA224_RSA_ENC,         /* sha224WithRSAEncryption */
    SHA256_RSA_ENC,         /* sha256WithRSAEncryption */
    SHA384_RSA_ENC,         /* sha384WithRSAEncryption */
    SHA512_RSA_ENC,         /* sha512WithRSAEncryption */

    MD4_RSA,                /* md4WithRSA */
    MD5_RSA,                /* md5WithRSA */
    MD2_RSA_SIGN,           /* md2WithRSASignature */
    MD5_RSA_SIGN,           /* md5WithRSASignature */
    SHA1_RSA_SIGN,          /* sha1WithRSASignature */

    LAST_RSA_SIGN_ALG       /* LEAVE ALWAYS LAST */
};

/* Type of pading as per Section 8.1 from RFC-2313 */

#define RSA_PAD_ZEROES  0    /* Pad all bits with zero */
#define RSA_PAD_ONES    1    /* Pad all bits with  one */
#define RSA_PAD_RANDOM  2    /* Pad with random bits */


/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    Creates a RSA context with a new random private
    and public key pair.

    bits   = Number of bits of the key

    prime  = Small prime (usually 65537) to be used as
             the public exponent.  If you send 0 the 
             function will use a random small prime. 
             
             Like this number should not be less 
             than 37 (RSA_EXP_PUTTY), if you send 0
             it will get a random value, but the result
             will never be less than that.
            
             You can send a smaller value if you wish 
             to experiment and the function will use it.

             The function will return NULL is this number
             is not prime.

    rc     = Random Number Generator to be used.

    If 'rc' is NULL, the function uses OSSYS or
    TLS_PRF if the other is not available.
 * -------------------------------------------------- */

rsa_t *rsa_generate_keys(unsigned int bits,int prime,rand_t *rc);

/* -------------------------------------------------- *
    Destroy a RSA context
 * -------------------------------------------------- */

void rsa_destroy(rsa_t *key);

/* -------------------------------------------------- *
    Creates a RSA context from bytes.

    Note that 'priv' can be NULL in order to create
    a contxet with just the public key.

 * -------------------------------------------------- */

rsa_t *rsa_from_bytes(int bits,const void *modulus,int mlen,const void *exponent,int elen,const void *priv,int plen);

/* -------------------------------------------------- *
   Same as previous but from strings instead of
   bytes. These strings are hexadecimal strings
   without separators, examples:

    "010203040506070809aabbccddeeff"

    Any other char not hexa or separator will stop
    the parsing of the string an use whatever has
    been read up to that moment.
 * -------------------------------------------------- */

rsa_t *rsa_from_chars(const char *mod,const char *exp,const char *priv);

/* -------------------------------------------------- *
    Creates a RSA context from a buffer of bytes
    encoded using ASN.1/DER in one opf these formas:

        - PublicKeyInfo (PKCS-8)

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

        - PrivateKeyInfo (PKCS-8)

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

   The format of the fields PublicKey and PrivateKey in PKCS8
   depends on the type of keys. For RSA the one used is the
   PKCS-1 format defined in RFC-3447 which is still compatible
   with the PKCS-1 v1.5 format defined in RFC-2313, and both are
   also used interchangeably in X509 certificates:

        - RSAPublicKey (PKCS-1 since 1.0)

            RSAPublicKey ::= SEQUENCE
            {
                modulus         INTEGER,  -- n
                publicExponent  INTEGER   -- e
            }

        - RSAPrivateKey (PKCS-1 v 2.0 compatible with 1.5)

            RSAPrivateKey ::= SEQUENCE
            {
                version           Version,
                modulus           INTEGER,  -- n
                publicExponent    INTEGER,  -- e
                privateExponent   INTEGER,  -- d
                prime1            INTEGER,  -- p
                prime2            INTEGER,  -- q
                exponent1         INTEGER,  -- d mod (p-1)
                exponent2         INTEGER,  -- d mod (q-1)
                coefficient       INTEGER,  -- (inverse of q) mod p

                otherPrimeInfos   OtherPrimeInfos OPTIONAL
            }

            Version ::= INTEGER { two-prime(0), multi(1) }

            -- version must be multi if otherPrimeInfos is present


            OtherPrimeInfos ::= SEQUENCE SIZE(1..MAX) OF OtherPrimeInfo

            OtherPrimeInfo ::= SEQUENCE {
                prime             INTEGER,  -- ri
                exponent          INTEGER,  -- di
                coefficient       INTEGER   -- ti
            }

   We do not yet handle the multi-prime mode. :-)

 * -------------------------------------------------- */

rsa_t *rsa_from_asn1(const void *bytes,int tam);

/* -------------------------------------------------- *
   Like the previous this function creates a RSA
   context from a sequence in ASN.1/DER but with
   the difference that sequence should not have the
   sequence header, onlt the body.
* -------------------------------------------------- */

rsa_t *rsa_from_seq_asn1(const void *bytes,int tam);

/* -------------------------------------------------- *
   Builds a private RSA key context from a buffer 
   encoded in ASN.1/DER with the private key format:

            RSAPrivateKey ::= SEQUENCE
            {
                version           Version,
                modulus           INTEGER,  -- n
                publicExponent    INTEGER,  -- e
                privateExponent   INTEGER,  -- d
                prime1            INTEGER,  -- p
                prime2            INTEGER,  -- q
                exponent1         INTEGER,  -- d mod (p-1)
                exponent2         INTEGER,  -- d mod (q-1)
                coefficient       INTEGER,  -- (inverse of q) mod p

                otherPrimeInfos   OtherPrimeInfos OPTIONAL
            }

            Version ::= INTEGER { two-prime(0), multi(1) }

        Basically 'seq' is RSAPrivateKey
*/

rsa_t *rsa_private_key_from_asn1(asn1_seq_t *seq);

/* -------------------------------------------------- *
   Builds a public RSA key context from 2 buffers 
   encoded in ASN.1/DER with the public key format

        - PublicKeyInfo (PKCS-8 public key)

            PublicKeyInfo ::= SEQUENCE
            {
                algorithm       AlgorithmIdentifier,
                RSAPublicKey    BIT STRING
            }
            
            AlgorithmIdentifier ::= SEQUENCE
            {
                algorithm       OBJECT IDENTIFIER,
                parameters      NULL
            }

            RSAPublicKey (PKCS-1 since 1.0)

            RSAPublicKey ::= SEQUENCE
            {
                modulus         INTEGER,  -- n
                publicExponent  INTEGER   -- e
            }

        Basically 'seq' is PublicKeyInfo and 'pub' is
        AlgorithmIdentifier

*/

rsa_t *rsa_public_key_from_asn1(asn1_seq_t *seq,asn1_seq_t *pub);

/* -------------------------------------------------- *
   Converts the public key of a RSA contexto to a
   buffer of bytes encoded with ASN.1/DER.

   The format used is the PKCS-8's  PublicKeiInfo
   explained above which in the X509 specs it becomes
   SubjectPublicKeyInfo but they are basically the
   same. :-)

   Call asn1_free(); to get rid of the ASN.1/DER
   struct once you are done with it.
 * -------------------------------------------------- */

asn1_t *rsa_public_to_asn1(rsa_t *key);

/* -------------------------------------------------- *

   Same as the previous but converts the private key

   If 'pkcs8' is TRUE woud use the complete PKCS-8.
   If 'pkcs8' is FALSE would use PKCS-1 directly.

   Call asn1_free(); to get rid of the ASN.1/DER
   struct once you are done with it.
 * -------------------------------------------------- */

asn1_t *rsa_private_to_asn1(rsa_t *key,int pkcs8);

/* -------------------------------------------------- *
   Save a public key on a PEM file in format PKCS-8
   compatible with OpenSSL and other products
 * -------------------------------------------------- */

int rsa_public_to_pem(rsa_t *key,FILE *fp);

/* -------------------------------------------------- *
   Saves a private key in a PEM fiel in  PKCS-1
   format compatible with OpenSSL and others.

    If 'passcode' is not NULL, its encripted using
    the algorithm specified in 'alg', whci is ignoerd
    if passcode is NULL.

 * -------------------------------------------------- */

int rsa_private_to_pem(rsa_t *key,const char *passcode,int alg,FILE *fp);

/* -------------------------------------------------- *

    Load a RSA key from a PEM fiel called 'file'

    if 'public' is TRUE it will find a PKCS-8
    public key in the file. If its FALSE then it will
    search for the private key in PKCS-1 format.

    The PKCS-1 RSA private key can be encrypted using
    AES-256-CBC, AES-192-CBC, AES-128-CBC or DES-EDE3-CBC
    using 'passcode' as the encription key.

    Returns:

         0 - Loaded
        -1 - 'cer' is NULL.
        -3 - Cannot load the file (it may not exist).
        -4 - The file format of the key is incorrect.
        -5 - The encrytion algorithm used on the key is not supported
        -6 - The key is encrypted but 'passcode' was not provided (NULL)
             or the the key failed the verification. It might be corrupt.
        -7 - The private key doesn't belong to this certificate.

 * -------------------------------------------------- */

int rsa_load_pem(const char *file,int public,const char *passcode,rsa_t **rsa);
int rsa_load_pemw(const wchar_t *file,int public,const char *passcode,rsa_t **rsa);

/* -------------------------------------------------- *
    Saves a RSA key context to a PEM file.

    If 'priv' is TRUE/TRUE it saves as a private key
    format, encrypting it using 'passcode' if
    especified, alg is the cypher to use. If you send
    0 the cipher AES-128-CBC will be used as default

    If 'priv' is FALSE it will save it as a PKCS-8
    public key format
 * -------------------------------------------------- */

int rsa_save_pem(const char *file,rsa_t *key,int priv,const char *passcode,int alg);
int rsa_save_pemw(const wchar_t *file,rsa_t *key,int priv,const char *passcode,int alg);

/* -------------------------------------------------- *
    Verify the integrity of the RSA key 'key' making
    sure the private key is the correct for the
    public key. If the p and q primes are defined,
    it checks whatever the keys come from p and q
    and that p > q as well as iqmp is actually the
    inverse of q mod p
 * -------------------------------------------------- */

int rsa_verify_keys(rsa_t *key);

/* -------------------------------------------------- *
    Encrypt a buffer using the RSA context 'key'.

    'tam' is the size of data in 'buf' to encrypt,
    which has to be at least rsa_block_size()
    bytes long as the result will be also in there.

    Also, 'tam' should not be larger than
    rsa_max_data() or the function will return -3

    'pad' the padding to use eg. RSA_PAD_ZEROS.

    'rc' points to a random number generator of our
    choice to perform the padding if we choose to
    pad with RSA_PAD_RANDOM

    If 'public' is TRUE, ecode using the public key
    otherwise uses the private one.

    it returns the number of bytes encoded including
    padding, which is actually the same value
    returned by rsa_block_size() as the whole
    buffer is alwas encrypted.

    It can also return a negative number for errors:

        -1   when parameters are incorrect (eg you
                    ask for RSA_PAD_RANDOM and 'rc' is NULL
                    or the padding is none of the defined.        

        -2   Not enough memory to operate

        -3   if the buffer is too big (ie there are
             not at least 11 bytes for padding.

        -4    Encoding error

 * -------------------------------------------------- */

int rsa_encode(rsa_t *key,void *buf,unsigned int tam,int public,int pad,rand_t *c);

/* -------------------------------------------------- *
   Decode a buffer 'buf' of 'tam' bytes using the
   private key unless 'public' is TRUE, where it
   will use the public one.

   'pad' is the type of padding we are expecting

    it returns the number of bytes decoded back into
    'buf' or negative for errors:

        -1   when parameters are incorrect (eg you
            ask for RSA_PAD_RANDOM and the type in
            'buf' is RSA_PAD_ZEROS or another)

        -2  Not enough memory to operate

        -3  if the size of the buffer is not fit for
            decryption, ie 'tam' is not the same
            size of rsa_block_size().      

        -4    Decoding error
 * -------------------------------------------------- */

int rsa_decode(rsa_t *key,void *buf,unsigned int tam,int public,int tipo);

/* -------------------------- *
     RSA DIGITAL SIGNATURES
 * -------------------------- */

/* -------------------------------------------------- *
    This is to check if  'alg' is a valid RSA sign
    algorithm. If it is it will return 'alg' otherwise
    it will return NPI_SIGN_ALG
 * -------------------------------------------------- */

int rsa_sign_alg(int alg);

/* -------------------------------------------------- *
    Returns the signature algorithm from an OID
 * -------------------------------------------------- */

int rsa_sign_algorithm(const char *oid);

/* -------------------------------------------------- *
    Returns the signature algorithm from an OID but
    encoded in ASN.1/DER
 * -------------------------------------------------- */

int rsa_sign_algorithm_asn1(const void *oid,unsigned int tam);

/* -------------------------------------------------- *
    Returns the OID from signature algorithm
 * -------------------------------------------------- */

char *rsa_sign_oid(int alg);


/* -------------------------------------------------- *
    Returns the name of a signature algorithm
 * -------------------------------------------------- */

char    *rsa_sign_name(int alg);
wchar_t *rsa_sign_namew(int alg);

/* -------------------------------------------------- *
    Returns the signature algorithm  from its name
 * -------------------------------------------------- */

int rsa_sign_alg_from_name(const char *nombre);
int rsa_sign_alg_from_namew(const wchar_t *nombre);


/* -------------------------------------------------- *

    Copies the OID -in ASN.1/DER format- of a given
    signature algorithm.

    It returns the number of bytes written or 0 if
    doesn't recognize the given algorithm.

    'des' can be NULL if you only want to know the
    length of a given signature in ASN.1/DER

    OID Examples:

        MD5_RSA_DIGEST:

            06 08 2a 86 48 86 f7 0d 02 05
            ----- =======================

            -- OID (8 bytes long)
            == 1.2.840.113549.2.5

        SHA1_RSA_ALG:

            06 05 2B 0E 03 02 1A
            ----- ==============

            -- OID  (5 bytes long)
            == 1.3.14.3.2.26

 * -------------------------------------------------- */

int rsa_sign_oid_asn1(int alg,void *dest,unsigned int max);

/* -------------------------------------------------- *
    Returns the hash used by a given signature
    algorithm
 * -------------------------------------------------- */

int rsa_signing_hash(int alg);

/* ---------------------------------------------------- *

    Uses the private key in 'key' to calculate the
    signature of 'tam' bytes from 'datos' and copy it
    in 'dest', returning the  number of bytes of the
    signature.

    'alg' defines the hash algorithm to use to sign
    the data and thus what signature and OID will be
    used.

    Some examples:

      HASH_MD2    = MD2_RSA_DIGEST    (md2Digest)
      HASH_MD4    = MD4_RSA_DIGEST    (md4Digest)
      HASH_MD5    = MD5_RSA_DIGEST    (md5Digest)
      HASH_SHA1   = SHA1_RSA_DIGEST   (sha1Digest)
      HASH_SHA224 = SHA224_RSA_DIGEST (sha224Digest)
      HASH_SHA256 = SHA256_RSA_DIGEST (sha256Digest)
      HASH_SHA384 = SHA384_RSA_DIGEST (sha384Digest)
      HASH_SHA512 = SHA512_RSA_DIGEST (sha512Digest)

    MD5_RSA_DIGEST and SHA1_RSA_ALG used to be the
    more common algorithms in RSA signatures, but
    these days is more common to see SHA256_RSA_DIGEST
    or stronger ones.

 * ---------------------------------------------------- */

int rsa_sign(rsa_t *key,void *dest,unsigned int max,int alg,const void *datos,unsigned int tam);

/* -------------------------------------------------- *

   Uses the public key 'key' to extract the hash of
   the signature 'orig' of 'tam' bytes long and copy
   it in 'hash' which is of 'max' bytes.

   'orig' must be in ASN.1/DER format.

    It returns the algorithm used to create 'hash':

        HASH_MD4, HASH_MD5, HASH_SHA1,
        HASH_SHA256, HASH_SHA384, ...

    If it it cannot decrypt or recognize the hash it
    will return an error (check defs.h for them).

 * -------------------------------------------------- */

int rsa_read_sign(rsa_t *key,void *hash,unsigned int max,const void *orig,unsigned int tam);

/* -------------------------------------------------- *

    This is the same as rsa_sign() but instead
    signing a data buffer, it actually calculates
    the hash of a file and uses the private key in
    'key' to create a RSA signature  of the file
    and copy it in 'dest'.

    Examples of 'alg':

      HASH_MD2    = MD2_RSA_DIGEST    (md2Digest)
      HASH_MD4    = MD4_RSA_DIGEST    (md4Digest)
      HASH_MD5    = MD5_RSA_DIGEST    (md5Digest)
      HASH_SHA1   = SHA1_RSA_DIGEST   (sha1Digest)
      HASH_SHA224 = SHA224_RSA_DIGEST (sha224Digest)
      HASH_SHA256 = SHA256_RSA_DIGEST (sha256Digest)
      HASH_SHA384 = SHA384_RSA_DIGEST (sha384Digest)
      HASH_SHA512 = SHA512_RSA_DIGEST (sha512Digest)

 * -------------------------------------------------- */

int rsa_calc_file_digest(rsa_t *key,void *dest,unsigned int max,int alg,const wchar_t *fich);

 /* -------------------------------------------------- *

    Like the previous is a file counterpart of the
    function rsa_sign(), this one is a kind of
    the same for rsa_read_sign(), but instead of
    returning the hash, it calculates that the hash
    of the file 'fich' an check if it matches the
    one in the signature 'orig' of tam bytes.

    It returns 0 if the hash of the file can be
    calculated and it matches the hash in the signature
    (i.e. the signatures is that of the file) or an
    error like ENOMATCH if the hash is not correct
    or others if an error arise, like file not found.
 * -------------------------------------------------- */

int rsa_check_file_digest(rsa_t *key,const void *orig,unsigned int tam,int alg,const wchar_t *fich);



#ifdef __cplusplus
};
#endif


#endif
