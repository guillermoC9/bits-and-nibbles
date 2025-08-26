/*
    test_ecc.c

    Test of Elliptic Curve Criptography

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

#include "ecc.h"
#include "random.h"

/* -------------------------------------- */

void print_bytes(char *prefix, void *d, size_t n)
{
    size_t t;
    unsigned char *dat;

    dat = (unsigned char *)d;

    if (prefix)
        printf("%s = ", prefix);

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

void print_bytes_hexa(char *prefix, void *d, size_t n)
{
    size_t t;
    unsigned char *dat;

    dat = (unsigned char *)d;

    if (prefix)
        printf("%s = 0x", prefix);

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

static void print_ecc_key(ecc_key_t *key)
{
    char tmp[ECC_MAX_HEX_BUFFER];
    int ret;

    if (!key)
    {
        printf("\n--- ECC KEY NULL ---\n\n");
        return;
    }

    printf("\n--- ECC KEY %s %d bits ---\n\n", key->ctx->alias[0], key->ctx->NUMBITS);
    ret = ecc_point_to_bytes(key->ctx, &(key->pub), tmp, ECC_MAX_HEX_BUFFER, FALSE);
    print_bytes("PUB", tmp, ret);
    if (key->pri)
    {
        mp_show("PRI", key->pri);        
    }
    printf("-----------------------------\n");
}



/* ------------------------------- */

void test_curve_25519(void)
{
    mp_int_t pri1,pri2,pub1,pub2;
    ecc_dh_t *alice,*bob;
    ecc_curve_t *curve;
    int ret;
    unsigned char alice_key[CURVE_25519_POINT_BYTES]={0};
    unsigned char alice_pub[CURVE_25519_POINT_BYTES + 1]={0};
    unsigned char alice_pri[CURVE_25519_POINT_BYTES]=
        {
            0x77,0x07,0x6d,0x0a,0x73,0x18,0xa5,0x7d,0x3c,0x16,0xc1,0x72,0x51,0xb2,0x66,0x45,
            0xdf,0x4c,0x2f,0x87,0xeb,0xc0,0x99,0x2a,0xb1,0x77,0xfb,0xa5,0x1d,0xb9,0x2c,0x2a
        };

    unsigned char bob_key[CURVE_25519_POINT_BYTES]={0};
    unsigned char bob_pub[CURVE_25519_POINT_BYTES + 1]={0};
    unsigned char bob_pri[CURVE_25519_POINT_BYTES]=
        {
            0x5d,0xab,0x08,0x7e,0x62,0x4a,0x8a,0x4b,0x79,0xe1,0x7f,0x8b,0x83,0x80,0x0e,0xe6,
            0x6f,0x3b,0xb1,0x29,0x26,0x18,0xb6,0xfd,0x1c,0x2f,0x8b,0x27,0xff,0x88,0xe0,0xeb
        };

    printf("===== TEST CURVE 25519 =====\n\n");

    mp_init(&pri1);
    mp_init(&pri2);
    mp_init(&pub1);
    mp_init(&pub2);

    mp_set_string(&pri1,"a546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4",16);
    mp_set_string(&pri2,"e6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c",16);

    curve25519_scalarmult(&pri1,&pri2,&pub1);

    mp_show("Scalar 1 ", &pri1);
    printf("It Must Be: 0xa546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4\n");
    mp_show("Coord  1 ", &pri2);
    printf("It Must Be: 0xe6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c\n");
    mp_show("Result 1 ", &pub1);
    printf("It Must Be: 0xc3da55379de9c6908e94ea4df28d084f32eccf03491c71f754b4075577a28552\n\n");

    mp_set_string(&pri1,"4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d",16);
    mp_set_string(&pri2,"e5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493",16);

    curve25519_scalarmult(&pri1,&pri2,&pub2);

    mp_show("Scalar 2 ", &pri1);
    printf("It Must Be: 0x4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d\n");
    mp_show("Coord  2 ", &pri2);
    printf("It Must Be: 0xe5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493\n");
    mp_show("Result 2 ", &pub2);
    printf("It Must Be: 0x95cbde9476e8907d7aade45cb4b873f88b595a68799fa152e6f8f7647aac7957\n\n");

    printf("\nCurve 25519 Diffie-Hellman:\n");

    curve = ecc_get_curve(ECC_CURVE_X25519);

    alice = ecc_dh_start_from_bytes(curve,NULL,0,alice_pri,CURVE_25519_POINT_BYTES);
    ret = ecc_dh_get_public(alice,alice_pub,CURVE_25519_POINT_BYTES + 1,TRUE);
    printf("    Generating Alice Keys (%d bytes)\n",ret);
    print_bytes("      Private",alice_pri,CURVE_25519_POINT_BYTES);
    printf("      Must Be = 77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a\n\n");
    print_bytes("       Public",alice_pub,ret);
    printf("      Must Be = a0e1a5c720fa1adad858c05b7e02cab912f59bf95abe4fe0db47c234dd2b2c21\n\n");


    bob = ecc_dh_start_from_bytes(curve,NULL,0,bob_pri,CURVE_25519_POINT_BYTES);
    ret = ecc_dh_get_public(bob,bob_pub,CURVE_25519_POINT_BYTES + 1,TRUE);
    printf("    Generating Bob Keys (%d bytes)\n",ret);
    print_bytes("      Private",bob_pri,CURVE_25519_POINT_BYTES);
    printf("      Must Be = 5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb\n\n");
    print_bytes("       Public",bob_pub,ret);
    printf("      Must Be = 87b3ccf50ce25da6ab7053e68c6eb5f7ea0969b51df6b84c60cdd7be15798404\n\n");

    ecc_dh_end(alice,bob_pub,ret,alice_key);
    ecc_dh_end(bob,alice_pub,ret,bob_key);

    printf("    Generated Secret (%d bytes)\n",CURVE_25519_POINT_BYTES);
    print_bytes("        Alice",alice_key,CURVE_25519_POINT_BYTES);
    print_bytes("          Bob",bob_key,CURVE_25519_POINT_BYTES);
    printf("      Must Be = 168579f5ac2414fe251e1365ec6c64d02d951a503a27dabeea6908da4e24f109\n\n");

    mp_clear(&pri1);
    mp_clear(&pri2);
    mp_clear(&pub1);
    mp_clear(&pub2);
}

/* ------------------------------- */

void test_curve_448(void)
{
    mp_int_t pri1,pri2,pub1,pub2;
    ecc_dh_t *alice,*bob;
    ecc_curve_t *curve;
    int ret;
    unsigned char alice_key[CURVE_448_POINT_BYTES]={0};
    unsigned char alice_pub[CURVE_448_POINT_BYTES + 1]={0};
    unsigned char alice_pri[CURVE_448_POINT_BYTES]=
        {
            0x9a,0x8f,0x49,0x25,0xd1,0x51,0x9f,0x57,0x75,0xcf,0x46,0xb0,0x4b,0x58,0x00,0xd4,
            0xee,0x9e,0xe8,0xba,0xe8,0xbc,0x55,0x65,0xd4,0x98,0xc2,0x8d,0xd9,0xc9,0xba,0xf5,
            0x74,0xa9,0x41,0x97,0x44,0x89,0x73,0x91,0x00,0x63,0x82,0xa6,0xf1,0x27,0xab,0x1d,
            0x9a,0xc2,0xd8,0xc0,0xa5,0x98,0x72,0x6b

            //0xf5,0xd2,0xf7,0x9f,0x95,0xa3,0xc6,0x81,0x74,0x2a,0x08,0x80,0x38,0x96,0x1b,0x6b,
            //0x37,0x1f,0x8f,0x2b,0xc0,0x81,0x39,0xa1,0xc5,0xe3,0xfe,0xe3,0x85,0x81,0xd9,0x21,
            //0x8f,0x49,0x6b,0x7b,0x0b,0x58,0x24,0x80,0x3a,0xdf,0x01,0x1a,0x0f,0x78,0xc3,0xf7,
            //0x4d,0x4d,0x5e,0xe8,0x38,0x1e,0xc9,0x98
        };

    unsigned char bob_key[CURVE_448_POINT_BYTES]={0};
    unsigned char bob_pub[CURVE_448_POINT_BYTES + 1]={0};
    unsigned char bob_pri[CURVE_448_POINT_BYTES]=
        {
            0x1c,0x30,0x6a,0x7a,0xc2,0xa0,0xe2,0xe0,0x99,0x0b,0x29,0x44,0x70,0xcb,0xa3,0x39,
            0xe6,0x45,0x37,0x72,0xb0,0x75,0x81,0x1d,0x8f,0xad,0x0d,0x1d,0x69,0x27,0xc1,0x20,
            0xbb,0x5e,0xe8,0x97,0x2b,0x0d,0x3e,0x21,0x37,0x4c,0x9c,0x92,0x1b,0x09,0xd1,0xb0,
            0x36,0x6f,0x10,0xb6,0x51,0x73,0x99,0x2d

            //0x0c,0x14,0x64,0x56,0x29,0x30,0x3e,0xc8,0x3e,0xac,0x91,0x52,0x04,0x66,0x89,0xc4,
            //0xf5,0xf4,0x4d,0x7c,0xea,0x9a,0x6b,0x9f,0xce,0x5a,0x7b,0xcf,0xd4,0x6e,0xa5,0xde,
            //0x02,0x7e,0x20,0x47,0x51,0x90,0x67,0x3e,0x10,0xdc,0xa4,0x1f,0x92,0x78,0xb7,0x22,
            //0x92,0x4a,0x05,0x62,0x0b,0x27,0x5e,0x92
        };

    printf("===== TEST CURVE 448 =====\n\n");

    mp_init(&pri1);
    mp_init(&pri2);
    mp_init(&pub1);
    mp_init(&pub2);

    mp_set_string(&pri1,"3d262fddf9ec8e88495266fea19a34d28882acef045104d0d1aae121700a779c984c24f8cdd78fbff44943eba368f54b29259a4f1c600ad3",16);
    mp_set_string(&pri2,"06fce640fa3487bfda5f6cf2d5263f8aad88334cbd07437f020f08f9814dc031ddbdc38c19c6da2583fa5429db94ada18aa7a7fb4ef8a086",16);

    curve448_scalarmult(&pri1,&pri2,&pub1);

    mp_show("Scalar 1 ", &pri1);
    printf("It Must Be: 0x3d262fddf9ec8e88495266fea19a34d28882acef045104d0d1aae121700a779c984c24f8cdd78fbff44943eba368f54b29259a4f1c600ad3\n");
    mp_show("Coord  1 ", &pri2);
    printf("It Must Be: 0x6fce640fa3487bfda5f6cf2d5263f8aad88334cbd07437f020f08f9814dc031ddbdc38c19c6da2583fa5429db94ada18aa7a7fb4ef8a086\n");

    mp_show("Result 1 ", &pub1);
    printf("It Must Be: 0xce3e4ff95a60dc6697da1db1d85e6afbdf79b50a2412d7546d5f239fe14fbaadeb445fc66a01b0779d98223961111e21766282f73dd96b6f\n\n");

    mp_set_string(&pri1,"203d494428b8399352665ddca42f9de8fef600908e0d461cb021f8c538345dd77c3e4806e25f46d3315c44e0a5b4371282dd2c8d5be3095f",16);
    mp_set_string(&pri2,"0fbcc2f993cd56d3305b0b7d9e55d4c1a8fb5dbb52f8e9a1e9b6201b165d015894e56c4d3570bee52fe205e28a78b91cdfbde71ce8d157db",16);

    curve448_scalarmult(&pri1,&pri2,&pub2);

    mp_show("Scalar 2 ", &pri1);
    printf("It Must Be: 0x203d494428b8399352665ddca42f9de8fef600908e0d461cb021f8c538345dd77c3e4806e25f46d3315c44e0a5b4371282dd2c8d5be3095f\n");
    mp_show("Coord  2 ", &pri2);
    printf("It Must Be: 0xfbcc2f993cd56d3305b0b7d9e55d4c1a8fb5dbb52f8e9a1e9b6201b165d015894e56c4d3570bee52fe205e28a78b91cdfbde71ce8d157db\n");

    mp_show("Result 2 ", &pub2);
    printf("It Must Be: 0x884a02576239ff7a2f2f63b2db6a9ff37047ac13568e1e30fe63c4a7ad1b3ee3a5700df34321d62077e63633c575c1c954514e99da7c179d\n\n");

    printf("\nCurve 448 Diffie-Hellman:\n");

    curve = ecc_get_curve(ECC_CURVE_X448);

    alice = ecc_dh_start_from_bytes(curve,NULL,0,alice_pri,CURVE_448_POINT_BYTES);
    ret = ecc_dh_get_public(alice,alice_pub,CURVE_448_POINT_BYTES + 1,TRUE);
    printf("    Generating Alice Keys (%d bytes)\n",ret);
    print_bytes("      Private",alice_pri,CURVE_448_POINT_BYTES);
    printf("      Must Be = 9a8f4925d1519f5775cf46b04b5800d4ee9ee8bae8bc5565d498c28dd9c9baf574a9419744897391006382a6f127ab1d9ac2d8c0a598726b\n\n");

    print_bytes("       Public",alice_pub,ret);
    printf("      Must Be = f9e60c4f68e01bc3c913c53f6d52efa73d49ef91a519e83266aa9d6f9af2091663efbdf79a01597c446cc2fbeb32da9b663273651cb00170\n\n");


    bob = ecc_dh_start_from_bytes(curve,NULL,0,bob_pri,CURVE_448_POINT_BYTES);
    ret = ecc_dh_get_public(bob,bob_pub,CURVE_448_POINT_BYTES + 1,TRUE);
    printf("    Generating Bob Keys (%d bytes)\n",ret);
    print_bytes("      Private",bob_pri,CURVE_448_POINT_BYTES);
    printf("      Must Be = 1c306a7ac2a0e2e0990b294470cba339e6453772b075811d8fad0d1d6927c120bb5ee8972b0d3e21374c9c921b09d1b0366f10b65173992d\n\n");
    print_bytes("       Public",bob_pub,ret);
    printf("      Must Be = 38475a2a6de13fcca293e78150a3b0144b504543e5196c4710186fe05e0863854224184a5a02d20d6ed51da965389ba69300164d367102e8\n\n");


    ecc_dh_end(alice,bob_pub,ret,alice_key);
    ecc_dh_end(bob,alice_pub,ret,bob_key);

    printf("    Generated Secret (%d bytes)\n",CURVE_448_POINT_BYTES);
    print_bytes("        Alice",alice_key,CURVE_448_POINT_BYTES);
    print_bytes("          Bob",bob_key,CURVE_448_POINT_BYTES);
    printf("      Must Be = 8bac1028804712ead5b64ce695dc08fb1432cb5f71faf0eca71f0dd5bfdd1d0b4bb7bb0307669f89f18d7845c7e35dc9c79f23b837f63cd4\n\n");


    mp_clear(&pri1);
    mp_clear(&pri2);
    mp_clear(&pub1);
    mp_clear(&pub2);

}

/* --------------------------------------------- *

    Test Vectors por NIST Prime Curves from the
    Document:

    Mathematical routines for the NIST prime
    elliptic curves - April 2010

    https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.204.9073&rep=rep1&type=pdf

 * --------------------------------------------- */

/* Point S */

enum {p192 = 0, p256, p384, p521, pMAX};


static int  curves[pMAX]={ECC_CURVE_192r1,ECC_CURVE_256r1,ECC_CURVE_384r1,ECC_CURVE_521r1};


const char *sX[pMAX] =
            {
                "d458e7d127ae671b0c330266d246769353a012073e97acf8",
                "de2444bebc8d36e682edd27e0f271508617519b3221a8fa0b77cab3989da97c9",
                "fba203b81bbd23f2b3be971cc23997e1ae4d89e69cb6f92385dda82768ada415ebab4167459da98e62b1332d1e73cb0e",
                "000001d5c693f66c08ed03ad0f031f937443458f601fd098d3d0227b4bf62873af50740b0bb84aa157fc847bcf8dc16a8b2b8bfd8e2d0a7d39af04b089930ef6dad5c1b4",
            };


const char *sY[pMAX] =
            {
                "325930500d851f336bddc050cf7fb11b5673a1645086df3b",
                "c093ae7ff36e5380fc01a5aad1e66659702de80f53cec576b6350b243042a256",
                "5ffedbaefdeba603e7923e06cdb5d0c65b22301429293376d5c6944e3fa6259f162b4788de6987fd59aed5e4b5285e45",
                "00000144b7770963c63a39248865ff36b074151eac33549b224af5c8664c54012b818ed037b2b7c1a63ac89ebaa11e07db89fcee5b556e49764ee3fa66ea7ae61ac01823",
            };

/* Point T */

const char *tX[pMAX] =
            {
                "f22c4395213e9ebe67ddecdd87fdbd01be16fb059b9753a4",
                "55a8b00f8da1d44e62f6b3b25316212e39540dc861c89575bb8cf92e35e0986b",
                "aacc05202e7fda6fc73d82f0a66220527da8117ee8f8330ead7d20ee6f255f582d8bd38c5a7f2b40bcdb68ba13d81051",
                "000000f411f2ac2eb971a267b80297ba67c322dba4bb21cec8b70073bf88fc1ca5fde3ba09e5df6d39acb2c0762c03d7bc224a3e197feaf760d6324006fe3be9a548c7d5",
            };

const char *tY[pMAX] =
            {
                "264424096af2b3597796db48f8dfb41fa9cecc97691a9c79",
                "5421c3209c2d6c704835d82ac4c3dd90f61a8a52598b9e7ab656e9d8c8b24316",
                "84009a263fefba7c2c57cffa5db3634d286131afc0fca8d25afa22a7b5dce0d9470da89233cee178592f49b6fecb5092",
                "000001fdf842769c707c93c630df6d02eff399a06f1b36fb9684f0b373ed064889629abb92b1ae328fdb45534268384943f0e9222afe03259b32274d35d1b9584c65e305",
            };

/* Point R = S + T*/

const char *rX[pMAX] =
            {
                "48e1e4096b9b8e5ca9d0f1f077b8abf58e843894de4d0290",
                "72b13dd4354b6b81745195e98cc5ba6970349191ac476bd4553cf35a545a067e",
                "12dc5ce7acdfc5844d939f40b4df012e68f865b89c3213ba97090a247a2fc009075cf471cd2e85c489979b65ee0b5eed",
                "000001264ae115ba9cbc2ee56e6f0059e24b52c8046321602c59a339cfb757c89a59c358a9a8e1f86d384b3f3b255ea3f73670c6dc9f45d46b6a196dc37bbe0f6b2dd9e9",
            };

const char *rY[pMAX] =
            {
                "408fa77c797cd7dbfb16aa48a3648d3d63c94117d7b6aa4b",
                "8d585cbb2e1327d75241a8a122d7620dc33b13315aa5c9d46d013011744ac264",
                "167312e58fe0c0afa248f2854e3cddcb557f983b3189b67f21eee01341e7e9fe67f6ee81b36988efa406945c8804a4b0",
                "00000062a9c72b8f9f88a271690bfa017a6466c31b9cadc2fc544744aeb817072349cfddc5ad0e81b03f1897bd9c8c6efbdf68237dc3bb00445979fb373b20c9a967ac55",
            };

/* Point P = double S */

const char *pX[pMAX] =
            {
                "30c5bc6b8c7da25354b373dc14dd8a0eba42d25a3f6e6962",
                "7669e6901606ee3ba1a8eef1e0024c33df6c22f3b17481b82a860ffcdb6127b0",
                "2a2111b1e0aa8b2fc5a1975516bc4d58017ff96b25e1bdff3c229d5fac3bacc319dcbec29f9478f42dee597b4641504c",
                "0000012879442f2450c119e7119a5f738be1f1eba9e9d7c6cf41b325d9ce6d643106e9d61124a91a96bcf201305a9dee55fa79136dc700831e54c3ca4ff2646bd3c36bc6",
            };
const char *pY[pMAX] =
            {
                "0dde14bc4249a721c407aedbf011e2ddbbcb2968c9d889cf",
                "fa878162187a54f6c39f6ee0072f33de389ef3eecd03023de10ca2c1db61d0c7",
                "fa2e3d9dc84db8954ce8085ef28d7184fddfd1344b4d4797343af9b5f9d837520b450f726443e4114bd4e5bdb2f65ddd",
                "0000019864a8b8855c2479cbefe375ae553e2393271ed36fadfc4494fc0583f6bd03598896f39854abeae5f9a6515a021e2c0eef139e71de610143f53382f4104dccb543",
            };

/* Scalar d */

const char *dE[pMAX] =
            {
                "a78a236d60baec0c5dd41b33a542463a8255391af64c74ee",
                "c51e4753afdec1e6b6c6a5b992f43f8dd0c7a8933072708b6522468b2ffb06fd",
                "a4ebcae5a665983493ab3e626085a24c104311a761b5a8fdac052ed1f111a5c44f76f45659d2d111a61b5fdd97583480",
                "000001eb7f81785c9629f136a7e8f8c674957109735554111a2a866fa5a166699419bfa9936c78b62653964df0d6da940a695c7294d41b2d6600de6dfcf0edcfc89fdcb1",
            };

/* Point Q = d * R */

const char *qX[pMAX] =
            {
                "1faee4205a4f669d2d0a8f25e3bcec9a62a6952965bf6d31",
                "51d08d5f2d4278882946d88d83c97d11e62becc3cfc18bedacc89ba34eeca03f",
                "e4f77e7ffeb7f0958910e3a680d677a477191df166160ff7ef6bb5261f791aa7b45e3e653d151b95dad3d93ca0290ef2",
                "00000091b15d09d0ca0353f8f96b93cdb13497b0a4bb582ae9ebefa35eee61bf7b7d041b8ec34c6c00c0c0671c4ae063318fb75be87af4fe859608c95f0ab4774f8c95bb",
            };

const char *qY[pMAX] =
            {
                "5ff2cdfa508a2581892367087c696f179e7a4d7e8260fb06",
                "75ee68eb8bf626aa5b673ab51f6e744e06f8fcf8a6c0cf3035beca956a7b41d5",
                "ac7dee41d8c5f4a7d5836960a773cfc1376289d3373f8cf7417b0c6207ac32e913856612fc9ff2e357eb2ee05cf9667f",
                "00000130f8f8b5e1abb4dd94f6baaf654a2d5810411e77b7423965e0c7fd79ec1ae563c207bd255ee9828eb7a03fed565240d2cc80ddd2cecbb2eb50f0951f75ad87977f",
            };


/* ------------------------ */

static void test_prime_points()
{
    ecc_curve_t *ctx;
    ecc_point_t pt,s,t,r,p,q;
    mp_int_t d;
    int i;

    printf("\n===== Test Prime Points =====\n");


    mp_init(&d);

    ecc_point_init(&pt);

    for(i=p192;i<pMAX;i++)
    {
        ctx = ecc_get_curve(curves[i]);

        printf("Curve '%s': p=%d bits, n=%d bits, a=%d bits, b=%d bits\n\n", ctx->alias[0],mp_count_bits(&(ctx->p)),mp_count_bits(&(ctx->n)),mp_count_bits(&(ctx->a)),mp_count_bits(&(ctx->b)));

        mp_set_string(&d,dE[i],16);

        ecc_point_init_string(&s,sX[i],sY[i],16);
        ecc_point_init_string(&t,tX[i],tY[i],16);
        ecc_point_init_string(&r,rX[i],rY[i],16);
        ecc_point_init_string(&p,pX[i],pY[i],16);
        ecc_point_init_string(&q,qX[i],qY[i],16);

        ecc_point_copy(&pt,&s);

        ecc_point_show("S",&pt,FALSE);
        ecc_point_show("T",&t,FALSE);

        ecc_point_add(ctx,&pt,&t);

        ecc_point_show("R = S + T",&pt,FALSE);
        ecc_point_show("Should be",&r,FALSE);
        printf("Which is %s\n\n",(ecc_point_is_equal(&pt,&r) ? "CORRECT" : "INCORRECT"));

        ecc_point_copy(&pt,&s);

        ecc_point_show("S",&pt,FALSE);

        ecc_point_double(ctx,&pt);

        ecc_point_show("Double S ",&pt,FALSE);
        ecc_point_show("Should be",&p,FALSE);
        printf("Which is %s\n\n",(ecc_point_is_equal(&pt,&p) ? "CORRECT" : "INCORRECT"));

        ecc_point_show("S",&s,FALSE);
        mp_show("  d",&d);

        ecc_point_mult(ctx,&pt,&s,&d);

        ecc_point_show("Q = d * S",&pt,FALSE);
        ecc_point_show("Should be",&q,FALSE);
        printf("Which is %s\n\n",(ecc_point_is_equal(&pt,&q) ? "CORRECT" : "INCORRECT"));

    }

    mp_clear(&d);

    ecc_point_clear(&pt);
    ecc_point_clear(&s);
    ecc_point_clear(&t);
    ecc_point_clear(&r);
    ecc_point_clear(&p);
    ecc_point_clear(&q);
}



static void test_random_point_ops(int curve)
{
    ecc_point_t res,pt;
    ecc_curve_t *ecc;

    ecc = ecc_get_curve(curve);

    printf("==== Random Point operation using Curve '%s' of %d bits ====\n\n", ecc->alias[0],ecc->NUMBITS);  

    ecc_point_init(&res);
    ecc_random_point(ecc,&pt,NULL);

      
    ecc_point_show("Random point",&pt,FALSE);
    printf("Is in the curve? %s\n\n",ecc_point_on_curve(ecc,&pt) ? "Yes" : "No");
    
    ecc_point_mult(ecc,&res,&pt,mp_two());
    ecc_point_show("Mulitplied by 2",&res,FALSE);
    printf("Is in the curve? %s\n\n",ecc_point_on_curve(ecc,&res) ? "Yes" : "No");

    ecc_point_copy(&res,&pt);
    ecc_point_add(ecc,&res,&pt);
    ecc_point_show("Added to itself",&res,FALSE);
    printf("Is in the curve? %s\n\n",ecc_point_on_curve(ecc,&res) ? "Yes" : "No");

    ecc_point_copy(&res,&pt);
    ecc_point_double(ecc,&res);
    ecc_point_show("Doubled point ",&res,FALSE);
    printf("Is in the curve? %s\n\n",ecc_point_on_curve(ecc,&res) ? "Yes" : "No");

    ecc_point_clear(&res);
    ecc_point_clear(&pt);    
}



/* ------------------------------------------------ *

    Vectors for point mulitplication
    are at:

    http://point-at-infinity.org/ecc/nisttv

* ------------------------------------------------ */


static void test_point_mult_by_k(ecc_curve_t *ecc,mp_int_t *k,char *x,char *y)
{
    ecc_point_t res,pt;

    ecc_point_init(&res);
    ecc_point_init(&pt);

    printf("Curve '%s' with ", ecc->alias[0]);
    mp_show("k",k);

    ecc_point_mult(ecc,&res,&(ecc->G),k);
    ecc_point_show(NULL,&res,FALSE);
    printf("Is in the curve? %s\n\n",ecc_point_on_curve(ecc,&res) ? "Yes" : "No");

    mp_set_string(&(pt.x),x,16);
    mp_set_string(&(pt.y),y,16);

    printf("It should be:\n");
    ecc_point_show(NULL,&pt,FALSE);

    printf("Which is %s\n\n",(ecc_point_is_equal(&pt,&res) ? "CORRECT" : "INCORRECT"));

    printf("Given point in the curve? %s\n\n",ecc_point_on_curve(ecc,&pt) ? "Yes" : "No");



    ecc_point_clear(&res);
}


/* ------------------------ */

static void test_point_mult(void)
{
    ecc_curve_t *ecc;
    mp_int_t   kBig;
    //bn_digit_t bn20[2]={1,20};

    printf("\n===== Test Point Multiplication =====\n");

    ecc = ecc_get_curve(ECC_CURVE_192r1);

    mp_init(&kBig);

    mp_submod(mp_three(),mp_six(),&(ecc->p),&kBig);

    mp_show("-3",&kBig);

    {
        ecc_point_t  p,r;
        mp_int_t m29,tmp;

        mp_init_set_d(&m29,29);
        mp_init(&tmp);

        ecc_point_init(&r);
        ecc_point_init_set(&p,mp_two(),mp_five());

        mp_show("modulus",&m29);

        mp_sub(&m29,mp_five(),&tmp);

        mp_show("modulus",&m29);

        ecc_point_inverse(&r,&p,&m29);

        ecc_point_show("inversed",&r,FALSE);

        mp_show("inversed",&r.y);

        mp_clear(&m29);

    }

    mp_set_string(&kBig,"112233445566778899",10);


    test_point_mult_by_k(ecc,mp_one(),
                         "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012",
                         "7192B95FFC8DA78631011ED6B24CDD573F977A11E794811");

    test_point_mult_by_k(ecc,mp_seven(),
                        "8DA75A1F75DDCD7660F923243060EDCE5DE37F007011FCFD",
                         "57CB5FCF6860B35418240DB8FDB3C01DD4B702F96409FFB5");

    test_point_mult_by_k(ecc,&kBig,
                         "81E6E0F14C9302C8A8DCA8A038B73165E9687D0490CD9F85",
                         "F58067119EED8579388C4281DC645A27DB7764750E812477");


    ecc = ecc_get_curve(ECC_CURVE_256r1);

    test_point_mult_by_k(ecc,mp_one(),
                         "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
                         "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5");

    test_point_mult_by_k(ecc,mp_seven(),
                         "8E533B6FA0BF7B4625BB30667C01FB607EF9F8B8A80FEF5B300628703187B2A3",
                         "73EB1DBDE03318366D069F83A6F5900053C73633CB041B21C55E1A86C1F400B4");

    test_point_mult_by_k(ecc,&kBig,
                         "339150844EC15234807FE862A86BE77977DBFB3AE3D96F4C22795513AEAAB82F",
                         "B1C14DDFDC8EC1B2583F51E85A5EB3A155840F2034730E9B5ADA38B674336A21");


    ecc = ecc_get_curve(ECC_CURVE_384r1);

    test_point_mult_by_k(ecc,mp_one(),
                         "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7",
                         "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F");

    test_point_mult_by_k(ecc,mp_seven(),
                         "283C1D7365CE4788F29F8EBF234EDFFEAD6FE997FBEA5FFA2D58CC9DFA7B1C508B05526F55B9EBB2040F05B48FB6D0E1",
                         "9475C99061E41B88BA52EFDB8C1690471A61D867ED799729D9C92CD01DBD225630D84EDE32A78F9E64664CDAC512EF8C");

    test_point_mult_by_k(ecc,&kBig,
                         "A499EFE48839BC3ABCD1C5CEDBDD51904F9514DB44F4686DB918983B0C9DC3AEE05A88B72433E9515F91A329F5F4FA60",
                         "3B7CA28EF31F809C2F1BA24AAED847D0F8B406A4B8968542DE139DB5828CA410E615D1182E25B91B1131E230B727D36A");


    ecc = ecc_get_curve(ECC_CURVE_521r1);

    test_point_mult_by_k(ecc,mp_one(),
                         "C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66",
                         "11839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650");

    test_point_mult_by_k(ecc,mp_seven(),
                         "56D5D1D99D5B7F6346EEB65FDA0B073A0C5F22E0E8F5483228F018D2C2F7114C5D8C308D0ABFC698D8C9A6DF30DCE3BBC46F953F50FDC2619A01CEAD882816ECD4",
                         "3D2D1B7D9BAAA2A110D1D8317A39D68478B5C582D02824F0DD71DBD98A26CBDE556BD0F293CDEC9E2B9523A34591CE1A5F9E76712A5DDEFC7B5C6B8BC90525251B");

    test_point_mult_by_k(ecc,&kBig,
                         "1650048FBD63E8C30B305BF36BD7643B91448EF2206E8A0CA84A140789A99B0423A0A2533EA079CA7E049843E69E5FA2C25A163819110CEC1A30ACBBB3A422A40D8",
                         "10C9C64A0E0DB6052DBC5646687D06DECE5E9E0703153EFE9CB816FE025E85354D3C5F869D6DB3F4C0C01B5F97919A5E72CEEBE03042E5AA99112691CFFC2724828");


    printf("\n\n");

    mp_clear(&kBig);
}


/* ------------------------ */

static void test_ecc_dh(ecc_curve_t *ctx, rand_t *rc)
{
    ecc_dh_t *dh1, *dh2;
    unsigned char pub1[ECC_MAX_PUBLIC_DH];
    unsigned char pub2[ECC_MAX_PUBLIC_DH];
    unsigned char shared1[ECC_MAX_SHARED_DH];
    unsigned char shared2[ECC_MAX_SHARED_DH];
    int spub1, spub2;

    printf("Testing DH Using curve '%s' (%ls)\n", ctx->alias[0], (rc) ? rc->namew : L"NULL");

    mp_show("curve p ", &(ctx->p));
    mp_show("curve a ", &(ctx->a));
    mp_show("curve b ", &(ctx->b));
    mp_show("curve Gx", &(ctx->G.x));
    mp_show("curve Gy", &(ctx->G.y));
    mp_show("curve n ", &(ctx->n));

    printf("curve h: %d\n", ctx->h);

    dh1 = ecc_dh_start(ctx, NULL, 0, rc);
    if (dh1 == NULL)
    {
        printf("ERROR getting dh1\n");
        return;
    }

    mp_show("dh1 lpri ", &(dh1->lpri));
    mp_show("dh1 lpubx", &(dh1->lpub.x));
    mp_show("dh1 lpuby", &(dh1->lpub.y));

    spub1 = ecc_dh_get_public(dh1, pub1, ECC_MAX_PUBLIC_DH, FALSE);

    dh2 = ecc_dh_start(ctx, pub1, spub1, rc);
    if (dh2 == NULL)
    {
        printf("ERROR getting dh2\n");
        ecc_dh_end(dh1, NULL, 0, NULL);
        return;
    }

    mp_show("dh2 lpri ", &(dh2->lpri));
    mp_show("dh2 lpubx", &(dh2->lpub.x));
    mp_show("dh2 lpuby", &(dh2->lpub.y));

    spub2 = ecc_dh_get_public(dh2, pub2, ECC_MAX_PUBLIC_DH, FALSE);

    printf("\n");

    print_bytes("public 1 ", pub1, spub1);
    print_bytes("public 2 ", pub2, spub2);

    printf("\n");

    ecc_dh_end(dh1, pub2, spub2, shared1);
    ecc_dh_end(dh2, NULL, 0, shared2);

    print_bytes("shared 1", shared1, ctx->NUMBYTES);
    print_bytes("shared 2", shared2, ctx->NUMBYTES);

    printf("\nResult is %s\n\n",memcmp(shared1,shared2,ctx->NUMBYTES) ? "INCORRECT" : "CORRECT");
}

/* ------------------------ */

static void test_ecc_rnd(int curve)
{
    ecc_curve_t *ecc = NULL;
    ecc_key_t *key = NULL;//, *k1, *k2;
    rand_t *rng;
    //char pub[ECC_MAX_HEX_BUFFER];
    //char pri[ECC_MAX_HEX_BUFFER];
    //int i, t;

    printf("\n===== Test ECC =====\n");

    rng = rand_start(RAND_OS, RAND_TLS_SHA256);
    if (rng)
        printf("Using prng '%ls'\n", rng->namew);

    printf("Getting curve ... %d\n", curve);

    ecc = ecc_get_curve(curve);

    if (ecc == NULL)
    {
        printf("ERROR getting curve\n");
        return;
    }

    printf("Using curve '%s'\n", ecc->alias[0]);

    printf("Base point is%son the curve\n", (ecc_point_on_curve(ecc, &(ecc->G)) ? " " : " NOT "));
    
    printf("NUMBITS  : %d\n", ecc->NUMBITS);
    printf("NUMBYTES : %d\n", ecc->NUMBYTES);

    test_ecc_dh(ecc, rng);

    printf("\n\nGenerating keys...\n");
    key = ecc_generate_keys(curve, rng);
    if (key)
    {
        //wchar_t pem_name[81];
        //int ret;

        print_ecc_key(key);

        /*
        wcsnprintf(pem_name,81,L"ec_pub_%s.pem",key->ctx->name);

        ret = ecc_save_pem(pem_name,key,FALSE,NULL,0);
        printf("saving %ls = %d\n",pem_name,ret);

        wcsnprintf(pem_name,81,L"ec_pri_%s.pem",key->ctx->name);

        ret = ecc_save_pem(pem_name,key,TRUE,"test",0);
        printf("saving %ls = %d\n",pem_name,ret);
        */
        ecc_free_keys(key);
    }
    rand_end(rng);
}

/* ------------------------ */

void test_compression(int curve)
{
    ecc_key_t *key = NULL;
    unsigned char pub[ECC_MAX_HEX_BUFFER];
    int ret;

    printf("\n===== Test Point Compresion =====\n");

    printf("\n Generating Key..\n");


    key = ecc_generate_keys(curve,NULL);
    if (key)
    {
        mp_int_t mp;

        printf("Using curve '%s'\n", key->ctx->alias[0]);

        mp_show("Private Key   ", key->pri);
        mp_show("Public Key's X", &(key->pub.x));
        mp_show("Public Key's Y", &(key->pub.y));

        mp_init_copy(&mp,&(key->pub.y));

        printf("\n");

        ret = ecc_point_to_bytes(key->ctx,&(key->pub),pub,ECC_MAX_HEX_BUFFER,TRUE);

        print_bytes("Public Key Bytes",pub,ret);
        printf("\n");

        ecc_free_keys(key);

        printf("\nGenerating Public Key...\n");
        key = ecc_keys_from_bytes(curve,NULL,0,pub,ret);
        if (key)
        {

            mp_show("Private Key   ", key->pri);
            mp_show("Public Key's X", &(key->pub.x));
            mp_show("Public Key's Y", &(key->pub.y));

            printf("\n");

            printf("Result is %s\n",(mp_cmp(&mp,&(key->pub.y)) == MP_EQ) ? "CORRECT" : "INCORRECT" );

            printf("\n");

            ecc_free_keys(key);

        }

        mp_clear(&mp);
    }

    printf("\n");
}

/* --------------- */

void test_ecc(void)
{
    int curve = ECC_CURVE_NONE;

    printf(" TEST ECC START\n");

    while (++curve < ECC_NUM_CURVES)
    {
        test_ecc_rnd(curve);
        test_compression(curve);
        test_random_point_ops(curve);
    }

    printf("\n\n");

    printf(" TEST ECC _END\n");
}


/* ------------------------------- */

static void generate_ecdsa(wchar_t *name, int curve)
{
    ecc_key_t *key;
    FILE *fp;

    printf("Generating key..\n");
    key = ecc_generate_keys(curve, NULL);
    if (key == NULL)
    {
        printf("Cannot generate key\n");
        return;
    }

    mp_show("Gen pri", key->pri);
    ecc_point_show("Gen pub", &key->pub,FALSE);

    printf("Saving private key\n");

    fp = fopenw(name, "wb");
    if (fp)
    {
        int ret = ecc_private_to_pem(key, "test", AES_128_CBC, fp);

        if (ret)
            printf("Cannot save private key\n");
        else
            ret = ecc_public_to_pem(key, fp);
        fflush(fp);
        fclose(fp);
        if (!ret)
        {
            printf("DONE IT\n");
            return;
        }
    }
    printf("ERROR saving it\n");
}

/* ------------------------------- */

void test_ecdsa(void)
{
    ecdsa_sign_t sign;
    ecc_key_t *key = NULL;
    int ret;
    void *data;
    size_t tam;

    printf("===== TEST ECDSA =====\n");

    generate_ecdsa(L"MyEcdsa384r1.pem", ECC_CURVE_384r1);

    ret = ecc_load_pemw(L"MyEcdsa384r1.pem", FALSE, "test", &key);
    if (ret)
    {
        printf("ERROR %d loading the 2nd key \n", ret);
        return;
    }
    printf("Signing...\n");
    ecdsa_init_sign(&sign);
    ret = ecdsa_sign_data(key, ECDSA_SHA224, "Hola Pepe\n",10, &sign);
    if (ret)
    {
        printf("Error %d Signing\n", ret);
    }
    else
    {
        asn1_t *asn;
        printf("OK, saving...\n");

        asn = ecdsa_sign_to_asn1(&sign, FALSE);

        if (asn)
        {
            ret = save_whole_filew(L"sign_224.bin", asn->start, asn->total);
            asn1_free(asn);
            if (ret)
                printf("ERROR %d saving\n", ret);
            else
                printf("Saved Correctly\n");
        }
        else
        {
            printf("ERROR Converting to ASN1\n");
        }
    }

    printf("Readign signature...\n");

    data = read_whole_filew(L"sign_224.bin", &tam);

    if (data)
    {
        ret = ecdsa_sign_from_asn1(&sign, data, (int)tam);

        free(data);

        if (ret)
        {
            printf("ERROR %d converting ASN1 to ecdsa signature\n", ret);
        }
        else
        {
            ret = ecdsa_verify_sign(key, ECDSA_SHA224, "Hola Pepe\n",10, &sign);
            printf("Got %d verifying signature\n", ret);
        }
    }
    else
    {
        printf("ERROR verifying signature\n");
    }
    ecdsa_destroy_sign(&sign);
    ecc_free_keys(key);
}

/* ------------------------------- */

void test_eddsa_25519(void)
{
    ecdsa_sign_t sign;
    ecc_key_t *key = NULL;
    int ret;
    void *data;
    size_t tam;

    printf("===== TEST EdDSA 25519 =====\n");



    generate_ecdsa(L"MyEcdsa25519.pem", ECC_CURVE_X25519);

    ret = ecc_load_pemw(L"MyEcdsa25519.pem", FALSE, "test", &key);
    if (ret)
    {
        printf("ERROR %d loading the 2nd key \n", ret);
        return;
    }
    else
    {
        mp_show("Loaded pri", key->pri);
        ecc_point_show("Loaded pub", &key->pub,FALSE);
    }
    printf("Signing...\n");
    ecdsa_init_sign(&sign);
    ret = ecdsa_sign_data(key, EdDSA_25519, "Hola Pepe\n",10, &sign);
    if (ret)
    {
        printf("Error %d Signing\n", ret);
    }
    else
    {
        asn1_t *asn;
        printf("OK, saving...\n");

        asn = ecdsa_sign_to_asn1(&sign, FALSE);

        if (asn)
        {
            ret = save_whole_filew(L"sign_25519.bin", asn->start, asn->total);
            asn1_free(asn);
            if (ret)
                printf("ERROR %d saving\n", ret);
            else
                printf("Saved Correctly\n");
        }
        else
        {
            printf("ERROR Converting to ASN1\n");
        }
    }

    printf("Readign signature...\n");

    data = read_whole_filew(L"sign_25519.bin", &tam);

    if (data)
    {
        ret = ecdsa_sign_from_asn1(&sign, data, (int)tam);

        free(data);

        if (ret)
        {
            printf("ERROR %d converting ASN1 to ecdsa signature\n", ret);
        }
        else
        {
            ret = ecdsa_verify_sign(key, EdDSA_25519,"Hola Pepe\n",10,&sign);
            printf("Got %d verifying signature\n", ret);
        }
    }
    else
    {
        printf("ERROR verifying signature\n");
    }
    ecdsa_destroy_sign(&sign);
    ecc_free_keys(key);
}



/* ------------------------------- */

void test_eddsa_448(void)
{
    ecdsa_sign_t sign;
    ecc_key_t *key = NULL;
    int ret;
    void *data;
    size_t tam;

    printf("===== TEST EdDSA 448 =====\n");

    generate_ecdsa(L"MyEcdsa448.pem", ECC_CURVE_X448);

    ret = ecc_load_pemw(L"MyEcdsa448.pem", FALSE, "test", &key);
    if (ret)
    {
        printf("ERROR %d loading the 2nd key \n", ret);
        return;
    }
    else
    {
        mp_show("Loaded pri", key->pri);
        ecc_point_show("Loaded pub", &key->pub,FALSE);
    }
    printf("Signing...\n");
    ecdsa_init_sign(&sign);
    ret = ecdsa_sign_data(key, EdDSA_448, "Hola Pepe\n",10, &sign);
    if (ret)
    {
        printf("Error %d Signing\n", ret);
    }
    else
    {
        asn1_t *asn;
        printf("OK, saving...\n");

        asn = ecdsa_sign_to_asn1(&sign, FALSE);

        if (asn)
        {
            ret = save_whole_filew(L"sign_448.bin", asn->start, asn->total);
            asn1_free(asn);
            if (ret)
                printf("ERROR %d saving\n", ret);
            else
                printf("Saved Correctly\n");
        }
        else
        {
            printf("ERROR Converting to ASN1\n");
        }
    }

    printf("Readign signature...\n");

    data = read_whole_filew(L"sign_448.bin", &tam);

    if (data)
    {
        ret = ecdsa_sign_from_asn1(&sign, data, (int)tam);

        free(data);

        if (ret)
        {
            printf("ERROR %d converting ASN1 to ecdsa signature\n", ret);
        }
        else
        {
            ret = ecdsa_verify_sign(key, EdDSA_448,"Hola Pepe\n",10,&sign);
            printf("Got %d verifying signature\n", ret);
        }
    }
    else
    {
        printf("ERROR verifying signature\n");
    }
    ecdsa_destroy_sign(&sign);
    ecc_free_keys(key);
}



/* ------------------------ */

static void keccak_hash(void *hash,const void *data,size_t len)
{
    keccak_t k3;

    keccak_init(&k3,256);
    keccak_update(&k3,data,len);
    keccak_final(&k3,hash);
}

/* ------------------------ */

static void test_ethereum_key(const char *pvk,const char *public)
{
    ecc_key_t *key = NULL;//, *k1, *k2;
    unsigned char tmp[ECC_MAX_HEX_BUFFER];
    char pub[ECC_MAX_HEX_BUFFER];
    char pri[ECC_MAX_HEX_BUFFER];
    int ret;


    printf("\n\nGenerating Ethereum Account...\n");
    key = ecc_keys_from_chars(ECC_CURVE_256k1,pvk,NULL,NULL);
    if (key)
    {
        mp_show("Private Key   ", key->pri);
        printf( "It should be   = 0x%s\n",pvk);

        ret = ecc_point_to_bytes(key->ctx, &(key->pub),pub,ECC_MAX_HEX_BUFFER, FALSE);

        memcpy(tmp,pub+1,--ret);

        print_bytes_hexa("Public Key    ",tmp,ret);

        hex_to_char(pub,ECC_MAX_HEX_BUFFER,tmp,ret,0);

        printf( "It should be   = 0x%s\n",pub);

        keccak_hash(pri,tmp,ret);

        print_bytes_hexa("Pub Key Hash  ",pri,32);
        print_bytes_hexa("Public Address",pri + 12,20);

        printf("It should be   = 0x%s (last 20 bytes of public key hash)\n\n",public);

        ecc_free_keys(key);
    }
}

/* ------------------------ */



unsigned char eth_priv_key [32] = {0xb7,0x1c,0x71,0xa6,0x7e,0x11,0x77,0xad,0x4e,0x90,0x16,0x95,0xe1,0xb4,0xb9,0xee,
                          0x17,0xae,0x16,0xc6,0x66,0x8d,0x31,0x3e,0xac,0x2f,0x96,0xdb,0xcd,0xa3,0xf2,0x91};

unsigned char eth_pub_key [33] = {0x03,0xca,0x63,0x4c,0xae,0x0d,0x49,0xac,0xb4,0x01,0xd8,0xa4,0xc6,0xb6,0xfe,0x8c,0x55,
                         0xb7,0x0d,0x11,0x5b,0xf4,0x00,0x76,0x9c,0xc1,0x40,0x0f,0x32,0x58,0xcd,0x31,0x38};


static void test_ethereum_public_key(void)
{
    ecc_key_t *key = NULL;//, *k1, *k2;
//    unsigned char tmp[ECC_MAX_HEX_BUFFER];
//    char pub[ECC_MAX_HEX_BUFFER];
//    char pri[ECC_MAX_HEX_BUFFER];
//    int ret;


    printf("\n\nGenerando Ethereum Public Key...\n");
    key = ecc_keys_from_bytes(ECC_CURVE_256k1,NULL,0,eth_pub_key,33);
    if (key)
    {

        mp_show("Private Key   ", key->pri);
        printf( "It should be   = 0xNULL\n\n");

        mp_show("Public Key's X", &(key->pub.x));
        printf( "It should be   = 0xca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd3138\n\n");
        mp_show("Public Key's Y", &(key->pub.y));
        printf( "It should be   = 0x7574077f301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f\n\n");

        ecc_free_keys(key);
    }

    printf("\n\nGenerando Ethereum Public Key...\n");
    key = ecc_keys_from_bytes(ECC_CURVE_256k1,eth_priv_key,32,NULL,0);
    if (key)
    {

        mp_show("Private Key   ", key->pri);
        printf( "It should be   = 0xb71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291\n\n");

        mp_show("Public Key's X", &(key->pub.x));
        printf( "It should be   = 0xca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd3138\n\n");
        mp_show("Public Key's Y", &(key->pub.y));
        printf( "It should be   = 0x7574077f301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f\n\n");

        ecc_free_keys(key);

    }

}


/* ------------------------ */

static void test_ethereum(void)
{
    printf("\n===== Test Ethereum =====\n");

    test_ethereum_key("8ce71d24a7d7692fe79f6967740450a6eaf84017930aa6395d09597a62091de6","71ceeB2Ab44262887c7C514f3Fb1F987D52E9755");
    test_ethereum_key("aa0aaa79deb7667d375e26194636ccb83be33d6c1d86b81e24c776d9cc462505","15b5C086D88e88B548008c72769c379b093c58e6");
    test_ethereum_key("badd7c8063c996d2d8f76d17622d83cd6da1100f6d09cee870344eb32ca460bc","986EA45e97ad7c638ACbf303Bb7d7a0a42C00722");
    test_ethereum_key("3f665e8d1f20ac9a92d4f76cfde429f7eca2ab8052f33c300d2edba65b37134b","FDc76Fc12B7B7818cD0e94Fba9a16B59a2c2703e");
    test_ethereum_key("78331f2676711da65bf9866a2b108390ae7a65e2a30da892009e14fb8ade8f7f","BFE3e39D2449e23aB6207260Fe501CFE84327F24");
    test_ethereum_key("371c9e8214f0af4fe8f009a7984723fe10463b0edf56af16d302002788558cdb","cDA4d74b609529Da25402bF2bA62D4B8d1DFb783");

    test_ethereum_public_key();

}


/* -------------------------------------- */

int main(void)
{

    fprintf(stdout,"\n== CURVES TEST START \n\n");


    test_curve_25519();
    test_curve_448();

    test_prime_points();

    test_point_mult();

    test_ecc();

    test_ecdsa();

    test_eddsa_25519();
    test_eddsa_448();

    test_ethereum();

    fprintf(stdout,"\n== CURVES TEST END\n\n");
}


