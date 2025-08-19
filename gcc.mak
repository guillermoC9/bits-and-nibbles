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

dummy:
	@echo "Tell me what to compile!"
	@echo "targets: buildit (build all dirs) or cleanit (clean all builds)"

cleanit:
	cd checksum; make -f gcc.mak cleanit; cd ..
	cd chipiona; make -f gcc.mak cleanit; cd ..
	cd ciphers;  make -f gcc.mak cleanit; cd ..
	cd csv;      make -f gcc.mak cleanit; cd ..
	cd ecc;      make -f gcc.mak cleanit; cd ..
	cd entropy;  make -f gcc.mak cleanit; cd ..
	cd hashes;   make -f gcc.mak cleanit; cd ..	
	cd mpint;    make -f gcc.mak cleanit; cd ..
	cd pdf;      make -f gcc.mak cleanit; cd ..
	cd pem;      make -f gcc.mak cleanit; cd ..
	cd primes;   make -f gcc.mak cleanit; cd ..
	cd random;   make -f gcc.mak cleanit; cd ..			
	cd rsa;      make -f gcc.mak cleanit; cd ..	
	cd time;     make -f gcc.mak cleanit; cd ..	

buildit: 
	cd checksum; make -f gcc.mak test; cd ..
	cd chipiona; make -f gcc.mak test; cd ..
	cd ciphers;  make -f gcc.mak test; cd ..
	cd csv;      make -f gcc.mak test; cd ..	
	cd ecc;      make -f gcc.mak test; cd ..	
	cd entropy;  make -f gcc.mak test; cd ..
	cd hashes;   make -f gcc.mak test; cd ..
	cd mpint;    make -f gcc.mak test; cd ..
	cd pdf;      make -f gcc.mak test; cd ..
	cd pem;      make -f gcc.mak test; cd ..
	cd primes;   make -f gcc.mak test; cd ..	
	cd random;   make -f gcc.mak test; cd ..
	cd rsa;      make -f gcc.mak test; cd ..	
	cd time;     make -f gcc.mak test; cd ..




