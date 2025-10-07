/*
    test_mpint.c

    Test of multiprecision integers routines

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

const char *pi_x_10_power_3009 = 
    "314159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664"
    "709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316"
    "527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036"
    "001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272"
    "489122793818301194912983367336244065664308602139494639522473719070217986094370277053921717629317675238467481846766940513"
    "200056812714526356082778577134275778960917363717872146844090122495343014654958537105079227968925892354201995611212902196"
    "086403441815981362977477130996051870721134999999837297804995105973173281609631859502445945534690830264252230825334468503"
    "526193118817101000313783875288658753320838142061717766914730359825349042875546873115956286388235378759375195778185778053"
    "217122680661300192787661119590921642019893809525720106548586327886593615338182796823030195203530185296899577362259941389"
    "124972177528347913151557485724245415069595082953311686172785588907509838175463746493931925506040092770167113900984882401"
    "285836160356370766010471018194295559619894676783744944825537977472684710404753464620804668425906949129331367702898915210"
    "475216205696602405803815019351125338243003558764024749647326391419927260426992279678235478163600934172164121992458631503"
    "028618297455570674983850549458858692699569092721079750930295532116534498720275596023648066549911988183479775356636980742"
    "654252786255181841757467289097777279380008164706001614524919217321721477235014144197356854816136115735255213347574184946"
    "843852332390739414333454776241686251898356948556209921922218427255025425688767179049460165346680498862723279178608578438"
    "382796797668145410095388378636095068006422512520511739298489608412848862694560424196528502221066118630674427862203919494"
    "504712371378696095636437191728746776465757396241389086583264599581339047802759009946576407895126946839835259570982582262"
    "052248940772671947826848260147699090264013639443745530506820349625245174939965143142980919065925093722169646151570985838"
    "741059788595977297549893016175392846813826868386894277415599185592524595395943104997252468084598727364469584865383673622"
    "262609912460805124388439045124413654976278079771569143599770012961608944169486855584840635342207222582848864815845602850"
    "601684273945226746767889525213852254995466672782398645659611635488623057745649803559363456817432411251507606947945109659"
    "609402522887971089314566913686722874894056010150330861792868092087476091782493858900971490967598526136554978189312978482"
    "168299894872265880485756401427047755513237964145152374623436454285844479526586782105114135473573952311342716610213596953"
    "623144295248493718711014576540359027993440374200731057853906219838744780847848968332144571386875194350643021845319104848"
    "100537061468067491927819119793995206141966342875444064374512371819217999839101591956181467514269123974894090718649423196"
    "1567945208";

void test_10_pow_3009_pi(void)
{
    mp_int_t big_pi,big_piSQ;
    int ret;
    char number[16384];

    ret = mp_init_set_string(&big_pi,pi_x_10_power_3009,10);
    printf("\nThe result of settign the big_pi is %d\n",ret);
    if(ret == MP_OKAY)
    {
        mp_show_decimal("big_pi * 10^3009 =",&big_pi);
        printf("This is a huge number of %lu digits and %d bits\n\n",strlen(pi_x_10_power_3009),mp_count_bits(&big_pi));        
        mp_init(&big_piSQ);
        printf("Let's square it see how big it is now...\n");
        mp_sqr(&big_pi,&big_piSQ);
        mp_toradix(&big_piSQ,(unsigned char *)number,10);
        mp_show_decimal("big_pi ^ 2 =",&big_piSQ);                
        printf("This is a huge number of %lu digits and %d bits\n\n",strlen(number),mp_count_bits(&big_piSQ));
        mp_set_zero(&big_pi);
        printf("Let's do the square root (we should get big_pi again)...\n");
        mp_sqrt(&big_piSQ,&big_pi);
        mp_show_decimal("sqrt(big_pi^2)  =",&big_pi);
        mp_toradix(&big_pi,(unsigned char *)number,10);
        printf("The square root of big_pi^2 is %lu digits and %d bits\n\n",strlen(number),mp_count_bits(&big_pi));        
        mp_clear(&big_piSQ);
        mp_clear(&big_pi);
    }
}


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

    mp_show("sqrt(r2)",&r);

    mp_sqr(&r2,&r2);

    mp_set_zero(&r);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("sqrt(r2)",&r);

    mp_sqr(&r2,&r2);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("sqrt(r2)",&r);

    mp_sqr(&r2,&r2);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("sqrt(r2)",&r);

    mp_sqr(&r2,&r2);
    mp_sqrt(&r2,&r);

    mp_show("r2     ",&r2);
    mp_show("sqrt(r2)",&r);

    mp_free(m1);
    mp_clear(&m2);
    mp_clear(&r);
    mp_clear(&r2);

    test_10_pow_3009_pi();

    fprintf(stdout,"\n== MPI TEST END\n\n");
}



/* -------------------------------------- */

int main(void)
{
    test_mpint();    
    return 0;
}
