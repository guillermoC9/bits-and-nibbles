# PEM  
## Privacy Enhanced Mail files

PEM files were originally described in RFC-1421 and where intended as a way to store encrypted electronic mail. Afterwards, when standards like: PKIX, PKCS and CMS appeared, the use of PEM files was widespread as these standards used them to store certificates, keys etc, as well as the  tool ```openssl``` made really easy to produce them.

Here we present a library that allow to read/write these files easily, although this is not trivial business, as there is many types of PEM files, as well as a wide variety of technologies involved to produce them: ciphers, hashes and ASN.1 encoding.

Yeah, the content of PEM files is always enconded using ASN.1 and thus we include a library that handles all kind of ASN.1 elements, especially OID. OID stands for *Object IDentifier* and is a very important element in PEM files, ASN.1 encoding and specially X509 certificates. The term OID was actually created for the standard X500, the base of most internet services: Directory Services, which allow users to get information on resources. For example: LDAP. X500 OID fields are used extensively everywhere, like in X509 Certificates or PKCS in order to identify algorithms and signatures.

The test program allows to show PEM files in ASN.1 notation format, even if they are encrypted. There is a bunch of them in the directory ```test``` that you can use to play around. The password for the encripted ones is **```test```**.

For those intereseted in understanding ASN.1, there is a lot of explanatory comments about it in ```asn1.h``` and ```ans1.c```. 

## To build it:

For Microsoft C/C++ for Windows (Visual Studio C++) use:  

```
nmake /f msc.mak test
```

For Macintosh, Linux and others using GNU C:

```
make -f gcc.mak test
```
### To clean the builds:

Do the same that for build it but use *cleanit* instead of *test* as the target.

## Example output from the test program:

```
$ cd test
$ ../test_pem test_priv.pem 

*** READING PEM test_priv.pem ***

----- RSA PRIVATE KEY -----

SEQUENCE {
    INTEGER 0 (decimal)
    INTEGER 00 c1 71 0a 15 4a 06 9f 69 1b 9f 14 0a c1 aa c5 ae 6e 1a 73 b4 60 13 da a0 9b 30 cc 24 cb b0 d5
            8b f9 96 66 58 5e d1 06 ef b1 69 85 15 ea 20 d1 e1 1b 43 79 83 85 97 13 d9 40 cc c2 a4 9e 8f 50
            c4 b3 a9 d7 b5 35 a7 5a 45 8e 81 e4 62 65 ff 60 a8 fa a9 87 89 84 9a 96 76 ff 93 2c e8 31 f3 ef
            bf b0 e3 51 77 f4 c9 8a 19 bb 67 ae a4 9b 8c d7 50 a7 f8 ac 3f 78 93 15 9b 1b da c1 f6 42 f1 1c
            dd
    INTEGER 43321 (decimal)
    INTEGER 7c 90 6f 7d 9d 5d 1c ae c8 cc bd 4f fb ee ad ba f0 8e c8 ee 7f 7f 56 84 38 fb d1 89 4a 3d 31 a4
            eb 0e ff 4c 9f e2 5d f2 3f c9 be 07 5c f3 d2 a1 28 6e 64 0d 84 f0 2c 50 4e a6 17 78 da d3 13 8e
            97 b4 48 5d de b8 d7 a6 b4 59 a7 09 28 b8 2e 9d f6 b1 3e 58 40 80 18 e3 9f 28 b9 49 a8 73 1c 57
            b1 bf 44 1a 25 8c b4 8b 3f 5b a0 16 89 ab 2b c5 b3 ce 54 5b 2a 9c 67 bf 47 b5 23 17 ae 79 99 09
    INTEGER 00 e0 55 19 e8 15 29 19 f3 33 02 f6 de 5c e6 ed 93 f9 ec 21 ae ee 76 ea 4d 0f 83 03 72 bc 1e 9e
            1b 0d 66 1d ac 1a 0a e1 7a 6e 7d ad 96 e4 d9 fb d3 56 fe ba 7f 76 d4 67 0f 8a 7c ca 2f ed 0b 4f
            3d
    INTEGER 00 dc bf 9b 92 0e 39 b4 32 c7 25 bc 26 9b 15 e1 c9 60 1d cb c3 33 96 be 1d 9d 8a 86 15 32 a2 98
            46 b4 64 fa b8 93 c9 4e f1 ac 9b 14 e6 03 de ea 72 0f 0c 04 63 82 2b 5c 89 5d b4 7e 21 59 29 de
            21
    INTEGER 25 d9 47 b0 c3 14 c6 36 2b 1f 89 42 3d ee c9 7a f7 9b 09 6c 70 87 42 a9 1a 7d b0 8d 15 bf 0b 8a
            8f 2b 6e 4e 2c 48 37 67 60 30 c0 e9 85 85 8b 2a 28 30 36 79 fd 08 17 6a 6a 0e 1a d1 ff 57 c5 75
    INTEGER 72 c7 88 09 3b 43 5e 52 51 17 9a 7a 32 f8 f6 cb ee 3c b3 04 52 88 15 a3 cd 01 b4 64 cf 17 01 fe
            99 bd e7 56 58 7b db f2 a2 ce 11 65 6a 1e 39 f0 2f 7f d0 e7 38 31 c9 cb 4a bc 4c 38 11 d6 27 a9
    INTEGER 00 a7 ec 25 23 4a 87 25 1e 22 92 49 42 87 3e f4 de 40 88 8b bd 28 fc 16 27 f4 0d 18 ca 56 b7 1a
            6c 07 80 a3 4c f0 74 29 da 14 fa be 9c ac e7 f5 35 2b fc 13 a7 23 e8 65 c7 b9 3b 39 ea 8a a6 5b
            4f
}
$ ../test_pem test_pub.pem 

*** READING PEM test_pub.pem ***

----- PUBLIC KEY -----

SEQUENCE {
    SEQUENCE {
        OBJECT IDENTIFIER 1.2.840.113549.1.1.1
        NULL
    }
    BITS 00 30 81 89 02 81 81 00 c1 71 0a 15 4a 06 9f 69 1b 9f 14 0a c1 aa c5 ae 6e 1a 73 b4 60 13 da a0
         9b 30 cc 24 cb b0 d5 8b f9 96 66 58 5e d1 06 ef b1 69 85 15 ea 20 d1 e1 1b 43 79 83 85 97 13 d9
         40 cc c2 a4 9e 8f 50 c4 b3 a9 d7 b5 35 a7 5a 45 8e 81 e4 62 65 ff 60 a8 fa a9 87 89 84 9a 96 76
         ff 93 2c e8 31 f3 ef bf b0 e3 51 77 f4 c9 8a 19 bb 67 ae a4 9b 8c d7 50 a7 f8 ac 3f 78 93 15 9b
         1b da c1 f6 42 f1 1c dd 02 03 00 a9 39
}

$ ../test_pem test_priv_des.pem 

*** READING PEM test_priv_des.pem ***

----- RSA PRIVATE KEY -----

Encrypted, please type a password:
SEQUENCE {
    INTEGER 0 (decimal)
    INTEGER 00 c1 71 0a 15 4a 06 9f 69 1b 9f 14 0a c1 aa c5 ae 6e 1a 73 b4 60 13 da a0 9b 30 cc 24 cb b0 d5
            8b f9 96 66 58 5e d1 06 ef b1 69 85 15 ea 20 d1 e1 1b 43 79 83 85 97 13 d9 40 cc c2 a4 9e 8f 50
            c4 b3 a9 d7 b5 35 a7 5a 45 8e 81 e4 62 65 ff 60 a8 fa a9 87 89 84 9a 96 76 ff 93 2c e8 31 f3 ef
            bf b0 e3 51 77 f4 c9 8a 19 bb 67 ae a4 9b 8c d7 50 a7 f8 ac 3f 78 93 15 9b 1b da c1 f6 42 f1 1c
            dd
    INTEGER 43321 (decimal)
    INTEGER 7c 90 6f 7d 9d 5d 1c ae c8 cc bd 4f fb ee ad ba f0 8e c8 ee 7f 7f 56 84 38 fb d1 89 4a 3d 31 a4
            eb 0e ff 4c 9f e2 5d f2 3f c9 be 07 5c f3 d2 a1 28 6e 64 0d 84 f0 2c 50 4e a6 17 78 da d3 13 8e
            97 b4 48 5d de b8 d7 a6 b4 59 a7 09 28 b8 2e 9d f6 b1 3e 58 40 80 18 e3 9f 28 b9 49 a8 73 1c 57
            b1 bf 44 1a 25 8c b4 8b 3f 5b a0 16 89 ab 2b c5 b3 ce 54 5b 2a 9c 67 bf 47 b5 23 17 ae 79 99 09
    INTEGER 00 e0 55 19 e8 15 29 19 f3 33 02 f6 de 5c e6 ed 93 f9 ec 21 ae ee 76 ea 4d 0f 83 03 72 bc 1e 9e
            1b 0d 66 1d ac 1a 0a e1 7a 6e 7d ad 96 e4 d9 fb d3 56 fe ba 7f 76 d4 67 0f 8a 7c ca 2f ed 0b 4f
            3d
    INTEGER 00 dc bf 9b 92 0e 39 b4 32 c7 25 bc 26 9b 15 e1 c9 60 1d cb c3 33 96 be 1d 9d 8a 86 15 32 a2 98
            46 b4 64 fa b8 93 c9 4e f1 ac 9b 14 e6 03 de ea 72 0f 0c 04 63 82 2b 5c 89 5d b4 7e 21 59 29 de
            21
    INTEGER 25 d9 47 b0 c3 14 c6 36 2b 1f 89 42 3d ee c9 7a f7 9b 09 6c 70 87 42 a9 1a 7d b0 8d 15 bf 0b 8a
            8f 2b 6e 4e 2c 48 37 67 60 30 c0 e9 85 85 8b 2a 28 30 36 79 fd 08 17 6a 6a 0e 1a d1 ff 57 c5 75
    INTEGER 72 c7 88 09 3b 43 5e 52 51 17 9a 7a 32 f8 f6 cb ee 3c b3 04 52 88 15 a3 cd 01 b4 64 cf 17 01 fe
            99 bd e7 56 58 7b db f2 a2 ce 11 65 6a 1e 39 f0 2f 7f d0 e7 38 31 c9 cb 4a bc 4c 38 11 d6 27 a9
    INTEGER 00 a7 ec 25 23 4a 87 25 1e 22 92 49 42 87 3e f4 de 40 88 8b bd 28 fc 16 27 f4 0d 18 ca 56 b7 1a
            6c 07 80 a3 4c f0 74 29 da 14 fa be 9c ac e7 f5 35 2b fc 13 a7 23 e8 65 c7 b9 3b 39 ea 8a a6 5b
            4f
}

$ ../test_pem X25519.pem 

*** READING PEM X25519.pem ***

----- PRIVATE KEY -----

SEQUENCE {
    INTEGER 0 (decimal)
    SEQUENCE {
        OBJECT IDENTIFIER 1.3.101.110
    }
    BYTES 04 20 d0 20 cb f9 fc 65 51 ab f4 f3 82 05 14 17 81 fe 89 49 68 70 fd 62 5a d0 3c 24 64 26 98 bd
          58 69
}

$ ../test_pem biombo.pem 

*** READING PEM biombo.pem ***

----- CERTIFICATE -----

SEQUENCE {
    SEQUENCE {
        INTEGER 33 5b 24 db 3f 3d 59 5b f6 72 fd 09 7c 0b b6 9d 45 a6 a8 c4
        SEQUENCE {
            OBJECT IDENTIFIER 1.2.840.113549.1.1.11
            NULL
        }
        SEQUENCE {
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.6
                    PRINTABLE STRING "ES"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.8
                    UTF8 STRING "Cadiz"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.7
                    UTF8 STRING "Chipiona"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.10
                    UTF8 STRING "Internet Widgits Pty Ltd"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.3
                    UTF8 STRING "biombo"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 1.2.840.113549.1.9.1
                    IA5 STRING "guillermo@test.com"
                }
            }
        }
        SEQUENCE {
            UTC 20250503064121
            UTC 20280128064121
        }
        SEQUENCE {
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.6
                    PRINTABLE STRING "ES"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.8
                    UTF8 STRING "Cadiz"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.7
                    UTF8 STRING "Chipiona"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.10
                    UTF8 STRING "Internet Widgits Pty Ltd"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.3
                    UTF8 STRING "biombo"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 1.2.840.113549.1.9.1
                    IA5 STRING "guillermo@test.com"
                }
            }
        }
        SEQUENCE {
            SEQUENCE {
                OBJECT IDENTIFIER 1.2.840.113549.1.1.1
                NULL
            }
            BITS 00 30 82 01 0a 02 82 01 01 00 a1 e7 cf 8f c1 28 55 f7 9e 35 d5 50 ac 9f 33 66 50 15 57 d0 e6 b3
                 c7 32 bc 63 cf 64 56 65 1e a0 c0 74 ae f3 1e 80 b6 0b ca c5 7e ab 65 b0 be 46 c5 a2 f8 16 84 68
                 c4 8a 13 db ca bb af 13 c1 6d 3a f0 7f c5 87 82 0a 72 84 9d 71 ca 0d 2d d8 32 fd 51 20 9d dc a5
                 5e 77 37 cd 95 59 f4 af ec cf 60 77 1c 75 5d c2 ff 67 ad 37 a9 a3 f5 5d b8 fe c1 18 96 38 17 05
                 c5 7f 14 f9 1f 35 76 b3 0d 49 54 8b de d3 69 0d 3d 8e 3f 84 e7 3b 29 4c a2 60 95 9f e9 17 57 87
                 2b 45 ff 60 21 b7 eb 61 a9 98 a7 d4 52 49 dc b9 73 e3 b6 7f 9c 89 52 9b 2b 70 d0 29 30 a7 1a c7
                 9e ec e9 1c f4 47 9c 2f 23 8d 12 cd 7f 2a e9 ec 80 21 ac 27 10 2f ac 2c 72 10 fb 85 ef ef df 0e
                 7d e4 84 c1 7b 92 d9 ca 42 49 29 a7 fb 15 4d 05 31 ed 1f 25 16 7e e3 9d 67 a1 38 49 52 70 60 22
                 92 1d 4e 90 34 ad aa 7d e4 77 02 03 01 00 01
        }
    }
    SEQUENCE {
        OBJECT IDENTIFIER 1.2.840.113549.1.1.11
        NULL
    }
    BITS 00 14 2b d0 28 85 97 cd 26 6f e4 13 04 0d 67 c4 c9 2e a5 6c ce 75 3d 6f 0b 6c 08 eb 38 4e 0c c0
         e9 27 67 48 36 f5 24 4f ad f1 00 ce 90 1f d5 4a c4 3b e9 22 80 4d af 94 42 d5 a9 b2 a3 77 f2 67
         a2 45 23 75 d3 65 1d 2c f7 cd 31 69 d5 af f4 dd fd c8 d5 7b 6d 77 fd 46 4b 68 8c 0d c5 57 6e 80
         b9 8c 2d 03 59 43 fa 5b b5 7d b7 0a 2b 73 c5 8f 2d ba 79 2e cf 01 6b 96 9d d0 bc be a9 79 3c f2
         6a 7d 90 44 c2 51 92 f7 3f b8 a2 7f 58 8f 29 53 2f 35 49 a1 b5 e2 2b 2a 3b d0 fb 13 83 89 29 a2
         4b d2 f5 e8 c3 ea f5 51 a5 53 4b 4c de d4 60 0c 8e ae 71 bc 19 8f eb 1c e0 a4 6e 87 50 82 18 4d
         67 1e 96 bc 42 3a c7 5c b4 0d 89 d2 45 8c d8 94 77 b1 49 a6 03 97 af 8d 20 e1 94 74 1e c0 7f f2
         38 5e e2 81 54 4f 7d 5e 6e 95 17 af 7d 70 0a 2e b8 dd 84 d5 14 43 5c b7 9d ea 83 a6 f9 1d 94 69
         64
}

----- PRIVATE KEY -----

SEQUENCE {
    INTEGER 0 (decimal)
    SEQUENCE {
        OBJECT IDENTIFIER 1.2.840.113549.1.1.1
        NULL
    }
    BYTES 30 82 04 a3 02 01 00 02 82 01 01 00 a1 e7 cf 8f c1 28 55 f7 9e 35 d5 50 ac 9f 33 66 50 15 57 d0
          e6 b3 c7 32 bc 63 cf 64 56 65 1e a0 c0 74 ae f3 1e 80 b6 0b ca c5 7e ab 65 b0 be 46 c5 a2 f8 16
          84 68 c4 8a 13 db ca bb af 13 c1 6d 3a f0 7f c5 87 82 0a 72 84 9d 71 ca 0d 2d d8 32 fd 51 20 9d
          dc a5 5e 77 37 cd 95 59 f4 af ec cf 60 77 1c 75 5d c2 ff 67 ad 37 a9 a3 f5 5d b8 fe c1 18 96 38
          17 05 c5 7f 14 f9 1f 35 76 b3 0d 49 54 8b de d3 69 0d 3d 8e 3f 84 e7 3b 29 4c a2 60 95 9f e9 17
          57 87 2b 45 ff 60 21 b7 eb 61 a9 98 a7 d4 52 49 dc b9 73 e3 b6 7f 9c 89 52 9b 2b 70 d0 29 30 a7
          1a c7 9e ec e9 1c f4 47 9c 2f 23 8d 12 cd 7f 2a e9 ec 80 21 ac 27 10 2f ac 2c 72 10 fb 85 ef ef
          df 0e 7d e4 84 c1 7b 92 d9 ca 42 49 29 a7 fb 15 4d 05 31 ed 1f 25 16 7e e3 9d 67 a1 38 49 52 70
          60 22 92 1d 4e 90 34 ad aa 7d e4 77 02 03 01 00 01 02 82 01 00 09 de 5c ad cb d2 ee ea 83 9f 8f
          11 0b c9 e8 68 24 19 7e 46 22 c9 c7 d5 35 31 20 43 0d 6b bc 77 85 43 97 7b af 85 9e 3d b9 c3 af
          fa 37 dd e8 bb 32 46 17 63 c7 f9 05 81 cf be bb 42 15 64 84 35 be e7 49 e2 7b 10 02 1d ae 2c ff
          f2 96 3b 0f cd 36 83 96 14 09 39 a0 8d c3 be 54 36 d7 78 a1 b1 42 40 00 61 67 05 ab c1 0b 5d de
          89 3a 90 d6 90 85 ff 3e fe 21 93 c8 7b 51 31 48 d0 5e c6 80 83 64 41 9c 0b 86 9d 62 4c 38 02 9f
          11 20 0a 19 a6 7a fd bc ec 7c 0b 69 81 23 89 b9 e7 08 5a ed bc 5b 0d fd 89 3e 02 d0 ad 14 6f c0
          8a 04 17 f9 d8 39 7f 8a 59 d3 35 7c 1b 97 ab 25 da a1 af 9b da 7e 7f 7c b5 22 a6 0d 63 e8 0e 4d
          1f 0e bf f0 b3 91 8d 23 ca 93 5b 56 a6 bc b1 94 20 3d aa 6c a1 f5 a0 b3 cf ce dd 53 9b 3f 8c b4
          44 36 c1 f0 b7 03 db ea 62 39 aa b8 06 81 56 c8 51 25 5d d1 01 02 81 81 00 ce 86 a3 a2 f5 73 40
          77 25 5b 3b 94 0d f2 04 b8 ac 46 be 85 f4 47 1b 22 c8 f9 65 e6 36 3c 78 11 5f 3c d9 35 5d 76 72
          ff 59 ee b5 41 83 c0 79 cd 0d 13 74 40 64 87 c4 03 b0 22 05 9b b0 1c fc 5c 40 93 8a c5 f8 f8 0f
          d5 06 9f 3b 68 55 a2 e9 55 95 df 36 fd f9 b4 57 f2 40 12 93 67 9c 0b 5e 90 67 ec ae 80 f6 fc a4
          f6 9c f5 68 37 48 d7 69 4e dc 33 44 fd 86 c3 a1 c9 02 1a cf c3 47 2a 9f 7f 02 81 81 00 c8 b0 ca
          3d 9c 63 12 b0 ee 25 75 11 75 e4 6b 60 2e 1e 8e 0e 97 61 33 e7 96 c5 ea 40 fc 74 19 22 ea f6 48
          15 1f d0 3f 1a b8 5e 57 71 e4 99 e2 d8 2e ef 35 7a 59 ab 38 7e 00 27 2e 8e ba 89 ef db 64 e5 d7
          0b c6 8a 23 c8 b5 74 04 74 e3 c2 a5 e6 a8 f0 34 dc 96 b4 14 b9 51 23 67 8a 24 cb 8f 30 94 71 e4
          a4 92 07 b5 6d 97 35 96 a3 2e 5f a9 53 6f 09 dc 59 a3 90 63 c6 8a 27 87 f4 1b 46 37 09 02 81 80
          0d 14 bc 12 9b 47 54 82 f7 6b c2 e7 4a c6 d4 83 b9 ed b5 2d 98 de 54 1a ac ee 17 05 30 d4 3d aa
          a4 04 9f a1 60 ad 1a f5 8f 85 ae 17 5b 37 fa bf 01 20 05 51 fa cc 1a 9f 3d 2d ba 7b 34 f9 71 06
          8a d4 8d 8a a2 8d 5b bb 5b 73 04 79 44 4b 50 4a 2a 76 fb ed b4 3e 36 ff d2 0d 1b 27 09 dc 32 7e
          25 06 f9 ba 93 6a 47 2c 93 9e 7b 9e 5c cf c0 0c cd a7 b5 ab f5 38 38 ec 19 65 07 59 d0 0d 24 13
          02 81 81 00 a4 ed 34 a6 c1 38 62 09 e9 0b fb 53 28 11 59 74 a7 32 11 08 48 bb 67 0e 71 47 a0 a5
          5b 52 85 bd be 5e 3b 0a b1 24 50 57 c3 20 34 b8 b2 67 0c 97 20 ab 10 c5 c5 9f 02 43 fa 18 39 18
          ae 8f e0 ff e9 c1 40 a2 ba bc 97 7f c0 2e e1 e2 b5 b2 26 e8 91 f9 25 58 0a b9 5f 36 94 49 86 33
          6e dd a2 85 07 d9 39 4f 4d 7a ec 4c 6d b1 6a 64 1c 1e 1d 67 25 9f 72 e8 6f 94 9f 8a 6f ca e0 dd
          68 f4 a1 11 02 81 80 1a e4 2c 14 a0 18 bb c6 04 9b 0e d0 7b 1c 67 02 e0 5a 0d 6b a7 7a ec 3e 46
          3c de 86 5d eb 3c 08 c8 c8 57 cb 36 15 52 92 18 ac 8e 84 8a 29 9a 62 50 8a 48 4f 3a a5 3e d7 df
          0b ec 32 bb 5b 86 fd 8c b6 e1 c1 ce f4 80 fb 6c f1 f4 1b 41 9e ca 97 7f f0 d1 57 2c d6 7e 49 d0
          93 0f d4 ff b3 68 8b 29 0a b3 00 a5 cf 13 b8 3d 8c 0e 24 5e b5 e3 ee 5b f9 48 d2 c8 66 db 3e cc
          60 4d 73 7d b6 46 40
}

$ ../test_pem dsa-cert-2048.pem 

*** READING PEM dsa-cert-2048.pem ***

----- CERTIFICATE -----

SEQUENCE {
    SEQUENCE {
        INTEGER 3586 (decimal)
        SEQUENCE {
            OBJECT IDENTIFIER 1.2.840.113549.1.1.5
            NULL
        }
        SEQUENCE {
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.6
                    PRINTABLE STRING "JP"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.8
                    PRINTABLE STRING "Tokyo"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.7
                    PRINTABLE STRING "Chuo-ku"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.10
                    PRINTABLE STRING "Frank4DD"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.11
                    PRINTABLE STRING "WebCert Support"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.3
                    PRINTABLE STRING "Frank4DD Web CA"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 1.2.840.113549.1.9.1
                    IA5 STRING "support@frank4dd.com"
                }
            }
        }
        SEQUENCE {
            UTC 20120822072722
            UTC 20170821072722
        }
        SEQUENCE {
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.6
                    PRINTABLE STRING "JP"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.8
                    UTF8 STRING "Tokyo"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.10
                    UTF8 STRING "Frank4DD"
                }
            }
            SET {
                SEQUENCE {
                    OBJECT IDENTIFIER 2.5.4.3
                    UTF8 STRING "www.example.com"
                }
            }
        }
        SEQUENCE {
            SEQUENCE {
                OBJECT IDENTIFIER 1.2.840.10040.4.1
                SEQUENCE {
                    INTEGER 00 c6 e6 bb ab 92 75 95 91 c6 73 b9 3e d7 80 47 3e 48 9e b7 4c ee f2 5a 77 32 5c 63 d4 a9 7f 47
                            00 85 40 d6 c8 83 42 ad 40 e7 f2 1f 10 8d b3 22 03 17 7e 13 bb f9 37 ce 53 6d fb eb 5b ef e0 ee
                            56 1a 7b b0 c1 d2 e5 f4 d9 1c 36 f7 2a 0a f9 31 3f ce 73 53 36 ae 87 74 f9 ff 1f 89 65 ef bb 89
                            aa 99 16 81 0c 17 80 04 37 a8 32 91 bf de 90 69 37 eb 7d 25 d8 47 11 bd 36 f2 b3 50 1b 3d 4b b5
                            a8 6f 2e ca e8 f8 21 ef f1 26 81 e8 8e 1c a4 97 e3 d5 c3 fa 28 6f 90 54 52 15 d7 8b e6 a5 79 87
                            30 df 7a 97 e4 f0 68 65 d1 cc 8a f6 3a 4c ff 22 a5 46 e5 14 90 fd 69 48 cc 14 ac 2a e9 b7 24 69
                            aa a5 e6 6a 6d 14 7c 5b 85 84 c4 f6 48 9c 8d 4f 2a 36 49 f9 f2 65 be fc 95 b6 1c 54 b2 50 2c 90
                            3e 3b 79 78 eb d7 8e 1f 73 9d bf f3 ea ed bb ce b2 d0 4a 83 de 90 ee 29 b3 e6 dd 21 a2 77 fa dc
                            43
                    INTEGER 00 b0 ec 57 98 d1 04 1b c5 66 32 f8 c2 3c ff 49 b6 92 24 73 22 bb 80 f1 15 0f 70 4b 38 8a 5d 66
                            c3
                    INTEGER 78 5b 74 0b 7a 94 11 4d 68 51 eb d5 ef f9 f0 b9 24 b1 03 ca 78 30 f4 a1 1a 94 aa 2b 96 0e a8 e1
                            9f 61 01 23 1b 14 cc 04 0a 39 57 64 2c 4a 77 f3 45 8d 43 1b b4 f7 2b 30 2a 1a fc ab 9a a9 1b c7
                            2e cd 9b a9 ef 78 5f 95 51 e2 55 82 bf 44 a0 3a e9 ca 06 3e 7c 8e 75 6e 37 f3 a4 90 bb b8 71 26
                            50 fe 38 9a ff 23 e4 75 c3 2f c3 c6 63 17 bc e3 3d 71 95 86 1f 57 0b 44 14 f6 2a 55 7e ba de 19
                            a0 52 37 ed 33 38 32 98 e5 3c 90 23 ce 05 b1 91 59 35 eb 4e 4c 84 f4 1d 3d 92 7c 80 d1 19 86 8b
                            ff 4c 4c 7d 0c 74 ef 46 74 ed c7 53 4e e8 9b cc 91 d5 af 7f 2a 8c aa 84 0e 2c 00 53 28 d7 55 d1
                            3e 18 9d 79 64 3e 1d e7 a1 1f d2 a8 11 a7 30 e6 ba f5 23 3a db f5 b7 c1 eb d8 ed 87 4c 00 6a 44
                            cb 40 29 9b 0b 11 53 e3 c2 7a 4a a5 c4 ad 8b 17 e0 d2 e2 0f 19 52 0a 6d e8 f8 17 31 c7 a4 ee 9b
                }
            }
            BITS 00 02 82 01 01 00 93 51 69 a4 c5 7b 6e 77 b0 e5 66 fc 99 94 82 c3 b5 40 1e 46 23 98 4e 11 62 e9
                 0b c2 87 c6 0c 27 9d ff c6 41 46 89 2e b3 7c 4d 3e 21 5e 02 4c d8 27 6f 74 09 18 e9 eb 75 1e 94
                 fa 05 4a 33 d5 c6 13 82 c1 37 4e 82 2c 6c 7d d2 cc f2 6f 53 b8 f3 70 f3 af 63 58 d0 bf ec 55 06
                 b5 3c 61 dc 36 0f ca 72 75 b0 1c c3 f2 fa 25 99 c6 a4 50 6a 78 76 b5 88 50 10 73 40 7c 4e 85 c4
                 83 19 e6 18 07 4f 20 e0 0a 00 75 f4 4b 89 ba 0a b3 f9 7c ac 18 6e 6c ee f1 6b 96 3b 86 d1 aa f5
                 fb 40 9a 1f ae cd 91 68 61 44 4a e9 70 d1 5a cc 15 f7 c9 fe 0b 9f 1d 3b e1 0d a3 16 ed 34 82 2a
                 8d 6d 05 92 bd 49 d1 1b 58 24 0d da cb 67 7e 74 52 c9 91 50 fa fd 36 a3 24 2d 2e 0f b7 bd 2b 82
                 c7 83 d2 f4 33 3d fd 48 77 af d2 43 0c 7d c6 bd e5 0c 6e ac 3c 0e 6c ba 58 7f 62 57 7a 0e 26 df
                 1a 54 5f 05 e0 23
        }
    }
    SEQUENCE {
        OBJECT IDENTIFIER 1.2.840.113549.1.1.5
        NULL
    }
    BITS 00 6f 10 e8 24 cf 2f 29 57 e1 96 da 5f 1b 76 17 98 12 a7 a0 66 f6 52 97 4a a5 c8 e6 c4 cb 75 ff
         25 fc 95 62 5a 44 1c 14 0d e8 83 aa 35 42 24 68 bd 81 73 b6 48 30 2d 5e 5e c6 23 57 17 4e cf e5
         ed 09 66 a1 39 a7 73 a9 03 c2 a9 37 a6 5e a3 0b bd ab 40 7d c4 fc 5a 60 ee 10 64 49 62 fd dd 5d
         5d 6c b5 39 89 de b7 95 50 fc b1 41 14 2f 27 d6 14 69 37 2f e5 24 6b 9a 22 4a cc 70 66 17 48 6f
         f4
}

$ ../test_pem dsaparam.pem 

*** READING PEM dsaparam.pem ***

----- DSA PARAMETERS -----

SEQUENCE {
    INTEGER 00 87 e2 5b 39 04 70 60 45 f4 cf 76 c1 89 45 cc b9 cc 59 37 6f 2a fe 54 0b d0 0d bf 6e 1e 13 53
            04 31 45 52 a2 a0 8a 3c 63 d8 ca fe 25 6a 6c 41 f8 32 ad f9 ac 76 ec 6d a5 be 07 85 68 2e 62 6d
            f4 52 ad 38 ff 08 f3 76 81 80 d9 e5 5a a4 a7 bb b0 16 85 56 1e 71 66 ab b2 80 27 15 b9 10 f7 56
            90 fc 10 d2 a2 5d 1a f5 91 2c 3e dc 97 82 af 26 10 d7 e1 2e a7 57 44 f7 2f cb 4a 0b ca c9 b2 51
            05
    INTEGER 00 ac 2e 96 b2 39 2b d6 c5 bc df 8f 65 4d dc ec 59 23 82 c1 57
    INTEGER 12 dd 94 85 7c 77 20 ae 24 a9 b4 43 72 c9 dc 7c 4d 14 f5 4a 93 aa 72 e5 62 0d 13 f3 de 93 53 23
            fa b8 46 65 1f d5 d0 60 2d f8 4a 6b 8a 9d ea cf d4 8d 9c e9 04 6d ba 78 5a 3f 43 e0 44 21 ad 46
            03 48 f7 56 03 28 f4 9d f5 aa 9f 2e de b5 4e 51 44 f7 62 09 6d 5c 8e 1c 07 73 0a dd af b8 ad 90
            ff df 6d d2 fe 45 de 63 4d a5 8d 02 9d 4d f4 24 57 8c f9 8d 4f 1a dd 5f 62 e2 90 44 25 4f d4 e8
}

$ ../test_pem dsapriv.pem 

*** READING PEM dsapriv.pem ***

----- DSA PRIVATE KEY -----

SEQUENCE {
    INTEGER 0 (decimal)
    INTEGER 00 87 e2 5b 39 04 70 60 45 f4 cf 76 c1 89 45 cc b9 cc 59 37 6f 2a fe 54 0b d0 0d bf 6e 1e 13 53
            04 31 45 52 a2 a0 8a 3c 63 d8 ca fe 25 6a 6c 41 f8 32 ad f9 ac 76 ec 6d a5 be 07 85 68 2e 62 6d
            f4 52 ad 38 ff 08 f3 76 81 80 d9 e5 5a a4 a7 bb b0 16 85 56 1e 71 66 ab b2 80 27 15 b9 10 f7 56
            90 fc 10 d2 a2 5d 1a f5 91 2c 3e dc 97 82 af 26 10 d7 e1 2e a7 57 44 f7 2f cb 4a 0b ca c9 b2 51
            05
    INTEGER 00 ac 2e 96 b2 39 2b d6 c5 bc df 8f 65 4d dc ec 59 23 82 c1 57
    INTEGER 12 dd 94 85 7c 77 20 ae 24 a9 b4 43 72 c9 dc 7c 4d 14 f5 4a 93 aa 72 e5 62 0d 13 f3 de 93 53 23
            fa b8 46 65 1f d5 d0 60 2d f8 4a 6b 8a 9d ea cf d4 8d 9c e9 04 6d ba 78 5a 3f 43 e0 44 21 ad 46
            03 48 f7 56 03 28 f4 9d f5 aa 9f 2e de b5 4e 51 44 f7 62 09 6d 5c 8e 1c 07 73 0a dd af b8 ad 90
            ff df 6d d2 fe 45 de 63 4d a5 8d 02 9d 4d f4 24 57 8c f9 8d 4f 1a dd 5f 62 e2 90 44 25 4f d4 e8
    INTEGER 12 ec 76 41 4b ba 56 d5 35 1c 51 91 87 e8 ad 88 21 c0 24 d2 49 c0 a3 3b b9 bb 02 99 69 32 55 ed
            0a 39 6c 51 1c 6a 19 e9 3c 94 3f 75 cf 50 ee f3 a5 86 03 84 b7 d6 66 8b f7 f2 f5 d6 40 76 a3 ae
            4a c5 5e c0 16 bd 48 43 73 1d 48 dd 47 4d a9 e8 b4 1e 59 d0 bb 5c 7e 28 62 44 42 70 5c 5f 6a c1
            f0 51 34 db af 1a 48 94 07 46 de 18 02 89 44 ac 1c 0d 7a d8 b6 84 67 14 b8 4b b5 e8 8c dd 1d 53
    INTEGER 6d fa e9 48 15 50 be 97 32 26 32 83 1a 09 78 90 70 12 1e 46
}

----- DH PARAMETERS -----

SEQUENCE {
    INTEGER 03 11 b7 9b b7 7b fa 91 a5 8b 34 38 f6 d0 04 f9 37 e3 2d 49 84 2c 2d 9f c2 19 71 15 5c 79 86 fc
            19 9e 92 3a
    INTEGER 01 0d 1d 41 a5 48 9c e5 99 62 0c 3a 7d f8 fc 1e 49 11 76 7f 38 04 f3 89 46 fa 3d 14 65 cb 29 73
            f7 e8 dd 61
}

$ ../test_pem dsaparam.pem 

*** READING PEM dsaparam.pem ***

----- DSA PARAMETERS -----

SEQUENCE {
    INTEGER 00 87 e2 5b 39 04 70 60 45 f4 cf 76 c1 89 45 cc b9 cc 59 37 6f 2a fe 54 0b d0 0d bf 6e 1e 13 53
            04 31 45 52 a2 a0 8a 3c 63 d8 ca fe 25 6a 6c 41 f8 32 ad f9 ac 76 ec 6d a5 be 07 85 68 2e 62 6d
            f4 52 ad 38 ff 08 f3 76 81 80 d9 e5 5a a4 a7 bb b0 16 85 56 1e 71 66 ab b2 80 27 15 b9 10 f7 56
            90 fc 10 d2 a2 5d 1a f5 91 2c 3e dc 97 82 af 26 10 d7 e1 2e a7 57 44 f7 2f cb 4a 0b ca c9 b2 51
            05
    INTEGER 00 ac 2e 96 b2 39 2b d6 c5 bc df 8f 65 4d dc ec 59 23 82 c1 57
    INTEGER 12 dd 94 85 7c 77 20 ae 24 a9 b4 43 72 c9 dc 7c 4d 14 f5 4a 93 aa 72 e5 62 0d 13 f3 de 93 53 23
            fa b8 46 65 1f d5 d0 60 2d f8 4a 6b 8a 9d ea cf d4 8d 9c e9 04 6d ba 78 5a 3f 43 e0 44 21 ad 46
            03 48 f7 56 03 28 f4 9d f5 aa 9f 2e de b5 4e 51 44 f7 62 09 6d 5c 8e 1c 07 73 0a dd af b8 ad 90
            ff df 6d d2 fe 45 de 63 4d a5 8d 02 9d 4d f4 24 57 8c f9 8d 4f 1a dd 5f 62 e2 90 44 25 4f d4 e8
}

$ ../test_pem dsapriv3des.pem 

*** READING PEM dsapriv3des.pem ***

----- DSA PRIVATE KEY -----

Encrypted, please type a password:
SEQUENCE {
    INTEGER 0 (decimal)
    INTEGER 00 87 e2 5b 39 04 70 60 45 f4 cf 76 c1 89 45 cc b9 cc 59 37 6f 2a fe 54 0b d0 0d bf 6e 1e 13 53
            04 31 45 52 a2 a0 8a 3c 63 d8 ca fe 25 6a 6c 41 f8 32 ad f9 ac 76 ec 6d a5 be 07 85 68 2e 62 6d
            f4 52 ad 38 ff 08 f3 76 81 80 d9 e5 5a a4 a7 bb b0 16 85 56 1e 71 66 ab b2 80 27 15 b9 10 f7 56
            90 fc 10 d2 a2 5d 1a f5 91 2c 3e dc 97 82 af 26 10 d7 e1 2e a7 57 44 f7 2f cb 4a 0b ca c9 b2 51
            05
    INTEGER 00 ac 2e 96 b2 39 2b d6 c5 bc df 8f 65 4d dc ec 59 23 82 c1 57
    INTEGER 12 dd 94 85 7c 77 20 ae 24 a9 b4 43 72 c9 dc 7c 4d 14 f5 4a 93 aa 72 e5 62 0d 13 f3 de 93 53 23
            fa b8 46 65 1f d5 d0 60 2d f8 4a 6b 8a 9d ea cf d4 8d 9c e9 04 6d ba 78 5a 3f 43 e0 44 21 ad 46
            03 48 f7 56 03 28 f4 9d f5 aa 9f 2e de b5 4e 51 44 f7 62 09 6d 5c 8e 1c 07 73 0a dd af b8 ad 90
            ff df 6d d2 fe 45 de 63 4d a5 8d 02 9d 4d f4 24 57 8c f9 8d 4f 1a dd 5f 62 e2 90 44 25 4f d4 e8
    INTEGER 00 82 68 ce e9 e9 c1 19 bc 35 0d 7b be cc 77 29 af ff 49 49 64 63 e7 ef 9b 4f b8 73 13 cf 44 ef
            51 be 8c 98 02 a7 26 22 95 58 b0 8e b9 6c 3f 44 39 b7 11 b7 21 e9 67 de e9 49 2e 35 55 97 c6 2f
            7f b7 9d 62 be 0f 06 11 a0 18 aa ad 59 b9 b9 f2 fe 0c 9f 98 c4 8b df d9 16 eb c7 77 6c 5d 5e a2
            bb 06 de 65 cb a3 f9 31 bf 85 d2 47 17 14 b1 eb 50 04 55 db c4 96 95 3a 55 8d 95 2f 65 71 73 58
            0d
    INTEGER 37 d6 64 c7 8a 37 d6 b2 0d 02 53 1a 1f 2f 01 71 d7 2d 91 41
}

                     
```




