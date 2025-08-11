/*
    s9_luhn.c

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


#include "luhn.h"

/* ------------------------------------------ *
    These functions play with the fact that 
    values for the numbers are the same in 
    all encodigns, whatever simple (ascii 
    or utf-8) or wide (utf-16 or ucs-32).

    For the digits 0-9, it goes from 0x30 
    for '0', up to 0x39 for '9'
 * ------------------------------------------ */

static int calc_luhn_digit(int *num,int len,int check,int wide)
{
    int dig,crc,par=!check;
    static int parity[10]={0,2,4,6,8,1,3,5,7,9};   
    
    crc = 0;
    if(len > 0)
    {
        while(len > 0)
        {         
            dig = num[--len];
            crc += (par) ? parity[dig] : dig;
            par = !par;        
        }

        dig = (crc % 10);
        if (dig)
            dig = (check) ? -1 : (10 - dig);        
        return dig;
    }
    return -1;
}

/* ------------------------------ */

char calc_luhnc(const char *num,int len,int check,int sep)
{
    int cnt,t,ret = 0; 
    const char *ch;
    int tmp[256];
    
    if(num)
    {        
        if(len < 1)
            len = 0x7fff;
        ch = (const char *) num;
        for(cnt=t=0;ch[t] && t<len;t++)
        {
            tmp[cnt] = (int) ch[t];            
            if(tmp[cnt] > 0x2f && tmp[cnt] < 0x3a) /* Equivalent to isdigit() */
            {
                 tmp[cnt] -= 0x30;
                if(++cnt == 256)
                    break;
            }         
            else if(!sep)
                break;
        }
 
        ret = calc_luhn_digit(tmp,cnt,check,FALSE);
        if(ret > -1)            
            return (char) (ret + 0x30);
    }
    return 0;
}


/* ------------------------------ */

wchar_t calc_luhnw(const wchar_t *num,int len,int check,int sep)
{
    int cnt,t,ret = 0; 
    const wchar_t *wc;
    int tmp[256];
    
    if(num)
    {
        if(len < 1)
            len = 0x7fff;
        wc = (const wchar_t *) num;
        for(cnt=t=0;wc[t] && t<len;t++)
        {
            tmp[cnt] = (int) wc[t];            
            if(tmp[cnt] > 0x2f && tmp[cnt] < 0x3a) /* Equivalent to isdigit() */
            {
                 tmp[cnt] -= 0x30;
                if(++cnt == 256)
                    break;
            }         
            else if(!sep)
                break;
        }
 
        ret = calc_luhn_digit(tmp,cnt,check,TRUE);
        if(ret > -1)            
            return (wchar_t) (ret + 0x30);
    }
    return 0;
}
