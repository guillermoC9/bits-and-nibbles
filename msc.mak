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
	cd avalanche
	nmake /f msc.mak cleanit 	
	cd checksum
	nmake /f msc.mak cleanit
 	cd ../chipiona
	nmake /f msc.mak cleanit
	cd ../ciphers
	nmake /f msc.mak cleanit
	cd ../csv
	nmake /f msc.mak cleanit
	cd ../ecc
	nmake /f msc.mak cleanit
	cd ../entropy
	nmake /f msc.mak cleanit
	cd ../hashes
	nmake /f msc.mak cleanit
	cd ../json
	nmake /f msc.mak cleanit
	cd ../mpint
	nmake /f msc.mak cleanit
	cd ../pdf
	nmake /f msc.mak cleanit
	cd ../pem
	nmake /f msc.mak cleanit
	cd ../primes
	nmake /f msc.mak cleanit
	cd ../random
	nmake /f msc.mak cleanit
	cd ../rsa
	nmake /f msc.mak cleanit
	cd ../sort
	nmake /f msc.mak cleanit
	cd ../time
	nmake /f msc.mak cleanit
	cd ..	
	

buildit: 
	cd avalanche
	nmake /f msc.mak test
	cd checksum
	nmake /f msc.mak test
	cd ../chipiona
	nmake /f msc.mak test
	cd ../ciphers
	nmake /f msc.mak test
	cd ../csv
	nmake /f msc.mak test
	cd ../ecc
	nmake /f msc.mak test
	cd ../entropy
	nmake /f msc.mak test
	cd ../hashes
	nmake /f msc.mak test
	cd ../json
	nmake /f msc.mak test
	cd ../mpint
	nmake /f msc.mak test
	cd ../pdf
	nmake /f msc.mak test
	cd ../pem
	nmake /f msc.mak test
	cd ../primes
	nmake /f msc.mak test
	cd ../random
	nmake /f msc.mak test
	cd ../rsa
	nmake /f msc.mak test
	cd ../sort
	nmake /f msc.mak test
	cd ../time
	nmake /f msc.mak test
	cd ..

