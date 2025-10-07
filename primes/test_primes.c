/*
    test_primes.c

    Test of prime numbers routines

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

void test_10_pow_3009_pi(rand_t *rc)
{
    mp_int_t big_pi;
    int ret;
    stopwatch_t sw;
    char number[16384];

    printf("\n*** BIG PI TEST ***\n");
    ret = mp_init_set_string(&big_pi,pi_x_10_power_3009,10);
    printf("The result of settign the big_pi is %d\n",ret);
    if(ret == MP_OKAY)
    {
        printf("This is a huge number of %lu digits and %d bits\n",strlen(pi_x_10_power_3009),mp_count_bits(&big_pi));
        printf("It is actually the result of multiply PI by 10 power 3009\n");
        mp_show_decimal("big_pi * 10^3009 =",&big_pi);
        printf("\nIt is not prime(), but I know the next one: big_pi + 2183.\nLet's see how long it takes the primarily test...\n\n");
        /* big_pi + 20183 is the first prime after it */
        mp_add_d(&big_pi,2813,&big_pi);
        stopwatch_start(&sw);
        ret = is_prime(&big_pi,rc);
        stopwatch_stop(&sw);
        mp_toradix(&big_pi,(unsigned char *)number,10);
        printf("Is big_pi + 2183 prime? %s. It has %lu digits (%d bits) after %.6f seconds)\n",(ret) ? "YES" : "NO",strlen(number),mp_count_bits(&big_pi),stopwatch_elapsed(&sw));        
        mp_show_decimal("big_pi + 2813   =",&big_pi);        
        mp_clear(&big_pi);
    }
}

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

        printf("Total took %.3f seconds\n\n", stopwatch_elapsed(&sw));

        closest_prime(-1);     // will return 3
        closest_prime(4096);   // will return 4091
        closest_prime(8192);   // will return 8191
        closest_prime(84096);  // will return 65537

        test_10_pow_3009_pi(rc);

        rand_end(rc);

        printf("\nEnd of primes test\n\n");

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
