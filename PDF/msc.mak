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

## Targets ##

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:		
	del /f *.obj
	del /f *.txt
	del /f test_pdf.exe

test: test_pdf.exe

## Other Stuff 

adler32.obj: zlib\adler32.c
	$(mkCC_OBJ) zlib\adler32.c

crc32.obj: zlib\crc32.c
	$(mkCC_OBJ) zlib\crc32.c

deflate.obj: zlib\deflate.c
	$(mkCC_OBJ) zlib\deflate.c

infback.obj: zlib\infback.c
	$(mkCC_OBJ) zlib\infback.c

inffast.obj: zlib\inffast.c
	$(mkCC_OBJ) zlib\inffast.c

inflate.obj: zlib\inflate.c
	$(mkCC_OBJ) zlib\inflate.c

inftrees.obj: zlib\inftrees.c
	$(mkCC_OBJ) zlib\inftrees.c

trees.obj: zlib\trees.c
	$(mkCC_OBJ) zlib\trees.c

zutil.obj: zlib\zutil.c
	$(mkCC_OBJ) zlib\zutil.c

compress.obj: zlib\compress.c
	$(mkCC_OBJ) zlib\compress.c

uncompr.obj: zlib\uncompr.c
	$(mkCC_OBJ) zlib\uncompr.c

gzclose.obj: zlib\gzclose.c
	$(mkCC_OBJ) zlib\gzclose.c
	
gzlib.obj: zlib\gzlib.c
	$(mkCC_OBJ) zlib\gzlib.c

gzread.obj: zlib\gzread.c
	$(mkCC_OBJ) zlib\gzread.c

gzwrite.obj: zlib\gzwrite.c
	$(mkCC_OBJ) zlib\gzwrite.c

zLibOBJ=adler32.obj crc32.obj deflate.obj infback.obj inffast.obj inflate.obj inftrees.obj \
		trees.obj zutil.obj compress.obj uncompr.obj gzclose.obj  gzlib.obj gzread.obj  gzwrite.obj  

## Other Stuff 

stuff.obj: $(zLibOBJ) stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

freetype.obj: freetype\freetype.c freetype\freetype.h
	$(mkCC_OBJ) freetype\freetype.c

barcode.obj: stuff.obj barcode.c barcode.h
	$(mkCC_OBJ) barcode.c

pdf_font.obj: stuff.obj freetype.obj pdf_font.c pdf_font.h pdf_defs.h
	$(mkCC_OBJ) pdf_font.c

pdf_image.obj: stuff.obj pdf_image.c pdf_image.h pdf_defs.h
	$(mkCC_OBJ) pdf_image.c

pdf.obj: barcode.obj pdf_font.obj pdf_image.obj pdf.c pdf.h
	$(mkCC_OBJ) pdf.c

test_pdf.exe: pdf.obj test_pdf.c
	$(mkCC_EXE) test_pdf.c $(zLibOBJ) freetype.obj stuff.obj barcode.obj pdf_font.obj pdf_image.obj pdf.obj
	