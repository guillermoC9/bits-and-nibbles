/*
    test_entropy.c

    Test of entropy routines

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

#include "entropy.h"

/* -------------------------------------- */

void print_bytes(char *prefix, void *d, size_t n)
{
    size_t t;
    unsigned char *dat;

    dat = (unsigned char *)d;

    if (prefix)
        printf("%s", prefix);

    if (d)
    {
        for (t = 0; t < n; t++)
            printf("%02x", dat[t]);
        printf("\n");
    }
    else
    {
        printf("NULL\n");
    }
}

/* -------------------------------------- */

void test_entropy(void)
{
    int t,bits;
    unsigned char entropy[64];
    unsigned int a,b;    

    fprintf(stdout,"\n*** Entropy Test ***\n\n");

    for (t = 0; t < 16; t++)
        fprintf(stdout,"unpredictable_seed()=%x\n", unpredictable_seed());

    fprintf(stdout,"\n");

    for (t = 0; t < 16; t++)
        fprintf(stdout,"unpredictable_seed_no_linear()=%x\n", unpredictable_seed_non_linear());

    fprintf(stdout,"\n");    

    for (t = 0; t < 16; t++)
    {
	    get_entropy(entropy, 64);	    
    	print_bytes("get_entropy()=", entropy, 64);        
    }

    fprintf(stdout,"\n");

    for (t = 0; t < 16; t++)
    {
	    get_entropy_non_linear(entropy, 64);	    
    	print_bytes("get_entropy_no_linear()=", entropy, 64);        
    }
}

/* -------------------------------------- */

int main(void)
{
    test_entropy();
    return 0;
}
