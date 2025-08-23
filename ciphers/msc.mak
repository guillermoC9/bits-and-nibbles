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
#   Read the CC license at https://creativecommons.objrg/publicdomain/zero/1.0/
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
	del /f test_ciphers.exe

test: test_ciphers.exe

libCIPHERS=stuff.obj aes.obj aria.obj blowfish.obj camellia.obj chacha.obj \
           des.obj poly1305.obj rc4.obj salsa20.obj twofish.obj xtea.obj

test_ciphers.exe: cipher.obj test_ciphers.c
	$(mkCC_EXE) test_ciphers.c cipher.obj $(libCIPHERS)

cipher.obj: $(libCIPHERS) cipher.c cipher.h
	$(mkCC_OBJ) cipher.c

stuff.obj: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

aes.obj: stuff.obj aes.c aes.h
	$(mkCC_OBJ) aes.c

aria.obj: stuff.obj aria.c aria.h
	$(mkCC_OBJ) aria.c

blowfish.obj: stuff.obj blowfish.c blowfish.h
	$(mkCC_OBJ) blowfish.c

camellia.obj: stuff.obj camellia.c camellia.h
	$(mkCC_OBJ) camellia.c

chacha.obj: stuff.obj chacha.c chacha.h
	$(mkCC_OBJ) chacha.c

des.obj: des.c des.h
	$(mkCC_OBJ) des.c

poly1305.obj: stuff.obj poly1305.c poly1305.h
	$(mkCC_OBJ) poly1305.c

rc4.obj: rc4.c rc4.h
	$(mkCC_OBJ) rc4.c

salsa20.obj: stuff.obj salsa20.c salsa20.h
	$(mkCC_OBJ) salsa20.c

twofish.obj: stuff.obj twofish.c twofish.h
	$(mkCC_OBJ) twofish.c

xtea.obj: stuff.obj xtea.c xtea.h
	$(mkCC_OBJ) xtea.c