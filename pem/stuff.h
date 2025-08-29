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
#include <wctype.h>
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

/* --------------------------------- *
    Broken down Point on time    
 * --------------------------------- */

typedef struct
{
    int local;  /* TRUE/FALSE time is local time */

    int  year;  /* Full year: 1946, 2006, 2025, etc.. */    
    int  mon;   /* Month: 1 - 12 */
    int  day;   /* Day: 1 - 31 */
    int  hour;  /* Hour: 0 - 23 */
    int  min;   /* Minute: 0 - 59 */
    int  sec;   /* Second: 0 - 59 */
    int  msec;  /* Millisecond: 0 - 999  */
    int  usec;  /* Microsecond: 0 - 999 */    

    int  wday;  /* Day of the week: 0 - 6 (where 0 is Sunday) */
    int  leap;  /* TRUE/FALSE year was a leap year */
} point_on_time_t;

/* Functions*/

#ifdef __cplusplus
extern "C" {
#endif

 /* -------------------------------------------------- *
   Same as memset() but instead of setting each byte 
   to 'c', it XOR, OR or AND c with each of the 'len'
   bytes of 'buf'.
 * -------------------------------------------------- */

void *memxor(void *b,int c,size_t len);

/* -------------------------------------------------- *
   Same as memmove() but instead copy it makes XOR
   of 'len' bytes of 'src' with the same number of bytes
   in 'dst' and stores them in 'dest'.

        'dst' = ('dst' XOR 'src')

    If 'dst' and 'src' point to the same buffer, all bits will become 0.

    return 'dst'.
 * -------------------------------------------------- */

void *mem_xor(void *dst,const void *src,size_t len);

/* ---------------------------------------------------- *
   Shift all the bits of a buffer one bit right or left
 * ---------------------------------------------------- */

#define S9_RSHIFT   0
#define S9_LSHIFT   1

void *mem_shift(void *buf,int len,int dir);

/* -------------------------------------------------- *
    Implementation of the GCM multiplication

        z = GF(x,y).

    Based on the implementation algorithm 1
    on page 9 of the document:

        The Galois/Counter Mode of Operation (GCM)
        David A. McGrew & John Viega
        NIST 2005

    http://csrc.nist.rip/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf

    The maximum block size is 128 bytes (1024 bits),
    so if you send a bigger block only the first 1024
    bits would be used. This is a huge block, as most
    GCM implementations like AES-GCM use 256 bits
    maximum.

    Note that z, x and y can ALL point to the
    same memory
 * -------------------------------------------------- */

#define GCM_MAX_BLK_SIZE    128

void gcm_mult(unsigned char *z, const unsigned char *x,const unsigned char *y,size_t block_size);

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

/** 64 bits little endian */

u64_t get_le64(const void *buf);
void  put_le64(void *buf,u64_t val);

/** 64 bits big endian */

u64_t get_be64(const void *buf);
void  put_be64(void *buf,u64_t val);

/* -------------------------------------------------- *
   Trims leading and trailing spaces from s
 * -------------------------------------------------- */

char *strtrim(char *s);

/* -------------------------------------------------- *
   strlcpy() is a better version of strncpy() as
   always adds the NUL. Like is only for Mac, we add
   a version for the other OS
 * -------------------------------------------------- */

 #ifndef FOR_MAC

size_t strlcpy(char *s1, const char *s2, size_t max);

#endif

/* ------------------------------------------------------ *
   Divide a time_t onto a struct point_on_time_t 'pot'.

    Send 'local' as TRUE to get the time expressed as 
    local time instead of system time.

    It is basically an all-in-one thread-safe version of
    gmtime() and localtime().

    point_on_time_t has some advantages over struct tm
    returned by both gmtime() and localtime():
        
        - Year is fully specified (no calculations needed)
        - Quantities are expressed in human terms (e.g. the
          month goes from 1 to 12 instead from 0 to 11).
        - Can include milliseconds.
        - Can include microseconds.        
        - Includes a field to tell you if time it 
          containss is local or system time.
        - Includes a field to tell if the year was a leap
          year.
        
    Note that this call always return 0 in msec and usec
    because it converts from seconds. 
 * ------------------------------------------------------ */

void time_to_pot(time_t ts,point_on_time_t *pot,int local);

/* The opposite */

time_t time_from_pot(point_on_time_t *pot);


/* ------------------------------------------------------ *

    Builds a stamp tring from time

    Format:   YYYYMMDDHHMMSS

    YYYY = Year   (4 digits)
      MM = Month  (2 digits)
      DD = Day    (2 digits)
      HH = Hour   (2 digits)
      MM = Minute (2 digits)
      SS = Second (2 digits)

      local = TRUE/FALSE convert to local time
 * ------------------------------------------------------ */

char *time_to_stamp(char *pot,size_t max,time_t t,int local);
wchar_t *time_to_stampw(wchar_t *pot,size_t max,time_t t,int local);

/* -------------------------------------------------- *

    Convert 'tam' bytes from 'buf' to a hexadecimal
    string and put it into 'dest' upto 'max' chars
    adding 'sep' between them. if 'sep' is FALSE (0)
    it will not add a separator.

    It returns the amount of chars copied to 'dest'.

    Note that if 'max' is smaller than the needed
    it will just copy the ones that will fit on it.
 * -------------------------------------------------- */

int hex_to_char(char *dest,size_t max,const void *buf,size_t tam,char sep);
int hex_to_wchar(wchar_t *dest,size_t max,const void *buf,size_t tam,wchar_t sep);

/* -------------------------------------------------- *
   Do the reverse of the above functions, that is
   convert 'tam' chars into bytes upto and will
   stop at 'max'.

   The functions will ignore spaces and colons (:)
   in 'buf', although it will stop before 'tam' if
   another char if found that is not 'sep', which
   should be something else than FALSE if another
   separator was used.

   Send 'tam' as 0 to mean 'the whole of buf'
 * -------------------------------------------------- */

size_t char_to_hex(void *dest,size_t max,const char *buf,size_t tam, char sep);
size_t wchar_to_hex(void *dest,size_t max,const wchar_t *buf,size_t tam,wchar_t sep);

/* --------------------------------------------------------------------- *

	int wchar_to_ucs4(unsigned int *resp,const wchar_t *wch,size_t max);

	Parameters:

		'resp' - pointer to destination string
		'wch'  - pointer to the wchar to convert
		'max'  - maximum number of chars that can be read from 'wch'

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

 * ------------------------------------------------------------------------ */


int wchar_to_ucs4(unsigned int *resp,const wchar_t *wch,size_t max);

/* ------------------------------------------------------ *

	int s9_ucs4_to_wchar(wchar_t *resp,unsigned int ucs4);

	parameters:

		'resp' - pointer to response buffer
		'ucs4' - unicode char to be converted

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

 * ------------------------------------------------------ */

int ucs4_to_wchar(wchar_t *resp,unsigned int ucs4);

/* ------------------------------------------------------ *

   Another set of functions but using an 8 bits type and
   codify in UTF-8, so they can return 0,1,2,3 or 4
   depending on how many bytes the UTF-8 sequence had.

    If the sequence is overlong (Eg: encode 0x80 in 3
    bytes instead of 2) or the resulting UCS-4 char is
    ourside the unicode ranges (over 0x10ffff or inside
    the range 0xd800 - 0xdfff), s9_utf8_to_ucs4() will
    write WCHAR_REPLACEMENT in 'resp' and return the
    length of the sequence to be ignored (or 0 if the
    sequende is longer than 'max')

    IMPORTANT: s9_ucs4_to_utf8 assumes that 'resp' can take
               upto 4 chars.
 * ------------------------------------------------------ */

int utf8_to_ucs4(unsigned int *resp,const unsigned char *utf8,size_t max);
int ucs4_to_utf8(unsigned char *resp,unsigned int val);

/* ------------------------------------------------------ *
   Convert an UTF-8 string to a wchar_t string and
   returns the number of bytes used from 'utf8'.

    'max' is the capacity of 'wch' (including a NUL that
    the function will always add).

    'num' is the number of bytes in 'utf8'.

	Erroneous, incomplete or overlong sequences are
	replaced by WCHAR_REPLACEMENT, and the string parsed
	as much as possible.

    If you do not know the exact size of 'utf8' but you
    know it is NUL terminated, use 0 as the size to make
    the function work until finding the NUL char.
 * ------------------------------------------------------ */

size_t utf8_to_wchar(wchar_t *wch,size_t max,const void *utf8,size_t num);

/* ------------------------------------------------------ *

   Opposite to the previous, to the point that it returns
   the number of bytes written in 'utf8', instead of the
   number read as the previous function does. The reason
   for this is that utf8 lenghts change depending on which
   characteres does the string have.

    'max' is the capacity of 'utf8' including the NUL
    that the function adds.

    'num' is the number of bytes to convert from wch.

    If you do not know the exact size of 'wch' but you
    know it is NUL terminated, use 0 as the size to make
    the function work until finding the NUL char.

 * ------------------------------------------------------ */

size_t wchar_to_utf8(unsigned char *utf8,size_t max,const wchar_t *wch,size_t num);

#ifdef __cplusplus
};
#endif



#endif
