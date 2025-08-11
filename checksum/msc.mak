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

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:
	del /f *.obj
	del /f test_checksum.exe

test: test_checksum.exe	

test_checksum.exe: luhn.obj dni.obj test_checksum.c
	cl /Fetest_checksum.exe test_checksum.c dni.obj luhn.obj

dni.obj: dni.c dni.h
	cl /c /Fodni.obj dni.c

luhn.obj: luhn.c luhn.h
	cl /c /Foluhn.obj luhn.c
