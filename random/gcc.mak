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
	rm -f test_random

test: test_random

test_random: random.o test_random.c
	$(mkCC_EXE) test_random.c random.o hmac.o md5.o sha2.o sha1.o stuff.o 

random.o: hmac.o random.c random.h
	$(mkCC_OBJ) random.c

hmac.o: md5.o sha2.o hmac.c hmac.h
	$(mkCC_OBJ) hmac.c

md5.o: stuff.o md5.c md5.h
	$(mkCC_OBJ) md5.c

sha1.o: stuff.o sha1.c sha1.h
	$(mkCC_OBJ) sha1.c

sha2.o: sha1.o sha2.c sha2.h
	$(mkCC_OBJ) sha2.c

stuff.o: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c
