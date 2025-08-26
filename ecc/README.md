# ECC
## Elliptic Curve Criptography

Before pictures were much bigger than the whole of Windows 95 (~30 Mb), people was really concerned about the size of ```RSA``` keys and the computational performance of the algorithm. So, they did something about it and divised a new criptography technique based on elliptic curves over finite -Galois- fields. 

Elliptic Curve Criptography bore a much smaller keys to -theoretically- do the same of bigger key algorithms. For example, an ```ECC``` key of 256 bits equalled the security of a counterpart with a 2048 bits key, that is less computational effort and smaller data sizes for the same benefit. 

Now, I said *theoretically* there because in reality ```ECC``` functions do not do **exactly the same** of the other algorithms, as they do not encode/decode data like ```RSA``` or ```AES``` do -if we ignore Elgamal at the moment-, but they can be used to produce signatures or perform key agreement, which is ideal to create smaller certificates and TLS handshakes.

There are many ```ECC``` algorithms, but here we present a small library that uses our typical one-stop shop approach to support the most common curves around, all of them prime curves:

    - Secp: secp192k1, secp192r1, secp256r1, secp256k1, secp384r1, secp521r1
    - Brainpool:  brainpool192r1, brainpool224r1, brainpool256r1 , brainpool320r1, brainpool384r1, brainpool512r1
    - Edwards: curve25519, curve448

There are also the same protocols for key exchange and signature used with the other encription algorithms: ```ECDSA```, ```ECDH```, etc.

Although these algorithms are said to also be vulnerable to the infamous -but great- Shor's algorithm for *Quantum computers*, ```TLSv1.2``` and ```TLSv1.3``` relay heavily on them to replace RSA, specially for key exchange -with a preference for curve25519-. 

To be honest, apart from the key size and a little performance gain, I am not sure how safer is to use ```ECC``` over ```RSA```, especially with regards of Quantum safety. But again, I am an implementor of algorithms, not a criptographer, so I cannot give a proper answer, just a guessed one. 

Said this, and as far as I know from reading stuff, the ratio to break ```ECC``` using Shor's is around ```key_bits * 6``` *qbits*, whereas ```RSA``` seem to require *only* ```key_bits * 2```. Whereas this seem to indicate that ```ECC``` is stronger than ```RSA```, the biggests curves I know of are smaller than ```RSA``` key bits you can easily use.

The curves ```Koblitz sect571k1 (K-571)``` and ```Weierstrass sect571r1 (B-571)``` -which are binary curves- are the longest I know of common use, although they are not used often, as the use of prime curves seem to be preferred over binary curves. This effectively makes the prime curve ```Weierstrass secp521r1``` the biggest curve of widespread use. 

This curve uses a key ```521``` bits long, so in the *Quantum world* it will require ```3126``` *qbits* to find the key. Therefore a ```RSA``` key of ```4096``` bits seem to beat it because it will require ```8192``` *qbits*. 

Again, I am not a criptographer and cannot confirm or deny these things, but to me it looks like ```RSA``` can be made safer by increasing the key size, something we will not get in ```ECC``` until curves of longer key bits are available. This is not to say that one is better than the other, its just to actually say that is very difficult to assess which one is better.  Although it doesn't really matter, as *real* experts say that they are both on similar safety with regards of *Quantum computers*. Basically doesn't matter which one you use, they are both in *danger*.

Now, I personally think that relay on key size is not a good strategy with *Quantum computers*, we need a different one. If I was a criptographer, I would put my effort on finding a way to neutralize Shor's algorithm and test it using an already-available Quantum computer with little amount of qbits, so increasing the number of them will have no effect on the attack.

**Food for thought:** Shor's algorithm realys on the predictability of the result. In common words is like baking with infinite ovens, and then Shor's finds out which one to open to find your cake ready. So if an algorithm that cannot be predicted on that manner is used, the cake will be either carbonized or raw when an ovan is chosen and opened.

Regarding binary curves, we do not use them here because they are not of widespread use these days, and this repository aims to support commonly used stuff. Michael Rosing's book *Implementing Elliptic Curve Cryptography* use binary curves extensively to the point of showing how to create your own random elliptic binary curves and many other interesting stuff -actually, the protocols for ```ECDH``` and ```Elgamal``` here were adapted from the code from this book-. On in his second book: *Elliptic Curve Cryptography for Developers* he talks exclusively about prime fields.

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
dh1 lpri  = 0xdfe471f4c7d06cab534bba604503cdd5f9ad715852910a3b
dh1 lpubx = 0xdcedd9240c9aa03cf20f87d4d178cac20e2c44a3633ed09
dh1 lpuby = 0xeaf68342573e4527aea3ecb1a150ef466866b54d6e27a9fb
dh2 lpri  = 0xbe049e085f54be85c246925d0cfc35bbd7ba6b605c2cb431
dh2 lpubx = 0xc2ca6992dac86d107e6e887b9e80d4121a0e32be24e7777f
dh2 lpuby = 0xc1f844dc4434b4179a86bdef37ec4d2b19a737c9fa315a03

public 1  = 040dcedd9240c9aa03cf20f87d4d178cac20e2c44a3633ed09eaf68342573e4527aea3ecb1a150ef466866b54d6e27a9fb
public 2  = 04c2ca6992dac86d107e6e887b9e80d4121a0e32be24e7777fc1f844dc4434b4179a86bdef37ec4d2b19a737c9fa315a03

shared 1 = 6984bbcb757c108e4708db9a48307089beb2b7600772dd61
shared 2 = 6984bbcb757c108e4708db9a48307089beb2b7600772dd61

Result is CORRECT



Generating keys...

--- ECC KEY secp192k1 192 bits ---

PUB = 04d310dcdd5b043f53e19bcb10743debd2ec856a47d3a4ebe161029ff76366171aab775babfc305c5f65c054684d2e1f67
PRI = 0xf3d549fc42fdc848eac871e028f366a8fcf048f4760c1b87
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp192k1'
Private Key    = 0x9db73970db78565a034558c1d08950071ceb06ed542b825b
Public Key's X = 0xe6b43581cffa9f9ba7eeeb4111e4c1b7be28658b18b64840
Public Key's Y = 0xdca7d9e07af98bd022147c1a199b79263c8e7573c24e8909

Public Key Bytes = 03e6b43581cffa9f9ba7eeeb4111e4c1b7be28658b18b64840


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xe6b43581cffa9f9ba7eeeb4111e4c1b7be28658b18b64840
Public Key's Y = 0xdca7d9e07af98bd022147c1a199b79263c8e7573c24e8909

Result is CORRECT


==== Random Point operation using Curve 'secp192k1' of 192 bits ====

Random point :
  x = 0xb88272347e57e6b511ae3653e280595e2bfacc9f9a33b5c0
  y = 0xf19a6568ea8ebb9b05d28c88c2426977ba675cf5e3d2e52a
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x7249d335fd5cf21c14febf757949aa67bfece1365366352d
  y = 0x5054bc7deca33e2df73effb2707717c6f4f5bb629ec879ba
Is in the curve? Yes

Added to itself :
  x = 0x7249d335fd5cf21c14febf757949aa67bfece1365366352d
  y = 0x5054bc7deca33e2df73effb2707717c6f4f5bb629ec879ba
Is in the curve? Yes

Doubled point  :
  x = 0x7249d335fd5cf21c14febf757949aa67bfece1365366352d
  y = 0x5054bc7deca33e2df73effb2707717c6f4f5bb629ec879ba
Is in the curve? Yes


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
dh1 lpri  = 0xc1d4df2155504a85142edd406dc97fe28db6b8c55f4f90bb
dh1 lpubx = 0x15f3f8568c7aecdae02cf0fb847e3795f2fab76f0e205d24
dh1 lpuby = 0xdad4f9eb1da75aef6e1a5d4c1c87f9fd88562b92dba5a9b8
dh2 lpri  = 0xf64b7d15cfcc74e2e4e342bd62e0cb26f4e9555a5884c45e
dh2 lpubx = 0x243977d801bd5abb2a42cd91df394d777b91fff13822b966
dh2 lpuby = 0x9d653fa63ad29005a3aa8dda5bcaccec3358b7fb45000979

public 1  = 0415f3f8568c7aecdae02cf0fb847e3795f2fab76f0e205d24dad4f9eb1da75aef6e1a5d4c1c87f9fd88562b92dba5a9b8
public 2  = 04243977d801bd5abb2a42cd91df394d777b91fff13822b9669d653fa63ad29005a3aa8dda5bcaccec3358b7fb45000979

shared 1 = 310f039fd313cb9614ae952425c72df894da386aefaadea3
shared 2 = 310f039fd313cb9614ae952425c72df894da386aefaadea3

Result is CORRECT



Generating keys...

--- ECC KEY secp192r1 192 bits ---

PUB = 0451607a8d291cf30e2fc59956eaaa88fb43f0a77dc6091e524cf6f2e1155636df9a08c396bf3fdbea4653a25d5e4cd1a8
PRI = 0x9fa5f8573ecdc008aaa4f34a035260b57eb5520d6f344e5c
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp192r1'
Private Key    = 0xd69fd6c293e5d14015acc4a5a48995616d8d26e3da154e61
Public Key's X = 0x34dc246b43ac88c5e6c15f264a5d76210172b6103f212794
Public Key's Y = 0x1bf19f57e9646aea863cc196ca0dd1bc3127d0ebfff62201

Public Key Bytes = 0334dc246b43ac88c5e6c15f264a5d76210172b6103f212794


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x34dc246b43ac88c5e6c15f264a5d76210172b6103f212794
Public Key's Y = 0x1bf19f57e9646aea863cc196ca0dd1bc3127d0ebfff62201

Result is CORRECT


==== Random Point operation using Curve 'secp192r1' of 192 bits ====

Random point :
  x = 0xec67c27fbd6624fdf9748e903564056e81e0ec7f13fb4e08
  y = 0x9f4eff7041ba6e431358d639f63afa60469364714cb0f3c2
Is in the curve? Yes

Mulitplied by 2 :
  x = 0xfa8ecee8e7f44b27bc10336e2667b59f26ffc66553b2850a
  y = 0xdc083e0436c8db1c618eb6c1aa00201f8e806b8c509e0836
Is in the curve? Yes

Added to itself :
  x = 0xfa8ecee8e7f44b27bc10336e2667b59f26ffc66553b2850a
  y = 0xdc083e0436c8db1c618eb6c1aa00201f8e806b8c509e0836
Is in the curve? Yes

Doubled point  :
  x = 0xfa8ecee8e7f44b27bc10336e2667b59f26ffc66553b2850a
  y = 0xdc083e0436c8db1c618eb6c1aa00201f8e806b8c509e0836
Is in the curve? Yes


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
dh1 lpri  = 0xacbc2c584840f0b81eeb3cc367d7b687f4401cd60ea6deecc23c0766beff6540
dh1 lpubx = 0xeb997cfad3048e4f084d7b4dc8cdab760b50cadd159a87a479d53e70c800c012
dh1 lpuby = 0x11587ee611320edd5c0661b7da35d47fa1763861d1f3d60da603f3f012c22e83
dh2 lpri  = 0xef78b4e34eb0ef2c66b18182dbdcd85d01175f38c001b2454ab0f692648bdd11
dh2 lpubx = 0xfe1e38c92046214c47a87b1aa96ad5557d3036e0bf0bbdbecf8a8c060e2a7184
dh2 lpuby = 0xbf9861df988d9472ec3c6168a6b1ed6a88920de348e82d1bd4b1338394516a4e

public 1  = 04eb997cfad3048e4f084d7b4dc8cdab760b50cadd159a87a479d53e70c800c01211587ee611320edd5c0661b7da35d47fa1763861d1f3d60da603f3f012c22e83
public 2  = 04fe1e38c92046214c47a87b1aa96ad5557d3036e0bf0bbdbecf8a8c060e2a7184bf9861df988d9472ec3c6168a6b1ed6a88920de348e82d1bd4b1338394516a4e

shared 1 = 2f3beea34d040ed42b2b25eb5892fb46352b32be8668f42166b434152414e686
shared 2 = 2f3beea34d040ed42b2b25eb5892fb46352b32be8668f42166b434152414e686

Result is CORRECT



Generating keys...

--- ECC KEY secp256r1 256 bits ---

PUB = 04473cc67c47828f79cede126846d4528c0f29171f199f06ea8672b3738386f8cd3608a98202a347f51d461e71751be260604f26dea0eae40329538d8dbc065533
PRI = 0xfd8dbf5c19cc0103bdebd55f43f4206ef53e8a22711e086c109709ca7a9097f8
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp256r1'
Private Key    = 0xc15f13556f9929e1eadb03a13041ca8a106a38f5f2c29b2b0a35135005bd63a6
Public Key's X = 0xc68b0f7e325abbaac35017bcfa3ef24f3e37e654798c746340673c3b6e72b139
Public Key's Y = 0x4d7aaed1a9efbd30fd55f02b31c92964a11d270dc468fa4d1089fc7fc88c2f1c

Public Key Bytes = 02c68b0f7e325abbaac35017bcfa3ef24f3e37e654798c746340673c3b6e72b139


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xc68b0f7e325abbaac35017bcfa3ef24f3e37e654798c746340673c3b6e72b139
Public Key's Y = 0x4d7aaed1a9efbd30fd55f02b31c92964a11d270dc468fa4d1089fc7fc88c2f1c

Result is CORRECT


==== Random Point operation using Curve 'secp256r1' of 256 bits ====

Random point :
  x = 0xa2dbde5abc05793a92eef8f6f7539985278eef1a5666628232af15b46f4ca185
  y = 0xade0bb57b0916a0ecca06fd1cde68d4378a3c0dce52bbc720068359d2a08f7b1
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x3d18c7511486d5a41d212ad2ee8ad5ec65d8cad349c38cfb88c49473f4e14545
  y = 0xca4f50c57f0c35f7bacf1b18e7657ee7b4cc3fe7952338350a915aea9ba53a67
Is in the curve? Yes

Added to itself :
  x = 0x3d18c7511486d5a41d212ad2ee8ad5ec65d8cad349c38cfb88c49473f4e14545
  y = 0xca4f50c57f0c35f7bacf1b18e7657ee7b4cc3fe7952338350a915aea9ba53a67
Is in the curve? Yes

Doubled point  :
  x = 0x3d18c7511486d5a41d212ad2ee8ad5ec65d8cad349c38cfb88c49473f4e14545
  y = 0xca4f50c57f0c35f7bacf1b18e7657ee7b4cc3fe7952338350a915aea9ba53a67
Is in the curve? Yes


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
dh1 lpri  = 0xe364f34be6f9740844b117983f850322f146457c951f40021aac05c13f784458
dh1 lpubx = 0x35706accc4d406e83e3d1e8b59a9ab9f7a89ce0d204ac52551d5aa6ba2718b6a
dh1 lpuby = 0x52daa21f4c0c870bd11a0dcecef76ac8d565e5d70b154443ef0f33cfd7491242
dh2 lpri  = 0x89b5721be83396f5366b9af18aa36df272bfd6f13f0e67b3b43e42a65bfbe014
dh2 lpubx = 0xc144ea1151057c34634ea00ebba9f35a1a550d621f3b65ce5faf7bbf7ff8d7e2
dh2 lpuby = 0xad2241ca53022356daea36f35eabae5ff4a109ac90eef987f4d37de8847bbb87

public 1  = 0435706accc4d406e83e3d1e8b59a9ab9f7a89ce0d204ac52551d5aa6ba2718b6a52daa21f4c0c870bd11a0dcecef76ac8d565e5d70b154443ef0f33cfd7491242
public 2  = 04c144ea1151057c34634ea00ebba9f35a1a550d621f3b65ce5faf7bbf7ff8d7e2ad2241ca53022356daea36f35eabae5ff4a109ac90eef987f4d37de8847bbb87

shared 1 = c3b32049cf9df6a023f750edb2eea9a4da5bf49e5c5ddbe8637a451922377fb3
shared 2 = c3b32049cf9df6a023f750edb2eea9a4da5bf49e5c5ddbe8637a451922377fb3

Result is CORRECT



Generating keys...

--- ECC KEY secp256k1 256 bits ---

PUB = 04a4e040ca2c6d87619ab59a991ba1945f4beae232499e95a7c7b6bcdde3cdf565a55b7c76855bcf548ae7624ed7bcafb508d368c000ff0fb6686d2fef96a0dd61
PRI = 0xe5fd2d0d959cc462536ab2641ad8e30d414ef71697329532133ecee4ce208eab
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp256k1'
Private Key    = 0xbe95a4b1ab5bf9635ea0d6b42284248ecc8376def2cdfe89f34e0219eb0ef9f6
Public Key's X = 0xa86f7c4bce8127c5b35d5bbbf4d918ee9bd21da0439149347442e13a09c784ab
Public Key's Y = 0xec2164f5c86d4c107f4a1acd82b1f33252499308203b5320a8f23cd8c68ccc15

Public Key Bytes = 03a86f7c4bce8127c5b35d5bbbf4d918ee9bd21da0439149347442e13a09c784ab


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xa86f7c4bce8127c5b35d5bbbf4d918ee9bd21da0439149347442e13a09c784ab
Public Key's Y = 0xec2164f5c86d4c107f4a1acd82b1f33252499308203b5320a8f23cd8c68ccc15

Result is CORRECT


==== Random Point operation using Curve 'secp256k1' of 256 bits ====

Random point :
  x = 0xfd37ba08139b706707c8b9fb3b2946ee70250ff4b208fa02ff4edfa7a8f380fe
  y = 0xc04d9c1047f8ee7c782a58e4ecc801a2706fdd7f496133b03e676f393d051dba
Is in the curve? Yes

Mulitplied by 2 :
  x = 0xce28f6533d5acc9327c6a657f159b905403434e9f0dd4d3b97ada4427652f134
  y = 0xf6fe184df0a0f40f00836db3aa454627128f5ccbce10758788a6629319149363
Is in the curve? Yes

Added to itself :
  x = 0xce28f6533d5acc9327c6a657f159b905403434e9f0dd4d3b97ada4427652f134
  y = 0xf6fe184df0a0f40f00836db3aa454627128f5ccbce10758788a6629319149363
Is in the curve? Yes

Doubled point  :
  x = 0xce28f6533d5acc9327c6a657f159b905403434e9f0dd4d3b97ada4427652f134
  y = 0xf6fe184df0a0f40f00836db3aa454627128f5ccbce10758788a6629319149363
Is in the curve? Yes


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
dh1 lpri  = 0xc4b8342974c758adfaea74e66017a65c433d068151fb22a70fadc324bbcf3a90708097ce479160c7be131f34b231c123
dh1 lpubx = 0x859c68d9b9ab046d5841a6e65085db945a946145da234d359f708cc4300123ef7669c2e6a2264210b703f44e92ec5add
dh1 lpuby = 0xc25f4e6e09a38e615a4dadd84f48b4a19d254c4c93ce4acd66c1fb89c642f184a959d4323b05b9e22dfab5d22ad893ef
dh2 lpri  = 0xbae2ef930206a5dc156055a2f1cebd7214af4f83d01a3ca091d24a7a265e3839ff45fe1307287b67971cc872fda99352
dh2 lpubx = 0xf79cdf52cf779e06c44dff2571f21e6e3e0816cd0a92cbdde250e6c3b21ecc1cf33026ac7f082df8434968446da462b8
dh2 lpuby = 0x94fd13efc8a5f1b1003c130f34b635bad899230579ddf17f361c9867d7690940a82ecc488d3517eed2c2864fbd5a6f3f

public 1  = 04859c68d9b9ab046d5841a6e65085db945a946145da234d359f708cc4300123ef7669c2e6a2264210b703f44e92ec5addc25f4e6e09a38e615a4dadd84f48b4a19d254c4c93ce4acd66c1fb89c642f184a959d4323b05b9e22dfab5d22ad893ef
public 2  = 04f79cdf52cf779e06c44dff2571f21e6e3e0816cd0a92cbdde250e6c3b21ecc1cf33026ac7f082df8434968446da462b894fd13efc8a5f1b1003c130f34b635bad899230579ddf17f361c9867d7690940a82ecc488d3517eed2c2864fbd5a6f3f

shared 1 = b48a91cfec9cefbaffb26dc89b38533f7622ff8901da0b989b514c804b17d0a766ca3395acfbee9b5e4b5a2df6e3783d
shared 2 = b48a91cfec9cefbaffb26dc89b38533f7622ff8901da0b989b514c804b17d0a766ca3395acfbee9b5e4b5a2df6e3783d

Result is CORRECT



Generating keys...

--- ECC KEY secp384r1 384 bits ---

PUB = 046999f28d546cdbe50a3682b01d36da0d6f7b36534e1c4a10fb9b935d82c0ad2bbad163df22de3de8b8b98b1ae676b339e1e25d46034f7ef4269df547a632d0e5329e480a5f24cdd0ca835a9b30f6b1ff0a80a932212acb25f7c8242bf8399774
PRI = 0xadcf48c9c1a58cc9a5555d68ecb0fead74020d1497c2592ba59c0f266843cd4a0e0497ee37dd559d396309ccf0405d70
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp384r1'
Private Key    = 0xa5e879c4adc0898c23b170c85299544f620cd01063028ce22da2f00f5e32cea95f5d9bf31aeb520540bc7dce6e90a1b7
Public Key's X = 0x27ac52e2d7861ed33561c89bc0da099cc6686d1dc9393e435e3e88dfb77e542614b0da76e855afd00bbb292c83de5c57
Public Key's Y = 0x3248c90f17da50adb30725feae73fda59be925b7697fe231c3c1198b1a7e0e5004bbd2767c9d3706cba6e1595bdb325c

Public Key Bytes = 0227ac52e2d7861ed33561c89bc0da099cc6686d1dc9393e435e3e88dfb77e542614b0da76e855afd00bbb292c83de5c57


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x27ac52e2d7861ed33561c89bc0da099cc6686d1dc9393e435e3e88dfb77e542614b0da76e855afd00bbb292c83de5c57
Public Key's Y = 0x3248c90f17da50adb30725feae73fda59be925b7697fe231c3c1198b1a7e0e5004bbd2767c9d3706cba6e1595bdb325c

Result is CORRECT


==== Random Point operation using Curve 'secp384r1' of 384 bits ====

Random point :
  x = 0xa0e7ac8fcbcfddcb627468183f2808649b037ed18435330f8bbeaf48643d1d9e6d654bc0f8a2bfb8ef2112f19beda38b
  y = 0xc2df744393656a29f8ccb10c49ff53c2ad3b2c30904e353fd0debebfa9f15a79372810a5d5dbe8757327d1f554387a65
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x68a86ba034f858e2086c642d67e9d1bfe02d594e35af6372f517be67c31b2793594b99d3cb28c26b310114455ed7ef1e
  y = 0x655d113bd6edaea7c4bc2db3d3ad0d4c259cd95bfb7c94c1de7e2ece82101f14ea07eade9689150670389e33c3baef7e
Is in the curve? Yes

Added to itself :
  x = 0x68a86ba034f858e2086c642d67e9d1bfe02d594e35af6372f517be67c31b2793594b99d3cb28c26b310114455ed7ef1e
  y = 0x655d113bd6edaea7c4bc2db3d3ad0d4c259cd95bfb7c94c1de7e2ece82101f14ea07eade9689150670389e33c3baef7e
Is in the curve? Yes

Doubled point  :
  x = 0x68a86ba034f858e2086c642d67e9d1bfe02d594e35af6372f517be67c31b2793594b99d3cb28c26b310114455ed7ef1e
  y = 0x655d113bd6edaea7c4bc2db3d3ad0d4c259cd95bfb7c94c1de7e2ece82101f14ea07eade9689150670389e33c3baef7e
Is in the curve? Yes


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
dh1 lpri  = 0x17fe501b5fc5bc9cf0cc626befa86d71507650bbb38851ee60abc98a21ca43b118145dfbbe66f124d715bfe433d9690c8c5824295ddbfcccbcd4d940cd17ecaf1d3
dh1 lpubx = 0xe7a6497237481550b2698ceb4da0db68bafcc08e1dc65a75df32c745a405c2a0c9b769b401cd150d21f019fcd4bb5e5de75b96a4a9ce0a6efb3307c801d994d21
dh1 lpuby = 0x9c17971e09696b3860010be500ca84b0d20ac9e107940bc0425d9b5e7b896b5cb752ac0bc05870624c85857ccf4ccc10031a3199c4e3543ef7cdae2f8cb5db6ef2
dh2 lpri  = 0x17667cdba4a7342f36c2437e8c8f9d38be021650c51f2ed4ca70395791e4caa4045dbc8bde246ee235c6bd95c70e394183c372c1c8877238467609bfb866ea014a5
dh2 lpubx = 0x1d02b89acbc1654921d6ba5b952d58dcd9833e363d9370425e1e9215ed0184974550b1dffee840f1497bf1acae9d71aebf05c021fbf696ead17bd13533ee8c0487b
dh2 lpuby = 0x61290854517267c03576c988cbaa45b0505b6b1e7d5a505d1bcf4cd4dfaf261d1d68651211e3f9d3f7a3370d4db01cf5398b89c83a0ba76385cf643d72d5ea0d1d

public 1  = 04000e7a6497237481550b2698ceb4da0db68bafcc08e1dc65a75df32c745a405c2a0c9b769b401cd150d21f019fcd4bb5e5de75b96a4a9ce0a6efb3307c801d994d21009c17971e09696b3860010be500ca84b0d20ac9e107940bc0425d9b5e7b896b5cb752ac0bc05870624c85857ccf4ccc10031a3199c4e3543ef7cdae2f8cb5db6ef2
public 2  = 0401d02b89acbc1654921d6ba5b952d58dcd9833e363d9370425e1e9215ed0184974550b1dffee840f1497bf1acae9d71aebf05c021fbf696ead17bd13533ee8c0487b0061290854517267c03576c988cbaa45b0505b6b1e7d5a505d1bcf4cd4dfaf261d1d68651211e3f9d3f7a3370d4db01cf5398b89c83a0ba76385cf643d72d5ea0d1d

shared 1 = 0117b38319cac951a6b967240f042db2b812e2d35a1b092fb96d4fc4a1363a667b57cc89f0f959656b7b2d6d00fcb895e89e8c67955cec2ebe27ace521e761d480ae
shared 2 = 0117b38319cac951a6b967240f042db2b812e2d35a1b092fb96d4fc4a1363a667b57cc89f0f959656b7b2d6d00fcb895e89e8c67955cec2ebe27ace521e761d480ae

Result is CORRECT



Generating keys...

--- ECC KEY secp521r1 521 bits ---

PUB = 04002b1e07e8aca20ecc93de967cd501c0ab85a606b3306ce606cfaea43c3d9e0e3ce608995d2b9c5ba9c70db142397a4edd9cbe174692bd009094d62bbcf1bf6bbf8d00ff5545a4c1302532881b06ae4d95996059b7c185288465cbb76caf8b01f16c0f50f3fc06276253ac3dcd93b91de13ffc2733f8fea828bc21f3c955d8b2ea680271
PRI = 0x1c42af1f1330c90a87adc4a5dedeaba9646584c569e159c3a87d760ca1410a91f4a03d318d7d659b708e4cf09880428b039f6c6071f11959a0a0d4c006972e07d7e
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'secp521r1'
Private Key    = 0x11b90a1d81a5b91284444e3c09cf2af4d2416fd25315029a57e748df966f4d2cacc3264e2f0e2951594f0fb79105b51a28f79ddb331dadddb98cdaed24e24ecca32
Public Key's X = 0x8cdb519d7491df05c1898c5841bbdb4538952dd28c241c001daf36f77fca531af9565ee19791a0d512472148d189d29a11a73db23cdb43bba57bfa61ac4131edf8
Public Key's Y = 0xe86c03eda57c0eee3249f24c39d5bcff0732ca435a2a0800119e3f4c7a10e28ed0288dabc5454629afc774db2694217e22d36e1ef190d1e907a2552a0c1fbcdbb7

Public Key Bytes = 03008cdb519d7491df05c1898c5841bbdb4538952dd28c241c001daf36f77fca531af9565ee19791a0d512472148d189d29a11a73db23cdb43bba57bfa61ac4131edf8


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x8cdb519d7491df05c1898c5841bbdb4538952dd28c241c001daf36f77fca531af9565ee19791a0d512472148d189d29a11a73db23cdb43bba57bfa61ac4131edf8
Public Key's Y = 0xe86c03eda57c0eee3249f24c39d5bcff0732ca435a2a0800119e3f4c7a10e28ed0288dabc5454629afc774db2694217e22d36e1ef190d1e907a2552a0c1fbcdbb7

Result is CORRECT


==== Random Point operation using Curve 'secp521r1' of 521 bits ====

Random point :
  x = 0x12aade0aac2e67c47c376cd765d9ffa5d987c0ccc6ccc50db91f9c50cb1ca4267ab6b66cc9fc8a257ef3bd8cf566e71fc297c2bce74cb377e3b01fc46c62241a013
  y = 0x6d0bbd04ab70a0f0782bd5398b93940a35f793522d5ffa0e12a072c64d17c3261596e7b8fa91c8d5cbaba42ded985010ed845bb71f3da4ecc497ed7d8a0418eb61
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x15ef6a23bc9afb1b58bfc87ae378dbb09a97ff74dfbe87900c3c9e3a377ef3c93ce3459555a03e93cf98c9a9710048c0635b3ba8d5d3d1aa5d6fd24ecfea959967b
  y = 0xdaf549a718bc6b73c32512d420fcc8b26ba82bedd5bf96ab2fb2992fee4555a7de15fea9992568bae4faabbf9ebd66a452d2479d4d737cbb628d6a22c4f43745af
Is in the curve? Yes

Added to itself :
  x = 0x15ef6a23bc9afb1b58bfc87ae378dbb09a97ff74dfbe87900c3c9e3a377ef3c93ce3459555a03e93cf98c9a9710048c0635b3ba8d5d3d1aa5d6fd24ecfea959967b
  y = 0xdaf549a718bc6b73c32512d420fcc8b26ba82bedd5bf96ab2fb2992fee4555a7de15fea9992568bae4faabbf9ebd66a452d2479d4d737cbb628d6a22c4f43745af
Is in the curve? Yes

Doubled point  :
  x = 0x15ef6a23bc9afb1b58bfc87ae378dbb09a97ff74dfbe87900c3c9e3a377ef3c93ce3459555a03e93cf98c9a9710048c0635b3ba8d5d3d1aa5d6fd24ecfea959967b
  y = 0xdaf549a718bc6b73c32512d420fcc8b26ba82bedd5bf96ab2fb2992fee4555a7de15fea9992568bae4faabbf9ebd66a452d2479d4d737cbb628d6a22c4f43745af
Is in the curve? Yes


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
dh1 lpri  = 0x2b9890dc3707099c2faee9c64158d1ea862d6ac1efbcc957
dh1 lpubx = 0x65b633b2139e1ed9199f3da0106327fb777998405d8c1f92
dh1 lpuby = 0xb9f665b6b36b79d3574166fab982eb7a391c4d09669ebba3
dh2 lpri  = 0xa5c72c19981f0a2994d5e46eae4444702efdd59361197d40
dh2 lpubx = 0x2d406a11c640a3440174216b6d967a92b5ceb0055bdb9709
dh2 lpuby = 0x6d9587cc276d29bfec1802d9695c52871e6351f7ebe72862

public 1  = 0465b633b2139e1ed9199f3da0106327fb777998405d8c1f92b9f665b6b36b79d3574166fab982eb7a391c4d09669ebba3
public 2  = 042d406a11c640a3440174216b6d967a92b5ceb0055bdb97096d9587cc276d29bfec1802d9695c52871e6351f7ebe72862

shared 1 = 73e10cd00e78e44ebca909c798e02ca75400fda3c3e1fa74
shared 2 = 73e10cd00e78e44ebca909c798e02ca75400fda3c3e1fa74

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP192r1 192 bits ---

PUB = 0449a2c4ce8113bee4c5df6a8eded30d64fd062322985e8bee5826fcdde23dc608a4d85ff9d8b8baca829e03217ae8b8f8
PRI = 0xaf4a9381fbffd5ab944a52361ec894131e5edf48c66a63ae
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP192r1'
Private Key    = 0xc11f032b83e206206483d8f0af7e326ef719a34bdf5fdb6c
Public Key's X = 0x6928c871b29a35e654786055c643a00128c223dad7180ec6
Public Key's Y = 0x987f1719774fb1d4e46ff2856e779ee9d3113a82375f31c4

Public Key Bytes = 026928c871b29a35e654786055c643a00128c223dad7180ec6


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x6928c871b29a35e654786055c643a00128c223dad7180ec6
Public Key's Y = 0x987f1719774fb1d4e46ff2856e779ee9d3113a82375f31c4

Result is CORRECT


==== Random Point operation using Curve 'brainpoolP192r1' of 192 bits ====

Random point :
  x = 0x2b6508ad1914c53585a250e7cd7aa55f81f006cfcae4f333
  y = 0x33fe6ce421ffcfca2f24d99f00aa7e90e98620d6928922b7
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x33fa6d22f21b2fcf8ac68515bac9af25d39811f9b574cabd
  y = 0x12793c5944941f8621769061a57fd9e80eb8e980c9967f21
Is in the curve? Yes

Added to itself :
  x = 0x33fa6d22f21b2fcf8ac68515bac9af25d39811f9b574cabd
  y = 0x12793c5944941f8621769061a57fd9e80eb8e980c9967f21
Is in the curve? Yes

Doubled point  :
  x = 0x33fa6d22f21b2fcf8ac68515bac9af25d39811f9b574cabd
  y = 0x12793c5944941f8621769061a57fd9e80eb8e980c9967f21
Is in the curve? Yes


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
dh1 lpri  = 0x8be2493bf049bbf7c3dab45a3236ee4f25583658e110283a3fd1a80b
dh1 lpubx = 0x4aba757ec5af83a3876479c8a9f703d7c59dabadd0e1051d653ed064
dh1 lpuby = 0x9901a8e91785afdb3d0701e459ce00fcb2083d06afc6a8538f244720
dh2 lpri  = 0xb5efaba8bddd43aad5e3d2b1fa4455d1716a86a2d483326f13259325
dh2 lpubx = 0xce5dd8111b3ed4631bf74d393d61d92aeaecffcf087ec3d65ada536a
dh2 lpuby = 0xb60cc29d02bfd4536a1b53b921880d30d2f951bac399d5dfdaacea6a

public 1  = 044aba757ec5af83a3876479c8a9f703d7c59dabadd0e1051d653ed0649901a8e91785afdb3d0701e459ce00fcb2083d06afc6a8538f244720
public 2  = 04ce5dd8111b3ed4631bf74d393d61d92aeaecffcf087ec3d65ada536ab60cc29d02bfd4536a1b53b921880d30d2f951bac399d5dfdaacea6a

shared 1 = 0fed4b32726da4a163e3e90acfe1615b2874256a9741ba26db260396
shared 2 = 0fed4b32726da4a163e3e90acfe1615b2874256a9741ba26db260396

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP224r1 224 bits ---

PUB = 046094e7ba9059b37d0bdbcc595a9e0b1d0a7027f2c1a4b1ecbd5b127b5601fa3b1320f3d50e682433341fdef8487a0381d30365362d6df2d5
PRI = 0xcdffbbac5efee01fc4dd72660f16b9c8ac5a85c58c2147c46e79f0e0
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP224r1'
Private Key    = 0xa50bcac426dec83c858ad619b421d2cfefdc8d15872b0b55d82585c7
Public Key's X = 0xb4002eb1ebc1bee9074d349f3c5729ee4078d16742280d3066347c1a
Public Key's Y = 0x231f27708f43b381df285259e76ef8e6f9ad07372d2570a301a71a7f

Public Key Bytes = 03b4002eb1ebc1bee9074d349f3c5729ee4078d16742280d3066347c1a


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0xb4002eb1ebc1bee9074d349f3c5729ee4078d16742280d3066347c1a
Public Key's Y = 0x231f27708f43b381df285259e76ef8e6f9ad07372d2570a301a71a7f

Result is CORRECT


==== Random Point operation using Curve 'brainpoolP224r1' of 224 bits ====

Random point :
  x = 0xaf4293d60232609972d4f86a6636e5161d3c3a43dd825831b1d70c60
  y = 0x9d207dc0204dc62bd0b05b5c5bee3172329e09fc5c09e31967d1ee34
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x926b719226e65e2c81b871e0f159f5bfa1d66df0f51ecc6e20fac84a
  y = 0x8fe53ec1dcf4c23e05845c36d6d58c277914742608867c5e3805b178
Is in the curve? Yes

Added to itself :
  x = 0x926b719226e65e2c81b871e0f159f5bfa1d66df0f51ecc6e20fac84a
  y = 0x8fe53ec1dcf4c23e05845c36d6d58c277914742608867c5e3805b178
Is in the curve? Yes

Doubled point  :
  x = 0x926b719226e65e2c81b871e0f159f5bfa1d66df0f51ecc6e20fac84a
  y = 0x8fe53ec1dcf4c23e05845c36d6d58c277914742608867c5e3805b178
Is in the curve? Yes


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
dh1 lpri  = 0xa63bb01f0ae14375850a02d58bfc83852bfebbac6156df8d2af1a82d499e0d81
dh1 lpubx = 0x1b8770ef18455e73ba937246e9719b5fe100084dc01402d431181e6f2c147edc
dh1 lpuby = 0x6c449939d39db2d7306aacf8f806dd76441fb6d0b1ab4d69df647a62afa1ea1b
dh2 lpri  = 0x81bf19493362ae41fd2a5750a1e5cd2ef6bebb2fff8bc7596c834323f65c4019
dh2 lpubx = 0x7e14626c16c8f53159edd401eed1ff761b4bf963a76f62e9f042b07b0ec95238
dh2 lpuby = 0x378953f32a756d19b997ea604259fae8d7251a9d672840a444c2205aa3517a71

public 1  = 041b8770ef18455e73ba937246e9719b5fe100084dc01402d431181e6f2c147edc6c449939d39db2d7306aacf8f806dd76441fb6d0b1ab4d69df647a62afa1ea1b
public 2  = 047e14626c16c8f53159edd401eed1ff761b4bf963a76f62e9f042b07b0ec95238378953f32a756d19b997ea604259fae8d7251a9d672840a444c2205aa3517a71

shared 1 = 6ab5fc387bb0e31da1eea72c5679c0dc8a9011580f3e7ae7725f7e1ec6866dac
shared 2 = 6ab5fc387bb0e31da1eea72c5679c0dc8a9011580f3e7ae7725f7e1ec6866dac

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP256r1 256 bits ---

PUB = 049f1ef8c62a5d65d1a227f205dabd4d59f3e50467cc5f0d7469b07c98ee429d5ca9c2229ec54fd17c0efc843b1f0d41ac29d82bc4f6319f14f3cafba9e38dd679
PRI = 0x194c1eecb4d45b1bbd53448f9464227509a6276c8f294d3697800351a3daf2c8
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP256r1'
Private Key    = 0x81ba8e2b3ff566e9ecb7858ebc350b78953782bbc794232070930897286e5950
Public Key's X = 0x7d3fef511214d2d599ec7e1709c4c08ab74b83a7c1d148206768db8285370ab8
Public Key's Y = 0x591d833237d1b5a40155a069f0a972c4d6b10ae5b3a620a8094de9df0d593e1c

Public Key Bytes = 027d3fef511214d2d599ec7e1709c4c08ab74b83a7c1d148206768db8285370ab8


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x7d3fef511214d2d599ec7e1709c4c08ab74b83a7c1d148206768db8285370ab8
Public Key's Y = 0x591d833237d1b5a40155a069f0a972c4d6b10ae5b3a620a8094de9df0d593e1c

Result is CORRECT


==== Random Point operation using Curve 'brainpoolP256r1' of 256 bits ====

Random point :
  x = 0x9a5aae56df97c01480346b8287d01031e97dc215a2812a404a94c4bae2e1a43e
  y = 0x1505853046f8c2e36a9ed7f71119d692cbfc9b108a796561b99de5c7703c50a
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x14f9e9ca27000f7c4947bee56cbb8eb1d9c8616975224228706ec9d6fd58a256
  y = 0x5bc59ab5006d7f2fd7fd8376fb00206810d3ee02a404e06fa491674396f137a2
Is in the curve? Yes

Added to itself :
  x = 0x14f9e9ca27000f7c4947bee56cbb8eb1d9c8616975224228706ec9d6fd58a256
  y = 0x5bc59ab5006d7f2fd7fd8376fb00206810d3ee02a404e06fa491674396f137a2
Is in the curve? Yes

Doubled point  :
  x = 0x14f9e9ca27000f7c4947bee56cbb8eb1d9c8616975224228706ec9d6fd58a256
  y = 0x5bc59ab5006d7f2fd7fd8376fb00206810d3ee02a404e06fa491674396f137a2
Is in the curve? Yes


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
dh1 lpri  = 0x18a00d243682f549dee79ad63b0ea30620f94cfc10a4f6d43902c5a92eb1a3de6c82eb11f12f873a
dh1 lpubx = 0x6293221bfcd90df75204989ec1c9c765a677dae7da16bcac8a1e29ab0d8e76bbee964c6a17e7698a
dh1 lpuby = 0x35935595ece4f8599d55e841f882b7045b9cf80923f08a224fbf2ccaa12f958974d48a299c424eb0
dh2 lpri  = 0x9f7ef09c3af789a53b6167ba55a8333613b7c8e5f84db2453f6e571b91db8f90eab503b3a22c44b9
dh2 lpubx = 0xaa8aaf0031daaaf82e0b7e046ba70511627e8727049e26ede5b949b183a1967c7fca9a32b72bb400
dh2 lpuby = 0x58396d4d1d53c2a6108332a74544d87f702c4ce146e5808f4feb037d9c06647d8143252d0ea610e1

public 1  = 046293221bfcd90df75204989ec1c9c765a677dae7da16bcac8a1e29ab0d8e76bbee964c6a17e7698a35935595ece4f8599d55e841f882b7045b9cf80923f08a224fbf2ccaa12f958974d48a299c424eb0
public 2  = 04aa8aaf0031daaaf82e0b7e046ba70511627e8727049e26ede5b949b183a1967c7fca9a32b72bb40058396d4d1d53c2a6108332a74544d87f702c4ce146e5808f4feb037d9c06647d8143252d0ea610e1

shared 1 = 19c2c1524d3a3fb88a85ef60c83daaf6de9b880e11685f043c3766318801b7a48473cdd7957f8878
shared 2 = 19c2c1524d3a3fb88a85ef60c83daaf6de9b880e11685f043c3766318801b7a48473cdd7957f8878

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP320r1 320 bits ---

PUB = 042f06e84a8855b25dbcbff861c6b39088da92be0d0f80e75170eae58817869a47830f76e1f29ad5340fe9f51c388969bff363087c88240b435e1ef3ab1beb8f649f18fd1f4a57dbfea20c55d4b1896ac6
PRI = 0x9fcaba42c7e526f4d120c1061655687084c1873d873e92a293ea4531f4762bbf6a2df0f04ecaefb7
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP320r1'
Private Key    = 0x963d2a9a8d1d3491ab47c556991821983047a7dde0a65ef2366a041210265283774e9c99e6a5a81b
Public Key's X = 0x52be5c301068700f5e25170a2d565a9df5fff5c7622b68991ebce9a69aa36f765f0e3b30f128be8e
Public Key's Y = 0x49c18e054e8216a3d9fbf86082a91261a7f55c170c195e6f9ea3c457eaff4153984d938f166ff479

Public Key Bytes = 0352be5c301068700f5e25170a2d565a9df5fff5c7622b68991ebce9a69aa36f765f0e3b30f128be8e


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x52be5c301068700f5e25170a2d565a9df5fff5c7622b68991ebce9a69aa36f765f0e3b30f128be8e
Public Key's Y = 0x49c18e054e8216a3d9fbf86082a91261a7f55c170c195e6f9ea3c457eaff4153984d938f166ff479

Result is CORRECT


==== Random Point operation using Curve 'brainpoolP320r1' of 320 bits ====

Random point :
  x = 0xb2b879b6be6afa4deac210f3b1c946051a30406e28c907cc3e7d7f1f6e613cf0bc5252af41bb3f65
  y = 0x51fd5164f1f48eb26c955334131135b6d930922602c553bb88ec6fd905c1ee5eb89c19815e6639b9
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x18c20549c41f7330eb7fa39083c613320d69366a5bb83b5f370101c15ee79f80a0f66f7dbb30addf
  y = 0x5d7d466d967d1d3a9266b6647033f5d921b4ce63f01f0df7fa6e0ace9d614fe254ba3e453b954504
Is in the curve? Yes

Added to itself :
  x = 0x18c20549c41f7330eb7fa39083c613320d69366a5bb83b5f370101c15ee79f80a0f66f7dbb30addf
  y = 0x5d7d466d967d1d3a9266b6647033f5d921b4ce63f01f0df7fa6e0ace9d614fe254ba3e453b954504
Is in the curve? Yes

Doubled point  :
  x = 0x18c20549c41f7330eb7fa39083c613320d69366a5bb83b5f370101c15ee79f80a0f66f7dbb30addf
  y = 0x5d7d466d967d1d3a9266b6647033f5d921b4ce63f01f0df7fa6e0ace9d614fe254ba3e453b954504
Is in the curve? Yes


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
dh1 lpri  = 0x31ed087c459af7c8704e3c500ff5b1fcef36876afdb90fba987904410794b26e4a1d9adeccd920c0bd3912c6d97d6ae3
dh1 lpubx = 0x683d07966ce3546f5ec3b576eafbdda2251ebd7434f30cc1c2b0aa39a00e6f7a4f76622cd2117805ba9d552353fbea5f
dh1 lpuby = 0x39d8ebe3115b1584246ae572d000d8e594ceaf560b76d75a0208e862e88df441da8bbf2c4801838b2d14e23ad0a31fe
dh2 lpri  = 0x833733c64b8473de4e6167a1423b4049800e601704bfb0f03c83f42b407445db6f69fac5166643b7a3eac04b9f4b7c95
dh2 lpubx = 0x3f595a8e27f9f0bcd7620b3fd1c8544c90406da5bd830496b3f32ce753ea2116bbbb6a87276554bf12a4376461a0dbed
dh2 lpuby = 0x7a768313ba40b561db1e5f8bf35cafafeeed580afd5b9d6866b123c9d175bf04afb9377e0a06058211a3bed6c52a8b4a

public 1  = 04683d07966ce3546f5ec3b576eafbdda2251ebd7434f30cc1c2b0aa39a00e6f7a4f76622cd2117805ba9d552353fbea5f039d8ebe3115b1584246ae572d000d8e594ceaf560b76d75a0208e862e88df441da8bbf2c4801838b2d14e23ad0a31fe
public 2  = 043f595a8e27f9f0bcd7620b3fd1c8544c90406da5bd830496b3f32ce753ea2116bbbb6a87276554bf12a4376461a0dbed7a768313ba40b561db1e5f8bf35cafafeeed580afd5b9d6866b123c9d175bf04afb9377e0a06058211a3bed6c52a8b4a

shared 1 = 71cb1aa9fc6ae4cf6f4ddb00731d2ac3b7018e5fb5c1785292bec6996f9afc6180c625fae66cdbab0190c0cf2f2e03b6
shared 2 = 71cb1aa9fc6ae4cf6f4ddb00731d2ac3b7018e5fb5c1785292bec6996f9afc6180c625fae66cdbab0190c0cf2f2e03b6

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP384r1 384 bits ---

PUB = 042a11c4fcb1a12b3ca10ba896190d20787b6f9812eb1cefbb42c4dc706c2bd76d66b6683f98e72abb3dab8cb3bd9afe0325e7469be1afa034883340f9f2168df268eb3627a36110255f3a8c7eaa643ee2e1881780184a7cf0d4db4c1e38f9a738
PRI = 0x618c78a8f495248aece03d11df93fe1cc73d3a8ef74e9bb6b147469b52ad79bedb6759625737b3a8e0447aac19d8c338
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP384r1'
Private Key    = 0x28b993a28644508c187e985a6697657127a2f928c1701a759f97baeba13bdee909e1c8a6d2dc7f285673be4dac499978
Public Key's X = 0x5250fce657ab9b2f99398b9b3a8c31677bdc64ee8654b57c19c37b30a2cd17b05017736f21ff263444d867f566a4dd60
Public Key's Y = 0x7e19ec6eaf78e5caf8c0ccf67ad705a2c0780b43f43c6c45296e9dba95a032d985275b0a7c4b11e391914b5e71e5e708

Public Key Bytes = 025250fce657ab9b2f99398b9b3a8c31677bdc64ee8654b57c19c37b30a2cd17b05017736f21ff263444d867f566a4dd60


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x5250fce657ab9b2f99398b9b3a8c31677bdc64ee8654b57c19c37b30a2cd17b05017736f21ff263444d867f566a4dd60
Public Key's Y = 0x7e19ec6eaf78e5caf8c0ccf67ad705a2c0780b43f43c6c45296e9dba95a032d985275b0a7c4b11e391914b5e71e5e708

Result is CORRECT


==== Random Point operation using Curve 'brainpoolP384r1' of 384 bits ====

Random point :
  x = 0x1e37b3c7752291a82ee59f1f3b377c60868145561c2fc46b2608720cc83f63b3f103c7288bf30291dcab7009dd54ce36
  y = 0x62c1eec7f37524b490f84978b2a1c477ffcc3286b6b3d5c1db45aae270107d5bb09bada03258cb81802c220762d835b4
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x3263d0eaaafada8b5603b5791915e3ebae42bcd064907e5838e1027b50c56248998c14b4e2e2e1e4a01c1e715b3afec1
  y = 0x4689c0e932c10f4d3d5f4bb70d078e53c86d4d1527e147099518c42d409381578d9d3ea08e2c03fde5ba0a33336f6612
Is in the curve? Yes

Added to itself :
  x = 0x3263d0eaaafada8b5603b5791915e3ebae42bcd064907e5838e1027b50c56248998c14b4e2e2e1e4a01c1e715b3afec1
  y = 0x4689c0e932c10f4d3d5f4bb70d078e53c86d4d1527e147099518c42d409381578d9d3ea08e2c03fde5ba0a33336f6612
Is in the curve? Yes

Doubled point  :
  x = 0x3263d0eaaafada8b5603b5791915e3ebae42bcd064907e5838e1027b50c56248998c14b4e2e2e1e4a01c1e715b3afec1
  y = 0x4689c0e932c10f4d3d5f4bb70d078e53c86d4d1527e147099518c42d409381578d9d3ea08e2c03fde5ba0a33336f6612
Is in the curve? Yes


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
dh1 lpri  = 0x9bada06dcfa086a93342299efd58909dd25bc2fc3c0b2b4999964b0845fe3a3e06ac9a548842fb45baeb15b9b489e2a6b56ddb3740de3d4fe5f222258689bede
dh1 lpubx = 0xa7a818954bb9ab0ba5d0e5ce20ebcad818a50cf30bf59c9b9ca2fb89023555085bcd2fc6785b245f72af0513fe68ec4ed6b506623403eddf73699b6008774c6c
dh1 lpuby = 0x41ef0602018ce537b4beb3f8b0e3a7d2167ed8db442e1c1989ff9278d14b7d3a0e5e5eea121b40c354cfd92b71e4c400fc8666c30861d840444582209a1dde91
dh2 lpri  = 0x8cea8a48ffbe6f2d38eee07d2d8a43f2bedacd0204bfc55bf2b2f9106cb5f25aec5ba5648d3b447c75f85a4d249878f27df456b45f5142ae0ca632ad530db419
dh2 lpubx = 0x8c635797b575b6d7ba858b9bba5bf75b0dc80ba18f350fe73fbd27e538909d545d50aac53fae5709fb51af69967a989d878e8bf49ecf15793c01cfbe1c0f54d2
dh2 lpuby = 0x358901ee27f2d0e3624bb12b555283862be32d269e1ae20fc258d4b0df3c2e7abca6c924979189074154d427a77b5e5a47faf3af45892cd336f8cbee97da257f

public 1  = 04a7a818954bb9ab0ba5d0e5ce20ebcad818a50cf30bf59c9b9ca2fb89023555085bcd2fc6785b245f72af0513fe68ec4ed6b506623403eddf73699b6008774c6c41ef0602018ce537b4beb3f8b0e3a7d2167ed8db442e1c1989ff9278d14b7d3a0e5e5eea121b40c354cfd92b71e4c400fc8666c30861d840444582209a1dde91
public 2  = 048c635797b575b6d7ba858b9bba5bf75b0dc80ba18f350fe73fbd27e538909d545d50aac53fae5709fb51af69967a989d878e8bf49ecf15793c01cfbe1c0f54d2358901ee27f2d0e3624bb12b555283862be32d269e1ae20fc258d4b0df3c2e7abca6c924979189074154d427a77b5e5a47faf3af45892cd336f8cbee97da257f

shared 1 = 0bb063230052f75116b9989110b143ab44ae40ba6e85153bac8d4e34df02b3639f34d033807ee04c843244ca6686cc017dab08b6a536f1ed5ba53caea992f687
shared 2 = 0bb063230052f75116b9989110b143ab44ae40ba6e85153bac8d4e34df02b3639f34d033807ee04c843244ca6686cc017dab08b6a536f1ed5ba53caea992f687

Result is CORRECT



Generating keys...

--- ECC KEY brainpoolP512r1 512 bits ---

PUB = 04aa22bd9ea6ea053ef8d9a2db455900df12379f3f365e9b1246e920b4a8a93fe0b2d18ceabeb6dde3be2c26e91ed2e80d1d3903a2ef1663dd043a44754819e56c260170954d1e2cbe39bf23f0c9c4dae4cecc3146fcde299b8d4b5255d44ee9a7fe8034ba80864e742c3092ff44df3521c6bb0f5b83580f24cfc19d08eb51ba55
PRI = 0x1f46d169fb24d13fbadd146b00cf3eab79f1d431ff5bbe35427acf7dca75c1af138fff83061ef52a08236e68ba439d95a97a99d2b20966d67a9392f9f9c05864
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'brainpoolP512r1'
Private Key    = 0xa6f58de5d438bc5ef67176feb03f20c868669ee5f7b6ad4a74d6290e3f39d3b11c0c5155f9fbaf14f98311aab417f402a900b700706540ca183a4264a14feb57
Public Key's X = 0x3ba24f474e42a2c7a5d8571195f173fde8d79326ee9cdf91ea6e8e92125f3943a50cbe776adb146f2d379568988011bf796af52b774ef96da602ad3a051fda8c
Public Key's Y = 0x1ab72738d47da101d7e8bf41d7eee75a8225e31fe2e03e4dbf77c17152cd65c561753678c7e0aa5ed22ccef7b0ded5f2329009c6758c95fa3a019d2841fef85b

Public Key Bytes = 033ba24f474e42a2c7a5d8571195f173fde8d79326ee9cdf91ea6e8e92125f3943a50cbe776adb146f2d379568988011bf796af52b774ef96da602ad3a051fda8c


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x3ba24f474e42a2c7a5d8571195f173fde8d79326ee9cdf91ea6e8e92125f3943a50cbe776adb146f2d379568988011bf796af52b774ef96da602ad3a051fda8c
Public Key's Y = 0x1ab72738d47da101d7e8bf41d7eee75a8225e31fe2e03e4dbf77c17152cd65c561753678c7e0aa5ed22ccef7b0ded5f2329009c6758c95fa3a019d2841fef85b

Result is CORRECT


==== Random Point operation using Curve 'brainpoolP512r1' of 512 bits ====

Random point :
  x = 0x2b2ce3f56969b493e1cb3b3f6ef7f4d65df9566eda270b7f22c71323bf0abd65a481dd965948b05d94052886c52639a645091d1e1246d2216150cad03aaf936e
  y = 0x894b3d45dde70b8863acb86c06c6d86a617190e59d6d2cf7aabf578e468be48d3885151032eec9ebb5efe78ef2eeebfa89ad5aeaa30127759f526c6742b27d40
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x612984f2e0945f68d689bcf992161442165ed470584ee8c89c6e52a9b65b4504f06c80916214a9ec518952b5656c5e420095fc19ecf9a0640a6be1b5437df4f0
  y = 0x1d66d98fe968cb8a285c63585ce407ff39b6e09156446c860970a4986d1b161e38f88b891b174c14709fe4e0ed31d142aad52a89331ca74c5c6bf26de0ccff4c
Is in the curve? Yes

Added to itself :
  x = 0x612984f2e0945f68d689bcf992161442165ed470584ee8c89c6e52a9b65b4504f06c80916214a9ec518952b5656c5e420095fc19ecf9a0640a6be1b5437df4f0
  y = 0x1d66d98fe968cb8a285c63585ce407ff39b6e09156446c860970a4986d1b161e38f88b891b174c14709fe4e0ed31d142aad52a89331ca74c5c6bf26de0ccff4c
Is in the curve? Yes

Doubled point  :
  x = 0x612984f2e0945f68d689bcf992161442165ed470584ee8c89c6e52a9b65b4504f06c80916214a9ec518952b5656c5e420095fc19ecf9a0640a6be1b5437df4f0
  y = 0x1d66d98fe968cb8a285c63585ce407ff39b6e09156446c860970a4986d1b161e38f88b891b174c14709fe4e0ed31d142aad52a89331ca74c5c6bf26de0ccff4c
Is in the curve? Yes


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
dh1 lpri  = 0x377d8649ade49e93854595d8b3ca81b549ea3abe69db2d213fde31e39a03878
dh1 lpubx = 0x4c6aa81c83aaf3efb7b75c283a26be8ec3af648a8afa35fea3209bc23ff57a5a
dh1 lpuby = 0x0
dh2 lpri  = 0x7e7ef591476f287d96d60cca7ad02f6862cd65256039c7a70d7982ffd7e7950
dh2 lpubx = 0x146e7aa7fc59ed050f7c6e1aff4870088842a87792ebd152c4582d6d6af18871
dh2 lpuby = 0x0

public 1  = 4c6aa81c83aaf3efb7b75c283a26be8ec3af648a8afa35fea3209bc23ff57a5a
public 2  = 146e7aa7fc59ed050f7c6e1aff4870088842a87792ebd152c4582d6d6af18871

shared 1 = 572789f322cec870521b7a5995e540cb203df44875f79315faa74e30b527316b
shared 2 = 572789f322cec870521b7a5995e540cb203df44875f79315faa74e30b527316b

Result is CORRECT



Generating keys...

--- ECC KEY curve25519 256 bits ---

PUB = 204f31d3f7072650d9f70c262fa6321b5030ff884c5d081ed696e4922071980e
PRI = 0x839573b5210cea0902315f1182b7accb575a7bd6fe73ab9d4d9e6f0f1fdc388
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'curve25519'
Private Key    = 0xae20982ead0aee24af1ad8fa81289900347db4f29893f672028f5429c604548
Public Key's X = 0x22531783cd4c1422d8823da8a9f2604b501efcd1351cc9c23ca6dc3df39b8148
Public Key's Y = 0x0

Public Key Bytes = 22531783cd4c1422d8823da8a9f2604b501efcd1351cc9c23ca6dc3df39b8148


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x22531783cd4c1422d8823da8a9f2604b501efcd1351cc9c23ca6dc3df39b8148
Public Key's Y = 0x0

Result is CORRECT


==== Random Point operation using Curve 'curve25519' of 256 bits ====

Random point :
  x = 0xbf11ea7ad27fd1427a7197f38a30df4b621ea175aac976dacc7c227ad2e4100
  y = 0x0
Is in the curve? Yes

Mulitplied by 2 :
  x = 0x125190992594a079d2103b53ec8317193e808a4e082447a4e453363169af4401
  y = 0x0
Is in the curve? Yes

Added to itself :
  x = 0x0
  y = 0x0
Is in the curve? Yes

Doubled point  :
  x = 0x0
  y = 0x0
Is in the curve? Yes


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
dh1 lpri  = 0x4f13c0ae4c8d13be4a79afe0132e6edace24ace2c707fe08058c906f200d74503b63900d553ee1d63ac583d38150366a92954228d7872b4
dh1 lpubx = 0xa4e457d636b2781850f9e00061492de4ae22d3b3d382950b8c08a6c858d0adceaa48d11a32c7b84068e119b019e3da032c811bdf0646a7a
dh1 lpuby = 0x0
dh2 lpri  = 0xd3e82022a5055988ed46285b1ebe8bf0f7cd2f0e2eed415da6977ccb9fdade120498f822ad53cc2e9f631508d71fcc62d11d3daaaf4a14
dh2 lpubx = 0x7e2474f80311a9e1e9af876a79ad16716fa270e289cb8f092aad6611dfb77940900f1583ce9d00d5fd3fe7221495a8d0de5eed97f1292f43
dh2 lpuby = 0x0

public 1  = 0a4e457d636b2781850f9e00061492de4ae22d3b3d382950b8c08a6c858d0adceaa48d11a32c7b84068e119b019e3da032c811bdf0646a7a
public 2  = 7e2474f80311a9e1e9af876a79ad16716fa270e289cb8f092aad6611dfb77940900f1583ce9d00d5fd3fe7221495a8d0de5eed97f1292f43

shared 1 = 697946e11d4b5e604a5b4abaf2bc944c10ddc9af9ec271ea7d7b3881d33679a94895f624e2c3aec13cc64a1b4f52374a31d8491fd19febe2
shared 2 = 697946e11d4b5e604a5b4abaf2bc944c10ddc9af9ec271ea7d7b3881d33679a94895f624e2c3aec13cc64a1b4f52374a31d8491fd19febe2

Result is CORRECT



Generating keys...

--- ECC KEY curve448 448 bits ---

PUB = 6ca8d1f45073371c1c3eacbbdac1c487c39ffbd3df28bdb4d9d8e7cdefe87bb922e0ee6f56de9d7f87714bc6057ea75ed4cd49667b3f1904
PRI = 0x12728bb7979d4dd655060c912a204c2e796fa5d492471c294908a82a4f2805a93b06d4a68f172a16f8247392067b97c9ff5e33e3d070a878
-----------------------------

===== Test Point Compresion =====

 Generating Key..
Using curve 'curve448'
Private Key    = 0x2173b747ae146ef4607cccf49bf21ea90cc7348a703fa5efae12a5d34adb85a0442b981d9d5ef1d91b1efe2da5e55645b9320ffdd8295790
Public Key's X = 0x2c37a54e50126087e6bbed8b3f94999543f130c6f5dfc5589f5104ee4724e4050ee4e7e7df7684b10dbdead9834555eda65e93cf9de91d4f
Public Key's Y = 0x0

Public Key Bytes = 2c37a54e50126087e6bbed8b3f94999543f130c6f5dfc5589f5104ee4724e4050ee4e7e7df7684b10dbdead9834555eda65e93cf9de91d4f


Generating Public Key...
Private Key    = 0xNULL
Public Key's X = 0x2c37a54e50126087e6bbed8b3f94999543f130c6f5dfc5589f5104ee4724e4050ee4e7e7df7684b10dbdead9834555eda65e93cf9de91d4f
Public Key's Y = 0x0

Result is CORRECT


==== Random Point operation using Curve 'curve448' of 448 bits ====

Random point :
  x = 0x5aa6b12c464cee98d846d8df9b431aa50cf3efcb84152f6d6d1e9e73b53389a42b50c8da3d105e12b105ca16614a2eff37bd8a5314c540c
  y = 0x0
Is in the curve? Yes

Mulitplied by 2 :
  x = 0xab3b11696008a028b8020d3ef76c558076c346735b192e086654adabef3e12666003ea262b07d58f5d466f445412b3aa33dc5fa69fa638bd
  y = 0x0
Is in the curve? Yes

Added to itself :
  x = 0x0
  y = 0x0
Is in the curve? Yes

Doubled point  :
  x = 0x0
  y = 0x0
Is in the curve? Yes



 TEST ECC _END
===== TEST ECDSA =====
Generating key..
Gen pri = 0xba9f8f2e3d66770b06c819f60bd48638688ec8836b5d64c310c5cb93ab15fa57d3d0ae526f3175e533a5ebce5460927f
Gen pub :
  x = 0x9569b5453b2d9493c8b8f2f2952783cfb874fb7079a266c47c3817f714bfa5c70234870b650a85f79dbf7b8a1413736d
  y = 0x9b9f79bd0a50c19bc9a647c8bc3916e17135574faa28dc8746fdaeb8075a6193fa35056b44d459a7ab197a8b2f60ed6d
Saving private key
DONE IT
Signing...
OK, saving...
Saved Correctly
Readign signature...
Got 0 verifying signature
===== TEST EdDSA 25519 =====
Generating key..
Gen pri = 0x38cadf4ef1dfbec55555b43b8a790e3ad9a350952a06223307a696a07322d30
Gen pub :
  x = 0xdb88eee949a8d976503f5d6472cf67010fbd7b42a1fab03cde5dc4038d333d7e
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
Gen pri = 0x944a849aeb01ed9a81290ef8875d278da13011f6f47d25b282a93da95a3d4af6e7974ab3cbdcef0e0e97bde5264a8fd0bfb3ce6ca6a3e4
Gen pub :
  x = 0x3123db0e4574fefa92da141c5df159348da4b93017f7492ca8117e47c506ce71d492322501ff9b6f6e5d8afa7d3f26d9dae745a26d081d51
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




