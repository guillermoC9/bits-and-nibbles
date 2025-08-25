# ECC
## Elliptic Curve Criptography

Before pictures were much bigger than the whole of Windows 95 (~30 Mb), people was really concerned about the size of ```RSA``` keys and the computational performance of the algorithm. So, they did something about it and divised a new criptography technique based on elliptic curves over finite -Galois- fields, a very interesting part of computational geometry.  

Elliptic Curve Criptography bore a much smaller keys to -theoretically- do the same of bigger key algorithms. For example, an ```ECC``` key of 256 bits equalled the security of a counterpart with a 2048 bits key, that is less computational effort and smaller data sizes for the same benefit. 

Now, I said *theoretically* there because in reality ```ECC``` functions do not do **exactly the same** of the other algorithms, as they do not encode/decode data like ```RSA``` or ```AES``` do -if we ignore Elgamal at the moment-, but they can be used to produce signatures or perform key agreement, which is ideal to create smaller certificates and TLS handshakes.

There are many ```ECC``` algorithms, but here we present a small library that uses our typical one-stop shop approach to support the most common curves around, all of them prime curves:

    - Secp: secp192k1, secp192r1, secp256r1, secp256k1, secp384r1, secp521r1
    - Brainpool:  brainpool192r1, brainpool224r1, brainpool256r1 , brainpool320r1, brainpool384r1, brainpool512r1
    - Edwards: curve25519, curve448

There are also the same protocols for key exchange and signature used with the other encription algorithms: ```ECDSA```, ```ECDH```, etc.

Although these algorithms are said to also be vulnerable to the infamous -but great- Shor's algorithm for *Quantum computers*, ```TLSv1.2``` and ```TLSv1.3``` relay heavily on them to replace RSA, specially for key exchange -with a preference for curve25519-. To be honest, apart from the key size and a little performance gain, I am not sure how safer is to use ```ECC``` over ```RSA```, especially with regards of Quantum safety. But again, I am an implementor of algorithms, not a mathematician, so I cannot give a proper answer. 

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


Curve 25519 Diffie-Hellman:
    Generating Alice Keys (32 bytes)
      Private = 77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a
      Must Be = 77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a

       Public = a0e1a5c720fa1adad858c05b7e02cab912f59bf95abe4fe0db47c234dd2b2c21
      Must Be = a0e1a5c720fa1adad858c05b7e02cab912f59bf95abe4fe0db47c234dd2b2c21

    Generating Bob Keys (32 bytes)
      Private = 5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb
      Must Be = 5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb

       Public = 87b3ccf50ce25da6ab7053e68c6eb5f7ea0969b51df6b84c60cdd7be15798404
      Must Be = 87b3ccf50ce25da6ab7053e68c6eb5f7ea0969b51df6b84c60cdd7be15798404

    Generated Secret (32 bytes)
        Alice = 168579f5ac2414fe251e1365ec6c64d02d951a503a27dabeea6908da4e24f109
          Bob = 168579f5ac2414fe251e1365ec6c64d02d951a503a27dabeea6908da4e24f109
      Must Be = 168579f5ac2414fe251e1365ec6c64d02d951a503a27dabeea6908da4e24f109

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


Curve 448 Diffie-Hellman:
    Generating Alice Keys (56 bytes)
      Private = 9a8f4925d1519f5775cf46b04b5800d4ee9ee8bae8bc5565d498c28dd9c9baf574a9419744897391006382a6f127ab1d9ac2d8c0a598726b
      Must Be = 9a8f4925d1519f5775cf46b04b5800d4ee9ee8bae8bc5565d498c28dd9c9baf574a9419744897391006382a6f127ab1d9ac2d8c0a598726b

       Public = f9e60c4f68e01bc3c913c53f6d52efa73d49ef91a519e83266aa9d6f9af2091663efbdf79a01597c446cc2fbeb32da9b663273651cb00170
      Must Be = f9e60c4f68e01bc3c913c53f6d52efa73d49ef91a519e83266aa9d6f9af2091663efbdf79a01597c446cc2fbeb32da9b663273651cb00170

    Generating Bob Keys (56 bytes)
      Private = 1c306a7ac2a0e2e0990b294470cba339e6453772b075811d8fad0d1d6927c120bb5ee8972b0d3e21374c9c921b09d1b0366f10b65173992d
      Must Be = 1c306a7ac2a0e2e0990b294470cba339e6453772b075811d8fad0d1d6927c120bb5ee8972b0d3e21374c9c921b09d1b0366f10b65173992d

       Public = 38475a2a6de13fcca293e78150a3b0144b504543e5196c4710186fe05e0863854224184a5a02d20d6ed51da965389ba69300164d367102e8
      Must Be = 38475a2a6de13fcca293e78150a3b0144b504543e5196c4710186fe05e0863854224184a5a02d20d6ed51da965389ba69300164d367102e8

    Generated Secret (56 bytes)
        Alice = 8bac1028804712ead5b64ce695dc08fb1432cb5f71faf0eca71f0dd5bfdd1d0b4bb7bb0307669f89f18d7845c7e35dc9c79f23b837f63cd4
          Bob = 8bac1028804712ead5b64ce695dc08fb1432cb5f71faf0eca71f0dd5bfdd1d0b4bb7bb0307669f89f18d7845c7e35dc9c79f23b837f63cd4
      Must Be = 8bac1028804712ead5b64ce695dc08fb1432cb5f71faf0eca71f0dd5bfdd1d0b4bb7bb0307669f89f18d7845c7e35dc9c79f23b837f63cd4


===== Test Prime & Binary Points =====
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
Testing DH Using curve 'secp192k1' (OSSYS)
curve p  = 0xfffffffffffffffffffffffffffffffffffffffeffffee37
curve a  = 0x0
curve b  = 0x3
curve Gx = 0xdb4ff10ec057e9ae26b07d0280b7f4341da5d1b1eae06c7d
curve Gy = 0x9b2f2f6d9c5628a7844163d015be86344082aa88d95e2f9d
curve n  = 0xfffffffffffffffffffffffe26f2fc170f69466a74defd8d
curve h: 1
dh1 lpri  = 0xc066eff65fbf2f989f19408075f727c7e14154c8d8242a35
dh1 lpubx = 0x39de27b46cf179ea8fe4c2e89cb932a770b2a9b5de4dcb48
dh1 lpuby = 0xaf10e8fe2977728ade72065043544e4dd307576e0881090a
dh2 lpri  = 0xd3e2f2148e5dc0f2d2c2f223e4d4e4d4a2a4d1ccdecc0c36
dh2 lpubx = 0x5db49ab87873a1c7aed6920f4349f6a3bf762a4956201929
dh2 lpuby = 0x8177c2bca72a0032b515e07f7cffee9ffc57ab74dc5d5e24

public 1  = 0439de27b46cf179ea8fe4c2e89cb932a770b2a9b5de4dcb48af10e8fe2977728ade72065043544e4dd307576e0881090a
public 2  = 045db49ab87873a1c7aed6920f4349f6a3bf762a49562019298177c2bca72a0032b515e07f7cffee9ffc57ab74dc5d5e24

shared 1 = 7de0d6291ac9592bb9d6c518746354d4f1a593b48b457cab
shared 2 = 7de0d6291ac9592bb9d6c518746354d4f1a593b48b457cab

Result is CORRECT



Generating keys...

--- ECC KEY secp192k1 192 bits ---

PUB = 04ab737c560d809d89b075169ca7f8a31bef78a25593602c520088014c41af3f9e318d5c8958334cc13814de5111e4e487
PRI = 0xdb6164017b3035fbc948832446aa56bfeaf4d90c16197ed7
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp192k1'
Private Key    = 0x990e15dfa3f5d109907eae8f9ec6c6e37843627c2ecd8624
Public Key's X = 0xa70709cb2b5cac2ae1ed0b724d5ee9d7953e534b1b6205e9
Public Key's Y = 0xc868a8ff06fe8a4b568b38199a78b7e542f91bc1517590d4

Public Key Bytes = 02a70709cb2b5cac2ae1ed0b724d5ee9d7953e534b1b6205e9


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xa70709cb2b5cac2ae1ed0b724d5ee9d7953e534b1b6205e9
Public Key's Y = 0xc868a8ff06fe8a4b568b38199a78b7e542f91bc1517590d4

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 2
Using curve 'secp192r1'
Base point is on the curve
NUMBITS  : 192
NUMBYTES : 24
Testing DH Using curve 'secp192r1' (OSSYS)
curve p  = 0xfffffffffffffffffffffffffffffffeffffffffffffffff
curve a  = 0xfffffffffffffffffffffffffffffffefffffffffffffffc
curve b  = 0x64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1
curve Gx = 0x188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012
curve Gy = 0x7192b95ffc8da78631011ed6b24cdd573f977a11e794811
curve n  = 0xffffffffffffffffffffffff99def836146bc9b1b4d22831
curve h: 1
dh1 lpri  = 0xe078be728983e4411f9adad1f1f8aac810f6b406788ff559
dh1 lpubx = 0xdfea01a35cde0d9eec21fab094f134d559b28529fac6e379
dh1 lpuby = 0xf1b2ba825ae10ddf0e7fb0a918441443e6791be64fb52be0
dh2 lpri  = 0xb71c489562893fdeb0d8d3fee8dddc788ded5e598b3e926b
dh2 lpubx = 0x2ca8691f3696ea75dbcb4bb0d705e5a72cb811a67cc80088
dh2 lpuby = 0xdad9f8344735aabe7da4bd5bf6bedd94c1a1affb3f7a6d5d

public 1  = 04dfea01a35cde0d9eec21fab094f134d559b28529fac6e379f1b2ba825ae10ddf0e7fb0a918441443e6791be64fb52be0
public 2  = 042ca8691f3696ea75dbcb4bb0d705e5a72cb811a67cc80088dad9f8344735aabe7da4bd5bf6bedd94c1a1affb3f7a6d5d

shared 1 = 3a3aafefaa92e5ce319718c7256d1c1a792b75d8500b0ad9
shared 2 = 3a3aafefaa92e5ce319718c7256d1c1a792b75d8500b0ad9

Result is CORRECT



Generating keys...

--- ECC KEY secp192r1 192 bits ---

PUB = 04ee841573153b8605136fb9a38fd1b289118a3b82eb66fb26f4ea6707470faec3114fc3ed4c0df8b2b0f477f69f2f8594
PRI = 0x812fe0eb7f2b57a40f74e2c697d36de1447467a42a5639c9
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp192r1'
Private Key    = 0xa47756c23edef70f39179e7cae0312b72c2110fdc863bb35
Public Key's X = 0xfbd06cb86591d2adfcc846cca4c6d70cb6e3d529f4e12dd9
Public Key's Y = 0xad011a31f134a7b2eafe2b669c816f17e4cb2f84ce8b3b98

Public Key Bytes = 02fbd06cb86591d2adfcc846cca4c6d70cb6e3d529f4e12dd9


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xfbd06cb86591d2adfcc846cca4c6d70cb6e3d529f4e12dd9
Public Key's Y = 0xad011a31f134a7b2eafe2b669c816f17e4cb2f84ce8b3b98

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 3
Using curve 'secp256r1'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32
Testing DH Using curve 'secp256r1' (OSSYS)
curve p  = 0xffffffff00000001000000000000000000000000ffffffffffffffffffffffff
curve a  = 0xffffffff00000001000000000000000000000000fffffffffffffffffffffffc
curve b  = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b
curve Gx = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
curve Gy = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5
curve n  = 0xffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551
curve h: 1
dh1 lpri  = 0xae8aec9812388d026fa1dcb6e3561a12bb408b800e4be6efdb3b70c4cb490eb7
dh1 lpubx = 0xdc66ca44592d10b05fdf78151528e2e54d9739c8fe3c660ec285dc80938606f2
dh1 lpuby = 0x4dccdd926998e295f44df9bf68bebcecf33c044a04766ac9fccffce47b354b94
dh2 lpri  = 0xa0126352bf61e2a64dc6c6622bf5241ef49fcb9c7f04064cf96fc4e226cd5573
dh2 lpubx = 0x153134306eb47390c3938b1fc992b623e2960bd7c15bca1880b092db663536af
dh2 lpuby = 0x7a45150d72c33099e3a3137d71d5aa4f2b2f644d68e1205fe5a4626a0c7898e

public 1  = 04dc66ca44592d10b05fdf78151528e2e54d9739c8fe3c660ec285dc80938606f24dccdd926998e295f44df9bf68bebcecf33c044a04766ac9fccffce47b354b94
public 2  = 04153134306eb47390c3938b1fc992b623e2960bd7c15bca1880b092db663536af07a45150d72c33099e3a3137d71d5aa4f2b2f644d68e1205fe5a4626a0c7898e

shared 1 = 1e21eb99cae067d26f754dca51581962201143e237b626b3d89af1f50f38c95b
shared 2 = 1e21eb99cae067d26f754dca51581962201143e237b626b3d89af1f50f38c95b

Result is CORRECT



Generating keys...

--- ECC KEY secp256r1 256 bits ---

PUB = 040e810fa42054169d17d21118a08d3d8a0884c447b78e488842837afd6d87c56c2b67098e071bcc4efacef8ec7ca16ea97153cf919ee99161ee3ea71e6cb8e90d
PRI = 0xca40d6265beb2ff6e7d9b103d922eb677a416dc2712973f8f0e6233d1f89f903
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp256r1'
Private Key    = 0x9633dea66920fdfb0fab1c56154e896693dfb3b722250fab3a5c48b02d1b5325
Public Key's X = 0x18f7a29a2b843d6d6c681ec49c2bce7437bdb6dbb9a3712a4c916da9bdc66a07
Public Key's Y = 0xeac77fdd6ef36337752fcf68cef1355ea957b55026d704a88bb290e7ec5f64a5

Public Key Bytes = 0318f7a29a2b843d6d6c681ec49c2bce7437bdb6dbb9a3712a4c916da9bdc66a07


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x18f7a29a2b843d6d6c681ec49c2bce7437bdb6dbb9a3712a4c916da9bdc66a07
Public Key's Y = 0xeac77fdd6ef36337752fcf68cef1355ea957b55026d704a88bb290e7ec5f64a5

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 4
Using curve 'secp256k1'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32
Testing DH Using curve 'secp256k1' (OSSYS)
curve p  = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f
curve a  = 0x0
curve b  = 0x7
curve Gx = 0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798
curve Gy = 0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8
curve n  = 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141
curve h: 1
dh1 lpri  = 0xf1a9a4d6eb109cb598525eda1da4d7b1cf17dede01dd83cb3fd793542398dd8b
dh1 lpubx = 0x36a4c4e8cc291694714ab725bf6f615ad9f75ac4d4fcf2729f8c55c1e40b07e7
dh1 lpuby = 0x707f4cc7db1be405c783d1497aeec8133009c9d3c96cddc941ec0d43f2c62377
dh2 lpri  = 0xb4c3dc70dc488368887d877829bf8c23dec4f793721132dd1af63018cb0f5ffc
dh2 lpubx = 0x285a7202395d5935cb60648a35a9ca4539b17846df33d01bfac4cc5d4842768b
dh2 lpuby = 0xb77157c8f805da9e7671ec8bcee0fd646ab7aadb43b2c5599c01acec66c1a8b8

public 1  = 0436a4c4e8cc291694714ab725bf6f615ad9f75ac4d4fcf2729f8c55c1e40b07e7707f4cc7db1be405c783d1497aeec8133009c9d3c96cddc941ec0d43f2c62377
public 2  = 04285a7202395d5935cb60648a35a9ca4539b17846df33d01bfac4cc5d4842768bb77157c8f805da9e7671ec8bcee0fd646ab7aadb43b2c5599c01acec66c1a8b8

shared 1 = d42b070cc6abe5bb4f60cb1c540ac20953bfb57ace94062231bad199620e1f65
shared 2 = d42b070cc6abe5bb4f60cb1c540ac20953bfb57ace94062231bad199620e1f65

Result is CORRECT



Generating keys...

--- ECC KEY secp256k1 256 bits ---

PUB = 047bc2e61c47d2454728bfe308b8397b56036c6ddb2cd283a305843b878a547e74b3eb149bbdb08bfd486d9cb2d171a92c3613afea390a724d9263dc9dfafa78f2
PRI = 0x80fa2dfa731e80939ca7edf6f5325954dbc60d0ca383481d2b7852210e75cf54
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp256k1'
Private Key    = 0xeecbb5da15f27dc54cd58a6839dd9d9c05947d84f10bef44e0231f90183ac7f6
Public Key's X = 0x4bc921946992ec4bc3104c1ad4a2ccd37a7389051c5b051fffead26f7665213f
Public Key's Y = 0x7e31818040ef89bdd5c6d2ba43f28aeb0191427124b660563ce66d07311372bd

Public Key Bytes = 034bc921946992ec4bc3104c1ad4a2ccd37a7389051c5b051fffead26f7665213f


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x4bc921946992ec4bc3104c1ad4a2ccd37a7389051c5b051fffead26f7665213f
Public Key's Y = 0x7e31818040ef89bdd5c6d2ba43f28aeb0191427124b660563ce66d07311372bd

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 5
Using curve 'secp384r1'
Base point is on the curve
NUMBITS  : 384
NUMBYTES : 48
Testing DH Using curve 'secp384r1' (OSSYS)
curve p  = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff
curve a  = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc
curve b  = 0xb3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef
curve Gx = 0xaa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7
curve Gy = 0x3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f
curve n  = 0xffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973
curve h: 1
dh1 lpri  = 0x88bcf491f75cd3c97d4a61a86b9f1f95425c09171a8250f306658dfe05ac8c4220d22a3e76759d58fb67ff9cb3c59be5
dh1 lpubx = 0x62b691907c45b63fd4bb9f2e3a2ec8eb0850d7e225730efac4206f8458077d94f3ee76209638735bd413d33bf1e98c98
dh1 lpuby = 0xdf4324a12cc05c7fdf8f6ea5c78dd1f3d7fa51a1411642d8545cb2b6f207391fd5eac4d28375e7f6937b4cc7818005cd
dh2 lpri  = 0x902673cb7aeca8830b3dbe39738ec0a38b2002deca8892dc91131b24e13451914b20530fbaa1f2b91bb913182eb679c4
dh2 lpubx = 0xbf2751065e77da6ab02a352b1e6930ecaef8a45fba58805c350e7a918039f49edb3c26b168cc5de6ded5c4360eb25e1b
dh2 lpuby = 0x550afb9621f104bbea3c469c6f4341fb85cabd465e906a035af353cb17287bda637de89b40c1d330809aeb43a0b7e30c

public 1  = 0462b691907c45b63fd4bb9f2e3a2ec8eb0850d7e225730efac4206f8458077d94f3ee76209638735bd413d33bf1e98c98df4324a12cc05c7fdf8f6ea5c78dd1f3d7fa51a1411642d8545cb2b6f207391fd5eac4d28375e7f6937b4cc7818005cd
public 2  = 04bf2751065e77da6ab02a352b1e6930ecaef8a45fba58805c350e7a918039f49edb3c26b168cc5de6ded5c4360eb25e1b550afb9621f104bbea3c469c6f4341fb85cabd465e906a035af353cb17287bda637de89b40c1d330809aeb43a0b7e30c

shared 1 = 91cc985a383b8fa2b8d7e249443b53ffe16039f926303e6becd140f6511fdd5d838c47f58b907ad9f4d4ca8e4b339138
shared 2 = 91cc985a383b8fa2b8d7e249443b53ffe16039f926303e6becd140f6511fdd5d838c47f58b907ad9f4d4ca8e4b339138

Result is CORRECT



Generating keys...

--- ECC KEY secp384r1 384 bits ---

PUB = 0458cb1e818817afe555ad9e3070d530b975fbe46a8e086ba024625388313f9c3d91e2d9a7e14fef04ee395738fbe296cf30fefa2ef3ee0e77c715fa92de7416414ac3e93f6f2b38300055e657df7159eaaba3cb54e869426c9d2ca6232f9ffce1
PRI = 0xbac832c9ca2df7e222db8784c97c867865c06196ba77e0f9f98619aad45f3185d96fc1c9a12c820a6ae79a7279ab966d
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp384r1'
Private Key    = 0xdd82fd1049e9d54ebced98143b7857e74d4c624012cfbacdec27c6a29f08e5cebf4f08d62ebd89780ed0a9f7cf126ca5
Public Key's X = 0xc61dcb76be255ac97dac9ab9298aedd77a91bb93b21a919f263ed5e9924d8613c3ee1dc7fca823cc3ef947ce4bfe8e18
Public Key's Y = 0xa4221b7a47cc75076b29ee0eaa1fcb4de7cb13d0238d250e57181152bccc7cd722fd1bd91b839099ba006b297517084d

Public Key Bytes = 03c61dcb76be255ac97dac9ab9298aedd77a91bb93b21a919f263ed5e9924d8613c3ee1dc7fca823cc3ef947ce4bfe8e18


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xc61dcb76be255ac97dac9ab9298aedd77a91bb93b21a919f263ed5e9924d8613c3ee1dc7fca823cc3ef947ce4bfe8e18
Public Key's Y = 0xa4221b7a47cc75076b29ee0eaa1fcb4de7cb13d0238d250e57181152bccc7cd722fd1bd91b839099ba006b297517084d

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 6
Using curve 'secp521r1'
Base point is on the curve
NUMBITS  : 521
NUMBYTES : 66
Testing DH Using curve 'secp521r1' (OSSYS)
curve p  = 0x1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
curve a  = 0x1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc
curve b  = 0x51953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00
curve Gx = 0xc6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66
curve Gy = 0x11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650
curve n  = 0x1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409
curve h: 1
dh1 lpri  = 0x14f50019f709d6b7a7badea2b5ca6131244f776d34b26a8a4330c86023725d079fd5348a247d32fc4367678a00aa3172de0dbc81ca5cc9562858b1ac4645cfbab18
dh1 lpubx = 0x8ebce0f48fcb2b24744c4be675d5bb17c4d5cf69237c9de164c1cbccfb2eb592648becece5a804970f6a81febc0997dd295452b40ab067f8bd84d28ada4a74bfb9
dh1 lpuby = 0xe6f1b52770a5c7495c0976be311b53b10579bc784a03108cee5c51e7fa0fc173f1385f3f516b8717475d09317ea740a25c17e52db5b8ccc6fc0d4a0cb6234d3e68
dh2 lpri  = 0x1ebcb04318626a5083aed7a7a5fadc14b7f3703444c8ac4875636fb1135d4a12794214e1b8fb428214c4944c5e6fe123a864d6a938c449c068b263bd551e872eca3
dh2 lpubx = 0x5c725d2edcfbe77f0c06e3fb8f5e2a29f2d9cbaa483035782ad33fc228a2192e099c5c8b6491df210acb5aadcd95d2b788ae5dd7cb38b0afe7df0c63ad7f98f013
dh2 lpuby = 0x9b7b9b1f981ad255385a1390f2eea1bc7c65e2d78ae70e767b3d57275ace7bc8b9e36cce69080136f46f8de5a419c2f15c77735c1d2627ffdb871a5899c215b6f5

public 1  = 04008ebce0f48fcb2b24744c4be675d5bb17c4d5cf69237c9de164c1cbccfb2eb592648becece5a804970f6a81febc0997dd295452b40ab067f8bd84d28ada4a74bfb900e6f1b52770a5c7495c0976be311b53b10579bc784a03108cee5c51e7fa0fc173f1385f3f516b8717475d09317ea740a25c17e52db5b8ccc6fc0d4a0cb6234d3e68
public 2  = 04005c725d2edcfbe77f0c06e3fb8f5e2a29f2d9cbaa483035782ad33fc228a2192e099c5c8b6491df210acb5aadcd95d2b788ae5dd7cb38b0afe7df0c63ad7f98f013009b7b9b1f981ad255385a1390f2eea1bc7c65e2d78ae70e767b3d57275ace7bc8b9e36cce69080136f46f8de5a419c2f15c77735c1d2627ffdb871a5899c215b6f5

shared 1 = 01c879681a6eb7907218cc65180e4646e6fc2cd0b3aa283d41742741c22e348f6222aef345ed8e8e005dc70ed86d557dc6ed66eee04ea640d77aa5c34a7b175c5799
shared 2 = 01c879681a6eb7907218cc65180e4646e6fc2cd0b3aa283d41742741c22e348f6222aef345ed8e8e005dc70ed86d557dc6ed66eee04ea640d77aa5c34a7b175c5799

Result is CORRECT



Generating keys...

--- ECC KEY secp521r1 521 bits ---

PUB = 04010b1b80a062ee0c0fe5ecf68f9df298f41ac0a951d359c4f37ebe4b43cceb504c614510623f32ee15c05f3e032f5a1167656299ba3e2f4ac9b7cfbe5e2e800313990068436eb6dfb10afcbb172de8e4e6246ea1d464c9a5d68c784deb42173384f7139350372155dce6d88e07c71112ec48e8be70e8926c9f2671e82ed577b61a4fc22f
PRI = 0x165d246804c83a86dce2241500ff51ebf5149437bf1fcce9f1e55823b30a6c9cc43d50db8787c9f2b46cab095df7ae9a8af7aa072123f71d0dc6501e5ce367cc535
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp521r1'
Private Key    = 0x1ecc633023a76a6c8b4da2a4418e539299ea2182ad7f9155e68b8806eef70b42a6cf58cbd898682eed7457cd5d22f3a4ded7db078d9ed79c88a7db5746ad15f744c
Public Key's X = 0x6fdec71f65c1ca776c85fd8eb4afb176a2290e4ec3cde430778fd5cbf1595ad7b33460a7a62b0fe90eec060e376de345d8cabe9fb7b2cae844a9a3155c48f1db22
Public Key's Y = 0x3b16c2131ffae64ab67e0a5af5e6e66eadb596a40fff5c9fa7dd1a2339a93aa5c258eed4ae05cbf52f55ad27a704617c6798d041581fc6d2e5c167b068d07cf445

Public Key Bytes = 03006fdec71f65c1ca776c85fd8eb4afb176a2290e4ec3cde430778fd5cbf1595ad7b33460a7a62b0fe90eec060e376de345d8cabe9fb7b2cae844a9a3155c48f1db22


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x6fdec71f65c1ca776c85fd8eb4afb176a2290e4ec3cde430778fd5cbf1595ad7b33460a7a62b0fe90eec060e376de345d8cabe9fb7b2cae844a9a3155c48f1db22
Public Key's Y = 0x3b16c2131ffae64ab67e0a5af5e6e66eadb596a40fff5c9fa7dd1a2339a93aa5c258eed4ae05cbf52f55ad27a704617c6798d041581fc6d2e5c167b068d07cf445

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 7
Using curve 'brainpoolP192r1'
Base point is on the curve
NUMBITS  : 192
NUMBYTES : 24
Testing DH Using curve 'brainpoolP192r1' (OSSYS)
curve p  = 0xc302f41d932a36cda7a3463093d18db78fce476de1a86297
curve a  = 0x6a91174076b1e0e19c39c031fe8685c1cae040e5c69a28ef
curve b  = 0x469a28ef7c28cca3dc721d044f4496bcca7ef4146fbf25c9
curve Gx = 0xc0a0647eaab6a48753b033c56cb0f0900a2f5c4853375fd6
curve Gy = 0x14b690866abd5bb88b5f4828c1490002e6773fa2fa299b8f
curve n  = 0xc302f41d932a36cda7a3462f9e9e916b5be8f1029ac4acc1
curve h: 1
dh1 lpri  = 0x2fdb1f7c74f2fc0d018d7275ecb604b1e2dbd1aa7dec8ec5
dh1 lpubx = 0x9797bd81ced1d5b8e6c9be1426951127f17768e9e339bd79
dh1 lpuby = 0x6103de00616c447d661c675539299d2b1044b8a796ee1a3d
dh2 lpri  = 0x34050fc3e5a013bf8d8e22aafc0b67cdf00014a0ac65fe9b
dh2 lpubx = 0xb8b5150e2270aa0fdf73d4b9fd442739d99e4792b47fcc6f
dh2 lpuby = 0x6c8abde143c8c65b6046828459df15d48fc57485874661a5

public 1  = 049797bd81ced1d5b8e6c9be1426951127f17768e9e339bd796103de00616c447d661c675539299d2b1044b8a796ee1a3d
public 2  = 04b8b5150e2270aa0fdf73d4b9fd442739d99e4792b47fcc6f6c8abde143c8c65b6046828459df15d48fc57485874661a5

shared 1 = 3e458ba309b31caf4093d1d137273d26ce8b18759c201fc1
shared 2 = 3e458ba309b31caf4093d1d137273d26ce8b18759c201fc1

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP192r1 192 bits ---

PUB = 04455f1ed173b80a18a71ac5361bfb83570905f460ff9e74088b5e7f9e5941941bd0218ae86a5dfbd454f46f414fa245c8
PRI = 0xb2fdc26701201ec37a11058b62caefccbcc950f0a0ff2a13
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP192r1'
Private Key    = 0xb52ea8a8d51da66ed50ed5c865b23e2dc1831ef6b7bb1d51
Public Key's X = 0x1ad9a284819e907de6b1e598ae6a070175ac882c7e83d471
Public Key's Y = 0x21ce3dde276d9028ba3617f21fbf2d894848570df9ef55b7

Public Key Bytes = 031ad9a284819e907de6b1e598ae6a070175ac882c7e83d471


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x1ad9a284819e907de6b1e598ae6a070175ac882c7e83d471
Public Key's Y = 0x21ce3dde276d9028ba3617f21fbf2d894848570df9ef55b7

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 8
Using curve 'brainpoolP224r1'
Base point is on the curve
NUMBITS  : 224
NUMBYTES : 28
Testing DH Using curve 'brainpoolP224r1' (OSSYS)
curve p  = 0xd7c134aa264366862a18302575d1d787b09f075797da89f57ec8c0ff
curve a  = 0x68a5e62ca9ce6c1c299803a6c1530b514e182ad8b0042a59cad29f43
curve b  = 0x2580f63ccfe44138870713b1a92369e33e2135d266dbb372386c400b
curve Gx = 0xd9029ad2c7e5cf4340823b2a87dc68c9e4ce3174c1e6efdee12c07d
curve Gy = 0x58aa56f772c0726f24c6b89e4ecdac24354b9e99caa3f6d3761402cd
curve n  = 0xd7c134aa264366862a18302575d0fb98d116bc4b6ddebca3a5a7939f
curve h: 1
dh1 lpri  = 0x3b8ef0fc4f8dc3cc2edea1cfabbecf041de63520b6a30a278b868a0
dh1 lpubx = 0x2e2d47507b4bce394843f97988baa3d79b9331b72450d0b29d10d86e
dh1 lpuby = 0x7992d986ba3bd3c54405ffe6440c41e851e4b0be6f14056d0385a644
dh2 lpri  = 0x512f63eb111820df546ed66cdc6b8a4742ba236ef7444de69e693ac
dh2 lpubx = 0xa004448f71deb4bfb3fbfc53c7e93cd0b0491ec3c631582ead2e9539
dh2 lpuby = 0x50f9673525b710b9d89cf75fcdc89c0945de4aca0bea9589c8946130

public 1  = 042e2d47507b4bce394843f97988baa3d79b9331b72450d0b29d10d86e7992d986ba3bd3c54405ffe6440c41e851e4b0be6f14056d0385a644
public 2  = 04a004448f71deb4bfb3fbfc53c7e93cd0b0491ec3c631582ead2e953950f9673525b710b9d89cf75fcdc89c0945de4aca0bea9589c8946130

shared 1 = 3c670b01f3da8fb7b14294cd41b4cee29b8a655ff3aaa2e197da9632
shared 2 = 3c670b01f3da8fb7b14294cd41b4cee29b8a655ff3aaa2e197da9632

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP224r1 224 bits ---

PUB = 041d70ae93f052c6e9d72de53957e01c89e0ce3a77798f082fdae64205bfa94a2b2eb7cbb6797a0e4e37970c3de3db9708f0ceecc8c6fa43e2
PRI = 0x8d3846382e8129d2c13d2532012a714695568742d566f98d46158fb8
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP224r1'
Private Key    = 0x2aaff3561cb55e580e78d8a6ee52a7f0416926cae3495205c84ce72
Public Key's X = 0x934478a9b8dcff747092e9d2b51ef8703dfd74d63be386b2c7cdf281
Public Key's Y = 0x57dbf0e39540b3dc636439125c2c0fc3f18d1e5472c97109d163c0bc

Public Key Bytes = 02934478a9b8dcff747092e9d2b51ef8703dfd74d63be386b2c7cdf281


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x934478a9b8dcff747092e9d2b51ef8703dfd74d63be386b2c7cdf281
Public Key's Y = 0x57dbf0e39540b3dc636439125c2c0fc3f18d1e5472c97109d163c0bc

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 9
Using curve 'brainpoolP256r1'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32
Testing DH Using curve 'brainpoolP256r1' (OSSYS)
curve p  = 0xa9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e5377
curve a  = 0x7d5a0975fc2c3057eef67530417affe7fb8055c126dc5c6ce94a4b44f330b5d9
curve b  = 0x26dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c07b6
curve Gx = 0x8bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262
curve Gy = 0x547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f046997
curve n  = 0xa9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f7901e0e82974856a7
curve h: 1
dh1 lpri  = 0x449224adea73877ee6aa3620d372b9601b042c63a2121d709868c2a6918afab8
dh1 lpubx = 0xa85c0b417685e9ce3a8b232dc640d26e67a52e94ee1c80a5fe9a91d936ce5336
dh1 lpuby = 0x374945284bde1d01bbc0ab43276ca058635468be73bc665468424b226b6a9f5e
dh2 lpri  = 0x15d269f1ad22c7fea57a44222ee2db4350c3dc86eb7e8b42c5574618b264d628
dh2 lpubx = 0xc79a0de3379f1c38dce8d48d3f93cf7e26a72384ef787816cd93d6b6437708c
dh2 lpuby = 0x62794eff58dab50fd1f79b8c95b32861355bad43b3151050cb15f96ad9568f1d

public 1  = 04a85c0b417685e9ce3a8b232dc640d26e67a52e94ee1c80a5fe9a91d936ce5336374945284bde1d01bbc0ab43276ca058635468be73bc665468424b226b6a9f5e
public 2  = 040c79a0de3379f1c38dce8d48d3f93cf7e26a72384ef787816cd93d6b6437708c62794eff58dab50fd1f79b8c95b32861355bad43b3151050cb15f96ad9568f1d

shared 1 = 26156c433dc14a9236ce27001c30e0e37799f79ffeb0f927b7b220df8fe4b959
shared 2 = 26156c433dc14a9236ce27001c30e0e37799f79ffeb0f927b7b220df8fe4b959

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP256r1 256 bits ---

PUB = 04a05ace0ecc9fa996479f0a804846cb321a127bc138223dc2e17b486e5d9501f925be147965b6cbfe9610867ece6655b3c87126269b6bb71906ccb32223610755
PRI = 0xa94701ef3719982c0c80e803d84a0ab4b9264807c6c96cea16cad37d11f8cd1a
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP256r1'
Private Key    = 0x3137b792d40790f65baeae71cae13bd97d46c1ff146e818cc0ec584de06aab4
Public Key's X = 0x5ab283272d662eb9692362dc1495ca9f0c8b5b9cf7be6c41f5425f07342e6f93
Public Key's Y = 0x20430d5925cc258c32f44857f0f03d43087bf4cd069233848760336a8465f12a

Public Key Bytes = 025ab283272d662eb9692362dc1495ca9f0c8b5b9cf7be6c41f5425f07342e6f93


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x5ab283272d662eb9692362dc1495ca9f0c8b5b9cf7be6c41f5425f07342e6f93
Public Key's Y = 0x20430d5925cc258c32f44857f0f03d43087bf4cd069233848760336a8465f12a

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 10
Using curve 'brainpoolP320r1'
Base point is on the curve
NUMBITS  : 320
NUMBYTES : 40
Testing DH Using curve 'brainpoolP320r1' (OSSYS)
curve p  = 0xd35e472036bc4fb7e13c785ed201e065f98fcfa6f6f40def4f92b9ec7893ec28fcd412b1f1b32e27
curve a  = 0x3ee30b568fbab0f883ccebd46d3f3bb8a2a73513f5eb79da66190eb085ffa9f492f375a97d860eb4
curve b  = 0x520883949dfdbc42d3ad198640688a6fe13f41349554b49acc31dccd884539816f5eb4ac8fb1f1a6
curve Gx = 0x43bd7e9afb53d8b85289bcc48ee5bfe6f20137d10a087eb6e7871e2a10a599c710af8d0d39e20611
curve Gy = 0x14fdd05545ec1cc8ab4093247f77275e0743ffed117182eaa9c77877aaac6ac7d35245d1692e8ee1
curve n  = 0xd35e472036bc4fb7e13c785ed201e065f98fcfa5b68f12a32d482ec7ee8658e98691555b44c59311
curve h: 1
dh1 lpri  = 0xbb5209d24652a11d22123183a5cb84bb148174e75c0fcb96528cc17ee6358cbc9456dc07d0e4a149
dh1 lpubx = 0x2f2e807ca5db9498c205f381dc8cfa8a9427922f74710743f9c0bc0df10fd9d712962bbd06dc5e0d
dh1 lpuby = 0xad47acda450e21e8a56fb2e09b9859800f0813523d8581814e44db62090fa215ee1885b23bba106d
dh2 lpri  = 0x1246deb78301faa642d1b0786fa769ab6b2ef178c3a1eb4d68d1ae46ed456cf7f71f4f2883b1ffa2
dh2 lpubx = 0x95250327892884489d9c6674eebdd15db5912fdae6515b36725a27a6f335bb15c944f9fdd406d012
dh2 lpuby = 0x68a0e77a108a42e7045014dcbab99e8882970b6337fe683f1a9218cb0e112d14a078f8bd880e1b67

public 1  = 042f2e807ca5db9498c205f381dc8cfa8a9427922f74710743f9c0bc0df10fd9d712962bbd06dc5e0dad47acda450e21e8a56fb2e09b9859800f0813523d8581814e44db62090fa215ee1885b23bba106d
public 2  = 0495250327892884489d9c6674eebdd15db5912fdae6515b36725a27a6f335bb15c944f9fdd406d01268a0e77a108a42e7045014dcbab99e8882970b6337fe683f1a9218cb0e112d14a078f8bd880e1b67

shared 1 = 7a2e0ca09a95618543fc37c6933b17604320716e393c2fa9663dbf1052f099c23c2f5137b4949a59
shared 2 = 7a2e0ca09a95618543fc37c6933b17604320716e393c2fa9663dbf1052f099c23c2f5137b4949a59

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP320r1 320 bits ---

PUB = 04bc74777b503375221cd8e5e45e4a58adea7165bc49b2bc86a0b58fdc3c758d68fd3d9d71405fbde619fd8b655d952d104bb5e80e855bdc4f83c6f645c9b2ca3c6d8515a36d21e7df3695474e1cf0c639
PRI = 0x1f049b8261b96949f892121217d3e7246e2b5cec9d9eb8be73b077e8506a1c1e8f3ba19bb27feedc
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP320r1'
Private Key    = 0x702666641b6b68bd7815f68243ed143dbb4fd3d4dabb8ce3f8a4cc9f9b205f14f3828679f496bd8
Public Key's X = 0x394e8fcd4045ed1b4740e4d1f335b35317f2c7bd42b50ed42acf81e1790bb5e04bb96785c147136d
Public Key's Y = 0xc32206b61adf4d864431245d740c338bfb41d0d1b69b23e5f372dd26ae68cd62c8c47dad94cd2e9a

Public Key Bytes = 02394e8fcd4045ed1b4740e4d1f335b35317f2c7bd42b50ed42acf81e1790bb5e04bb96785c147136d


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x394e8fcd4045ed1b4740e4d1f335b35317f2c7bd42b50ed42acf81e1790bb5e04bb96785c147136d
Public Key's Y = 0xc32206b61adf4d864431245d740c338bfb41d0d1b69b23e5f372dd26ae68cd62c8c47dad94cd2e9a

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 11
Using curve 'brainpoolP384r1'
Base point is on the curve
NUMBITS  : 384
NUMBYTES : 48
Testing DH Using curve 'brainpoolP384r1' (OSSYS)
curve p  = 0x8cb91e82a3386d280f5d6f7e50e641df152f7109ed5456b412b1da197fb71123acd3a729901d1a71874700133107ec53
curve a  = 0x7bc382c63d8c150c3c72080ace05afa0c2bea28e4fb22787139165efba91f90f8aa5814a503ad4eb04a8c7dd22ce2826
curve b  = 0x4a8c7dd22ce28268b39b55416f0447c2fb77de107dcd2a62e880ea53eeb62d57cb4390295dbc9943ab78696fa504c11
curve Gx = 0x1d1c64f068cf45ffa2a63a81b7c13f6b8847a3e77ef14fe3db7fcafe0cbd10e8e826e03436d646aaef87b2e247d4af1e
curve Gy = 0x8abe1d7520f9c2a45cb1eb8e95cfd55262b70b29feec5864e19c054ff99129280e4646217791811142820341263c5315
curve n  = 0x8cb91e82a3386d280f5d6f7e50e641df152f7109ed5456b31f166e6cac0425a7cf3ab6af6b7fc3103b883202e9046565
curve h: 1
dh1 lpri  = 0x12b2d916a14d1fb15d8cf9b0782fdaf637b8b75b0f4a32fa321d81c0b5161d80c7181adcd6290419a2c13a0c51331d3d
dh1 lpubx = 0x1f9655b4d7cbd3ba82e46031e84a4d49c114f15e9fdb9401e0adab9836f209bc7f82005f9a0fc8b9b96a1ede93862747
dh1 lpuby = 0x1d489ad42731734fd91004cffc1407143e375966deac2a4d14e4bbdd00d08ad7344030ee608f8f246bbac1d7a2858abc
dh2 lpri  = 0x340e3c49d8edeea2cd88221c44f90e93b4114ba3fe8340597c2dcba251a0b0a3d06259dc92ddd96cd21c36281ade4305
dh2 lpubx = 0x136b09be3df2ea351335fad214b8b415779d599a51f0af84e8c8340223ff15ba3dcbafbdec018021dc18c5f07df25c60
dh2 lpuby = 0x4ceeb49a07905e7100fb62312decab09caa33002a72664bfd21aad812faca4343c49df89332ccb462afeb7465e60272f

public 1  = 041f9655b4d7cbd3ba82e46031e84a4d49c114f15e9fdb9401e0adab9836f209bc7f82005f9a0fc8b9b96a1ede938627471d489ad42731734fd91004cffc1407143e375966deac2a4d14e4bbdd00d08ad7344030ee608f8f246bbac1d7a2858abc
public 2  = 04136b09be3df2ea351335fad214b8b415779d599a51f0af84e8c8340223ff15ba3dcbafbdec018021dc18c5f07df25c604ceeb49a07905e7100fb62312decab09caa33002a72664bfd21aad812faca4343c49df89332ccb462afeb7465e60272f

shared 1 = 84ed014f4e0102bb0f54aa4a42dc83da780f98649cd4f7db9544cdf6ac3e4bdbb09c6d82ba1a18c2b234d77c76962686
shared 2 = 84ed014f4e0102bb0f54aa4a42dc83da780f98649cd4f7db9544cdf6ac3e4bdbb09c6d82ba1a18c2b234d77c76962686

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP384r1 384 bits ---

PUB = 0436ed3bbb75805e341b360e1f256f35b1a52e452e76f5211011acd4b71b703ab53f4fad1b069964f205d18fd86f62370107d722ab7524d654554e95c4f8ee564300e42b6cc9b167a2fbef8be89ad65e1664ebe54dffbd6e505122f6f5b3b27a77
PRI = 0x2af9f4f87d0cd8b6d00e67e986e16b36c6105a864daf1049897a4e96739c4566190e35ee73b737e167eed0c4da26c93a
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP384r1'
Private Key    = 0xb41551975d3aa4cae0c86e33d1c6aea495db2bec7f55a5fb6811afddf82c83268a8dfc72ee0c108b8eec16c7fa3e22d
Public Key's X = 0x2e18bee5bff5978d8cf4e45313f661333be88b1719429591de9e51181ff1fb5e7b758b78415f93422eaec6d341aa5bc2
Public Key's Y = 0x6d75bd4967446dd1006a065c8ac4096022f35f633f4b27e62087234980a62d07268bbdaee476ebd3537c4c1593f20d5c

Public Key Bytes = 022e18bee5bff5978d8cf4e45313f661333be88b1719429591de9e51181ff1fb5e7b758b78415f93422eaec6d341aa5bc2


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x2e18bee5bff5978d8cf4e45313f661333be88b1719429591de9e51181ff1fb5e7b758b78415f93422eaec6d341aa5bc2
Public Key's Y = 0x6d75bd4967446dd1006a065c8ac4096022f35f633f4b27e62087234980a62d07268bbdaee476ebd3537c4c1593f20d5c

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 12
Using curve 'brainpoolP512r1'
Base point is on the curve
NUMBITS  : 512
NUMBYTES : 64
Testing DH Using curve 'brainpoolP512r1' (OSSYS)
curve p  = 0xaadd9db8dbe9c48b3fd4e6ae33c9fc07cb308db3b3c9d20ed6639cca703308717d4d9b009bc66842aecda12ae6a380e62881ff2f2d82c68528aa6056583a48f3
curve a  = 0x7830a3318b603b89e2327145ac234cc594cbdd8d3df91610a83441caea9863bc2ded5d5aa8253aa10a2ef1c98b9ac8b57f1117a72bf2c7b9e7c1ac4d77fc94ca
curve b  = 0x3df91610a83441caea9863bc2ded5d5aa8253aa10a2ef1c98b9ac8b57f1117a72bf2c7b9e7c1ac4d77fc94cadc083e67984050b75ebae5dd2809bd638016f723
curve Gx = 0x81aee4bdd82ed9645a21322e9c4c6a9385ed9f70b5d916c1b43b62eef4d0098eff3b1f78e2d0d48d50d1687b93b97d5f7c6d5047406a5e688b352209bcb9f822
curve Gy = 0x7dde385d566332ecc0eabfa9cf7822fdf209f70024a57b1aa000c55b881f8111b2dcde494a5f485e5bca4bd88a2763aed1ca2b2fa8f0540678cd1e0f3ad80892
curve n  = 0xaadd9db8dbe9c48b3fd4e6ae33c9fc07cb308db3b3c9d20ed6639cca70330870553e5c414ca92619418661197fac10471db1d381085ddaddb58796829ca90069
curve h: 1
dh1 lpri  = 0x87dff6b8de2f8ded6e2cea0ed9765880198cf4ff036af668e76b126412e349d4a87d4344b4db5765aaf487b9b7a57b3670407161a349bca2c3a1ab6855a2b506
dh1 lpubx = 0x13d404ce4a0317b3ef12afe8d349ef56440e09ca02b530d0b00d4cfc81d7ed18da3823bcdae8f6c09849a5bb965fae2458f4b7de90c19a7a4e26729c1524dc3d
dh1 lpuby = 0x2a77de611ab9da8b2c93b159f18d7b32d508be577728a55db2d5e46d73a8b533af1f812798805aba4d96e2fa9ffcbc14fa2d033d95648d4a6dfe09b49d1638e1
dh2 lpri  = 0x43c93d91b067da83579f0fd8017c753e1068613d691eec56226703e0c420c80017d5b92497a001c029cf930270e55907bc7171d0d1dc3781d1bfd6514ca33af5
dh2 lpubx = 0x85e536fafc441d64bcdffe826a0b67ae9e6fc163fee4e64502410fca06017c61c52183b458c5fe7cb406c333d507b31313dd394b9da8429750d555a3eef8617b
dh2 lpuby = 0x204770866e368a512b00d874b7735999afd8c01517fc5b2e7235ac3302aa9a62c06898b616028a91f4fa61b0129677bcf390bf4e27d302253d8ee243f092c39e

public 1  = 0413d404ce4a0317b3ef12afe8d349ef56440e09ca02b530d0b00d4cfc81d7ed18da3823bcdae8f6c09849a5bb965fae2458f4b7de90c19a7a4e26729c1524dc3d2a77de611ab9da8b2c93b159f18d7b32d508be577728a55db2d5e46d73a8b533af1f812798805aba4d96e2fa9ffcbc14fa2d033d95648d4a6dfe09b49d1638e1
public 2  = 0485e536fafc441d64bcdffe826a0b67ae9e6fc163fee4e64502410fca06017c61c52183b458c5fe7cb406c333d507b31313dd394b9da8429750d555a3eef8617b204770866e368a512b00d874b7735999afd8c01517fc5b2e7235ac3302aa9a62c06898b616028a91f4fa61b0129677bcf390bf4e27d302253d8ee243f092c39e

shared 1 = 90f48007d2a8bf84cc6679ec1fd9d44b659aee12977904615dc65e2636e19a0a944415b68048472a67a104c5eb93df5fdd31902968fbbef9a401821f7a359801
shared 2 = 90f48007d2a8bf84cc6679ec1fd9d44b659aee12977904615dc65e2636e19a0a944415b68048472a67a104c5eb93df5fdd31902968fbbef9a401821f7a359801

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP512r1 512 bits ---

PUB = 045ad6246cdf5a618ca467e9a20dfeac7112580cde2c5c31a21be71d36d923ea9029d682b0dbca3d340e9335509d396ee080f6deb99a5617799c74b8f3548c27621d7df942715cc76befaa915cd01c67a94957b9cf36f9ab2b9c4bc0e295be1d708cc5ea4286f6344cdbe8306edd7686ee4a77424e267aaa7cd1e0f9da61ab384c
PRI = 0x9c7b43d155f27e19d189858bbb8223135efcec7175de17a50221728c634c8d34cd23d5c06fe9ad13edfd90ef0321409eb6bc53be2db15266ae76ebca65795949
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP512r1'
Private Key    = 0x309f7d39871be1f3ccb4f17005f2c7b4744abe53c1d4821a1ea371509515a727c3ba1b3d1da783a31cc538bb9fb048701fe3f9f3967b4527c086eb1abb7b9557
Public Key's X = 0x75fb674f270e4216d20ddb7cfcc858dd530bdb9eecd8fd8a73cdd7ec82589348326a10d5e21c6af958ddfc6fabe4691c8838fa2aa2a16709de6ecfa3a2f8831f
Public Key's Y = 0x90e5d1bf7c9f04b78100191950cd2468b9660f1a802de73d4330d998579871cb7bf73a219c378b96d04f0d881574f099bdc5b210bbb97f1d71c4bfd771e7088e

Public Key Bytes = 0275fb674f270e4216d20ddb7cfcc858dd530bdb9eecd8fd8a73cdd7ec82589348326a10d5e21c6af958ddfc6fabe4691c8838fa2aa2a16709de6ecfa3a2f8831f


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x75fb674f270e4216d20ddb7cfcc858dd530bdb9eecd8fd8a73cdd7ec82589348326a10d5e21c6af958ddfc6fabe4691c8838fa2aa2a16709de6ecfa3a2f8831f
Public Key's Y = 0x90e5d1bf7c9f04b78100191950cd2468b9660f1a802de73d4330d998579871cb7bf73a219c378b96d04f0d881574f099bdc5b210bbb97f1d71c4bfd771e7088e

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 13
Using curve 'curve25519'
Base point is on the curve
NUMBITS  : 256
NUMBYTES : 32
Testing DH Using curve 'curve25519' (OSSYS)
curve p  = 0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
curve a  = 0x76d06
curve b  = 0x1
curve Gx = 0x900000000000000000000000000000000000000000000000000000000000000
curve Gy = 0x20ae19a1b8a086b4e01edd2c7748d14c923d4d7e6d7c61b229e9c5a27eced3d9
curve n  = 0x1000000000000000000000000000000014def9dea2f79cd65812631a5cf5d3ed
curve h: 8
dh1 lpri  = 0x7694f419881e6c1869b6e2cfb531050fe3b89e1ffab66bf5083ed9f405fcd78
dh1 lpubx = 0x35eae03d974b32b9ea795a30592aef5f8e08511f03835c44be8b94696679f848
dh1 lpuby = 0x0
dh2 lpri  = 0x8ddc8e5af5039a141c3cea7eb58cf023599b891013cafe3513d0d0191633260
dh2 lpubx = 0x41078b4277c11d3c943c270233c0cce5280b8882b926b7b3c2454345f03aa95f
dh2 lpuby = 0x0

public 1  = 35eae03d974b32b9ea795a30592aef5f8e08511f03835c44be8b94696679f848
public 2  = 41078b4277c11d3c943c270233c0cce5280b8882b926b7b3c2454345f03aa95f

shared 1 = 5791b1348a3272113d7d170c062c2e9ea11401a479be6d7d9bd3cb36e5531e0f
shared 2 = 5791b1348a3272113d7d170c062c2e9ea11401a479be6d7d9bd3cb36e5531e0f

Result is CORRECT



Generating keys...

--- ECC KEY curve25519 256 bits ---

PUB = 8a1eb5f685ee3250271185339b762692355e6be4efee2ab657a23394870a3a3b
PRI = 0x8548edab29e8d416fb9aaf77965cc7e7cdc36ee43d58cc18459494c7b1eb378
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'curve25519'
Private Key    = 0x75e089b66fea51034584ac2c60aa852099ca1715a2a07e6379400d0068f31c8
Public Key's X = 0xb1bdbc4da9a70e6c767a9a750a92ee22e48c8b2eff13be0301cf42ab1c18b72d
Public Key's Y = 0x0

Public Key Bytes = b1bdbc4da9a70e6c767a9a750a92ee22e48c8b2eff13be0301cf42ab1c18b72d


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xb1bdbc4da9a70e6c767a9a750a92ee22e48c8b2eff13be0301cf42ab1c18b72d
Public Key's Y = 0x0

Result is CORRECT



===== Test ECC =====
Using prng 'OSSYS'
Getting curve ... 14
Using curve 'curve448'
Base point is on the curve
NUMBITS  : 448
NUMBYTES : 56
Testing DH Using curve 'curve448' (OSSYS)
curve p  = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffffffffffffffffffffffffffffffffffffffffffffffffffff
curve a  = 0x98a9
curve b  = 0x0
curve Gx = 0x500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
curve Gy = 0x7d235d1295f5b1f66c98ab6e58326fcecbae5d34f55545d060f75dc28df3f6edb8027e2346430d211312c4b150677af76fd7223d457b5b1a
curve n  = 0x3fffffffffffffffffffffffffffffffffffffffffffffffffffffff7cca23e9c44edb49aed63690216cc2728dc58f552378c292ab5844f3
curve h: 4
dh1 lpri  = 0x20faaa60b4280b32155ff7b06186e845dbdabff549509f2ba9c43936995ac5fc6dca7f0006b16bd18936e9747aa5744e0057091f7f7614c
dh1 lpubx = 0x4e1a732e8905f13c876548fd6f9388d855e0ee0edc002df5f67de208cb1520488b34ad8df95f5f7a9618871000fd1bda855b8e55d44829fb
dh1 lpuby = 0x0
dh2 lpri  = 0x163b55f397c81cd0e37e2ad2906e2594216ffb3f90e6c83cc086e417fe669f40213051c988ab58d9f8758d25e27ebf4b5f761aaf4592f614
dh2 lpubx = 0xf91b395eb54a1e1e98ee46c8e96e1f5b1c26311f0679858cf3ad8a7836d16da6d86a73415c7178cf2ff09415c1b61d2a97cd9bd260bb2e1
dh2 lpuby = 0x0

public 1  = 4e1a732e8905f13c876548fd6f9388d855e0ee0edc002df5f67de208cb1520488b34ad8df95f5f7a9618871000fd1bda855b8e55d44829fb
public 2  = 0f91b395eb54a1e1e98ee46c8e96e1f5b1c26311f0679858cf3ad8a7836d16da6d86a73415c7178cf2ff09415c1b61d2a97cd9bd260bb2e1

shared 1 = 83c1153921f418705e10ecd96f0c1ce5f9b7204fb1ee1f96ffaadb0b86f4ad967e1b7ac48b07fa2fdcb92881f85525b80bd1149bcf1b3ea2
shared 2 = 83c1153921f418705e10ecd96f0c1ce5f9b7204fb1ee1f96ffaadb0b86f4ad967e1b7ac48b07fa2fdcb92881f85525b80bd1149bcf1b3ea2

Result is CORRECT



Generating keys...

--- ECC KEY curve448 448 bits ---

PUB = 82c10e2de25815cd3dc7efc261ac9270648bf84670a931d9410cde0b820a0a02fd3802f3d6d2326cc07bbb7dc58622c2eded90c307588921
PRI = 0x9606b3d18409127b246027fcdd58d447a532a69d4eff5db25a85eeb81da003b5420b50f8debd4e1c27f5fd0d9ffcd41fc41b6b7f3ce9b70
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'curve448'
Private Key    = 0x3a8c71eb063e56688ed2ee74e4a725de67ce4c80dd10e88ae1a0104e928d12f2ef0982848fdb5a14d529d6c4a42420886c5d4d12443ae33c
Public Key's X = 0x4bc946e9049b8310648e1061903e29372eb1264208041a37130620c46df3c3c84a37d5f81a60393fdbb3c59ca95a8dbdc8c26560499f9b52
Public Key's Y = 0x0

Public Key Bytes = 4bc946e9049b8310648e1061903e29372eb1264208041a37130620c46df3c3c84a37d5f81a60393fdbb3c59ca95a8dbdc8c26560499f9b52


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x4bc946e9049b8310648e1061903e29372eb1264208041a37130620c46df3c3c84a37d5f81a60393fdbb3c59ca95a8dbdc8c26560499f9b52
Public Key's Y = 0x0

Result is CORRECT




 TEST ECC _END
===== TEST ECDSA =====
Generating key..
Gen pri = 0xc391908c80fde93c3c4940064eace3d41bb7abb3c7bd75697f719313a248e7f076fa50407e412a30c758af9cc654e74b
Gen pub :
  x = 0xc6c12826a118e463115fd8ddbdc060a56d56d613031cca3a9715f89d3593097c76a8952c0a914f24eb8a35ac6201bb1b
  y = 0x148a0b8a86e370904f841a9b60d1dc01564ee70a0c6dde0652c110f6c30d757cda0a35233773b147e6d47c6431dae1ed
Saving private key
DONE IT
Signing...
OK, saving...
Saved Correctly
Readign signature...
Got 0 verifying signature
===== TEST EdDSA 25519 =====
Generating key..
Gen pri = 0x43c887b9238168755a07a6f42f40996cb670de4281fc83e00a640d81cc49760
Gen pub :
  x = 0x617814a42d40f8434feabe8427ebd9e5dde1b1c98566533efb3487935048c321
  y = 0x0
Saving private key
DONE IT
Loaded pri = 0x6032b6570
Loaded pub :
  x = 0xe87f87abd0e3c8d4a83118623962a4544454e4d5081535f77412a48f6c6c5328
  y = 0x0
Signing...
OK, saving...
Saved Correctly
Readign signature...
Got 0 verifying signature
===== TEST EdDSA 448 =====
Generating key..
Gen pri = 0x21bb525657a86f42b9d34433a11f50248d386e76a242d7ed0b3e3fe12272fa15db55c9f0c45bbe02d239a2974f246a10cf43330e58fd274
Gen pub :
  x = 0xfcb8383e8491b940ad9ab0a06e7ecb07bbaf79b3fe9d158f49de7119b974e556dce8b9da32370d5556658b553b1171b56b0a0c5d5652d801
  y = 0x0
Saving private key
DONE IT
Loaded pri = 0x6032b656f
Loaded pub :
  x = 0x23aaa0c980c275969d37dcc8cdaacbe0d35962aac2387053c00ffc0e24fbe23dd83d6b522b537b1c71ced499d41b47ac72d5e85488cdfc03
  y = 0x0
Signing...
OK, saving...
Saved Correctly
Readign signature...
Got 0 verifying signature

===== Test Ethereum =====


Generating Ethereum Account...
Private Key    = 0x8ce71d24a7d7692fe79f6967740450a6eaf84017930aa6395d09597a62091de6
It should be   = 0x8ce71d24a7d7692fe79f6967740450a6eaf84017930aa6395d09597a62091de6
Public Key     = 0x98c5e985b625e825c2503f17bb9b8c422500734c40b8c4016a834da4237f3e4a7ef8f8fe9fdf8b464cd9f5b886a2427991cfff557f81a044573c22abe1ffa5b4
It should be   = 0x98c5e985b625e825c2503f17bb9b8c422500734c40b8c4016a834da4237f3e4a7ef8f8fe9fdf8b464cd9f5b886a2427991cfff557f81a044573c22abe1ffa5b4
Pub Key Hash   = 0x08d59c1fea9c77b377df6a2c71ceeb2ab44262887c7c514f3fb1f987d52e9755
Public Address = 0x71ceeb2ab44262887c7c514f3fb1f987d52e9755
It should be   = 0x71ceeB2Ab44262887c7C514f3Fb1F987D52E9755 (last 20 bytes of public key hash)



Generating Ethereum Account...
Private Key    = 0xaa0aaa79deb7667d375e26194636ccb83be33d6c1d86b81e24c776d9cc462505
It should be   = 0xaa0aaa79deb7667d375e26194636ccb83be33d6c1d86b81e24c776d9cc462505
Public Key     = 0x61fa19f02c951b5c43a152e9dc22582c3b9e53abaa269dc910ef89516f4902a1eedfc5059e93d0708a26495bb238ed74d206652a6d89cc26b78ba928a7cfd6f3
It should be   = 0x61fa19f02c951b5c43a152e9dc22582c3b9e53abaa269dc910ef89516f4902a1eedfc5059e93d0708a26495bb238ed74d206652a6d89cc26b78ba928a7cfd6f3
Pub Key Hash   = 0xa4ecda31bf5dfee3bf6e15b215b5c086d88e88b548008c72769c379b093c58e6
Public Address = 0x15b5c086d88e88b548008c72769c379b093c58e6
It should be   = 0x15b5C086D88e88B548008c72769c379b093c58e6 (last 20 bytes of public key hash)



Generating Ethereum Account...
Private Key    = 0xbadd7c8063c996d2d8f76d17622d83cd6da1100f6d09cee870344eb32ca460bc
It should be   = 0xbadd7c8063c996d2d8f76d17622d83cd6da1100f6d09cee870344eb32ca460bc
Public Key     = 0xcad19a0839eab76a7223bf9193ee8292a33181603064a181347e065d4550fcf38a758b60e0d2996731133b2e30aacc06a5fdd2a797929ae2f71bc1aa9db6496d
It should be   = 0xcad19a0839eab76a7223bf9193ee8292a33181603064a181347e065d4550fcf38a758b60e0d2996731133b2e30aacc06a5fdd2a797929ae2f71bc1aa9db6496d
Pub Key Hash   = 0x1e5850f2c0e650f975cc48ef986ea45e97ad7c638acbf303bb7d7a0a42c00722
Public Address = 0x986ea45e97ad7c638acbf303bb7d7a0a42c00722
It should be   = 0x986EA45e97ad7c638ACbf303Bb7d7a0a42C00722 (last 20 bytes of public key hash)



Generating Ethereum Account...
Private Key    = 0x3f665e8d1f20ac9a92d4f76cfde429f7eca2ab8052f33c300d2edba65b37134b
It should be   = 0x3f665e8d1f20ac9a92d4f76cfde429f7eca2ab8052f33c300d2edba65b37134b
Public Key     = 0xad7a53c34b1c4afc48c35bc09e19678173948a43960713e1e2aff755a4c3858b6781f845dcbc5f952f9fc3ca4c1d54ccd2282f8b59d7853bf2f75fdbeabae58d
It should be   = 0xad7a53c34b1c4afc48c35bc09e19678173948a43960713e1e2aff755a4c3858b6781f845dcbc5f952f9fc3ca4c1d54ccd2282f8b59d7853bf2f75fdbeabae58d
Pub Key Hash   = 0x9cbe48ee5991d40a27969525fdc76fc12b7b7818cd0e94fba9a16b59a2c2703e
Public Address = 0xfdc76fc12b7b7818cd0e94fba9a16b59a2c2703e
It should be   = 0xFDc76Fc12B7B7818cD0e94Fba9a16B59a2c2703e (last 20 bytes of public key hash)



Generating Ethereum Account...
Private Key    = 0x78331f2676711da65bf9866a2b108390ae7a65e2a30da892009e14fb8ade8f7f
It should be   = 0x78331f2676711da65bf9866a2b108390ae7a65e2a30da892009e14fb8ade8f7f
Public Key     = 0x58a6a6c887cb2a468be785e1a6d6eb977d6317ad599ceebaa31547cb63a14a0d1d2590300516193d092616b937bdae04bee36e2dca2adaf47073d9831f758cda
It should be   = 0x58a6a6c887cb2a468be785e1a6d6eb977d6317ad599ceebaa31547cb63a14a0d1d2590300516193d092616b937bdae04bee36e2dca2adaf47073d9831f758cda
Pub Key Hash   = 0xf9c6ea8e83bbba30e5da6ae4bfe3e39d2449e23ab6207260fe501cfe84327f24
Public Address = 0xbfe3e39d2449e23ab6207260fe501cfe84327f24
It should be   = 0xBFE3e39D2449e23aB6207260Fe501CFE84327F24 (last 20 bytes of public key hash)



Generating Ethereum Account...
Private Key    = 0x371c9e8214f0af4fe8f009a7984723fe10463b0edf56af16d302002788558cdb
It should be   = 0x371c9e8214f0af4fe8f009a7984723fe10463b0edf56af16d302002788558cdb
Public Key     = 0xcaad623d04e822705458c5748bdf45b66d446cbf3eff60d8d4f90f697e67a28a2a0c6fe5a40c33d046f3c23d292db4f1ec570750a0378b348273acde67cdea33
It should be   = 0xcaad623d04e822705458c5748bdf45b66d446cbf3eff60d8d4f90f697e67a28a2a0c6fe5a40c33d046f3c23d292db4f1ec570750a0378b348273acde67cdea33
Pub Key Hash   = 0x00d4ec416a431983f6dd540dcda4d74b609529da25402bf2ba62d4b8d1dfb783
Public Address = 0xcda4d74b609529da25402bf2ba62d4b8d1dfb783
It should be   = 0xcDA4d74b609529Da25402bF2bA62D4B8d1DFb783 (last 20 bytes of public key hash)



Generando Ethereum Public Key...
Private Key    = 0xNULL
It should be   = 0xNULL

Public Key's X = 0xca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd3138
It should be   = 0xca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd3138

Public Key's Y = 0x7574077f301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f
It should be   = 0x7574077f301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f



Generando Ethereum Public Key...
Private Key    = 0xb71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291
It should be   = 0xb71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291

Public Key's X = 0xca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd3138
It should be   = 0xca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd3138

Public Key's Y = 0x7574077f301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f
It should be   = 0x7574077f301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f


== CURVES TEST END



```




