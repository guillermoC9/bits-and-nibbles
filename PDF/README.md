# PDF Generator for C
## Generate your own PDF files from a C program
### Written by Guillermo Amodeo Ojeda

In order to write this, I used FDPF.PHP by Olivier Plathey as guide (version 1.82 dated from 07/12/2019) and some extra scripts from his site (http://www.fpdf.org). Some of the scripts were written by Olivier himself and other from people like Christopher Prugnaud. We all owe Olivier and its contributors a big thank you for his code, and specially for making it freely available and modifiable, which have forked many versions and portings, including this one here.

The generated PDF 'code' have been fine-tuned using PDF specs and what I have been learning about PDF lingo while writting this thingy.

This code forces de internal encoding of the PDF to CP1252 enconding (aka Windows 1252) while externally using wchar_t to ease the font handling. This has been done like that so in the future it can be made to support languages not supported by CP1252 without having to change the public API functions, just the internals.

This distribution here includes parts of an old version (1.2.6) of Zlib by Mark Adler and Jean-loup Gailly. Yeah quite old in fact (2012), but good enough to add deflating/inflating support on the PDF and PNG Files. The reasons for using this particular version and not other are its permissive license, and because all the code is in C.

In order to support the embedding of arbitrary TrueType font files, we also include part of an old version of the FreeType-1, actually version XXX, which the authors of it (David Turner, Robert Wilhelm, and Werner Lemberg) let anyone use it under 3 conditions:
    - We don't promise that this software works. However, we will be interested in any kind of bug reports. (`as is' distribution)
    - You can use this software for whatever you want, in parts or full form, without having to pay us. (`royalty-free' usage)
    - You may not pretend that  you wrote this software. If you use it, or only parts of it,  in a program,  you must acknowledge somewhere in  your documentation that you have used the FreeType code. (`credits')

    And we do comply with them all. :-)
 



