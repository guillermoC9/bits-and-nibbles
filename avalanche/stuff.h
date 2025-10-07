/*
    stuff.h

    Assorted stuff to support the PDF code

    (CC) Creative Commons 2018-2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

	  - Originally written by Guillermo Amodeo Ojeda

     
                                --oO0Oo--
*/

#ifndef EXTENSIONS_OF_STRING_H
#define EXTENSIONS_OF_STRING_H

/** Essential headers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <wchar.h>
#include <time.h>
#include <locale.h>

#ifndef FALSE

#define FALSE   0
#define TRUE    1

#endif


/* --------------------------------- *
   Make sure unsigned int is at 
   least 32 bits long 
 * --------------------------------- */

 #if UINT_MAX <  0xffffffffu
#error "unsigned int is expected to have at least 32 bits"
#endif

/* ---------------------------------------------- *
   Detect target operating system. If not Mac or 
   Windows, asume Linux or UNIX (BSD, AIX,...)   
 * ---------------------------------------------- */

#if defined(__APPLE__)

#define FOR_MAC     /* This is for Macintosh */

#elif defined(_WIN32)

#define FOR_WIN     /* This is for Windows */

#else 

#define FOR_NIX      /* This is for Unix-like */

#endif

/* --------------------------------- *
   We need fixed types for some suff 
 * --------------------------------- */

#ifdef FOR_WIN

typedef unsigned __int64    u64_t;

/* Microsoft C has different names than the rest for useful functions */

#define snprintf        _snprintf
#define strcasecmp      _stricmp
#define strncasecmp     _strnicmp

#else 

typedef unsigned long long  u64_t;

#endif

/* Functions*/

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   These functions are used to conert values from/to
   varios sizes an endians.

   The format to recognize the functions are:
   
   get = read, put = write.
   be = Big Endian, le = Little Endian.

   and then the number of bits, so:

    get_be32() will read 4 bytes in big endian 
    format and conver it to a 32 bits value in the 
    endian of the current machine.

    put_be32() will do the opposite, writting a 32
    bit value in the local endian to 4 bytes in
    big endian.
 * -------------------------------------------------- */

/* 32 bits big endian */

unsigned int get_be32(const void *buf);
void         put_be32(void *buf,unsigned int val);

/** 32 bits little endian */

unsigned int get_le32(const void *buf);
void         put_le32(void *buf,unsigned int val);

/** 64 bits little endian */

u64_t get_le64(const void *buf);
void  put_le64(void *buf,u64_t val);

/** 64 bits big endian */

u64_t get_be64(const void *buf);
void  put_be64(void *buf,u64_t val);

#ifdef __cplusplus
};
#endif



#endif
