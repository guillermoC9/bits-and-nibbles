/*
    test_ciphers.c

    Test of cipher routines

    (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Written by Guillermo Amodeo Ojeda.                  
            
                        --oO0Oo--
*/

#include "cipher.h"

/* -------------------------------------- */

void print_bytes(char *prefix, void *d, size_t n)
{
    size_t t;
    unsigned char *dat;

    dat = (unsigned char *)d;

    if (prefix)
        printf("%s=", prefix);

    if (d)
    {
        for (t = 0; t < n; t++)
            printf("%02x", dat[t]);
        printf("\n");
    }
    else
    {
        printf("NULL\n");
    }
}

/* -------------------------------------- */
    
void test_des(void)
{   
    cipher_t des1, des2;
    unsigned char k3[24] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67
    };
    
    unsigned char cipher[8], plain[8] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xE7
    };
    

    printf("**** Test Cipher Engine with 3DES ****\n\n");
    
    printf("EBC Mode Key=192 bits IV=NULL\n\n");
    
    cipher_init(&des1, DES_EDE3_ECB, k3,NULL,0);
    cipher_init(&des2, DES_EDE3_ECB, k3,NULL,0);

    cipher_encode(&des1, cipher, plain, 8);
    print_bytes("Text    ", plain, DES_BLOCK_SIZE);
    printf("Expected=de0b7c06ae5e0ed5\n");
    print_bytes("Cyphered", cipher, DES_BLOCK_SIZE);
    cipher_decode(&des2, cipher, cipher, DES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, DES_BLOCK_SIZE);
    printf("\n");

    printf("CBC Mode Key=192 bits IV=01234567890ABCDEF\n\n");

    cipher_init(&des1, DES_EDE3_CBC, k3, "01234567890ABCDEF",0);
    cipher_init(&des2, DES_EDE3_CBC, k3, "01234567890ABCDEF",0);
    printf("\nNames = %s - %s\n\n",des1.namec,des2.namec);
    cipher_encode(&des1, cipher, plain, 8);
    print_bytes("Text    ", plain, DES_BLOCK_SIZE);
    printf("Expected=af8878a4e954559e\n");
    print_bytes("Cyphered", cipher, DES_BLOCK_SIZE);
    cipher_decode(&des2, cipher, cipher, DES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, DES_BLOCK_SIZE);
    printf("\n");

    cipher_encode(&des1, cipher, plain, 8);
    print_bytes("Text    ", plain, DES_BLOCK_SIZE);
    printf("Expected=db14f3aed8c6784b\n");
    print_bytes("Cyphered", cipher, DES_BLOCK_SIZE);
    cipher_decode(&des2, cipher, cipher, DES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, DES_BLOCK_SIZE);
    printf("\n");

    cipher_encode(&des1, cipher, plain, 8);
    print_bytes("Text    ", plain, DES_BLOCK_SIZE);
    printf("Expected=2926ceff2dcefba6\n");
    print_bytes("Cyphered", cipher, DES_BLOCK_SIZE);
    cipher_decode(&des2, cipher, cipher, DES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, DES_BLOCK_SIZE);
    printf("\n");

    cipher_end(&des1);
    cipher_end(&des2);

}

/* -------------------------------------- */
/* -------------------------------------- */

unsigned char bf_key1[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char bf_pla1[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char bf_cip1[8] = {0x4E,0xF9,0x97,0x45,0x61,0x98,0xDD,0x78};

unsigned char bf_key2[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char bf_pla2[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char bf_cip2[8] = {0x51,0x86,0x6F,0xD5,0xB8,0x5E,0xCB,0x8A};

unsigned char bf_key3[8] = {0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char bf_pla3[8] = {0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
unsigned char bf_cip3[8] = {0x7D,0x85,0x6F,0x9A,0x61,0x30,0x63,0xF2};

unsigned char bf_key4[8] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
unsigned char bf_pla4[8] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
unsigned char bf_cip4[8] = {0x24,0x66,0xDD,0x87,0x8B,0x96,0x3C,0x9D};

unsigned char bf_key5[8] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF};
unsigned char bf_pla5[8] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
unsigned char bf_cip5[8] = {0x61,0xF9,0xC3,0x80,0x22,0x81,0xB0,0x96};


unsigned char bf_key_cbc[16] = {0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xF0,0xE1,0xD2,0xC3,0xB4,0xA5,0x96,0x87};
unsigned char bf_iv_cbc[8]   = {0xFE,0xDC,0xBA,0x98,0x76,0x54,0x32,0x10};
unsigned char bf_pla_cbc[32] = {0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x20,0x4E,0x6F,0x77,0x20,0x69,0x73,0x20,0x74,0x68,0x65,0x20,0x74,0x69,0x6D,0x65,0x20,0x66,0x6F,0x72,0x20,0x00,0x00,0x00,0x00};
unsigned char bf_cip_cbc[32] = {0x6B,0x77,0xB4,0xD6,0x30,0x06,0xDE,0xE6,0x05,0xB1,0x56,0xE2,0x74,0x03,0x97,0x93,0x58,0xDE,0xB9,0xE7,0x15,0x46,0x16,0xD9,0x59,0xF1,0x65,0x2B,0xD5,0xFF,0x92,0xCC};

/* -------------------------------------- */

void test_blowfish(void)
{
    cipher_t bf1, bf2;
    unsigned char cipher[32];

    printf("**** Test Cipher Engine uisng BLOWFISH ****\n\n");

    printf("CBC Mode Key=128 bits IV=...\n\n");

    cipher_init(&bf1, BLOWFISH_128_CBC, bf_key_cbc, bf_iv_cbc,0);
    cipher_init(&bf2, BLOWFISH_128_CBC, bf_key_cbc, bf_iv_cbc,0);

    print_bytes("Text    ", bf_pla_cbc,32);
    printf("Stringfy= %s\n",(const char *)bf_pla_cbc);

    cipher_encode(&bf1, cipher, bf_pla_cbc, 32);
    print_bytes("Ciphered", cipher, 32);
    print_bytes("Expected", bf_cip_cbc, 32);

    cipher_decode(&bf2, cipher, cipher, 32);
    print_bytes("Text    ", cipher, 32);
    printf("Stringfy= %s\n",(const char *)cipher);
    printf("\n");

    cipher_encode(&bf1, cipher, bf_pla_cbc, 32);
    print_bytes("Ciphered", cipher, 32);

    cipher_decode(&bf2, cipher, cipher, 32);
    print_bytes("Text    ", cipher, 32);
    printf("Stringfy= %s\n",(const char *)cipher);
    printf("\n");


    cipher_encode(&bf1, cipher, bf_pla_cbc, 32);
    print_bytes("Ciphered", cipher, 32);

    cipher_decode(&bf2, cipher, cipher, 32);
    print_bytes("Text    ", cipher, 32);
    printf("Stringfy= %s\n",(const char *)cipher);
    printf("\n");

    cipher_end(&bf1);
    cipher_end(&bf2);
}

/* -------------------------------------------------------------------------------- *

    Test vectors can be found at the end
    of the following document:

    http://portfolio.goliathdesigns.com/wp-content/uploads/files/aes/fips-197.html

 * -------------------------------------------------------------------------------- */

void test_aes(void)
{
    cipher_t  aes1, aes2;
    unsigned char cipher[AES_BLOCK_SIZE];
    unsigned char k[32] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    unsigned char plain[AES_BLOCK_SIZE] =
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    printf("**** Test Cipher Engine with AES ****\n\n");


    printf("EBC Mode Key=128 bits IV=NULL\n\n");

    print_bytes("Text    ", plain, AES_BLOCK_SIZE);
    
    cipher_init(&aes1, AES_128_ECB, k,NULL,0);
    cipher_init(&aes2, AES_128_ECB, k,NULL,0);    
    
    cipher_encode(&aes1, cipher, plain, AES_BLOCK_SIZE);
    printf("Expected=69c4e0d86a7b0430d8cdb78070b4c55a\n");
    print_bytes("Ciphered", cipher, AES_BLOCK_SIZE);
    cipher_decode(&aes2, cipher, cipher, AES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, AES_BLOCK_SIZE);
    printf("\n");

    cipher_end(&aes1);
    cipher_end(&aes2);

    printf("EBC Mode Key=192 bits IV=NULL\n\n");

    cipher_init(&aes1, AES_192_ECB, k,NULL,0);
    cipher_init(&aes2, AES_192_ECB, k,NULL,0);    

    cipher_encode(&aes1, cipher, plain, AES_BLOCK_SIZE);
    printf("Expected=dda97ca4864cdfe06eaf70a0ec0d7191\n");
    print_bytes("Ciphered", cipher, AES_BLOCK_SIZE);
    cipher_decode(&aes2, cipher, cipher, AES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, AES_BLOCK_SIZE);
    printf("\n");

    cipher_end(&aes1);
    cipher_end(&aes2);

    printf("EBC Mode Key=256 bits IV=NULL\n\n");

    cipher_init(&aes1, AES_256_ECB, k,NULL,0);
    cipher_init(&aes2, AES_256_ECB, k,NULL,0);

    cipher_encode(&aes1, cipher, plain, AES_BLOCK_SIZE);
    printf("Expected=8ea2b7ca516745bfeafc49904b496089\n");
    print_bytes("Ciphered", cipher, AES_BLOCK_SIZE);
    cipher_decode(&aes2, cipher, cipher, AES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, AES_BLOCK_SIZE);
    printf("\n");

    cipher_end(&aes1);
    cipher_end(&aes2);

    printf("CBC Mode Key=256 bits IV=01234567890ABCDEF\n");

    cipher_init(&aes1, AES_256_CBC, k, "01234567890ABCDEF",0);
    cipher_init(&aes2, AES_256_CBC, k, "01234567890ABCDEF",0);

    cipher_encode(&aes1, cipher, plain, AES_BLOCK_SIZE);
    printf("Expected=767465408bc57ec25ab6d215e7363acf\n");
    print_bytes("Ciphered", cipher, AES_BLOCK_SIZE);
    cipher_decode(&aes2, cipher, cipher, AES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, AES_BLOCK_SIZE);
    printf("\n");

    cipher_encode(&aes1, cipher, plain, AES_BLOCK_SIZE);
    printf("Expected=3b0b6f6b342037bb7ca71619dc1a995d\n");
    print_bytes("Ciphered", cipher, AES_BLOCK_SIZE);
    cipher_decode(&aes2, cipher, cipher, AES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, AES_BLOCK_SIZE);
    printf("\n");

    cipher_encode(&aes1, cipher, plain, AES_BLOCK_SIZE);
    printf("Expected=89409042a3d74cefdaa8f663aa73fa96\n");
    print_bytes("Ciphered", cipher, AES_BLOCK_SIZE);
    cipher_decode(&aes2, cipher, cipher, AES_BLOCK_SIZE);
    print_bytes("Text    ", cipher, AES_BLOCK_SIZE);

    printf("\n");

    cipher_end(&aes1);
    cipher_end(&aes2);
}


/* -------------------------------------------------------------------------------- *

    Test vectors can be found at section 6 of RFC-3686 at:

    https://datatracker.ietf.org/doc/html/rfc3686#section-6

 * -------------------------------------------------------------------------------- */

void test_aes_ctr0(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67, 0xCC,
        0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E
    };

    unsigned char iv[12] =
    {
        0x00, 0x00, 0x00, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    unsigned char a[16] =
    {
       0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
       0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    };
    unsigned char p[16] =
    {
        0xE4, 0x09, 0x5D, 0x4F, 0xB7, 0xA7, 0xB3, 0x79,
        0x2D, 0x61, 0x75, 0xA3, 0x26, 0x13, 0x11, 0xB8
    };


    cipher_init(&aes1, AES_128_CTR, k,iv,1);
    cipher_init(&aes2, AES_128_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 16);
    cipher_encode(&aes1, final, a, 16);
    print_bytes("Ciphered  ", final, 16);
    print_bytes("It must be", p, 16);
    cipher_decode(&aes2,test,final, 16);
    print_bytes("Deciphered", test, 16);

    if (!memcmp(p,final,16))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,16))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr1(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[32], test[32];
    unsigned char k[16] =
    {
        0x7E, 0x24, 0x06, 0x78, 0x17, 0xFA, 0xE0, 0xD7,
        0x43, 0xD6, 0xCE, 0x1F, 0x32, 0x53, 0x91, 0x63
    };

    unsigned char iv[12] =
    {
        0x00, 0x6c, 0xb6, 0xdb,
        0xc0, 0x54, 0x3b, 0x59, 0xda, 0x48, 0xd9, 0x0b
    };

    unsigned char a[32] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    unsigned char p[32] =
    {
        0x51, 0x04, 0xA1, 0x06, 0x16, 0x8A, 0x72, 0xD9,
        0x79, 0x0D, 0x41, 0xEE, 0x8E, 0xDA, 0xD3, 0x88,
        0xEB, 0x2E, 0x1E, 0xFC, 0x46, 0xDA, 0x57, 0xC8,
        0xFC, 0xE6, 0x30, 0xDF, 0x91, 0x41, 0xBE, 0x28
    };

    cipher_init(&aes1, AES_128_CTR, k,iv,1);
    cipher_init(&aes2, AES_128_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 32);
    cipher_encode(&aes1, final, a, 32);
    print_bytes("Ciphered  ", final, 32);
    print_bytes("It must be", p, 32);
    cipher_decode(&aes2,test,final, 32);
    print_bytes("Deciphered", test, 32);

    if (!memcmp(p,final,32))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,32))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr2(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[36], test[36];
    unsigned char k[16] =
    {
        0x76, 0x91, 0xbe, 0x03, 0x5e, 0x50, 0x20, 0xa8,
        0xac, 0x6e, 0x61, 0x85, 0x29, 0xf9, 0xa0, 0xdc
    };

    unsigned char iv[12] =
    {
        0x00, 0xe0, 0x01, 0x7b,
        0x27, 0x77, 0x7f, 0x3f, 0x4a, 0x17, 0x86, 0xf0
    };

    unsigned char a[36] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
       0x20, 0x21, 0x22, 0x23
    };

    unsigned char p[36] =
    {
        0xC1, 0xCF, 0x48, 0xA8, 0x9F, 0x2F, 0xFD, 0xD9,
        0xCF, 0x46, 0x52, 0xE9, 0xEF, 0xDB, 0x72, 0xD7,
        0x45, 0x40, 0xA4, 0x2B, 0xDE, 0x6D, 0x78, 0x36,
        0xD5, 0x9A, 0x5C, 0xEA, 0xAE, 0xF3, 0x10, 0x53,
        0x25, 0xB2, 0x07, 0x2F
    };

    cipher_init(&aes1, AES_128_CTR, k,iv,1);
    cipher_init(&aes2, AES_128_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 36);
    cipher_encode(&aes1, final, a, 36);
    print_bytes("Ciphered  ", final, 36);
    print_bytes("It must be", p, 36);
    cipher_decode(&aes2,test,final, 36);
    print_bytes("Deciphered", test, 36);

    if (!memcmp(p,final,36))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,36))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr3(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[16], test[16];
    unsigned char k[24] =
    {
        0x16, 0xAF, 0x5B, 0x14, 0x5F, 0xC9, 0xF5, 0x79,
        0xC1, 0x75, 0xF9, 0x3E, 0x3B, 0xFB, 0x0E, 0xED,
        0x86, 0x3D, 0x06, 0xCC, 0xFD, 0xB7, 0x85, 0x15
    };

    unsigned char iv[12] =
    {
        0x00, 0x00, 0x00, 0x48,
        0x36, 0x73, 0x3C, 0x14,
        0x7D, 0x6D, 0x93, 0xCB
    };

    unsigned char a[16] =
    {
       0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
       0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    };
    unsigned char p[16] =
    {
        0x4B, 0x55, 0x38, 0x4F, 0xE2, 0x59, 0xC9, 0xC8,
        0x4E, 0x79, 0x35, 0xA0, 0x03, 0xCB, 0xE9, 0x28
    };

    cipher_init(&aes1, AES_192_CTR, k,iv,1);
    cipher_init(&aes2, AES_192_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 16);
    cipher_encode(&aes1, final, a, 16);

    print_bytes("Ciphered  ", final, 16);
    print_bytes("It must be", p, 16);
    cipher_decode(&aes2,test,final, 16);
    print_bytes("Deciphered", test, 16);

    if (!memcmp(p,final,16))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,16))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr4(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[32], test[32];
    unsigned char k[24] =
    {
        0x7C, 0x5C, 0xB2, 0x40, 0x1B, 0x3D, 0xC3, 0x3C,
        0x19, 0xE7, 0x34, 0x08, 0x19, 0xE0, 0xF6, 0x9C,
        0x67, 0x8C, 0x3D, 0xB8, 0xE6, 0xF6, 0xA9, 0x1A
    };

    unsigned char iv[12] =
    {
        0x00, 0x96, 0xB0, 0x3B,
        0x02, 0x0C, 0x6E, 0xAD,
        0xC2, 0xCB, 0x50, 0x0D
    };

    unsigned char a[32] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    unsigned char p[32] =
    {
        0x45, 0x32, 0x43, 0xFC, 0x60, 0x9B, 0x23, 0x32,
        0x7E, 0xDF, 0xAA, 0xFA, 0x71, 0x31, 0xCD, 0x9F,
        0x84, 0x90, 0x70, 0x1C, 0x5A, 0xD4, 0xA7, 0x9C,
        0xFC, 0x1F, 0xE0, 0xFF, 0x42, 0xF4, 0xFB, 0x00
    };

    cipher_init(&aes1, AES_192_CTR, k,iv,1);
    cipher_init(&aes2, AES_192_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 32);
    cipher_encode(&aes1, final, a, 32);
    print_bytes("Ciphered  ", final, 32);
    print_bytes("It must be", p, 32);
    cipher_decode(&aes2,test,final, 32);
    print_bytes("Deciphered", test, 32);

    if (!memcmp(p,final,32))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,32))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr5(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[36], test[36];
    unsigned char k[24] =
    {
        0x02, 0xBF, 0x39, 0x1E, 0xE8, 0xEC, 0xB1, 0x59,
        0xB9, 0x59, 0x61, 0x7B, 0x09, 0x65, 0x27, 0x9B,
        0xF5, 0x9B, 0x60, 0xA7, 0x86, 0xD3, 0xE0, 0xFE
    };

    unsigned char iv[12] =
    {
        0x00, 0x07, 0xBD, 0xFD,
        0x5C, 0xBD, 0x60, 0x27,
        0x8D, 0xCC, 0x09, 0x12
    };

    unsigned char a[36] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
       0x20, 0x21, 0x22, 0x23
    };

    unsigned char p[36] =
    {
        0x96, 0x89, 0x3F, 0xC5, 0x5E, 0x5C, 0x72, 0x2F,
        0x54, 0x0B, 0x7D, 0xD1, 0xDD, 0xF7, 0xE7, 0x58,
        0xD2, 0x88, 0xBC, 0x95, 0xC6, 0x91, 0x65, 0x88,
        0x45, 0x36, 0xC8, 0x11, 0x66, 0x2F, 0x21, 0x88,
        0xAB, 0xEE, 0x09, 0x35
    };

    cipher_init(&aes1, AES_192_CTR, k,iv,1);
    cipher_init(&aes2, AES_192_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 36);
    cipher_encode(&aes1, final, a, 36);
    print_bytes("Ciphered  ", final, 36);
    print_bytes("It must be", p, 36);
    cipher_decode(&aes2,test,final, 36);
    print_bytes("Deciphered", test, 36);

    if (!memcmp(p,final,36))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,36))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr6(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[16], test[16];
    unsigned char k[32] =
    {
        0x77, 0x6B, 0xEF, 0xF2, 0x85, 0x1D, 0xB0, 0x6F,
        0x4C, 0x8A, 0x05, 0x42, 0xC8, 0x69, 0x6F, 0x6C,
        0x6A, 0x81, 0xAF, 0x1E, 0xEC, 0x96, 0xB4, 0xD3,
        0x7F, 0xC1, 0xD6, 0x89, 0xE6, 0xC1, 0xC1, 0x04
    };

    unsigned char iv[12] =
    {
        0x00, 0x00, 0x00, 0x60,
        0xDB, 0x56, 0x72, 0xC9,
        0x7A, 0xA8, 0xF0, 0xB2
    };

    unsigned char a[16] =
    {
       0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
       0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    };
    unsigned char p[16] =
    {
       0x14, 0x5A, 0xD0, 0x1D, 0xBF, 0x82, 0x4E, 0xC7,
       0x56, 0x08, 0x63, 0xDC, 0x71, 0xE3, 0xE0, 0xC0
    };


    cipher_init(&aes1, AES_256_CTR, k,iv,1);
    cipher_init(&aes2, AES_256_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 16);
    cipher_encode(&aes1, final, a, 16);
    print_bytes("Ciphered  ", final, 16);
    print_bytes("It must be", p, 16);
    cipher_decode(&aes2,test,final, 16);
    print_bytes("Deciphered", test, 16);

    if (!memcmp(p,final,16))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,16))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr7(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[32], test[32];
    unsigned char k[32] =
    {
        0xF6, 0xD6, 0x6D, 0x6B, 0xD5, 0x2D, 0x59, 0xBB,
        0x07, 0x96, 0x36, 0x58, 0x79, 0xEF, 0xF8, 0x86,
        0xC6, 0x6D, 0xD5, 0x1A, 0x5B, 0x6A, 0x99, 0x74,
        0x4B, 0x50, 0x59, 0x0C, 0x87, 0xA2, 0x38, 0x84
    };

    unsigned char iv[12] =
    {
        0x00, 0xFA, 0xAC, 0x24,
        0xC1, 0x58, 0x5E, 0xF1,
        0x5A, 0x43, 0xD8, 0x75
    };

    unsigned char a[32] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };
    unsigned char p[32] =
    {
        0xF0, 0x5E, 0x23, 0x1B, 0x38, 0x94, 0x61, 0x2C,
        0x49, 0xEE, 0x00, 0x0B, 0x80, 0x4E, 0xB2, 0xA9,
        0xB8, 0x30, 0x6B, 0x50, 0x8F, 0x83, 0x9D, 0x6A,
        0x55, 0x30, 0x83, 0x1D, 0x93, 0x44, 0xAF, 0x1C
    };

    cipher_init(&aes1, AES_256_CTR, k,iv,1);
    cipher_init(&aes2, AES_256_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 32);
    cipher_encode(&aes1, final, a, 32);
    print_bytes("Ciphered  ", final, 32);
    print_bytes("It must be", p, 32);
    cipher_decode(&aes2,test,final, 32);
    print_bytes("Deciphered", test, 32);

    if (!memcmp(p,final,32))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,32))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_ctr8(void)
{
    cipher_t  aes1, aes2;
    unsigned char final[36], test[36];
    unsigned char k[32] =
    {
        0xFF, 0x7A, 0x61, 0x7C, 0xE6, 0x91, 0x48, 0xE4,
        0xF1, 0x72, 0x6E, 0x2F, 0x43, 0x58, 0x1D, 0xE2,
        0xAA, 0x62, 0xD9, 0xF8, 0x05 ,0x53, 0x2E, 0xDF,
        0xF1, 0xEE, 0xD6, 0x87, 0xFB, 0x54, 0x15, 0x3D
    };

    unsigned char iv[12] =
    {
        0x00, 0x1C, 0xC5, 0xB7,
        0x51, 0xA5, 0x1D, 0x70,
        0xA1, 0xC1, 0x11, 0x48
    };

    unsigned char a[36] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
       0x20, 0x21, 0x22, 0x23
    };

    unsigned char p[36] =
    {
        0xEB, 0x6C, 0x52, 0x82, 0x1D, 0x0B, 0xBB, 0xF7,
        0xCE, 0x75, 0x94, 0x46, 0x2A, 0xCA, 0x4F, 0xAA,
        0xB4, 0x07, 0xDF, 0x86, 0x65, 0x69, 0xFD, 0x07,
        0xF4, 0x8C, 0xC0, 0xB5, 0x83, 0xD6, 0x07, 0x1F,
        0x1E, 0xC0, 0xE6, 0xB8
    };

    cipher_init(&aes1, AES_256_CTR, k,iv,1);
    cipher_init(&aes2, AES_256_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 36);
    cipher_encode(&aes1, final, a, 36);
    print_bytes("Ciphered  ", final, 36);
    print_bytes("It must be", p, 36);
    cipher_decode(&aes2,test,final, 36);
    print_bytes("Deciphered", test, 36);

    if (!memcmp(p,final,36))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,36))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&aes1);
    cipher_end(&aes2);
}


/* -------------------------------------- */

void test_aes_ctr(void)
{

    printf("**** Test Cipher Engine with AES-CTR ****\n\n");
    printf("Key=128 bits\n\n");
    test_aes_ctr0();
    test_aes_ctr1();
    test_aes_ctr2();
    printf("\nKey=192 bits\n\n");
    test_aes_ctr3();
    test_aes_ctr4();
    test_aes_ctr5();
    printf("\bKey=256 bits\n\n");
    test_aes_ctr6();
    test_aes_ctr7();
    test_aes_ctr8();
}

/* -------------------------------------- */
/* -------------------------------------- */

void test_aes_gcm0(void)
{
    cipher_t  aes;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0xe9, 0x8b, 0x72, 0xa9, 0x88, 0x1a, 0x84, 0xca,
        0x6b, 0x76, 0xe0, 0xf4, 0x3e, 0x68, 0x64, 0x7a
    };

    unsigned char iv[12] =
    {
        0x8b, 0x23, 0x29, 0x9f, 0xde, 0x17,
        0x40, 0x53, 0xf3, 0xd6, 0x52, 0xba
    };

    unsigned char a[16] = {0};
    unsigned char p[16] =
    {
        0x5a, 0x3c, 0x1c, 0xf1, 0x98, 0x5d, 0xbb, 0x8b,
        0xed, 0x81, 0x80, 0x36, 0xfd, 0xd5, 0xab, 0x42
    };

    unsigned char t[16] =
    {
        0x23, 0xc7, 0xab, 0x0f, 0x95, 0x2b, 0x70, 0x91,
        0xcd, 0x32, 0x48, 0x35, 0x04, 0x3b, 0x5e, 0xb5
    };

    cipher_init(&aes, AES_128_GCM, k,NULL,0);

    cipher_set_aead_params(&aes, a, 0, iv, 12, 16);

    printf("TamIV=%d, TamAEAD=%d, TamTag=%d, TamPlain=%d\n", (int)aes.tam_iv, (int)aes.tam_aead, (int)aes.tam_tag, 16);
    print_bytes("Ciphered ", p, 16);
    cipher_decode(&aes, final, p, 16);
    print_bytes("Result", final, 16);
    printf("MustBe=28286a321293253c3e0aa2704a278032\n");
    print_bytes("Tag final", aes.dat_tag, aes.tam_tag);
    printf("MustBe=23c7ab0f952b7091cd324835043b5eb5\n");

    if (cipher_check_aead_tag(&aes, t,16))
        printf("TAG IS CORRECT\n");
    else
        printf("TAG IS INCORRECT\n");

    cipher_encode(&aes, test, final, 16);
    print_bytes("ReCiphered", test, 16);
    printf("MustBe=5a3c1cf1985dbb8bed818036fdd5ab42\n\n");

    cipher_end(&aes);
}


/* -------------------------------------- */

void test_aes_gcm1(void)
{
    cipher_t  aes1,aes2;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0x76, 0x80, 0xc5, 0xd3, 0xca, 0x61, 0x54, 0x75,
        0x8e, 0x51, 0x0f, 0x4d, 0x25, 0xb9, 0x88, 0x20
    };
    unsigned char iv[12] =
    {
        0xf8, 0xf1, 0x05, 0xf9, 0xc3, 0xdf,
        0x49, 0x65, 0x78, 0x03, 0x21, 0xf8
    };
    unsigned char a[16] =
    {
        0xc9, 0x4c, 0x41, 0x01, 0x94, 0xc7, 0x65, 0xe3,
        0xdc, 0xc7, 0x96, 0x43, 0x79, 0x75, 0x8e, 0xd3
    };
    unsigned char p[16] = {0};

    cipher_init(&aes1, AES_128_GCM, k,NULL,0);
    cipher_set_aead_params(&aes1, a, 16, iv, 12, 16);

    cipher_init(&aes2, AES_128_GCM, k,NULL,0);
    cipher_set_aead_params(&aes2, a, 16, iv, 12, 16);

    printf("TamIV=%d, TamAEAD=%d, TamTag=%d, TamPlain=%d\n", (int)aes1.tam_iv, (int)aes1.tam_aead, (int)aes1.tam_tag, 0);
    print_bytes("Original ", p, 0);
    cipher_encode(&aes1, final, p, 0);
    print_bytes("Result", final, 0);
    printf("MustBe=\n");
    print_bytes("Tag final", aes1.dat_tag, aes1.tam_tag);
    printf("MustBe=94dca8edfcf90bb74b153c8d48a17930\n");
    
    cipher_decode(&aes2, test, final, 0);
    print_bytes("Deciphered", test, 0);
    printf("MustBe=\n\n");

    cipher_end(&aes1);
    cipher_end(&aes2);
}

/* -------------------------------------- */

void test_aes_gcm2(void)
{
    cipher_t  aes;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0x96, 0x74, 0x8c, 0xbe, 0x6f, 0x30, 0x51, 0x45,
        0x7e, 0x28, 0x09, 0x3e, 0xf9, 0xa0, 0x26, 0x6b
    };
    unsigned char iv[128] =
    {
        0xcd, 0x66, 0x8d, 0xa3, 0x86, 0xee, 0x2d, 0x1f,
        0xc1, 0x86, 0xb2, 0xe1, 0xfc, 0xdd, 0x88, 0x1d,
        0x67, 0x0d, 0xec, 0x06, 0x23, 0xf9, 0x7d, 0x9a,
        0xbf, 0x3e, 0x22, 0xcb, 0x49, 0x40, 0x72, 0x0b,
        0x5d, 0x1c, 0x08, 0x39, 0x63, 0x99, 0xea, 0x54,
        0x01, 0x04, 0x5c, 0xd7, 0xce, 0xfa, 0xfd, 0xb9,
        0xa5, 0x8b, 0xcf, 0xe2, 0x89, 0x65, 0x3f, 0xf7,
        0x0f, 0x1f, 0xec, 0xea, 0x6a, 0xd9, 0x9d, 0x70,
        0xae, 0x33, 0x50, 0xb1, 0x8b, 0x5d, 0xa1, 0x9a,
        0x61, 0x88, 0x5e, 0x6d, 0xa0, 0xd8, 0x6a, 0xef,
        0x61, 0x89, 0x3d, 0x9f, 0xe0, 0x85, 0x18, 0xc2,
        0x37, 0xed, 0xff, 0x40, 0x1e, 0x4e, 0xd5, 0x41,
        0x76, 0xcd, 0x90, 0xa1, 0x65, 0xf5, 0x39, 0x83,
        0xb8, 0x44, 0xc6, 0x31, 0x18, 0x08, 0x74, 0x74,
        0x23, 0x70, 0x06, 0x30, 0x41, 0xd8, 0xf1, 0x5c,
        0xd3, 0x81, 0xa1, 0x98, 0xed, 0xfd, 0xbc, 0x2d
    };
    unsigned char a[20] =
    {
        0x95, 0x02, 0x97, 0x3e, 0xa6, 0xb2, 0x01, 0x60, 0xd0, 0xed,
        0x78, 0xf5, 0xb1, 0xcb, 0x04, 0x36, 0x51, 0x12, 0xbd, 0x4f
    };
    unsigned char p[16] = {0};

    cipher_init(&aes, AES_128_GCM, k,NULL,0);
    cipher_set_aead_params(&aes, a, 20, iv, 128, 13);

    printf("TamIV=%d, TamAEAD=%d, TamTag=%d, TamPlain=%d\n", (int)aes.tam_iv, (int)aes.tam_aead, (int)aes.tam_tag, 0);
    print_bytes("Original ", p, 0);
    cipher_encode(&aes, final, p, 0);
    print_bytes("Result", final, 0);
    printf("MustBe=\n");
    print_bytes("Tag final", aes.dat_tag, aes.tam_tag);
    printf("MustBe=28a43cc3764d53bed53142e33e\n");
    cipher_encode(&aes, test, final, 0);
    print_bytes("Decifrado", test, 0);
    printf("MustBe=\n\n");
    cipher_end(&aes);
}

/* -------------------------------------- */

void test_aes_gcm3(void)
{
    cipher_t  aes;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0x81, 0x6e, 0x39, 0x07, 0x04, 0x10, 0xcf, 0x21,
        0x84, 0x90, 0x4d, 0xa0, 0x3e, 0xa5, 0x07, 0x5a
    };
    unsigned char iv[12] =
    {
        0x32, 0xc3, 0x67, 0xa3, 0x36, 0x26,
        0x13, 0xb2, 0x7f, 0xc3, 0xe6, 0x7e
    };
    unsigned char a[16] =
    {
        0xf2, 0xa3, 0x07, 0x28, 0xed, 0x87, 0x4e, 0xe0,
        0x29, 0x83, 0xc2, 0x94, 0x43, 0x5d, 0x3c, 0x16
    };
    unsigned char p[16] =
    {
        0x55, 0x2e, 0xbe, 0x01, 0x2e, 0x7b, 0xcf, 0x90,
        0xfc, 0xef, 0x71, 0x2f, 0x83, 0x44, 0xe8, 0xf1
    };
    unsigned char t[16] =
    {
        0xec, 0xaa, 0xe9, 0xfc, 0x68, 0x27, 0x6a, 0x45,
        0xab, 0x0c, 0xa3, 0xcb, 0x9d, 0xd9, 0x53, 0x9f
    };

    cipher_init(&aes, AES_128_GCM, k,NULL,0);
    cipher_set_aead_params(&aes, a, 16, iv, 12, 16);

    printf("TamIV=%d, TamAEAD=%d, TamTag=%d, TamPlain=%d\n", (int)aes.tam_iv, (int)aes.tam_aead, (int)aes.tam_tag, 16);
    print_bytes("Ciphered ", p, 16);
    cipher_decode(&aes, final, p, 16);
    print_bytes("Result", final, 16);
    printf("MustBe=ecafe96c67a1646744f1c891f5e69427\n");
    print_bytes("Tag final", aes.dat_tag, aes.tam_tag);
    printf("MustBe=ecaae9fc68276a45ab0ca3cb9dd9539f\n");

    if (cipher_check_aead_tag(&aes, t,16))
        printf("TAG IS CORRECT\n");
    else
        printf("TAG IS INCORRECT\n");

    cipher_encode(&aes, test, final, 16);
    print_bytes("Ciphered ", test, 16);
    printf("MustBe=552ebe012e7bcf90fcef712f8344e8f1\n\n");

    cipher_end(&aes);
}

/* -------------------------------------- */

void test_aes_gcm4(void)
{
    cipher_t  aes;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0x7f, 0xdd, 0xb5, 0x74, 0x53, 0xc2, 0x41, 0xd0,
        0x3e, 0xfb, 0xed, 0x3a, 0xc4, 0x4e, 0x37, 0x1c
    };
    unsigned char iv[12] =
    {
        0xee, 0x28, 0x3a, 0x3f, 0xc7, 0x55,
        0x75, 0xe3, 0x3e, 0xfd, 0x48, 0x87
    };
    unsigned char a[16] = {0};
    unsigned char p[16] =
    {
        0xd5, 0xde, 0x42, 0xb4, 0x61, 0x64, 0x6c, 0x25,
        0x5c, 0x87, 0xbd, 0x29, 0x62, 0xd3, 0xb9, 0xa2
    };

    cipher_init(&aes, AES_128_GCM, k,NULL,0);
    cipher_set_aead_params(&aes, a, 0, iv, 12, 16);

    printf("TamIV=%d, TamAEAD=%d, TamTag=%d, TamPlain=%d\n", (int)aes.tam_iv, (int)aes.tam_aead, (int)aes.tam_tag, 16);
    print_bytes("Original ", p, 16);
    cipher_encode(&aes, final, p, 16);
    print_bytes("Result", final, 16);
    printf("MustBe=2ccda4a5415cb91e135c2a0f78c9b2fd\n");
    print_bytes("Tag final", aes.dat_tag, aes.tam_tag);
    printf("MustBe=b36d1df9b9d5e596f83e8b7f52971cb3\n");
    cipher_decode(&aes, test, final, 16);
    print_bytes("Deciphered", test, 16);
    printf("MustBe=d5de42b461646c255c87bd2962d3b9a2\n\n");

    cipher_end(&aes);
}

/* -------------------------------------- */

void test_aes_gcm(void)
{

    printf("**** Test Cipher Engine with AES-GCM ****\n\n");
    printf("GCM Mode Key=128 bits\n\n");

    test_aes_gcm0();
    test_aes_gcm1();
    test_aes_gcm2();
    test_aes_gcm3();
    test_aes_gcm4();
}



/* -------------------------------------- */

void test_rc4(void)
{
    unsigned char cipher[64];
    //char *fox = "The quick brown fox jumps over the lazy dog";
    cipher_t rc4;

    printf("**** Test Cipher Engine with RC4 ****\n\n");

    printf("Stream ECB Mode Key=64 bits IV=NULL\n\n");

    cipher_init(&rc4, RC4_64, "Password",NULL,0);
    cipher_encode(&rc4, cipher, "Message", 7);
    printf("Text    =Message\n");
    printf("Expected=6d8a1c142309c3\n");
    print_bytes("Ciphered", cipher, 7);


    cipher_init(&rc4, RC4_64, "Password",NULL,0);
    cipher_decode(&rc4, cipher, cipher, 7);
    cipher[7] = 0;
    printf("Text    =%s\n\n", cipher);

    cipher_end(&rc4);
}

/* -------------------------------------- */

void test_xtea(void)
{
    unsigned char k[16] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };
    unsigned char cipher[8];
    unsigned char plain[3][8] =
    {
        {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48},
        {0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41},
        {0x5a, 0x5b, 0x6e, 0x27, 0x89, 0x48, 0xd7, 0x7f},
    };
    cipher_t ctx1, ctx2;

    printf("**** Test Cipher Engine with XTEA ****\n\n");

    printf("EBC Mode Key=128 bits IV=NULL\n\n");

    cipher_init(&ctx1, XTEA_128_ECB, k,NULL,0);
    cipher_init(&ctx2, XTEA_128_ECB, k,NULL,0);

    print_bytes("Text    ", plain[0], XTEA_BLOCK_SIZE);
    cipher_encode(&ctx1, cipher, plain[0], 8);
    printf("Expected=497df3d072612cb5\n");
    print_bytes("Ciphered", cipher, XTEA_BLOCK_SIZE);
    cipher_decode(&ctx2, cipher, cipher, XTEA_BLOCK_SIZE);
    print_bytes("Text    ", cipher, XTEA_BLOCK_SIZE);
    printf("\n");

    print_bytes("Text    ", plain[1], XTEA_BLOCK_SIZE);
    cipher_encode(&ctx1, cipher, plain[1], 8);
    printf("Expected=e78f2d13744341d8\n");
    print_bytes("Ciphered", cipher, XTEA_BLOCK_SIZE);
    cipher_decode(&ctx2, cipher, cipher, XTEA_BLOCK_SIZE);
    print_bytes("Text    ", cipher, XTEA_BLOCK_SIZE);
    printf("\n");

    print_bytes("Text    ", plain[2], XTEA_BLOCK_SIZE);
    cipher_encode(&ctx1, cipher, plain[2], 8);
    printf("Expected=4141414141414141\n");
    print_bytes("Ciphered", cipher, XTEA_BLOCK_SIZE);
    cipher_decode(&ctx2, cipher, cipher, XTEA_BLOCK_SIZE);
    print_bytes("Text    ", cipher, XTEA_BLOCK_SIZE);
    printf("\n");

    cipher_end(&ctx1);
    cipher_end(&ctx2);

    printf("\n");
}

/* -------------------------------------------------------------------------------- *

    Test vectors can be found at section 4.1 of RFC-5528 at:

    https://datatracker.ietf.org/doc/html/rfc5528#section-4.1


 * -------------------------------------------------------------------------------- */

void test_camellia_ctr0(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[16], test[16];
    unsigned char k[16] =
    {
        0xAE, 0x68, 0x52, 0xF8, 0x12, 0x10, 0x67, 0xCC,
        0x4B, 0xF7, 0xA5, 0x76, 0x55, 0x77, 0xF3, 0x9E
    };

    unsigned char iv[12] =
    {
        0x00, 0x00, 0x00, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    unsigned char a[16] =
    {
        0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
        0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    };
    unsigned char p[16] =
    {
        0xD0, 0x9D, 0xC2, 0x9A, 0x82, 0x14, 0x61, 0x9A,
        0x20, 0x87, 0x7C, 0x76, 0xDB, 0x1F, 0x0B, 0x3F
    };


   cipher_init(&ctx1, CAMELLIA_128_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_128_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 16);
    cipher_encode(&ctx1, final, a, 16);
    print_bytes("Ciphered  ", final, 16);
    print_bytes("It must be", p, 16);
    cipher_decode(&ctx2,test,final, 16);
    print_bytes("Deciphered", test, 16);

    if (!memcmp(p,final,16))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,16))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);
}



/* -------------------------------------- */

void test_camellia_ctr1(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[32], test[32];
    unsigned char k[16] =
    {
        0x7E, 0x24, 0x06, 0x78, 0x17, 0xFA, 0xE0, 0xD7,
        0x43, 0xD6, 0xCE, 0x1F, 0x32, 0x53, 0x91, 0x63
    };

    unsigned char iv[12] =
    {
        0x00, 0x6C, 0xB6, 0xDB,
        0xC0, 0x54, 0x3B, 0x59, 0xDA, 0x48, 0xD9, 0x0B
    };

    unsigned char a[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    unsigned char p[32] =
    {
        0xDB, 0xF3, 0xC7, 0x8D, 0xC0, 0x83, 0x96, 0xD4,
        0xDA, 0x7C, 0x90, 0x77, 0x65, 0xBB, 0xCB, 0x44,
        0x2B, 0x8E, 0x8E, 0x0F, 0x31, 0xF0, 0xDC, 0xA7,
        0x2C, 0x74, 0x17, 0xE3, 0x53, 0x60, 0xE0, 0x48
    };

   cipher_init(&ctx1, CAMELLIA_128_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_128_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 32);
    cipher_encode(&ctx1, final, a, 32);
    print_bytes("Ciphered  ", final, 32);
    print_bytes("It must be", p, 32);
    cipher_decode(&ctx2,test,final, 32);
    print_bytes("Deciphered", test, 32);

    if (!memcmp(p,final,32))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,32))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr2(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[36], test[36];
    unsigned char k[16] =
    {
        0x76, 0x91, 0xBE, 0x03, 0x5E, 0x50, 0x20, 0xA8,
        0xAC, 0x6E, 0x61, 0x85, 0x29, 0xF9, 0xA0, 0xDC
    };

    unsigned char iv[12] =
    {
        0x00, 0xE0, 0x01, 0x7B,
        0x27, 0x77, 0x7F, 0x3F, 0x4A, 0x17, 0x86, 0xF0
    };

    unsigned char a[36] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23
    };

    unsigned char p[36] =
    {
        0xB1, 0x9D, 0x1F, 0xCD, 0xCB, 0x75, 0xEB, 0x88,
        0x2F, 0x84, 0x9C, 0xE2, 0x4D, 0x85, 0xCF, 0x73,
        0x9C, 0xE6, 0x4B, 0x2B, 0x5C, 0x9D, 0x73, 0xF1,
        0x4F, 0x2D, 0x5D, 0x9D, 0xCE, 0x98, 0x89, 0xCD,
        0xDF, 0x50, 0x86, 0x96
    };

   cipher_init(&ctx1, CAMELLIA_128_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_128_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 36);
    cipher_encode(&ctx1, final, a, 36);
    print_bytes("Ciphered  ", final, 36);
    print_bytes("It must be", p, 36);
    cipher_decode(&ctx2,test,final, 36);
    print_bytes("Deciphered", test, 36);

    if (!memcmp(p,final,36))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,36))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr3(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[16], test[16];
    unsigned char k[24] =
    {
        0x16, 0xAF, 0x5B, 0x14, 0x5F, 0xC9, 0xF5, 0x79,
        0xC1, 0x75, 0xF9, 0x3E, 0x3B, 0xFB, 0x0E, 0xED,
        0x86, 0x3D, 0x06, 0xCC, 0xFD, 0xB7, 0x85, 0x15
    };

    unsigned char iv[12] =
    {
        0x00, 0x00, 0x00, 0x48,
        0x36, 0x73, 0x3C, 0x14, 0x7D, 0x6D, 0x93, 0xCB
    };

    unsigned char a[16] =
    {
        0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
        0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    };
    unsigned char p[16] =
    {
        0x23, 0x79, 0x39, 0x9E, 0x8A, 0x8D, 0x2B, 0x2B,
        0x16, 0x70, 0x2F, 0xC7, 0x8B, 0x9E, 0x96, 0x96
    };


   cipher_init(&ctx1, CAMELLIA_192_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_192_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 16);
    cipher_encode(&ctx1, final, a, 16);
    print_bytes("Ciphered  ", final, 16);
    print_bytes("It must be", p, 16);
    cipher_decode(&ctx2,test,final, 16);
    print_bytes("Deciphered", test, 16);

    if (!memcmp(p,final,16))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,16))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr4(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[32], test[32];
    unsigned char k[24] =
    {
        0x7C, 0x5C, 0xB2, 0x40, 0x1B, 0x3D, 0xC3, 0x3C,
        0x19, 0xE7, 0x34, 0x08, 0x19, 0xE0, 0xF6, 0x9C,
        0x67, 0x8C, 0x3D, 0xB8, 0xE6, 0xF6, 0xA9, 0x1A
    };

    unsigned char iv[12] =
    {
        0x00, 0x96, 0xB0, 0x3B,
        0x02, 0x0C, 0x6E, 0xAD,
        0xC2, 0xCB, 0x50, 0x0D
    };

    unsigned char a[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    unsigned char p[32] =
    {
        0x7D, 0xEF, 0x34, 0xF7, 0xA5, 0xD0, 0xE4, 0x15,
        0x67, 0x4B, 0x7F, 0xFC, 0xAE, 0x67, 0xC7, 0x5D,
        0xD0, 0x18, 0xB8, 0x6F, 0xF2, 0x30, 0x51, 0xE0,
        0x56, 0x39, 0x2A, 0x99, 0xF3, 0x5A, 0x4C, 0xED
    };

   cipher_init(&ctx1, CAMELLIA_192_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_192_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 32);
    cipher_encode(&ctx1, final, a, 32);
    print_bytes("Ciphered  ", final, 32);
    print_bytes("It must be", p, 32);
    cipher_decode(&ctx2,test,final, 32);
    print_bytes("Deciphered", test, 32);

    if (!memcmp(p,final,32))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,32))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr5(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[36], test[36];
    unsigned char k[24] =
    {
        0x02, 0xBF, 0x39, 0x1E, 0xE8, 0xEC, 0xB1, 0x59,
        0xB9, 0x59, 0x61, 0x7B, 0x09, 0x65, 0x27, 0x9B,
        0xF5, 0x9B, 0x60, 0xA7, 0x86, 0xD3, 0xE0, 0xFE
    };

    unsigned char iv[12] =
    {
        0x00, 0x07, 0xBD, 0xFD,
        0x5C, 0xBD, 0x60, 0x27, 0x8D, 0xCC, 0x09, 0x12
    };

    unsigned char a[36] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23
    };

    unsigned char p[36] =
    {
        0x57, 0x10, 0xE5, 0x56, 0xE1, 0x48, 0x7A, 0x20,
        0xB5, 0xAC, 0x0E, 0x73, 0xF1, 0x9E, 0x4E, 0x78,
        0x76, 0xF3, 0x7F, 0xDC, 0x91, 0xB1, 0xEF, 0x4D,
        0x4D, 0xAD, 0xE8, 0xE6, 0x66, 0xA6, 0x4D, 0x0E,
        0xD5, 0x57, 0xAB, 0x57
    };

   cipher_init(&ctx1, CAMELLIA_192_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_192_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 36);
    cipher_encode(&ctx1, final, a, 36);
    print_bytes("Ciphered  ", final, 36);
    print_bytes("It must be", p, 36);
    cipher_decode(&ctx2,test,final, 36);
    print_bytes("Deciphered", test, 36);

    if (!memcmp(p,final,36))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,36))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr6(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[16], test[16];
    unsigned char k[32] =
    {
        0x77, 0x6B, 0xEF, 0xF2, 0x85, 0x1D, 0xB0, 0x6F,
        0x4C, 0x8A, 0x05, 0x42, 0xC8, 0x69, 0x6F, 0x6C,
        0x6A, 0x81, 0xAF, 0x1E, 0xEC, 0x96, 0xB4, 0xD3,
        0x7F, 0xC1, 0xD6, 0x89, 0xE6, 0xC1, 0xC1, 0x04
    };

    unsigned char iv[12] =
    {
        0x00, 0x00, 0x00, 0x60,
        0xDB, 0x56, 0x72, 0xC9, 0x7A, 0xA8, 0xF0, 0xB2
    };

    unsigned char a[16] =
    {
        0x53, 0x69, 0x6E, 0x67, 0x6C, 0x65, 0x20, 0x62,
        0x6C, 0x6F, 0x63, 0x6B, 0x20, 0x6D, 0x73, 0x67
    };
    unsigned char p[16] =
    {
        0x34, 0x01, 0xF9, 0xC8, 0x24, 0x7E, 0xFF, 0xCE,
        0xBD, 0x69, 0x94, 0x71, 0x4C, 0x1B, 0xBB, 0x11
    };


   cipher_init(&ctx1, CAMELLIA_256_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_256_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 16);
    cipher_encode(&ctx1, final, a, 16);
    print_bytes("Ciphered  ", final, 16);
    print_bytes("It must be", p, 16);
    cipher_decode(&ctx2,test,final, 16);
    print_bytes("Deciphered", test, 16);

    if (!memcmp(p,final,16))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,16))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr7(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[32], test[32];
    unsigned char k[32] =
    {
        0xF6, 0xD6, 0x6D, 0x6B, 0xD5, 0x2D, 0x59, 0xBB,
        0x07, 0x96, 0x36, 0x58, 0x79, 0xEF, 0xF8, 0x86,
        0xC6, 0x6D, 0xD5, 0x1A, 0x5B, 0x6A, 0x99, 0x74,
        0x4B, 0x50, 0x59, 0x0C, 0x87, 0xA2, 0x38, 0x84
    };

    unsigned char iv[12] =
    {
        0x00, 0xFA, 0xAC, 0x24,
        0xC1, 0x58, 0x5E, 0xF1, 0x5A, 0x43, 0xD8, 0x75
    };

    unsigned char a[32] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    unsigned char p[32] =
    {
        0xD6, 0xC3, 0x03, 0x92, 0x24, 0x6F, 0x78, 0x08,
        0xA8, 0x3C, 0x2B, 0x22, 0xA8, 0x83, 0x9E, 0x45,
        0xE5, 0x1C, 0xD4, 0x8A, 0x1C, 0xDF, 0x40, 0x6E,
        0xBC, 0x9C, 0xC2, 0xD3, 0xAB, 0x83, 0x41, 0x08
    };

   cipher_init(&ctx1, CAMELLIA_256_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_256_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 32);
    cipher_encode(&ctx1, final, a, 32);
    print_bytes("Ciphered  ", final, 32);
    print_bytes("It must be", p, 32);
    cipher_decode(&ctx2,test,final, 32);
    print_bytes("Deciphered", test, 32);

    if (!memcmp(p,final,32))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,32))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}

/* -------------------------------------- */

void test_camellia_ctr8(void)
{
    cipher_t ctx2,ctx1;
    unsigned char final[36], test[36];
    unsigned char k[32] =
    {
        0xFF, 0x7A, 0x61, 0x7C, 0xE6, 0x91, 0x48, 0xE4,
        0xF1, 0x72, 0x6E, 0x2F, 0x43, 0x58, 0x1D, 0xE2,
        0xAA, 0x62, 0xD9, 0xF8, 0x05, 0x53, 0x2E, 0xDF,
        0xF1, 0xEE, 0xD6, 0x87, 0xFB, 0x54, 0x15, 0x3D
    };

    unsigned char iv[12] =
    {
        0x00, 0x1C, 0xC5, 0xB7,
        0x51, 0xA5, 0x1D, 0x70, 0xA1, 0xC1, 0x11, 0x48
    };

    unsigned char a[36] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23
    };

    unsigned char p[36] =
    {
        0xA4, 0xDA, 0x23, 0xFC, 0xE6, 0xA5, 0xFF, 0xAA,
        0x6D, 0x64, 0xAE, 0x9A, 0x06, 0x52, 0xA4, 0x2C,
        0xD1, 0x61, 0xA3, 0x4B, 0x65, 0xF9, 0x67, 0x9F,
        0x75, 0xC0, 0x1F, 0x10, 0x1F, 0x71, 0x27, 0x6F,
        0x15, 0xEF, 0x0D, 0x8D
    };

   cipher_init(&ctx1, CAMELLIA_256_CTR, k,iv,1);
   cipher_init(&ctx2, CAMELLIA_256_CTR, k,iv,1);

    print_bytes("Plaintext ", a, 36);
    cipher_encode(&ctx1, final, a, 36);
    print_bytes("Ciphered  ", final, 36);
    print_bytes("It must be", p, 36);
    cipher_decode(&ctx2,test,final, 36);
    print_bytes("Deciphered", test, 36);

    if (!memcmp(p,final,36))
        printf("Encode IS CORRECT\n");
    else
        printf("Encode IS INCORRECT\n");

    if (!memcmp(a,test,36))
        printf("Decode IS CORRECT\n");
    else
        printf("Decode IS INCORRECT\n");
    printf("----\n");
  cipher_end(&ctx1);
  cipher_end(&ctx2);
}


/* -------------------------------------- */

void test_camellia_ctr(void)
{

    printf("**** Test Cipher Engine with CAMELLIA-CTR ****\n\n");
    printf("Key=128 bits\n\n");
    test_camellia_ctr0();
    test_camellia_ctr1();
    test_camellia_ctr2();
    printf("\nKey=192 bits\n\n");
    test_camellia_ctr3();
    test_camellia_ctr4();
    test_camellia_ctr5();
    printf("\bKey=256 bits\n\n");
    test_camellia_ctr6();
    test_camellia_ctr7();
    test_camellia_ctr8();
}

/* ------------------------- */

unsigned char key[CAMELLIA_256_KEY_SIZE] = 
{
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

unsigned char plain[CAMELLIA_BLOCK_SIZE] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

unsigned char cypher128[CAMELLIA_BLOCK_SIZE] = {0x67, 0x67, 0x31, 0x38, 0x54, 0x96, 0x69, 0x73, 0x08, 0x57, 0x06, 0x56, 0x48, 0xea, 0xbe, 0x43};
unsigned char cypher192[CAMELLIA_BLOCK_SIZE] = {0xb4, 0x99, 0x34, 0x01, 0xb3, 0xe9, 0x96, 0xf8, 0x4e, 0xe5, 0xce, 0xe7, 0xd7, 0x9b, 0x09, 0xb9};
unsigned char cypher256[CAMELLIA_BLOCK_SIZE] = {0x9a, 0xcc, 0x23, 0x7d, 0xff, 0x16, 0xd7, 0x6c, 0x20, 0xef, 0x7c, 0x91, 0x9e, 0x3a, 0x75, 0x09};

unsigned char camiv[CAMELLIA_IV_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

unsigned char cam128key[CAMELLIA_128_KEY_SIZE] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
unsigned char cam192key[CAMELLIA_192_KEY_SIZE] = {0x8E, 0x73, 0xB0, 0xF7, 0xDA, 0x0E, 0x64, 0x52, 0xC8, 0x10, 0xF3, 0x2B, 0x80, 0x90, 0x79, 0xE5, 
                                                  0x62, 0xF8, 0xEA, 0xD2, 0x52, 0x2C, 0x6B, 0x7B};
unsigned char cam256key[CAMELLIA_256_KEY_SIZE] = {0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 
                                                  0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4};

unsigned char plaincbc[CAMELLIA_BLOCK_SIZE] = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A};

unsigned char cypher128cbc[CAMELLIA_BLOCK_SIZE] = {0x16, 0x07, 0xCF, 0x49, 0x4B, 0x36, 0xBB, 0xF0, 0x0D, 0xAE, 0xB0, 0xB5, 0x03, 0xC8, 0x31, 0xAB};
unsigned char cypher192cbc[CAMELLIA_BLOCK_SIZE] = {0x2A, 0x48, 0x30, 0xAB, 0x5A, 0xC4, 0xA1, 0xA2, 0x40, 0x59, 0x55, 0xFD, 0x21, 0x95, 0xCF, 0x93};
unsigned char cypher256cbc[CAMELLIA_BLOCK_SIZE] = {0xE6, 0xCF, 0xA3, 0x5F, 0xC0, 0x2B, 0x13, 0x4A, 0x4D, 0x2C, 0x0B, 0x67, 0x37, 0xAC, 0x3E, 0xDA};

unsigned char keygcm[CAMELLIA_256_KEY_SIZE] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
       0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08};

unsigned char ivgcm[12] = {0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88};
unsigned char aeadgcm[20] = {0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef, 0xab, 0xad, 0xda, 0xd2};

unsigned char plaingcm[60] =
{
0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5, 0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda, 0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53, 0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57, 0xba, 0x63, 0x7b, 0x39};

unsigned char cypher128gcm[60] =
{
0xd0, 0xd9, 0x4a, 0x13, 0xb6, 0x32, 0xf3, 0x37, 0xa0, 0xcc, 0x99, 0x55, 0xb9, 0x4f, 0xa0, 0x20,
0xc8, 0x15, 0xf9, 0x03, 0xaa, 0xb1, 0x2f, 0x1e, 0xfa, 0xf2, 0xfe, 0x9d, 0x90, 0xf7, 0x29, 0xa6,
0xcc, 0xcb, 0xfa, 0x98, 0x6e, 0xf2, 0xff, 0x2c, 0x33, 0xde, 0x41, 0x8d, 0x9a, 0x25, 0x29, 0x09,
0x1c, 0xf1, 0x8f, 0xe6, 0x52, 0xc1, 0xcf, 0xde, 0x13, 0xf8, 0x26, 0x06};

unsigned char tag128gcm[CAMELLIA_BLOCK_SIZE] = {0x9f, 0x45, 0x88, 0x69, 0x43, 0x15, 0x76, 0xea, 0x6a, 0x09, 0x54, 0x56, 0xec, 0x6b, 0x81, 0x01};

unsigned char cypher192gcm[60] =
{
0x0f, 0x00, 0x9e, 0x88, 0x41, 0x0d, 0x84, 0xad, 0x93, 0xc9, 0x0d, 0x55, 0xef, 0xbe, 0x20, 0xff,
0xa8, 0x55, 0x49, 0x2f, 0x4d, 0xfd, 0x0f, 0xb4, 0x85, 0xc4, 0xf0, 0x2f, 0x53, 0x6f, 0xef, 0xfb,
0xb4, 0xd9, 0x67, 0x72, 0x9e, 0x5c, 0x67, 0xf1, 0xde, 0x07, 0x50, 0x25, 0x5c, 0xc5, 0x00, 0x71,
0x6b, 0xa4, 0x83, 0xeb, 0x3b, 0x0a, 0x2b, 0xf6, 0x07, 0xaf, 0x28, 0xf6};

unsigned char tag192gcm[CAMELLIA_BLOCK_SIZE] = {0x01, 0xb1, 0x5b, 0xb5, 0xab, 0x6f, 0xac, 0x0c, 0x42, 0x20, 0x14, 0xe9, 0x1e, 0xac, 0xbf, 0x2b};

unsigned char cypher256gcm[60] =
{
0xad, 0x14, 0x2c, 0x11, 0x57, 0x9d, 0xd9, 0x5e, 0x41, 0xf3, 0xc1, 0xf3, 0x24, 0xda, 0xbc, 0x25,
0x58, 0x64, 0xd9, 0x20, 0xf1, 0xb6, 0x57, 0x59, 0xd8, 0xf5, 0x60, 0xd4, 0x94, 0x8d, 0x44, 0x77,
0x58, 0xdf, 0xdc, 0xf7, 0x7a, 0xa9, 0xf6, 0x25, 0x81, 0xc7, 0xff, 0x57, 0x2a, 0x03, 0x7f, 0x81,
0x0c, 0xb1, 0xa9, 0xc4, 0xb3, 0xca, 0x6e, 0xd6, 0x38, 0x17, 0x9b, 0x77};

unsigned char tag256gcm[CAMELLIA_BLOCK_SIZE] = {0x4e, 0x4b, 0x17, 0x8d, 0x8f, 0xe2, 0x6f, 0xdc, 0x95, 0xe2, 0xe7, 0x24, 0x6d, 0xd9, 0x4b, 0xec};

void test_camellia(void)
{
    cipher_t ctx, ctx2;
    unsigned char tmp[CAMELLIA_BLOCK_SIZE * 6];
    int ret;

    printf("**** Test Cipher Engine with Camellia ****\n\n");

    print_bytes("Plain Text ECB", plain, CAMELLIA_BLOCK_SIZE);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_128_ECB, key,NULL,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_encode(&ctx, tmp, plain, CAMELLIA_BLOCK_SIZE);
        print_bytes("Cypher 128 ECB", tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Should Be ECB ", cypher128, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_128_ECB, key,NULL,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Plain Text ECB", tmp, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_192_ECB, key,NULL,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_encode(&ctx, tmp, plain, CAMELLIA_BLOCK_SIZE);
        print_bytes("Cypher 192 ECB", tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Should Be ECB ", cypher192, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_192_ECB, key,NULL,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Plain Text ECB", tmp, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_256_ECB, key,NULL,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_encode(&ctx, tmp, plain, CAMELLIA_BLOCK_SIZE);
        print_bytes("Cypher 256 ECB", tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Should Be ECB ", cypher256, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_256_ECB, key,NULL,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Plain Text ECB", tmp, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    print_bytes("\nPlain Text CBC", plaincbc, CAMELLIA_BLOCK_SIZE);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_128_CBC, cam128key, camiv,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_encode(&ctx, tmp, plaincbc, CAMELLIA_BLOCK_SIZE);
        print_bytes("Cypher 128 CBC", tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Should Be CBC ", cypher128cbc, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_128_CBC, cam128key, camiv,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Plain Text CBC", tmp, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_192_CBC, cam192key, camiv,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_encode(&ctx, tmp, plaincbc, CAMELLIA_BLOCK_SIZE);
        print_bytes("Cypher 192 CBC", tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Should Be CBC ", cypher192cbc, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx);

        cipher_init(&ctx2, CAMELLIA_192_CBC, cam192key, camiv,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Plain Text CBC", tmp, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx2);

        memcpy(tmp,plaincbc,CAMELLIA_BLOCK_SIZE);
        memcpy(tmp + CAMELLIA_BLOCK_SIZE,plaincbc,CAMELLIA_BLOCK_SIZE);

        print_bytes("\nPlain Text CBC 2", tmp, CAMELLIA_BLOCK_SIZE * 2);
        
        cipher_init(&ctx, CAMELLIA_192_CBC, cam192key, camiv,0);
        cipher_encode(&ctx, tmp, tmp, CAMELLIA_BLOCK_SIZE * 2);
        print_bytes("Cypher 192 CBC 2", tmp, CAMELLIA_BLOCK_SIZE * 2);

        cipher_init(&ctx2, CAMELLIA_192_CBC, cam192key, camiv,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE * 2);        
        print_bytes("Plain Text CBC 2", tmp, CAMELLIA_BLOCK_SIZE * 2);

        if(memcmp(tmp,plaincbc,CAMELLIA_BLOCK_SIZE) || memcmp(tmp + CAMELLIA_BLOCK_SIZE,plaincbc,CAMELLIA_BLOCK_SIZE))
            printf("ERROR\n\n");
        else
            printf("CORRECT\n\n");


        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_256_CBC, cam256key, camiv,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_encode(&ctx, tmp, plaincbc, CAMELLIA_BLOCK_SIZE);
        print_bytes("Cypher 256 CBC", tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Should Be CBC ", cypher256cbc, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_256_CBC, cam256key, camiv,0);
        cipher_decode(&ctx2, tmp, tmp, CAMELLIA_BLOCK_SIZE);
        print_bytes("Plain Text CBC", tmp, CAMELLIA_BLOCK_SIZE);
        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    print_bytes("\nPlain Text GCM", plaingcm, 60);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_128_GCM, keygcm,NULL,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        unsigned char tag[CAMELLIA_BLOCK_SIZE];

        printf("\nCipher Name = %s \n\n",ctx.namec);

        cipher_set_aead_params(&ctx, aeadgcm, 20, ivgcm, 12, CAMELLIA_BLOCK_SIZE);

        cipher_encode(&ctx, tmp, plaingcm, 60);
        print_bytes("Cypher 128 GCM", tmp, 60);

        cipher_get_aead_tag(&ctx, tag, CAMELLIA_BLOCK_SIZE);
        print_bytes("Tag of 128 GCM", tag, CAMELLIA_BLOCK_SIZE);

        print_bytes("Should Be GCM ", cypher128gcm, 60);
        print_bytes("Should Tag GCM", tag128gcm, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_128_GCM, keygcm,NULL,0);

        cipher_set_aead_params(&ctx2, aeadgcm, 20, ivgcm, 12, CAMELLIA_BLOCK_SIZE);
        cipher_decode(&ctx2, tmp, tmp, 60);

        print_bytes("Plain 128 GCM ", tmp, 60);
        cipher_get_aead_tag(&ctx2, tag, CAMELLIA_BLOCK_SIZE);
        print_bytes("Tag of 128 GCM", tag, CAMELLIA_BLOCK_SIZE);

        print_bytes("Should Be GCM ", plaingcm, 60);
        print_bytes("Should Tag GCM", tag128gcm, CAMELLIA_BLOCK_SIZE);

        cipher_end(&ctx2);
    }
    cipher_end(&ctx);

    printf("\n");

    ret = cipher_init(&ctx, CAMELLIA_192_GCM, keygcm,NULL,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        unsigned char tag[CAMELLIA_BLOCK_SIZE];

        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_set_aead_params(&ctx, aeadgcm, 20, ivgcm, 12, CAMELLIA_BLOCK_SIZE);

        cipher_encode(&ctx, tmp, plaingcm, 60);
        print_bytes("Cypher 192 GCM", tmp, 60);

        cipher_get_aead_tag(&ctx, tag, CAMELLIA_BLOCK_SIZE);
        print_bytes("Tag of 192 GCM", tag, CAMELLIA_BLOCK_SIZE);

        print_bytes("Should Be GCM ", cypher192gcm, 60);
        print_bytes("Should Tag GCM", tag192gcm, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_192_GCM, keygcm,NULL,0);
        cipher_set_aead_params(&ctx2, aeadgcm, 20, ivgcm, 12, CAMELLIA_BLOCK_SIZE);
        cipher_decode(&ctx2, tmp, tmp, 60);
        print_bytes("Plain 192 GCM ", tmp, 60);
        cipher_get_aead_tag(&ctx, tag, CAMELLIA_BLOCK_SIZE);
        print_bytes("Tag of 192 GCM", tag, CAMELLIA_BLOCK_SIZE);

        print_bytes("Should Be GCM ", plaingcm, 60);
        print_bytes("Should Tag GCM", tag128gcm, CAMELLIA_BLOCK_SIZE);
    }
    cipher_end(&ctx);

    printf("\n");
    ret = cipher_init(&ctx, CAMELLIA_256_GCM, keygcm,NULL,0);
    if (ret)
    {
        printf("ERROR %d initializing crypto\n", ret);
    }
    else
    {
        unsigned char tag[CAMELLIA_BLOCK_SIZE];

        printf("\nCipher Name = %s \n\n",ctx.namec);
        cipher_set_aead_params(&ctx, aeadgcm, 20, ivgcm, 12, CAMELLIA_BLOCK_SIZE);

        cipher_encode(&ctx, tmp, plaingcm, 60);
        print_bytes("Cypher 256 GCM", tmp, 60);

        cipher_get_aead_tag(&ctx, tag, CAMELLIA_BLOCK_SIZE);
        print_bytes("Tag of 256 GCM", tag, CAMELLIA_BLOCK_SIZE);

        print_bytes("Should Be GCM ", cypher256gcm, 60);
        print_bytes("Should Tag GCM", tag256gcm, CAMELLIA_BLOCK_SIZE);

        cipher_init(&ctx2, CAMELLIA_256_GCM, keygcm,NULL,0);
        cipher_set_aead_params(&ctx2, aeadgcm, 20, ivgcm, 12, CAMELLIA_BLOCK_SIZE);
        cipher_decode(&ctx2, tmp, tmp, 60);
        print_bytes("Plain 256 GCM ", tmp, 60);
        cipher_get_aead_tag(&ctx, tag, CAMELLIA_BLOCK_SIZE);
        print_bytes("Tag of 256 GCM", tag, CAMELLIA_BLOCK_SIZE);

        print_bytes("Should Be GCM ", plaingcm, 60);
        print_bytes("Should Tag GCM", tag128gcm, CAMELLIA_BLOCK_SIZE);
    }
    cipher_end(&ctx);

    printf("\n");
}


/* -------------------------------------- */

/* Test Vectors from here: https://github.com/weidai11/cryptopp/blob/master/TestVectors/aria.txt */

void test_aria(void)
{
    cipher_t ctx1,ctx2;    
    unsigned char cipher[ARIA_BLOCK_SIZE * 12];
    unsigned char k[32] =
    {
       0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
       0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };

    unsigned char iv[ARIA_IV_SIZE] = 
    {
        0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78, 0x87, 0x96, 0xa5, 0xb4, 0xc3, 0xd2, 0xe1, 0xf0
    };

    unsigned char plain[ARIA_BLOCK_SIZE * 10] =
    {        
        0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb, 
        0x11, 0x11, 0x11, 0x11, 0xcc, 0xcc, 0xcc, 0xcc, 0x11, 0x11, 0x11, 0x11, 0xdd, 0xdd, 0xdd, 0xdd,
        0x22, 0x22, 0x22, 0x22, 0xaa, 0xaa, 0xaa, 0xaa, 0x22, 0x22, 0x22, 0x22, 0xbb, 0xbb, 0xbb, 0xbb,
        0x22, 0x22, 0x22, 0x22, 0xcc, 0xcc, 0xcc, 0xcc, 0x22, 0x22, 0x22, 0x22, 0xdd, 0xdd, 0xdd, 0xdd,
        0x33, 0x33, 0x33, 0x33, 0xaa, 0xaa, 0xaa, 0xaa, 0x33, 0x33, 0x33, 0x33, 0xbb, 0xbb, 0xbb, 0xbb,
        0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc, 0x33, 0x33, 0x33, 0x33, 0xdd, 0xdd, 0xdd, 0xdd,
        0x44, 0x44, 0x44, 0x44, 0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 0xbb, 0xbb, 0xbb, 0xbb,
        0x44, 0x44, 0x44, 0x44, 0xcc, 0xcc, 0xcc, 0xcc, 0x44, 0x44, 0x44, 0x44, 0xdd, 0xdd, 0xdd, 0xdd,
        0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xbb, 0xbb, 0xbb, 0xbb, 
        0x55, 0x55, 0x55, 0x55, 0xcc, 0xcc, 0xcc, 0xcc, 0x55, 0x55, 0x55, 0x55, 0xdd, 0xdd, 0xdd, 0xdd, 
    };

    printf("**** Test Cypher Engine using ARIA ****\n\n");

    printf("ECB Mode Key=128 bits\n");

    cipher_init(&ctx1, ARIA_128_ECB, k,NULL,0);    
    cipher_init(&ctx2, ARIA_128_ECB, k,NULL,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", plain, ARIA_BLOCK_SIZE * 2);
    printf("Expected=11111111aaaaaaaa11111111bbbbbbbb11111111cccccccc11111111dddddddd...\n");
    print_bytes("Ciphered", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Expected=c6ecd08e22c30abdb215cf74e2075e6e29ccaac63448708d331b2f816c51b17d...\n");    
    cipher_decode(&ctx2, cipher, cipher,ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Result : %s\n",(memcmp(cipher,plain,ARIA_BLOCK_SIZE * 10)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

    printf("ECB Mode Key=192 bits\n");

    cipher_init(&ctx1, ARIA_192_ECB, k,NULL,0);    
    cipher_init(&ctx2, ARIA_192_ECB, k,NULL,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", plain, ARIA_BLOCK_SIZE * 2);
    printf("Expected=11111111aaaaaaaa11111111bbbbbbbb11111111cccccccc11111111dddddddd...\n");
    print_bytes("Ciphered", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Expected=8d1470625f59ebacb0e55b534b3e462b5f23d33bff78f46c3c15911f4a21809a...\n");    
    cipher_decode(&ctx2, cipher, cipher,ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Result : %s\n",(memcmp(cipher,plain,ARIA_BLOCK_SIZE * 10)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);        
    cipher_end(&ctx2);

    printf("ECB Mode Key=256 bits\n");

    cipher_init(&ctx1, ARIA_256_ECB, k,NULL,0);    
    cipher_init(&ctx2, ARIA_256_ECB, k,NULL,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", plain, ARIA_BLOCK_SIZE * 2);
    printf("Expected=11111111aaaaaaaa11111111bbbbbbbb11111111cccccccc11111111dddddddd...\n");
    print_bytes("Ciphered", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Expected=58a875e6044ad7fffa4f58420f7f442d8e191016f28e79aefc01e204773280d7...\n");    
    cipher_decode(&ctx2, cipher, cipher,ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Result : %s\n",(memcmp(cipher,plain,ARIA_BLOCK_SIZE * 10)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);    
    cipher_end(&ctx2); 
    

    printf("CBC Mode Key=128 bits IV=0f1e2d3c4b5a69788796a5b4c3d2e1f0\n");

    cipher_init(&ctx1, ARIA_128_CBC, k,iv,0);    
    cipher_init(&ctx2, ARIA_128_CBC, k,iv,0);        
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", plain, ARIA_BLOCK_SIZE * 2);
    printf("Expected=11111111aaaaaaaa11111111bbbbbbbb11111111cccccccc11111111dddddddd...\n");
    print_bytes("Ciphered", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Expected=49d61860b14909109cef0d22a9268134fadf9fb23151e9645fba75018bdb1538...\n");    
    cipher_decode(&ctx2, cipher, cipher,ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Result : %s\n",(memcmp(cipher,plain,ARIA_BLOCK_SIZE * 10)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

    printf("CBC Mode Key=192 bits IV=0f1e2d3c4b5a69788796a5b4c3d2e1f0\n");

    cipher_init(&ctx1, ARIA_192_CBC, k,iv,0);    
    cipher_init(&ctx2, ARIA_192_CBC, k,iv,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", plain, ARIA_BLOCK_SIZE * 2);
    printf("Expected=11111111aaaaaaaa11111111bbbbbbbb11111111cccccccc11111111dddddddd...\n");
    print_bytes("Ciphered", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Expected=afe6cf23974b533c672a826264ea785f4e4f7f780dc7f3f1e0962b80902386d5...\n");    
    cipher_decode(&ctx2, cipher, cipher,ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Result : %s\n",(memcmp(cipher,plain,ARIA_BLOCK_SIZE * 10)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

    printf("CBC Mode Key=256 bits IV=0f1e2d3c4b5a69788796a5b4c3d2e1f0\n");

    cipher_init(&ctx1, ARIA_256_CBC, k,iv,0);    
    cipher_init(&ctx2, ARIA_256_CBC, k,iv,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", plain, ARIA_BLOCK_SIZE * 2);
    printf("Expected=11111111aaaaaaaa11111111bbbbbbbb11111111cccccccc11111111dddddddd...\n");
    print_bytes("Ciphered", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Expected=523a8a806ae621f155fdd28dbc34e1ab7b9b42432ad8b2efb96e23b13f0a6e52...\n");    
    cipher_decode(&ctx2, cipher, cipher,ARIA_BLOCK_SIZE * 10);
    print_bytes("Text    ", cipher, ARIA_BLOCK_SIZE * 2);
    printf("Result : %s\n",(memcmp(cipher,plain,ARIA_BLOCK_SIZE * 10)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

}

/* -------------------------------------- */

/* Test Vectors from here: https://www.schneier.com/wp-content/uploads/2015/12/ecb_ival.txt */

void test_twofish(void)
{
    cipher_t ctx1,ctx2;    
    unsigned char cipher[TWOFISH_BLOCK_SIZE * 10];
    unsigned char k1[16] = {0};    
    unsigned char k2[32] = 
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };        
/*
    unsigned char iv[TWOFISH_IV_SIZE] = 
    {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
*/
    unsigned char plain[TWOFISH_BLOCK_SIZE * 10] = {0}; 


    printf("**** Test Cypher Engine using TWOFISH ****\n\n");

    printf("ECB Mode Key=128 bits\n");
    cipher_init(&ctx1, TWOFISH_128_ECB, k1,NULL,0);    
    cipher_init(&ctx2, TWOFISH_128_ECB, k1,NULL,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", plain, TWOFISH_BLOCK_SIZE);
    printf("Expected=00000000000000000000000000000000\n");
    print_bytes("Ciphered", cipher, TWOFISH_BLOCK_SIZE);
    printf("Expected=9f589f5cf6122c32b6bfec2f2ae8c35a\n");    
    cipher_decode(&ctx2, cipher, cipher,TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", cipher, TWOFISH_BLOCK_SIZE);
    printf("Result : %s\n",(memcmp(cipher,plain,TWOFISH_BLOCK_SIZE)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

    printf("ECB Mode Key=192 bits\n");

    cipher_init(&ctx1, TWOFISH_192_ECB, k2,NULL,0);    
    cipher_init(&ctx2, TWOFISH_192_ECB, k2,NULL,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", plain, TWOFISH_BLOCK_SIZE);
    printf("Expected=00000000000000000000000000000000\n");
    print_bytes("Ciphered", cipher, TWOFISH_BLOCK_SIZE);
    printf("Expected=cfd1d2e5a9be9cdf501f13b892bd2248\n");    
    cipher_decode(&ctx2, cipher, cipher,TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", cipher, TWOFISH_BLOCK_SIZE);
    printf("Result : %s\n",(memcmp(cipher,plain,TWOFISH_BLOCK_SIZE)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

    printf("ECB Mode Key=256 bits\n");

    cipher_init(&ctx1, TWOFISH_256_ECB, k2,NULL,0);    
    cipher_init(&ctx2, TWOFISH_256_ECB, k2,NULL,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", plain, TWOFISH_BLOCK_SIZE);
    printf("Expected=00000000000000000000000000000000\n");
    print_bytes("Ciphered", cipher, TWOFISH_BLOCK_SIZE);
    printf("Expected=37527be0052334b89f0cfccae87cfa20\n");    
    cipher_decode(&ctx2, cipher, cipher,TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", cipher, TWOFISH_BLOCK_SIZE);
    printf("Result : %s\n",(memcmp(cipher,plain,TWOFISH_BLOCK_SIZE)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

/*
    printf("CBC Mode Key=128 bits IV=0f1e2d3c4b5a69788796a5b4c3d2e1f0\n");
    
    cipher_init(&ctx1, TWOFISH_128_CBC, k, iv,0);    
    cipher_init(&ctx2, TWOFISH_128_CBC, k, iv,0);    
    printf("\nNames = %s\n\n",ctx1.namec);
    cipher_encode(&ctx1, cipher, plain, TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", plain, TWOFISH_BLOCK_SIZE);
    printf("Expected=00000000000000000000000000000000\n");
    print_bytes("Ciphered", cipher, TWOFISH_BLOCK_SIZE);
    printf("Expected=9F589F5CF6122C32B6BFEC2F2AE8C35A\n");    
    cipher_decode(&ctx2, cipher, cipher,TWOFISH_BLOCK_SIZE);
    print_bytes("Text    ", cipher, TWOFISH_BLOCK_SIZE);
    printf("Result : %s\n",(memcmp(cipher,plain,TWOFISH_BLOCK_SIZE)) ? "ERROR" : "CORRECT");
    printf("\n");
    cipher_end(&ctx1);
    cipher_end(&ctx2);

*/
    
}

/* -------------------------------------- */

unsigned char key1[16] =
{
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char *esp1[4] =
{
    "4DFA5E481DA23EA09A31022050859936DA52FCEE218005164F267CB65F5CFD7F2B4F97E0FF16924A52DF269515110A07F9E460BC65EF95DA58F740B7D1DBB0AA",
    "DA9C1581F429E0A00F7D67E23B730676783B262E8EB43A25F55FB90B3E753AEF8C6713EC66C51881111593CCB3E8CB8F8DE124080501EEEB389C4BCB6977CF95",
    "7D5789631EB4554400E1E025935DFA7B3E9039D61BDC58A8697D36815BF1985CEFDF7AE112E5BB81E37ECF0616CE7147FC08A93A367E08631F23C03B00A8DA2F",
    "B375703739DACED4DD4059FD71C3C47FC2F9939670FAD4A46066ADCC6A5645783308B90FFB72BE04A6B147CBE38CC0C3B9267C296A92A7C69873F9F263BE9703"
};

unsigned char key2[16] =
{
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char *esp2[4] =
{
    "0471076057830FB99202291177FBFE5D38C888944DF8917CAB82788B91B53D1CFB06D07A304B18BB763F888A61BB6B755CD58BEC9C4CFB7569CB91862E79C459",
    "D1D7E97556426E6CFC21312AE38114259E5A6FB10DACBD88E4354B04725569352B6DA5ACAFACD5E266F9575C2ED8E6F2EFE4B4D36114C3A623DD49F4794F865B",
    "AF06FAA82C73291231E1BD916A773DE152FD2126C40A10C3A6EB40F22834B8CC68BD5C6DBD7FC1EC8F34165C517C0B639DB0C60506D3606906B8463AA0D0EC2F",
    "AB3216F1216379EFD5EC589510B8FD35014D0AA0B613040BAE63ECAB90A9AF79661F8DA2F853A5204B0F8E72E9D9EB4DBA5A4690E73A4D25F61EE7295215140C"
};

unsigned char key3[32] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char *esp3[4] =
{
    "98951956F4BD5E2E9DC624CCD2D79E606D24A4DB51D413FDAF9A9741A6F079B421400FDA0B4D8785578BB318BDAD4ABCA8C2D1BA3BA4E18C2F5572499F345BC1",
    "C3A267F0EB87ED714E09CABC2780FEF6E5F665BBBBB44C8448D8EB42D88275CD62AD759AAC9F4080F73993DE50FF94E834E2CF7B74A91E68B38EACE9C12922C2",
    "78BD0BB32A69E62362EE7E31F1DD9E96CA6E196844EFD9459F270D612119DFA45DD1522967629143CECD585CFE62B7FD9D1503A62A238C35A66595C49DD71575",
    "C17F946C14A492392A1C554993F406B2EA806E4186D97FCB420C21FB4245A3DB4EBA2BCB59D2C33CE2CD5044A79A96F95182112D9724E16AD9E965047DA71F05"
};

char *vacio[4] = {
    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
    "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
};

/* -------------------------------------- */

void print64(int pos, unsigned char *buf, char *esperado[4])
{
    int t;
    char *ptr;

    ptr = esperado[pos];

    printf("buf[%d]=", pos);

    for (t = 0; t < 64; t++)
        printf("%02X", buf[t]);

    printf("\nesp[%d]=", pos);
    for (t = 0; t < 64; t++)
    {
        printf("%c", *ptr++);
        printf("%c", *ptr++);
    }
    printf("\n\n");
}

/* -------------------------------------- */

void test_salsa20(void)
{
    cipher_t ctx;
    unsigned char buf[512];
    unsigned char iv3[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00};        

    printf("**** Salsa20 Test ****\n\n");

    printf("Stream Mode Key=128 bits IV=NULL\n\n");

    memset(buf, 0, 512);

    cipher_init(&ctx, SALSA20_128, key1, NULL,0);
    cipher_encode(&ctx, buf, buf, 512);

    print64(0, buf, esp1);
    print64(1, buf + 192, esp1);
    print64(2, buf + 256, esp1);
    print64(3, buf + 448, esp1);

    // Decrypt must leav all back to 0

    cipher_init(&ctx, SALSA20_128, key1, NULL,0);
    cipher_decode(&ctx, buf, buf, 512);

    //memset(buf,0,512);

    cipher_init(&ctx, SALSA20_128,key2, NULL,0);

    cipher_encode(&ctx, buf, buf, 512);

    print64(0, buf, esp2);
    print64(1, buf + 192, esp2);
    print64(2, buf + 256, esp2);
    print64(3, buf + 448, esp2);

    // Decrypt must leav all back to 0

    cipher_init(&ctx, SALSA20_128, key2, NULL,0);
    cipher_decode(&ctx, buf, buf, 512);

    //memset(buf,0,512);

    printf("Stream Mode Key=256 bits IV=....\n\n");

    //memset(buf,0,512);

    cipher_init(&ctx, SALSA20_256, key3, iv3,0);
    cipher_encode(&ctx, buf, buf, 512);

    print64(0, buf, esp3);
    print64(1, buf + 192, esp3);
    print64(2, buf + 256, esp3);
    print64(3, buf + 448, esp3);

    // Decrypt must leav all back to 0

    cipher_init(&ctx, SALSA20_256,key3,iv3,0);
    cipher_decode(&ctx, buf, buf, 512);

    print64(0, buf, vacio);
    print64(1, buf + 192, vacio);
    print64(2, buf + 256, vacio);
    print64(3, buf + 448, vacio);

    //memset(buf,0,512);

    cipher_end(&ctx);

    printf("\n");
}

/* -------------------------------------- *
    These are for test_chacha()
 * -------------------------------------- */

unsigned char esp8_0[64] =
    {
        0xe2, 0x8a, 0x5f, 0xa4, 0xa6, 0x7f, 0x8c, 0x5d, 0xef, 0xed, 0x3e, 0x6f, 0xb7, 0x30, 0x34, 0x86,
        0xaa, 0x84, 0x27, 0xd3, 0x14, 0x19, 0xa7, 0x29, 0x57, 0x2d, 0x77, 0x79, 0x53, 0x49, 0x11, 0x20,
        0xb6, 0x4a, 0xb8, 0xe7, 0x2b, 0x8d, 0xeb, 0x85, 0xcd, 0x6a, 0xea, 0x7c, 0xb6, 0x08, 0x9a, 0x10,
        0x18, 0x24, 0xbe, 0xeb, 0x08, 0x81, 0x4a, 0x42, 0x8a, 0xab, 0x1f, 0xa2, 0xc8, 0x16, 0x08, 0x1b};

unsigned char esp8_1[64] =
    {
        0x8a, 0x26, 0xaf, 0x44, 0x8a, 0x1b, 0xa9, 0x06, 0x36, 0x8f, 0xd8, 0xc8, 0x38, 0x31, 0xc1, 0x8c,
        0xec, 0x8c, 0xed, 0x81, 0x1a, 0x02, 0x8e, 0x67, 0x5b, 0x8d, 0x2b, 0xe8, 0xfc, 0xe0, 0x81, 0x16,
        0x5c, 0xea, 0xe9, 0xf1, 0xd1, 0xb7, 0xa9, 0x75, 0x49, 0x77, 0x49, 0x48, 0x05, 0x69, 0xce, 0xb8,
        0x3d, 0xe6, 0xa0, 0xa5, 0x87, 0xd4, 0x98, 0x4f, 0x19, 0x92, 0x5f, 0x5d, 0x33, 0x8e, 0x43, 0x0d};

unsigned char esp12_0[64] =
    {
        0xe1, 0x04, 0x7b, 0xa9, 0x47, 0x6b, 0xf8, 0xff, 0x31, 0x2c, 0x01, 0xb4, 0x34, 0x5a, 0x7d, 0x8c,
        0xa5, 0x79, 0x2b, 0x0a, 0xd4, 0x67, 0x31, 0x3f, 0x1d, 0xc4, 0x12, 0xb5, 0xfd, 0xce, 0x32, 0x41,
        0x0d, 0xea, 0x8b, 0x68, 0xbd, 0x77, 0x4c, 0x36, 0xa9, 0x20, 0xf0, 0x92, 0xa0, 0x4d, 0x3f, 0x95,
        0x27, 0x4f, 0xbe, 0xff, 0x97, 0xbc, 0x84, 0x91, 0xfc, 0xef, 0x37, 0xf8, 0x59, 0x70, 0xb4, 0x50};

unsigned char esp12_1[64] =
    {
        0x1d, 0x43, 0xb6, 0x1a, 0x8f, 0x7e, 0x19, 0xfc, 0xed, 0xde, 0xf3, 0x68, 0xae, 0x6b, 0xfb, 0x11,
        0x10, 0x1b, 0xd9, 0xfd, 0x3e, 0x4d, 0x12, 0x7d, 0xe3, 0x0d, 0xb2, 0xdb, 0x1b, 0x47, 0x2e, 0x76,
        0x42, 0x68, 0x03, 0xa4, 0x5e, 0x15, 0xb9, 0x62, 0x75, 0x19, 0x86, 0xef, 0x1d, 0x9d, 0x50, 0xf5,
        0x98, 0xa5, 0xdc, 0xdc, 0x9f, 0xa5, 0x29, 0xa2, 0x83, 0x57, 0x99, 0x1e, 0x78, 0x4e, 0xa2, 0x0f};

unsigned char esp20_0[64] =
    {
        0x89, 0x67, 0x09, 0x52, 0x60, 0x83, 0x64, 0xfd, 0x00, 0xb2, 0xf9, 0x09, 0x36, 0xf0, 0x31, 0xc8,
        0xe7, 0x56, 0xe1, 0x5d, 0xba, 0x04, 0xb8, 0x49, 0x3d, 0x00, 0x42, 0x92, 0x59, 0xb2, 0x0f, 0x46,
        0xcc, 0x04, 0xf1, 0x11, 0x24, 0x6b, 0x6c, 0x2c, 0xe0, 0x66, 0xbe, 0x3b, 0xfb, 0x32, 0xd9, 0xaa,
        0x0f, 0xdd, 0xfb, 0xc1, 0x21, 0x23, 0xd4, 0xb9, 0xe4, 0x4f, 0x34, 0xdc, 0xa0, 0x5a, 0x10, 0x3f};

unsigned char esp20_1[64] =
    {
        0x6c, 0xd1, 0x35, 0xc2, 0x87, 0x8c, 0x83, 0x2b, 0x58, 0x96, 0xb1, 0x34, 0xf6, 0x14, 0x2a, 0x9d,
        0x4d, 0x8d, 0x0d, 0x8f, 0x10, 0x26, 0xd2, 0x0a, 0x0a, 0x81, 0x51, 0x2c, 0xbc, 0xe6, 0xe9, 0x75,
        0x8a, 0x71, 0x43, 0xd0, 0x21, 0x97, 0x80, 0x22, 0xa3, 0x84, 0x14, 0x1a, 0x80, 0xce, 0xa3, 0x06,
        0x2f, 0x41, 0xf6, 0x7a, 0x75, 0x2e, 0x66, 0xad, 0x34, 0x11, 0x98, 0x4c, 0x78, 0x7e, 0x30, 0xad};

/* -------------------------------------- */

void printCha(int pos, unsigned char *buf, unsigned char *esp)
{
    int t;

    printf("buf[%d]=", pos);

    for (t = 0; t < 64; t++)
        printf("%02X", buf[t]);

    printf("\nesp[%d]=", pos);
    for (t = 0; t < 64; t++)
        printf("%02X", esp[t]);
    printf("\n\n");
}

/* -------------------------------------- */

void test_chacha(void)
{
    cipher_t ctx;
    unsigned char buf[512];
    unsigned char ke[64];
    //    unsigned char iv3[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00};

    printf("**** Test Chacha ****\n\n");

    printf("Stream Mode Key=128 bits IV=NULL\n\n");

    memset(ke, 0, 64);
    memset(buf, 0, 512);

    cipher_init(&ctx, CHACHA8_128, ke,NULL,0);
    cipher_encode(&ctx, buf, buf, 512);

    printCha(0, buf, esp8_0);
    printCha(1, buf + 64, esp8_1);

    // Decrypt must leav all back to 0

    cipher_init(&ctx, CHACHA8_128, ke,NULL,0);
    cipher_decode(&ctx, buf, buf, 512);

    //memset(buf,0,512);

    printf("\n");

    cipher_init(&ctx, CHACHA12_128,ke,NULL,0);
    cipher_encode(&ctx, buf, buf, 512);

    printCha(0, buf, esp12_0);
    printCha(1, buf + 64, esp12_1);

    // Decrypt must leav all back to 0

    cipher_init(&ctx, CHACHA12_128,ke,NULL,0);
    cipher_decode(&ctx, buf, buf, 512);

    //memset(buf,0,512);

    printf("\n");

    //memset(buf,0,512);

    cipher_init(&ctx, CHACHA20_128,ke,NULL,0);
    cipher_encode(&ctx, buf, buf, 512);

    printCha(0, buf, esp20_0);
    printCha(1, buf + 64, esp20_1);

    // Decrypt must leav all back to 0

    cipher_init(&ctx, CHACHA20_128,ke,NULL,0);
    cipher_decode(&ctx, buf, buf, 512);

    //memset(buf,0,512);

    cipher_end(&ctx);

    printf("\n");
}

/* -------------------------------------- */

void test_chacha20(void)
{
    cipher_t ctx;
    unsigned char key[32] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    unsigned char iv[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
    unsigned char crypt[512];
    char *plain = "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.";
    size_t tam = 114;

    printf("**** Test Chacha20 IETF ****\n\n");

    printf("Stream Mode Key=256 bits IV=....\n\n");

    cipher_init(&ctx, CHACHA20_IETF,key, iv, 1);
    cipher_encode(&ctx, crypt, plain, tam);

    print_bytes("plain ", plain, tam);
    print_bytes("crypt ", crypt, tam);
    printf("mustbe=6e2e359a2568f98041ba0728dd0d6981e97e7aec1d4360c20a27afccfd9fae0bf91b65c5524733ab8f593dabcd62b3571639d624e65152ab8f530c359f0861d807ca0dbf500d6a6156a38e088a22b65e52bc514d16ccf806818ce91ab77937365af90bbf74a35be6b40b8eedf2785e42874d\n");

    cipher_end(&ctx);

    printf("\n");
}

/* -------------------------------------- */

void test_chacha20poly1305(void)
{
    cipher_t ctx;
    unsigned char crypt[512];
    char *plain = "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.";
    size_t tam = 114;
    unsigned char iv[12]   =
    {
        0x07, 0x00, 0x00, 0x00, 0x40, 0x41,
        0x42, 0x43, 0x44, 0x45, 0x46, 0x47
    };
    unsigned char aead[12] =
    {
        0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1,
        0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7
    };
    unsigned char key[32]  =
    {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
    };
    unsigned char tag[POLY1305_SIZE] =
    {
        0x1a, 0xe1, 0x0b, 0x59, 0x4f, 0x09, 0xe2, 0x6a,
        0x7e, 0x90, 0x2e, 0xcb, 0xd0, 0x60, 0x06, 0x91
    };

    printf("**** Chacha20-Poly1305 Test ****\n\n");

    printf("Stream Mode Key=256 bits + HMAC 256 bits - IV=...Key\n\n");

    cipher_init(&ctx, CHACHA20_POLY_1305, key,NULL,0);
    cipher_set_aead_params(&ctx, aead, 12, iv, 12, POLY1305_SIZE);
    cipher_encode(&ctx, crypt, plain, tam);

    print_bytes("plain ", plain, tam);
    print_bytes("crypt ", crypt, tam);
    printf("mustbe=d31a8d34648e60db7b86afbc53ef7ec2a4aded51296e08fea9e2b5a736ee62d63dbea45e8ca9671282fafb69da92728b1a71de0a9e060b2905d6a5b67ecd3b3692ddbd7f2d778b8c9803aee328091b58fab324e4fad675945585808b4831d7bc3ff4def08e4b7a9de576d26586cec64b6116\n\n");
    print_bytes("tag is", ctx.dat_tag, POLY1305_SIZE);
    printf("mustbe=1ae10b594f09e26a7e902ecbd0600691\n");
    if (cipher_check_aead_tag(&ctx, tag,POLY1305_SIZE))
    {
        printf("TAG IS CORRECT\n");
    }
    else
    {
        printf("TAG IS INCORRECT\n");
    }

    cipher_end(&ctx);

    printf("\n");
}

/* -------------------------------------- */

int main(void)
{
    fprintf(stdout,"\n== CRYPTO TEST START \n\n");

    
    test_aria();
    

    test_aes();
    test_aes_ctr();
    test_aes_gcm();    

    test_blowfish();

    test_camellia();
    test_camellia_ctr();

    test_des();      
    
    test_rc4();

    test_salsa20();

    test_chacha();
    test_chacha20();
    test_chacha20poly1305();

    test_twofish();
    test_xtea();    

    fprintf(stdout,"\n== CRYPTO TEST END\n\n");

    return 0;
}