/*
    entropy.h

    Entropy generation routines

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

        - Designed and written by Guillermo Amodeo Ojeda.                  
            
                        --oO0Oo--
*/

#ifndef ENTROPY_GENERATOR_BY_GUILLE
#define ENTROPY_GENERATOR_BY_GUILLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* --------------------------------- *
   Make sure unsigned int is at 
   least 32 bits long 
 * --------------------------------- */

#if UINT_MAX <  0xffffffffu
#error "unsigned int is expected to have at least 32 bits"
#endif

/* -------------------------------------------- *
    This function generates an unpredictable
    unsigned integer of up to 32 bits.

    It is garanteed to return a different number
    everytime you call it.

    REMEMBER: 

    The number is unpredictable -not random-,
    and intended for feeding pseudo-random
    generators with unpredictable entorpy
 * -------------------------------------------- */

unsigned int unpredictable_seed(void);

/* -------------------------------------------- *   
    This function is the same as the previous
    BUT it takes an unpredictable amount of 
    time to execute.
 * -------------------------------------------- */

unsigned int unpredictable_seed_non_linear(void);

/* -------------------------------------------- *
    These functions use the previous functions
    to generate an unpredictable amount of data,
    and in the  case of get_enmtropy_non_linear()
    also takes an unpredictable amount of time to 
    execute.

                 *** WARNING ***

    You may be tempted to use these functions 
    as random data generators, but I discorage 
    it if you are going to do something other
    than feed pseud-random generators.

    Unless of course you know pretty well what
    you are doing and have measured the risk. 
    
    Random data generation is not a trivial 
    business, specially for use in criptography.
    Therefore choose a good pseudo-random 
    generator for that, and may be use these 
    functions to feed it with entropy if you
    do not have a better source for it.
 * -------------------------------------------- */

void get_entropy(void *dest,size_t len);
void get_entropy_non_linear(void *dest,size_t len);


#endif


