/*
    primes.h

    Prime number generator

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

        - Written by Guillermo Amodeo Ojeda using some code from SSHPRIME.C
          that came with PUTTY 0.60.

        - Many comments come from the original code in putty files but
          modified to match the changes made on them.        

                                --oO0Oo--
*/



#ifndef PRIME_NUMBER_GENERATOR_ADAPTED_FROM_PUTTY
#define PRIME_NUMBER_GENERATOR_ADAPTED_FROM_PUTTY

#include "random.h"
#include "mpint.h"

#define NUM_SMALL_PRIMES        6541   /* Number of primes between 0 and 65535 */

#define SMALLEST_SMALL_PRIME       3   /* Smallest 'small prime' we manage */
#define BIGGEST_SMALL_PRIME    65537   /* Biggest 'small prime' we manage */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    Returns a prime between 3-65537 (both included).

    Yeah 2 is prime, but not useful for the intended uses
    we want these functions to be used for

    'pos' is the position of the prime in the table,
    which is 0 to NUM_SMALL_PRIMES-1 (3 - 65521).

    If 'pos' is less than 0 returns 3 and if equal 
    or bigger than NUM_SMALL_PRIMES it returns 65537
 * -------------------------------------------------- */

int small_prime(int pos);

/* -------------------------------------------------- *
    It returns the closest small prime to the given
    number, which could be the number itself if prime. 
    
    It will never return a prime smaller than 3 neiter 
    bigger than 65537.

    Yeah 2 is prime, but not useful for the intended uses
    we want these functions to be used for

    Examples:

        closest_small_prime(-1);     // will return 3
        closest_small_prime(4096);   // will return 4093
        closest_small_prime(8191);   // will return 8191
        closest_small_prime(8192);   // will return 8191
        closest_small_prime(65537);  // will return 65537
        closest_small_prime(84096);  // will return 65537
 * -------------------------------------------------- */

int closest_small_prime(int number);

/* -------------------------------------------------- *
    It returns the next small prime to the given
    number, even if the number is not prime, 
    
    It will never return a prime smaller than 3 neiter 
    bigger than 65537.

    Yeah 2 is prime, but not useful for the intended uses
    we want these functions to be used for

    Examples:

        next_small_prime(-1);     // will return 3
        next_small_prime(4096);   // will return 4099
        next_small_prime(8191);   // will return 8209
        next_small_prime(8192);   // will return 8209
        next_small_prime(65537);  // will return 65537
        next_small_prime(84096);  // will return 65537

 * -------------------------------------------------- */

int next_small_prime(int number);

/* -------------------------------------------------- *
    Returns a random prime between 3-65537

    Yeah 2 is prime, but not useful for the intended uses
    we want these functions to be used for

    If the random context 'rc' is not NULL it will 
    use it to find the prime.
    
    Otherwise will use an umpredictable number to 
    find it.    
 * -------------------------------------------------- */

int random_small_prime(rand_t *rc);

/* -------------------------------------------------- *
   Returns TRUE/FALSE 'prime' is a prime in the
   range 2-65537 (both included).
 * -------------------------------------------------- */

int is_small_prime(int prime);

/* -------------------------------------------------- *
 *
 * THIS IS AN ORIGINAL COMMENT FROM SSHPRIME.C
 *
 * Generate a prime. We can deal with various extra properties of
 * the prime:
 *
 *  - to speed up use in RSA, we can arrange to select a prime with
 *    the property (prime % modulus) != residue.
 *
 *  - If modulus is not a small prime uses 37 (Putty's exponent)
 *
 *  - for use in DSA, we can arrange to select a prime which is one
 *    more than a multiple of a dirty great bignum. In this case
 *    `bits' gives the size of the factor by which we _multiply_
 *    that bignum, rather than the size of the whole number.
 *
 * END OF ORIGINAL COMMENT FROM SSHPRIME.C
 *
 *
 * Guilermo Amodeo Ojeda added the random context 'rc' so
 * callers will choose the random generator, instead of the
 * library choosing one for them.
 *
 * The random context 'rc' cannot be NULL
 *
 * -------------------------------------------------- */

mp_int_t *quality_prime(int nbits, int modulus, int residue, mp_int_t *factor,unsigned firstbits,rand_t *rc);

/* -------------------------------------------------- *
   Generates a prime of 'nbits' bits long with no
   special properties (uses quality_prime() with
   a small modulus, but unlike that function, this 
   one returns a small prime other than 65537 if 
   'nbits' is smaller than 17.

   The random context 'rc' cannot be NULL
 * -------------------------------------------------- */

mp_int_t *generate_prime(int nbits,rand_t *rc);

/* -------------------------------------------------- *
    Uses Miller-Rabin primality test to determine if
    the number p is prime

    If the random context 'rc' is NULL it will use
    the OS random generator, and if none is present,
    it will use George Marsaglia's Xorshift128 with
    a random seed.
 * -------------------------------------------------- */

int is_prime(mp_int_t *p,rand_t *rc);

/* -------------------------------------------------- *
   It returns the next prime after the given number, 
   even if the number is not a prime. IT MODIFIES 
   'number' as it does return the new prime there.

   WARNING: This function can take long time to run

   If the random context 'rc' is NULL it will use
   the OS random generator, and if none is present,
   it will use George Marsaglia's Xorshift128 with
   a random seed.
* -------------------------------------------------- */

int next_prime(mp_int_t *number,rand_t *rc);

/* -------------------------------------------------- *
   Generates a bignum of EXACTLY 'nbits' bits long

   The random context 'rc' cannot be NULL
 * -------------------------------------------------- */

mp_int_t *random_bignum(int nbits,rand_t *rc);

/* ------------------------------------------------------------------- *
 *
 * THIS IS AN ORIGINAL COMMENT FROM SSHPRIME.C
 *
 * Invent a pair of values suitable for use as 'firstbits' in the
 * above function, such that their product is at least 2.
 *
 * This is used for generating both RSA and DSA keys which have
 * exactly the specified number of bits rather than one fewer - if you
 * generate an a-bit and a b-bit number completely at random and
 * multiply them together, you could end up with either an (ab-1)-bit
 * number or an (ab)-bit number. The former happens log(2)*2-1 of the
 * time (about 39%) and, though actually harmless, every time it
 * occurs it has a non-zero probability of sparking a user email along
 * the lines of 'Hey, I asked PuTTYgen for a 2048-bit key and I only
 * got 2047 bits! Bug!'
 * ------------------------------------------------------------------- */

void invent_firstbits(unsigned *one, unsigned *two,rand_t *rc);

#ifdef __cplusplus
};
#endif


#endif
