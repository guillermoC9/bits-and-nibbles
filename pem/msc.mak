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
	del /f test_pem.exe

test: test_pem.exe

libOBJS=stuff.obj aes.obj des.obj cipher.obj  md2.obj md5.obj \
        sha1.obj sha2.obj hmac.obj oid.obj mpint.obj asn1.obj \
        base64.obj pem.obj

test_pem.exe: pem.obj test_pem.c
	$(mkCC_EXE) test_pem.c $(libOBJS)

pem.obj: asn1.obj base64.obj cipher.obj hmac.obj pem.c pem.h
	$(mkCC_OBJ) pem.c

stuff.obj: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

oid.obj: stuff.obj oid.c oid.h
	$(mkCC_OBJ) oid.c

asn1.obj: mpint.obj oid.obj asn1.c asn1.h
	$(mkCC_OBJ) asn1.c

mpint.obj: stuff.obj mpint.c mpint.h
	$(mkCC_OBJ) mpint.c

base64.obj: stuff.obj base64.c base64.h
	$(mkCC_OBJ) base64.c

cipher.obj: aes.obj des.obj cipher.c cipher.h
	$(mkCC_OBJ) cipher.c

aes.obj: stuff.obj aes.c aes.h
	$(mkCC_OBJ) aes.c

des.obj: des.c des.h
	$(mkCC_OBJ) des.c

hmac.obj: md5.obj sha2.obj hmac.c hmac.h
	$(mkCC_OBJ) hmac.c

md5.obj: md2.obj md5.c md5.h
	$(mkCC_OBJ) md5.c

md2.obj: stuff.obj md2.c md2.h
	$(mkCC_OBJ) md2.c

sha1.obj: stuff.obj sha1.c sha1.h
	$(mkCC_OBJ) sha1.c

sha2.obj: sha1.obj sha2.c sha2.h
	$(mkCC_OBJ) sha2.c

