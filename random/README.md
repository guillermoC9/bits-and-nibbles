# Random
## Pseudo-Random Numbers

Probably we will never know if the sentence *"Random numbers are too important to be left to chance"* came from Von Neummann, Coveyou or someone else, but whoever said it does not have any impact on the truthfulness of the afirmation. Random numbers are both important and neither should be left to chance. Random numbers need to have some properties which depend on the use you want to give them. It is not the same to get random numbers for chance games than for applied criptography: **not at all**.  

As we discussed at [**Entropy**](https://github.com/guillermoC9/bits-and-nibbles/edit/main/entropy) in this same repository, when people talk about random numbers they actually mean pseudo-random numbers, as true random numbers should be called *unpredictable numbers*.

Here we present a set of functions to generate proper pseudo-random numbers using well-known general purpose pseudo-random generators, as well as a few criptographically safe ones, being one of them the system's own generator.

This code here is designed to follow a principle of separation, as long ago -after reading a paper by Bruce Schneier- I decided to separate the generation of random numbers from the tasks that used them, so a generator that is not fit for purpose anymore can be changed without refactoring all the code that uses it, only the code of the generator. I am convinced that this makes the code not only flexible, but also more robust. 

I did this by abstracting the generators into a struct called ```rand_t``` which contains pointers to functions for the generators, allowing the use of different generators by just changing the parameters to ```rand_start()```. There is also a mechanism to provision new generators through the function ```rand_create()```.

If you think for a second, this is the same abstraction principal used in [**Hashes**](https://github.com/guillermoC9/bits-and-nibbles/edit/main/hashes)  and [**Ciphers**](https://github.com/guillermoC9/bits-and-nibbles/edit/main/ciphers). 

The pseudo-random generators here are quite interesting actually, you can even find TLS PRF ready to be used in real TLS implementations. 
The complete list is: 
``` 

    RAND_GM             George Marsaglia's Mother.
    RAND_MT             Mersenne Twister.  
    RAND_GX             George Marsaglia's Xorshift128.  
    RAND_TLS_MD5_SHA1   MD5+SHA1 based PRF (Default for TLSv1.0 and TLSv1.1)
    RAND_TLS_SHA256     SHA256 based PRF (Default for TLSv1.2)
    RAND_TLS_SHA384     SHA384 base PRF (Especified by connection since TLSv1.2)
    RAND_OS             Operating System generator. 
```

There is also an implementation in C of the ```AutoId:newId()``` JavaScript function from the Firestore library.

The test program for this stuff is actually a quite interesting test tool. So have a look at the code and give it a good play :-)

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
$ ./test_random

TEST RANDOM

use:	test_random [-v] alg seed num

Generate 'num' different random numbers using 'alg' algorithm
counting repetitions and showing the numbers repeated more 
than once, as well as the total number of tries.

'seed' is a number used to seed the algorithm. Unless is 0, which
makes the algorithm to be seeded with an umpredictable number.

Algorithms:

	1 = Mother of George Marsaglia (RAND_GM)
	2 = Mersenne Twister of Matsumoto & Nishimura (RAND_MT)
	3 = Xorshift of George Marsaglia (RAND_GX)
	4 = TLS-PRF-MD5_SHA1 (RAND_TLS_MD5_SHA1)
	5 = TLS-PRF-SHA256 (RAND_TLS_SHA256)
	6 = TLS-PRF-SHA384 (RAND_TLS_SHA384)
	7 = OSSYS of Kenneth MacKay (RAND_OS)

options:
	-v = Verbose. Would also show the numbers repeated once

	Set 'alg' as 'test' and 'seed' as a number of bits instead for a quick test.

$ ./test_random test

*** JavaScript AutoID Test ***

firestore_auto_id() = YeGCVUVMONBuDCXmT7GE
firestore_auto_id() = OH3XDraxiaAqmWvMl3Tg
firestore_auto_id() = GNG9VD54iSiw5jtGEyNO
firestore_auto_id() = Ik2OroKxjEXfmykYiiFe
firestore_auto_id() = pLfzaOjx35lW9TVpkVX9
firestore_auto_id() = lEjin9twwIUPPKJh0E7w
firestore_auto_id() = m94xkuLb4Syj67P5b3kc
firestore_auto_id() = 7a4MRlYxDBliGVkI1ie3
firestore_auto_id() = cjCWn7cG1fNcKmH1nSig
firestore_auto_id() = 837vH1Gr7s9hw4dSY1iX
firestore_auto_id() = 4GGq1pImosuqko9dW4EV
firestore_auto_id() = 1qto9f7NZZFsNHhr4f0S
firestore_auto_id() = nln0XN5xXZ0JpV6lexed
firestore_auto_id() = Tka3mSbhLYCMuhySNr6t
firestore_auto_id() = QtUGL6aGFFSoWTONMiHr
firestore_auto_id() = H9rIlXR1C0hA0TCWtyNu
firestore_auto_id() = uGIH1uCUKXaNYruYkKq7
firestore_auto_id() = 3Dd2CR7lV6yfHQHFjBhU
firestore_auto_id() = GSfJ1Jj9Yyl9lA3bGjLt
firestore_auto_id() = C4dqDH8EzVBQ5z8Bj1o6


*** Test RANDOM bits (seed = 1234) ***

Using 53 bits

53 bits : 00000000000100101100000011010110 01100010111010010000010000111011 (0x0012c0d662e9043b) PRF: Mother
53 bits : 00000000000100100100110001000000 00100010010010101101010101100011 (0x00124c40224ad563) PRF: Mersenne-Twister
53 bits : 00000000000110011010000001100110 11111111001000001110111111011000 (0x0019a066ff20efd8) PRF: XorShift128
53 bits : 00000000000100110111110101101111 00110110111100010000000110101110 (0x00137d6f36f101ae) PRF: TLS-PRF(MD5-SHA1)
53 bits : 00000000000111001101000001011010 00010010011101000100111111001000 (0x001cd05a12744fc8) PRF: TLS-PRF(SHA256)
53 bits : 00000000000110011100100001101000 10101011111111111100000011110100 (0x0019c868abffc0f4) PRF: TLS-PRF(SHA384)
53 bits : 00000000000110100000110101001101 11011100011110001111001010111011 (0x001a0d4ddc78f2bb) PRF: OSSYS

*** Test PRF ***

tls_test_prf(REAL) -> TLSv1.0 (MD5^SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55
tls_test_prf(CALC) -> TLSv1.0 (MD5+SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55
tls_test_prf(REAL) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754
tls_test_prf(CALC) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754
tls_test_prf(REAL) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a
tls_test_prf(CALC) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a

**** Test hash_get_entropy() ****

hash_get_entropy(MD5)=c1fbc5869a4ad3609b955d8cc7ca054c6ef3dcb1fbeccc146c379f664bf7209d5d7a83e5195e6257b651bb7af9d3c4f249161ad4e429b8de12e2a495e01371a4
hash_get_entropy(SHA-1)=eb14aa5e3e30ae9cefb02054b8cfa9c698df4f5ccd75e7155f36ef5e25983d8850b18ea7d9d96d1c7cf797147200ce2cb16597791cd10f7e3d3f68de8168d8ba
hash_get_entropy(SHA-224)=88b4f5f4985b6fc7ae40cf7ecd8325cb07f7dadd9d51a00468405ba5e9c4dd53d8a69559338e5c7092533e4c34aa0b684b7e3b9a732b3843bad68c160ec1d1c8
hash_get_entropy(SHA-256)=5a5432358d8bdd9989c4725b0be03e06344503a3fb1199d1a7062720dd1df2071da6a929831e17fa808c33e385a1f5c8cf5fffe0b2d3504f2610d472d6348e37
hash_get_entropy(SHA-384)=32b6e46577b3702a801224de7e16fbeb220ab6478857ef5aa7cb9a471577ac5ee6964c91ef23cf547eb68e32a7979ae41d4073c9c53449feda70c390a25336c9
hash_get_entropy(SHA-512)=3d9da142c145e3c21ed88cbcd8984e88bf393b9d51eb3bbdd79b0bb0e0985ef0e11f46c14dd3afffa6c7679950bfdaccf407039dfbb387235c000203f104072e

$ ./test_random test 33

*** JavaScript AutoID Test ***

firestore_auto_id() = WV4LrpVjXz2R2Yk5y6KX
firestore_auto_id() = nNAJkCQth2nwNWOP67FV
firestore_auto_id() = 88UqfsAq8pnGdh5eIprM
firestore_auto_id() = zj4EEc3H3LCsmIS5wNiY
firestore_auto_id() = iANufn02FXU9jLlYE3lS
firestore_auto_id() = QG9JwjWzNDf15xvM9VT3
firestore_auto_id() = ha0TYbNT29HnkNRQpgNc
firestore_auto_id() = hOFoHuhOqQjesTlzHgRd
firestore_auto_id() = kYwAD93GEa4S1kId0iVw
firestore_auto_id() = kY2KkQouCxe20yYcQJgj
firestore_auto_id() = MwSmc0cqvY439vG8NFi2
firestore_auto_id() = 3WLWNAp0JH35ADhqEOrS
firestore_auto_id() = pe9GwzZZpdn11EfaBwF7
firestore_auto_id() = n9qCZdwcRhuwHFF6Yhhj
firestore_auto_id() = xall92iaKwpRwMC0TwGC
firestore_auto_id() = O1KNmgpbKMxjXzSJUKky
firestore_auto_id() = DY3NYeFJPfqYLauI4urV
firestore_auto_id() = 7lyAwJYnyFB85Jy1SnGP
firestore_auto_id() = kXJj8tKd0ou92RpXg3dI
firestore_auto_id() = KM8rwjCMN3ogMQq0w1iX

*** Test RANDOM bits (seed = 1234) ***

Using 33 bits

33 bits : 00000000000000000000000000000001 11000000110101100110001011101001 (0x00000001c0d662e9) PRF: Mother
33 bits : 00000000000000000000000000000001 01001100010000000010001001001010 (0x000000014c40224a) PRF: Mersenne-Twister
33 bits : 00000000000000000000000000000001 10100000011001101111111100100000 (0x00000001a066ff20) PRF: XorShift128
33 bits : 00000000000000000000000000000001 01111101011011110011011011110001 (0x000000017d6f36f1) PRF: TLS-PRF(MD5-SHA1)
33 bits : 00000000000000000000000000000001 11010000010110100001001001110100 (0x00000001d05a1274) PRF: TLS-PRF(SHA256)
33 bits : 00000000000000000000000000000001 11001000011010001010101111111111 (0x00000001c868abff) PRF: TLS-PRF(SHA384)
33 bits : 00000000000000000000000000000001 11111000010010001100011010010111 (0x00000001f848c697) PRF: OSSYS

*** Test PRF ***

tls_test_prf(REAL) -> TLSv1.0 (MD5^SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55
tls_test_prf(CALC) -> TLSv1.0 (MD5+SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55
tls_test_prf(REAL) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754
tls_test_prf(CALC) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754
tls_test_prf(REAL) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a
tls_test_prf(CALC) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a

**** Test hash_get_entropy() ****

hash_get_entropy(MD5)=9752f042e35538b9869164830a9e3d743473feafb14ca19d89a5c450838c32b4287bc5202e114274942475ff2f351c658dba0533a61d68786d3e76de6bbe03cc
hash_get_entropy(SHA-1)=91ef11bee7e81f3e34849319f889bac0e0ad3c51346bb653289ce688a98367c142597473b776d39805122590f957bd6d0a0d9cf3ab1c5b018874c17941efe27c
hash_get_entropy(SHA-224)=e92f12b3a51440cc8b87d8ad2ff0ffd3c9fa2c333792faafad532fd4562b1c24345c75c2d9d1e52170864c894382ed0a3f1f8e3dbd883a8ef8b7e6cc67571208
hash_get_entropy(SHA-256)=a79332507e76cec0593878862a2190ac64a6451b96fe7dcd38fda329db2b6d6371f67f695da68805fc40be3a4b93c3fc83476489cf17deae4e2e602fef18598c
hash_get_entropy(SHA-384)=20ac27b3c171af14fdb16c3ac65dbdb1572d00519b1c7f69a5baf605cc633fb62c040aebc2144692b5a16745ea388eba0dcd6454ea7c39657f0098854cdb9331
hash_get_entropy(SHA-512)=81badfabc1d1bcfec2f9c34174e8fff1c13525fc421786c9e5e6bd638f6e85c2ccfa8d268e2df6bc0f2f758d73231f1407011d6c479479109e13afb9d566402f

$ ./test_random test 7

*** JavaScript AutoID Test ***

firestore_auto_id() = kXSbWtmzCbAb6Cbr0nKh
firestore_auto_id() = GegKac5CQd3TGGwz7obA
firestore_auto_id() = HWQEyzTdeEoLdomnlMiB
firestore_auto_id() = HE8x9bZH5lZbmGoG9ZUm
firestore_auto_id() = 9Wd7cWLIzUekEeI0o54M
firestore_auto_id() = FSLnJTq16iVXYy3jD6gj
firestore_auto_id() = oTaN0s3ddSEbQIwT47sd
firestore_auto_id() = 1Z6RnQO0xoQeFAqPXykg
firestore_auto_id() = 95dL6eDBxEtJLpIWlM6H
firestore_auto_id() = BSAmUGvXJmMfT9OykiGq
firestore_auto_id() = d9DKgFA23iynbSqkk1q1
firestore_auto_id() = j3d611VOCdggRamo8ADW
firestore_auto_id() = Wthc6V5TRkCVxTtIAT9h
firestore_auto_id() = rS9DlxtNT1akxflgvPjt
firestore_auto_id() = LfjdbO0Jtvz3kTWNKalL
firestore_auto_id() = k96MnrjMuE0Fhe8GgePm
firestore_auto_id() = 4d33j9TympGaIWVNXgr4
firestore_auto_id() = IvX1SwlT4z9piBOMOusY
firestore_auto_id() = lMQmcgD2xR3ECNbsjNq2
firestore_auto_id() = f1YwY0vVkuhRYV0wrD3g

*** Test RANDOM bits (seed = 1234) ***

Using 7 bits

7 bits : 00000000000000000000000000000000 00000000000000000000000001010010 (0x0000000000000052) PRF: Mother
7 bits : 00000000000000000000000000000000 00000000000000000000000001000010 (0x0000000000000042) PRF: Mersenne-Twister
7 bits : 00000000000000000000000000000000 00000000000000000000000001101001 (0x0000000000000069) PRF: XorShift128
7 bits : 00000000000000000000000000000000 00000000000000000000000001100011 (0x0000000000000063) PRF: TLS-PRF(MD5-SHA1)
7 bits : 00000000000000000000000000000000 00000000000000000000000001011100 (0x000000000000005c) PRF: TLS-PRF(SHA256)
7 bits : 00000000000000000000000000000000 00000000000000000000000001001001 (0x0000000000000049) PRF: TLS-PRF(SHA384)
7 bits : 00000000000000000000000000000000 00000000000000000000000001010110 (0x0000000000000056) PRF: OSSYS

*** Test PRF ***

tls_test_prf(REAL) -> TLSv1.0 (MD5^SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55
tls_test_prf(CALC) -> TLSv1.0 (MD5+SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55
tls_test_prf(REAL) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754
tls_test_prf(CALC) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754
tls_test_prf(REAL) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a
tls_test_prf(CALC) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a

**** Test hash_get_entropy() ****

hash_get_entropy(MD5)=0baa31c9a9c2845a3b42371350d3bcf1d1155e552a410b61713899ecc3d84a6095a526a6e7709a758c0f0d5a42b731012a89a8a667a22f5c666d4b3074a38803
hash_get_entropy(SHA-1)=1f4c442795070be5d0c8115e8ed77135648980b2321c29b5f6be21c46157a823371858a45607d024f84171786064c64b204a8f38006eda74f588a11ccd07b91c
hash_get_entropy(SHA-224)=32f6df470f610d6344093d9311aab3b1200059bb89b0babcec78a80bc1db5ee2a4badea1385dfaa4f686d904d4299fa0e4dc9f1564e59934b0c9238674e3f336
hash_get_entropy(SHA-256)=87821656a6347b4c3d5e917c61eb9454d002fcbaa018904000510efba089be633d30211d880a465c6273b35fe9ebdd11669678b77b6b65909b0ab282eb5acb5f
hash_get_entropy(SHA-384)=7d3e9dc3320c853a1a16414fd76f818cd9af3a940c0c397edd0f0c85afaec57660221ce3d33fa8c819582c48880a6311c3d65c1791c624f50807c6f06c4b057a
hash_get_entropy(SHA-512)=7ff39b569042455e287ac304e6320527194e76b353e9de0e49d6b687a54860e3ba6c357067e7114b65df756d3d94e50fcb839a8a72d73adf6e4e1fa26f454466

$ ./test_random 1 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using Mother
Done!
Counting repetitions...
0xb1be31c1 repeated 2 time(s)
0xa1d98baf repeated 2 time(s)
0x48281cb7 repeated 2 time(s)
0xa4d12c83 repeated 2 time(s)
0x0de72c23 repeated 2 time(s)

Total of numbers    : 6000000

Total of tries      : 6004195
Total of repeats    : 4195
Number of 1 repeats : 4185
Number of 2 repeats : 5
Number of 3 repeats : 0
Generation time     : 1.125 seconds

Note: 1 repeat means the same number was generated twice

$ ./test_random 2 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using Mersenne-Twister
Done!
Counting repetitions...
0x28184080 repeated 2 time(s)

Total of numbers    : 6000000

Total of tries      : 6004153
Total of repeats    : 4153
Number of 1 repeats : 4151
Number of 2 repeats : 1
Number of 3 repeats : 0
Generation time     : 1.130 seconds

Note: 1 repeat means the same number was generated twice


$ ./test_random 3 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using XorShift128
Done!
Counting repetitions...
0x28445d25 repeated 2 time(s)
Total of numbers    : 6000000

Total of tries      : 6004150
Total of repeats    : 4150
Number of 1 repeats : 4148
Number of 2 repeats : 1
Number of 3 repeats : 0
Generation time     : 1.086 seconds

Note: 1 repeat means the same number was generated twice

$ ./test_random 4 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using TLS-PRF(MD5-SHA1)
Done!
Counting repetitions...
0xbdea5718 repeated 2 time(s)
0x7ea79780 repeated 2 time(s)
0x23deec34 repeated 2 time(s)
0xb9bf1f9e repeated 2 time(s)
0x0d8ce403 repeated 2 time(s)

Total of numbers    : 6000000

Total of tries      : 6004198
Total of repeats    : 4198
Number of 1 repeats : 4188
Number of 2 repeats : 5
Number of 3 repeats : 0
Generation time     : 48.831 seconds


$ ./test_random 5 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using TLS-PRF(SHA256)
Done!
Counting repetitions...
0x81e3baf2 repeated 2 time(s)
0xad480fcb repeated 2 time(s)

Total of numbers    : 6000000

Total of tries      : 6004246
Total of repeats    : 4246
Number of 1 repeats : 4242
Number of 2 repeats : 2
Number of 3 repeats : 0
Generation time     : 42.898 seconds

Note: 1 repeat means the same number was generated twice

$ ./test_random 6 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using TLS-PRF(SHA384)
Done!
Counting repetitions...
0xa202109b repeated 2 time(s)
0x18b2d4a4 repeated 2 time(s)
0x9e4852ca repeated 2 time(s)

Total of numbers    : 6000000

Total of tries      : 6004172
Total of repeats    : 4172
Number of 1 repeats : 4166
Number of 2 repeats : 3
Number of 3 repeats : 0
Generation time     : 52.367 seconds

Note: 1 repeat means the same number was generated twice

$ ./test_random 7 1234 6000000

*** Test RANDOM ***

Generate 6000000 numbers using OSSYS
Done!
Counting repetitions...

Total of numbers    : 6000000

Total of tries      : 6004154
Total of repeats    : 4154
Number of 1 repeats : 4154
Number of 2 repeats : 0
Number of 3 repeats : 0
Generation time     : 2.515 seconds

Note: 1 repeat means the same number was generated twice

                     
```




