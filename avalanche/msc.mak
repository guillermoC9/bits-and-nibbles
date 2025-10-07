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
	del /f test_avalanche.exe

test: test_avalanche.exe	

test_avalanche.exe: avalanche.obj test_avalanche.c
	$(mkCC_EXE) test_avalanche.c avalanche.obj random.obj  sha512.obj stuff.obj 

avalanche.obj: random.obj sha512.obj avalanche.c avalanche.h
	$(mkCC_OBJ) avalanche.c

random.obj: random.c random.h
	$(mkCC_OBJ) random.c

sha512.obj: stuff.obj sha512.c sha512.h
	$(mkCC_OBJ) sha512.c

stuff.obj: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c
