# RSA
## RSA public key criptography

Most of the credit should be for Simon Tatham, as this code took base on SSHRSAG.c SSHRSA.c that came with Putty v0.60, and even if it is largely changed, the core thing is still his. ;-)

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

$ ./test_rsa

                     
```




