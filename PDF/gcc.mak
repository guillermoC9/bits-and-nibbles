#
#   gcc.mak
#
#   Makefile for Macintosh, Linux and any gcc  -Izlib based
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

## Flags ##

mkC_FLAGS=-Wall -O2 -o $@ -fPIC -fno-strict-aliasing

mkCC_ZLIB=gcc $(mkCC) $(mkC_FLAGS) -c -Wno-strict-prototypes
mkCC_OBJ=gcc $(mkCC)  $(mkC_FLAGS) -c
mkCC_EXE=gcc $(mkCC)  $(mkC_FLAGS)

## Targets ##

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:		
	rm -f *.o
	rm -f *.txt	
	rm -f test_pdf

test: test_pdf

## Other Stuff 

adler32.o: zlib/adler32.c
	$(mkCC_ZLIB) zlib/adler32.c

crc32.o: zlib/crc32.c
	$(mkCC_ZLIB) zlib/crc32.c

deflate.o: zlib/deflate.c
	$(mkCC_ZLIB) zlib/deflate.c

infback.o: zlib/infback.c
	$(mkCC_ZLIB) zlib/infback.c

inffast.o: zlib/inffast.c
	$(mkCC_ZLIB) zlib/inffast.c

inflate.o: zlib/inflate.c
	$(mkCC_ZLIB) zlib/inflate.c

inftrees.o: zlib/inftrees.c
	$(mkCC_ZLIB) zlib/inftrees.c

trees.o: zlib/trees.c
	$(mkCC_ZLIB) zlib/trees.c

zutil.o: zlib/zutil.c
	$(mkCC_ZLIB) zlib/zutil.c

compress.o: zlib/compress.c
	$(mkCC_ZLIB) zlib/compress.c

uncompr.o: zlib/uncompr.c
	$(mkCC_ZLIB) zlib/uncompr.c

gzclose.o: zlib/gzclose.c
	$(mkCC_ZLIB) zlib/gzclose.c
	
gzlib.o: zlib/gzlib.c
	$(mkCC_ZLIB) zlib/gzlib.c

gzread.o: zlib/gzread.c
	$(mkCC_ZLIB) zlib/gzread.c

gzwrite.o: zlib/gzwrite.c
	$(mkCC_ZLIB) zlib/gzwrite.c

zLibOBJ=adler32.o crc32.o deflate.o infback.o inffast.o inflate.o inftrees.o \
		trees.o zutil.o compress.o uncompr.o gzclose.o  gzlib.o gzread.o  gzwrite.o  

## Other Stuff 

stuff.o: $(zLibOBJ) stuff.c stuff.h
	$(mkCC_OBJ) stuff.c

freetype.o: freetype/freetype.c freetype/freetype.h
	$(mkCC_OBJ) freetype/freetype.c

barcode.o: stuff.o barcode.c barcode.h
	$(mkCC_OBJ) barcode.c

pdf_font.o: stuff.o freetype.o pdf_font.c pdf_font.h pdf_defs.h
	$(mkCC_OBJ) pdf_font.c

pdf_image.o: stuff.o pdf_image.c pdf_image.h pdf_defs.h
	$(mkCC_OBJ) pdf_image.c

pdf.o: barcode.o pdf_font.o pdf_image.o pdf.c pdf.h
	$(mkCC_OBJ) pdf.c

test_pdf: pdf.o test_pdf.c
	$(mkCC_EXE) test_pdf.c $(zLibOBJ) freetype.o stuff.o barcode.o pdf_font.o pdf_image.o pdf.o
	