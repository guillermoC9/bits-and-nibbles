/*

    mpint.h

    Multiprecisicon Math Library

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

        - Adapted and complemented by Guillermo Amodeo Ojeda from Michael J. Fromberger's
          implementation which he donated to the public domain in 2002.

        - The original code can be found at https://fossies.org/linux/txr/mpi/mpi.h

        - IMPORTANT:   This file is a mess in terms of parameters order and you 
                       need to be extra careful with the parameter calling. 
                       This is due to me having the habit of using the order like
                       where func(c,a,b) to say c = a + b, and Michael here used
                       mostly: function(a,b,c) to say a + b = c.

                       As I was adapting the file to my needs, I was quite clumsy and 
                       added to the mess instead of fixing it. :-(

                                --oO0Oo--
*/

#ifndef MIKE_FROMBERGER_MPI
#define MIKE_FROMBERGER_MPI


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#ifndef FALSE

#define FALSE   0
#define TRUE    1

#endif


/* --------------------------------- *
   Make sure unsigned int is 32 bits
 * --------------------------------- */

 #if UINT_MAX !=  0xffffffffu
#error "unsigned int is expected to have at least 32 bits"
#endif

/* ---------------------------------------------- *
   Detect target operating system. If not Mac or 
   Windows, asume Linux or UNIX (BSD, AIX,...)   
 * ---------------------------------------------- */

#if defined(__APPLE__)

#define FOR_MAC     /* This is for Macintosh */

#elif defined(_WIN32)

#define FOR_WIN     /* This is for Windows */

#else 

#define FOR_NIX      /* This is for Unix-like */

#endif

/* --------------------------------- *
   We need fixed types for some suff 
 * --------------------------------- */

 typedef unsigned int  mp_digit_t;

#ifdef FOR_WIN

typedef unsigned __int64    mp_word_t;

/* Microsoft C has different names than the rest for useful functions */

#define snprintf        _snprintf
#define strcasecmp      _stricmp
#define strncasecmp     _strnicmp

#else 

typedef unsigned long long  mp_word_t;

#endif

#define MP_DIGIT_BYTES  4
#define MP_DIGIT_BITS   32
#define MP_DIGIT_LOG    31


/* Multiprecision integer */

typedef struct
{
    int                sign;     /* sign of this quantity */
    unsigned int       alloc;    /* how many digits allocated */
    unsigned int       used;     /* how many digits used */
    mp_digit_t        *dp;       /* the digits themselves */
} mp_int_t;

/* Macros */

#define MP_NEG          1
#define MP_ZPOS         0

#define MP_OKAY         0    /* no error, all is well */
#define MP_YES          0    /* yes (boolean result) */
#define MP_NO          -1    /* no (boolean result) */
#define MP_MEM         -2    /* out of memory */
#define MP_RANGE       -3    /* argument out of range */
#define MP_BADARG      -4    /* invalid parameter */
#define MP_UNDEF       -5    /* answer is undefined */
#define MP_TOOBIG      -6    /* number is too large */

#define MP_LAST_CODE    MP_TOOBIG

#define MP_LT          -1
#define MP_EQ           0
#define MP_GT           1

/* Macros for accessing the mp_int_t internals */

#define mp_sign(_MP)    ((_MP)->sign)
#define mp_isneg(_MP)   ((_MP)->sign == MP_NEG)
#define mp_used(_MP)    ((_MP)->used)
#define mp_alloc(_MP)   ((_MP)->alloc)
#define mp_digits(_MP)  ((_MP)->dp)
#define mp_digit(_MP,N)  (_MP)->dp[(N)]

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* Static one digit numbers */

mp_int_t *mp_zero(void);
mp_int_t *mp_one(void);
mp_int_t *mp_two(void);
mp_int_t *mp_three(void);
mp_int_t *mp_four(void);
mp_int_t *mp_five(void);
mp_int_t *mp_six(void);
mp_int_t *mp_seven(void);
mp_int_t *mp_eight(void);
mp_int_t *mp_nine(void);

unsigned int mp_get_prec(void);

void mp_set_prec(unsigned int prec);

/*  Return if a mp is odd */

int mp_is_odd(mp_int_t *mp);

/* Returns a true (non-zero) value if mp is zero, false (zero) otherwise. */

#define mp_is_zero(_mp)     (mp_cmp_z(_mp) == MP_EQ)

/* Returns a true (non-zero) value if mp is 1, false (zero) otherwise. */

int mp_is_one(mp_int_t *mp);


/* Initialize a new zero-valued mp_int.  Returns MP_OKAY if successful,
 * MP_MEM if memory could not be allocated for the structure.
 */

int mp_init(mp_int_t *mp);

/* Same as previous but using a string in base 'radix' */

int mp_init_set_string(mp_int_t *mp,const char *str,int radix);
int mp_init_set_wide_string(mp_int_t *mp,const wchar_t *str,int radix);

/* Initialize a new zero-valued mp_int_t with at least the given
 * precision; returns MP_OKAY if successful, or MP_MEM if memory could
 * not be allocated for the structure.
 */

int mp_init_size(mp_int_t *mp, unsigned int prec);

/* Initmp init  and set a digit */

int mp_init_set_d(mp_int_t *mp,mp_digit_t d);


/* Creates a dynamic MPI */

mp_int_t *mp_create(void);

/* Makes a dynamic copy of a MPI */

mp_int_t *mp_dup(mp_int_t *mp);

/* Frees a dynamic MPI */

void mp_free(mp_int_t *mp);

/* Initialize a mp_int_t with a given list of digits.
 * It returns MP_OKAY if successful, MP_BADARG if
 * arguments are invalid (although 0 in count will
 * create an mp_int_t with value 0) or MP_MEM if memory
 * could not be allocated for the structure.
 *
 * THIS FUNCTION is intended to be used to convert from
 * other big number formats.
 *
 *                   IMPORTANT
 *
 *    The order of the digits is BIG ENDIAN, which
 *    means the first digit is the less significant.
 *
 *    This is opposed at the way that this library
 *    stores the digits, which is little endian order.
 *
 */

int mp_init_set_digits(mp_int_t *mpi,unsigned int count,mp_digit_t *lst);

/* Initialize a mp_int_t with a given list of bytes.
 * It returns MP_OKAY if successful, MP_BADARG if
 * arguments are invalid (although 0 in count will
 * create an mp_int_t with value 0) or MP_MEM if memory
 * could not be allocated for the structure.
 *
 * THIS FUNCTION is intended to be used to convert from
 * other big number formats.
 *
 *                   IMPORTANT
 *
 *    The order of the bytes is BIG ENDIAN, which
 *    means the first byte is the less significant.
 *
 */

int mp_init_set_bytes(mp_int_t *mpi,unsigned int count,const void *lst);

/* Same as previous but using a string in base 'radix' */

int mp_init_set_string(mp_int_t *mp,const char *str,int radix);
int mp_init_set_wide_string(mp_int_t *mp,const wchar_t *str,int radix);

/* Initialize mp as an exact copy of from.  Returns MP_OKAY if
 * successful, MP_MEM if memory could not be allocated for the new
 * structure.
 */

 int mp_init_copy(mp_int_t *mp, mp_int_t *from);

/* Copies the mp_int_t 'from' to the mp_int_t 'to'.  It is presumed that
 * 'to' has already been initialized (if not, use mp_init_copy()
 * instead). If 'from' and 'to' are identical, nothing happens.
 */

int mp_copy(const mp_int_t *from, mp_int_t *to);

/* Copy 'num' bytes from mpi to 'dest'

   it returns MP_OKAY if the number of bytes
   copied are actually the correct number
   of bytes of the number or MP_RANGE if
   not.

   It will return MP_BADARG if any of the
   parameters is wrong

*/

int mp_copy_bytes(mp_int_t *mpi,void *dest,unsigned int num);

/*  Same as previous but instead of falling if the size
    is incorrect, it will pad the bytes with 0 at the left
    if the buffer is bigger than the actual MPI or will cut
    the number from the left if the buffer is maller.

    Example:   1234567

        mp_copy_bytes(...,10) = 0001234567
        mp_copy_bytes(...,5)  = 34567
*/

int mp_copy_exact_bytes(mp_int_t *mp, void *data, unsigned int num);

/* Same as mp_copy_bytes() but instead it copies the bytes
   as an hexadecimal string
*/

int mp_copy_string(mp_int_t *mpi,char *dest,int num);

/* Exchange mp1 and mp2 without allocating any intermediate memory
 * (well, unless you count the stack space needed for this call and the
 * locals it creates...).  This cannot fail.
 */

void mp_exch(mp_int_t *mp1, mp_int_t *mp2);

/* Release the storage used by an mp_int, and void its fields so that
 * if someone calls mp_clear() again for the same int later, we won't
 * get tollchocked. It also frees the mp struct itself if the number
 * was created with mp_create()
 */

void mp_clear(mp_int_t *mp);

/* Set mp to zero.  Does not change the allocated size of the structure,
 * and therefore cannot fail (except on a bad argument, which we ignore)
 */

void mp_set_zero(mp_int_t *mp);

/* Set mp to various single digit values */

void mp_set_d(mp_int_t *mp, mp_digit_t d);
int  mp_set_int(mp_int_t *mp, long z);
int  mp_set_word(mp_int_t *mp, mp_word_t w, int sign);

/* Copies 'count' mp_digits_t to 'lst'.
 *
 * Set 'le' as TRUE to get the digits in little
 * endian order or as FALSE to get them in big endian
 * order.
 *
 * THIS FUNCTION is intended to be used to convert to
 * other big number formats.
 */

int mp_export_digits(mp_int_t *mpi,mp_digit_t *lst,size_t count,int le);

/* The opposite, but now 'le' determines the order of 'lst' */

int mp_import_digits(mp_int_t *mpi,mp_digit_t *lst,size_t count,int le);


/* Returns an array of mp_digits_t with all the digits
 * of the mp_int_t and puts the number of elements of
 * the array in 'count'. It will return NULL if the
 * arguments are invalid (ie: any of them is NULL)
 * or if memory could not be allocated for the array.
 *
 * When you are done with the array call free() on it.
 *
 * THIS FUNCTION is intended to be used to convert to
 * other big number formats.
 */

mp_digit_t *mp_get_digits(mp_int_t *mpi,unsigned int *count);

/* Same as previous but it returns bytes rather than digits */

unsigned char *mp_get_bytes(mp_int_t *mpi,unsigned int *count);

/* Return number as a wchar_t string of hexa chars or decimal */

wchar_t *mp_get_wide_string(mp_int_t *mpi);
wchar_t *mp_get_wide_string_decimal(mp_int_t *mpi);

/*
 * Get the last digit of a mp_int_t as int
 * Intended to convert small mp_int_t to int
 */

int mp_get_int(mp_int_t *mp);

/* set MPI to bytes and strings */

int mp_set_bytes(mp_int_t *mpi,const void *lst,unsigned int count);
int mp_set_string(mp_int_t *mp,const char *str,int radix);
int mp_set_wide_string(mp_int_t *mp,const wchar_t *str,int radix);

/* Compute the sum b = a + d, for a single digit d.  Respects the sign of
 * its primary addend (single digits are unsigned anyway).
 */

int mp_add_d(mp_int_t *a, mp_digit_t d, mp_int_t *b);

/* Compute the difference b = a - d, for a single digit d.  Respects the
 * sign of its subtrahend (single digits are unsigned anyway).
 */

int mp_sub_d(mp_int_t *a, mp_digit_t d, mp_int_t *b);

/* Compute the product b = a * d, for a single digit d.  Respects the sign
 * of its multiplicand (single digits are unsigned anyway)
 */

int mp_mul_d(mp_int_t *a, mp_digit_t d, mp_int_t *b);

/* Compute the product c = a * 2 */

int mp_mul_2(mp_int_t *a, mp_int_t *c);

/* Compute the quotient q = a / d and remainder r = a mod d, for a
 * single digit d.  Respects the sign of its divisor (single digits are
 * unsigned anyway).
 */

int mp_div_d(mp_int_t *a, mp_digit_t d, mp_int_t *q, mp_digit_t *r);

/* Compute c = a / 2, disregarding the remainder.  */

int mp_div_2(mp_int_t *a, mp_int_t *c);

/* Compute a /= 2, disregarding the remainder.  */

int mp_div_by_2(mp_int_t *a);

/* Compute c = a ** d, that is, raise a to the power of single digit d. */

int mp_expt_d(mp_int_t *a, mp_digit_t d, mp_int_t *c);

/* Compute b = |a|.  'a' and 'b' may be identical.  */

int mp_abs(mp_int_t *a, mp_int_t *b);

/* Compute b = -a.  'a' and 'b' may be identical.  */

int mp_neg(mp_int_t *a, mp_int_t *b);

/* Compute c = a + b.  All parameters may be identical.  */

int mp_add(mp_int_t *a, mp_int_t *b, mp_int_t *c);

/* Compute c = a - b.  All parameters may be identical.  */

int mp_sub(mp_int_t *a, mp_int_t *b, mp_int_t *c);

/* Compute c = a * b.  All parameters may be identical.  */

int mp_mul(mp_int_t *a, mp_int_t *b, mp_int_t *c);

/* Compute d = a * b + c.  All parameters may be identical.  */

int mp_mul_add(mp_int_t *a, mp_int_t *b, mp_int_t *c, mp_int_t *d);

/* Compute c = a * 2^d.  a may be the same as c.  */

int mp_mul_2d(mp_int_t *a, mp_digit_t d, mp_int_t *c);

/* Compute b = a * a.  a may be the same as b.  */

int mp_sqr(mp_int_t *a, mp_int_t *b);

/* Compute q = a / b and r = a mod b.  Input parameters may be re-used
 * as output parameters.  If q or r is NULL, that portion of the
 * computation will be discarded (although it will still be computed)
 * Pay no attention to the hacker behind the curtain.
 */

int mp_div(mp_int_t *a, mp_int_t *b, mp_int_t *q, mp_int_t *r);

/* Compute q = a / 2^d  and r = a mod 2^d */

int mp_div_2d(mp_int_t *a, mp_digit_t d, mp_int_t *q, mp_int_t *r);

/* Compute c = a ** b, that is, raise a to the b power.  Uses a
 * standard iterative square-and-multiply technique.
 */

int mp_expt(mp_int_t *a, mp_int_t *b, mp_int_t *c);

/* Compute a = 2^k */

int mp_2expt(mp_int_t *a, mp_digit_t k);

/* Compute b = square root of a */

int mp_sqrt(mp_int_t *a, mp_int_t *b);

/* Compute c = a (mod m).  Result will always be 0 <= c < m.  */

int mp_mod(mp_int_t *a, mp_int_t *m, mp_int_t *c);

/* Compute c = a (mod single digit d).  Result will always be 0 <= c < d */

int mp_mod_d(mp_int_t *a, mp_digit_t d, mp_digit_t *c);

/* Compute c = (a + b) mod m */

int mp_addmod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c);

/* Compute c = (a - b) mod m */

int mp_submod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c);

/* Compute c = (a * b) mod m */

int mp_mulmod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c);

/* Compute c = (a ** a) mod m. */

int mp_sqrmod(mp_int_t *a, mp_int_t *m, mp_int_t *c);

/* Compute c = (a ** b) mod m.  Uses a standard square-and-multiply
 * method with modular reductions at each step. (This is basically the
 * same code as mp_expt(), except for the addition of the reductions)
 * The modular reductions are done using Barrett's algorithm (see
 * s_mp_reduce() below for details)
 */

int mp_exptmod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c);

/* Single digit version of the previous */

int mp_exptmod_d(mp_int_t *a, mp_digit_t d, mp_int_t *m, mp_int_t *c);

/* Compare a <=> 0.  Returns <0 if a<0, 0 if a=0, >0 if a>0.  */

int mp_cmp_z(mp_int_t *a);

/* Compare a <=> c.  Returns <0 if a<d, 0 if a=d, >0 if a>d */

int mp_cmp(mp_int_t *a, mp_int_t *b);

/* Single digit version */

int mp_cmp_d(mp_int_t *a, mp_digit_t d);

/* Compares |a| <=> |b|, and returns an appropriate comparison result */

int mp_cmp_mag(mp_int_t *a, mp_int_t *b);

/* This just converts z to an mp_int, and uses the existing comparison
 * routines.  This is sort of inefficient, but it's not clear to me how
 * frequently this wil get used anyway.  For small positive constants,
 * you can always use mp_cmp_d(), and for zero, there is mp_cmp_z().
 */

int mp_cmp_int(mp_int_t *a, long z);

/* Returns a true (non-zero) value if a is odd, false (zero) otherwise. */

int mp_isodd(mp_int_t *a);

/* Returns a true (non-zero) value if a is even, false (zero) otherwise. */

int mp_iseven(mp_int_t *a);

/* Compute c = gcd(a,b)  - Greatest Common Divisor of a and b */

int mp_gcd(mp_int_t *a, mp_int_t *b, mp_int_t *c);

/* Compute  c = lcm(a,b) - Least Common Multiple of a and b */

int mp_lcm(mp_int_t *a, mp_int_t *b, mp_int_t *c);

/* Compute g = (a, b) and values x and y satisfying Bezout's identity
 * (that is, ax + by = g).  This uses the extended binary GCD algorithm
 * based on the Stein algorithm used for mp_gcd()
 */

int mp_xgcd(mp_int_t *a, mp_int_t *b, mp_int_t *g, mp_int_t *x, mp_int_t *y);

/* Compute c = a^-1 (mod m), if there is an inverse for a (mod m).
 * This is equivalent to the question of whether (a, m) = 1.  If not,
 * MP_UNDEF is returned, and there is no inverse.
 */

int mp_invmod(mp_int_t *a, mp_int_t *m, mp_int_t *c);

/* Convert a's bit vector to its two's complement, up to the
 * number of words that it contains, storing result in b. The numeric value of
 * this result depends on the size of mpi_digit. This is a building block for
 * handling negative operands in the bit operations.
 */

int mp_2comp(mp_int_t *a, mp_int_t *b, unsigned int dig); /* peculiar semantics */

/* Binary Operations */

int mp_and(mp_int_t *a, mp_int_t *b, mp_int_t *c);
int mp_or(mp_int_t *a, mp_int_t *b, mp_int_t *c);
int mp_xor(mp_int_t *a, mp_int_t *b, mp_int_t *c);

int mp_comp(mp_int_t *a, mp_int_t *b);

int mp_trunc_comp(mp_int_t *a, mp_int_t *b, unsigned int bits);
int mp_trunc(mp_int_t *a, mp_int_t *b, unsigned int bits);
int mp_shift(mp_int_t *a, mp_int_t *b, int bits); /* + left, - right */
int mp_bit(mp_int_t *a, unsigned int bit);

/* Get the value of bit n */

int mp_get_bit(mp_int_t *a, unsigned int n);

/* Set the value of bit n */

int mp_set_bit(mp_int_t *a, unsigned int bit,unsigned int value);

/* Convert to double */

int mp_to_double(mp_int_t *mp, double *d);

/* Print a textual representation of the given mp_int_t on the output
 * stream 'ofp'.  Output is generated using the given radix.
 */

void mp_print(mp_int_t *mp, int radix, FILE *ofp);

void mp_show(const char *prefix,mp_int_t *mp);

void mp_show_decimal(const char *prefix,mp_int_t *mp);

/* Read in a raw value (base 256) into the given mp_int_t */

int mp_read_signed_bin(mp_int_t *mp, unsigned char *str, size_t len);

size_t mp_signed_bin_size(mp_int_t *mp);
int mp_to_signed_bin(mp_int_t *mp, unsigned char *str);

/* Read in an unsigned value (base 256) into the given mp_int_t */

int mp_read_unsigned_bin(mp_int_t *mp, unsigned char *str, size_t len);
size_t mp_unsigned_bin_size(mp_int_t *mp);


int mp_to_unsigned_bin(mp_int_t *mp, unsigned char *str);
int mp_to_unsigned_buf(mp_int_t *mp, unsigned char *str, size_t size);

unsigned int mp_count_bits(mp_int_t *mp);
int mp_count_ones(mp_int_t *mp);
unsigned int mp_is_pow_two(mp_int_t *mp);

/* Compatibility macros */

#define mp_read_raw(mp, str, len)   mp_read_signed_bin((mp), (str), (len))
#define mp_raw_size(mp)             mp_signed_bin_size(mp)
#define mp_toraw(mp, str)           mp_to_signed_bin((mp), (str))
#define mp_read_mag(mp, str, len)   mp_read_unsigned_bin((mp), (str), (len))
#define mp_mag_size(mp)             mp_unsigned_bin_size(mp)
#define mp_tomag(mp, str)           mp_to_unsigned_bin((mp), (str))

/* Read an integer from the given string, and set mp to the resulting
 * value.  The input is presumed to be in base 10.  Leading non-digit
 * characters are ignored, and the function reads until a non-digit
 * character or the end of the string.
 */

int mp_read_radix(mp_int_t *mp, const wchar_t *str, int radix);
int mp_read_radix_char(mp_int_t *mp, const char *str, int radix);

unsigned int mp_radix_size(mp_int_t *mp, int radix);

/* Return the number of digits in the specified radix that would be
 * needed to express 'num' digits of 'qty' bits each.
 */

unsigned int mp_value_radix_size(unsigned int num, unsigned int qty, int radix);

int mp_toradix(mp_int_t *mp, unsigned char *str, int radix);
int mp_toradix_case(mp_int_t *mp, unsigned char *str, int radix, int low);

int mp_toradixw(mp_int_t *mp, wchar_t *str, int radix);
int mp_toradix_casew(mp_int_t *mp, wchar_t *str, int radix, int low);

int mp_char2value(char ch, int r);

#define mp_tobinary(M, S)           mp_toradix((M), (S), 2)
#define mp_tooctal(M, S)            mp_toradix((M), (S), 8)
#define mp_todecimal(M, S)          mp_toradix((M), (S), 10)
#define mp_tohex(M, S)              mp_toradix((M), (S), 16)

/* Return a string describing the meaning of error code 'ec'.  The
 * string returned is allocated in static memory, so the caller should
 * not attempt to modify or free the memory associated with this
 * string.
 */

const char *mp_strerror(int ec);

#ifdef __cplusplus
};
#endif

#endif


