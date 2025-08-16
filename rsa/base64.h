/*
    base64.h

    Base64 encoding/decoding

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

#ifndef GAO_BASE64
#define GAO_BASE64

#include "stuff.h"

/** Macros to calculate size and decoded size*/

#define base64_size_enc(x)   ((((x) + 2) / 3) * 4)
#define base64_size_dec(x)   (((x) / 4) * 3)

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Encode a maximum of 3 bytes from 'origin' to a
   block of 4 chars in base64 and return the number
   of chars consumed from a max of 'num'.

   Like in the base64 encoding the result is bigger
   than the original, 'dest' and 'origin' SHOULD NOT
   point the same buffer unless you know there is
   enough space available for the conversion.

   WARNING, this función DO NOT add the NUL char
   and the resulting string is always a base64
   block (4 chars), padded with '=' if needed.

   'dest' is assumed to be able to hold 4 chars.

   'safe' = TRUE/FALSE use the URL/File Safe alphabet 
   from RFC 3548(-,_  instead of +,/  )
 * -------------------------------------------------- */

size_t bytes_to_base64(char *dest,const void *origin,size_t num,int safe);

/* -------------------------------------------------- *
    Decodes the base64 block 'origin' in 'dest'
    returning the number of bytes written to it.
    The function stops after 4th chars or before
    if it finds a non base64 char.

    In this case, 'dest' and 'origin' can point to
    the same buffer as the output is smaller than
    the input.

    It also understands the URL/File Safe alphabet
    from RFC 3548.
 * -------------------------------------------------- */

size_t base64_to_bytes(void *dest,const char *origin);

/* -------------------------------------------------- *

   Encode a buuffer of bytes to base64 and return
   the number of chars written to 'dest'.

   As usual with encoding, 'dest' should not point
   to the same buffer as 'orig'.

   This function always add the NUL char at the end
   even if there is not space to convert the whole
   buffer. The output is padded with '=' if the
   output is not a multiple of 4.

   'safe' = TRUE/FALSE use the URL/File Safe alphabet 
   from RFC 3548(-,_  instead of +,/  )
 * -------------------------------------------------- */

size_t base64_encode(char *dest,size_t max,const void *origin,size_t num,int safe);

/* -------------------------------------------------- *

   Decodes the base64 string 'origin' onto 'dest'
   and return the number of bytes written to it.

   If ignore is TRUE, this function ignores all
   non-base64 chars and keep going until 'num'
   chars or until the NUL char is found before
   'num' chars, or 'num' is 0.

    If you would like the function to stop in
    the first non-base64 char that if finds,
    set 'ignore' to FALSE.

    In this case, 'dest' and 'origin' can point
    to the same buffer as the output is smaller
    than the input.

    It also understands the URL/File Safe alphabet
    from RFC 3548.
 * -------------------------------------------------- */

size_t base64_decode(void *dest,size_t max,const char *origin,size_t num,int ignore);

/* -------------------------------------------------- *
   wchar_t versions of the previous functions
 * -------------------------------------------------- */

size_t base64_encodew(wchar_t *dest,size_t max,const void *origin,size_t num,int safe);
size_t base64_decodew(void *dest,size_t max,const wchar_t *origin,size_t num,int ignore);

/* -------------------------------------------------- *
   Saves 'num' bytes from 'buf' in the file 'fp' in
   Base64 encoding.

   'width' is the numbers of char to write in each
           line (0 means just one line). it is adjusted
           to the next 4 chars so each lien contains
           full blocks.

    'safe' TRUE/FALSE use the URL/File Safe alphabet
           from RFC 3548(-,_  instead of +,/  )
 * -------------------------------------------------- */

int base64_save(FILE *fp,size_t width,const void *buf,size_t num,int safe);

#ifdef __cplusplus
};
#endif


#endif
