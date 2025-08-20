# ECC
## Elliptic Curve Criptography

Before pictures were much bigger than the whole of Windows 95 (~30 Mb), people was really concerned about the size of ```RSA``` keys and the computational performance of the algorithm. So, they did something about it and divised a new criptography technique based on elliptic curves over finite -Galois- fields, a very interesting part of computational geometry.  

Elliptic Curve Criptography bore a much smaller keys to -theoretically- do the same of bigger key algorithms. For example, an ```ECC``` key of 256 bits equalled the security of a counterpart with a 2048 bits key, that is less computational effort and smaller data sizes for the same benefit. 

Now, I said *theoretically* there because in reality ```ECC``` functions do not do **exactly the same** of the other algorithms, as they do not encode/decode data like ```RSA``` or ```AES``` do -if we ignore Elgamal at the moment-, but they can be used to produce signatures or perform key agreement, which is ideal to create smaller certificates and TLS handshakes.

There are many ```ECC``` algorithms, but here we present a small library that uses our typical one-stop shop approach to support the most common curves around, all of them prime curves:

    - Secp: secp192k1, secp192r1, secp256r1, secp256k1, secp384r1, secp521r1
    - Brainpool:  brainpool192r1, brainpool224r1, brainpool256r1 , brainpool320r1, brainpool384r1, brainpool512r1
    - Edwards: curve25519, curve448

This can be used as the basis of the same protocols used with the normal encription algorithms: ```DSA```, ```DH```, etc.

This algorithms are also vulnerable to the infamous -but great- Shor's algorithm for *Quantum computers*.

## To build it:

For Microsoft C/C++ for Windows (Visual Studio C++) use:  

```
nmake /f msc.mak test
```

For Macintosh, Linux and others using GNU C:

```
make -f gcc.mak test
```
### To clean the builds:

Do the same that for build it but use *cleanit* instead of *test* as the target.

## Example output from the test program:

```

$./test_ecc

== CURVES TEST START 

===== TEST CURVE 25519 =====

Scalar 1  = 0xa546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4
It Must Be: 0xa546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4
Coord  1  = 0xe6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c
It Must Be: 0xe6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c
Result 1  = 0xc3da55379de9c6908e94ea4df28d084f32eccf03491c71f754b4075577a28552
It Must Be: 0xc3da55379de9c6908e94ea4df28d084f32eccf03491c71f754b4075577a28552

Scalar 2  = 0x4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d
It Must Be: 0x4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d
Coord  2  = 0xe5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493
It Must Be: 0xe5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493
Result 2  = 0x95cbde9476e8907d7aade45cb4b873f88b595a68799fa152e6f8f7647aac7957
It Must Be: 0x95cbde9476e8907d7aade45cb4b873f88b595a68799fa152e6f8f7647aac7957

===== TEST CURVE 448 =====

Scalar 1  = 0x3d262fddf9ec8e88495266fea19a34d28882acef045104d0d1aae121700a779c984c24f8cdd78fbff44943eba368f54b29259a4f1c600ad3
It Must Be: 0x3d262fddf9ec8e88495266fea19a34d28882acef045104d0d1aae121700a779c984c24f8cdd78fbff44943eba368f54b29259a4f1c600ad3
Coord  1  = 0x6fce640fa3487bfda5f6cf2d5263f8aad88334cbd07437f020f08f9814dc031ddbdc38c19c6da2583fa5429db94ada18aa7a7fb4ef8a086
It Must Be: 0x6fce640fa3487bfda5f6cf2d5263f8aad88334cbd07437f020f08f9814dc031ddbdc38c19c6da2583fa5429db94ada18aa7a7fb4ef8a086
Result 1  = 0xce3e4ff95a60dc6697da1db1d85e6afbdf79b50a2412d7546d5f239fe14fbaadeb445fc66a01b0779d98223961111e21766282f73dd96b6f
It Must Be: 0xce3e4ff95a60dc6697da1db1d85e6afbdf79b50a2412d7546d5f239fe14fbaadeb445fc66a01b0779d98223961111e21766282f73dd96b6f

Scalar 2  = 0x203d494428b8399352665ddca42f9de8fef600908e0d461cb021f8c538345dd77c3e4806e25f46d3315c44e0a5b4371282dd2c8d5be3095f
It Must Be: 0x203d494428b8399352665ddca42f9de8fef600908e0d461cb021f8c538345dd77c3e4806e25f46d3315c44e0a5b4371282dd2c8d5be3095f
Coord  2  = 0xfbcc2f993cd56d3305b0b7d9e55d4c1a8fb5dbb52f8e9a1e9b6201b165d015894e56c4d3570bee52fe205e28a78b91cdfbde71ce8d157db
It Must Be: 0xfbcc2f993cd56d3305b0b7d9e55d4c1a8fb5dbb52f8e9a1e9b6201b165d015894e56c4d3570bee52fe205e28a78b91cdfbde71ce8d157db
Result 2  = 0x884a02576239ff7a2f2f63b2db6a9ff37047ac13568e1e30fe63c4a7ad1b3ee3a5700df34321d62077e63633c575c1c954514e99da7c179d
It Must Be: 0x884a02576239ff7a2f2f63b2db6a9ff37047ac13568e1e30fe63c4a7ad1b3ee3a5700df34321d62077e63633c575c1c954514e99da7c179d


===== Test Prime Points =====
Curve 'secp192r1': p=192 bits, n=192 bits, a=192 bits, b=191 bits

S :
  x = 0xd458e7d127ae671b0c330266d246769353a012073e97acf8
  y = 0x325930500d851f336bddc050cf7fb11b5673a1645086df3b
T :
  x = 0xf22c4395213e9ebe67ddecdd87fdbd01be16fb059b9753a4
  y = 0x264424096af2b3597796db48f8dfb41fa9cecc97691a9c79
R = S + T :
  x = 0x48e1e4096b9b8e5ca9d0f1f077b8abf58e843894de4d0290
  y = 0x408fa77c797cd7dbfb16aa48a3648d3d63c94117d7b6aa4b
Should be :
  x = 0x48e1e4096b9b8e5ca9d0f1f077b8abf58e843894de4d0290
  y = 0x408fa77c797cd7dbfb16aa48a3648d3d63c94117d7b6aa4b
Which is CORRECT

S :
  x = 0xd458e7d127ae671b0c330266d246769353a012073e97acf8
  y = 0x325930500d851f336bddc050cf7fb11b5673a1645086df3b
Double S  :
  x = 0x30c5bc6b8c7da25354b373dc14dd8a0eba42d25a3f6e6962
  y = 0xdde14bc4249a721c407aedbf011e2ddbbcb2968c9d889cf
Should be :
  x = 0x30c5bc6b8c7da25354b373dc14dd8a0eba42d25a3f6e6962
  y = 0xdde14bc4249a721c407aedbf011e2ddbbcb2968c9d889cf
Which is CORRECT

S :
  x = 0xd458e7d127ae671b0c330266d246769353a012073e97acf8
  y = 0x325930500d851f336bddc050cf7fb11b5673a1645086df3b
  d = 0xa78a236d60baec0c5dd41b33a542463a8255391af64c74ee
Q = d * S :
  x = 0x1faee4205a4f669d2d0a8f25e3bcec9a62a6952965bf6d31
  y = 0x5ff2cdfa508a2581892367087c696f179e7a4d7e8260fb06
Should be :
  x = 0x1faee4205a4f669d2d0a8f25e3bcec9a62a6952965bf6d31
  y = 0x5ff2cdfa508a2581892367087c696f179e7a4d7e8260fb06
Which is CORRECT

Curve 'secp256r1': p=256 bits, n=256 bits, a=256 bits, b=255 bits

S :
  x = 0xde2444bebc8d36e682edd27e0f271508617519b3221a8fa0b77cab3989da97c9
  y = 0xc093ae7ff36e5380fc01a5aad1e66659702de80f53cec576b6350b243042a256
T :
  x = 0x55a8b00f8da1d44e62f6b3b25316212e39540dc861c89575bb8cf92e35e0986b
  y = 0x5421c3209c2d6c704835d82ac4c3dd90f61a8a52598b9e7ab656e9d8c8b24316
R = S + T :
  x = 0x72b13dd4354b6b81745195e98cc5ba6970349191ac476bd4553cf35a545a067e
  y = 0x8d585cbb2e1327d75241a8a122d7620dc33b13315aa5c9d46d013011744ac264
Should be :
  x = 0x72b13dd4354b6b81745195e98cc5ba6970349191ac476bd4553cf35a545a067e
  y = 0x8d585cbb2e1327d75241a8a122d7620dc33b13315aa5c9d46d013011744ac264
Which is CORRECT

S :
  x = 0xde2444bebc8d36e682edd27e0f271508617519b3221a8fa0b77cab3989da97c9
  y = 0xc093ae7ff36e5380fc01a5aad1e66659702de80f53cec576b6350b243042a256
Double S  :
  x = 0x7669e6901606ee3ba1a8eef1e0024c33df6c22f3b17481b82a860ffcdb6127b0
  y = 0xfa878162187a54f6c39f6ee0072f33de389ef3eecd03023de10ca2c1db61d0c7
Should be :
  x = 0x7669e6901606ee3ba1a8eef1e0024c33df6c22f3b17481b82a860ffcdb6127b0
  y = 0xfa878162187a54f6c39f6ee0072f33de389ef3eecd03023de10ca2c1db61d0c7
Which is CORRECT

S :
  x = 0xde2444bebc8d36e682edd27e0f271508617519b3221a8fa0b77cab3989da97c9
  y = 0xc093ae7ff36e5380fc01a5aad1e66659702de80f53cec576b6350b243042a256
  d = 0xc51e4753afdec1e6b6c6a5b992f43f8dd0c7a8933072708b6522468b2ffb06fd
Q = d * S :
  x = 0x51d08d5f2d4278882946d88d83c97d11e62becc3cfc18bedacc89ba34eeca03f
  y = 0x75ee68eb8bf626aa5b673ab51f6e744e06f8fcf8a6c0cf3035beca956a7b41d5
Should be :
  x = 0x51d08d5f2d4278882946d88d83c97d11e62becc3cfc18bedacc89ba34eeca03f
  y = 0x75ee68eb8bf626aa5b673ab51f6e744e06f8fcf8a6c0cf3035beca956a7b41d5
Which is CORRECT

Curve 'secp384r1': p=384 bits, n=384 bits, a=384 bits, b=384 bits

S :
  x = 0xfba203b81bbd23f2b3be971cc23997e1ae4d89e69cb6f92385dda82768ada415ebab4167459da98e62b1332d1e73cb0e
  y = 0x5ffedbaefdeba603e7923e06cdb5d0c65b22301429293376d5c6944e3fa6259f162b4788de6987fd59aed5e4b5285e45
T :
  x = 0xaacc05202e7fda6fc73d82f0a66220527da8117ee8f8330ead7d20ee6f255f582d8bd38c5a7f2b40bcdb68ba13d81051
  y = 0x84009a263fefba7c2c57cffa5db3634d286131afc0fca8d25afa22a7b5dce0d9470da89233cee178592f49b6fecb5092
R = S + T :
  x = 0x12dc5ce7acdfc5844d939f40b4df012e68f865b89c3213ba97090a247a2fc009075cf471cd2e85c489979b65ee0b5eed
  y = 0x167312e58fe0c0afa248f2854e3cddcb557f983b3189b67f21eee01341e7e9fe67f6ee81b36988efa406945c8804a4b0
Should be :
  x = 0x12dc5ce7acdfc5844d939f40b4df012e68f865b89c3213ba97090a247a2fc009075cf471cd2e85c489979b65ee0b5eed
  y = 0x167312e58fe0c0afa248f2854e3cddcb557f983b3189b67f21eee01341e7e9fe67f6ee81b36988efa406945c8804a4b0
Which is CORRECT

S :
  x = 0xfba203b81bbd23f2b3be971cc23997e1ae4d89e69cb6f92385dda82768ada415ebab4167459da98e62b1332d1e73cb0e
  y = 0x5ffedbaefdeba603e7923e06cdb5d0c65b22301429293376d5c6944e3fa6259f162b4788de6987fd59aed5e4b5285e45
Double S  :
  x = 0x2a2111b1e0aa8b2fc5a1975516bc4d58017ff96b25e1bdff3c229d5fac3bacc319dcbec29f9478f42dee597b4641504c
  y = 0xfa2e3d9dc84db8954ce8085ef28d7184fddfd1344b4d4797343af9b5f9d837520b450f726443e4114bd4e5bdb2f65ddd
Should be :
  x = 0x2a2111b1e0aa8b2fc5a1975516bc4d58017ff96b25e1bdff3c229d5fac3bacc319dcbec29f9478f42dee597b4641504c
  y = 0xfa2e3d9dc84db8954ce8085ef28d7184fddfd1344b4d4797343af9b5f9d837520b450f726443e4114bd4e5bdb2f65ddd
Which is CORRECT

S :
  x = 0xfba203b81bbd23f2b3be971cc23997e1ae4d89e69cb6f92385dda82768ada415ebab4167459da98e62b1332d1e73cb0e
  y = 0x5ffedbaefdeba603e7923e06cdb5d0c65b22301429293376d5c6944e3fa6259f162b4788de6987fd59aed5e4b5285e45
  d = 0xa4ebcae5a665983493ab3e626085a24c104311a761b5a8fdac052ed1f111a5c44f76f45659d2d111a61b5fdd97583480
Q = d * S :
  x = 0xe4f77e7ffeb7f0958910e3a680d677a477191df166160ff7ef6bb5261f791aa7b45e3e653d151b95dad3d93ca0290ef2
  y = 0xac7dee41d8c5f4a7d5836960a773cfc1376289d3373f8cf7417b0c6207ac32e913856612fc9ff2e357eb2ee05cf9667f
Should be :
  x = 0xe4f77e7ffeb7f0958910e3a680d677a477191df166160ff7ef6bb5261f791aa7b45e3e653d151b95dad3d93ca0290ef2
  y = 0xac7dee41d8c5f4a7d5836960a773cfc1376289d3373f8cf7417b0c6207ac32e913856612fc9ff2e357eb2ee05cf9667f
Which is CORRECT

Curve 'secp521r1': p=521 bits, n=521 bits, a=521 bits, b=519 bits

S :
  x = 0x1d5c693f66c08ed03ad0f031f937443458f601fd098d3d0227b4bf62873af50740b0bb84aa157fc847bcf8dc16a8b2b8bfd8e2d0a7d39af04b089930ef6dad5c1b4
  y = 0x144b7770963c63a39248865ff36b074151eac33549b224af5c8664c54012b818ed037b2b7c1a63ac89ebaa11e07db89fcee5b556e49764ee3fa66ea7ae61ac01823
T :
  x = 0xf411f2ac2eb971a267b80297ba67c322dba4bb21cec8b70073bf88fc1ca5fde3ba09e5df6d39acb2c0762c03d7bc224a3e197feaf760d6324006fe3be9a548c7d5
  y = 0x1fdf842769c707c93c630df6d02eff399a06f1b36fb9684f0b373ed064889629abb92b1ae328fdb45534268384943f0e9222afe03259b32274d35d1b9584c65e305
R = S + T :
  x = 0x1264ae115ba9cbc2ee56e6f0059e24b52c8046321602c59a339cfb757c89a59c358a9a8e1f86d384b3f3b255ea3f73670c6dc9f45d46b6a196dc37bbe0f6b2dd9e9
  y = 0x62a9c72b8f9f88a271690bfa017a6466c31b9cadc2fc544744aeb817072349cfddc5ad0e81b03f1897bd9c8c6efbdf68237dc3bb00445979fb373b20c9a967ac55
Should be :
  x = 0x1264ae115ba9cbc2ee56e6f0059e24b52c8046321602c59a339cfb757c89a59c358a9a8e1f86d384b3f3b255ea3f73670c6dc9f45d46b6a196dc37bbe0f6b2dd9e9
  y = 0x62a9c72b8f9f88a271690bfa017a6466c31b9cadc2fc544744aeb817072349cfddc5ad0e81b03f1897bd9c8c6efbdf68237dc3bb00445979fb373b20c9a967ac55
Which is CORRECT

S :
  x = 0x1d5c693f66c08ed03ad0f031f937443458f601fd098d3d0227b4bf62873af50740b0bb84aa157fc847bcf8dc16a8b2b8bfd8e2d0a7d39af04b089930ef6dad5c1b4
  y = 0x144b7770963c63a39248865ff36b074151eac33549b224af5c8664c54012b818ed037b2b7c1a63ac89ebaa11e07db89fcee5b556e49764ee3fa66ea7ae61ac01823
Double S  :
  x = 0x12879442f2450c119e7119a5f738be1f1eba9e9d7c6cf41b325d9ce6d643106e9d61124a91a96bcf201305a9dee55fa79136dc700831e54c3ca4ff2646bd3c36bc6
  y = 0x19864a8b8855c2479cbefe375ae553e2393271ed36fadfc4494fc0583f6bd03598896f39854abeae5f9a6515a021e2c0eef139e71de610143f53382f4104dccb543
Should be :
  x = 0x12879442f2450c119e7119a5f738be1f1eba9e9d7c6cf41b325d9ce6d643106e9d61124a91a96bcf201305a9dee55fa79136dc700831e54c3ca4ff2646bd3c36bc6
  y = 0x19864a8b8855c2479cbefe375ae553e2393271ed36fadfc4494fc0583f6bd03598896f39854abeae5f9a6515a021e2c0eef139e71de610143f53382f4104dccb543
Which is CORRECT

S :
  x = 0x1d5c693f66c08ed03ad0f031f937443458f601fd098d3d0227b4bf62873af50740b0bb84aa157fc847bcf8dc16a8b2b8bfd8e2d0a7d39af04b089930ef6dad5c1b4
  y = 0x144b7770963c63a39248865ff36b074151eac33549b224af5c8664c54012b818ed037b2b7c1a63ac89ebaa11e07db89fcee5b556e49764ee3fa66ea7ae61ac01823
  d = 0x1eb7f81785c9629f136a7e8f8c674957109735554111a2a866fa5a166699419bfa9936c78b62653964df0d6da940a695c7294d41b2d6600de6dfcf0edcfc89fdcb1
Q = d * S :
  x = 0x91b15d09d0ca0353f8f96b93cdb13497b0a4bb582ae9ebefa35eee61bf7b7d041b8ec34c6c00c0c0671c4ae063318fb75be87af4fe859608c95f0ab4774f8c95bb
  y = 0x130f8f8b5e1abb4dd94f6baaf654a2d5810411e77b7423965e0c7fd79ec1ae563c207bd255ee9828eb7a03fed565240d2cc80ddd2cecbb2eb50f0951f75ad87977f
Should be :
  x = 0x91b15d09d0ca0353f8f96b93cdb13497b0a4bb582ae9ebefa35eee61bf7b7d041b8ec34c6c00c0c0671c4ae063318fb75be87af4fe859608c95f0ab4774f8c95bb
  y = 0x130f8f8b5e1abb4dd94f6baaf654a2d5810411e77b7423965e0c7fd79ec1ae563c207bd255ee9828eb7a03fed565240d2cc80ddd2cecbb2eb50f0951f75ad87977f
Which is CORRECT


===== Test Point Multiplication =====
-3 = 0xfffffffffffffffffffffffffffffffefffffffffffffffc
modulus = 0x1d
modulus = 0x1d
inversed :
  x = 0x2
  y = 0x18
inversed = 0x18
Curve 'secp192r1' with k = 0x1
  x = 0x188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012
  y = 0x7192b95ffc8da78631011ed6b24cdd573f977a11e794811
Is in the curve? Yes

It should be:
  x = 0x188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012
  y = 0x7192b95ffc8da78631011ed6b24cdd573f977a11e794811
Which is CORRECT

Given point in the curve? Yes

Curve 'secp192r1' with k = 0x7
  x = 0x8da75a1f75ddcd7660f923243060edce5de37f007011fcfd
  y = 0x57cb5fcf6860b35418240db8fdb3c01dd4b702f96409ffb5
Is in the curve? Yes

It should be:
  x = 0x8da75a1f75ddcd7660f923243060edce5de37f007011fcfd
  y = 0x57cb5fcf6860b35418240db8fdb3c01dd4b702f96409ffb5
Which is CORRECT

Given point in the curve? Yes

Curve 'secp192r1' with k = 0x18ebbb95eed0e13
  x = 0x81e6e0f14c9302c8a8dca8a038b73165e9687d0490cd9f85
  y = 0xf58067119eed8579388c4281dc645a27db7764750e812477
Is in the curve? Yes

It should be:
  x = 0x81e6e0f14c9302c8a8dca8a038b73165e9687d0490cd9f85
  y = 0xf58067119eed8579388c4281dc645a27db7764750e812477
Which is CORRECT

Given point in the curve? Yes

Curve 'secp256r1' with k = 0x1
  x = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
  y = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5
Is in the curve? Yes

It should be:
  x = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
  y = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5
Which is CORRECT

Given point in the curve? Yes

Curve 'secp256r1' with k = 0x7
  x = 0x8e533b6fa0bf7b4625bb30667c01fb607ef9f8b8a80fef5b300628703187b2a3
  y = 0x73eb1dbde03318366d069f83a6f5900053c73633cb041b21c55e1a86c1f400b4
Is in the curve? Yes

It should be:
  x = 0x8e533b6fa0bf7b4625bb30667c01fb607ef9f8b8a80fef5b300628703187b2a3
  y = 0x73eb1dbde03318366d069f83a6f5900053c73633cb041b21c55e1a86c1f400b4
Which is CORRECT

Given point in the curve? Yes

Curve 'secp256r1' with k = 0x18ebbb95eed0e13
  x = 0x339150844ec15234807fe862a86be77977dbfb3ae3d96f4c22795513aeaab82f
  y = 0xb1c14ddfdc8ec1b2583f51e85a5eb3a155840f2034730e9b5ada38b674336a21
Is in the curve? Yes

It should be:
  x = 0x339150844ec15234807fe862a86be77977dbfb3ae3d96f4c22795513aeaab82f
  y = 0xb1c14ddfdc8ec1b2583f51e85a5eb3a155840f2034730e9b5ada38b674336a21
Which is CORRECT

Given point in the curve? Yes

Curve 'secp384r1' with k = 0x1
  x = 0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7
  y = 0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f
Is in the curve? Yes

It should be:
  x = 0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7
  y = 0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f
Which is CORRECT

Given point in the curve? Yes

Curve 'secp384r1' with k = 0x7
  x = 0x283c1d7365ce4788f29f8ebf234edffead6fe997fbea5ffa2d58cc9dfa7b1c508b05526f55b9ebb2040f05b48fb6d0e1
  y = 0x9475c99061e41b88ba52efdb8c1690471a61d867ed799729d9c92cd01dbd225630d84ede32a78f9e64664cdac512ef8c
Is in the curve? Yes

It should be:
  x = 0x283c1d7365ce4788f29f8ebf234edffead6fe997fbea5ffa2d58cc9dfa7b1c508b05526f55b9ebb2040f05b48fb6d0e1
  y = 0x9475c99061e41b88ba52efdb8c1690471a61d867ed799729d9c92cd01dbd225630d84ede32a78f9e64664cdac512ef8c
Which is CORRECT

Given point in the curve? Yes

Curve 'secp384r1' with k = 0x18ebbb95eed0e13
  x = 0xa499efe48839bc3abcd1c5cedbdd51904f9514db44f4686db918983b0c9dc3aee05a88b72433e9515f91a329f5f4fa60
  y = 0x3b7ca28ef31f809c2f1ba24aaed847d0f8b406a4b8968542de139db5828ca410e615d1182e25b91b1131e230b727d36a
Is in the curve? Yes

It should be:
  x = 0xa499efe48839bc3abcd1c5cedbdd51904f9514db44f4686db918983b0c9dc3aee05a88b72433e9515f91a329f5f4fa60
  y = 0x3b7ca28ef31f809c2f1ba24aaed847d0f8b406a4b8968542de139db5828ca410e615d1182e25b91b1131e230b727d36a
Which is CORRECT

Given point in the curve? Yes

Curve 'secp521r1' with k = 0x1
  x = 0xc6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66
  y = 0x11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650
Is in the curve? Yes

It should be:
  x = 0xc6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66
  y = 0x11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650
Which is CORRECT

Given point in the curve? Yes

Curve 'secp521r1' with k = 0x7
  x = 0x56d5d1d99d5b7f6346eeb65fda0b073a0c5f22e0e8f5483228f018d2c2f7114c5d8c308d0abfc698d8c9a6df30dce3bbc46f953f50fdc2619a01cead882816ecd4
  y = 0x3d2d1b7d9baaa2a110d1d8317a39d68478b5c582d02824f0dd71dbd98a26cbde556bd0f293cdec9e2b9523a34591ce1a5f9e76712a5ddefc7b5c6b8bc90525251b
Is in the curve? Yes

It should be:
  x = 0x56d5d1d99d5b7f6346eeb65fda0b073a0c5f22e0e8f5483228f018d2c2f7114c5d8c308d0abfc698d8c9a6df30dce3bbc46f953f50fdc2619a01cead882816ecd4
  y = 0x3d2d1b7d9baaa2a110d1d8317a39d68478b5c582d02824f0dd71dbd98a26cbde556bd0f293cdec9e2b9523a34591ce1a5f9e76712a5ddefc7b5c6b8bc90525251b
Which is CORRECT

Given point in the curve? Yes

Curve 'secp521r1' with k = 0x18ebbb95eed0e13
  x = 0x1650048fbd63e8c30b305bf36bd7643b91448ef2206e8a0ca84a140789a99b0423a0a2533ea079ca7e049843e69e5fa2c25a163819110cec1a30acbbb3a422a40d8
  y = 0x10c9c64a0e0db6052dbc5646687d06dece5e9e0703153efe9cb816fe025e85354d3c5f869d6db3f4c0c01b5f97919a5e72ceebe03042e5aa99112691cffc2724828
Is in the curve? Yes

It should be:
  x = 0x1650048fbd63e8c30b305bf36bd7643b91448ef2206e8a0ca84a140789a99b0423a0a2533ea079ca7e049843e69e5fa2c25a163819110cec1a30acbbb3a422a40d8
  y = 0x10c9c64a0e0db6052dbc5646687d06dece5e9e0703153efe9cb816fe025e85354d3c5f869d6db3f4c0c01b5f97919a5e72ceebe03042e5aa99112691cffc2724828
Which is CORRECT

Given point in the curve? Yes



 TEST ECC START

===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 1
Using curve 'secp192k1'
Base point is on the curve
NUMBITS  : 192
NUMBYTES : 24


Generando claves...

--- ECC KEY secp192k1 192 bits ---

PUB = 04686cf74ae330d0cf70387ae3c1bfa107dea60703b0d0f88cadd4a836883f30145075a1a1f4b96f992d8756f73f635e1c
PRI = 0x89d32a7c0b09c6b33666c92c92dc7dfcb599da39b83cdc6f
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp192k1'
Private Key    = 0xc4798ab943cd634ed1510225d6802fde86a9202d8d9813f9
Public Key's X = 0xcc02cc8a25211d6caae0136f94ac70a3c3880f6895c9018
Public Key's Y = 0x8dfc630014d7577146d50ab38c23e5b024cf6e98d93a4259

Public Key Bytes = 030cc02cc8a25211d6caae0136f94ac70a3c3880f6895c9018


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xcc02cc8a25211d6caae0136f94ac70a3c3880f6895c9018
Public Key's Y = 0x8dfc630014d7577146d50ab38c23e5b024cf6e98d93a4259

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 2
Using curve 'secp192r1'
Base point is on the curve
NUMBITS  : 192
NUMBYTES : 24


Generando claves...

--- ECC KEY secp192r1 192 bits ---

PUB = 0408df8485976d56cf71477449f2453b33e47db50e56ad86ab9cd941e48c86ea46a1392ae66e485c1c198215a7652cf49d
PRI = 0x8d43bdd07ed152bcf599f8204725384626e33977f312c051
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp192r1'
Private Key    = 0xb8f45be919b9e8e0352db6f96d2d264a165789cb19b4e5e4
Public Key's X = 0x8b739aa36a8d4eda7d5a2bc86539554e32214b832c24f555
Public Key's Y = 0x78398e1121ca255a853b9bf29670cc3cda228a3577c7af15

Public Key Bytes = 038b739aa36a8d4eda7d5a2bc86539554e32214b832c24f555


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x8b739aa36a8d4eda7d5a2bc86539554e32214b832c24f555
Public Key's Y = 0x78398e1121ca255a853b9bf29670cc3cda228a3577c7af15

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 3
Using curve 'secp256r1'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32


Generando claves...

--- ECC KEY secp256r1 256 bits ---

PUB = 0463a7248bc7d3cbfbe672926c29fee230b289e18d366dbc448934ab4e9f357dde73e884e559f6c40a2347d09f42c52887f85a4fa4ffeffb5ff02e603641ec2150
PRI = 0xa628be81245e3244ae58f3db7a43470362df51ea9c9f043bafb1916b55d7621a
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp256r1'
Private Key    = 0x8b27af769a0b8eebb857839a140cbd6c01f6bf7ed908758e28b37a4da80c8b84
Public Key's X = 0xf2774654f18f5fe243e308de15851c1365bc213421e343caa60ac294a02ca70b
Public Key's Y = 0x8d591d8e078497cf0653f8ae24b9f7b3754226dd0dd5cde28f85964b50861690

Public Key Bytes = 02f2774654f18f5fe243e308de15851c1365bc213421e343caa60ac294a02ca70b


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xf2774654f18f5fe243e308de15851c1365bc213421e343caa60ac294a02ca70b
Public Key's Y = 0x8d591d8e078497cf0653f8ae24b9f7b3754226dd0dd5cde28f85964b50861690

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 4
Using curve 'secp256k1'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32


Generando claves...

--- ECC KEY secp256k1 256 bits ---

PUB = 0403b44ab75f785226e62fc2188762b32db02d24bd0ec18b0e09bacc249c1a29b6cf86b34e7195cf25e374f6e39456f245eec85711eefd768868810bc4f5755965
PRI = 0x9bdd28427491b13e347c690c045af36bcbc5f89e5c5ca5f0fe818967807136af
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp256k1'
Private Key    = 0xdaa76df5088d9070080ca326a1e960416968b24f88f7a49fcc43138ca815e863
Public Key's X = 0xc26cff44880883295462ca3c2ea24ef1e32fef56bb1f401e00ce43d9de579d86
Public Key's Y = 0x121a633e3820e9ba884ec5a3b6581871873ee76b66c9b903b9941066e5df0c02

Public Key Bytes = 02c26cff44880883295462ca3c2ea24ef1e32fef56bb1f401e00ce43d9de579d86


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xc26cff44880883295462ca3c2ea24ef1e32fef56bb1f401e00ce43d9de579d86
Public Key's Y = 0x121a633e3820e9ba884ec5a3b6581871873ee76b66c9b903b9941066e5df0c02

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 5
Using curve 'secp384r1'
Base point is on the curve
NUMBITS  : 384
NUMBYTES : 48


Generando claves...

--- ECC KEY secp384r1 384 bits ---

PUB = 04c657eafb24aca9f4c4c827a83ee785c557baada3a685545e5beb0f3c40a3881d9f819b48db1728edee38958204129123fc959d5462b7e14e152c6a2ccfe77388be078e1743280c7b274f2d9bd30645c8a129673709ed82398138055df45ee1c8
PRI = 0x97ebf167124ff267364eb9a5cf83a7058449ceaea5cfcb94ea5084f8c9f06cd01a5ba4154a53600b4b74a5dcef971636
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp384r1'
Private Key    = 0xf0c7e404d0ca5a598ac21d6e9ede9a0ec65170800deed5fe7e04878c7cd0e0b6032a8ed99d525141ee6c7204947dff14
Public Key's X = 0xf15505304f7f8ca2488be65c1ae9a30c257669826eae9ea51ab451ecbc5213b0765231475c5f999a3eb5705db46043a4
Public Key's Y = 0xde929b2057fab43599a0367e8473079ccb1f0d434b5608c8aa1d8ad600daf0820a720ba89f49b8c061f86a4e51e7b6b1

Public Key Bytes = 03f15505304f7f8ca2488be65c1ae9a30c257669826eae9ea51ab451ecbc5213b0765231475c5f999a3eb5705db46043a4


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xf15505304f7f8ca2488be65c1ae9a30c257669826eae9ea51ab451ecbc5213b0765231475c5f999a3eb5705db46043a4
Public Key's Y = 0xde929b2057fab43599a0367e8473079ccb1f0d434b5608c8aa1d8ad600daf0820a720ba89f49b8c061f86a4e51e7b6b1

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 6
Using curve 'secp521r1'
Base point is on the curve
NUMBITS  : 521
NUMBYTES : 66


Generando claves...

--- ECC KEY secp521r1 521 bits ---

PUB = 040153ad56f21d7d46b0a73c406825ddb98b6e226838e1b6071d50b53d499e07dc9a0dc88981da7d74d10dd08e1dc9e8f2682542c1223e3f08a12848c31e6ee5dc4b830044f00c92d2a699cf12de94c912bec83e1f94c71457bc1a292233fd64fea3394de5da63dc5360c53392b21ba4d21a7152a17048534ebaa9c60e3a32d0ac7007f42e
PRI = 0x1a45a235658e56e461e199a94999dde122989a193db3425b9db479e26f403392444bd6d5bc3012e90abef1fca0f179c17e5fec531299a1f4e209574c099428e490b
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp521r1'
Private Key    = 0x1feaf0dd8ea64cfaf38e96dc7e86e58a28ce4b123d43dfe7d2795788cf337eb9e1181ea4581afab85ba4872ca263f995195940c12babe953b0f9e8b855429449281
Public Key's X = 0x5db80814341f6848f128bbc7e9a2ba76d0661b41764783cb89f00cae3bc1fcc469ed3023ff6d1ac3e930e0612c0401ec736893ed39da1b9e5834a6cd3f48cab27f
Public Key's Y = 0x3f75a16b296b236df971dbced2954423c638a311f9e96cdfeca94b7616f4dab07207ab388ae5cf15f6ae9ccdd56a1022a62fa37378e9aa3e11cd2abfd249bf2243

Public Key Bytes = 03005db80814341f6848f128bbc7e9a2ba76d0661b41764783cb89f00cae3bc1fcc469ed3023ff6d1ac3e930e0612c0401ec736893ed39da1b9e5834a6cd3f48cab27f


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x5db80814341f6848f128bbc7e9a2ba76d0661b41764783cb89f00cae3bc1fcc469ed3023ff6d1ac3e930e0612c0401ec736893ed39da1b9e5834a6cd3f48cab27f
Public Key's Y = 0x3f75a16b296b236df971dbced2954423c638a311f9e96cdfeca94b7616f4dab07207ab388ae5cf15f6ae9ccdd56a1022a62fa37378e9aa3e11cd2abfd249bf2243

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 7
Using curve 'brainpoolP192r1'
Base point is on the curve
NUMBITS  : 192
NUMBYTES : 24


Generando claves...

--- ECC KEY brainpoolP192r1 192 bits ---

PUB = 041c6a5dd915493b459919d88bf3494df40421ff86f789be62656b435d6e5fb90d79ef6baba72bc49dd0530f19d574e4be
PRI = 0xb6872820c73d0c5c730b185f971acfcfb0caf08870b71ead
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP192r1'
Private Key    = 0x39152948b91858eb9ed6865739603c2b25ab02e08e754681
Public Key's X = 0x660b7771e2ce45a6f8ef666677070e0653b535dbdec57ce9
Public Key's Y = 0xbd847483bca96a78acb8b76076a213da5e7e2125420f7449

Public Key Bytes = 03660b7771e2ce45a6f8ef666677070e0653b535dbdec57ce9


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x660b7771e2ce45a6f8ef666677070e0653b535dbdec57ce9
Public Key's Y = 0xbd847483bca96a78acb8b76076a213da5e7e2125420f7449

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 8
Using curve 'brainpoolP224r1'
Base point is on the curve
NUMBITS  : 224
NUMBYTES : 28


Generando claves...

--- ECC KEY brainpoolP224r1 224 bits ---

PUB = 04ac87f0b0f3f9279fe2ea699bdef0a8f157e326e4222cc27a7ae8c226caedc7346d24309f626134b31651004c111adb73db377deb4e11e8c2
PRI = 0x8776a9d8d28bf2f3f646c9559288efb8dbb9a01148bb918fe6ae1e9a
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP224r1'
Private Key    = 0x8b40dc8d53837850bd9ac338a478b4324fdeb9d821c60b65350de350
Public Key's X = 0x3aca9014472b77b70881d3263aad548ce168b79d97a16315e6889c76
Public Key's Y = 0x91075a86ca5ec7ba1472b969ada8fd2677635e1180af6f5e9d77785e

Public Key Bytes = 023aca9014472b77b70881d3263aad548ce168b79d97a16315e6889c76


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x3aca9014472b77b70881d3263aad548ce168b79d97a16315e6889c76
Public Key's Y = 0x91075a86ca5ec7ba1472b969ada8fd2677635e1180af6f5e9d77785e

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 9
Using curve 'brainpoolP256r1'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32


Generando claves...

--- ECC KEY brainpoolP256r1 256 bits ---

PUB = 046cc74939cddc9dcab57e0c4ed427d069b40d226b3e9e3fad81f59cc14fe5df049d94a002eea6407eca0ad8989ebc9e311735b7dac71606fb249966e72700c755
PRI = 0x9bc3ee58460802033941bc01346a9d9e521d7b22ac157910c79cce04b73a480f
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP256r1'
Private Key    = 0xa4a2534acf20c69467bc13509e83b2ed92a2afc6e70a794d4922113a547454ec
Public Key's X = 0x9ea9f6e4ffcfe35f17aac55cf6623d243e76531f58a2f9e1dfc1596427e28e11
Public Key's Y = 0x4c6aa319480fbef4018378e9dcafc89086e7c7db691062c5de6b52f54d8a07b9

Public Key Bytes = 039ea9f6e4ffcfe35f17aac55cf6623d243e76531f58a2f9e1dfc1596427e28e11


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x9ea9f6e4ffcfe35f17aac55cf6623d243e76531f58a2f9e1dfc1596427e28e11
Public Key's Y = 0x4c6aa319480fbef4018378e9dcafc89086e7c7db691062c5de6b52f54d8a07b9

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 10
Using curve 'brainpoolP320r1'
Base point is on the curve
NUMBITS  : 320
NUMBYTES : 40


Generando claves...

--- ECC KEY brainpoolP320r1 320 bits ---

PUB = 0459c7485f1290c9faaeebc654d9fbc4c178bc7443d2b6fd7f00bb1f73b7b644d2ada965c00120cb541fdfb6c294a52c38638c8b130c74320c8778361d7b139b46c6691722c1918a38e4629f651d193b7d
PRI = 0xc19825faf6039fcc4793fbb055b6ee715d85a62cfeba794ee3f516650ebadf0fb11815aec321f79e
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP320r1'
Private Key    = 0xbf5b6d61d83b5914f82cae106b8cf47c2c7f707a9e4c88210e51ba965081a1b794936a3fc250b03d
Public Key's X = 0xc7c423b60b8acef84b59db3ae69947233e0097bc2bdd44c1558dceb88bb3e92ad9d5c16270f70809
Public Key's Y = 0x1f1c93337a1a374a444a21c412c224a12dbee8c9bb30ef802b594f54b068b7899abd459281961a65

Public Key Bytes = 03c7c423b60b8acef84b59db3ae69947233e0097bc2bdd44c1558dceb88bb3e92ad9d5c16270f70809


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xc7c423b60b8acef84b59db3ae69947233e0097bc2bdd44c1558dceb88bb3e92ad9d5c16270f70809
Public Key's Y = 0x1f1c93337a1a374a444a21c412c224a12dbee8c9bb30ef802b594f54b068b7899abd459281961a65

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 11
Using curve 'brainpoolP384r1'
Base point is on the curve
NUMBITS  : 384
NUMBYTES : 48


Generando claves...

--- ECC KEY brainpoolP384r1 384 bits ---

PUB = 0468e9ec9ee39644a08e118eba44595b53405e80615c2c76d62b340fe8e7575b42018031320ee140280e95bc7cc9b97a29828766b55c536fd84b33418f4a7cc167807af554e10b5db1b48ddfae90b12c755cf5c4ad15d8e1a114fdf44a64ebfdef
PRI = 0x28f0b86981687f555fe2fa0bdd553aea7c4e48d847869d996f46ca22339222277a4528fc245e14ebc7d12bf631816c6c
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP384r1'
Private Key    = 0xebd7aaefcde9a2f4201aa12b18db63b5535186363b966e043333ebbfe3aaf0b02adf6559c64687ee50dd65cef9739cc
Public Key's X = 0x535ce97c68479285cb1a5df4ddd5ec7a6d6a2e11c6bba57cb54de52d5418f2833ba2d541f91bf8eb4dbc0f3b37d84927
Public Key's Y = 0x39bcb96dddcd86a145b126b3ca2ea8a022114b0baae36184e7f03eb3f70ed71235ce2e8c822e16f31de2d321ab95cdac

Public Key Bytes = 02535ce97c68479285cb1a5df4ddd5ec7a6d6a2e11c6bba57cb54de52d5418f2833ba2d541f91bf8eb4dbc0f3b37d84927


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x535ce97c68479285cb1a5df4ddd5ec7a6d6a2e11c6bba57cb54de52d5418f2833ba2d541f91bf8eb4dbc0f3b37d84927
Public Key's Y = 0x39bcb96dddcd86a145b126b3ca2ea8a022114b0baae36184e7f03eb3f70ed71235ce2e8c822e16f31de2d321ab95cdac

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 12
Using curve 'brainpoolP512r1'
Base point is on the curve
NUMBITS  : 512
NUMBYTES : 64


Generando claves...

--- ECC KEY brainpoolP512r1 512 bits ---

PUB = 045f58a6e3a90138a403dfe8c54d1685c0485fe5bfd34280ef991e466efd34f0e6e395e8e030bda9f67bb5321c9d764604148784296d19aeaaf2ce20c5b09a09767313ae67be8032829c5fefc1a573de5581e5bfbafa8f8c649aa16cd2220780ae58c6f62cdf890a06f253218b0566ed217bdf9fc790239fb03a7d5f8f695b18c2
PRI = 0x2813b9993c0b62b516cbb5a8e9afc3ee0023980261fbbc89dc82de06d45860c21821bc635cc94a69d39228dd8c35736614392258630d2f72b54c72d610f0a953
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP512r1'
Private Key    = 0x9722451dc65678dd03b40abcf0e46b4220726157d8ea7983a71fa4c2d96c39f2e130f4c2d9dac1b4250426534c3765cf179c3037410a3d4a1adfb90d05e6645
Public Key's X = 0x105c79320efdf840f80f9ed30e3631a296e45fcb8b39a5a149f8739295d0329fa70ff701e6b659b302c87ebc02d65c7acc352787b77e1cdd1027fbc87a40d1c3
Public Key's Y = 0x42e32dd8ec711762144419361b541ff57a401695734df1104f3047a7e5b35555c9535099d8556953a5b6a6ee46e956649a3933ad93123ff342fe9884a14a85f4

Public Key Bytes = 02105c79320efdf840f80f9ed30e3631a296e45fcb8b39a5a149f8739295d0329fa70ff701e6b659b302c87ebc02d65c7acc352787b77e1cdd1027fbc87a40d1c3


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x105c79320efdf840f80f9ed30e3631a296e45fcb8b39a5a149f8739295d0329fa70ff701e6b659b302c87ebc02d65c7acc352787b77e1cdd1027fbc87a40d1c3
Public Key's Y = 0x42e32dd8ec711762144419361b541ff57a401695734df1104f3047a7e5b35555c9535099d8556953a5b6a6ee46e956649a3933ad93123ff342fe9884a14a85f4

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 13
Using curve 'curve25519'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32


Generando claves...

--- ECC KEY curve25519 256 bits ---

PUB = d77e3016ace8ad8a1fee741f6ab5cd23d49ce8cfb85ee6e7eb3caac17dd46f0e
PRI = 0x9b03282d67e3e84a0c5b9e55e65ec3c3cce1b1303e1522371f693e07afd0a90
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'curve25519'
Private Key    = 0x6d8c679991e95ed60f6f006917f08ac4145e9d8b64e1bc747ea2b44b04e0918
Public Key's X = 0xc1b1a47a094c82ebfe14344e060a3549b660b378ae5a6f01226dcaf2fcd35a23
Public Key's Y = 0x0

Public Key Bytes = c1b1a47a094c82ebfe14344e060a3549b660b378ae5a6f01226dcaf2fcd35a23


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xc1b1a47a094c82ebfe14344e060a3549b660b378ae5a6f01226dcaf2fcd35a23
Public Key's Y = 0x0

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 14
Using curve 'curve448'
Base point is on the curve
NUMBITS  : 448
NUMBYTES : 56


Generando claves...

--- ECC KEY curve448 448 bits ---

PUB = bccb2d32195ca0f7312544109b87986a50cd1ff94a77879139d31a3107626efddd15e51b19c4c8946d17fa721b14e1f03b62e1ca97f2affb
PRI = 0x399c1154d2dc8fa695c2191b430a6d02c25574b3a98da3778f9fec775ca03c85c71b79170be8a442cbd941515820553f1fcd913b6ffc3248
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'curve448'
Private Key    = 0x2662221513e9d661bd255e2376294a1856d3d78048b0d1c5a2bc0be1ed209c80b88dd587a87016fc4b7f5bcb8d817c62ea5d1c27a533f7cc
Public Key's X = 0xa687b424bd9dbc11ad9612283cced83f831a1539a1b825f17215544ba7d1738a7b28638e2662e2679be2045c23de9c929258e5b9e96a9097
Public Key's Y = 0x0

Public Key Bytes = a687b424bd9dbc11ad9612283cced83f831a1539a1b825f17215544ba7d1738a7b28638e2662e2679be2045c23de9c929258e5b9e96a9097


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xa687b424bd9dbc11ad9612283cced83f831a1539a1b825f17215544ba7d1738a7b28638e2662e2679be2045c23de9c929258e5b9e96a9097
Public Key's Y = 0x0

Result is CORRECT



 TEST ECC _END



```




