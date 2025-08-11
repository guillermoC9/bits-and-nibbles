/*
    test_mpint.c

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

#include "mpint.h"

/* -------------------------------------- */

void test_mpint(void)
{
    mp_int_t *m1,m2,r,r2;
    mp_digit_t sdig[7]={0xcafe,0x01234567,0x89abcdef,0x00112233,0x44556677,0x8899aabb,0xccddeeff};
    mp_digit_t *ddig;
    unsigned int t,count;
    unsigned char *dbytes,sbytes[7]={0x00,0xca,0xfe,0xba,0xbe,0xfe,0xca},ebytes[10];

    fprintf(stdout,"\n== MPI TEST START \n\n");

    m1 = mp_create();
    mp_set_bytes(m1,sbytes,7);
    if(m1 == NULL)
    {
        printf("Error creating m1 with bytes\n");
    }
    else
    {
        int ret;

        dbytes = mp_get_bytes(m1,&count);
        if(dbytes == NULL)
        {
            printf("Error reading m1's bytes\n");
        }
        else
        {
            printf("m1 has %u bytes (%s)\n\n",count,(count==6) ? "CORRECT" : "WRONG");

            printf("Showing them:\n\n");

            mp_show("Real m1  ",m1);

            printf("sbytes(%u) = ",count);
            for(t=0;t<7;t++)
                printf("%02x",sbytes[t]);

            printf("\ndbytes(%u) = ",count);
            for(t=0;t<count;t++)
                printf("%02x",dbytes[t]);
            free(dbytes);

            if(mp_copy_bytes(m1,sbytes,7)==MP_OKAY)
            {
                printf("\nsbytes(7) = ");
                for(t=0;t<7;t++)
                    printf("%02x",sbytes[t]);
            }

            printf(" (Copied)\n\n");
        }

        printf("m1 - Copy Exact Test\n\n");

        ret = mp_copy_exact_bytes(m1,ebytes,10);
        printf("%d - ebytes(10) = ",ret);
        for(t=0;t<10;t++)
            printf("%02x",ebytes[t]);

        printf("\n");

        ret = mp_copy_exact_bytes(m1,ebytes,4);
        printf("%d - ebytes(4)  = ",ret);
        for(t=0;t<4;t++)
            printf("%02x",ebytes[t]);

        printf("\n\n");

        mp_free(m1);
    }

    m1 = mp_create();
    mp_set_wide_string(m1,L"cafe0123456789abcdef00112233445566778899aabbccddeeff",16);
    if(m1 == NULL)
    {
        printf("Error creating m1\n");
    }
    else
    {
        mp_show("m1",m1);
        mp_free(m1);
    }

    mp_init_set_digits(&m2,7,sdig);

    mp_show("m2",&m2);

    m1 = mp_create();
    if(m1 == NULL)
    {
        printf("Error creating m1\n");
    }
    else
    {
        mp_set_d(m1,9);
    }

    mp_show("m1",m1);
    mp_show("m2",&m2);


    printf("m1 have %d bits\n",mp_count_bits(m1));
    printf("m2 have %d bits\n\n",mp_count_bits(&m2));

    ddig = mp_get_digits(&m2,&count);
    if(ddig == NULL)
    {
        printf("Error reading m2's digits\n");
    }
    else
    {
        char *fmt = "0x%x";

        printf("m2 has %u digits (%s)\n\n",count,(count==7) ? "CORRECT" : "WRONG");

        printf("Showing them:\n\n");

        mp_show("Real m2",&m2);

        printf("sdig[%u] = ",count);
        for(t=0;t<7;t++)
        {
            printf(fmt,sdig[t]);
            fmt = "%08x";
        }
        fmt = "0x%x";
        printf("\nddig[%u] = ",count);
        for(t=0;t<count;t++)
        {
            printf(fmt,ddig[t]);
            fmt = "%08x";
        }
        free(ddig);

        printf("\n\n");
    }

    mp_show("m1",m1);
    mp_show("m2",&m2);

    printf("\n");

    printf("bit   0 of m1 = %d (1)\n",mp_get_bit(m1,0));
    printf("bit   1 of m1 = %d (0)\n",mp_get_bit(m1,1));
    printf("bit   2 of m1 = %d (0)\n",mp_get_bit(m1,2));
    printf("bit   3 of m1 = %d (1)\n",mp_get_bit(m1,3));
    printf("bit   4 of m1 = %d (0)\n",mp_get_bit(m1,4));

    printf("\n");

    printf("bit 203 of m2 = %d (1)\n",mp_get_bit(&m2,203));
    printf("bit 204 of m2 = %d (0)\n",mp_get_bit(&m2,204));
    printf("bit 205 of m2 = %d (0)\n",mp_get_bit(&m2,205));
    printf("bit 206 of m2 = %d (1)\n",mp_get_bit(&m2,206));
    printf("bit 207 of m2 = %d (1)\n",mp_get_bit(&m2,207));

    printf("\n");

    for(count=8,t=0;t<10;t++)
    {
        mp_2expt(m1,count);
        printf("2^%2u = ",count);
        mp_print(m1,10,stdout);
        mp_show(" - HEX ",m1);
        count += 2;
    }


    printf("\n");


    mp_init_set_d(&r,0xe447);
    mp_init_set_d(&r2,0x13);

    mp_show("r ",&r);
    mp_show("r2",&r2);

    printf("\n");

    mp_shift(&r,&r,12);
    mp_shift(&r2,&r2,32);

    mp_show("r ",&r);
    mp_show("r2",&r2);

    printf("\n");

    mp_shift(&r,&r,-12);
    mp_shift(&r2,&r2,-32);

    mp_show("r ",&r);
    mp_show("r2",&r2);

    printf("\n");


    printf("bit  0 of r = %d (1)\n",mp_get_bit(&r,0));
    printf("bit  1 of r = %d (1)\n",mp_get_bit(&r,1));
    printf("bit  2 of r = %d (1)\n",mp_get_bit(&r,2));
    printf("bit  3 of r = %d (0)\n",mp_get_bit(&r,3));
    printf("bit  4 of r = %d (0)\n",mp_get_bit(&r,4));
    printf("bit 50 of r = %d (0)\n",mp_get_bit(&r,50));

    printf("\n");

    printf("bit  0 of r2 = %d (1)\n",mp_get_bit(&r2,0));
    printf("bit  1 of r2 = %d (1)\n",mp_get_bit(&r2,1));
    printf("bit  2 of r2 = %d (0)\n",mp_get_bit(&r2,2));
    printf("bit  3 of r2 = %d (0)\n",mp_get_bit(&r2,3));
    printf("bit  4 of r2 = %d (1)\n",mp_get_bit(&r2,4));
    printf("bit 51 of r2 = %d (0)\n",mp_get_bit(&r2,51));

    printf("\n");

    mp_set_bit(&r,50,1);
    mp_set_bit(&r2,51,1);

    mp_show("r ",&r);
    mp_show("r2",&r2);

    printf("\n");

    printf("bit 50 of r  = %d (1)\n",mp_get_bit(&r,0));
    printf("bit 51 of r2 = %d (1)\n",mp_get_bit(&r2,51));

    printf("\n");

    mp_set_bit(&r,50,0);
    mp_set_bit(&r2,51,0);

    mp_show("r      ",&r);
    mp_show("r2     ",&r2);


    printf("\n");

    printf("bit 50 of r  = %d (0)\n",mp_get_bit(&r,50));
    printf("bit 51 of r2 = %d (0)\n",mp_get_bit(&r2,51));

    printf("\n");

    mp_sqr(&r,&r2);

    mp_show("r      ",&r);
    mp_show("r2     ",&r2);

    mp_sqrt(&r2,&r);

    mp_show("srt(r2)",&r);

    mp_sqr(&r2,&r2);

    mp_set_zero(&r);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("srt(r2)",&r);

    mp_sqr(&r2,&r2);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("srt(r2)",&r);

    mp_sqr(&r2,&r2);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("srt(r2)",&r);

    mp_sqr(&r2,&r2);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("srt(r2)",&r);

    mp_free(m1);
    mp_clear(&m2);
    mp_clear(&r);
    mp_clear(&r2);

    fprintf(stdout,"\n== MPI TEST END\n\n");
}



/* -------------------------------------- */

int main(void)
{
    test_mpint();    
    return 0;
}
