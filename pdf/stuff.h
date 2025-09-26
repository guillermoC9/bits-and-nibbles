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

#ifndef STUFF_FOR_PDF
#define STUFF_FOR_PDF

/** Essential headers */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <wchar.h>
#include <time.h>
#include <locale.h>

#include "zlib/zlib.h"

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

/* --------------------------------- *
   Detect wchar_t's encoding 
 * --------------------------------- */

#ifndef WCHAR_MAX
#error "Humm, this compiler doesn't define WCHAR_MAX. We need the value to determine the encoding."
#endif

#if WCHAR_MAX < 0x110000
#define WCHAR_UTF_16
#endif

/* Unicode Replacement Char. */

#define WCHAR_REPLACEMENT     0xfffd

/* Functions*/

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Return the value of bit x in a buffer of 'len'
   bytes (0 or 1).

   Note that bit x should be in the range from 
   (len * 8 - 1) for the top bit (bit 7 of byte 0)
   or most significative, to 0, which is the first
   bite of byte len - 1 (least significative).
 * -------------------------------------------------- */

int mem_get_bit(void *b,size_t len,int x);

/* -------------------------------------------------- *
   Set the value of bit x in a buffer of 'len'
   bytes (0 or 1).
   The same bit range than the previous apply
 * -------------------------------------------------- */

void mem_set_bit(void *b,size_t len,int x,int val);

/* -------------------------------------------------- *
   Count the size in bits of a a value stored in a
   buffer of 'len' bytes.

   Basically returns the most significant bit in b
   which is set to 1, plus 1.

   You can considered it an unoptimized function 
   to count bits for a data type with an arbitrary 
   size that stores the bytes in big endian.
 * -------------------------------------------------- */

int mem_bits(void *b,size_t len);

/* --------------------------------------------------------------------- *	

    This function converts a wide character (wchar_t) on an unicode
    char encoded in UCS-4 (UTF-32) and returns how many wchar_t
    elements it used from 'wch'. The reason why more than one could
    be used is that the wchar_t string could be enconded in UTF-16.

    Basically converts an unicode char from the BMP (one wchar_t element)
    or an UTF-16 surrogate pair (two wchar_t elements) into an unicode
    value in the range 0x000000 - 0x10FFFF, then returns one of these
    values:

    	0 - Which means that 'wch' points to end of a string
    	    (NUL char = 0), so the function copies the NUL onto
    	    'resp' and returns 0 so the caller knows is the end
    	    of the string.

    	1 - It used just one wchar_t from 'wch'

    	2 - It used 2 wchar_t from 'wch' becuase they were a
    	    UTF-16 surrogates pair.

	If the function finds an incomplete UTF-16 surrogate sequence
	-that is the first wchar_t is a surrogate code but the second isn't-,
	the function puts WCHAR_REPLACEMENT in 'resp' and returns 1 so
	the caller can keep processing chars.

    Parameters:

		'resp' - pointer to destination string
		'wch'  - pointer to the wchar to convert
		'max'  - maximum number of chars that can be read from 'wch'

 * ------------------------------------------------------------------------ */

int wchar_to_ucs4(unsigned int *resp,const wchar_t *wch,size_t max);

/* ------------------------------------------------------ *
	This function does the opposite of the previous
	function, as it converts an unicode char in	the
	UCS-4 (UTF-32) encoding to an wchar_t char encoded
	using the systems encoding, and then returns one of
	these values:

	    0 - Same as previous, 'ucs4' is 0, so the char
	        is copied over 'resp' and 0 returned.

   		1 - One wchar_t written.

   		2 - The encoding of wchar_t in the system is
   		    UTF-16 and the char had to be encoded using
   		    surrogates.

	If the UCS-4 is incorrect (outside of the permitted
	ranges), the func puts WCHAR_REPLACEMENT in 'resp'
	and returns 1.

	IMPORTANT: It is assumed that 'resp' can take 2
	wchar_t chars.

  	parameters:

		'resp' - pointer to response buffer
		'ucs4' - unicode char to be converted

 * ------------------------------------------------------ */

int ucs4_to_wchar(wchar_t *resp,unsigned int ucs4);

/* ------------------------------------------------------ *

   The rest of conversion functions here are modelled on
   the previous ones, but using other encodings and
   returning bigger values.

    Eg. these use a 16 bits type because UTF-16 has 16 bits,
    and also returns 0,1 or 2, although it will return  2
    more often than the previous as any value over 0xffff
    will generate surrogate pairs.

 * ------------------------------------------------------ */

int utf16_to_ucs4(unsigned int *resp,const unsigned short *utf16,size_t max);
int ucs4_to_utf16(unsigned short *resp,unsigned int ucs4);

/* ------------------------------------------------------ *

   Another set of functions but using an 8 bits type and
   codify in UTF-8, so they can return 0,1,2,3 or 4
   depending on how many bytes the UTF-8 sequence had.

    If the sequence is overlong (Eg: encode 0x80 in 3
    bytes instead of 2) or the resulting UCS-4 char is
    outside the unicode ranges (over 0x10ffff or inside
    the range 0xd800 - 0xdfff), utf8_to_ucs4() will
    write WCHAR_REPLACEMENT in 'resp' and return the
    length of the sequence to be ignored (or 0 if the
    sequende is longer than 'max')

    IMPORTANT: ucs4_to_utf8() assumes that 'resp' can 
               take upto 4 chars.
 * ------------------------------------------------------ */

int utf8_to_ucs4(unsigned int *resp,const unsigned char *utf8,size_t max);
int ucs4_to_utf8(unsigned char *resp,unsigned int val);

/* ------------------------------------------------------ *
   This returns the number of chars in a UTF-8 string.

   The function will stop parsing if finds an erroneus
   sequence (eg. 5,6 bytes long) or a NUL char before
   checking 'max' bytes.
 * ------------------------------------------------------ */

size_t utf8len(const unsigned char *ch,size_t max);

/* ------------------------------------------------------ *
   Convert from utf-8 to utf-16 and viceversa.

   Works like the previous, but returns the number of 
   chars written in the destination.
 * ------------------------------------------------------ */

size_t utf8_to_utf16(unsigned short *utf16,size_t max,const unsigned char *utf8,size_t num);
size_t utf16_to_utf8(unsigned char *utf8,size_t max,const unsigned short *utf16,size_t num);

/* ------------------------------------------------------ *
    Convert Windows 1252 to wchar_t and viceversa

    If the wchar character is out of the range of the
    1252 encoding, wchar_to_win1525() will return '?'
    and if win1252_to_wchar() detects a non-encodable
    char it will return WCHAR_REPLACEMENT
 * ------------------------------------------------------ */

wchar_t win1252_to_wchar(char ch);
char    wchar_to_win1252(wchar_t wc);

/* -------------------------------------------------- *
   Read a whole file in memory

   It returns a pointer to the file in memory and
   if 'tam' is not NULL it puts the size of the 
   file (i.e. the size of the allocated buffer).

   Use free() on the returned buffer to free it
 * -------------------------------------------------- */

void *read_whole_file(const char *file,size_t *tam);
void *read_whole_filew(const wchar_t *file,size_t *tam);

/* -------------------------------------------------- *
   Compress/decompress a memory buffer using the ZIP
   algorithm, and return a buffer with the compressed
   or decompressed data.

    If not NULL, ret will receive the resulting size.

    Level is the compression level being 0 for default
    and 1 to 9 where 1 is best speed and 9 best
    compression.

    The default level will suffice for most uses.
 * -------------------------------------------------- */

void *zcompress(const void *data,size_t tam,size_t *ret,int level);
void *zuncompress(const void *data,size_t tam,size_t *ret);

/* -------------------------------------------------- *
   Same as read_whole_file() but compressing the file
   using the ZIP algorithm, so the buffer is a
   compressed version of the file in disk.

    If not NULL, comp will have compressed size and
    orig the original size after the call
 * -------------------------------------------------- */

void *read_compressed(const char *file,size_t *tam,size_t *orig);
void *read_compressedw(const wchar_t *file,size_t *tam,size_t *orig);


/* -------------------------------------------------- *
   These funcitons are used to conert values from/to
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

/** 16 bits big endian */

unsigned short int get_be16(const void *buf);
void               put_be16(void *buf,unsigned short int val);

/** 16 bits little endian */

unsigned short int get_le16(const void *buf);
void               put_le16(void *buf,unsigned short int val);

/* -------------------------------------------------- *
   Change the case of strings.

	Like strncpy(), 'max' is the lenght of the string
	to be lowered or uppered, thus the buffer is 
    assumed 'max' + 1 chars, so the NUL is always added.

	'orig' and 'dest' can point to the same buffer.

	'max' can be 0 to say 'whole string', but you
	 need to be sure that 'dest' has enough
	 space.

   Trim the string 's' and returns it (s is MODIFIED)
* -------------------------------------------------- */

size_t strlower(char *dest,const char *orig,size_t max);
size_t strupper(char *dest,const char *orig,size_t max);

char *strtrim(char *s);

/* -------------------------------------------------- *
    This are realloc() and strcat() together in one 
    function only, which also allows to specify an 
    optional separator to use in the concatenation.

    If 'clen' (number of chars from 'cat' to use) has
    the value 0, it then uses all the characters in 
    'cat'.

    Result is orig + [sep] + (clen chars from cat)
 * -------------------------------------------------- */

char *realloc_strcat(char *orig,const char *cat,size_t clen,char sep,size_t *nlen);

/* -------------------------------------------------- *
   malloc() and sprintf() in one go for both char 
   and wchar strings.

   They put in len (if not NULL) the number of 
   chars/wchars  of the allocated string.

   Due to the intrinsics of swprintf(), the 
   functions wcsallocf() and wcsallocfv() have a 
   limitation of 32727 wchars in the resulting 
   string. The char functions do not have that 
   limitation.

   strtowcs() allocs memory and convert 'len' chars 
   from the string to wchar (assuming it is either 
   ascii or utf-8). It doesn't have size limitations
   but will copy less chars if NUL char is found. 
   Send 'len' as 0 for converting the whole string.

   Use free() to reclaim the memory
 * -------------------------------------------------- */

wchar_t *wcsallocf(size_t *len,const wchar_t *fmt,...);
char    *strallocf(size_t *len,const char *fmt,...);

wchar_t *wcsallocfv(size_t *len,const wchar_t *fmt,va_list vl);
char    *strallocfv(size_t *len,const char *fmt,va_list vl);

wchar_t *strtowcs(const char *str,size_t len);
char    *wcstostr(const wchar_t *str,size_t len);

/* -------------------------------------------------- *
   strlcpy() is a better version of strncpy() as
   always adds the NUL. Like is only for Mac, we add
   a version for the other OS
 * -------------------------------------------------- */

#ifndef FOR_MAC

size_t strlcpy(char *s1, const char *s2, size_t max);

#endif

#ifdef __cplusplus
};
#endif



#endif
