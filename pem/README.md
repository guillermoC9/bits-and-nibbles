# PEM  
## Privacy Enhanced Mail Files

PEM files were originally described in RFC-1421 and where intended as a way to store encrypted electronic mail. Afterwards, when standards like: PKIX, PKCS and CMS appeared, the use of PEM was widespread as these standards used them to store certificates, keys etc, as well as the  tool ```openssl``` made really easy to produce them.

Here we present a library that allow to read/write these files easily, although this is not trivial business, as there is many types of PEM files, as well as a wide variety of technologies involved to produce them: ciphers, hashes and ASN.1 encoding.

Yeah, the content of PEM files is always enconded using ASN.1 and thus we include a library that handles all kind of ASN.1 elements, especially OID. OID stands for *Object IDentifier* and is a very important element in PEM files, ASN.1 and specially X509 certificates. The term OID was actually created for the standard X500, the base of most internet services: Directory Services. Which allow users to get information on resources. For example: LDAP. X500 OID fields are used extensively everywhere, like in X509 Certificates or PKCS in order to identify algorithms and signatures.

The test program allows to show PEM files in ASN.1 notation format, even if they are encrypted. Thre is a bunch of them in the directory ```test``` that you can use to play around. The password for the encripted ones is **```test```**.

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
$ ./test_ciphers


== CRYPTO TEST END
                     
```




