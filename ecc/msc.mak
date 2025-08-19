#
#   msc.mak
#
#   Makefile for Microsoft C for Windows
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

mkC_FLAGS=/nologo /O2 /W1

mkCC_OBJ=cl $(mkCC)  $(mkC_FLAGS) /c /Fo$@
mkCC_EXE=cl $(mkCC)  $(mkC_FLAGS) /Fe$@

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:
	del /f *.obj
	del /f *.txt	
	del /f test_ecc.exe

test: test_ecc.exe

libOBJS=stuff.obj md5.obj sha1.obj sha2.obj hmac.obj random.obj \
        mpint.obj curve448.obj curve25519.obj 

test: test_ecc.exe

test_ecc.exe: ecc.obj test_ecc.c
	$(mkCC_EXE) test_ecc.c ecc.obj $(libOBJS)

ecc.obj: random.obj curve25519.obj curve448.obj ecc.c ecc.h
	$(mkCC_OBJ) ecc.c

mpint.obj: stuff.obj mpint.c mpint.h
	$(mkCC_OBJ) mpint.c

curve25519.obj: mpint.obj curve25519.c curve25519.h
	$(mkCC_OBJ) curve25519.c

curve448.obj: mpint.obj curve448.c curve448.h
	$(mkCC_OBJ) curve448.c

random.obj: hmac.obj random.c random.h
	$(mkCC_OBJ) random.c	

hmac.obj: md5.obj sha2.obj hmac.c hmac.h
	$(mkCC_OBJ) hmac.c

md5.obj: stuff.obj md5.c md5.h
	$(mkCC_OBJ) md5.c

sha1.obj: stuff.obj sha1.c sha1.h
	$(mkCC_OBJ) sha1.c

sha2.obj: sha1.obj sha2.c sha2.h
	$(mkCC_OBJ) sha2.c

stuff.obj: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c