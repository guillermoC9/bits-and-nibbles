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
	del /f test_3.json
	del /f test_json.exe


libOBJS=stuff.obj base64.obj mpint.obj

test: test_json.exe

test_json.exe: json.obj test_json.c 
	$(mkCC_EXE) test_json.c json.obj $(libOBJS)

json.obj: mpint.obj base64.obj json.c json.h
	$(mkCC_OBJ) json.c

base64.obj: stuff.obj base64.c base64.h
	$(mkCC_OBJ) base64.c

mpint.obj: stuff.obj mpint.c mpint.h
	$(mkCC_OBJ) mpint.c

stuff.obj: stuff.c stuff.h
	$(mkCC_OBJ) stuff.c