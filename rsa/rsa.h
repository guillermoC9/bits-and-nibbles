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

/* pre-defined exponents for rsa_generate_keys() */

#define RSA_EXP_PUTTY   37      /* PUTTY's exponent */
#define RSA_EXP_CERT    65537   /* X509 Certificates exponent  */

/* Limits */

#define RSA_MIN_BITS    64      /* This allows for experiments */
#define RSA_MAX_BITS    32768   /* This allows for security */

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

/* Type of pading as per Section 8.1 from RFC-2313 */

#define RSA_PAD_ZEROES  0    /* Pad all bits with zero */
#define RSA_PAD_ONES    1    /* Pad all bits with  one */
#define RSA_PAD_RANDOM  2    /* Pad with random bits */

/* -------------------------------------------------- *
    Functions 
 * -------------------------------------------------- */

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

    If 'rc' is NULL, the function uses RAND_OS or
    RAND_TLS_SHA256 if the other is not available.
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

    Any other char not hexa or separator, will stop
    the parsing of the string and use whatever has
    been read up to that moment.
 * -------------------------------------------------- */

rsa_t *rsa_from_chars(const char *mod,const char *exp,const char *priv);

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
                
        -4   Encoding error

 * -------------------------------------------------- */

int rsa_encode(rsa_t *key,void *buf,unsigned int tam,int public,int pad,rand_t *c);

/* -------------------------------------------------- *
   Decode a buffer 'buf' of 'tam' bytes using the
   private key unless 'public' is TRUE, where it
   will use the public one.

   'pad' is the type of padding we are expecting

   it returns the number of bytes decoded back into
   'buf' or negative for errors:

        -1  when parameters are incorrect (eg you
            ask for RSA_PAD_RANDOM and the type in
            'buf' is RSA_PAD_ZEROS or another)
        
        -2  Not enough memory to operate

        -3  if the size of the buffer is not fit for
            decryption, ie 'tam' is not the same
            size of rsa_block_size()

        -4  Decoding error
 * -------------------------------------------------- */

int rsa_decode(rsa_t *key,void *buf,unsigned int tam,int public,int pad);

/* -------------------------- *
     RSA DIGITAL SIGNATURES
 * -------------------------- */

/* ---------------------------------------------------- *
    Uses the private key in 'key' to calculate the
    signature of 'tam' bytes from 'datos' and copy it
    in 'dest', returning the  number of bytes of the
    signature.

    'alg' defines the hash algorithm to use to sign
    the data 

    Some examples:

      HASH_MD5    
      HASH_SHA1   
      HASH_SHA224 
      HASH_SHA256 
      HASH_SHA384 
      HASH_SHA512 

    HASH_MD5 and HAS_SHA1 used to be the more common 
    algorithms in RSA signatures, but hese days is more 
    common to see HASH_SHA256 or stronger ones.

    it returns the number of bytes encoded into
    'dest' or negative for errors:

        -1  when parameters are incorrect (eg the
            key or dest are NULL or the private
            key i snot present)
        
        -2  Not enough memory to operate

        -3  if the size of the buffer is not fit for
            encryption.

        -4  The hash cannot be recognized

 * ---------------------------------------------------- */

int rsa_sign(rsa_t *key,void *dest,unsigned int max,int alg,const void *datos,unsigned int tam);

/* -------------------------------------------------- *
   Uses the public key 'key' to extract the hash of
   the signature 'orig' of 'tam' bytes long and copy
   it in 'hash' which is of 'max' bytes.

    it returns the size of the hash signature in
    'hash' or negative for errors:

        -1  when parameters are incorrect (eg the
            key or hash are NULL or the public 
            key i snot present)
        
        -2  Not enough memory to operate

        -3  if the size of the buffer is not fit for
            encryption.

        -4  The hash cannot be recognized
 * -------------------------------------------------- */

int rsa_read_sign(rsa_t *key,void *hash,unsigned int max,const void *orig,unsigned int tam);

 /* -------------------------------------------------- *
    This function  calculates the hash of the 
    the data 'buf' of 'len' bytes matches the one in 
    the signature 'orig' of 'tam' bytes.

    It returns 0 if the calculated hash matches the 
    hash in the  signature 'sign' of 'tam' bytes.

    Otherwise returns -1 in bad parameters, -2 if not memory 
    can be allocated, and -3 if it cannot calculate the
    hash.
 * -------------------------------------------------- */

int rsa_check_sign(rsa_t *key,const void *sign,unsigned int tam,int alg,const void *buf,unsigned int len);

/* -------------------------------------------------- *

    This is the same as rsa_sign() but instead
    signing a data buffer, it actually calculates
    the hash of a file and uses the private key in
    'key' to create a RSA signature  of the file
    and copy it in 'dest'.

    it returns the size of the hash signature in
    'dest' or negative for errors:

        -1  when parameters are incorrect (eg the
            key or hash are NULL or the private 
            key is not present)
        
        -2  Not enough memory to operate

        -3  if the size of the buffer is not fit for
            encryption.

        -4  The hash cannot be recognized

 * -------------------------------------------------- */

int rsa_sign_file(rsa_t *key,void *dest,unsigned int max,int alg,const char *fich);
int rsa_sign_filew(rsa_t *key,void *dest,unsigned int max,int alg,const wchar_t *fich);

 /* -------------------------------------------------- *
    Just like the previous is a file counterpart of 
    rsa_sign(), this one is kinda of the same as
    rsa_check_sign(). It  calculates the hash of the 
    file 'fich' and check if it matches the one in 
    the signature 'orig' of 'tam' bytes.

    It returns 0 if the hash of the file can be
    calculated, and that it matches the hash in the 
    signature 'orig'.

    Otherwise returns -1 in bad parameters, -2 if not memory 
    can be allocated, and -3 if it cannot calculate the
    file hash (e.g. file does not exist of cannot be 
    opened for reading).
 * -------------------------------------------------- */

int rsa_check_file_sign(rsa_t *key,const void *orig,unsigned int tam,int alg,const char *fich);
int rsa_check_file_signw(rsa_t *key,const void *orig,unsigned int tam,int alg,const wchar_t *fich);


#ifdef __cplusplus
};
#endif


#endif
