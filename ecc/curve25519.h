/*
    curve25519.h

    Dj Bernstein's Curve 25519 implementation

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

                                --oO0Oo--
*/

#ifndef CURVE25519_IMPL
#define CURVE25519_IMPL

#include "mpint.h"

#define CURVE_25519_POINT_BYTES     32

/** Funciones */

#ifdef __cplusplus
extern "C" {
#endif

/* Calculates  resp = Curve25519(point,base). All args can point to the same MPI */

void curve25519_scalarmult(mp_int_t *point,mp_int_t *base,mp_int_t *resp);

#ifdef __cplusplus
};
#endif


#endif
