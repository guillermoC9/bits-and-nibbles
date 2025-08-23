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
	del /f test_hashes.exe

test: test_hashes.exe

libHASHES=stuff.obj md2.obj md4.obj md5.obj sha1.obj sha2.obj sha3.obj \
          ripe128.obj ripe160.obj ripe256.obj ripe320.obj poly1305.obj

test_hashes.exe: hash.obj  test_hashes.c
	$(mkCC_EXE) test_hashes.c hash.obj $(libHASHES)

hash.obj: $(libHASHES) hash.c hash.h
	$(mkCC_OBJ) hash.c

stuff.obj: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

md2.obj: stuff.obj md2.c md2.h
	$(mkCC_OBJ) md2.c

md4.obj: stuff.obj md4.c md4.h
	$(mkCC_OBJ) md4.c

md5.obj: stuff.obj md5.c md5.h
	$(mkCC_OBJ) md5.c

sha1.obj: stuff.obj sha1.c sha1.h
	$(mkCC_OBJ) sha1.c

sha2.obj: sha1.obj sha2.c sha2.h
	$(mkCC_OBJ) sha2.c

sha3.obj: sha2.obj sha3.c sha3.h
	$(mkCC_OBJ) sha3.c

ripe128.obj: stuff.obj ripe128.c ripe128.h
	$(mkCC_OBJ) ripe128.c

ripe160.obj: stuff.obj ripe160.c ripe160.h
	$(mkCC_OBJ) ripe160.c

ripe256.obj: stuff.obj ripe256.c ripe256.h
	$(mkCC_OBJ) ripe256.c

ripe320.obj: stuff.obj ripe320.c ripe320.h
	$(mkCC_OBJ) ripe320.c

poly1305.obj: stuff.obj poly1305.c poly1305.h
	$(mkCC_OBJ) poly1305.c


