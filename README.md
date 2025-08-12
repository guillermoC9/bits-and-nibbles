# bits-and-nibbles
Bits and nibbles of code from ideas, articles, conferences and someday books.
Each directory is an independent piece of code that can be used separately.
I know this causes code duplication, but the idea here is to grab whatever you are interested on without dependencies, as the code should be easy to include in your own proyects. Just remember that the duplication always bear smaller functionality than the one in their own directory just to avoid making the repository too big.

## Directory Content:
- ### Chipiona:
  The first SHAX cipher from Guillermo Amodeo Ojeda, intented for ciphering big amounts of data-in-motion. Although it can cipher small amounts of data-at-rest too.
- ### Hashes & HMACs:
  Criptographic Hashes & HMACs are extremely important, thus here is an one-stop shop to use many of them easily. Some very secure, some considered broken. There are a couple of good reasons to have an easy way to handle them from C.
- ### Ciphers:
  Just like the one-stop shop for criptographic hashes, here you have a one-stop shop for symmetric criptographic cipher primitives. Note that my Chipiona cipher is not included becuse this is for well-known ciphers. Although you can easily add it if you like. ;-)  
- ### Entropy:
  Pseudo-random algorithms require unpredictable entropy for seeding. Here is an easy way to obtain it.
- ### Random:
  Pseudo-random number generators for general purpose or criptography.
- ### Primes:
  Generate quality prime numbers is really important. Let's do it taking advantage of our random stuff.
- ### RSA:
  The infamous Public Key algorithm that many people assume broken by *'Quantum computers'*, although none of them proved it completely broken. I think it still has few tricks up the sleeve.
- ### mpint:
  Multi-precision number support.
- ### Time (time & ticks):
  Measure time acurately is extremely important in computing. Let's have a look at ticks and learn how to manage time better. You will also find useful time utilities here, like calendar sheets.
- ### CSV:
  Data is the most important thing in computing, as that is what computers do: process data. CSV (Comma Separated Values) files are a **very** popular way to share data. Here is a very easy way to handle them. And not only CSV files, also other kind of separator based files like TSV (Tabulator Separated Values) in two encondings like Windows 1252 and UTF-8.  
- ### Checksum: 
  There are some checksums which are very uself for everyday life, like the Luhn's checksum or the Spanish control letter (letra del DNI, NIF o NIE).
- ### PDF:
  Wouldn't you like to create full fledged PDF files from C?

## Licensing:
The plan is that ALL code here would be released as public domain so people can use it for whatever they please, hopefully for the benefit of many people. Although in some point some code may be released under another license, just check the license at the top of each source code file to be sure what it is. 

Of course everything here is **free of cost**. But if you are making profit from stuff you got from here, it would be good to donate some if you can afford to, just to square the paradox of you making profit from work that we give away for free. It could be to someone in need or to charity, whatever you think is fair. And again: only if you can afford to, as if you can make a decent living from our stuff here, it would be good enough for us.

## Disclaimer:
Most of the code here have been throughly tested, and some just functionally -not stressed- as has been modernized for publishing. Therefore, you will find bugs. Please report them to me and accept my apologies, as no programmer is free of them due to the nature of what we do: compulsory write thousands of lines of code. Of course I disclaim any liability for problems that may arise with code from here, whatever it comes from me or from someone else. The code here is provided 'as is' for the purpose of information and -hopefully- usefulness.

## To build all the tests:

Each of them has its own makefile (read the *README.md* in each dir to know how to build each of them, but to build or clear them all from here, do:

For Microsoft C/C++ for Windows (Visual Studio C++) use:  

```
nmake /f msc.mak buildit
```

For Macintosh, Linux and others using GNU C:

```
make -f gcc.mak buildit
```

Please note that some test may need extra steps, read the output messages.

### To clean all the builds:

Do the same that to build them but use *cleanit* instead of *build it* as the target.
