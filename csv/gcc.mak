#
#   gcc.mak
#
#   Makefile for Macintosh, Linux and any gcc based
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

mkC_FLAGS=-Wall -O2 -o $@ -fPIC -fno-strict-aliasing

mkCC_OBJ=gcc $(mkCC)  $(mkC_FLAGS) -c
mkCC_EXE=gcc $(mkCC)  $(mkC_FLAGS)

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: test or cleanit (to  clean the builds)"

cleanit:
	rm -f *.o
	rm -f *.txt
	rm -f uk_postcodes.csv
	rm -f test_csv

test: test_csv uk_postcodes.csv

test_csv: csv.o test_csv.c
	$(mkCC_EXE) test_csv.c csv.o

csv.o: csv.c csv.h
	$(mkCC_OBJ) csv.c

uk_postcodes.csv:
	@echo ""
	@echo "You need to unzip uk_postcodes.zip to obtain uk_postcodes.csv"
	@echo "WARNING: The result file would be ~75 MB in size"
	@echo ""
