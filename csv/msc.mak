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
	del /f uk_postcodes.csv
	del /f test_csv.exe

test: test_csv.exe uk_postcodes.csv

csv.obj: csv.c csv.h
	$(mkCC_OBJ) csv.c

test_csv.exe: csv.obj test_csv.c 
	$(mkCC_EXE) test_csv.c csv.obj 

uk_postcodes.csv:
	@echo ""
	@echo "You need to unzip uk_postcodes.zip to obtain uk_postcodes.csv"
	@echo "WARNING: The result file would be ~75 MB in size"
	@echo ""