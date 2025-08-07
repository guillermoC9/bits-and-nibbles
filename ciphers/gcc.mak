#
#   gcc.mak
#
#   Makefile for Macintosh, Linux and any gcc based
#
#   (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.
#
#   I dedicate the software in this file to the public domain, so it is entirely free.
#   Thus you may use it and/or redistribute it for whatever purpose you may choose.
#
#   It is actually distributed with the hope that it would be useful to someone,
#   but with no guarantee whatsoever. Not even the guarantee that it will work.
#
#   Also, being free software, it is provided without warranty of any kind,
#   not even the implied warranty of merchantability or fitness for a particular
#   purpose. I also disclaim any liability arising from any use of this software.
#
#   Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/
#

## Flags ##

mkC_FLAGS=-Wall -O2 -o $@ -fPIC -fno-strict-aliasing

mkCC_OBJ=gcc $(mkCC)  $(mkC_FLAGS) -c
mkCC_EXE=gcc $(mkCC)  $(mkC_FLAGS)

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:
	rm -f *.o
	rm -f *.txt	
	rm -f test_ciphers

test: test_ciphers

libCIPHERS=stuff.o aes.o aria.o blowfish.o camellia.o chacha.o \
           des.o poly1305.o rc4.o salsa20.o twofish.o xtea.o

test_ciphers: cipher.o test_ciphers.c
	$(mkCC_EXE) test_ciphers.c cipher.o $(libCIPHERS)

cipher.o: $(libCIPHERS) cipher.c cipher.h
	$(mkCC_OBJ) cipher.c

stuff.o: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

aes.o: stuff.o aes.c aes.h
	$(mkCC_OBJ) aes.c

aria.o: stuff.o aria.c aria.h
	$(mkCC_OBJ) aria.c

blowfish.o: stuff.o blowfish.c blowfish.h
	$(mkCC_OBJ) blowfish.c

camellia.o: stuff.o camellia.c camellia.h
	$(mkCC_OBJ) camellia.c

chacha.o: stuff.o chacha.c chacha.h
	$(mkCC_OBJ) chacha.c

des.o: des.c des.h
	$(mkCC_OBJ) des.c

poly1305.o: stuff.o poly1305.c poly1305.h
	$(mkCC_OBJ) poly1305.c

rc4.o: rc4.c rc4.h
	$(mkCC_OBJ) rc4.c

salsa20.o: stuff.o salsa20.c salsa20.h
	$(mkCC_OBJ) salsa20.c

twofish.o: stuff.o twofish.c twofish.h
	$(mkCC_OBJ) twofish.c

xtea.o: stuff.o xtea.c xtea.h
	$(mkCC_OBJ) xtea.c


