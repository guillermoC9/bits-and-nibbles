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

mkC_FLAGS=-Wall -O2 -o $@ -fPIC -fno-strict-aliasing

mkCC_OBJ=gcc $(mkCC)  $(mkC_FLAGS) -c
mkCC_EXE=gcc $(mkCC)  $(mkC_FLAGS)

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:
	rm -f *.o
	rm -f *.txt	
	rm -f *.pem	
	rm -f *.bin	
	rm -f test_ecc

libOBJS=stuff.o md2.o md5.o sha1.o sha2.o sha3.o hmac.o random.o \
        mpint.o ticks.o curve448.o curve25519.o oid.o asn1.o \
		aes.o des.o cipher.o base64.o pem.o curves.o

test: test_ecc

test_ecc: ecc.o test_ecc.c
	$(mkCC_EXE) test_ecc.c ecc.o $(libOBJS)

ecc.o: curves.o pem.o mpint.o ecc.c ecc.h
	$(mkCC_OBJ) ecc.c

pem.o: random.o asn1.o base64.o cipher.o pem.c pem.h
	$(mkCC_OBJ) pem.c

curves.o: random.o curve25519.o curve448.o curves.c curves.h
	$(mkCC_OBJ) curves.c

mpint.o: stuff.o mpint.c mpint.h
	$(mkCC_OBJ) mpint.c

curve25519.o: mpint.o curve25519.c curve25519.h
	$(mkCC_OBJ) curve25519.c

curve448.o: mpint.o curve448.c curve448.h
	$(mkCC_OBJ) curve448.c

oid.o: stuff.o oid.c oid.h
	$(mkCC_OBJ) oid.c

asn1.o: mpint.o oid.o asn1.c asn1.h
	$(mkCC_OBJ) asn1.c

base64.o: stuff.o base64.c base64.h
	$(mkCC_OBJ) base64.c

cipher.o: aes.o des.o cipher.c cipher.h
	$(mkCC_OBJ) cipher.c

aes.o: stuff.o aes.c aes.h
	$(mkCC_OBJ) aes.c

des.o: des.c des.h
	$(mkCC_OBJ) des.c

random.o: ticks.o hmac.o random.c random.h
	$(mkCC_OBJ) random.c	

hmac.o: md5.o sha3.o hmac.c hmac.h
	$(mkCC_OBJ) hmac.c

md5.o: md2.o md5.c md5.h
	$(mkCC_OBJ) md5.c

md2.o: stuff.o md2.c md2.h
	$(mkCC_OBJ) md2.c

sha1.o: stuff.o sha1.c sha1.h
	$(mkCC_OBJ) sha1.c

sha2.o: sha1.o sha2.c sha2.h
	$(mkCC_OBJ) sha2.c

sha3.o: sha2.o sha3.c sha3.h
	$(mkCC_OBJ) sha3.c


ticks.o: stuff.o ticks.c ticks.h
	$(mkCC_OBJ) ticks.c

stuff.o: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c


