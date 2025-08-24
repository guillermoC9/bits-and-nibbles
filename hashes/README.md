# Hashes and HMACs
## Criptographic Hashing

I personally think that hashes are the second most useful thing ever brought by computers, being the first one the spreadsheet. 

Hashes have one main function: **The identificaction of information**, and from there derive many other applications. They work by compressing information into a -hopefully- unique hash code that identifies the compressed data and can be used for many things: sorting and searching, data integrity, tampering or corruption detection, provenance, digital fingerprints, signatures and many other. But the two main uses are: **clasification** and **protection** of information.

The clasification of information was the first use of hashes, which was invented to index the information in files and tables, known as *hash tables*. From there, criptographers created the second version of them: criptographic hashes, as they saw then as a way to protect information. It is important to understand the differences on the way to use both hashes, as what are big problems on criptographic hashes: collisions and preimage, are actually the reasons to use indexing hashes. 

On hash tables, collisions are handled naturally, because it is understood that they will appear often, as these hashes tend to be small and also are usually reduced even more by modular arithmetics to fit into manageable tables. On criptographic hashes collisions are **very dangerous**, as will allow someone to impersonate forged data as genuine information -e.g. forge the fingerprint of a digital certificate-. 
The preimage problem arises from the possibility to know the original information that generated the hash, which means a way to know that hash ```123``` was generated from the string ```I am a hash```. That is a huge problem and extremely dangerous in criptography, but is the intended use for indexing hashes: to find the original information. 

Criptographic hashes are extremely complex to create, as they need to be not only intrinsic -the result is always the same when giving the same input-, but also non-commutative -```a + b``` is not the same as ```b + a```-. This ensures that the information they represent has not been changed and have been fed in the same order. One typical use would be a control version, where I hash the first version of a text. Then add the new modifications to the previous hash to obtain a new one. Since then, by hahsing the new document and comparing the hashes I can determine not only if the new document is legit, but also if **it is** the result of the new addtions to the previous document, which gives a better protection against collision attacks.

That is one of the uses of criptographic hashes that are not widespread, as neither one where you take a secret ```S```, hash it -lets say- a 100 times in a row to obtain ```H100``` -basically doing ```HASH(HASH(...HASH(S)))```-, publish it and then call it *signature*. I can now sign a piece of info ```X``` by just hashing it with a previous version of the hash like ```XH = HASH(H99,X)``` and publish ```XH, H99``` and  the info. Then someone can verify the signature by hasing the info with ```H99``` to verify is ```XH```, and then hash ```H99``` once and check that it becomes ```H100```. The only drawback is that you need to make sure everybody knows that ```H99``` has been published so it cannot be used to impersonate you by signing something with ```H99``` and claiming it comes from you. 

There are many criptographic hash primitives around, with some used to great sucess like ```SHA256```, and some with less like ```RIPE160```. Some broken like ```MD4```, some presumibly broken like ```MD5```, and some definitively not broken like ```SHA512```. My advise is that you should use whatever is best for the case, taking into account the risks. 

For example, it is widely advised not to use ```MD5``` because collissions have been created to forge things, and some preimage attacks are confirmed feasible -although not proven certain-. But you will find that MD5 is still widely used to hash files in file servers to avoid uploading the same file twice. This is not dangerous to do, as the only pausible problem with this -a collision-, can be easily handled by comparing the size of the file, as if two files of different size generates the same hash, it will be a collision and can be handled.

The industry tend to 'forget' broken or pressumibly broken algorithms, but I think it is good to have them around for research. For example to study the integrity of old documents like certificates X509. Here I present a set of functions that  manage a good array of criptographic hash primitives all in the same way as they will be used separately: ```Init, update as many times as needed and finalize to obtain the hash```. All you need to do is choose the primitive on initialization time with needed parameters.

The list of supported hash primitives is: MD2, MD4, MD5, SHA1, SHA2, SHA2, RIPE128, RIPE160, RIPE256, RIPE320 and POLY1305.

Also included here is the same mechanism to create HMACs with any of the same primitives. HMAC means **H**ash-based **M**essage **A**uthentication **C**ode, and as its name indicates, it is a code obtained by hashing a message together with some key, which allows not only to know that the message has not been tampered with, but also that it comes from whoever has set the key. Protocols like ```TLS``` use HMACs to operate. 

Just to let you know, the primitives: ```Poly1305```, ```Shake128``` and ```Shake256``` are not a pure hash primitives, with the former being a HMAC algorithm, and the latter **H**ash-based **K**ey **D**erivation **F**unctions or HKDF. 

The HMAC protocol is defined by RFC-2104, RFC-6234 and RFC-4868.

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

$ ./test_hashes

== HASHES TEST START 

**** Test MD2 ****

Contens : ""
Result=8350e5a3e24c153df2275c9f80692773
MustBe=8350e5a3e24c153df2275c9f80692773
Contens : The quick brown fox jumps over the lazy dog
Result=03d85a0d629d2c442e987525319fc471
MustBe=03d85a0d629d2c442e987525319fc471
Contens : ""
Result=8350e5a3e24c153df2275c9f80692773
MustBe=8350e5a3e24c153df2275c9f80692773
Contens : The quick brown fox jumps over the lazy dog
Result=03d85a0d629d2c442e987525319fc471
MustBe=03d85a0d629d2c442e987525319fc471

**** Test MD4 ****

Contens : ""
Result=31d6cfe0d16ae931b73c59d7e0c089c0
MustBe=31d6cfe0d16ae931b73c59d7e0c089c0
Contens : The quick brown fox jumps over the lazy dog
Result=1bee69a46ba811185c194762abaeae90
MustBe=1bee69a46ba811185c194762abaeae90
Contens : ""
Result=31d6cfe0d16ae931b73c59d7e0c089c0
MustBe=31d6cfe0d16ae931b73c59d7e0c089c0
Contens : The quick brown fox jumps over the lazy dog
Result=1bee69a46ba811185c194762abaeae90
MustBe=1bee69a46ba811185c194762abaeae90
Contens : a
Result=bde52cb31de33e46245e05fbdbd6fb24
MustBe=bde52cb31de33e46245e05fbdbd6fb24
Contens : abc
Result=a448017aaf21d8525fc10ae87aa6729d
MustBe=a448017aaf21d8525fc10ae87aa6729d
Contens : message digest
Result=d9130a8164549fe818874806e1c7014b
MustBe=d9130a8164549fe818874806e1c7014b

**** Test MD5 ****

Contens=""
Result =d41d8cd98f00b204e9800998ecf8427e
Must Be=d41d8cd98f00b204e9800998ecf8427e

Contens=The quick brown fox jumps over the lazy dog
Result =9e107d9d372bb6826bd81d3542a419d6
Must Be=9e107d9d372bb6826bd81d3542a419d6

Contens=""
Result =d41d8cd98f00b204e9800998ecf8427e
Must Be=d41d8cd98f00b204e9800998ecf8427e

Contens=The quick brown fox jumps over the lazy dog
Result =9e107d9d372bb6826bd81d3542a419d6
Must Be=9e107d9d372bb6826bd81d3542a419d6

HMAC-MD5=74e6f7298a9c2d168935f58c001bad88
Must Be =74e6f7298a9c2d168935f58c001bad88

HMAC-MD5=80070713463e7749b90c2dc24911e275
Must Be =80070713463e7749b90c2dc24911e275

**** Test SHA-1 ****

Contens=""
Result =da39a3ee5e6b4b0d3255bfef95601890afd80709
Must Be=da39a3ee5e6b4b0d3255bfef95601890afd80709

Contens=The quick brown fox jumps over the lazy dog
Result =2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
Must Be=2fd4e1c67a2d28fced849ee1bb76e7391b93eb12

Contens=""
Result =da39a3ee5e6b4b0d3255bfef95601890afd80709
Must Be=da39a3ee5e6b4b0d3255bfef95601890afd80709

Contens=The quick brown fox jumps over the lazy dog
Result =2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
Must Be=2fd4e1c67a2d28fced849ee1bb76e7391b93eb12

HMAC-SHA1=fbdb1d1b18aa6c08324b7d64b71fb76370690e1d
Must Be  =fbdb1d1b18aa6c08324b7d64b71fb76370690e1d

HMAC-SHA1=de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9
Must Be  =de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9

HMAC-SHA1=b617318655057264e28bc0b6fb378c8ef146be00
Must Be  =b617318655057264e28bc0b6fb378c8ef146be00

HMAC-SHA1=aa4ae5e15272d00e95705637ce8a3b55ed402112
Must Be  =aa4ae5e15272d00e95705637ce8a3b55ed402112

**** Test SHA-224 ****

Contens=""
Result =d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f
Must Be=d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f

Contens=The quick brown fox jumps over the lazy dog
Result =730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525
Must Be=730e109bd7a8a32b1cb9d9a09aa2325d2430587ddbc0c38bad911525

Contens=abc
Result =23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7
Must Be=23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7

Key    =''
Contens=''
HMAC-SHA224=5ce14f72894662213e2748d2a6ba234b74263910cedde2f5a9271524
Must Be    =5ce14f72894662213e2748d2a6ba234b74263910cedde2f5a9271524

Key    =key
Contens=The quick brown fox jumps over the lazy dog
HMAC-SHA224=88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69
Must Be    =88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69

Key    =Jefe
Contens=what do ya want for nothing?
HMAC-SHA224=a30e01098bc6dbbf45690f3a7e9e6d0f8bbea2a39e6148008fd05e44
Must Be    =a30e01098bc6dbbf45690f3a7e9e6d0f8bbea2a39e6148008fd05e44

**** Test SHA-256 ****

Contens=""
Result =e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
Must Be=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855

Contens=abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
Result =248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1
Must Be=248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1

Contens=abc
Result =ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
Must Be=ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad

Key    =''
Contens=''
HMAC-SHA256=b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad
Must Be    =b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad

Key    =key
Contens=The quick brown fox jumps over the lazy dog
HMAC-SHA256=f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
Must Be    =f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8

Key    =Jefe
Contens=what do ya want for nothing?
HMAC-SHA256=5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843
Must Be    =5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843

**** Test SHA-384 ****

Contens=""
Result =38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b
Must Be=38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b

Contens=abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
Result =3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05abfe8f450de5f36bc6b0455a8520bc4e6f5fe95b1fe3c8452b
Must Be=3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05abfe8f450de5f36bC6b0455a8520bc4e6f5fe95b1fe3c8452b

Contens=abc
Result =cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7
Must Be=cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7

Contens    =''
HMAC-SHA384=6c1f2ee938fad2e24bd91298474382ca218c75db3d83e114b3d4367776d14d3551289e75e8209cd4b792302840234adc
Must Be    =6c1f2ee938fad2e24bd91298474382ca218c75db3d83e114b3d4367776d14d3551289e75e8209cd4b792302840234adc

Key    =key
Contens=The quick brown fox jumps over the lazy dog
HMAC-SHA384=d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237
Must Be    =d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237

Key    =Jefe
Contens=what do ya want for nothing?
HMAC-SHA384=af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649
Must Be    =af45d2e376484031617f78d2b58a6b1b9c7ef464f5a01b47e42ec3736322445e8e2240ca5e69e2c78b3239ecfab21649

**** Test SHA-512 ****

Contens=""
Result =cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e
Must Be=cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e

Contens=abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
Result =204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445
Must Be=204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c33596fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445

Contens=abc
Result =ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f
Must Be=ddaf35a193617abacc417349ae20413112e6Fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f

Key    =''
Contens=''
HMAC-SHA512=b936cee86c9f87aa5d3c6f2e84cb5a4239a5fe50480a6ec66b70ab5b1f4ac6730c6c515421b327ec1d69402e53dfb49ad7381eb067b338fd7b0cb22247225d47
Must Be    =b936cee86c9f87aa5d3c6f2e84cb5a4239a5fe50480a6ec66b70ab5b1f4ac6730c6c515421b327ec1d69402e53dfb49ad7381eb067b338fd7b0cb22247225d47

Key    =key
Contens=The quick brown fox jumps over the lazy dog
HMAC-SHA512=b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a
Must Be    =b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a

Key    =Jefe
Contens=what do ya want for nothing?
HMAC-SHA512=164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737
Must Be    =164b7a7bfcf819e2e395fbe73b56e0a387bd64222e831fd610270cd7ea2505549758bf75c05a994a6d034f65f8f0e6fdcaeab1a34d4a6b4b636e070a38bce737

**** Test SHA-512_224 ****

Contens=""
Result =6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4
Must Be=6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4

Contens=abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
Result =e5302d6d54bb242275d1e7622d68df6eb02dedd13f564c13dbda2174
Must Be=e5302d6d54bb242275d1e7622d68df6eb02dedd13f564c13dbda2174

Contens=abc
Result =4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa
Must Be=4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa

Key    =''
Contens=''
HMAC-SHA512_224=62c5386083df6fd3ca009c94c0086b7728f055787791429e1ae6e007
Must Be        =62c5386083df6fd3ca009c94c0086b7728f055787791429e1ae6e007

Key    =key
Contens=The quick brown fox jumps over the lazy dog
HMAC-SHA512_224=f797c5aa83c39f686c5892e15bfac4a4b5421c4c8a12fb34a6701c10
Must Be        =f797c5aa83c39f686c5892e15bfac4a4b5421c4c8a12fb34a6701c10

Key    =Jefe
Contens=what do ya want for nothing?
HMAC-SHA512_224=a186461f12da6cb8713dd8d41fddfc62c39c219a5a3f550552c17a07
Must Be        =a186461f12da6cb8713dd8d41fddfc62c39c219a5a3f550552c17a07

**** Test SHA-512_256 ****

Contens=""
Result =c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a
Must Be=c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a

Contens=abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq
Result =bde8e1f9f19bb9fd3406c90ec6bc47bd36d8ada9f11880dbc8a22a7078b6a461
Must Be=bde8e1f9f19bb9fd3406c90ec6bc47bd36d8ada9f11880dbc8a22a7078b6a461

Contens=abc
Result =53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23
Must Be=53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23

Key    =''
Contens=''
HMAC-SHA512_256=3c800ad43216d695e18251795743ef81c446a4ca4f38f48a948b887201ca16f5
Must Be        =3c800ad43216d695e18251795743ef81c446a4ca4f38f48a948b887201ca16f5

Key    =key
Contens=The quick brown fox jumps over the lazy dog
HMAC-SHA512_256=832e660f415c6b80b144b149f9698cc6dd226d85e8cf2deb8a379be7e22aa824
Must Be        =832e660f415c6b80b144b149f9698cc6dd226d85e8cf2deb8a379be7e22aa824

Key    =Jefe
Contens=what do ya want for nothing?
HMAC-SHA512_256=a544d591d4b71b34398f5be4fd1c0e408df97a2cbcf1546e13498785e8f2a1dd
Must Be        =a544d591d4b71b34398f5be4fd1c0e408df97a2cbcf1546e13498785e8f2a1dd

**** Test SHA3-224 ****

Contens=""
Result =6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7
Must Be=6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7

Contens=The quick brown fox jumps over the lazy dog
Result =d15dadceaa4d5d7bb3b48f446421d542e08ad8887305e28d58335795
Must Be=d15dadceaa4d5d7bb3b48f446421d542e08ad8887305e28d58335795

Contens=''
Key    ='key'
HMAC-SHA3-224=8f481e10aa1ab054f9862d9b2c2ec2be515ec8355e60c452eff83efc
Must Be      =8f481e10aa1ab054f9862d9b2c2ec2be515ec8355e60c452eff83efc

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-SHA3-224=ff6fa8447ce10fb1efdccfe62caf8b640fe46c4fb1007912bf85100f
Must Be      =ff6fa8447ce10fb1efdccfe62caf8b640fe46c4fb1007912bf85100f

**** Test SHA3-256 ****

Contens=""
Result =a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a
Must Be=a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a

Contens=The quick brown fox jumps over the lazy dog
Result =69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04
Must Be=69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-SHA3-256=8c6e0683409427f8931711b10ca92a506eb1fafa48fadd66d76126f47ac2c333
Must Be      =8c6e0683409427f8931711b10ca92a506eb1fafa48fadd66d76126f47ac2c333

**** Test SHA3-384 ****

Contens=""
Result =0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004
Must Be=0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004

Contens=The quick brown fox jumps over the lazy dog
Result =7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41
Must Be=7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-SHA3-384=aa739ad9fcdf9be4a04f06680ade7a1bd1e01a0af64accb04366234cf9f6934a0f8589772f857681fcde8acc256091a2
Must Be      =aa739ad9fcdf9be4a04f06680ade7a1bd1e01a0af64accb04366234cf9f6934a0f8589772f857681fcde8acc256091a2

**** Test SHA3-512 ****

Contens=""
Result =a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26
Must Be=a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26

Contens=The quick brown fox jumps over the lazy dog
Result =01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450
Must Be=01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-SHA3-512=237a35049c40b3ef5ddd960b3dc893d8284953b9a4756611b1b61bffcf53edd979f93547db714b06ef0a692062c609b70208ab8d4a280ceee40ed8100f293063
Must Be      =237a35049c40b3ef5ddd960b3dc893d8284953b9a4756611b1b61bffcf53edd979f93547db714b06ef0a692062c609b70208ab8d4a280ceee40ed8100f293063

**** Test KECCAK-224 ****

Contens=""
Result =f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd
Must Be=f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd

Contens=The quick brown fox jumps over the lazy dog
Result =310aee6b30c47350576ac2873fa89fd190cdc488442f3ef654cf23fe
Must Be=310aee6b30c47350576ac2873fa89fd190cdc488442f3ef654cf23fe

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-KECCAK-224=763e70a1ec866fbc1c6e6c398cd6e2383e2ad3aecbb3d6150f1e56fd
Must Be        =763e70a1ec866fbc1c6e6c398cd6e2383e2ad3aecbb3d6150f1e56fd

**** Test KECCAK-256 ****

Contens=""
Result =c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470
Must Be=c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470

Contens=The quick brown fox jumps over the lazy dog
Result =4d741b6f1eb29cb2a9b9911c82f56fa8d73b04959d3d9d222895df6c0b28aa15
Must Be=4d741b6f1eb29cb2a9b9911c82f56fa8d73b04959d3d9d222895df6c0b28aa15

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-KECCAK-256=74547bc8c8e1ef02aec834ca60ff24cc316d4c2244a360fe17448cb53410bed4
Must Be        =74547bc8c8e1ef02aec834ca60ff24cc316d4c2244a360fe17448cb53410bed4

**** Test KECCAK-384 ****

Contens=""
Result =2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff
Must Be=2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff

Contens=The quick brown fox jumps over the lazy dog
Result =283990fa9d5fb731d786c5bbee94ea4db4910f18c62c03d173fc0a5e494422e8a0b3da7574dae7fa0baf005e504063b3
Must Be=283990fa9d5fb731d786c5bbee94ea4db4910f18c62c03d173fc0a5e494422e8a0b3da7574dae7fa0baf005e504063b3

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-KECCAK-384=73acb07b5b1db5431758262b55e5923d362de4492229a7420302c80d4348ca1b11ecea06fb1c232f9b832aadca8cd289
Must Be        =73acb07b5b1db5431758262b55e5923d362de4492229a7420302c80d4348ca1b11ecea06fb1c232f9b832aadca8cd289

**** Test KECCAK-512 ****

Contens=""
Result =0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e
Must Be=0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e

Contens=The quick brown fox jumps over the lazy dog
Result =d135bb84d0439dbac432247ee573a23ea7d3c9deb2a968eb31d47c4fb45f1ef4422d6c531b5b9bd6f449ebcc449ea94d0a8f05f62130fda612da53c79659f609
Must Be=d135bb84d0439dbac432247ee573a23ea7d3c9deb2a968eb31d47c4fb45f1ef4422d6c531b5b9bd6f449ebcc449ea94d0a8f05f62130fda612da53c79659f609

Contens='The quick brown fox jumps over the lazy dog'
Key    ='key'
HMAC-KECCAK-512=22fb03b3391bc0adfc73c18e0919d9f142390e81d6cc2689716ac53ab75458a718059d58cfbb23c6a416c32b8afa84a9a7a9d852312a743bef0a55148e5a1b8a
Must Be        =22fb03b3391bc0adfc73c18e0919d9f142390e81d6cc2689716ac53ab75458a718059d58cfbb23c6a416c32b8afa84a9a7a9d852312a743bef0a55148e5a1b8a

**** Test HMAC SHA3-256 ****

Contens="Sample message for keylen<blocklen"
Result =4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205
Must Be=4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205

Contens="Sample message for keylen<blocklen"
Result =4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205
Must Be=4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205

**** Test HMAC SHA3-512 ****

Contens="Sample message for keylen<blocklen"
Result =4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196
Must Be=4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196

Contens="Sample message for keylen<blocklen"
Result =4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196
Must Be=4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196

**** Test SHAKE 128 ****

Contens=""
Result =7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26
Must Be=7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26

**** Test SHAKE 256 ****

Contens=""
Result =46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4ddd8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be
Must Be=46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4ddd8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be

**** Test SHAKE 128 ****

Contens="The quick brown fox jumps over the lazy dog"
Result =f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e
Must Be=f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e

**** Test SHAKE 128 ****

Contens="The quick brown fox jumps over the lazy dof"
Result =853f4538be0db9621a6cea659a06c1107b1f83f02b13d18297bd39d7411cf10c
Must Be=853f4538be0db9621a6cea659a06c1107b1f83f02b13d18297bd39d7411cf10c

**** Test RipeMD 128 ****

Contens="a"
Result =86be7afa339d0fc7cfc785e72f578d33
Must Be=86be7afa339d0fc7cfc785e72f578d33

Contens=""
Result =cdf26213a150dc3ecb610f18f6b38b46
Must Be=cdf26213a150dc3ecb610f18f6b38b46

HMAC-RIPE128=fbf61f9492aa4bbf81c172e84e0734db
Must Be     =fbf61f9492aa4bbf81c172e84e0734db

HMAC-RIPE128=5c6bec96793e16d40690c237635f30c5
Must Be     =5c6bec96793e16d40690c237635f30c5

**** Test RipeMD 160 ****

Contens=The quick brown fox jumps over the lazy dog
Result =37f332f68db77bd9d7edd4969571ad671cf9dd3b
Must Be=37f332f68db77bd9d7edd4969571ad671cf9dd3b

Contens=""
Result =9c1185a5c5e9fc54612808977ee8f548b2258d31
Must Be=9c1185a5c5e9fc54612808977ee8f548b2258d31

HMAC-RIPE160=24cb4bd67d20fc1a5d2ed7732dcc39377f0a5668
Must Be     =24cb4bd67d20fc1a5d2ed7732dcc39377f0a5668

HMAC-RIPE160=6466ca07ac5eac29e1bd523e5ada7605b791fd8b
Must Be     =6466ca07ac5eac29e1bd523e5ada7605b791fd8b

**** Test RipeMD 256 ****

Contens=The quick brown fox jumps over the lazy dog
Result =c3b0c2f764ac6d576a6c430fb61a6f2255b4fa833e094b1ba8c1e29b6353036f
Must Be=c3b0c2f764ac6d576a6c430fb61a6f2255b4fa833e094b1ba8c1e29b6353036f

Contens=""
Result =02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d
Must Be=02ba4c4e5f8ecd1877fc52d64d30e37a2d9774fb1e5d026380ae0168e3c5522d

HMAC-RIPE256=919482ec55136475ff25071750a6bce1e4159c6b0b724f7309c927aaaea5a6c7
Must Be     =

HMAC-RIPE320=821543a295eccf093af9b461f630d7c49206fb2158e5ac89e73a02cf089528ed
Must Be     =

**** Test RipeMD 320 ****

Contens=The quick brown fox jumps over the lazy dog
Result =e7660e67549435c62141e51c9ab1dcc3b1ee9f65c0b3e561ae8f58c5dba3d21997781cd1cc6fbc34
Must Be=e7660e67549435c62141e51c9ab1dcc3b1ee9f65c0b3e561ae8f58c5dba3d21997781cd1cc6fbc34

Contens=""
Result =22d65d5661536cdc75c1fdf5c6de7b41b9f27325ebc61e8557177d705a0ec880151c3a32a00899b8
Must Be=22d65d5661536cdc75c1fdf5c6de7b41b9f27325ebc61e8557177d705a0ec880151c3a32a00899b8

HMAC-RIPE320=c6e94996ce2f32b5e819a9b6d2ec2c69d727a66612729fa070deeffed1ac066fe73fd1397d02d049
Must Be     =

HMAC-RIPE320=a61c1123c1a23ffdd9af5fb28b62b95f989876ac322452f5e4f4470c25182ef4816aaec7f0377167
Must Be     =

**** Test Poly1305 ****

Contens=43727970746f6772617068696320466f72756d2052657365617263682047726f7570
Result =a8061dc1305136c6c22b8baf0c0127a9
Must Be=a8061dc1305136c6c22b8baf0c0127a9

Contens=2754776173206272696c6c69672c20616e642074686520736c6974687920746f7665730a446964206779726520616e642067696d626c6520696e2074686520776162653a0a416c6c206d696d737920776572652074686520626f726f676f7665732c0a416e6420746865206d6f6d65207261746873206f757467726162652e
Result =4541669a7eaaee61e708dc7cbcc5eb62
Must Be=4541669a7eaaee61e708dc7cbcc5eb62

Contens=43727970746f6772617068696320466f72756d2052657365617263682047726f7570
HMAC is=a8061dc1305136c6c22b8baf0c0127a9
Must Be=a8061dc1305136c6c22b8baf0c0127a9

Contens=2754776173206272696c6c69672c20616e642074686520736c6974687920746f7665730a446964206779726520616e642067696d626c6520696e2074686520776162653a0a416c6c206d696d737920776572652074686520626f726f676f7665732c0a416e6420746865206d6f6d65207261746873206f757467726162652e
HMAC is=4541669a7eaaee61e708dc7cbcc5eb62
Must Be=4541669a7eaaee61e708dc7cbcc5eb62
                     
```




