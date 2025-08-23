/*
    s9_dni.h

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

        - DNI format is XXXXXXXXC  (X = number, C = control letter)
        - NIE format is LXXXXXXXC  (L is a letter (X, Y or Z), X = Number and C = control letter)


*/

#ifndef DNI_ROUTINES_BY_GAO
#define DNI_ROUTINES_BY_GAO

#include <ctype.h>
#include <wctype.h>
#include <stddef.h>

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    Calculate and return DNI/NIE's control letter

    'num' is the DNI/NIE number to check. If control
    letter is given it is ignored, and if an invalid
    NIE number is given it returns 0

    Examples.

            calc_dni_letter("12345678") == 'Z'
            calc_dni_letter("X1234567") == 'L'

 * -------------------------------------------------- */

char    calc_dni_letter(const char *num);
wchar_t calc_dni_letterw(const wchar_t *num);

/* -------------------------------------------------- *
    Check and return TRUE if a DNI/NIE's control
    letter is correct (basically if the number has
    the correct format).

    'num' is the COMPLETE DNI/NIE number to check.
    If control letter is not given or its wrong
    the function return FALSE

    Examples.

            check_dni_letter("12345678Z") == TRUE
            check_dni_letter("X1234567L") == TRUE
            check_dni_letter("X1234567X") == FALSE

 * -------------------------------------------------- */


int check_dni_letter(const char *num);
int check_dni_letterw(const wchar_t *num);



#ifdef __cplusplus
};
#endif



#endif
