/*
    test_primes.c

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

#include "primes.h"


/* -------------------------------------- */

void random_mpi(void)
{
    mp_int_t *r;
    rand_t *rc;

    rc = rand_start(RAND_OS,RAND_GX);

    printf("\nSome random bignums of various bits size:\n");
    if(rc == NULL)
    {
        printf("CANNOT START random generator\n");
        return;
    }

    r = random_bignum(667,rc);
    if(r)
    {
        mp_show("For 667 bits  ",r);
        printf( "In reality has %d bits\n\n",mp_count_bits(r));
        mp_free(r);
    }
    else
    {
        printf("Failed to get 667 bits mpi\n\n");
    }


    r = random_bignum(2048,rc);
    if(r)
    {
        mp_show("For 1024 bits ",r);
        printf( "In reality has %d bits\n\n",mp_count_bits(r));
        mp_free(r);
    }
    else
    {
        printf("Failed to get 1024 bits mpi\n\n");
    }


    r = random_bignum(4096,rc);
    if(r)
    {
        mp_show("For 4096 bits ",r);
        printf( "In reality has %d bits\n\n",mp_count_bits(r));
        mp_free(r);
    }
    else
    {
        printf("Failed to get 4096 bits mpi\n\n");
    }


    r = random_bignum(8192,rc);
    if(r)
    {
        mp_show("For 8192 bits ",r);
        printf( "In reality has %d bits\n\n",mp_count_bits(r));
        mp_free(r);
    }
    else
    {
        printf("Failed to get 8192 bits mpi\n\n");
    }

    rand_end(rc);
}

/* -------------------------------------- */
/* -------------------------------------- */

void closest_prime(int number)
{
    int ret;
    
    ret = closest_small_prime(number);
    printf("Closest small prime to %d is %d\n",number,ret);
    ret = next_small_prime(number);
    printf("Next small prime to %d is %d\n",number,ret);
}

/* -------------------------------------- */

#define NCARMICHAEL 37

const char *carmichael[NCARMICHAEL] = {
    "561",
    "1729",
    "2465",
    "2821",
    "6601",
    "8911",
    "41041",
    "825265",
    "321197185",
    "5394826801",
    "232250619601",
    "9746347772161",
    "1436697831295441",
    "60977817398996785",    
    "7156857700403137441",
    "1791562810662585767521",
    "87674969936234821377601",    
    "1590231231043178376951698401",
    "35237869211718889547310642241",
    "32809426840359564991177172754241",
    "2810864562635368426005268142616001",
    "349407515342287435050603204719587201",
    "125861887849639969847638681038680787361",
    "12758106140074522771498516740500829830401",
    "2333379336546216408131111533710540349903201",
    "294571791067375389885907239089503408618560001",
    "130912961974316767723865201454187955056178415601",
    "13513093081489380840188651246675032067011140079201",
    "7482895937713262392883306949172917048928068129206401",
    "1320340354477450170682291329830138947225695029536281601",
    "379382381447399527322618466130154668512652910714224209601",
    "70416887142533176417390411931483993124120785701395296424001",
    "2884167509593581480205474627684686008624483147814647841436801",
    "4754868377601046732119933839981363081972014948522510826417784001",
    "1334733877147062382486934807105197899496002201113849920496510541601",
    "260849323075371835669784094383812120359260783810157225730623388382401",
    "17013424409194376369553227755505575285717138619698240485929600333778347855414621053360296779596185601",
}; 

/* -------------------------------------- */

void test_is_prime()
{
    mp_int_t p,*q;
    int ret,t;
    rand_t *rc = NULL;
    struct { int p,r,n; } sp[9]={{-1,3,3},{4096,4093,4099},{8191,8191,8209},{8192,8191,9209},{65537,65537,65537},{84096,65537,65537},{52905,52903,52919},{52921,52919,52937},{52919,52919,52937}};     
    rc = rand_start(RAND_GX,123);

    printf("\n*** Test is PRIME ***\n\n");


    printf("--> Small primes START\n");
    printf("small_prime(%d) = %d\n",11,small_prime(11));            
    printf("small_prime(%d) = %d\n",333,small_prime(333));            
    printf("small_prime(%d) = %d\n",(NUM_SMALL_PRIMES / 2),small_prime((NUM_SMALL_PRIMES / 2)));                
    for(t=0;t<9;t++) 
    {
        printf("closest_small_prime(%d)  = %d // should be %d\n",sp[t].p,closest_small_prime(sp[t].p),sp[t].r);
        printf("next_small_prime(%d)     = %d // should be %d\n",sp[t].p,next_small_prime(sp[t].p),sp[t].n);
        printf("is_small_prime(%d) = %d\n",sp[t].p,is_small_prime(sp[t].p));        
        printf("random_small_prime(NULL) = %d \n",random_small_prime(NULL));        
    }
    printf("--> Small primes END\n\n");

    mp_init(&p);
    
    printf("--> Carmichael Set START\n");
    
    for(t=0;t<NCARMICHAEL;t++)
    {
        mp_set_string(&p, carmichael[t],10);
        ret = is_prime(&p,rc);
        printf("is_prime() : ");
        mp_show_decimal((ret) ? "Prime    " : "Composite", &p);        
    }

    printf("--> Carmichael Set END\n\n");

    mp_set_d(&p,41);
    ret = is_prime(&p,rc);
    mp_show_decimal((ret) ? "Prime    " : "Composite", &p);
    
    mp_set_d(&p, 55213);
    ret = is_prime(&p,rc);
    mp_show_decimal((ret) ? "Prime    " : "Composite", &p);
    
    mp_set_d(&p, 52122);
    ret = is_prime(&p,rc);
    mp_show_decimal((ret) ? "Prime    " : "Composite", &p);
    
    mp_set_d(&p, 65521);
    ret = is_prime(&p,rc);
    mp_show_decimal((ret) ? "Prime    " : "Composite", &p);
    
    q = generate_prime(128,rc);   
    ret = is_prime(q,rc);
    mp_show_decimal((ret) ? "Prime    " : "Composite", q);
    
    mp_clear(q);

    q = random_bignum(128,rc);
    ret = is_prime(q,rc);
    printf("Random ");
    mp_show_decimal((ret) ? "Prime    " : "Composite", q);
    
    mp_clear(q);

    q = generate_prime(1208,rc);
    ret = is_prime(q,rc);
    mp_show_decimal((ret) ? "Prime    " : "Composite", q);
    
    mp_clear(q);

    q = random_bignum(1208,rc);
    ret = is_prime(q,rc);
    printf("Random ");
    mp_show_decimal((ret) ? "Prime    " : "Composite", q);
    
    mp_set_d(q,60000);
    printf("\n--> Prime Find START\n");
    mp_show_decimal("Starting point", q);
    for(t=0;t<10;t++)    
    {
        if(!next_prime(q,rc))
        {
            printf("ERROR finding next number\n");
        }
        mp_show_decimal("Next Prime is", q);
    }
    printf("--> Prime Find END\n");
 


    mp_clear(q);
    mp_clear(&p);
    printf("\n");

    rand_end(rc);
}


/* -------------------------------------- */

void test_primes(int alg,int seed,int bits)
{
    rand_t *rc = NULL;
    mp_int_t *bprime;
    int t,ret;
    stopwatch_t sw;

    printf("\n*** Test PRIME ***\n\n");

    rc = rand_start(alg,seed);
    if (rc)
    {
       printf("Generate 10 Primes using %s\n\n", rand_name(rc));

       
        for (t = 0; t < 10; t++)
        {
            printf("Generating %d bits prime...\n",bits);

            stopwatch_start(&sw);

            bprime = generate_prime(bits,rc);
            
            stopwatch_stop(&sw);

            if(bprime)
            {
                mp_show("Done!", bprime);
                printf("Took %.3f seconds\n", stopwatch_elapsed(&sw));

                stopwatch_start(&sw);
                ret = is_prime(bprime,rc);
                stopwatch_stop(&sw);

                printf("Took %.3f seconds to check that is %s\n",stopwatch_elapsed(&sw),(ret) ? "prime" : "NOT prime");

                mp_sub_d(bprime,1,bprime);
                mp_show("minus 1", bprime);

                stopwatch_resume(&sw);
                ret = is_prime(bprime,rc);
                stopwatch_stop(&sw);

                printf("Took %.3f seconds to check that minus 1 is %s\n\n",stopwatch_elapsed(&sw),(ret) ? "prime" : "NOT prime");


                mp_clear(bprime);
            }
            
        }

        rand_end(rc);

        printf("Total took %.3f seconds\n\n", stopwatch_elapsed(&sw));

        closest_prime(-1);     // will return 3
        closest_prime(4096);   // will return 4091
        closest_prime(8192);   // will return 8191
        closest_prime(84096);  // will return 65537

        printf("End of primes test\n\n");

    }
    else
    {
        printf("Error initializing random context with %d\n",alg);
    }
}

/* -------------------------------------- */

void usage(void)
{
    printf("\nTEST PRIMES\n\nuse:\ttest_primes alg [seed] [bits]\n\n"
           "Test different random prime numbers using 'alg' algorithm.\n"
           "'seed' is a number used to seed the algorithm. Unless is 0, which\n"
           "makes the algorithm to be seeded with an umpredictable number.\n"
           "'bits' is the number of bits of the primes (default 768)\n\n"
           "Algorithms:\n\n\t1 = Mother of George Marsaglia (RAND_GM)\n"
           "\t2 = Mersenne Twister of Matsumoto & Nishimura (RAND_MT)\n"
           "\t3 = Xorshift of George Marsaglia (RAND_GX)\n"
           "\t4 = OSSYS of Kenneth MacKay (RAND_OS)\n"
           "\n"
        );

}


/* -------------------------------------- */

int main(int argc,char **argv)
{
    int arg = 1,alg = 0, seed = 0,bits = 768;    

    if(arg < argc)
    {
        alg = atoi(argv[arg++]);
        if(arg < argc)
        {
            seed = atoi(argv[arg++]);        
            if(arg < argc)
                bits = atoi(argv[arg++]);        
        }
    }

    if(alg < RAND_NONE + 1 || alg > RAND_NUM - 1)
    {
        usage();
    }
    else
    {
        random_mpi();
        test_is_prime();   
        test_primes(alg,seed,bits);
    }        
    return 0;
}
