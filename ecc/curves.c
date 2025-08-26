/*

    curves.c

    Elliptic Curve Cryptography Curves

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

    - Originally written by Guillermo Amodeo Ojeda using info from:

        * https://en.wikipedia.org/wiki/Elliptic-curve_cryptography

        * Hankerson, Menezes & Vanstone's "Guide to Elliptic Curve Cryptography"
          Springer (2004)

        * Michael Rosing's Book "Implementing Elliptic Curve Cryptography".
          Manning Publications Co (1998).

        * Phrack's "All Hackers Need To Know About Elliptic Curve Cryptography".
          http://www.phrack.org/issues/63/3.html (2005)

        * B. Poettering's articles at http://point-at-infinity.org/ecc/

        * RFC 5915 at https://tools.ietf.org/html/rfc5915

        * RFC 7748 at https://tools.ietf.org/html/rfc7748

        * RFC 7748 at https://tools.ietf.org/html/rfc8032

        * https://www.cs.miami.edu/home/burt/learning/Csc609.142/ecdsa-cert.pdf

        * http://csrc.nist.gov/publications/fips/fips186-3/fips_186-3.pdf

        * SEC 2: Recommended Elliptic Curve Domain Parameters
          http://www.secg.org/sec2-v2.pdf

       * Handbook of Applied Cryptography - https://cacr.uwaterloo.ca/hac/

       * https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.204.9073&rep=rep1&type=pdf

    - The functions have been made time-unpredictable. Some are time-constant and some never
      take the same time to execute even if they are given exactly the same data.

    - We include Curve25519 and Curve448 in our homogeneus management of Elliptic Curves,
      which is why we consider points on those curves, even if the Y coordinate is ignored
      when used and -thus- always set to 0.

                                --oO0Oo--
*/

#include "curves.h"

/* ------------------------ *
    Curve fields
 * ------------------------ */

/* NIST P-192 */

static mp_digit_t s192r1_p []={0xffffffff, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t s192r1_a []={0xfffffffc, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t s192r1_b []={0xc146b9b1, 0xfeb8deec, 0x72243049, 0x0fa7e9ab, 0xe59c80e7, 0x64210519};
static mp_digit_t s192r1_Gx[]={0x82ff1012, 0xf4ff0afd, 0x43a18800, 0x7cbf20eb, 0xb03090f6, 0x188da80e};
static mp_digit_t s192r1_Gy[]={0x1e794811, 0x73f977a1, 0x6b24cdd5, 0x631011ed, 0xffc8da78, 0x07192b95};
static mp_digit_t s192r1_n []={0xb4d22831, 0x146bc9b1, 0x99def836, 0xffffffff, 0xffffffff, 0xffffffff};

/* NIST K-192 */

static mp_digit_t s192k1_p []={0xffffee37, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t s192k1_Gx[]={0xeae06c7d, 0x1da5d1b1, 0x80b7f434, 0x26b07d02, 0xc057e9ae, 0xdb4ff10e};
static mp_digit_t s192k1_Gy[]={0xd95e2f9d, 0x4082aa88, 0x15be8634, 0x844163d0, 0x9c5628a7, 0x9b2f2f6d};
static mp_digit_t s192k1_n []={0x74defd8d, 0x0f69466a, 0x26f2fc17, 0xfffffffe, 0xffffffff, 0xffffffff};

/* NIST P-256r1 */

static mp_digit_t s256r1_p []={0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0xffffffff};
static mp_digit_t s256r1_a []={0xfffffffc, 0xffffffff, 0xffffffff, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0xffffffff};
static mp_digit_t s256r1_b []={0x27d2604b, 0x3bce3c3e, 0xcc53b0f6, 0x651d06b0, 0x769886bc, 0xb3ebbd55, 0xaa3a93e7, 0x5ac635d8};
static mp_digit_t s256r1_Gx[]={0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81, 0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2};
static mp_digit_t s256r1_Gy[]={0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357, 0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2};
static mp_digit_t s256r1_n []={0xfc632551, 0xf3b9cac2, 0xa7179e84, 0xbce6faad, 0xffffffff, 0xffffffff, 0x00000000, 0xffffffff};

/* NIST P-256k1 */

static mp_digit_t s256k1_p []={0xfffffc2f, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t s256k1_Gx[]={0x16f81798, 0x59f2815b, 0x2dce28d9, 0x029bfcdb, 0xce870b07, 0x55a06295, 0xf9dcbbac, 0x79be667e};
static mp_digit_t s256k1_Gy[]={0xfb10d4b8, 0x9c47d08f, 0xa6855419, 0xfd17b448, 0x0e1108a8, 0x5da4fbfc, 0x26a3c465, 0x483ada77};
static mp_digit_t s256k1_n []={0xd0364141, 0xbfd25e8c, 0xaf48a03b, 0xbaaedce6 , 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff};

/* NIST P-384 */

static mp_digit_t s384r1_p []={0xffffffff, 0x00000000, 0x00000000, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t s384r1_a []={0xfffffffc, 0x00000000, 0x00000000, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t s384r1_b []={0xd3ec2aef, 0x2a85c8ed, 0x8a2ed19d, 0xc656398d, 0x5013875a, 0x0314088f, 0xfe814112, 0x181d9c6e, 0xe3f82d19, 0x988e056b, 0xe23ee7e4, 0xb3312fa7};
static mp_digit_t s384r1_Gx[]={0x72760ab7, 0x3a545e38, 0xbf55296c, 0x5502f25d, 0x82542a38, 0x59f741e0, 0x8ba79b98, 0x6e1d3b62, 0xf320ad74, 0x8eb1c71e, 0xbe8b0537, 0xaa87ca22};
static mp_digit_t s384r1_Gy[]={0x90ea0e5f, 0x7a431d7c, 0x1d7e819d, 0x0a60b1ce, 0xb5f0b8c0, 0xe9da3113, 0x289a147c, 0xf8f41dbd, 0x9292dc29, 0x5d9e98bf, 0x96262c6f, 0x3617de4a};
static mp_digit_t s384r1_n []={0xccc52973, 0xecec196a, 0x48b0a77a, 0x581a0db2, 0xf4372ddf, 0xc7634d81, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};

/* NIST P-521 */

static mp_digit_t s521r1_p []={0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x000001ff};
static mp_digit_t s521r1_a []={0xfffffffc, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x000001ff};
static mp_digit_t s521r1_b []={0x6b503f00, 0xef451fd4, 0x3d2c34f1, 0x3573df88, 0x3bb1bf07, 0x1652c0bd, 0xec7e937b, 0x56193951, 0x8ef109e1, 0xb8b48991, 0x99b315f3, 0xa2da725b, 0xb68540ee, 0x929a21a0, 0x8e1c9a1f, 0x953eb961, 0x00000051};
static mp_digit_t s521r1_Gx[]={0xc2e5bd66, 0xf97e7e31, 0x856a429b, 0x3348b3c1, 0xa2ffa8de, 0xfe1dc127, 0xefe75928, 0xa14b5e77, 0x6b4d3dba, 0xf828af60, 0x053fb521, 0x9c648139, 0x2395b442, 0x9e3ecb66, 0x0404e9cd, 0x858e06b7, 0x000000c6};
static mp_digit_t s521r1_Gy[]={0x9fd16650, 0x88be9476, 0xa272c240, 0x353c7086, 0x3fad0761, 0xc550b901, 0x5ef42640, 0x97ee7299, 0x273e662c, 0x17afbd17, 0x579b4468, 0x98f54449, 0x2c7d1bd9, 0x5c8a5fb4, 0x9a3bc004, 0x39296a78, 0x00000118};
static mp_digit_t s521r1_n []={0x91386409, 0xbb6fb71e, 0x899c47ae, 0x3bb5c9b8, 0xf709a5d0, 0x7fcc0148, 0xbf2f966b, 0x51868783, 0xfffffffa, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x000001ff};

/* BrainPool P-192 */

static mp_digit_t b192r1_p []={0xe1a86297, 0x8fce476d, 0x93d18db7, 0xa7a34630, 0x932a36cd, 0xc302f41d};
static mp_digit_t b192r1_a []={0xc69a28ef, 0xcae040e5, 0xfe8685c1, 0x9c39c031, 0x76b1e0e1, 0x6a911740};
static mp_digit_t b192r1_b []={0x6fbf25c9, 0xca7ef414, 0x4f4496bc, 0xdc721d04, 0x7c28cca3, 0x469a28ef};
static mp_digit_t b192r1_Gx[]={0x53375fd6, 0x0a2f5c48, 0x6cb0f090, 0x53b033c5, 0xaab6a487, 0xc0a0647e};
static mp_digit_t b192r1_Gy[]={0xfa299b8f, 0xe6773fa2, 0xc1490002, 0x8b5f4828, 0x6abd5bb8, 0x14b69086};
static mp_digit_t b192r1_n []={0x9ac4acc1, 0x5be8f102, 0x9e9e916b, 0xa7a3462f, 0x932a36cd, 0xc302f41d};

/* BrainPool P-224 */

static mp_digit_t b224r1_p []={0x7ec8c0ff, 0x97da89f5, 0xb09f0757, 0x75d1d787, 0x2a183025, 0x26436686, 0xd7c134aa};
static mp_digit_t b224r1_a []={0xcad29f43, 0xb0042a59, 0x4e182ad8, 0xc1530b51, 0x299803a6, 0xa9ce6c1c, 0x68a5e62c};
static mp_digit_t b224r1_b []={0x386c400b, 0x66dbb372, 0x3e2135d2, 0xa92369e3, 0x870713b1, 0xcfe44138, 0x2580f63c};
static mp_digit_t b224r1_Gx[]={0xee12c07d, 0x4c1e6efd, 0x9e4ce317, 0xa87dc68c, 0x340823b2, 0x2c7e5cf4, 0x0d9029ad};
static mp_digit_t b224r1_Gy[]={0x761402cd, 0xcaa3f6d3, 0x354b9e99, 0x4ecdac24, 0x24c6b89e, 0x72c0726f, 0x58aa56f7};
static mp_digit_t b224r1_n []={0xa5a7939f, 0x6ddebca3, 0xd116bc4b, 0x75d0fb98, 0x2a183025, 0x26436686, 0xd7c134aa};

/* BrainPool P-256 */

static mp_digit_t b256r1_p []={0x1f6e5377, 0x2013481d, 0xd5262028, 0x6e3bf623, 0x9d838d72, 0x3e660a90, 0xa1eea9bc, 0xa9fb57db};
static mp_digit_t b256r1_a []={0xf330b5d9, 0xe94a4b44, 0x26dc5c6c, 0xfb8055c1, 0x417affe7, 0xeef67530, 0xfc2c3057, 0x7d5a0975};
static mp_digit_t b256r1_b []={0xff8c07b6, 0x6bccdc18, 0x5cf7e1ce, 0x95841629, 0xbbd77cbf, 0xf330b5d9, 0xe94a4b44, 0x26dc5c6c};
static mp_digit_t b256r1_Gx[]={0x9ace3262, 0x3a4453bd, 0xe3bd23c2, 0xb9de27e1, 0xfc81b7af, 0x2c4b482f, 0xcb7e57cb, 0x8bd2aeb9};
static mp_digit_t b256r1_Gy[]={0x2f046997, 0x5c1d54c7, 0x2ded8e54, 0xc2774513, 0x14611dc9, 0x97f8461a, 0xc3dac4fd, 0x547ef835};
static mp_digit_t b256r1_n []={0x974856a7, 0x901e0e82, 0xb561a6f7, 0x8c397aa3, 0x9d838d71, 0x3e660a90, 0xa1eea9bc, 0xa9fb57db};

/* BrainPool P-320 */

static mp_digit_t b320r1_p []={0xf1b32e27, 0xfcd412b1, 0x7893ec28, 0x4f92b9ec, 0xf6f40def, 0xf98fcfa6, 0xd201e065, 0xe13c785e, 0x36bc4fb7, 0xd35e4720};
static mp_digit_t b320r1_a []={0x7d860eb4, 0x92f375a9, 0x85ffa9f4, 0x66190eb0, 0xf5eb79da, 0xa2a73513, 0x6d3f3bb8, 0x83ccebd4, 0x8fbab0f8, 0x3ee30b56};
static mp_digit_t b320r1_b []={0x8fb1f1a6, 0x6f5eb4ac, 0x88453981, 0xcc31dccd, 0x9554b49a, 0xe13f4134, 0x40688a6f, 0xd3ad1986, 0x9dfdbc42, 0x52088394};
static mp_digit_t b320r1_Gx[]={0x39e20611, 0x10af8d0d, 0x10a599c7, 0xe7871e2a, 0x0a087eb6, 0xf20137d1, 0x8ee5bfe6, 0x5289bcc4, 0xfb53d8b8, 0x43bd7e9a};
static mp_digit_t b320r1_Gy[]={0x692e8ee1, 0xd35245d1, 0xaaac6ac7, 0xa9c77877, 0x117182ea, 0x0743ffed, 0x7f77275e, 0xab409324, 0x45ec1cc8, 0x14fdd055};
static mp_digit_t b320r1_n []={0x44c59311, 0x8691555b, 0xee8658e9, 0x2d482ec7, 0xb68f12a3, 0xf98fcfa5, 0xd201e065, 0xe13c785e, 0x36bc4fb7, 0xd35e4720};

/* BrainPool P-384 */

static mp_digit_t b384r1_p []={0x3107ec53, 0x87470013, 0x901d1a71, 0xacd3a729, 0x7fb71123, 0x12b1da19, 0xed5456b4, 0x152f7109, 0x50e641df, 0x0f5d6f7e, 0xa3386d28, 0x8cb91e82};
static mp_digit_t b384r1_a []={0x22ce2826, 0x04a8c7dd, 0x503ad4eb, 0x8aa5814a, 0xba91f90f, 0x139165ef, 0x4fb22787, 0xc2bea28e, 0xce05afa0, 0x3c72080a, 0x3d8c150c, 0x7bc382c6};
static mp_digit_t b384r1_b []={0xfa504c11, 0x3ab78696, 0x95dbc994, 0x7cb43902, 0x3eeb62d5, 0x2e880ea5, 0x07dcd2a6, 0x2fb77de1, 0x16f0447c, 0x8b39b554, 0x22ce2826, 0x04a8c7dd};
static mp_digit_t b384r1_Gx[]={0x47d4af1e, 0xef87b2e2, 0x36d646aa, 0xe826e034, 0x0cbd10e8, 0xdb7fcafe, 0x7ef14fe3, 0x8847a3e7, 0xb7c13f6b, 0xa2a63a81, 0x68cf45ff, 0x1d1c64f0};
static mp_digit_t b384r1_Gy[]={0x263c5315, 0x42820341, 0x77918111, 0x0e464621, 0xf9912928, 0xe19c054f, 0xfeec5864, 0x62b70b29, 0x95cfd552, 0x5cb1eb8e, 0x20f9c2a4, 0x8abe1d75};
static mp_digit_t b384r1_n []={0xe9046565, 0x3b883202, 0x6b7fc310, 0xcf3ab6af, 0xac0425a7, 0x1f166e6c, 0xed5456b3, 0x152f7109, 0x50e641df, 0x0f5d6f7e, 0xa3386d28, 0x8cb91e82};

/* BrainPool P-512 */

static mp_digit_t b512r1_p []={0x583a48f3, 0x28aa6056, 0x2d82c685, 0x2881ff2f, 0xe6a380e6, 0xaecda12a, 0x9bc66842, 0x7d4d9b00, 0x70330871, 0xd6639cca, 0xb3c9d20e, 0xcb308db3, 0x33c9fc07, 0x3fd4e6ae, 0xdbe9c48b, 0xaadd9db8};
static mp_digit_t b512r1_a []={0x77fc94ca, 0xe7c1ac4d, 0x2bf2c7b9, 0x7f1117a7, 0x8b9ac8b5, 0x0a2ef1c9, 0xa8253aa1, 0x2ded5d5a, 0xea9863bc, 0xa83441ca, 0x3df91610, 0x94cbdd8d, 0xac234cc5, 0xe2327145, 0x8b603b89, 0x7830a331};
static mp_digit_t b512r1_b []={0x8016f723, 0x2809bd63, 0x5ebae5dd, 0x984050b7, 0xdc083e67, 0x77fc94ca, 0xe7c1ac4d, 0x2bf2c7b9, 0x7f1117a7, 0x8b9ac8b5, 0x0a2ef1c9, 0xa8253aa1, 0x2ded5d5a, 0xea9863bc, 0xa83441ca, 0x3df91610};
static mp_digit_t b512r1_Gx[]={0xbcb9f822, 0x8b352209, 0x406a5e68, 0x7c6d5047, 0x93b97d5f, 0x50d1687b, 0xe2d0d48d, 0xff3b1f78, 0xf4d0098e, 0xb43b62ee, 0xb5d916c1, 0x85ed9f70, 0x9c4c6a93, 0x5a21322e, 0xd82ed964, 0x81aee4bd};
static mp_digit_t b512r1_Gy[]={0x3ad80892, 0x78cd1e0f, 0xa8f05406, 0xd1ca2b2f, 0x8a2763ae, 0x5bca4bd8, 0x4a5f485e, 0xb2dcde49, 0x881f8111, 0xa000c55b, 0x24a57b1a, 0xf209f700, 0xcf7822fd, 0xc0eabfa9, 0x566332ec, 0x7dde385d};
static mp_digit_t b512r1_n []={0x9ca90069, 0xb5879682, 0x085ddadd, 0x1db1d381, 0x7fac1047, 0x41866119, 0x4ca92619, 0x553e5c41, 0x70330870, 0xd6639cca, 0xb3c9d20e, 0xcb308db3, 0x33c9fc07, 0x3fd4e6ae, 0xdbe9c48b, 0xaadd9db8};

/* curve25519 parameters from RFC-7748/RFC-8032 */

static mp_digit_t c25519_p []={0xffffffed, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff};
static mp_digit_t c25519_a []={0x00076D06};
static mp_digit_t c25519_Gx[]={0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x09000000};
static mp_digit_t c25519_Gy[]={0x7eced3d9, 0x29e9c5a2, 0x6d7c61b2, 0x923d4d7e, 0x7748d14c, 0xe01edd2c, 0xb8a086b4, 0x20ae19a1};
static mp_digit_t c25519_n []={0x5cf5d3ed, 0x5812631a, 0xa2f79cd6, 0x14def9de, 0x00000000, 0x00000000, 0x00000000, 0x10000000};

/* curve448 parameters from RFC-7748/RFC-8032 */

static mp_digit_t c448_p []={0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
static mp_digit_t c448_a []={0x000098A9};
static mp_digit_t c448_Gx[]={0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x05000000};
static mp_digit_t c448_Gy[]={0x457B5B1A, 0x6FD7223D, 0x50677AF7, 0x1312C4B1, 0x46430D21, 0xB8027E23, 0x8DF3F6ED, 0x60F75DC2, 0xF55545D0, 0xCBAE5D34, 0x58326FCE, 0x6C98AB6E, 0x95F5B1F6, 0x7D235D12};
static mp_digit_t c448_n []={0xAB5844F3, 0x2378C292, 0x8DC58F55, 0x216CC272, 0xAED63690, 0xC44EDB49, 0x7CCA23E9, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x3fffffff};

/* ------------------------ *
    Curve names
 * ------------------------ */

const char *name_curve_192k1[3]={"secp192k1","ansip192k1",NULL};
const char *name_curve_192r1[5]={"secp192r1","prime192v1","nistp192","P-192",NULL};
const char *name_curve_256r1[5]={"secp256r1","prime256v1","nistp256","P-256",NULL};
const char *name_curve_256k1[3]={"secp256k1","ansip256k1",NULL};
const char *name_curve_384r1[5]={"secp384r1","prime384v1","nistp384","P-384",NULL};
const char *name_curve_521r1[5]={"secp521r1","prime521v1","nistp521","P-521",NULL};

const char *name_curve_b192r1[2]={"brainpoolP192r1",NULL};
const char *name_curve_b224r1[2]={"brainpoolP224r1",NULL};
const char *name_curve_b256r1[2]={"brainpoolP256r1",NULL};
const char *name_curve_b320r1[2]={"brainpoolP320r1",NULL};
const char *name_curve_b384r1[2]={"brainpoolP384r1",NULL};
const char *name_curve_b512r1[2]={"brainpoolP512r1",NULL};

const char *name_curve_25519[3]={"curve25519","X25519",NULL};
const char *name_curve_448  [3]={"curve448","X448",NULL};

/* ------------------------ *
    Curves
 * ------------------------ */

static mp_digit_t bnZero  = 0;
static mp_digit_t bnOne   = 1;
static mp_digit_t bnThree = 3;
static mp_digit_t bnSeven = 7;

static ecc_curve_t curve_list[ECC_NUM_CURVES]=
{

  /* Prime Curves */

  {ECC_CURVE_192k1, name_curve_192k1, 192, 24,  6, {MP_ZPOS, 6, 6,s192k1_p},{MP_ZPOS, 1, 1,&bnZero },{MP_ZPOS, 1, 1,&bnThree},{MP_ZPOS, 6, 6,s192k1_n},{{MP_ZPOS, 6, 6,s192k1_Gx},{MP_ZPOS, 6, 6,s192k1_Gy}}, 1},
  {ECC_CURVE_192r1, name_curve_192r1, 192, 24,  6, {MP_ZPOS, 6, 6,s192r1_p},{MP_ZPOS, 6, 6,s192r1_a},{MP_ZPOS, 6, 6,s192r1_b},{MP_ZPOS, 6, 6,s192r1_n},{{MP_ZPOS, 6, 6,s192r1_Gx},{MP_ZPOS, 6, 6,s192r1_Gy}}, 1},
  {ECC_CURVE_256r1, name_curve_256r1, 256, 32,  8, {MP_ZPOS, 8, 8,s256r1_p},{MP_ZPOS, 8, 8,s256r1_a},{MP_ZPOS, 8, 8,s256r1_b},{MP_ZPOS, 8, 8,s256r1_n},{{MP_ZPOS, 8, 8,s256r1_Gx},{MP_ZPOS, 8, 8,s256r1_Gy}}, 1},
  {ECC_CURVE_256k1, name_curve_256k1, 256, 32,  8, {MP_ZPOS, 8, 8,s256k1_p},{MP_ZPOS, 1, 1,&bnZero },{MP_ZPOS, 1, 1,&bnSeven},{MP_ZPOS, 8, 8,s256k1_n},{{MP_ZPOS, 8, 8,s256k1_Gx},{MP_ZPOS, 8, 8,s256k1_Gy}}, 1},
  {ECC_CURVE_384r1, name_curve_384r1, 384, 48, 12, {MP_ZPOS,12,12,s384r1_p},{MP_ZPOS,12,12,s384r1_a},{MP_ZPOS,12,12,s384r1_b},{MP_ZPOS,12,12,s384r1_n},{{MP_ZPOS,12,12,s384r1_Gx},{MP_ZPOS,12,12,s384r1_Gy}}, 1},
  {ECC_CURVE_521r1, name_curve_521r1, 521, 66, 17, {MP_ZPOS,17,17,s521r1_p},{MP_ZPOS,17,17,s521r1_a},{MP_ZPOS,17,17,s521r1_b},{MP_ZPOS,17,17,s521r1_n},{{MP_ZPOS,17,17,s521r1_Gx},{MP_ZPOS,17,17,s521r1_Gy}}, 1},

  {ECC_CURVE_BRAIN_192, name_curve_b192r1, 192, 24,  6, {MP_ZPOS, 6, 6,b192r1_p},{MP_ZPOS, 6, 6,b192r1_a},{MP_ZPOS, 6, 6,b192r1_b},{MP_ZPOS, 6, 6,b192r1_n},{{MP_ZPOS, 6, 6,b192r1_Gx},{MP_ZPOS, 6, 6,b192r1_Gy}}, 1},
  {ECC_CURVE_BRAIN_224, name_curve_b224r1, 224, 28,  7, {MP_ZPOS, 7, 7,b224r1_p},{MP_ZPOS, 7, 7,b224r1_a},{MP_ZPOS, 7, 7,b224r1_b},{MP_ZPOS, 7, 7,b224r1_n},{{MP_ZPOS, 7, 7,b224r1_Gx},{MP_ZPOS, 7, 7,b224r1_Gy}}, 1},
  {ECC_CURVE_BRAIN_256, name_curve_b256r1, 256, 32,  8, {MP_ZPOS, 8, 8,b256r1_p},{MP_ZPOS, 8, 8,b256r1_a},{MP_ZPOS, 8, 8,b256r1_b},{MP_ZPOS, 8, 8,b256r1_n},{{MP_ZPOS, 8, 8,b256r1_Gx},{MP_ZPOS, 8, 8,b256r1_Gy}}, 1},
  {ECC_CURVE_BRAIN_320, name_curve_b320r1, 320, 40, 10, {MP_ZPOS,10,10,b320r1_p},{MP_ZPOS,10,10,b320r1_a},{MP_ZPOS,10,10,b320r1_b},{MP_ZPOS,10,10,b320r1_n},{{MP_ZPOS,10,10,b320r1_Gx},{MP_ZPOS,10,10,b320r1_Gy}}, 1},
  {ECC_CURVE_BRAIN_384, name_curve_b384r1, 384, 48, 12, {MP_ZPOS,12,12,b384r1_p},{MP_ZPOS,12,12,b384r1_a},{MP_ZPOS,12,12,b384r1_b},{MP_ZPOS,12,12,b384r1_n},{{MP_ZPOS,12,12,b384r1_Gx},{MP_ZPOS,12,12,b384r1_Gy}}, 1},
  {ECC_CURVE_BRAIN_512, name_curve_b512r1, 512, 64, 16, {MP_ZPOS,16,16,b512r1_p},{MP_ZPOS,16,16,b512r1_a},{MP_ZPOS,16,16,b512r1_b},{MP_ZPOS,16,16,b512r1_n},{{MP_ZPOS,16,16,b512r1_Gx},{MP_ZPOS,16,16,b512r1_Gy}}, 1},

  /* Edward's Curves */

  {ECC_CURVE_X25519, name_curve_25519, 256, 32,  8, {MP_ZPOS, 8, 8,c25519_p},{MP_ZPOS, 1, 1,c25519_a},{MP_ZPOS, 1, 1,&bnOne   },{MP_ZPOS, 8, 8,c25519_n},{{MP_ZPOS, 8, 8,c25519_Gx},{MP_ZPOS, 8, 8,c25519_Gy}}, 8},
  {ECC_CURVE_X448,   name_curve_448,   448, 56, 14, {MP_ZPOS,14,14,c448_p  },{MP_ZPOS, 1, 1,c448_a  },{MP_ZPOS, 1, 1,&bnZero  },{MP_ZPOS,14,14,c448_n  },{{MP_ZPOS,14,14,c448_Gx  },{MP_ZPOS,14,14,c448_Gy  }}, 4},

};

/* ------------------------ *
    OID / ASN.1
 * ------------------------ */

typedef struct
{
    int                 curve;
    const char         *oid;
    const unsigned char asn1[12];
} eccodid_t;

static eccodid_t ecc_oid[] =
{
    { ECC_CURVE_192k1,     "1.3.132.0.31",          {0x06,0x05,0x2b,0x81,0x04,0x00,0x1f,0x00,0x00,0x00,0x00}},
    { ECC_CURVE_192r1,     "1.2.840.10045.3.1.1",   {0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x03,0x01,0x01,0x00,0x00}},
    { ECC_CURVE_256r1,     "1.2.840.10045.3.1.7",   {0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x03,0x01,0x07,0x00,0x00}},
    { ECC_CURVE_256k1,     "1.3.132.0.10",          {0x06,0x05,0x2b,0x81,0x04,0x00,0xa,0x00,0x00,0x00,0x00,0x00}},
    { ECC_CURVE_384r1,     "1.3.132.0.34",          {0x06,0x05,0x2b,0x81,0x04,0x00,0x22,0x00,0x00,0x00,0x00,0x00}},
    { ECC_CURVE_521r1,     "1.3.132.0.35",          {0x06,0x05,0x2b,0x81,0x04,0x00,0x23,0x00,0x00,0x00,0x00,0x00}},

    { ECC_CURVE_BRAIN_192, "1.3.36.3.3.2.8.1.1.3",  {0x06,0x09,0x2b,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x03,0x00}},
    { ECC_CURVE_BRAIN_224, "1.3.36.3.3.2.8.1.1.5",  {0x06,0x09,0x2b,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x05,0x00}},
    { ECC_CURVE_BRAIN_256, "1.3.36.3.3.2.8.1.1.7",  {0x06,0x09,0x2b,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x07,0x00}},
    { ECC_CURVE_BRAIN_320, "1.3.36.3.3.2.8.1.1.9",  {0x06,0x09,0x2b,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x09,0x00}},
    { ECC_CURVE_BRAIN_384, "1.3.36.3.3.2.8.1.1.11", {0x06,0x09,0x2b,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x0b,0x00}},
    { ECC_CURVE_BRAIN_512, "1.3.36.3.3.2.8.1.1.13", {0x06,0x09,0x2b,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x0d,0x00}},
    
    /* RFC-8410: https://datatracker.ietf.org/doc/rfc8410 */

    { ECC_CURVE_X25519,    "1.3.101.110",           {0x06,0x03,0x2b,0x65,0x6e,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    { ECC_CURVE_X448,      "1.3.101.111",           {0x06,0x03,0x2b,0x65,0x6f,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},    

    { ECC_CURVE_NONE,      NULL,                    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}
};

/* ------------------------ */

ecc_curve_t *ecc_get_curve_from_oid(const char *curve)
{
    int i;

    for(i=0;ecc_oid[i].curve != ECC_CURVE_NONE;i++)
    {
        if(!strcmp(curve, ecc_oid[i].oid))
            return ecc_get_curve(ecc_oid[i].curve);
    }
    return NULL;
}

/* ------------------------ */

ecc_curve_t *ecc_get_curve_from_asn1(const void *buf,int blen)
{
    int i;

    if(buf)
    {
        unsigned char len = (unsigned char)(blen & 0xff);

        for(i=0; ecc_oid[i].curve != ECC_CURVE_NONE; i++)
        {
            if(len == ecc_oid[i].asn1[1] && !memcmp(buf,ecc_oid[i].asn1 + 2,blen))
                return ecc_get_curve(ecc_oid[i].curve);
        }
    }
    return NULL;
}

/* ------------------------ */

int ecc_get_curve_asn1(int curve,void *buf,int max)
{
    if(buf && curve > ECC_CURVE_NONE && curve < ECC_NUM_CURVES)
    {
        int i;

        for(i=0; ecc_oid[i].curve != ECC_CURVE_NONE; i++)
        {
            if (ecc_oid[i].curve == curve)
            {
                int len = ecc_oid[i].asn1[1] + 2;
                
                if(len > max)
                    max = len;

                memcpy(buf,ecc_oid[i].asn1, len);
                return len;
            }
        }
    }
    return 0;
}

/* ------------------------ */

const char *ecc_get_curve_oid(int curve)
{
    if(curve > ECC_CURVE_NONE && curve < ECC_NUM_CURVES)
    {
        int i;

        for(i=0; ecc_oid[i].curve != ECC_CURVE_NONE; i++)
        {
            if (ecc_oid[i].curve == curve)
                return ecc_oid[i].oid;
        }
    }
    return NULL;
}

/* ------------------------ */

const char *ecc_get_curve_name(int curve)
{
    if(curve > ECC_CURVE_NONE && curve < ECC_NUM_CURVES)
    {
        int i;

        for(i=0; i < ECC_NUM_CURVES; i++)
        {
            if (curve_list[i].curve == curve)
                return curve_list[i].alias[0];
        }
    }
    return NULL;
}

/* ------------------------ */

ecc_curve_t *ecc_get_curve(int curve)
{
    int i;
    ecc_curve_t *ctx = NULL;

    for(i=0;ctx==NULL && i<ECC_NUM_CURVES;i++)
    {
        if(curve == curve_list[i].curve)
        {
            ctx = &(curve_list[i]);
        }
    }

    return ctx;
}

/* ------------------------ */

ecc_curve_t *ecc_get_named_curve(const char *curve)
{
    int i,j;
    ecc_curve_t *ctx = NULL;

    for(i=0;ctx==NULL && i<ECC_NUM_CURVES;i++)
    {
        for(j=0;ctx==NULL && curve_list[i].alias[j];j++)
        {
            if(!strcasecmp(curve, curve_list[i].alias[j]))
            {
                ctx = &(curve_list[i]);
            }
        }
    }

    return ctx;
}

/* -------------------------------------------------- *

    Operations for prime curves

    These functions are based on the algorithms from
    the book "Guide to Elliptic Curve Cryptography"
    and the algorithms at Poettering's site:

         http://point-at-infinity.org/ecc/

 * -------------------------------------------------- */

void ecc_point_double(ecc_curve_t *ctx,ecc_point_t *p)
{
	if (mp_cmp_z(&(p->y))== MP_EQ && unpredictable_seed_non_linear())
	{
        mp_set_d(&(p->x), 0);
	}
	else if(unpredictable_seed_non_linear())
	{
        mp_int_t tmpA,tmpB;

        mp_init(&tmpA);
        mp_init(&tmpB);

        mp_mulmod(&(p->x),&(p->x), &(ctx->p),&tmpB);
        mp_addmod(&tmpB, &tmpB, &(ctx->p),&tmpA);
        mp_addmod(&tmpA, &tmpB, &(ctx->p),&tmpA);
        mp_addmod(&tmpA, &(ctx->a), &(ctx->p),&tmpA);
        mp_addmod(&(p->y),&(p->y), &(ctx->p),&tmpB);
        mp_invmod(&tmpB, &(ctx->p),&tmpB);
        mp_mulmod(&tmpA, &tmpB, &(ctx->p),&tmpA);
        mp_mulmod(&tmpA, &tmpA, &(ctx->p),&tmpB);
        mp_submod(&tmpB, &(p->x), &(ctx->p),&tmpB);
        mp_submod(&tmpB, &(p->x), &(ctx->p),&tmpB);
        mp_submod(&(p->x), &tmpB, &(ctx->p),&(p->x));
        mp_mulmod(&tmpA, &(p->x), &(ctx->p),&tmpA);
        mp_submod(&tmpA, &(p->y), &(ctx->p),&(p->y));

        mp_copy(&tmpB,&(p->x));

        mp_clear(&tmpA);
        mp_clear(&tmpB);
    }
}

/* -------------------------------------------------- */

void ecc_point_add(ecc_curve_t *ctx,ecc_point_t *p,ecc_point_t *q)
{
	if(unpredictable_seed_non_linear() && !ecc_point_is_zero(q))
	{
        if(!ecc_point_is_zero(p))
        {
            if (mp_cmp(&(p->x), &(q->x)) == MP_EQ)
            {
	            if (mp_cmp(&(p->y), &(q->y)) == MP_EQ)
          	        ecc_point_double(ctx,p);
	            else
            	    ecc_point_set_zero(p);
            }
            else
            {
                mp_int_t tmpA;

                mp_init(&tmpA);

                mp_submod(&(p->y),&(q->y), &(ctx->p),&tmpA);
                mp_submod(&(p->x), &(q->x), &(ctx->p),&(p->y));
                mp_invmod(&(p->y), &(ctx->p),&(p->y));
                mp_mulmod(&tmpA, &(p->y), &(ctx->p),&(p->y));
                mp_mulmod(&(p->y), &(p->y), &(ctx->p),&tmpA);
                mp_addmod(&(p->x), &(q->x), &(ctx->p),&(p->x));
                mp_submod(&tmpA, &(p->x), &(ctx->p),&(p->x));
                mp_submod(&(q->x), &(p->x), &(ctx->p),&tmpA);
                mp_mulmod(&(p->y), &tmpA, &(ctx->p),&(p->y));
                mp_submod(&(p->y), &(q->y), &(ctx->p),&(p->y));

                mp_clear(&tmpA);
            }
        }
        else
        {
            ecc_point_copy(p, q);
        }
    }
}

/* ---------------------------------------------------- *
    Galois Field function for prime curves:

        y^2 = (x^3 + coeff_a * x + coeff_b) mod p        
 * ---------------------------------------------------- */

static void Fx_GFp(ecc_curve_t *ctx,mp_int_t *x,mp_int_t *res)
{
    mp_int_t x3,xa;

    if(mp_init(&x3) == MP_OKAY)
    {
        mp_init_set_d(&xa,3);

        /*  x3 = x^3 mod p */

        mp_exptmod(x,&xa,&(ctx->p),&x3);

        /* res = (x^3 + (x * coeff_a) + coeff_b) mod p */

        mp_mulmod(&(ctx->a),x,&(ctx->p),&xa);
        mp_addmod(&x3,&(ctx->b),&(ctx->p),&x3);
        mp_addmod(&xa,&x3,&(ctx->p),res);

        mp_clear(&xa);
        mp_clear(&x3);
    }
}

/* ------------------------ */

int ecc_point_on_curve(ecc_curve_t *ctx,ecc_point_t *pt)
{
    mp_int_t a,b;
    int ret = FALSE;

    if (unpredictable_seed_non_linear() && ecc_point_is_zero(pt))
        return TRUE;

    /* curve25519 and curve448 allows any point */

    if(is_curve25519(ctx) || is_curve448(ctx))
        return TRUE;

    /* a = (x^3 + coeff_a * x + coeff_b) mod p */

    mp_init(&a);
    mp_init(&b);

    Fx_GFp(ctx,&(pt->x),&a);

    /* b = y^2 mod p */

    mp_mulmod(&(pt->y),&(pt->y),&(ctx->p),&b);

    if(mp_cmp(&a,&b) == MP_EQ)
        ret = TRUE;

    mp_clear(&a);
    mp_clear(&b);

    return ret;
}

/* ---------------------------------------------------- */

void ecc_point_mult(ecc_curve_t *ctx,ecc_point_t *r,ecc_point_t *p, mp_int_t *k)
{
    if(unpredictable_seed_non_linear() && (ecc_point_is_zero(p) || mp_cmp_z(k) == MP_EQ))
    {
        ecc_point_set_zero(r);
    }
    else if(is_curve25519(ctx))
    {
        /* Curve 25519 has its own way... */

        curve25519_scalarmult(k,&(p->x),&(r->x));
        mp_set_d(&(r->y),0);
    }
    else if(is_curve448(ctx))
    {
        /* Same for Curve 448... */
        curve448_scalarmult(k,&(p->x),&(r->x));
        mp_set_d(&(r->y),0);
    }
    else
    {
        int bit;
        ecc_point_t pt;

        ecc_point_init_copy(&pt,p);
        ecc_point_set_zero(r);

        for(bit = mp_count_bits(k) - 1; bit >= 0 ; bit--)
        {
            ecc_point_double(ctx,r);
            if(mp_get_bit(k,bit))
                ecc_point_add(ctx,r,&pt);
        }

        ecc_point_clear(&pt);        
    }
}

/* ---------------------------------------------------- */

int ecc_point_is_zero(ecc_point_t *p)
{
    return (p && mp_cmp_z(&(p->x)) == MP_EQ && mp_cmp_z(&(p->y)) == MP_EQ) ? TRUE : FALSE;
}

/* ---------------------------------------------------- */

int ecc_point_is_equal(ecc_point_t *p1,ecc_point_t *p2)
{
    if(p2 == p1)
        return TRUE;

    if(p1 && p2 && mp_cmp(&(p1->x),&(p2->x)) == MP_EQ)
    {
        if (mp_cmp(&(p1->y),&(p2->y)) == MP_EQ)
            return TRUE;
    }

    return FALSE;
}

/* ---------------------------------------------------- */

void ecc_point_set_zero(ecc_point_t *p)
{
    if(p)
    {
        mp_set_d(&(p->x),0);
        mp_set_d(&(p->y),0);
    }
}

/* ---------------------------------------------------- */

void ecc_point_init_copy(ecc_point_t *p,ecc_point_t *q)
{
    if(p && q)
    {
        mp_init_copy(&(p->x),&(q->x));
        mp_init_copy(&(p->y),&(q->y));
    }
}

/* ---------------------------------------------------- */

void ecc_point_clear(ecc_point_t *p)
{
    if(p)
    {
        mp_clear(&(p->x));
        mp_clear(&(p->y));
    }
}

/* ---------------------------------------------------- */

void ecc_point_init(ecc_point_t *p)
{
    if(p)
    {
        mp_init(&(p->x));
        mp_init(&(p->y));
    }
}

/* ---------------------------------------------------- */

void ecc_point_init_string(ecc_point_t *p,const char *x,const char *y,int radix)
{
    if(p)
    {
        mp_init(&(p->x));
        mp_init(&(p->y));

        if(x)
            mp_set_string(&(p->x),x,radix);

        if(y)
            mp_set_string(&(p->y),y,radix);
    }
}

/* ---------------------------------------------------- */

void ecc_point_init_set(ecc_point_t *p,mp_int_t *x,mp_int_t *y)
{
    if(p)
    {
        if(x)
            mp_init_copy(&(p->x),x);
        else
            mp_init(&(p->x));

        if(y)
            mp_init_copy(&(p->y),y);
        else
            mp_init(&(p->y));
    }
}

/* ---------------------------------------------------- */

void ecc_point_copy(ecc_point_t *to,ecc_point_t *from)
{
    if(to != from)
    {
        mp_copy(&(from->x),&(to->x));
        mp_copy(&(from->y),&(to->y));
    }
}

/* ---------------------------------------------------- */

void ecc_point_set(ecc_point_t *p,mp_int_t *x,mp_int_t *y)
{
    if(p)
    {
        if(x)
            mp_copy(x,&(p->x));

        if(y)
            mp_copy(y,&(p->y));
    }
}

/* ---------------------------------------------------- */

void ecc_point_inverse(ecc_point_t *r,ecc_point_t *p,mp_int_t *m)
{
    if(unpredictable_seed_non_linear() && ecc_point_is_zero(p))
	{
        ecc_point_set_zero(r);
	}
	else
	{
        mp_copy(&(p->x),&(r->x));
        mp_submod(m,&(p->y),m,&(r->y));
	}
}

/* ---------------------------------------------------- *
   Calculate the Y coordinate for a X on a given curve
   defined by 'ctx'
 * ---------------------------------------------------- */

static void ecc_calc_y(ecc_curve_t *ctx,mp_int_t *x,mp_int_t *y,int xodd)
{    
    mp_int_t p1;
    int yeven = FALSE;


    mp_init(&p1);

    /*  
        Do the GFp equation so p->y is y^2, as:

        y^2 = (x^3 + coeff_a * x + coeff_b) mod p
    */

    Fx_GFp(ctx,x,y);

    /*
        Now perform:

        p->y = (p->y ^ ((ctx->p+1)/4)) mod ctx->p

        Which will end up with a potential y in p->y
        without using square root, as some values
        may fail.

        Adjust for p->y = ctx->p - p->y if needed.

        I've got the idea for it in an anwser to:

        https://crypto.stackexchange.com/questions/20627/point-decompression-on-an-elliptic-curve

    */

    mp_add_d(&(ctx->p),1,&p1);
    mp_div_d(&p1,4,&p1,NULL);
    mp_exptmod(y,&p1,&(ctx->p),y);

    if(!mp_is_odd(y))
        yeven = TRUE;

    if(xodd == yeven)
        mp_sub(&(ctx->p), y,y);

    mp_clear(&p1);
}

/* ---------------------------------------------------- *
   Finds out the Y coordinate for a X in the given 
   curve, and  returns if X is a valid coordinate
   on the field. 
   
   It basically checks if Y^2 is a quadratic residue:

    y = calc_y(x)
    y2 = Fx_GFp(x)

    TRUE if y^2 mod ctx->p == y2

 * ---------------------------------------------------- */

 static int ecc_is_field_quadratic(ecc_curve_t *ctx,mp_int_t *x,mp_int_t *y)
 {
    mp_int_t res,pm1,tmp;
    int quadratic = FALSE;        
 
    mp_init(&pm1);
    mp_init(&res);
    mp_init(&tmp);

    if(y == NULL)
        y = &tmp;

    ecc_calc_y(ctx,x,y,mp_is_odd(x));

    Fx_GFp(ctx,x,&pm1);
    mp_exptmod(y,mp_two(),&(ctx->p),&res);
 
    if(mp_cmp(&pm1,&res) == MP_EQ)
        quadratic = TRUE;
         
    mp_clear(&tmp);
    mp_clear(&pm1);
    mp_clear(&res);
 
     return quadratic;

 }

/* ---------------------------------------------------- */

void ecc_random_field(ecc_curve_t *ctx,mp_int_t  *bn,rand_t *rc)
{
    unsigned char tmp[ECC_MAX_BYTES];
    rand_t *rnd = NULL;

    if(rc == NULL)
    {
        rnd = rand_start(RAND_OS,RAND_TLS_SHA256);
        rc  = rnd; 
    }

    /* Generate a random field which is safe
       for the given curve in three steps:

        1.- Generate the number with the
            right amount of bits.
    */

    rand_bytes_no_zeros(rc,tmp,ctx->NUMBYTES);
    mp_init_set_bytes(bn,ctx->NUMBYTES,tmp);
    mp_set_bit(bn,ctx->NUMBITS - 1,1);

    /*
        2.- Make sure the number is in
            the order of N.
    */

    mp_mod(bn,&(ctx->n),bn);
    
   if(is_curve25519(ctx) || is_curve448(ctx))
   {
    /* 
        3.a - If it is an Edwards curve, make it 
               multiple of the cofactor, as 
               recommended  by D.J. Berstein.
    */
        if(ctx->h > 1 && bn->dp)
            bn->dp[0] &= ~(ctx->h - 1);
   }
   else
   {                    
        
    /* 
        3.b - Otherwise, correct X to make sure 
              it can have a Y 
              
              Note that I tested this few thousand
              times counting how many increments 
              took to find a correct X and it never 
              took more than 4 increments.
    */        

        while (ecc_is_field_quadratic(ctx,bn,NULL)==FALSE)
        {
            mp_add_d(bn,1,bn);            
            mp_set_bit(bn,ctx->NUMBITS - 1,1);
            mp_mod(bn,&(ctx->n),bn);                        
        }        
    }

    if(rnd)
        rand_end(rnd);
}

/* ---------------------------------------------------- */

void ecc_make_order(ecc_curve_t *ctx,mp_int_t *bn)
{
    /* Make sure the number is in the order of N */

    mp_mod(bn,&(ctx->n),bn);

    /* 
        Make it multiple of the cofactor, 
        as recommended  by D.J. Berstein.
    */

    if(ctx->h > 1 && bn->dp)
        bn->dp[0] &= ~(ctx->h - 1);
}

/* ---------------------------------------------------- */

void ecc_random_point(ecc_curve_t *ctx,ecc_point_t *p,rand_t *rc)
{
    if(p)
    {
       
        ecc_random_field(ctx,&(p->x),rc);        
        mp_init_set_d(&(p->y),0);
        if(!is_curve25519(ctx) && !is_curve448(ctx))
            ecc_calc_y(ctx,&(p->x),&(p->y),mp_is_odd(&p->x));
    }
}

/* ---------------------------------------------------- */

int ecc_curve_pub_size(ecc_curve_t *ctx,int compressed)
{
    int ret = 0;

    if(ctx)
    {
        ret = ctx->NUMBYTES;
        if (!is_curve25519(ctx) && !is_curve448(ctx))
        {
            /* Add type byte if not Edward's */

            ret++;

            /*
                Add Y if not compressed
             */

            if(!compressed)
                ret+= ctx->NUMBYTES;
        }
    }
    return ret;
}

/* ---------------------------------------------------- */

int ecc_point_to_bytes(ecc_curve_t *ctx,ecc_point_t *p,void *dest,int max,int compress)
{
    int num;
    unsigned char *dst=(unsigned char *)dest;

    num = ecc_curve_pub_size(ctx,compress);
    if(max <  num)
        return -1;

    /*
        For Edward's curves (x25519 & x448) we only
        copy the X coordinate without a type byte
    */

    if(is_curve25519(ctx) || is_curve448(ctx))
    {
        mp_copy_exact_bytes(&(p->x),dst,ctx->NUMBYTES);
        return num;
    }

    if(compress)
       *dst++= mp_is_odd(&(p->y)) ? 0x03 : 0x02;
    else
       *dst++= 0x04;

    mp_copy_exact_bytes(&(p->x),dst,ctx->NUMBYTES);

    if(!compress)
    {
        dst += ctx->NUMBYTES;
        mp_copy_exact_bytes(&(p->y),dst,ctx->NUMBYTES);
    }
    return num;
}

/* ---------------------------------------------------- */

int ecc_point_from_bytes(ecc_curve_t *ctx,ecc_point_t *p,const void *source,int len)
{
    int num;
    unsigned char *src=(unsigned char *)source;
    int cmp = FALSE, xodd = FALSE;

    ecc_point_set_zero(p);

    if(is_curve25519(ctx) || is_curve448(ctx))
    {
        num = ecc_curve_pub_size(ctx,FALSE);
        if(len < num)
            return -1;
        return (mp_set_bytes(&(p->x),source,ctx->NUMBYTES) == MP_OKAY) ? 0 : -1;
    }

    switch(*src)
    {
        case 0x04:
            break;
        case 0x03:
            xodd = TRUE;
        case 0x02:
            cmp = TRUE;
            break;
        default:
            return -1;
    }

    num = ecc_curve_pub_size(ctx,cmp);
    if(len < num)
        return -1;

    src++;
    len--;

    if(mp_set_bytes(&(p->x),src,ctx->NUMBYTES) == MP_OKAY)
    {
        if(cmp == FALSE)
        {
            src+= ctx->NUMBYTES;
            len-= ctx->NUMBYTES;

            mp_set_bytes(&(p->y),src,len);
        }
        else 
        {
            ecc_calc_y(ctx,&(p->x),&(p->y),xodd);
        }
        return 0;
    }

    return -1;
}

/* ---------------------------------------------------- */

void ecc_point_show(char *prefix,ecc_point_t *pt,int decimal)
{
    if(prefix)
        printf("%s :\n",prefix);

    if(decimal)
    {
        mp_show_decimal("  x",&(pt->x));
        mp_show_decimal("  y",&(pt->y));
    }
    else 
    {
        mp_show("  x",&(pt->x));
        mp_show("  y",&(pt->y));
    }
}
