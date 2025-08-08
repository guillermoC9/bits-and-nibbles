/*
    barcode.h

    Barcode Generation Routines

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

        - Written by Guillermo Amodeo Ojeda using the info from:

          https://en.wikipedia.org/wiki/International_Article_Number

        - This code allows to build a string of bits that can be used to draw
          a barcode either in text or graphics

                                --oO0Oo--
*/

#ifndef BARCODE_BY_GAO
#define BARCODE_BY_GAO

#include "stuff.h"

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

#define EAN_13_BYTES     12
#define EAN_8_BYTES       9
#define EAN_5_BYTES       6
#define EAN_2_BYTES       3

/* -------------------------------------------------- *
   Return a 12 byte array (96 bits) from which the
   lower 95 identify a EAN-13 barcode with the
   following format:

   bits 94 to 92 are the prefix (always 101)
   bits 91 to 50 are the 1st 6 digits (7 bits each)
   bits 49 to 45 are the separator (always 01010)
   bits 44 to 3 are the 2nd 6 digits (7 bits each)
   bist 2 to 0 are the suffix (always 101)

   Bits as 0 are white, bits as 1 are black.

   Note that this bits are expressed as big endian
   so bit 94 is actually the second bit of the first
   byte of the array (you should use something like
   mem_get_bit(array,12,94) to get it)

    'resp' is the string that receives the row

    'max' is the number of bytes that fit in 'resp',
          if less than 12 it returns S9_ERANGE

    'bc' is the numeric code of the barcode. If
           less than 13 digits, it adds the check
           digit at the right end and pads with 0
           at the left until its 13 chars

    If 'bc' is not a correct EAN-13 it returns
    the error S9_EINCORRECT

 * -------------------------------------------------- */

int get_ean_13(unsigned char *resp,size_t max,const char *bc);

/* -------------------------------------------------- *
   Return a 13 char string with a corrected EAN-13
   code.

    'resp' is the string that receives the code

    'max' is the number of chars that fit in 'resp'
          counting the final 0. If less than 14
          it returns S9_ERANGE

    'bc'  is the numeric code of the barcode. If
          less than 13 digits, it adds the check
          digit at the right end and pads with 0
          at the left until its 13 chars

    If 'bc' is not a correct EAN-13 it returns
    the error S9_EINCORRECT

 * -------------------------------------------------- */


int format_ean_13(char *resp,size_t max,const char *bc);

/* -------------------------------------------------- *
   Same as previous two but uses 8 digits long codes
   instead 13 and the returned array is 72 bits (9
   bytes long) instead 96 bits like the EAN-13.
   Likewise EAN-8 barcode mask is in the lower 67
   bits using the following format:

   Barcode row format:

   bits 66 to 64 are the prefix (always 101)
   bits 63 to 36 are the 1st 4 digits (7 bits each)
   bits 35 to 31 are the separator (always 01010)
   bits 30 to 3 are the 2nd 4 digits (7 bits each)
   bist 2 to 0 are the suffix (always 101)

   Here max has to be a minimum of 9 on both cases
   to avoid fails

   If 'bc' is not a correct EAN-8 it returns
    the error S9_EINCORRECT

 * -------------------------------------------------- */

int get_ean_8(unsigned char *resp,size_t max,const char *bc);
int format_ean_8(char *resp,size_t max,const char *bc);

/* -------------------------------------------------- *
   Same as previous although it uses 5 digits long
   codes. In here the resulting barcode row is 48
   bits (6 bytes) long and all the bits are significant
   and have the following format:

   bits 47 to 43 are the prefix (always 01011)
   bits 42 to 36 are the 1st digit (7 bits)
   bits 35 to 34 are a separator (always 01)
   bits 33 to 27 are the 2ndt digit (7 bits)
   bits 26 to 25 are a separator (always 01)
   bits 24 to 18 are the 3rd digit (7 bits)
   bits 17 to 16 are a separator (always 01)
   bits 15 to 9 are the 4th digit (7 bits)
   bits 8 to 7 are a separator (always 01)
   bits 6 to 0 are the 5th digit (7 bits)

   Its important to know that the check digit is
   not included in the 5 digits as its not used to
   validate the number, only to generate the code
   bitmask and to validate the read by the reader.

   if bc is not 5 digits long or max is less
   than 10 it will return S9_ERANGE

 * -------------------------------------------------- */

int get_ean_5(unsigned char *resp,size_t max,const char *bc);

/* -------------------------------------------------- *
   Same as previous although it uses 2 digits long
   codes. In here the resulting barcode row is 24
   bits (3 bytes) long and the EAN-2 barcode mask
   is in the lower 21 bits with the following format:

   bits 20 to 16 are the prefix (always 01011)
   bits 15 to 9 are the 1st digit (7 bits)
   bits 8 to 7 are a separator (always 01)
   bits 6 to 0 are the 2nd digit (7 bits)

   Its important to know that the check digit is
   not included in the 5 digits as its not used to
   validate the number, the code is generated from
   the two codes.

   if bc is not 2 digits long or max is less
   than 3 it will return S9_ERANGE

 * -------------------------------------------------- */

int get_ean_2(unsigned char *resp,size_t max,const char *bc);



#ifdef __cplusplus
};
#endif



#endif
