/*
    dni.c

    Spanish DNI/NIE routines

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

            https://www.interior.gob.es/opencms/ca/servicios-al-ciudadano/tramites-y-gestiones/dni/calculo-del-digito-de-control-del-nif-nie/

                                --oO0Oo--
*/

#include "dni.h"

/* -------------------------------------------------------------------------------------------------------------- *

  Remainder - 0   1   2   3   4   5   6   7   8   9   10   11  12   13   14   15  16  17   18   19   20   21  22
  Letter    - T   R   W   A   G   M   Y   F   P   D    X    B   N    J    Z    S   Q   V    H    L    C    K   E

   This function plays with the fact that values for all the latin alphabet are the same in all encodigns, 
   whatever simple (ascii or utf-8) or wide (utf-16 or ucs-32).

   For the digits 0-9, it goes from 0x30 for '0', up to 0x39 for '9'
* -------------------------------------------------------------------------------------------------------------- */

static int get_control_letter(const void *num,int wide,int *last)
{
    static const char *letter="TRWAGMYFPDXBNJZSQVHLCKE";
    unsigned int dni = 0;
    int val,alpha;
    const wchar_t *wc = NULL;
    const char *ch = NULL;
    
    if(num)
    {
        if(wide)
        {
            wc = (const wchar_t *)num;
            while(iswspace(*wc))
                wc++;
            alpha = iswalpha(*wc);
            val = (int) *wc++;
        }
        else
        {
            ch = (const char *)num;
            while(isspace(*ch))
                ch++;        
            alpha = isalpha(*ch);
            val = (int)*ch++;           
        }

        if(alpha)
        {
            val = toupper(val);
            if(val == (int)'Z')
                val = 0x32;
            else if(val == (int)'Y')
                val = 0x31;
            else if(val == (int)'X')
                val = 0x30;
            else    
                return 0; /* Wrong format */
        }                     

        *last = val; 

        while(val > 0x2f && val < 0x3a) /* Equivalent to isdigit() */
        {                   
            dni *=10;
            dni += (val - 0x30);
            if(wide)
                val = (int) *wc++;
            else
                val = (int) *ch++;
           *last = toupper(val);
        }

        return (int)letter[(dni % 23)];
    }
    return 0;
}

/* ------------------------------ */

char calc_dni_letter(const char *num)
{
    int last;

    return (char)get_control_letter(num,FALSE,&last);
}

/* ------------------------------ */

wchar_t calc_dni_letterw(const wchar_t *num)
{
    int last;

    return (wchar_t) get_control_letter(num,TRUE,&last);
}

/* ------------------------------ */

int check_dni_letter(const char *num)
{
    int last,ret;    

    ret = get_control_letter(num,FALSE,&last);

    return (ret && ret == last);

}

/* ------------------------------ */

int check_dni_letterw(const wchar_t *num)
{
    int last,ret;

    ret = get_control_letter(num,TRUE,&last);
    return (ret && ret == last);
}

