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
	rm -f test_hashes

test: test_hashes

libHASHES=stuff.o md2.o md4.o md5.o sha1.o sha2.o sha3.o    \
          ripe128.o ripe160.o ripe256.o ripe320.o poly1305.o 

test_hashes: hash.o  test_hashes.c
	$(mkCC_EXE) test_hashes.c hash.o $(libHASHES)

hash.o: $(libHASHES) hash.c hash.h
	$(mkCC_OBJ) hash.c

stuff.o: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

md2.o: stuff.o md2.c md2.h
	$(mkCC_OBJ) md2.c

md4.o: stuff.o md4.c md4.h
	$(mkCC_OBJ) md4.c

md5.o: stuff.o md5.c md5.h
	$(mkCC_OBJ) md5.c

sha1.o: stuff.o sha1.c sha1.h
	$(mkCC_OBJ) sha1.c

sha2.o: sha1.o sha2.c sha2.h
	$(mkCC_OBJ) sha2.c

sha3.o: sha2.o sha3.c sha3.h
	$(mkCC_OBJ) sha3.c

ripe128.o: stuff.o ripe128.c ripe128.h
	$(mkCC_OBJ) ripe128.c

ripe160.o: stuff.o ripe160.c ripe160.h
	$(mkCC_OBJ) ripe160.c

ripe256.o: stuff.o ripe256.c ripe256.h
	$(mkCC_OBJ) ripe256.c

ripe320.o: stuff.o ripe320.c ripe320.h
	$(mkCC_OBJ) ripe320.c

poly1305.o: stuff.o poly1305.c poly1305.h
	$(mkCC_OBJ) poly1305.c


