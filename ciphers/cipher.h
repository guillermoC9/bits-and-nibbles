/*
    cipher.h

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

#ifndef SYMMETRIC_CRYPTOGRAPHY
#define SYMMETRIC_CRYPTOGRAPHY

#include "aes.h"
#include "des.h"
#include "rc4.h"
#include "xtea.h"
#include "salsa20.h"
#include "chacha.h"
#include "aria.h"
#include "camellia.h"
#include "blowfish.h"
#include "twofish.h"
#include "poly1305.h"

/* Supported encription algorithms */

enum
{
    NULL_CIPHER = -1,

    AES_128_ECB, AES_192_ECB, AES_256_ECB, 
    AES_128_CBC, AES_192_CBC, AES_256_CBC, 
    AES_128_CTR, AES_192_CTR, AES_256_CTR, 
    AES_128_GCM, AES_192_GCM, AES_256_GCM,

    ARIA_128_ECB, ARIA_192_ECB, ARIA_256_ECB,
    ARIA_128_CBC, ARIA_192_CBC, ARIA_256_CBC,
    ARIA_128_CTR, ARIA_192_CTR, ARIA_256_CTR,
    ARIA_128_GCM, ARIA_192_GCM, ARIA_256_GCM,

    BLOWFISH_128_ECB, BLOWFISH_128_CBC,

    CAMELLIA_128_ECB, CAMELLIA_192_ECB, CAMELLIA_256_ECB,
    CAMELLIA_128_CBC, CAMELLIA_192_CBC, CAMELLIA_256_CBC,
    CAMELLIA_128_CTR, CAMELLIA_192_CTR, CAMELLIA_256_CTR,
    CAMELLIA_128_GCM, CAMELLIA_192_GCM, CAMELLIA_256_GCM,

    CHACHA8_128,  CHACHA8_256, 
    CHACHA12_128, CHACHA12_256,
    CHACHA20_128, CHACHA20_256,

    CHACHA20_IETF, CHACHA20_POLY_1305,

    DES_ECB, DES_CBC, DES_EDE3_ECB, DES_EDE3_CBC,

    RC4_64, RC4_128,

    SALSA20_128, SALSA20_256,
   
    TWOFISH_128_ECB, TWOFISH_192_ECB, TWOFISH_256_ECB,
    TWOFISH_128_CBC, TWOFISH_192_CBC, TWOFISH_256_CBC,
    TWOFISH_128_CTR, TWOFISH_192_CTR, TWOFISH_256_CTR,
    TWOFISH_128_GCM, TWOFISH_192_GCM, TWOFISH_256_GCM,

    XTEA_128_ECB, XTEA_128_CBC, 

    CIPHER_COUNT  /* Always last, add new ones above this */

};

/** Cipher modes */

enum
{
    CIPHER_MODE_NONE=0,
    CIPHER_MODE_ECB,         /* Electronic Code Book */
    CIPHER_MODE_CBC,         /* Cipher Block Chaining */
    CIPHER_MODE_CTR,         /* Cipher Counter  */
    CIPHER_MODE_AEAD,        /* Authenticated Encription with Associated Data */
    CIPHER_MODE_STREAM,      /* Stream */

    CIPHER_MODE_COUNT        /* Always last, add new ones above this */
};

/*  ------------------------------------------------------------------ *
    Types of Padding:

    Section 6.2.3.2 of RFC-4346 (TLS v 1.1 specs) or says this about
    padding: 

    Padding that is added to force the length of the plaintext to be
    an integral multiple of the block cipher's block length.  The
    padding MAY be any length up to 255 bytes, as long as it results
    in the TLSCiphertext.length being an integral multiple of the
    block length.  Lengths longer than necessary might be desirable to
    frustrate attacks on a protocol that are based on analysis of the
    lengths of exchanged messages.  Each uint8 in the padding data
    vector MUST be filled with the padding length value.  The receiver
    MUST check this padding and SHOULD use the bad_record_mac alert to
    indicate padding errors.

    The padding length MUST be such that the total size of the
    GenericBlockCipher structure is a multiple of the cipher's block
    length.  Legal values range from zero to 255, inclusive.  This
    length specifies the length of the padding field exclusive of the
    padding_length field itself.

    Section 6.1.1 of RFC-2898 describes a similar scheme but using 8 
    as the integral multiple, but it makes sense because its the block
    size of the cipher used. I guess the former specs took the technique
    from the later.

    We will embrace the padding technique of both RFC but with few 
    differences so different techniques can be studied and used if
    chosen to.    
 *  ------------------------------------------------------------------ */

enum
{
    PAD_NONE=0,      /* No padding (eg for MODE_AEAD and MODE_STREAM) */    
    PAD_SIZE,        /* All pad bytes are the pad size value (which is a multiple of the block size) */        
    PAD_ZEROS,       /* Values are all 0x00 except the first (NOT LAST) of the padding, which is the size  */
    PAD_ONES,        /* Like PAD_ZEROS but pad values are not 0x00 but 0xFF (which is not allowed as pad count) */
    
    PAD_COUNT        /* Number of padding schemes, add new ones above this */    
};

/* Various Limits */

#define CIPHER_MAX_KEY_SIZE    32                        /* Biggest key size we handle ath the moment (256 bits) */
#define CIPHER_MAX_BLOCK_SIZE  CHACHA_BLOCK_SIZE         /* Biggest block size we handle (64 bytes  - 512 bits)  */
#define CIPHER_MAX_PAD_SIZE    CIPHER_MAX_BLOCK_SIZE     /* Biggest pad size (PAD_SIZE) */
#define CIPHER_MAX_IV_SIZE     CIPHER_MAX_BLOCK_SIZE     /* Biggest IV (usually same size of cipher's block size) */
#define CIPHER_AEAD_IV_SIZE    12                        /* Size of the nonce/Position of the counter in GCM mode */ 
#define CIPHER_CTR_IV_SIZE     12                        /* Size of the CTR mode IV size (without the 4 bytes of the counter) */ 


/** Function pointers for ciphers */

typedef void (*block_cipher_t)(void *ctx,void *dest,const void *src);
typedef void (*stream_cipher_t)(void *ctx,void *dest,const void *src,size_t cnt);
typedef void (*shuffle_cipher_t)(void *ctx,const void *iv,int cnt);

/* ------------------------------------------------ *
        Encryption context for all algorithms
 * ------------------------------------------------ */

typedef struct
{
    int     alg;          /* Cipher Algorithm */
    int     kbits;        /* Cipher key bits */    
    int     mode;         /* Mode of cipher */
    int     pad;          /* Type of padding */
    int     iv_sz;        /* IV size */    
    int     iv_len;       /* IV len for CBC */
    int     blk_sz;       /* Block size */

    int      is_stream;   /* TRUE/FALSE cipher is in stream mode */

    char      *namec;     /* Name of cipher (char) */
    wchar_t   *namew;     /* Name of cipher (wchar_t) */

    block_cipher_t   encode_block;
    block_cipher_t   decode_block;

    stream_cipher_t  encode_stream;
    stream_cipher_t  decode_stream;

    unsigned char  iv[CIPHER_MAX_IV_SIZE];

     /** GCM mode parameters */

    unsigned char gcm_h[CIPHER_MAX_BLOCK_SIZE];
    int           gcm_blk;   /* GCM size */

    /*
        tag is filled by cipher_encode() and cipher_decode().
        You can check it by using cipher_correct_tag().
    */

    unsigned char dat_tag[CIPHER_MAX_BLOCK_SIZE];    /* Tag */
    size_t        tam_tag;                              /* Tag size (less or equal to CIPHER_MAX_BLOCK_SIZE) */

    /*
        These must be defined by calling cipher_set_aead_param() before
        calling the functions to encode/decode when in AEAD mode
    */

    const void *dat_aead;   /* Authentication Data */
    size_t      tam_aead;   /* Size of dat_aead */

    const void *dat_iv;     /* Initialization Vector */
    size_t      tam_iv;     /* Size of dat_iv */

    /** CIPHER_MODE_CTR parameters **/
    
    unsigned int   ctr_count;   /* Counter  */  
    int            ctr_iv;      /* IV size wihout counter */

    /* Internal cipher's context */

    union
    {
        aes_t          aes;    /* American Encrption Standard */
        des_t          des;    /* Data Encription Standard */
        rc4_t          rc4;    /* RC4 */
        xtea_t         tea;    /* XTEA */
        blowf_t        blf;    /* BlowFish */
        salsa20_t      sal;    /* Salsa */
        chacha_t       cha;    /* Chacha */
        camellia_t     cam;    /* Camellia */
        aria_t         aria;   /* Aria */
        twof_t         two;    /* Twofish */
        unsigned long  gen;    /* Generic */
    } a;

} cipher_t;


/* ------------------------------------------------ *
   Generic Functions
 * ------------------------------------------------ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------ *
   Given a cipher name  returns algorithm id
 * ------------------------------------------------ */

int cipher_alg_from_name(const char *name);
int cipher_alg_from_namew(const wchar_t *name);

/* -------------------------------------------------- *
   Given a Cipher algorithm id returns the name
 * -------------------------------------------------- */

char    *cipher_alg_name(int cual);
wchar_t *cipher_alg_namew(int cual);

/* ------------------------------------------------ *
    Return the block size of a cipher algortihm
 * ------------------------------------------------ */

int cipher_alg_block_size(int alg);

/* ------------------------------------------------ *
    Return the iv size of a cipher algortihm
 * ------------------------------------------------ */

int cipher_alg_iv_size(int alg);

/* ------------------------------------------------ *
   Return info on the encryption context
 * ------------------------------------------------ */

#define cipher_alg(_ctx)             ((_ctx)->alg)
#define cipher_name(_ctx)            ((_ctx)->namec)
#define cipher_namew(_ctx)           ((_ctx)->namew)
#define cipher_mode(_ctx)            ((_ctx)->mode)
#define cipher_is_stream(_ctx)       ((_ctx)->is_stream)
#define cipher_get_padding(_ctx)     ((_ctx)->pad)
#define cipher_block_size(_ctx)      ((_ctx)->blk_sz)
#define cipher_iv_size(_ctx)         ((_ctx)->iv_sz)
#define cipher_bits(_ctx)            ((_ctx)->kbits)

/* ------------------------------------------------ *

    int cipher_init(cipher_t *ctx,int alg,const void *key,const void *iv,unsigned int counter);

    Creates and encryption context for a given algorithm and mode.

        ctx     = Encryption context
        alg     = algorithm/mode to initialize for
        key     = key material (size dependes on chosen algoritm)
        iv      = initialization vector for those who use it (eg AES_128_CBC)
                  again size depends on chosen algorithm.
        counter = For those who use it (e.g. CHACHA20_IETF)

        IMPORTANT:  'counter' -as its name implies- is a counter to init
                    the ciphers that use counters. IT IS NOT the size of 
                    'key' or 'iv'. 
                    The size of these two depend on the cipher used, so it 
                    is assumed that the programmer will point 'key' and 'iv'
                    to buffers with the appropiate amount of data.

                    The algorithms which use 'iv' will set it as ALL ZEROS
                    if iv is sent as NULL 

    Return CIPHER_OK (0) or error (negative)

 * ------------------------------------------------ */

int cipher_init(cipher_t *ctx,int alg,const void *key,const void *iv,unsigned int counter);

/* ------------------------------------------------ *
   Clean up a crypto context
 * ------------------------------------------------ */

void cipher_end(cipher_t *ctx);

/* ---------------------------------------------------- *
             *** WARNING - BE CAREFUL ***

    Usually, block encryption algorithms specs say 
    nothing about padding, so these routines do not 
    pad data unless you set a specific padding scheme
    through cipher_set_padding(). If you don't, it  
    means padding is PAD_NONE and spare data that is
    not a multiple of the cipher block size would be
    IGNORED, ie. the remaining bytes in the last block
    will not be ciphered (stream ciphers do not suffer
    this problem).

    This is why the functions cipher_encode() and 
    cipher_decode() will return the number of bytes 
    PROCESSED, so if the return value is bigger than
    'tam', you know the mode underneath padded the data
    to be a multiple of the cipher's blocksize. FOR THIS
    TO WORK properly, 'dest' should point to a buffer 
    which is a multiple of the block size, usually by 
    adding the number of bytes returned by the function 
    cipher_pad_needed(), which returns the number 
    of bytes needed to make a buffer a multiple of 
    the cipher's block size.

    One easy way to solve this problem is to always 
    allocate one extra  block and then use one of the 
    built-in padding schemes. Which is what the PKCS 
    code in pem.c does.

    So if the cipher is in block mode, cipher_encode()
    assumes that 'dest' has enough space for the padding
    scheme, i.e. up to one extra block. Note that, even
    if the buffer pointed to by 'dest' has enough for 
    the padded data, 'tam' should be the amount of bytes 
    to encode, not the size of the buffer.

    cipher_decode() will remove padding if the cipher 
    is in block mode and a padding scheme has been 
    selected beforehand.

    As said, they return the size of the data processed 
    by the functions or (in the case of cipher_decode(), 
    it can also return (size_t) -1  if the padding is 
    incorrect for the  selected padding scheme. It does 
    always checks the padding as this helps to detect 
    decryption problems (e.g. wrong key was sent).

    Another way to solve the padding problem is to handle
    the padding externally and give cipher_encode() an
    already padded buffer, multiple of the block size, which 
    is what the TLS code in tls_crypto.c does.

 * ------------------------------------------------ */

size_t cipher_encode(cipher_t *ctx,void *dest,const void *orig,size_t tam);
size_t cipher_decode(cipher_t *ctx,void *dest,const void *orig,size_t tam);

 /* ------------------------------------------------ *
   It returns the number of bytes that needs to be
   added to make 'size' a multiple of the algorithm
   block size.

    If the cipher is not a block cipher (ECB or CBC)
    it returns 0.

    Otherwise it will return a number between 1 and
    the blocksize, as in block mode padding is always
    needed to enhance security.
 * ------------------------------------------------ */

size_t cipher_pad_needed(cipher_t *ctx,size_t tam);

/* ------------------------------------------------ *
   Set the padding scheme to use in modes
   MODE_ECB and MODE_CBC, in any other mode
   the call is ignored.   
 * ------------------------------------------------ */

int cipher_set_padding(cipher_t *ctx,int pad_mode);

/* ------------------------------------------------ *
    Set the aead parameters to be used by the
    AEAD modes:

        aead    - AED data buffer
        asz     - Size of data in aead
        iv      - AEAD IV buffer
        ivsz    - Size of data in iv
        tagsz   - Size iof GCM tag

    return 0
 * ------------------------------------------------ */

int cipher_set_aead_params(cipher_t *ctx,const void *aead,size_t asz,const void *iv,size_t ivsz,int tagsz);

/* ------------------------------------------------ *
   Copies the AEAD tag in 'tag'. Returns 0 for OK
   -1 ctx or tag are NULL and -2 if 'max' is 
   too small.
 * ------------------------------------------------ */

int cipher_get_aead_tag(cipher_t *ctx,void *tag,int max);

/* ------------------------------------------------ *
   Returns TRUE/FALSE the tags are teh same
 * ------------------------------------------------ */

int cipher_check_aead_tag(cipher_t *ctx,void *tag,int tam);

#ifdef __cplusplus
}
#endif

#endif
