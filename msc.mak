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
	@echo "targets: buildit (build all dirs) or cleanit (clean all builds)"

cleanit:
	cd checksum; nmake /f gcc.mak cleanit; cd ..
	cd chipiona; nmake /f gcc.mak cleanit; cd ..
	cd ciphers;  nmake /f gcc.mak cleanit; cd ..
	cd csv;      nmake /f gcc.mak cleanit; cd ..	
	cd entropy;  nmake /f gcc.mak cleanit; cd ..
	cd hashes;   nmake /f gcc.mak cleanit; cd ..
	cd pdf;      nmake /f gcc.mak cleanit; cd ..	
	cd random;   nmake /f gcc.mak cleanit; cd ..
	cd time;     nmake /f gcc.mak cleanit; cd ..	
	

buildit: 
	cd checksum; nmake /f gcc.mak test; cd ..
	cd chipiona; nmake /f gcc.mak test; cd ..
	cd ciphers;  nmake /f gcc.mak test; cd ..
	cd csv;      nmake /f gcc.mak test; cd ..
	cd entropy;  nmake /f gcc.mak test; cd ..
	cd hashes;   nmake /f gcc.mak test; cd ..
	cd pdf;      nmake /f gcc.mak test; cd ..
	cd random;   nmake /f gcc.mak test; cd ..
	cd time;     nmake /f gcc.mak test; cd ..

