/*
    s9_luhn.h

    Luhn CRC routines

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

            https://en.wikipedia.org/wiki/Luhn_algorithm

        - This algorithm is used to detect if the order of the digits
          of a number is correct.  Its mostly used to check credit
          card numbers and bank accounts numbers, as the bank usually
          adds the checksum of the number at the end of the same.

                                --oO0Oo--
*/

#ifndef CRC_LUHN_BY_GAO
#define CRC_LUHN_BY_GAO

#include <ctype.h>
#include <wctype.h>

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    Calculate and return Luhn's digit.     

    'num' is a string with the number to process, 
          the function will stop in the first 
          non-numeric  char that is not a separator 
          if 'sep' is not TRUE

    'len' is the maximum number of chars to process
          including separators. Set it to 0 to check 
          all the string.

    'check' TRUE/FALSE to tell the function if it 
            should expect and check the checksum 
            digit at the end of the string, or just 
            return it.

    'sep' is TRUE/FALSE if jump over separators, 
          being this a non-numerical value. 

    Exmples:

            calc_luhnc("7992739871",0,FALSE,FALSE) == '3'
            calc_luhnc("79927398713",0,TRUE,FALSE) == '0'
            calc_luhnw(L"79-927398-71",0,FALSE,TRUE) == L'3'
            calc_luhnw(L"X79-927398-71-3",0,TRUE,TRUE) == L'0'

    The functions will return 0 if the check fails or no
    digits are found

            calc_luhnw(L"X79-927398-71-3",0,TRUE,FALSE) == 0
            calc_luhnc("7927398713 8",0,TRUE,TRUE) == 0
            calc_luhnc("X7927398713 8",0,TRUE,FALSE) == 0
            calc_luhnc("X792739871 3",0,TRUE,TRUE) == '0'

    *** IMPORTANT ***

    There is a limitation of 256 chars in the number
    of digits to be processed (not counting separators
    if allowed)

 * -------------------------------------------------- */

char    calc_luhnc(const char *num,int digits,int check,int sep);
wchar_t calc_luhnw(const wchar_t *num,int digits,int check, int sep);


#ifdef __cplusplus
};
#endif



#endif
