/*
    test_hashes.c

    Test of entropy routines

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

#include "hash.h"

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


/* --------------- */

void test_sha1(void)
{
    sha1_t ctx;
    hash_t h;
    unsigned char tmp[SHA1_SIZE], test[1024];
    char final[BUF_SIZE_SHA1];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test SHA-1 ****\n\n");
    sha1_init(&ctx);
    sha1_update(&ctx, "", 0);
    sha1_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA1, tmp, SHA1_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=da39a3ee5e6b4b0d3255bfef95601890afd80709\n\n");

    calc_hash(HASH_SHA1, ptr, strlen(ptr), tmp);
    hex_to_char(final, BUF_SIZE_SHA1, tmp, SHA1_SIZE, FALSE);
    printf("Contens=%s\n", ptr);
    printf("Result =%s\n", final);
    printf("Must Be=2fd4e1c67a2d28fced849ee1bb76e7391b93eb12\n\n");

    hash_init(&h, HASH_SHA1, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, SHA1_SIZE);
    printf("Must Be=da39a3ee5e6b4b0d3255bfef95601890afd80709\n\n");

    hash_init(&h, HASH_SHA1, NULL, 0);
    hash_update(&h, ptr, strlen(ptr));
    hash_final(&h, final);
    printf("Contens=%s\n", ptr);
    print_bytes("Result ", final, SHA1_SIZE);
    printf("Must Be=2fd4e1c67a2d28fced849ee1bb76e7391b93eb12\n\n");

    calc_hmac(HASH_SHA1, "", 0, "", 0, tmp);
    print_bytes("HMAC-SHA1", tmp, SHA1_SIZE);
    printf("Must Be  =fbdb1d1b18aa6c08324b7d64b71fb76370690e1d\n\n");

    calc_hmac(HASH_SHA1, "key", 3, ptr, strlen(ptr), tmp);
    print_bytes("HMAC-SHA1", tmp, SHA1_SIZE);
    printf("Must Be  =de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9\n\n");

    memset(test, 0x0b, 20);
    calc_hmac(HASH_SHA1, test, 20, "Hi There", 8, tmp);
    print_bytes("HMAC-SHA1", tmp, SHA1_SIZE);
    printf("Must Be  =b617318655057264e28bc0b6fb378c8ef146be00\n\n");

    memset(test, 0xaa, 80);
    calc_hmac(HASH_SHA1, test, 80, "Test Using Larger Than Block-Size Key - Hash Key First", 54, tmp);
    print_bytes("HMAC-SHA1", tmp, SHA1_SIZE);
    printf("Must Be  =aa4ae5e15272d00e95705637ce8a3b55ed402112\n\n");

}

static char *sha_key = "key";
static char *sha_tst = "The quick brown fox jumps over the lazy dog";

static char *sha_key2 = "Jefe";
static char *sha_tst2 = "what do ya want for nothing?";

static char *sha_ptr = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

void test_sha224(void)
{
    sha224_t ctx;
    unsigned char tmp[SHA224_SIZE];
    char final[BUF_SIZE_SHA224];

    printf("**** Test SHA-224 ****\n\n");
    sha224_init(&ctx);
    sha224_update(&ctx, "", 0);
    sha224_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f\n\n");

    calc_hash(HASH_SHA224, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525\n\n");

    calc_hash(HASH_SHA224, "abc", 3, tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=abc\n");
    printf("Result =%s\n", final);
    printf("Must Be=23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7\n\n");

    calc_hmac(HASH_SHA224, "", 0, "", 0, tmp);
    printf("Key    =''\n");
    printf("Contens=''\n");
    print_bytes("HMAC-SHA224", tmp, SHA224_SIZE);
    printf("Must Be    =5ce14f72894662213e2748d2a6ba234b74263910cedde2f5a9271524\n\n");

    calc_hmac(HASH_SHA224, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    printf("Key    =%s\n", sha_key);
    printf("Contens=%s\n", sha_tst);
    print_bytes("HMAC-SHA224", tmp, SHA224_SIZE);
    printf("Must Be    =88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69\n\n");

    calc_hmac(HASH_SHA224, sha_key2, strlen(sha_key2), sha_tst2, strlen(sha_tst2), tmp);
    printf("Key    =%s\n", sha_key2);
    printf("Contens=%s\n", sha_tst2);
    print_bytes("HMAC-SHA224", tmp, SHA224_SIZE);
    printf("Must Be    =a30e01098bc6dbbf45690f3a7e9e6d0f8bbea2a39e6148008fd05e44\n\n");

}

void test_sha256(void)
{
    sha256_t ctx;
    unsigned char tmp[SHA256_SIZE];
    char final[BUF_SIZE_SHA256];

    printf("**** Test SHA-256 ****\n\n");
    sha256_init(&ctx);
    sha256_update(&ctx, "", 0);
    sha256_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\n\n");

    calc_hash(HASH_SHA256, sha_ptr, strlen(sha_ptr), tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=%s\n", sha_ptr);
    printf("Result =%s\n", final);
    printf("Must Be=248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1\n\n");

    calc_hash(HASH_SHA256, "abc", 3, tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=abc\n");
    printf("Result =%s\n", final);
    printf("Must Be=ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\n\n");

    calc_hmac(HASH_SHA256, "", 0, "", 0, tmp);
    printf("Key    =''\n");
    printf("Contens=''\n");
    print_bytes("HMAC-SHA256", tmp, SHA256_SIZE);
    printf("Must Be    =b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad\n\n");

    calc_hmac(HASH_SHA256, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    printf("Key    =%s\n", sha_key);
    printf("Contens=%s\n", sha_tst);
    print_bytes("HMAC-SHA256", tmp, SHA256_SIZE);
    printf("Must Be    =f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8\n\n");

    calc_hmac(HASH_SHA256, sha_key2, strlen(sha_key2), sha_tst2, strlen(sha_tst2), tmp);
    printf("Key    =%s\n", sha_key2);
    printf("Contens=%s\n", sha_tst2);
    print_bytes("HMAC-SHA256", tmp, SHA256_SIZE);
    printf("Must Be    =5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843\n\n");

}

void test_sha384(void)
{
    sha384_t ctx;
    unsigned char tmp[SHA384_SIZE];
    char final[BUF_SIZE_SHA384];

    printf("**** Test SHA-384 ****\n\n");
    sha384_init(&ctx);
    sha384_update(&ctx, "", 0);
    sha384_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b\n\n");

    calc_hash(HASH_SHA384, sha_ptr, strlen(sha_ptr), tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=%s\n", sha_ptr);
    printf("Result =%s\n", final);
    printf("Must Be=3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05abfe8f450de5f36bC6b0455a8520bc4e6f5fe95b1fe3c8452b\n\n");

    calc_hash(HASH_SHA384, "abc", 3, tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=abc\n");
    printf("Result =%s\n", final);
    printf("Must Be=cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7\n\n");

    calc_hmac(HASH_SHA384, "", 0, "", 0, tmp);
    printf("Contens    =''\n");
    print_bytes("HMAC-SHA384", tmp, SHA384_SIZE);
    printf("Must Be    =6c1f2ee938fad2e24bd91298474382ca218c75db3d83e114b3d4367776d14d3551289e75e8209cd4b792302840234adc\n\n");

    calc_hmac(HASH_SHA384, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    printf("Key    =%s\n", sha_key);
    printf("Contens=%s\n", sha_tst);
    print_bytes("HMAC-SHA384", tmp, SHA384_SIZE);
    printf("Must Be    =d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237\n\n");

    calc_hmac(HASH_SHA384, sha_key2, strlen(sha_key2), sha_tst2, strlen(sha_tst2), tmp);
    printf("Key    =%s\n", sha_key2);
    printf("Contens=%s\n", sha_tst2);
    print_bytes("HMAC-SHA384", tmp, SHA384_SIZE);
    printf("Must Be    =af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649\n\n");
}

void test_sha512(void)
{
    sha512_t ctx;
    unsigned char tmp[SHA512_SIZE];
    char final[BUF_SIZE_SHA512];

    printf("**** Test SHA-512 ****\n\n");
    sha512_init(&ctx);
    sha512_update(&ctx, "", 0);
    sha512_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e\n\n");

    calc_hash(HASH_SHA512, sha_ptr, strlen(sha_ptr), tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=%s\n", sha_ptr);
    printf("Result =%s\n", final);
    printf("Must Be=204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445\n\n");

    calc_hash(HASH_SHA512, "abc", 3, tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=abc\n");
    printf("Result =%s\n", final);
    printf("Must Be=ddaf35a193617abacc417349ae20413112e6Fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f\n\n");

    calc_hmac(HASH_SHA512, "", 0, "", 0, tmp);
    printf("Key    =''\n");
    printf("Contens=''\n");
    print_bytes("HMAC-SHA512", tmp, SHA512_SIZE);
    printf("Must Be    =b936cee86c9f87aa5d3c6f2e84cb5a4239a5fe50480a6ec66b70ab5b1f4ac6730c6c515421b327ec1d69402e53dfb49ad7381eb067b338fd7b0cb22247225d47\n\n");

    calc_hmac(HASH_SHA512, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    printf("Key    =%s\n", sha_key);
    printf("Contens=%s\n", sha_tst);
    print_bytes("HMAC-SHA512", tmp, SHA512_SIZE);
    printf("Must Be    =b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a\n\n");

    calc_hmac(HASH_SHA512, sha_key2, strlen(sha_key2), sha_tst2, strlen(sha_tst2), tmp);
    printf("Key    =%s\n", sha_key2);
    printf("Contens=%s\n", sha_tst2);
    print_bytes("HMAC-SHA512", tmp, SHA512_SIZE);
    printf("Must Be    =164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737\n\n");

}

void test_sha512_224(void)
{
    sha512_t ctx;
    unsigned char tmp[SHA512_224_SIZE];
    char final[BUF_SIZE_SHA512_224];

    printf("**** Test SHA-512_224 ****\n\n");
    sha512_224_init(&ctx);
    sha512_224_update(&ctx, "", 0);
    sha512_224_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA512_224, tmp, SHA512_224_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4\n\n");

    calc_hash(HASH_SHA512_224, sha_ptr, strlen(sha_ptr), tmp);
    hex_to_char(final, BUF_SIZE_SHA512_224, tmp, SHA512_224_SIZE, FALSE);
    printf("Contens=%s\n", sha_ptr);
    printf("Result =%s\n", final);
    printf("Must Be=e5302d6d54bb242275d1e7622d68df6eb02dedd13f564c13dbda2174\n\n");

    calc_hash(HASH_SHA512_224, "abc", 3, tmp);
    hex_to_char(final, BUF_SIZE_SHA512_224, tmp, SHA512_224_SIZE, FALSE);
    printf("Contens=abc\n");
    printf("Result =%s\n", final);
    printf("Must Be=4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa\n\n");

    calc_hmac(HASH_SHA512_224, "", 0, "", 0, tmp);
    printf("Key    =''\n");
    printf("Contens=''\n");
    print_bytes("HMAC-SHA512_224", tmp, SHA512_224_SIZE);
    printf("Must Be        =62c5386083df6fd3ca009c94c0086b7728f055787791429e1ae6e007\n\n");

    calc_hmac(HASH_SHA512_224, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    printf("Key    =%s\n", sha_key);
    printf("Contens=%s\n", sha_tst);
    print_bytes("HMAC-SHA512_224", tmp, SHA512_224_SIZE);
    printf("Must Be        =f797c5aa83c39f686c5892e15bfac4a4b5421c4c8a12fb34a6701c10\n\n");

    calc_hmac(HASH_SHA512_224, sha_key2, strlen(sha_key2), sha_tst2, strlen(sha_tst2), tmp);
    printf("Key    =%s\n", sha_key2);
    printf("Contens=%s\n", sha_tst2);
    print_bytes("HMAC-SHA512_224", tmp, SHA512_224_SIZE);
    printf("Must Be        =a186461f12da6cb8713dd8d41fddfc62c39c219a5a3f550552c17a07\n\n");

}

void test_sha512_256(void)
{
    sha512_t ctx;
    unsigned char tmp[SHA512_256_SIZE];
    char final[BUF_SIZE_SHA512_256];

    printf("**** Test SHA-512_256 ****\n\n");
    sha512_256_init(&ctx);
    sha512_256_update(&ctx, "", 0);
    sha512_256_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA512_256, tmp, SHA512_256_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a\n\n");

    calc_hash(HASH_SHA512_256, sha_ptr, strlen(sha_ptr), tmp);
    hex_to_char(final, BUF_SIZE_SHA512_256, tmp, SHA512_256_SIZE, FALSE);
    printf("Contens=%s\n", sha_ptr);
    printf("Result =%s\n", final);
    printf("Must Be=bde8e1f9f19bb9fd3406c90ec6bc47bd36d8ada9f11880dbc8a22a7078b6a461\n\n");

    calc_hash(HASH_SHA512_256, "abc", 3, tmp);
    hex_to_char(final, BUF_SIZE_SHA512_256, tmp, SHA512_256_SIZE, FALSE);
    printf("Contens=abc\n");
    printf("Result =%s\n", final);
    printf("Must Be=53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23\n\n");

    calc_hmac(HASH_SHA512_256, "", 0, "", 0, tmp);
    printf("Key    =''\n");
    printf("Contens=''\n");
    print_bytes("HMAC-SHA512_256", tmp, SHA512_256_SIZE);
    printf("Must Be        =3c800ad43216d695e18251795743ef81c446a4ca4f38f48a948b887201ca16f5\n\n");

    calc_hmac(HASH_SHA512_256, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    printf("Key    =%s\n", sha_key);
    printf("Contens=%s\n", sha_tst);
    print_bytes("HMAC-SHA512_256", tmp, SHA512_256_SIZE);
    printf("Must Be        =832e660f415c6b80b144b149f9698cc6dd226d85e8cf2deb8a379be7e22aa824\n\n");

    calc_hmac(HASH_SHA512_256, sha_key2, strlen(sha_key2), sha_tst2, strlen(sha_tst2), tmp);
    printf("Key    =%s\n", sha_key2);
    printf("Contens=%s\n", sha_tst2);
    print_bytes("HMAC-SHA512_256", tmp, SHA512_256_SIZE);
    printf("Must Be        =a544d591d4b71b34398f5be4fd1c0e408df97a2cbcf1546e13498785e8f2a1dd\n\n");

}

void test_sha3_224(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA224_SIZE];
    char final[BUF_SIZE_SHA224];

    printf("**** Test SHA3-224 ****\n\n");
    sha3_224_init(&ctx);
    sha3_update(&ctx, "", 0);
    sha3_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7\n\n");

    calc_hash(HASH_SHA3_224, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=d15dadceaa4d5d7bb3b48f446421d542e08ad8887305e28d58335795\n\n");

    printf("Contens=''\n");
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_SHA3_224, sha_key, strlen(sha_key), "", 0, tmp);
    print_bytes("HMAC-SHA3-224", tmp, SHA224_SIZE);
    printf("Must Be      =8f481e10aa1ab054f9862d9b2c2ec2be515ec8355e60c452eff83efc\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_SHA3_224, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-SHA3-224", tmp, SHA224_SIZE);
    printf("Must Be      =ff6fa8447ce10fb1efdccfe62caf8b640fe46c4fb1007912bf85100f\n\n");


}

void test_sha3_256(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA256_SIZE];
    char final[BUF_SIZE_SHA256];

    printf("**** Test SHA3-256 ****\n\n");
    sha3_256_init(&ctx);
    sha3_update(&ctx, "", 0);
    sha3_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a\n\n");

    calc_hash(HASH_SHA3_256, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_SHA3_256, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-SHA3-256", tmp, SHA256_SIZE);
    printf("Must Be      =8c6e0683409427f8931711b10ca92a506eb1fafa48fadd66d76126f47ac2c333\n\n");

}

void test_sha3_384(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA384_SIZE];
    char final[BUF_SIZE_SHA384];

    printf("**** Test SHA3-384 ****\n\n");
    sha3_384_init(&ctx);
    sha3_update(&ctx, "", 0);
    sha3_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004\n\n");

    calc_hash(HASH_SHA3_384, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_SHA3_384, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-SHA3-384", tmp, SHA384_SIZE);
    printf("Must Be      =aa739ad9fcdf9be4a04f06680ade7a1bd1e01a0af64accb04366234cf9f6934a0f8589772f857681fcde8acc256091a2\n\n");

}

void test_sha3_512(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA512_SIZE];
    char final[BUF_SIZE_SHA512];

    printf("**** Test SHA3-512 ****\n\n");
    sha3_512_init(&ctx);
    sha3_update(&ctx, "", 0);
    sha3_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26\n\n");

    calc_hash(HASH_SHA3_512, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_SHA3_512, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-SHA3-512", tmp, SHA512_SIZE);
    printf("Must Be      =237a35049c40b3ef5ddd960b3dc893d8284953b9a4756611b1b61bffcf53edd979f93547db714b06ef0a692062c609b70208ab8d4a280ceee40ed8100f293063\n\n");

}

void test_keccak_224(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA224_SIZE];
    char final[BUF_SIZE_SHA224];

    printf("**** Test KECCAK-224 ****\n\n");
    keccak_init(&ctx,224);
    keccak_update(&ctx, "", 0);
    keccak_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd\n\n");

    calc_hash(HASH_KECCAK_224, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA224, tmp, SHA224_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=310aee6b30c47350576ac2873fa89fd190cdc488442f3ef654cf23fe\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_KECCAK_224, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-KECCAK-224", tmp, SHA224_SIZE);
    printf("Must Be        =763e70a1ec866fbc1c6e6c398cd6e2383e2ad3aecbb3d6150f1e56fd\n\n");
}

void test_keccak_256(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA256_SIZE];
    char final[BUF_SIZE_SHA256];

    printf("**** Test KECCAK-256 ****\n\n");
    keccak_init(&ctx,256);
    keccak_update(&ctx, "", 0);
    keccak_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470\n\n");

    calc_hash(HASH_KECCAK_256, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=4d741b6f1eb29cb2a9b9911c82f56fa8d73b04959d3d9d222895df6c0b28aa15\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_KECCAK_256, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-KECCAK-256", tmp, SHA256_SIZE);
    printf("Must Be        =74547bc8c8e1ef02aec834ca60ff24cc316d4c2244a360fe17448cb53410bed4\n\n");

}

void test_keccak_384(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA384_SIZE];
    char final[BUF_SIZE_SHA384];

    printf("**** Test KECCAK-384 ****\n\n");
    keccak_init(&ctx,384);
    keccak_update(&ctx, "", 0);
    keccak_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff\n\n");

    calc_hash(HASH_KECCAK_384, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA384, tmp, SHA384_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=283990fa9d5fb731d786c5bbee94ea4db4910f18c62c03d173fc0a5e494422e8a0b3da7574dae7fa0baf005e504063b3\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_KECCAK_384, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-KECCAK-384", tmp, SHA384_SIZE);
    printf("Must Be        =73acb07b5b1db5431758262b55e5923d362de4492229a7420302c80d4348ca1b11ecea06fb1c232f9b832aadca8cd289\n\n");

}

void test_keccak_512(void)
{
    sha3_t ctx;
    unsigned char tmp[SHA512_SIZE];
    char final[BUF_SIZE_SHA512];

    printf("**** Test KECCAK-512 ****\n\n");
    keccak_init(&ctx,512);
    keccak_update(&ctx, "", 0);
    keccak_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e\n\n");

    calc_hash(HASH_KECCAK_512, sha_tst, strlen(sha_tst), tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=%s\n", sha_tst);
    printf("Result =%s\n", final);
    printf("Must Be=d135bb84d0439dbac432247ee573a23ea7d3c9deb2a968eb31d47c4fb45f1ef4422d6c531b5b9bd6f449ebcc449ea94d0a8f05f62130fda612da53c79659f609\n\n");

    printf("Contens='%s'\n", sha_tst);
    printf("Key    ='%s'\n", sha_key);
    calc_hmac(HASH_KECCAK_512, sha_key, strlen(sha_key), sha_tst, strlen(sha_tst), tmp);
    print_bytes("HMAC-KECCAK-512", tmp, SHA512_SIZE);
    printf("Must Be        =22fb03b3391bc0adfc73c18e0919d9f142390e81d6cc2689716ac53ab75458a718059d58cfbb23c6a416c32b8afa84a9a7a9d852312a743bef0a55148e5a1b8a\n\n");

}

void test_hmac_sha3(void)
{
    hmac_t ctx;
    unsigned char tmp[SHA512_SIZE];
    char final[BUF_SIZE_SHA512];
    const char *text = "Sample message for keylen<blocklen";
    unsigned char key[64] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
                             0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
                             0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
                             0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f};


    printf("**** Test HMAC SHA3-256 ****\n\n");

    hmac_init(&ctx,HASH_SHA3_256,key,32);
    hmac_update(&ctx, text, strlen(text));
    hmac_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"%s\"\n",text);
    printf("Result =%s\n", final);
    printf("Must Be=4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205\n\n");

    calc_hmac(HASH_SHA3_256, key, 32, text, strlen(text),tmp);
    hex_to_char(final, BUF_SIZE_SHA256, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"%s\"\n",text);
    printf("Result =%s\n", final);
    printf("Must Be=4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205\n\n");


    printf("**** Test HMAC SHA3-512 ****\n\n");

    hmac_init(&ctx,HASH_SHA3_512,key,64);
    hmac_update(&ctx, text, strlen(text));
    hmac_final(&ctx, tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=\"%s\"\n",text);
    printf("Result =%s\n", final);
    printf("Must Be=4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196\n\n");

    calc_hmac(HASH_SHA3_512, key, 64, text, strlen(text),tmp);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=\"%s\"\n",text);
    printf("Result =%s\n", final);
    printf("Must Be=4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196\n\n");
}


void test_shake(void)
{
    shake_t ctx;
    unsigned char tmp[SHA512_SIZE];
    char final[BUF_SIZE_SHA512];
    hash_t hct;


    printf("**** Test SHAKE 128 ****\n\n");

    shake_init_hkdf(&ctx,128,"",0);
    shake_get(&ctx, tmp,SHA256_SIZE);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26\n\n");
    shake_clean(&ctx);

    printf("**** Test SHAKE 256 ****\n\n");

    shake_init_hkdf(&ctx,256,"",0);
    shake_get(&ctx, tmp,SHA512_SIZE);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA512_SIZE, FALSE);
    printf("Contens=\"\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4ddd8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be\n\n");
    shake_clean(&ctx);

    printf("**** Test SHAKE 128 ****\n\n");


    hash_init(&hct,HASH_SHAKE_128,"The quick brown fox jumps over the lazy dog",43);
    hash_final_hkdf(&hct, tmp,SHA256_SIZE);
    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"The quick brown fox jumps over the lazy dog\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e\n\n");

    printf("**** Test SHAKE 128 ****\n\n");

    hash_init(&hct,HASH_SHAKE_128,NULL,0);
    hash_update(&hct,"The quick brown fox jumps over the lazy dof",43);
    hash_final_hkdf(&hct, tmp,SHA256_SIZE);

    hex_to_char(final, BUF_SIZE_SHA512, tmp, SHA256_SIZE, FALSE);
    printf("Contens=\"The quick brown fox jumps over the lazy dof\"\n");
    printf("Result =%s\n", final);
    printf("Must Be=853f4538be0db9621a6cea659a06c1107b1f83f02b13d18297bd39d7411cf10c\n\n");

}



/* --------------- */

void test_sha(void)
{
    test_sha1();

    test_sha224();
    test_sha256();
    test_sha384();
    test_sha512();
    test_sha512_224();
    test_sha512_256();

    test_sha3_224();
    test_sha3_256();
    test_sha3_384();
    test_sha3_512();

    test_keccak_224();
    test_keccak_256();
    test_keccak_384();
    test_keccak_512();

    test_hmac_sha3();
    test_shake();
}


/* --------------- */

void test_md2(void)
{
    md2_t ctx;
    hash_t h;
    unsigned char final[MD2_SIZE];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test MD2 ****\n\n");
    md2_init(&ctx);
    md2_update(&ctx, "", 0);
    md2_final(&ctx, final);
    printf("Contens : \"\"\n");
    print_bytes("Result", final, MD2_SIZE);
    printf("MustBe=8350e5a3e24c153df2275c9f80692773\n");

    calc_hash(HASH_MD2, ptr, strlen(ptr), final);
    printf("Contens : %s\n", ptr);
    print_bytes("Result", final, MD2_SIZE);
    printf("MustBe=03d85a0d629d2c442e987525319fc471\n");

    hash_init(&h, HASH_MD2, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens : \"\"\n");
    print_bytes("Result", final, MD2_SIZE);
    printf("MustBe=8350e5a3e24c153df2275c9f80692773\n");

    hash_init(&h, HASH_MD2,NULL,  0);
    hash_update(&h, ptr, strlen(ptr));
    hash_final(&h, final);
    printf("Contens : %s\n", ptr);
    print_bytes("Result", final, MD2_SIZE);
    printf("MustBe=03d85a0d629d2c442e987525319fc471\n");

    printf("\n");
}

/* --------------- */

void test_md4(void)
{
    md4_t ctx;
    hash_t h;
    unsigned char final[MD4_SIZE];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test MD4 ****\n\n");
    md4_init(&ctx);
    md4_update(&ctx, "", 0);
    md4_final(&ctx, final);
    printf("Contens : \"\"\n");
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=31d6cfe0d16ae931b73c59d7e0c089c0\n");

    calc_hash(HASH_MD4, ptr, strlen(ptr), final);
    printf("Contens : %s\n", ptr);
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=1bee69a46ba811185c194762abaeae90\n");

    hash_init(&h, HASH_MD4,NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens : \"\"\n");
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=31d6cfe0d16ae931b73c59d7e0c089c0\n");

    hash_init(&h, HASH_MD4, NULL, 0);
    hash_update(&h, ptr, strlen(ptr));
    hash_final(&h, final);
    printf("Contens : %s\n", ptr);
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=1bee69a46ba811185c194762abaeae90\n");

    calc_hash(HASH_MD4, "a", 1, final);
    printf("Contens : a\n");
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=bde52cb31de33e46245e05fbdbd6fb24\n");

    calc_hash(HASH_MD4, "abc", 3, final);
    printf("Contens : abc\n");
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=a448017aaf21d8525fc10ae87aa6729d\n");

    calc_hash(HASH_MD4, "message digest", 14, final);
    printf("Contens : message digest\n");
    print_bytes("Result", final, MD4_SIZE);
    printf("MustBe=d9130a8164549fe818874806e1c7014b\n");

    printf("\n");
}

/* --------------- */

void test_md5(void)
{
    md5_t ctx;
    hash_t h;
    unsigned char final[MD5_SIZE];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test MD5 ****\n\n");
    md5_init(&ctx);
    md5_update(&ctx, "", 0);
    md5_final(&ctx, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, MD5_SIZE);
    printf("Must Be=d41d8cd98f00b204e9800998ecf8427e\n\n");

    calc_hash(HASH_MD5, ptr, strlen(ptr), final);
    printf("Contens=%s\n", ptr);
    print_bytes("Result ", final, MD5_SIZE);
    printf("Must Be=9e107d9d372bb6826bd81d3542a419d6\n\n");

    hash_init(&h, HASH_MD5, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, MD5_SIZE);
    printf("Must Be=d41d8cd98f00b204e9800998ecf8427e\n\n");

    hash_init(&h, HASH_MD5, NULL, 0);
    hash_update(&h, ptr, strlen(ptr));
    hash_final(&h, final);
    printf("Contens=%s\n", ptr);
    print_bytes("Result ", final, MD5_SIZE);
    printf("Must Be=9e107d9d372bb6826bd81d3542a419d6\n\n");

    calc_hmac(HASH_MD5, "", 0, "", 0, final);
    print_bytes("HMAC-MD5", final, MD5_SIZE);
    printf("Must Be =74e6f7298a9c2d168935f58c001bad88\n\n");

    calc_hmac(HASH_MD5, "key", 3, ptr, strlen(ptr), final);
    print_bytes("HMAC-MD5", final, MD5_SIZE);
    printf("Must Be =80070713463e7749b90c2dc24911e275\n\n");
}

/* --------------- */

void test_ripe160(void)
{    
    unsigned char test[1024];
    hash_t h;
    unsigned char final[RIPE160_SIZE];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test RipeMD 160 ****\n\n");

    calc_hash(HASH_RIPE_160, ptr, strlen(ptr), final);
    printf("Contens=%s\n", ptr);
    print_bytes("Result ", final, RIPE160_SIZE);
    printf("Must Be=37f332f68db77bd9d7edd4969571ad671cf9dd3b\n\n");

    hash_init(&h, HASH_RIPE_160, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, RIPE160_SIZE);
    printf("Must Be=9c1185a5c5e9fc54612808977ee8f548b2258d31\n\n");

    memset(test, 0x0b, 20);
    calc_hmac(HASH_RIPE_160, test, 20, "Hi There", 8, final);
    print_bytes("HMAC-RIPE160", final, RIPE160_SIZE);
    printf("Must Be     =24cb4bd67d20fc1a5d2ed7732dcc39377f0a5668\n\n");

    memset(test, 0xaa, 80);
    calc_hmac(HASH_RIPE_160, test, 80, "Test Using Larger Than Block-Size Key - Hash Key First", 54, final);
    print_bytes("HMAC-RIPE160", final, RIPE160_SIZE);
    printf("Must Be     =6466ca07ac5eac29e1bd523e5ada7605b791fd8b\n\n");

}

/* --------------- */

void test_ripe320(void)
{    
    unsigned char test[1024];
    hash_t h;
    unsigned char final[RIPE320_SIZE];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test RipeMD 320 ****\n\n");

    calc_hash(HASH_RIPE_320, ptr, strlen(ptr), final);
    printf("Contens=%s\n", ptr);
    print_bytes("Result ", final, RIPE320_SIZE);
    printf("Must Be=e7660e67549435c62141e51c9ab1dcc3b1ee9f65c0b3e561ae8f58c5dba3d21997781cd1cc6fbc34\n\n");

    hash_init(&h, HASH_RIPE_320, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, RIPE320_SIZE);
    printf("Must Be=22d65d5661536cdc75c1fdf5c6de7b41b9f27325ebc61e8557177d705a0ec880151c3a32a00899b8\n\n");

    memset(test, 0x0b, 20);
    calc_hmac(HASH_RIPE_320, test, 20, "Hi There", 8, final);
    print_bytes("HMAC-RIPE320", final, RIPE320_SIZE);
    printf("Must Be     =\n\n");

    memset(test, 0xaa, 80);
    calc_hmac(HASH_RIPE_320, test, 80, "Test Using Larger Than Block-Size Key - Hash Key First", 54, final);
    print_bytes("HMAC-RIPE320", final, RIPE320_SIZE);
    printf("Must Be     =\n\n");

}

/* --------------- */

void test_ripe256(void)
{    
    unsigned char test[1024];
    hash_t h;
    unsigned char final[RIPE256_SIZE];
    char *ptr = "The quick brown fox jumps over the lazy dog";

    printf("**** Test RipeMD 256 ****\n\n");

    calc_hash(HASH_RIPE_256, ptr, strlen(ptr), final);
    printf("Contens=%s\n", ptr);
    print_bytes("Result ", final, RIPE256_SIZE);
    printf("Must Be=c3b0c2f764ac6d576a6c430fb61a6f2255b4fa833e094b1ba8c1e29b6353036f\n\n");

    hash_init(&h, HASH_RIPE_256, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, RIPE256_SIZE);
    printf("Must Be=02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d\n\n");

    memset(test, 0x0b, 20);
    calc_hmac(HASH_RIPE_256, test, 20, "Hi There", 8, final);
    print_bytes("HMAC-RIPE256", final, RIPE256_SIZE);
    printf("Must Be     =\n\n");

    memset(test, 0xaa, 80);
    calc_hmac(HASH_RIPE_256, test, 80, "Test Using Larger Than Block-Size Key - Hash Key First", 54, final);
    print_bytes("HMAC-RIPE320", final, RIPE256_SIZE);
    printf("Must Be     =\n\n");

}

/* --------------- */

void test_ripe128(void)
{
     unsigned char test[1024];
    hash_t h;
    unsigned char final[RIPE128_SIZE];

    printf("**** Test RipeMD 128 ****\n\n");

    calc_hash(HASH_RIPE_128, "a", 1, final);
    printf("Contens=\"a\"\n");
    print_bytes("Result ", final, RIPE128_SIZE);
    printf("Must Be=86be7afa339d0fc7cfc785e72f578d33\n\n");

    hash_init(&h, HASH_RIPE_128, NULL, 0);
    hash_update(&h, "", 0);
    hash_final(&h, final);
    printf("Contens=\"\"\n");
    print_bytes("Result ", final, RIPE128_SIZE);
    printf("Must Be=cdf26213a150dc3ecb610f18f6b38b46\n\n");

    memset(test, 0x0b, 16);
    calc_hmac(HASH_RIPE_128, test, 16, "Hi There", 8, final);
    print_bytes("HMAC-RIPE128", final, RIPE128_SIZE);
    printf("Must Be     =fbf61f9492aa4bbf81c172e84e0734db\n\n");

    memset(test, 0xaa, 80);
    calc_hmac(HASH_RIPE_128, test, 80, "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", 73, final);
    print_bytes("HMAC-RIPE128", final, RIPE128_SIZE);
    printf("Must Be     =5c6bec96793e16d40690c237635f30c5\n\n");

}

void test_poly1305(void)
{
    hmac_t hmac;
    poly1305_t ctx;
    char final[BUF_SIZE_POLY1305];
    unsigned char key1[POLY1305_KEY_SIZE] =
        {
            0x85, 0xd6, 0xbe, 0x78, 0x57, 0x55, 0x6d, 0x33,
            0x7f, 0x44, 0x52, 0xfe, 0x42, 0xd5, 0x06, 0xa8,
            0x01, 0x03, 0x80, 0x8a, 0xfb, 0x0d, 0xb2, 0xfd,
            0x4a, 0xbf, 0xf6, 0xaf, 0x41, 0x49, 0xf5, 0x1b};
    unsigned char key2[POLY1305_KEY_SIZE] =
        {
            0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a,
            0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0,
            0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09,
            0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0};

    unsigned char tmp[POLY1305_SIZE];
    char *plain1 = "Cryptographic Forum Research Group";
    char *plain2 = "'Twas brillig, and the slithy toves\nDid gyre and gimble in the wabe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.";
    size_t tam = strlen(plain2);

    printf("**** Test Poly1305 ****\n\n");

    poly1305_init(&ctx, key1);
    poly1305_update(&ctx, plain1, 34);
    poly1305_final(&ctx, final);

    print_bytes("Contens", plain1, 34);
    print_bytes("Result ", final, POLY1305_SIZE);
    printf("Must Be=a8061dc1305136c6c22b8baf0c0127a9\n\n");

    poly1305_init(&ctx, key2);
    poly1305_update(&ctx, plain2, tam);
    poly1305_final(&ctx, final);

    print_bytes("Contens", plain2, tam);
    print_bytes("Result ", final, POLY1305_SIZE);
    printf("Must Be=4541669a7eaaee61e708dc7cbcc5eb62\n\n");

    calc_hmac(HASH_POLY1305, key1, POLY1305_KEY_SIZE,plain1,34,final);
    print_bytes("Contens", plain1, 34);
    print_bytes("HMAC is", final, POLY1305_SIZE);
    printf("Must Be=a8061dc1305136c6c22b8baf0c0127a9\n\n");

    hmac_init(&hmac,HASH_POLY1305,key2,POLY1305_KEY_SIZE);
    hmac_update(&hmac,plain2,tam);
    hmac_final(&hmac, tmp);
    hex_to_char(final, BUF_SIZE_POLY1305, tmp, POLY1305_SIZE, FALSE);
    print_bytes("Contens", plain2, tam);
    print_bytes("HMAC is", tmp, POLY1305_SIZE);
    printf("Must Be=4541669a7eaaee61e708dc7cbcc5eb62\n\n");

    printf("\n");
}

/* -------------------------------------- */
/* -------------------------------------- */

int main(void)
{
    fprintf(stdout,"\n== HASHES TEST START \n\n");

    test_md2();
    test_md4();
    test_md5();

    test_sha();

    test_ripe128();
    test_ripe160();

    test_ripe256();
    test_ripe320();

    test_poly1305();

    fprintf(stdout,"\n== HASHES TEST END\n\n");
}