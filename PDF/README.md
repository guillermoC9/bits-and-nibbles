# PDF Generator for C
## Generate your own PDF files from a C program
### Written by Guillermo Amodeo Ojeda

In order to write this, I used FPDF.PHP by Olivier Plathey as guide (version 1.82 dated from 07/12/2019) and some extra scripts from his site (http://www.fpdf.org). Some of the scripts were written by Olivier himself and other from people like Christopher Prugnaud. We all owe Olivier and its contributors a big thank you for his code, and specially for making it freely available and modifiable, which have forked many versions and portings, including this one here.

The generated PDF 'code' have been fine-tuned using PDF specs and what I have been learning about PDF lingo while writting this thingy.

This code forces de internal encoding of the PDF to CP1252 enconding (aka Windows 1252) while externally using wchar_t to ease the font handling. This has been done like that so in the future it can be made to support languages not supported by CP1252 without having to change the public API functions, just the internals.

This distribution here includes parts of an old version (1.2.6) of Zlib by Mark Adler and Jean-loup Gailly. Yeah quite old in fact (2012), but good enough to add deflating/inflating support on the PDF and PNG Files. The reasons for using this particular version and not other are its permissive license, and because all the code is in C.

In order to support the embedding of arbitrary TrueType font files, we also include part of the FreeType-1 library -actually version 1.4-, which the authors of it (David Turner, Robert Wilhelm, and Werner Lemberg) let anyone use under 3 conditions that we reproduce here:

- We don't promise that this software works. However, we will be interested in any kind of bug reports. (`as is' distribution)
- You can use this software for whatever you want, in parts or full form, without having to pay us. (`royalty-free' usage)
- You may not pretend that  you wrote this software. If you use it, or only parts of it,  in a program,  you must acknowledge somewhere in  your documentation that you have used the FreeType code. (`credits')

And we do comply with them all. :-)

Note that even if this is an old and unmantained version of the library, it is still a very capable TTF engine, despite its limitations, specially for small proyects wanting to enable support for TTF fonts. As said, FreeType-1 is not longer mantained in favour of FreeType-2, but you can still play with it, specially now that the patents on the rasterizer -that forced the authors to move away from it- have expired. The full library is still available at:

- https://freetype.org/freetype1/index.html 
 
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

The program outputs a multiple page PDF file with compression ON (test_pdf.pdf) and one with compression OFF (test_pdf_uncompressed.pdf) so you can look at their size and content to see the differences. 
