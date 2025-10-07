/*
    random.h

    Random and pseudo-ramdom numbers generation

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

        - Written by Guillermo Amodeo Ojeda, there is also code from:

            - George Marsaglia, Florida University (Mother and Xorshift)
            - Julienne Walker, Eternallyconfuzzled.com (Mersenne Twister)

        A WORD ABOUT RANDOM GENERATORS...

        I have decided to separate the generation of random numbers from the
        tasks that use them, as this will allow to change a generator if it
        stops being fit for purpose. I believe that this makes the code not
        only flexible, but also more robust.

        Bruce Schneier says in 'Cryptanalytic Attacks on Pseudo-random Number 
        Generators' that all the succesful attacks to crypto-systems directed
        the bulk of the attack to the pseudo-random generator behind it. 
        
        Therefore, I realized that making unbeatable generators was much more
        complex than replace a compromised generator for one which was not. 
        Thus, I separated the code that generates the numbers from the code 
        that uses them to ease the process.

        I did this by abstracting the generators into a struct called rand_t
        which contains pointers to functions for the generators, allowing the
        use of different generators without refactoring the code, as one just 
        need to change the parameters to rand_start() or replacing the later 
        with a call to rand_create() with a new uncompromised generator.

        Here not only have few generators availble, there is also a framework 
        to implement other generators through the mentioned rand_create().

        Look at random.c to study the ones we define here as examples of how to
        implement your own.
        
        ABOUT THE IMPLEMENTATION...

        We use pointers to functions to create the generator abstraction, which work
        mainly through 2 functions: one to generate random bytes and another one to
        generate a 32 bit random number. These are their prototypes:

            unsigned int random_u32_function(void *ctx);

            void random_u8_function(void *ctx,void *dest,size_t num);

        The first function returns a value between 0 and 4294967295 (both included)
        and the second generates 'num' random bytes and puts them in 'dest'.

        'ctx' is the generator's context, which is defined by the creator of the
        generator, and must be initialized after the call to rand_create().

        That function takes the size of the context and the pointers to the two
        functions explained above and then creates a rand_t struct with all
        the info in it -including space for the context- then returns a pointer
        to it.

        The caller should now initialize the context inside the struct (that can
        be accessed through the 'ctx' pointer in it) before allowing the use of the
        generator function. The context should be deinitialized before calling
        rand_end(), which clears the rand_t context and frees the memory used
        by it.        

                                --oO0Oo--
*/

#ifndef RANDOM_STUFF
#define RANDOM_STUFF

#include "hmac.h"

/* -------------------------------------------------- *
   Declare the function pointers which allow casting
   like (rand32_func_t).
* -------------------------------------------------- */

typedef unsigned int (*rand32_func_t)(void *);
typedef void         (*rand8_func_t)(void *,void *,size_t);

/* -------------------------------------------------- *

    The next two pointers are also part of rand_t
    and defined on the call to rand_create().

    They are used to save and load the context of a
    generator so code to study the generator can be
    written.

    Bruce Schneier says in his paper 'Cryptanalytic
    Attacks on Pseudo-random Number Generators' that
    one needs to implement them in order to allow
    your generator to be tested properly.

    This is why you should implement them in any
    generator that you create or simply implement,
    as this allows to abstract the testing code from
    the generator itself.

    Look at examples of them for the provided generators
    in random.c

 * -------------------------------------------------- */

typedef int (*rand_load_func_t)(void *ctx,FILE *fp);
typedef int (*rand_save_func_t)(void *ctx,FILE *fp);

/* -------------------------------------------------- *
   Generators' Context
 * -------------------------------------------------- */

typedef struct
{
    void            *ctx;    /** Context's data */
    size_t           tam;    /** Context's size */

    const char      *namec;   /** Name of the Generator */
    const wchar_t   *namew;   /** Name of the Generator */

    rand32_func_t    func32; /** 32 bits generation function */
    rand8_func_t     func8;  /** Bytes generation function */

    rand_save_func_t save;   /** Save Context function */
    rand_load_func_t load;   /** Load Context function */

    unsigned char    dont_touch[1];         /** Internal data */
} rand_t;

/* ----------------------------------------------------------- */

#define rand_name(_rc)   ((_rc)->namec)
#define rand_namew(_rc)  ((_rc)->namew)

/* ----------------------------------------------------------- *
         Avaliable Generators in rand_start()
 * ----------------------------------------------------------- *

                     ** Mother **

    This is an algorithm that generates 32 bit pseudo-random
    numbers with a periodicity near 2^250.

    It was created by George Marsaglia in the 90s and published
    by Bob Wheeler in 1994, on the forums sc.stat.consult and
    sci.math.num-analysis.

    Marsaglia named it Mother because acording to him was 'The
    Mother of All Random Number Generators'.

    I am not an expert on random numbers, but I think this is
    a very simple and effective generator, as probed by the
    test program test_random.c

    That program tries to generate a given number of random
    numbers that are all different, so if it finds a repeated
    number, it take note of it and keep looking for different
    numbers until it reach its target.

    The seed, the generator and the target number can be
    specified on the command line so algorithms can be tested
    and studied.

    The seed for all the tests was 1234.

    Results for Mother:

          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000      1           0
          200,000      5           0
        2,000,000    488           1 (0x0DE72C23)
        3,000,000   1076           1 (0x0DE72C23)
        5,000,000   2939           2 (0x0DE72C23, 0x48281CB7)
        6,000,000   4195           5 (0x0DE72C23, 0x48281CB7, 0xA1D98BAF, 0xA4D12C83, 0xB1BE31C1)

    Marsaglia died on 2011. RIP, thanks for your legacy.

 * ----------------------------------------------------------- *

                 ** Mersenne Twister **

    This algorithm produces 32 bits pseudo-random numbers with
    a periodicity of (2^19937)−1.

	It was ccreated by Makoto Matsumoto and Takuji Nishimura
	in 1997 to correct the deficiencies of the generators
	found at the time.

    It is called Mersenne twister because it uses a Mersenne 
    prime as his periodiciy.

    It is one of the most used generators in the industry,
    as is used by Ruby, Perl, Python and even Visual C++
    for their implementation of random functions like
    rand().

    Many people think its the best because its periodicity
    is much bigger than any common algorithm and the bit
    distribution is great.

    But apparently is not recommendable for cryptography
    as it has 3 known problems:

    1.- The state array is too big (624 x 32 bits = 2496
        bytes), which apparently causes a lot of cache
        misses on Level 1 caches, slowing down the
        algorithm.

    2.- It is vulnerable to 'State Attacks'. Aparently if
        someone gets hold of a complete sequence of
        generated numbers (624), the state can be reproduced 
        and all the numbers from that point can be predicted.

    3.- If the seed used to initialize the algorithm contains
        too many bits with the value 0, the generator takes
        long time to generate quality numbers.

    I cannot confirm or deny these facts, but you can check
    them by searching the web, as there are plenty references 
    to all these problems, as well as controversy about their 
    veracity.

    The implementation of Mersenne Twister used here was
    originally written by Julienne Walker, who published as
    public domain code. I modified it to adapt it to the
    coding style here.

    The original version was at:

    http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_rand.aspx#mersenne

    Although her website has been changed completely, the
    code is still available at web archives and at github:

    https://github.com/mignon-p/jsw-libs/tree/master/jsw_rand

    As with Mother, I tested it with test_random.c, and these are
    the results (seed was also 1234):

          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000       0          0
          200,000       5          0
        2,000,000     478          0
        3,000,000    1045          0
        5,000,000    2864          0
        6,000,000    4153          0

 * ----------------------------------------------------------- *

                       ** Xorshift 128 **

    This algorithm by George Marsaglia produces pseudo-random
    32 bits numbers with a periodicity of 2^128.

    It is said to probably be the fastest good quality
    pseudo-random generator in the industry, apparently the
    fastest that passes the DieHard test.

    People says its not good for cryptography because it fails
    the tests MatrixRank and LinearComb in the BigCrush suite.

    Again, my knowledge is limited in what makes a generator
    a quality generator for criptography, so I cannot confirm
    or deny these facts.

    I used the orginal version written by Marsaglia himself and
    published in 2003 -> http://www.jstatsoft.org/v08/i14/paper

    The reason why I am not using the 64 bit variant called
    Xorshift128+ and described in this paper:

      http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf

    is because its still to be seen if its a better algorithm
    than the original, even if the big players of the industry
    are using it: JavaScript, Chrome, Safari, Firefox, ...

    I say this because in my tests the 64 bit version is much
    slower than the original, and produces much more repetitions.

    In fact, the 64 bit version is around 30% slower, and the
    number of repetitions cuadruplied, so I discarded it on
    favor of the original 32 bit version. :-(

    As with the previous, I tested it with test_random.c, and
    these are the results (seed was also 1234):

          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000       0           0
          200,000       2           0
        2,000,000     508           0
        3,000,000    1064           0
        5,000,000    2886           0
        6,000,000    4150           1 (0x28445D25)


* ----------------------------------------------------------- *

                        ** TLS-PRF **

    This is an attempt to write a TLS Pseudo-Ramdom Function 
    (PRF) seeding it  with 'seed'. Like the previous, when it 
    is 0, the function takes  as 'use me as true-random' and 
    then it will produce  unpredictable input to generate the 
    random data.

    There are tree versions:

        RAND_TLS_MD5_SHA1 for TLS 1.0 and 1.1's
        RAND_TLS_SHA256 for TLS 1.2's
        RAND_TLS_SHA384 for TLS 1.2's
   
                          IMPORTANT

   Although TLS use this interface for some randomness, mostly
   this is a way to let the standard TLS-PRF be used from other
   applications through the rand_...() functions.
   
   As with the previous, I tested it with test_random.c, and 
   these are the average results (seed was also 1234):

   RAND_TLS_MD5_SHA1:

          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000       0          0
          200,000       7          0
        2,000,000     410          0
        3,000,000    1028          1 (0x23deec34)
        5,000,000    2093          3 (0x23deec34,0x0d8ce403,0x7ea79780)
        6,000,000    4188          5 (0xbdea5718,0x7ea79780,0x23deec34,0xb9bf1f9e,0x0d8ce403)

    RAND_TLS_SHA256:
   
          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000       1          0
          200,000       5          0
        2,000,000     499          0
        3,000,000    1072          1 (0xad480fcb)
        5,000,000    2695          2 (0xad480fcb,0x81e3baf2)
        6,000,000    4242          2 (0x81e3baf2,0xad480fcb)
    
    RAND_TLS_SHA384:

          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000       0          0
          200,000       5          0
        2,000,000     480          0
        3,000,000     992          1 (0x9e4852ca)
        5,000,000    2841          2 (0x9e4852ca,0xa202109b)
        6,000,000    4166          2 (0xa202109b,0x18b2d4a4,0x9e4852ca)

* ----------------------------------------------------------- *

                        ** OSSYS **

    This a true-random generator which produces random bytes
    for cryptography using the device's Operating System
    capabilities, which hopefully would use hardware entropy
    when possible.

    Given the nature of this generator, there is no 'seed'
    which in the call to rand_start() is used to especify
    a fallback generator if a Random Generator is not available
    in the system.

    Saving the state and loading it again should yell different 
    number sequences as soon as the remaining bits of the current 
    buffer are consumed. Thus, it should be *very* complex that 
    the new hash would be the same in two contexts, but is not 
    imposible. Hopefully, it will be complex enough to make 
    impractical to build an attack against it. But -again-
    save/load functions are there so the generator can be studied
    to find problems with it.

    As with the previous, I tested it with test_random.c, and here
    are the results (seed was also 1234):

          Target    1 Repeat   2 Repeats
        ---------   --------   ---------
           20,000      0           0
          200,000      7           0
        2,000,000    454           0
        3,000,000   1000           0
        5,000,000   2827           0
        6,000,000   4092           0

* ----------------------------------------------------------- *
* ----------------------------------------------------------- *

    Now, which algorithm to use for which task is up to you,
    I just made the tests and offer the results, which shows
    them all to be -at least- acceptable algorithms.

    The most noticeable difference was actually the speed, as
    generating the 6 million numbers on a PowerMac G5 Dual 2GHz
    (early 2007) offered the following results in time an the
    amount of generated numbers per second:

        Xorshift128      -->  21.0 seconds. (285,714.2 per second)
        Mother           -->  27.0 seconds. (222,222.2 per second)
        Mersenne Twister -->  31.0 seconds. (193,548.3 per second)
        TLS-PRF-MD5-SHA1 -->   Not yet tested on this system
        TLS-PRF-SHA-256  -->   Not yet tested on this system
        TLS-PRF-SHA-384  -->   Not yet tested on this system
        OSSYS            -->   Not yet tested on this system
    
    Results on a MacPro 3.5 Ghz 6-Core Intel Xeon E5 (Late 2013).

        Xorshift128      -->   Not yet tested on this system
        Mother           -->   Not yet tested on this system
        Mersenne Twister -->   Not yet tested on this system
        OSSYS            -->   Not yet tested on this system
        TLS-PRF-MD5-SHA1 -->   Not yet tested on this system
        TLS-PRF-SHA-256  -->   Not yet tested on this system
        TLS-PRF-SHA-384  -->   Not yet tested on this system

    Results on a MacBook Pro 3.46 Ghz 6-Core Intel Xeon (Mid 2010).

        Xorshift128      -->   1.1 seconds. (5,458,318.1 per second)
        Mother           -->   1.1 seconds. (5,458,318.1 per second)
        Mersenne Twister -->   1.1 seconds. (5,458,318.1 per second)
        OSSYS            -->   2.5 seconds. (2,401,678.4 per second)
        TLS-PRF-MD5-SHA1 -->   48.8 seconds (  122,950.8 per second)
        TLS-PRF-SHA-256  -->   42.8 seconds (  140,845.1 per second)
        TLS-PRF-SHA-384  -->   52.3 seconds (  114,722.7 per second)

    Results on a MacBook Pro 2.5 Ghz Quad-Core Intel Core i7 (Mid 2015).

        Xorshift128      -->   2.8 seconds. ( 2,142,857.1 per second)
        Mother           -->   2.0 seconds. ( 3,000,000.0 per second)
        Mersenne Twister -->   1.9 seconds. ( 3,157,894.7 per second)
        OSSYS            -->   3.7 seconds. ( 1,621,621.6 per second)
        TLS-PRF-MD5-SHA1 -->   Not yet tested on this system
        TLS-PRF-SHA-256  -->   Not yet tested on this system
        TLS-PRF-SHA-384  -->   Not yet tested on this system

    Results on a MacBook Pro M1 8 Core (2020).

        Xorshift128      -->   0.9 seconds. ( 6,666,666.6 per second)
        Mother           -->   1.0 seconds. ( 6,000,000.0 per second)
        Mersenne Twister -->   1.0 seconds. ( 6,000,000.0 per second)
        OSSYS            -->   1.9 seconds. ( 3,157,894.7 per second)
        TLS-PRF-MD5-SHA1 -->   Not yet tested on this system
        TLS-PRF-SHA-256  -->   Not yet tested on this system
        TLS-PRF-SHA-384  -->   Not yet tested on this system


    These speeds include the insertion in a hash table with the
    10% entries of the number of numbers to be generated, so the
    real generation speeds are much faster :-).

    As said, you should take your pick for your intended use
    because -as noted before- some people points out that 'simple'
    generators like Xorshift and Mother are better suited for
    cryptography than Mersenne Twister for the known problems
    explained, but these two algorithms fail some of the BigCrush
    tests, and therefore some people say none of them are actually
    suited for cryptography.

    The TLS stuff have been implemented as to comply with the TLS 
    requirements so it can be used for real TLS imlementations.

    Its dificult to assess OSSYS but it should be pretty good as it
    uses the recommended random devices in Unix, Linux and macOS and 
    in Windows it uses Crypt Services, all of them very well regarded 
    in the industry. The only drawback is that there is not garantee 
    that it would be present in the system, so if you use it, just 
    make sure that your code is ready for the possibility of not being
    available by sending a fallback algorithm in rand_create() or
    -if its crucial to use it- by not sending a fallback and letting
    the function to fail by returning a NULL pointer.

    As said: take your pick, and if none of them convince you just
    use one of your own making/likeness. Which is the advantage of
    the code abstraction used here.

 * ----------------------------------------------------------- *
 * ----------------------------------------------------------- */

enum
{
    RAND_NONE = 0,      /* Undefined */
    RAND_GM,            /* George Marsaglia's Mother. 'param' is a seed */
    RAND_MT,            /* Mersenne Twister. 'param' is a seed */
    RAND_GX,            /* George Marsaglia's Xorshift128. 'param' is a seed */    
    RAND_TLS_MD5_SHA1,  /* MD5+SHA1 based PRF (Default for TLSv1.0 and TLSv1.1) */
    RAND_TLS_SHA256,    /* SHA256 based PRF (Default for TLSv1.2) */
    RAND_TLS_SHA384,    /* SHA384 base PRF (Especified by connection since TLSv1.2) */
    RAND_OS,            /* OSSYS generator. 'param' is fallback algorithm if not present  */    
    RAND_NUM,
    /* Always last */
    RAND_TEST,       /** Test random generator (look at rand_tester() below) */
};

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------- *
    Creates a context for a random number generator
    that uses a context of 'len' bytes in size and
    the 4 functions to:

        - generate 32 bits numbers,
        - generate bytes, 
        - load and save the context.

    Above in ABOUT THE IMPLEMENTATION there is an
    explantion of these 4 functions, which
    are used through the rand_...() functions.
 * ----------------------------------------------------------- */

rand_t *rand_create(size_t len,rand32_func_t f32,rand8_func_t f8,rand_save_func_t fsave,rand_load_func_t fload);

/* ----------------------------------------------------------- *
   Saves the status of a generator to fp.
   Returns 0 if works or error if it doesn't.    
 * ----------------------------------------------------------- */

int rand_save_status(rand_t *rand,FILE *fp);

/* ----------------------------------------------------------- *
   This function load the status of the generator 'status'
   into 'rand' 'len' is the size of 'status'.
 * ----------------------------------------------------------- */

int rand_load_status(rand_t *rand,FILE *fp);

/* ----------------------------------------------------------- *
    Uses rand_create() to create a generator context for one
    of the described above and implemented by us.

   'which' is the algorithm to use in the returned context.
    Before returning the context it is initialized using
    the 'param' parameter, which is explained in each
    generator. Send 0 for the default behaviour, which
    in the case of 'param' being a seed, it means 'please
    use an unpredictable seed'.
 * ----------------------------------------------------------- */

rand_t *rand_start(int cual,unsigned int param);

/* ----------------------------------------------------------- *
   Creates a random test context that provides known random 
   sequences so routines that consume random numbers can be 
   tested with known random sequences.

    'buf' is the buffer that contain the sequence to be 
    returned and 'len' is the size in bytes of 'buf'.

    IMPORTANT:  The generator produced by this routine repeats the
                sequence of values in 'buf' over an over, so make
                sure you provide enough values to cover your test
                and make sure you consume them in the proper order
                and size to make your tests meaningful.
*/

rand_t *rand_tester(void *buf,size_t len);

/* ----------------------------------------------------------- *
    Free and clean a generator making it unusable. ;-) 
 * ----------------------------------------------------------- */

void rand_end(rand_t *rc);


/* -------------------------------------------------------------- *
        Generate random data EXACTLY in the same way as TLS does, so 
    it can be used for TLS as well as  for other applications.

    'prf' is one of RAND_TLS_MD5_SHA1, RAND_TLS_SHA256 or 
     RAND_TLS_SHA384, any other gets silently discarded and
    set tothe default RAND_TLS_SHA256.

    Refer to section 5 of RFC-4346 and RFC-5246 to see how does
    s9_rand_tls_prf() works.
* -------------------------------------------------------------- */ 

void rand_tls_prf(int prf,void *sec,int tam_sec,const char *label,void *s1,int tam_s1,void *s2,int tam_s2,void *ret,size_t num);

/* ----------------------------------------------------------- *
   Returns a random integer between -2147483647 and 2147483647 
   using the context 'rc'
 * ----------------------------------------------------------- */

int rand_integer(rand_t *rc);

/* ----------------------------------------------------------- *
   Returns a random unsigned integer between 0 and 4294967295 
   using the context 'rc'
 * ----------------------------------------------------------- */

unsigned int rand_unsigned(rand_t *rc);

/* ----------------------------------------------------------- *
   Returns a single random byte from 1 to 254 so neither all 
   bits are the same (nor 0 nor 1)
 * ----------------------------------------------------------- */

unsigned char rand_byte(rand_t *rc);

/* ----------------------------------------------------------- *
   Returns a random double between 0.0 and 4294967296.0 
   using the context 'rc' 
 * ----------------------------------------------------------- */

double rand_double(rand_t *rc);

/* ----------------------------------------------------------- *
   Returns a random double between 0.0 and 1.0 using the 
   context 'rc'
 * ----------------------------------------------------------- */

double rand_decimal(rand_t *rc);

/* ----------------------------------------------------------- *
    Generates 'num' random bytes in 'data' using the context 
    'rc' 
 * ----------------------------------------------------------- */

void rand_bytes(rand_t *rc,void *data,size_t num);

/* ----------------------------------------------------------- *
    Same as the previous but no byte will have the value 0 
 * ----------------------------------------------------------- */

void rand_bytes_no_zeros(rand_t *rc,void *data,size_t num);

/* ----------------------------------------------------------- *
    Returns a 64 bits random number using the context 'rc' 
 * ----------------------------------------------------------- */

u64_t rand_u64(rand_t *rc);

/* ----------------------------------------------------------- *
    Produces a random number with a determinated number of 
    bits using the context 'rc'.

    'bits' is the number of bits you want to generate. This  
    means the top bit of the set is always set.

    The number of bits is used to calculate the size of the
    recipient buffer, assumed to be in big endian although 
    compatible with the machine endian.

    The asumed size in bits for buf is (bits + 7) / 8
 
* ----------------------------------------------------------- */

void  rand_bits(rand_t *rc,void *buf, int bits);

/* ------------------------------------------------------------------- *
    These functions generate a string ID compatible with Firestore's 
    AutoId generation, as they use the same technique than function
    AutoId:newId(), which is the function that generates all the IDs 
    used  by the Firestore's Javascript SDK.

    Auto:newId() can be found in the file misc.ts inside the directory 
    utils in the source code of the mentioned SDK, at:

    https://github.com/firebase/firebase-js-sdk/blob/main/packages/firestore/src/util/misc.ts

    On my view this is a risky way of generating IDs that are assumed
    unique and unpredictable, as nothing is actually done to ensure 
    their uniqueness besides hoping that Math.random() doesn't repeat
    sequences of numbers.

    Even if this repetition is very difficult, it is not impossible,
    and as some experts said: "Random numbers are too important to be
    left to chance". 
    
    We chose to use the original Marsaglia's Xorshift128 and not 
    Xorshitf128+, which replaced MWC1616 in V8 v4.9.41.0 (see details in
    https://v8.dev/blog/math-random).

        firestore_auto_id()  -> Same as V8
        firestore_auto_idw() -> Same as V8 (wchar)

    Note that these functions can generate IDs smaller or longer than 
    20 chars by changing 'max', which is assumed to be the lengh of
    the desired string + 1 for the NUL. ;-)
 * ------------------------------------------------------------------- */

char    *firestore_auto_id(char *string,size_t max);
wchar_t *firestore_auto_idw(wchar_t *string,size_t max);


/* -------------------------------------------- *
    This function generates an unpredictable 
    amount of data hashed with the specified
    hash algorithm. 
    
                 *** WARNING ***

    You may be tempted to use it as random data 
    generator, but I discorage it if you are 
    going to do something serious with the 
    generated data. 
    
    Unless of course you have measured the risk,
    or doing something like generating a random
    IV.
    
    Random data generation is not a trivial 
    business, specially for use in criptography.
 * -------------------------------------------- */

void hash_get_entropy(int hash,void *dest,size_t num);


#ifdef __cplusplus
};
#endif

#endif
