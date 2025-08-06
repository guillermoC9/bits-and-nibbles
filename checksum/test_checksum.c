/*
    test_checksum.c

    Test of checksum routines

    (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Written by Guillermo Amodeo Ojeda.                  
            
                        --oO0Oo--
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dni.h"
#include "luhn.h"

/* --------------- */

void test_crc_luhn(void)
{
    char *ptr;
    wchar_t *wptr;    

    printf("\n*** Test Luhn's CRC ***\n\n");
    
    ptr = "7992739871 3";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE,TRUE));

    printf("calc_luhnc('%s',0,FALSE,FALSE) = '%c'. Must be '3'\n", ptr, calc_luhnc(ptr, 0, FALSE, FALSE));

    ptr = "7992739871 3";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, TRUE));

    ptr = "7992-7398-71";
    printf("calc_luhnc('%s',0,FALSE,TRUE) = '%c'. Must be '3'\n", ptr, calc_luhnc(ptr, 0, FALSE, TRUE));

    ptr = "7992-7398-71-3";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, TRUE));

    ptr = "7992739871";
    printf("calc_luhnc('%s',0,FALSE,FALSE) = '%c'. Must be '3'\n", ptr, calc_luhnc(ptr, 0, FALSE, FALSE));

    ptr = "7992739871 3";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, TRUE));

    ptr = "7992739878";
    printf("calc_luhnc('%s',0,FALSE,FALSE) = '%c'. Must be '8'\n", ptr, calc_luhnc(ptr, 0, FALSE, FALSE));

    ptr = "7992739878 8";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, TRUE));

    ptr = "7992739878 3";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be ''\n", ptr, calc_luhnc(ptr, 0, TRUE, FALSE));

    ptr = "502071110577440";
    printf("calc_luhnc('%s',0,FALSE,FALSE) = '%c'. Must be '7'\n", ptr, calc_luhnc(ptr, 0, FALSE, TRUE));

    ptr = "5020711105774407";
    printf("calc_luhnc('%s',0,TRUE,FALSE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, FALSE));

    ptr = "502071110577440";
    printf("calc_luhnc('%s',0,FALSE,FALSE) = '%c'. Must be '7'\n", ptr, calc_luhnc(ptr, 0, FALSE, TRUE));

    ptr = "5020 7111 0577 4407";
    printf("calc_luhnc('%s',0,TRUE,TRUE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, TRUE));

    ptr = "411111111111111";
    printf("calc_luhnc('%s',0,FALSE,FALSE) = '%c'. Must be '1'\n", ptr, calc_luhnc(ptr, 0, FALSE, FALSE));

    ptr = "4111111111111111";
    printf("calc_luhnc('%s',0,TRUE,FALSE) = '%c'. Must be '0'\n", ptr, calc_luhnc(ptr, 0, TRUE, FALSE));

    wptr = L"4111111112111111";
    printf("calc_luhnc('%ls',0,TRUE,FALSE) = '%C'. Must be ''\n",wptr, calc_luhnw(wptr, 0, TRUE, FALSE));

    wptr = L"555544443333111 ";
    printf("calc_luhnw('%ls',0,FALSE,FALSE) = '%C'. Must be '1'\n", wptr, calc_luhnw(wptr, 0, FALSE, FALSE));

    wptr = L"5555444433331111";
    printf("calc_luhnw('%ls',0,TRUE,FALSE) = '%C'. Must be '0'\n", wptr, calc_luhnw(wptr, 0, TRUE, FALSE));

    printf("\n");
}

/* -------------------------- */

static int  strempty(const char *s)
{
    if(s)
    {
        while(*s)
        {
            if(!isspace(*s++))
                return FALSE;
        }
    }
    return TRUE;
}

/* -------------------------------------- */

void test_dni_letter(void)
{
    char num[1025] = {0},*ptr;

    printf("\n*** Test DNI/NIE's check ***\n\n");

    do {
        printf("Write a DNI or NIE (empty to end):\n");
        if(fgets(num,1024,stdin) && !strempty(num))
        {
            ptr=strpbrk(num,"\r\n");
            if(ptr)
                *ptr='\0';
            printf("The control letter for '%s' is '%c', ",num,calc_dni_letter(num));
            printf("so the one given is %s\n",check_dni_letter(num) ? "Ok" : "Wrong");
        }
    } while(!strempty(num));

    printf("calc_dni_letterw(12345678)   = '%c'\n",calc_dni_letterw(L"12345678"));
    printf("check_dni_letterw(12345678Z) = %s\n",check_dni_letterw(L"12345678Z") ? "Ok" : "Bad");

    printf("calc_dni_letterw(2564589)    = '%c'\n",calc_dni_letterw(L"(2564589"));
    printf("check_dni_letterw(2564589C)  = %s\n",check_dni_letterw(L"2564589C") ? "Ok" : "Bad");

    printf("calc_dni_letterw(X1234567)   = '%c'\n",calc_dni_letterw(L"X1234567"));
    printf("check_dni_letterw(X1234567L) = %s\n",check_dni_letterw(L"X1234567L") ? "Ok" : "Bad");

    printf("calc_dni_letterw(Z3456789)   = '%c'\n",calc_dni_letterw(L"Z3456789"));
    printf("check_dni_letterw(X3456789T) = %s (it should be D)\n",check_dni_letterw(L"Z3456789T") ? "Ok" : "Bad");

    printf("\n");
}


int main()
{   
    test_dni_letter();
    test_crc_luhn(); 
    return 0;
}
