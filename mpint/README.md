# mpint
## Multiprecision Integers

Big numbers are essential to many tasks, especially to criptography. Here we present a multi-precision integer library called mpint, which was originally written by  Michael J. Fromberger's, who donated it to the public domain in 2002. The original code can be found at https://fossies.org/linux/txr/mpi/mpi.h 

I adapted the original code to my needs addig few bits and nibbles to it so I can use it for my criptography stuff like: prime numbers, RSA, ECC, etc. 

I left this here alone in case all you are interested on is a multi-precision library implemntation that works, even if I messed up a bit the function interface. :-(

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

$./test_mpint

== MPI TEST START 

m1 has 6 bytes (CORRECT)

Showing them:

Real m1   = 0xcafebabefeca
sbytes(6) = 00cafebabefeca
dbytes(6) = cafebabefeca
sbytes(7) = 00cafebabefeca (Copied)

m1 - Copy Exact Test

0 - ebytes(10) = 00000000cafebabefeca
0 - ebytes(4)  = babefeca

m1 = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff
m2 = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff
m1 = 0x9
m2 = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff
m1 have 4 bits
m2 have 208 bits

m2 has 7 digits (CORRECT)

Showing them:

Real m2 = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff
sdig[7] = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff
ddig[7] = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff

m1 = 0x9
m2 = 0xcafe0123456789abcdef00112233445566778899aabbccddeeff

bit   0 of m1 = 1 (1)
bit   1 of m1 = 0 (0)
bit   2 of m1 = 0 (0)
bit   3 of m1 = 1 (1)
bit   4 of m1 = 0 (0)

bit 203 of m2 = 1 (1)
bit 204 of m2 = 0 (0)
bit 205 of m2 = 0 (0)
bit 206 of m2 = 1 (1)
bit 207 of m2 = 1 (1)

2^ 8 = 256 - HEX  = 0x100
2^10 = 1024 - HEX  = 0x400
2^12 = 4096 - HEX  = 0x1000
2^14 = 16384 - HEX  = 0x4000
2^16 = 65536 - HEX  = 0x10000
2^18 = 262144 - HEX  = 0x40000
2^20 = 1048576 - HEX  = 0x100000
2^22 = 4194304 - HEX  = 0x400000
2^24 = 16777216 - HEX  = 0x1000000
2^26 = 67108864 - HEX  = 0x4000000

r  = 0xe447
r2 = 0x13

r  = 0xe447000
r2 = 0x1300000000

r  = 0xe447
r2 = 0x13

bit  0 of r = 1 (1)
bit  1 of r = 1 (1)
bit  2 of r = 1 (1)
bit  3 of r = 0 (0)
bit  4 of r = 0 (0)
bit 50 of r = 0 (0)

bit  0 of r2 = 1 (1)
bit  1 of r2 = 1 (1)
bit  2 of r2 = 0 (0)
bit  3 of r2 = 0 (0)
bit  4 of r2 = 1 (1)
bit 51 of r2 = 0 (0)

r  = 0x400000000e447
r2 = 0x8000000000013

bit 50 of r  = 1 (1)
bit 51 of r2 = 1 (1)

r       = 0xe447
r2      = 0x13

bit 50 of r  = 0 (0)
bit 51 of r2 = 0 (0)

r       = 0xe447
r2      = 0xcb8e8bb1
srt(r2) = 0xe447
r2      = 0xa1db60e9fa95b061
srt(r2) = 0xcb8e8bb1
r2      = 0x6655abe5423355c0260284963e6f84c1
srt(r2) = 0xa1db60e9fa95b061
r2      = 0x28e861a64a6af22c2567de617e09dae1a48d1e630fe2a93799d4cf0de0359981
srt(r2) = 0x6655abe5423355c0260284963e6f84c1
r2      = 0x689717519e7bdee5f8dc809b898531ff0433b88edd14e3d747ef74a6fde93ceb006598c64df4448cd5a8e29a07396f1fd9076c96deb2307a7b60cf4ab757301
srt(r2) = 0x28e861a64a6af22c2567de617e09dae1a48d1e630fe2a93799d4cf0de0359981

== MPI TEST END
                     
```




