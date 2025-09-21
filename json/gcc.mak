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
	rm -f test_3.json
	rm -f test_json

test: test_json

libOBJS=stuff.o base64.o mpint.o

test_json: json.o test_json.c 
	$(mkCC_EXE) test_json.c json.o $(libOBJS)

json.o: mpint.o base64.o json.c json.h
	$(mkCC_OBJ) json.c

base64.o: stuff.o base64.c base64.h
	$(mkCC_OBJ) base64.c

mpint.o: stuff.o mpint.c mpint.h
	$(mkCC_OBJ) mpint.c

stuff.o: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c