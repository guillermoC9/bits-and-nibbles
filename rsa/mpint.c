/*

    mpint.c

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

        - The original code can be found at https://fossies.org/linux/txr/mpi/mpi.c

        - IMPORTANT:   This file is a mess in terms of parameters order and you 
                       need to be extra careful with the parameter calling. 
                       This is due to me having the habit of using the order like
                       where func(c,a,b) to say c = a + b, and Michael here used
                       mostly: function(a,b,c) to say a + b = c.

                       As I was adapting the file to my needs, I was quite clumsy and 
                       added to the mess instead of fixing it. :-(

                                --oO0Oo--

 */

#include "mpint.h"

/* -------------------------- */

#ifdef __cplusplus
  #define convert(TYPE, EXPR)   (static_cast<TYPE>(EXPR))
  #define coerce(TYPE, EXPR)    (reinterpret_cast<TYPE>(EXPR))
#else
  #define convert(TYPE, EXPR)   ((TYPE) (EXPR))
  #define coerce(TYPE, EXPR)    ((TYPE) (EXPR))
#endif

/* -------------------------- */

#define MP_DIGIT_SIZE   4
#define MP_WORD_SIZE    8
#define MP_SIZE_MAX     0xffffffff
#define MP_DIGIT_FMT_1  "%x"
#define MP_DIGIT_FMT    "%08x"

#define MP_DIGIT_BIT    convert(unsigned int, CHAR_BIT * MP_DIGIT_SIZE)
#define MP_DIGIT_MAX    convert(mp_digit_t, -1)
#define MP_WORD_BIT     convert(unsigned  int, CHAR_BIT * MP_WORD_SIZE)
#define MP_WORD_MAX     convert(mp_word_t, -1)

#define MP_MAX_BITS     (MP_SIZE_MAX - 1)
#define MP_MAX_DIGITS   (MP_MAX_BITS / MP_DIGIT_BIT)

#define RADIX           (convert(mp_word_t, MP_DIGIT_MAX) + 1)

/* -------------------------- */

#define shortjmp(_lbl)   goto _lbl
#define EVER             ;;
#define mpi_min(_a,_b)    (((_a) < (_b)) ? (_a) : (_b))
#define mpi_max(_a,_b)    (((_a) > (_b)) ? (_a) : (_b))
#define mpi_abs(_a)       (((_a) < 0) ? -(_a) : (_a))

/* -------------------------- *
 * A table of the logs of 2 for various bases scaled by a factor
 * and converted to integer.
 *
 * This table is used to compute output lengths for the mp_toradix
 * function.
 * -------------------------- */

#define MP_LOG_SCALE 16384

const unsigned int s_logv_2[] =
{
     0,    0, 16384, 10338,
  8192, 7057,  6339,  5837,
  5462, 5169,  4933,  4737,
  4571, 4428,  4304,  4194,
  4096, 4009,  3930,  3857,
  3791, 3731,  3675,  3622,
  3574, 3529,  3486,  3446,
  3409, 3373,  3339,  3308,
  3277, 3248,  3221,  3195,
  3170, 3146,  3122,  3100,
  3079, 3059,  3039,  3020,
  3002, 2984,  2967,  2950,
  2934, 2919,  2903,  2889,
  2875, 2861,  2847,  2834,
  2822, 2809,  2797,  2786,
  2774, 2763,  2752,  2742,
  2731
};

/* -------------------------- */
/* Default precision for newly created mp_int's */

static unsigned int s_mp_defprec = 8;

#define NEG         MP_NEG
#define ZPOS        MP_ZPOS
#define DIGIT_BIT   MP_DIGIT_BIT
#define DIGIT_MAX   MP_DIGIT_MAX

#define CARRYOUT(W) ((W)>>DIGIT_BIT)
#define ACCUM(W)    ((W)&MP_DIGIT_MAX)

#define ARGCHK(X,Y) {if(!(X)){return (Y);}}

/* Nicknames for access macros */

#define SIGN(MP)        mp_sign(MP)
#define ISNEG(MP)       mp_isneg(MP)
#define USED(MP)        mp_used(MP)
#define ALLOC(MP)       mp_alloc(MP)
#define DIGITS(MP)      mp_digits(MP)
#define DIGIT(MP,N)     mp_digit(MP,N)

/* This defines the maximum I/O base (minimum is 2) */

#define MAX_RADIX   64

/* Constant strings returned by mp_strerror() */

static const char *int_string[] =
{
    "unknown result code",      /* say what? */
    "boolean true",             /* MP_OKAY, MP_YES */
    "boolean false",            /* MP_NO */
    "out of memory",            /* MP_MEM */
    "argument out of range",    /* MP_RANGE */
    "invalid input parameter",  /* MP_BADARG */
    "result is undefined",      /* MP_UNDEF */
    "result is too large"       /* MP_TOOBIG */
};

static const char    *s_dmap_1 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";
static const wchar_t *s_dmap_w = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

#define s_mp_setz(dp, count)        memset(dp, 0, (count) * MP_DIGIT_SIZE)
#define s_mp_copy(sp, dp, count)    memcpy(dp, sp, (count) * MP_DIGIT_SIZE)
#define s_mp_alloc(nb, ni)          calloc(nb, ni)
#define s_mp_free(ptr)              {if (ptr) free(ptr);}

/* -------------------------- */

static int s_mp_grow(mp_int_t *mp, unsigned int min); /* increase allocated size */
static int s_mp_pad(mp_int_t *mp, unsigned int min); /* left pad with zeroes */

static unsigned int s_highest_bit(mp_digit_t n);
static unsigned int s_highest_bit_mp(mp_int_t *a);

static int s_mp_set_bit(mp_int_t *a, unsigned int bit, unsigned int value);

static void s_mp_clamp(mp_int_t *mp); /* clip leading zeroes */
static void s_mp_exch(mp_int_t *a, mp_int_t *b); /* swap a and b in place */

static int s_mp_lshd(mp_int_t *mp, unsigned int p); /* left-shift by p digits */
static void s_mp_rshd(mp_int_t *mp, unsigned int p); /* right-shift by p digits */
static void s_mp_div_2d(mp_int_t *mp, mp_digit_t d); /* divide by 2^d in place */
static void s_mp_mod_2d(mp_int_t *mp, mp_digit_t d); /* modulo 2^d in place */
static int  s_mp_mul_2d(mp_int_t *mp, mp_digit_t d); /* multiply by 2^d in place */
static void s_mp_div_2(mp_int_t *mp); /* divide by 2 in place */
static int  s_mp_mul_2(mp_int_t *mp); /* multiply by 2 in place */

static mp_digit_t s_mp_norm(mp_int_t *a, mp_int_t *b); /* normalize for division */

static int s_mp_add_d(mp_int_t *mp, mp_digit_t d); /* unsigned digit addition */
static int s_mp_sub_d(mp_int_t *mp, mp_digit_t d); /* unsigned digit subtract */
static int s_mp_mul_d(mp_int_t *mp, mp_digit_t d); /* unsigned digit multiply */
static int s_mp_div_d(mp_int_t *mp, mp_digit_t d, mp_digit_t *r); /* unsigned digit divide */
static int s_mp_reduce(mp_int_t *x, mp_int_t *m, mp_int_t *mu); /* Barrett reduction */
static int s_mp_add(mp_int_t *a, mp_int_t *b); /* magnitude addition */
static int s_mp_sub(mp_int_t *a, mp_int_t *b); /* magnitude subtract */
static int s_mp_mul(mp_int_t *a, mp_int_t *b); /* magnitude multiply */
static int s_mp_sqr(mp_int_t *a); /* magnitude square */

static int s_mp_div(mp_int_t *a, mp_int_t *b); /* magnitude divide */
static int s_mp_2expt(mp_int_t *a, unsigned int k); /* a = 2^k */
static int s_mp_cmp(mp_int_t *a, mp_int_t *b); /* magnitude comparison */
static int s_mp_cmp_d(mp_int_t *a, mp_digit_t d); /* magnitude digit compare */

static unsigned int s_mp_ispow2(mp_int_t *v); /* is v a power of 2? */

static int s_mp_ispow2d(mp_digit_t d); /* is d a power of 2? */
static int s_mp_tovalue(wchar_t ch, int r); /* convert ch to value */
static char s_mp_todigit(int val, int r, int low); /* convert val to digit */
static wchar_t s_mp_todigitw(int val, int r, int low); /* convert val to digit */
static size_t s_mp_outlen(unsigned int bits, int r); /* output length in bytes */

/* -------------------------- */

int mp_is_odd(mp_int_t *mp)
{
    if(mp && mp->dp)
        return (mp->dp[0] & 1);

    return FALSE;
}

/* -------------------------- */

int mp_is_one(mp_int_t *mp)
{
    ARGCHK(mp != NULL, 0);

    if (USED(mp) == 1 && DIGIT(mp, 0) == 1)
        return 1;
    return 0;
}


/* -------------------------- */

unsigned int mp_get_prec(void)
{
  return s_mp_defprec;
}

/* -------------------------- */

void mp_set_prec(unsigned int prec)
{
    s_mp_defprec = (prec) ? prec : 8;
}

/* -------------------------- */

/* Initialize a new zero-valued mp_int.  Returns MP_OKAY if successful,
 * MP_MEM if memory could not be allocated for the structure.
 */

int mp_init(mp_int_t *mp)
{
  return mp_init_size(mp,s_mp_defprec);
}

/* -------------------------- */

mp_int_t *mp_create(void)
{
    mp_int_t *mp;

    mp = (mp_int_t *)calloc(sizeof(mp_int_t),1);
    if(mp)
        mp_init(mp);
    return mp;
}


/* -------------------------- */

mp_int_t *mp_dup(mp_int_t *old)
{
    mp_int_t *mp = NULL;

    if(old)
    {
        mp = (mp_int_t *)calloc(sizeof(mp_int_t),1);
        if(mp)
            mp_init_copy(mp,old);
    }
    return mp;
}


/* -------------------------- */

void  mp_free(mp_int_t *mp)
{
    if(mp)
    {
        mp_clear(mp);
        free(mp);
    }
}

/* -------------------------- */

/* Initialize a new zero-valued mp_int_t with at least the given
 * precision; returns MP_OKAY if successful, or MP_MEM if memory could
 * not be allocated for the structure.
 */

int mp_init_size(mp_int_t *mp, unsigned int prec)
{
    ARGCHK(mp != NULL, MP_BADARG);

    if (prec > MP_MAX_DIGITS)
        return MP_TOOBIG;

    if ((DIGITS(mp) = coerce(mp_digit_t *,s_mp_alloc(prec, MP_DIGIT_SIZE))) == NULL)
        return MP_MEM;

    SIGN(mp) = MP_ZPOS;
    USED(mp) = 1;
    ALLOC(mp) = prec;

    return MP_OKAY;
}

/* -------------------------- */

int mp_init_set_digits(mp_int_t *mpi,unsigned int count,mp_digit_t *lst)
{
    int ret = MP_BADARG;

    if(mpi && lst)
    {
        if(count)
        {
            ret = mp_init_size(mpi,count);
            if(ret == MP_OKAY)
            {
                unsigned int t;
                USED(mpi) = count;
                for(t = 0; count > 0; t++)
                    DIGIT(mpi, --count) = lst[t];
            }
        }
        else
        {
            ret = mp_init_size(mpi,s_mp_defprec);
        }
    }
    return ret;
}


/* -------------------------- */

mp_digit_t *mp_get_digits(mp_int_t *mpi,unsigned int *count)
{
    mp_digit_t *ret = NULL;

    if(mpi && count)
    {
        unsigned int used;

        s_mp_clamp(mpi);

        used = USED(mpi);
        if(used == 0)
        {
            ret = (mp_digit_t *)calloc(MP_DIGIT_SIZE,2);
           *count = 1;
        }
        else
        {
            size_t total;

            total = (size_t) used;
            total*= MP_DIGIT_SIZE;

            ret = (mp_digit_t *) malloc(total);
            if(ret)
            {
                unsigned int t;

               *count = used;
                for(t = 0; used > 0; t++)
                    ret[t]= DIGIT(mpi, --used);
            }
        }
    }
    return ret;
}

/* -------------------------- */

int mp_export_digits(mp_int_t *mpi,mp_digit_t *lst,size_t count,int le)
{
    int ret = MP_BADARG;

    if(mpi && lst)
    {
        ret = MP_OKAY;

        if(count > 0)
        {
            size_t pos,total;
            unsigned int dig;

            pos   = 0;
            total = (size_t) USED(mpi);

            if(total > count)
                total = count;

            if(le)
            {
                dig = 0;

                while(pos < total)
                    lst[pos++] = DIGIT(mpi, dig++);

                while(total < count)
                    lst[total++]=0;
            }
            else
            {
                dig = USED(mpi);

                while(total  < count)
                {
                    lst[pos++]=0;
                    count--;
                }

                while(pos < total)
                    lst[pos++] = DIGIT(mpi, --dig);
            }
        }
    }
    return ret;
}

/* -------------------------- */

int mp_import_digits(mp_int_t *mpi,mp_digit_t *lst,size_t count,int le)
{
    int ret = MP_BADARG;

    if(mpi && lst)
    {
        ret = MP_OKAY;

        if(count < 1)
        {
            mp_set_zero(mpi);
        }
        else
        {
            size_t pos = 0;
            unsigned int dig = (unsigned int) count;

            ret = s_mp_grow(mpi, dig);
            if(ret == MP_OKAY)
            {
                USED(mpi) = dig;

                if(le)
                {
                    dig = 0;
                    while(pos < count)
                        DIGIT(mpi, dig++) = lst[pos++];
                }
                else
                {
                    while(pos < count)
                        DIGIT(mpi, --dig) = lst[pos++];
                }
                s_mp_clamp(mpi);
            }
        }
    }
    return ret;
}



/* -------------------------- */

int mp_init_set_bytes(mp_int_t *mp,unsigned int len,const void *lst)
{
    int ret = MP_BADARG;

    if(mp && lst && len)
    {
        unsigned int digits;

        digits = (unsigned int) (len / MP_DIGIT_SIZE);
        if(len % MP_DIGIT_SIZE)
            digits++;

        ret = mp_init_size(mp,digits);
        if(ret == MP_OKAY)
            ret = mp_set_bytes(mp,lst,len);
    }
    return ret;
}

/* -------------------------- */

int mp_set_bytes(mp_int_t *mp,const void *lst,unsigned int len)
{
    int ret = MP_BADARG;

    if(mp && lst && len)
    {
        unsigned int digits,remain,t;
        mp_digit_t value;
        const unsigned char *origin;

        origin = (const unsigned char *)lst;
        digits = (unsigned int) (len / MP_DIGIT_SIZE);
        remain = (unsigned int) (len % MP_DIGIT_SIZE);

        if(remain)
            digits++;

        ret = s_mp_grow(mp,digits);
        if(ret == MP_OKAY)
        {
            USED(mp) = digits;

            if(remain)
            {
                value = 0;
                while(remain > 0)
                {
                    value <<= 8;
                    value |= *origin++;
                    remain--;
                }
                DIGIT(mp,--digits) = value;
            }

            while(digits > 0)
            {
                value = 0;
                for(t = 0; t < MP_DIGIT_SIZE; t++)
                {
                    value <<= 8;
                    value |= *origin++;
                }
                DIGIT(mp, --digits) = value;
            }

            s_mp_clamp(mp);
        }
    }
    return ret;
}

/* -------------------------- */

unsigned int s_mp_byte_count(mp_int_t *mpi)
{
    unsigned int used, num = 0;

    used = USED(mpi);
    if(used)
    {
        mp_digit_t val;
        unsigned int shift,t;

        num= used * MP_DIGIT_SIZE;

        val = DIGIT(mpi, --used);
        shift = MP_DIGIT_BIT - 8;

        for(t=0; t < MP_DIGIT_SIZE ; t++)
        {
            if( (val >> shift) & 0xff)
                return num;
            num--;
            shift -= 8;
        }
    }
    return num;
}

/* -------------------------- */

unsigned char *mp_get_bytes(mp_int_t *mpi,unsigned int *count)
{
    if(mpi && count)
    {
        unsigned int used;

        s_mp_clamp(mpi);

        used = USED(mpi);

        if(used)
        {
            unsigned int num = s_mp_byte_count(mpi);
            unsigned char *ret;

            ret = (unsigned char *) malloc(num);
            if(ret)
            {
                if(mp_copy_bytes(mpi,ret,num) == MP_OKAY)
                {
                   *count = num;
                    return ret;
                }
                free(ret);
            }
        }
    }
    return NULL;
}

/* -------------------------- */

wchar_t *mp_get_wide_string(mp_int_t *mpi)
{
    wchar_t *ret = NULL;

    if(mpi)
    {
        unsigned int len;

        s_mp_clamp(mpi);

        len = mp_radix_size(mpi, 16);
        if(len < 1)
        {
            ret = (wchar_t *)calloc(sizeof(wchar_t),2);
            if(ret)
            {
                ret[0]=L'0';
                ret[1]=0;
            }
        }
        else
        {
            ret = (wchar_t *)calloc(sizeof(wchar_t),len + 1);
            if(ret)
                mp_toradix_casew(mpi,ret,16,0);
        }
    }
    return ret;
}

/* -------------------------- */

wchar_t *mp_get_wide_string_decimal(mp_int_t *mpi)
{
    wchar_t *ret = NULL;

    if(mpi)
    {
        unsigned int len;

        s_mp_clamp(mpi);

        len = mp_radix_size(mpi, 10);
        if(len < 1)
        {
            ret = (wchar_t *)calloc(sizeof(wchar_t),2);
            if(ret)
            {
                ret[0]=L'0';
                ret[1]=0;
            }
        }
        else
        {
            ret = (wchar_t *)calloc(sizeof(wchar_t),len + 1);
            if(ret)
                mp_toradixw(mpi,ret,10);
        }
    }
    return ret;
}


/* -------------------------- */

int mp_init_set_string(mp_int_t *mp,const char *str,int radix)
{
    int ret = MP_BADARG;

    if(mp && str)
    {
        ret = mp_init(mp);
        if(ret == MP_OKAY)
        {
            ret = mp_read_radix_char(mp,str,radix);
            if(ret != MP_OKAY)
                mp_clear(mp);
        }
    }
    return ret;
}

/* -------------------------- */

int mp_init_set_wide_string(mp_int_t *mp,const wchar_t *str,int radix)
{
    int ret = MP_BADARG;

    if(mp && str)
    {
        ret = mp_init(mp);
        if(ret == MP_OKAY)
        {
            ret = mp_read_radix(mp,str,radix);
            if(ret != MP_OKAY)
                mp_clear(mp);
        }
    }
    return ret;
}


/* -------------------------- */

int mp_set_string(mp_int_t *mp,const char *str,int radix)
{
    int ret = MP_BADARG;

    if(mp && str)
        ret = mp_read_radix_char(mp,str,radix);
    return ret;
}

/* -------------------------- */

int mp_set_wide_string(mp_int_t *mp,const wchar_t *str,int radix)
{
    int ret = MP_BADARG;

    if(mp && str)
    {
        ret = mp_read_radix(mp,str,radix);
    }
    return ret;
}


/* -------------------------- */

/* Initialize mp as an exact copy of from.  Returns MP_OKAY if
 * successful, MP_MEM if memory could not be allocated for the new
 * structure.
 */

int mp_init_copy(mp_int_t *mp, mp_int_t *from)
{
    ARGCHK(mp != NULL && from != NULL, MP_BADARG);

    if (mp == from)
        return MP_OKAY;

    if ((DIGITS(mp) = coerce(mp_digit_t *,s_mp_alloc(USED(from), MP_DIGIT_SIZE))) == NULL)
        return MP_MEM;

    s_mp_copy(DIGITS(from), DIGITS(mp), USED(from));

    USED(mp) = USED(from);
    ALLOC(mp) = USED(from);
    SIGN(mp) = SIGN(from);

    return MP_OKAY;
}

/* -------------------------- */

/* Copies the mp_int_t 'from' to the mp_int_t 'to'.  It is presumed that
 * 'to' has already been initialized (if not, use mp_init_copy()
 * instead). If 'from' and 'to' are identical, nothing happens.
 */

int mp_copy(const mp_int_t *from, mp_int_t *to)
{
    mp_digit_t *tmp;

    ARGCHK(from != NULL && to != NULL, MP_BADARG);

    if ((void *)from ==(void *) to)
        return MP_OKAY;

    /* If the allocated buffer in 'to' already has enough space to hold
     * all the used digits of 'from', we'll re-use it to avoid hitting
     * the memory allocater more than necessary; otherwise, we'd have
     * to grow anyway, so we just allocate a hunk and make the copy as
     * usual
     */

    if (ALLOC(to) >= USED(from))
    {
        s_mp_setz(DIGITS(to) + USED(from), ALLOC(to) - USED(from));
        s_mp_copy(DIGITS(from), DIGITS(to), USED(from));
    }
    else
    {
        if ((tmp = coerce(mp_digit_t *,s_mp_alloc(USED(from), MP_DIGIT_SIZE))) == NULL)
            return MP_MEM;

        s_mp_copy(DIGITS(from), tmp, USED(from));

        if (DIGITS(to) != NULL)
        {
            s_mp_setz(DIGITS(to), ALLOC(to));
            s_mp_free(DIGITS(to));
        }

        DIGITS(to) = tmp;
        ALLOC(to) = USED(from);
    }

    /* Copy the precision and sign from the original */

    USED(to) = USED(from);
    SIGN(to) = SIGN(from);

    return MP_OKAY;
}

/* -------------------------- */

int mp_copy_bytes(mp_int_t *mpi,void *dest,unsigned int num)
{
    if(mpi && dest)
    {
        unsigned int used;

        s_mp_clamp(mpi);

        used = USED(mpi);

        if(used)
        {
            mp_digit_t val;
            unsigned int t,i;
            unsigned char *ret = (unsigned char *)dest;

            if(!num)
                return MP_RANGE;

            for(i=0; i < used; i++)
            {
                val= DIGIT(mpi,i);
                for(t=0; t < MP_DIGIT_SIZE && num; t++)
                {
                    ret[--num]=(unsigned char)(val & 0xff);
                    val >>= 8;
                }
            }
        }
        return (num == 0) ? MP_OKAY : MP_RANGE;
    }
    return MP_BADARG;
}

/* ----------------------- */

int mp_copy_exact_bytes(mp_int_t *mp, void *data, unsigned int num)
{
    unsigned char *ptr = (unsigned char *)data;
    unsigned int len = 0, dif;

    if(mp && data && num)
    {
        s_mp_clamp(mp);

        len = s_mp_byte_count(mp);

        if (len > num)
        {
            len = num;
        }
        else
        {
           dif = (num - len);

            while (dif > 0)
            {
               *ptr++ = 0;
                dif--;
            }
        }
        return mp_copy_bytes(mp,ptr,len);
    }
    return MP_BADARG;
}

/* -------------------------- */

int mp_copy_string(mp_int_t *mpi,char *dest,int num)
{
    int len = MP_BADARG;
    unsigned char *str;

    if (mpi && num > 0)
    {
        len =  mp_radix_size(mpi, 16);
        if(len < 1)
        {
            dest[0]='0';
            dest[1]=0;
            len = 1;
        }
        else if (num >= len)
        {
            len = num;
            mp_toradix_case(mpi,(unsigned char *)dest,16,0);
        }
        else
        {
            str = malloc(len);
            if(str)
            {
                len = num;
                mp_toradix_case(mpi,str,16,0);
                memcpy(dest,str,len);
                free(str);
            }
            else
            {
                len = MP_MEM;
            }
        }
    }
    return len;
}




/* -------------------------- */

/* Exchange mp1 and mp2 without allocating any intermediate memory
 * (well, unless you count the stack space needed for this call and the
 * locals it creates...).  This cannot fail.
 */

void mp_exch(mp_int_t *mp1, mp_int_t *mp2)
{
    if (mp1 != NULL && mp2 != NULL)
        s_mp_exch(mp1, mp2);
}

/* -------------------------- */

/* Release the storage used by an mp_int, and void its fields so that
 * if someone calls mp_clear() again for the same int later, we won't
 * get tollchocked.
 */

void mp_clear(mp_int_t *mp)
{
    if (mp == NULL)
        return;

    if (DIGITS(mp) != NULL)
    {
        s_mp_setz(DIGITS(mp), ALLOC(mp));
        s_mp_free(DIGITS(mp));
        DIGITS(mp) = NULL;
    }

    USED(mp) = 0;
    ALLOC(mp) = 0;
}

/* -------------------------- */

/* Set mp to zero.  Does not change the allocated size of the structure,
 * and therefore cannot fail (except on a bad argument, which we ignore)
 */

void mp_set_zero(mp_int_t *mp)
{
    if (mp == NULL)
        return;

    s_mp_setz(DIGITS(mp), ALLOC(mp));
    USED(mp) = 1;
    SIGN(mp) = MP_ZPOS;
}

/* -------------------------- */

void mp_set_d(mp_int_t *mp, mp_digit_t d)
{
    if (mp == NULL)
        return;

    mp_set_zero(mp);
    DIGIT(mp, 0) = d;
}

/* -------------------------- */

int mp_init_set_d(mp_int_t *mp,mp_digit_t d)
{
    if (mp == NULL)
        return MP_MEM;

    mp_init_size(mp,s_mp_defprec);
    DIGIT(mp, 0) = d;
    return MP_OKAY;
}

/* -------------------------- */

int mp_get_int(mp_int_t *mp)
{
    if (mp == NULL)
        return MP_MEM;

    if(USED(mp) > 0)
        return (int)(DIGIT(mp,0) & 0x7fffffff);
    return 0;
}

/* -------------------------- */

int mp_set_int(mp_int_t *mp, long z)
{
    unsigned int ix;
    unsigned long w = z;
    unsigned long v = z >= 0 ? w : -w;
    int res;

    ARGCHK(mp != NULL, MP_BADARG);

    mp_set_zero(mp);
    if (z == 0)
        return MP_OKAY; /* shortcut for zero */

    for (ix = sizeof (long) - 1; ix < MP_SIZE_MAX; ix--)
    {
        if ((res = s_mp_mul_2d(mp, CHAR_BIT)) != MP_OKAY)
          return res;
        res = s_mp_add_d(mp,convert(mp_digit_t, ((v >> (ix * CHAR_BIT)) & UCHAR_MAX)));
        if (res != MP_OKAY)
          return res;
    }

    if (z < 0)
        SIGN(mp) = MP_NEG;

    return MP_OKAY;
}

/* -------------------------- */

int mp_set_word(mp_int_t *mp, mp_word_t w, int sign)
{
    USED(mp) = 2;
    DIGIT(mp, 0) = w & MP_DIGIT_MAX;
    DIGIT(mp, 1) = w >> MP_DIGIT_BIT;
    SIGN(mp) = sign;
    return MP_OKAY;
}

/* -------------------------- */

/* Compute the sum b = a + d, for a single digit d.  Respects the sign of
 * its primary addend (single digits are unsigned anyway).
 */

int mp_add_d(mp_int_t *a, mp_digit_t d, mp_int_t *b)
{
    int res = MP_OKAY;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if ((res = mp_copy(a, b)) != MP_OKAY)
        return res;

    if (SIGN(b) == MP_ZPOS)
    {
        res = s_mp_add_d(b, d);
    }
    else if (s_mp_cmp_d(b, d) >= 0)
    {
        res = s_mp_sub_d(b, d);
    }
    else
    {
        SIGN(b) = MP_ZPOS;
        DIGIT(b, 0) = d - DIGIT(b, 0);
    }
    return res;
}

/* -------------------------- */

/* Compute the difference b = a - d, for a single digit d.  Respects the
 * sign of its subtrahend (single digits are unsigned anyway).
 */

int mp_sub_d(mp_int_t *a, mp_digit_t d, mp_int_t *b)
{
    int res;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if ((res = mp_copy(a, b)) != MP_OKAY)
        return res;

    if (SIGN(b) == MP_NEG)
    {
        if ((res = s_mp_add_d(b, d)) != MP_OKAY)
            return res;
    }
    else if (s_mp_cmp_d(b, d) >= 0)
    {
        if ((res = s_mp_sub_d(b, d)) != MP_OKAY)
            return res;
    } else
    {
        mp_neg(b, b);

        DIGIT(b, 0) = d - DIGIT(b, 0);
        SIGN(b) = MP_NEG;
    }
    if (s_mp_cmp_d(b, 0) == 0)
        SIGN(b) = MP_ZPOS;
    return MP_OKAY;
}

/* -------------------------- */

/* Compute the product b = a * d, for a single digit d.  Respects the sign
 * of its multiplicand (single digits are unsigned anyway)
 */

int mp_mul_d(mp_int_t *a, mp_digit_t d, mp_int_t *b)
{
    int res;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (d == 0)
    {
        mp_set_zero(b);
        return MP_OKAY;
    }

    if ((res = mp_copy(a, b)) != MP_OKAY)
        return res;

    res = s_mp_mul_d(b, d);

    return res;
}

/* -------------------------- */

int mp_mul_2(mp_int_t *a, mp_int_t *c)
{
    int res;

    ARGCHK(a != NULL && c != NULL, MP_BADARG);

    if ((res = mp_copy(a, c)) != MP_OKAY)
        return res;

    return s_mp_mul_2(c);
}

/* Compute the quotient q = a / d and remainder r = a mod d, for a
 * single digit d.  Respects the sign of its divisor (single digits are
 * unsigned anyway).
 */

int mp_div_d(mp_int_t *a, mp_digit_t d, mp_int_t *q, mp_digit_t *r)
{
    int res;
    mp_digit_t rem;
    int pow;

    ARGCHK(a != NULL, MP_BADARG);

    if (d == 0)
        return MP_RANGE;

    /* Shortcut for powers of two ... */
    if ((pow = s_mp_ispow2d(d)) >= 0)
    {
        mp_digit_t mask;

        mask = (convert(mp_digit_t, 1) << pow) - 1;
        rem = DIGIT(a, 0) & mask;

        if (q)
        {
            mp_copy(a, q);
            s_mp_div_2d(q, pow);
        }

        if (r)
           *r = rem;
        return MP_OKAY;
    }

  /* If the quotient is actually going to be returned, we'll try to
   * avoid hitting the memory allocator by copying the dividend into it
   * and doing the division there.  This can't be any _worse_ than
   * always copying, and will sometimes be better (since it won't make
   * another copy)
   * If it's not going to be returned, we need to allocate a temporary
   * to hold the quotient, which will just be discarded.
   */
    if (q)
    {
        if ((res = mp_copy(a, q)) != MP_OKAY)
            return res;

        res = s_mp_div_d(q, d, &rem);
        if (s_mp_cmp_d(q, 0) == MP_EQ)
        SIGN(q) = MP_ZPOS;

    }
    else
    {
        mp_int_t qp;

        if ((res = mp_init_copy(&qp, a)) != MP_OKAY)
            return res;

        res = s_mp_div_d(&qp, d, &rem);
        if (s_mp_cmp_d(&qp, 0) == 0)
            SIGN(&qp) = MP_ZPOS;
        mp_clear(&qp);
    }

    if (r)
        *r = rem;

    return res;
}

/* -------------------------- */

int mp_div_by_2(mp_int_t *a)
{
    ARGCHK(a != NULL, MP_BADARG);

    s_mp_div_2(a);

    return MP_OKAY;
}


/* -------------------------- */

/* Compute c = a / 2, disregarding the remainder.  */

int mp_div_2(mp_int_t *a, mp_int_t *c)
{
    int res;

    ARGCHK(a != NULL && c != NULL, MP_BADARG);

    if ((res = mp_copy(a, c)) != MP_OKAY)
        return res;

    s_mp_div_2(c);

    return MP_OKAY;
}

/* -------------------------- */

int mp_expt_d(mp_int_t *a, mp_digit_t d, mp_int_t *c)
{
    mp_int_t s, x;
    int res;
    int cs = MP_ZPOS;

    ARGCHK(a != NULL && c != NULL, MP_BADARG);

    if ((res = mp_init(&s)) != MP_OKAY)
        return res;
    if ((res = mp_init_copy(&x, a)) != MP_OKAY)
        shortjmp(X);

    DIGIT(&s, 0) = 1;

    if ((d % 2) == 1)
        cs = SIGN(a);

    while (d != 0)
    {
        if (d & 1)
        {
            if ((res = s_mp_mul(&s, &x)) != MP_OKAY)
            shortjmp(CLEANUP);
        }

        d >>= 1;

        if ((res = s_mp_sqr(&x)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    SIGN(&s) = cs;

    s_mp_exch(&s, c);

CLEANUP:
    mp_clear(&x);
X:
    mp_clear(&s);

    return res;
}

/* -------------------------- */

/* Compute b = |a|.  'a' and 'b' may be identical.  */

int mp_abs(mp_int_t *a, mp_int_t *b)
{
    int res;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if ((res = mp_copy(a, b)) != MP_OKAY)
        return res;

    SIGN(b) = MP_ZPOS;

    return MP_OKAY;
}

/* -------------------------- */

/* Compute b = -a.  'a' and 'b' may be identical.  */

int mp_neg(mp_int_t *a, mp_int_t *b)
{
    int res;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if ((res = mp_copy(a, b)) != MP_OKAY)
        return res;

    if (s_mp_cmp_d(b, 0) == MP_EQ)
        SIGN(b) = MP_ZPOS;
    else
        SIGN(b) = (SIGN(b) == MP_NEG) ? MP_ZPOS : MP_NEG;

    return MP_OKAY;
}

/* -------------------------- */

/* Compute c = a + b.  All parameters may be identical.  */

int mp_add(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res;
    int cmp;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (SIGN(a) == SIGN(b)) /* same sign:  add values, keep sign */
    {

        /* Commutativity of addition lets us do this in either order,
         * so we avoid having to use a temporary even if the result
         * is supposed to replace the output
         */
        if (c == b)
        {
            if ((res = s_mp_add(c, a)) != MP_OKAY)
                return res;
        }
        else
        {
            if (c != a && (res = mp_copy(a, c)) != MP_OKAY)
                return res;

            if ((res = s_mp_add(c, b)) != MP_OKAY)
                return res;
        }
    }
    else if ((cmp = s_mp_cmp(a, b)) > 0)   /* different sign: a > b */
    {
        /* If the output is going to be clobbered, we will use a temporary
         * variable; otherwise, we'll do it without touching the memory
         * allocator at all, if possible
         */
        if (c == b)
        {
            mp_int_t tmp;

            if ((res = mp_init_copy(&tmp, a)) != MP_OKAY)
                return res;
            if ((res = s_mp_sub(&tmp, b)) != MP_OKAY)
            {
                mp_clear(&tmp);
                return res;
            }
            s_mp_exch(&tmp, c);
            mp_clear(&tmp);
        }
        else
        {
            if (c != a && (res = mp_copy(a, c)) != MP_OKAY)
                return res;
            if ((res = s_mp_sub(c, b)) != MP_OKAY)
                return res;
        }
    }
    else if (cmp == 0)              /* different sign, a == b */
    {
        mp_set_zero(c);
        return MP_OKAY;
    }
    else                           /* different sign: a < b */
    {
        /* See above... */
        if (c == a)
        {
            mp_int_t tmp;

            if ((res = mp_init_copy(&tmp, b)) != MP_OKAY)
                return res;
            if ((res = s_mp_sub(&tmp, a)) != MP_OKAY)
            {
                mp_clear(&tmp);
                return res;
            }
            s_mp_exch(&tmp, c);
            mp_clear(&tmp);

        }
        else
        {
            if (c != b && (res = mp_copy(b, c)) != MP_OKAY)
                return res;
            if ((res = s_mp_sub(c, a)) != MP_OKAY)
                return res;
        }
    }

    if (USED(c) == 1 && DIGIT(c, 0) == 0)
        SIGN(c) = MP_ZPOS;

    return MP_OKAY;
}

/* -------------------------- */

/* Compute c = a - b.  All parameters may be identical.  */

int mp_sub(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res;
    int cmp;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (SIGN(a) != SIGN(b))
    {
        if (c == a)
        {
            if ((res = s_mp_add(c, b)) != MP_OKAY)
                return res;
        }
        else
        {
            if (c != b && ((res = mp_copy(b, c)) != MP_OKAY))
                return res;
            if ((res = s_mp_add(c, a)) != MP_OKAY)
                return res;
            SIGN(c) = SIGN(a);
        }
    }
    else if ((cmp = s_mp_cmp(a, b)) > 0) /* Same sign, a > b */
    {
        if (c == b)
        {
            mp_int_t tmp;

            if ((res = mp_init_copy(&tmp, a)) != MP_OKAY)
                return res;
            if ((res = s_mp_sub(&tmp, b)) != MP_OKAY)
            {
                mp_clear(&tmp);
                return res;
            }
            s_mp_exch(&tmp, c);
            mp_clear(&tmp);
        }
        else
        {
            if (c != a && ((res = mp_copy(a, c)) != MP_OKAY))
                return res;
            if ((res = s_mp_sub(c, b)) != MP_OKAY)
                return res;
        }
    }
    else if (cmp == 0)   /* Same sign, equal magnitude */
    {
        mp_set_zero(c);
        return MP_OKAY;
    }
    else               /* Same sign, b > a */
    {
        if (c == a)
        {
            mp_int_t tmp;

            if ((res = mp_init_copy(&tmp, b)) != MP_OKAY)
                return res;

            if ((res = s_mp_sub(&tmp, a)) != MP_OKAY)
            {
                mp_clear(&tmp);
                return res;
            }
            s_mp_exch(&tmp, c);
            mp_clear(&tmp);
        }
        else
        {
            if (c != b && ((res = mp_copy(b, c)) != MP_OKAY))
                return res;
            if ((res = s_mp_sub(c, a)) != MP_OKAY)
                return res;
        }

        SIGN(c) = !SIGN(b);
    }
    if (USED(c) == 1 && DIGIT(c, 0) == 0)
        SIGN(c) = MP_ZPOS;

    return MP_OKAY;
}

/* -------------------------- */

/* Compute c = a * b.  All parameters may be identical.  */

int mp_mul(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res;
    int sgn;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    sgn = (SIGN(a) == SIGN(b)) ? MP_ZPOS : MP_NEG;

    if (c == b)
    {
        if ((res = s_mp_mul(c, a)) != MP_OKAY)
            return res;
    }
    else
    {
        if ((res = mp_copy(a, c)) != MP_OKAY)
            return res;

        if ((res = s_mp_mul(c, b)) != MP_OKAY)
            return res;
    }

    if (sgn == MP_ZPOS || s_mp_cmp_d(c, 0) == MP_EQ)
        SIGN(c) = MP_ZPOS;
    else
        SIGN(c) = sgn;

    return MP_OKAY;
}


/* -------------------------- */

/* Compute d = a * b + c.  All parameters may be identical.  */

int mp_mul_add(mp_int_t *a, mp_int_t *b, mp_int_t *c, mp_int_t *d)
{
    mp_int_t tmp;
    int ret;

    mp_init_set_d(&tmp,0);
    ret = mp_mul(a,b,&tmp);
    if(ret == MP_OKAY)
        ret = mp_add(&tmp,c,d);
    mp_clear(&tmp);
    return ret;
}



/* -------------------------- */

/* Compute c = a * 2^d.  a may be the same as c.  */

int mp_mul_2d(mp_int_t *a, mp_digit_t d, mp_int_t *c)
{
    int res;

    ARGCHK(a != NULL && c != NULL, MP_BADARG);

    if ((res = mp_copy(a, c)) != MP_OKAY)
        return res;

    if (d == 0)
        return MP_OKAY;

    return s_mp_mul_2d(c, d);
}

/* -------------------------- */

/* Compute b = a * a.  a may be the same as b.  */

int mp_sqr(mp_int_t *a, mp_int_t *b)
{
    int res;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if ((res = mp_copy(a, b)) != MP_OKAY)
        return res;

    if ((res = s_mp_sqr(b)) != MP_OKAY)
        return res;

    SIGN(b) = MP_ZPOS;

    return MP_OKAY;
}

/* -------------------------- */

/* Compute q = a / b and r = a mod b.  Input parameters may be re-used
 * as output parameters.  If q or r is NULL, that portion of the
 * computation will be discarded (although it will still be computed)
 * Pay no attention to the hacker behind the curtain.
 */

int mp_div(mp_int_t *a, mp_int_t *b, mp_int_t *q, mp_int_t *r)
{
    int res;
    mp_int_t qtmp, rtmp;
    int cmp;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (mp_cmp_z(b) == MP_EQ)
        return MP_RANGE;

    /* If a <= b, we can compute the solution without division, and
     * avoid any memory allocation
    */

    if ((cmp = s_mp_cmp(a, b)) < 0)
    {
        if (r)
        {
            if ((res = mp_copy(a, r)) != MP_OKAY)
                return res;
        }
        if (q)
            mp_set_zero(q);

        return MP_OKAY;
    }
    else if (cmp == 0)
    {

        /* Set quotient to 1, with appropriate sign */
        if (q)
        {
            int qneg = (SIGN(a) != SIGN(b));

            mp_set_d(q, 1);
            if (qneg)
                SIGN(q) = MP_NEG;
        }

        if (r)
            mp_set_zero(r);

        return MP_OKAY;
    }

    /* If we get here, it means we actually have to do some division */

    /* Set up some temporaries... */

    if ((res = mp_init_copy(&qtmp, a)) != MP_OKAY)
        return res;

    if ((res = mp_init_copy(&rtmp, b)) != MP_OKAY)
        shortjmp(CLEANUP);

    if ((res = s_mp_div(&qtmp, &rtmp)) != MP_OKAY)
        shortjmp(CLEANUP);

    /* Compute the signs for the output */

    SIGN(&rtmp) = SIGN(a); /* Sr = Sa */

    if (SIGN(a) == SIGN(b))
        SIGN(&qtmp) = MP_ZPOS; /* Sq = MP_ZPOS if Sa = Sb */
    else
        SIGN(&qtmp) = MP_NEG; /* Sq = MP_NEG if Sa != Sb */

    if (s_mp_cmp_d(&qtmp, 0) == MP_EQ)
        SIGN(&qtmp) = MP_ZPOS;

    if (s_mp_cmp_d(&rtmp, 0) == MP_EQ)
        SIGN(&rtmp) = MP_ZPOS;

    /* Copy output, if it is needed */

    if (q)
        s_mp_exch(&qtmp, q);

    if (r)
        s_mp_exch(&rtmp, r);

CLEANUP:
    mp_clear(&rtmp);
    mp_clear(&qtmp);

    return res;
}

/* -------------------------- */

int mp_div_2d(mp_int_t *a, mp_digit_t d, mp_int_t *q, mp_int_t *r)
{
    int res;

    ARGCHK(a != NULL, MP_BADARG);

    if (q)
    {
        if ((res = mp_copy(a, q)) != MP_OKAY)
            return res;
        s_mp_div_2d(q, d);
    }

    if (r)
    {
        if ((res = mp_copy(a, r)) != MP_OKAY)
            return res;
        s_mp_mod_2d(r, d);
    }
    return MP_OKAY;
}

/* -------------------------- */

/* Compute c = a ** b, that is, raise a to the b power.  Uses a
 * standard iterative square-and-multiply technique.
 */

int mp_expt(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    mp_int_t s, x;
    int res;
    mp_digit_t d;
    unsigned int dig, bit;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (mp_cmp_z(b) < 0)
        return MP_RANGE;

    if ((res = mp_init(&s)) != MP_OKAY)
        return res;

    mp_set_d(&s, 1);

    if ((res = mp_init_copy(&x, a)) != MP_OKAY)
        shortjmp(X);

    /* Loop over low-order digits in ascending order */
    for (dig = 0; dig < (USED(b) - 1); dig++)
    {
        d = DIGIT(b, dig);

        /* Loop over bits of each non-maximal digit */
        for (bit = 0; bit < DIGIT_BIT; bit++)
        {
            if (d & 1)
            {
                if ((res = s_mp_mul(&s, &x)) != MP_OKAY)
                    shortjmp(CLEANUP);
            }
            d >>= 1;
            if ((res = s_mp_sqr(&x)) != MP_OKAY)
                shortjmp(CLEANUP);
        }
    }

    /* Consider now the last digit... */

    d = DIGIT(b, dig);

    while (d)
    {
        if (d & 1)
        {
            if ((res = s_mp_mul(&s, &x)) != MP_OKAY)
                shortjmp(CLEANUP);
        }

        d >>= 1;

        if ((res = s_mp_sqr(&x)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    if (mp_iseven(b))
        SIGN(&s) = SIGN(a);

    res = mp_copy(&s, c);

CLEANUP:
    mp_clear(&x);
X:
    mp_clear(&s);

    return res;
}

/* -------------------------- */
/* Compute a = 2^k */


int mp_2expt(mp_int_t *a, mp_digit_t k)
{
  ARGCHK(a != NULL, MP_BADARG);

  return s_mp_2expt(a, k);
}


/* -------------------------- */
/* Compute b = square root of a */


int mp_sqrt(mp_int_t *a, mp_int_t *b)
{
    unsigned int mask_shift;
    mp_int_t root, guess, guess_sqr;
    mp_int_t *temp,*proot,*pguess;
    int cmp,err;

    proot = &root;
    pguess = &guess;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (mp_cmp_z(a) == MP_LT)
        return MP_RANGE;

    if ((err = mp_init(proot)) != MP_OKAY)
        shortjmp(out);

    if ((err = mp_init(pguess)) != MP_OKAY)
        shortjmp(cleanup_root);

    if ((err = mp_init(&guess_sqr)) != MP_OKAY)
        shortjmp(cleanup_guess);

    for (mask_shift = s_highest_bit_mp(a) / 2; mask_shift < MP_SIZE_MAX; mask_shift--)
    {
        if ((err = mp_copy(proot, pguess)) != MP_OKAY)
            shortjmp(cleanup);

        if ((err = s_mp_set_bit(pguess, mask_shift,1)) != MP_OKAY)
            shortjmp(cleanup);

        if ((err = mp_copy(pguess, &guess_sqr)) != MP_OKAY)
            shortjmp(cleanup);

        if ((err = s_mp_sqr(&guess_sqr)) != MP_OKAY)
            shortjmp(cleanup);

        cmp = s_mp_cmp(&guess_sqr, a);

        if (cmp < 0)
        {
            temp = proot;
            proot = pguess;
            pguess = temp;
        }
        else if (cmp == 0)
        {
            proot = pguess;
            break;
        }
    }

    if(err == MP_OKAY)
        err = mp_copy(proot, b);

cleanup:
    mp_clear(&guess_sqr);

cleanup_guess:
    mp_clear(pguess);

cleanup_root:
    mp_clear(proot);

out:
    return err;
}

/* -------------------------- */
/* Compute c = a (mod m).  Result will always be 0 <= c < m.  */

int mp_mod(mp_int_t *a, mp_int_t *m, mp_int_t *c)
{
    int res;
    int mag;

    ARGCHK(a != NULL && m != NULL && c != NULL, MP_BADARG);

    if (SIGN(m) == MP_NEG)
        return MP_RANGE;

  /* If |a| > m, we need to divide to get the remainder and take the
   * absolute value.
   * If |a| < m, we don't need to do any division, just copy and adjust
   * the sign (if a is negative).
   * If |a| == m, we can simply set the result to zero.
   * This order is intended to minimize the average path length of the
   * comparison chain on common workloads -- the most frequent cases are
   * that |a| != m, so we do those first.
   */

    if ((mag = s_mp_cmp(a, m)) > 0)
    {
        if ((res = mp_div(a, m, NULL, c)) != MP_OKAY)
            return res;

        if (SIGN(c) == MP_NEG)
        {
            if ((res = mp_add(c, m, c)) != MP_OKAY)
            return res;
        }
    }
    else if (mag < 0)
    {
        if ((res = mp_copy(a, c)) != MP_OKAY)
            return res;

        if (mp_cmp_z(a) < 0)
        {
            if ((res = mp_add(c, m, c)) != MP_OKAY)
                return res;
        }
    }
    else
    {
        mp_set_zero(c);
    }
    return MP_OKAY;
}

/* -------------------------- */

/* Compute c = a (mod d).  Result will always be 0 <= c < d */

int mp_mod_d(mp_int_t *a, mp_digit_t d, mp_digit_t *c)
{
  int res;
  mp_digit_t rem;

  ARGCHK(a != NULL && c != NULL, MP_BADARG);

  if (s_mp_cmp_d(a, d) > 0) {
    if ((res = mp_div_d(a, d, NULL, &rem)) != MP_OKAY)
      return res;

  } else {
    if (SIGN(a) == MP_NEG)
      rem = d - DIGIT(a, 0);
    else
      rem = DIGIT(a, 0);
  }

  if (c)
    *c = rem;

  return MP_OKAY;
}

/* -------------------------- */
/* Compute c = (a + b) mod m */

int mp_addmod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c)
{
    int res;

    ARGCHK(a != NULL && b != NULL && m != NULL && c != NULL, MP_BADARG);

    if ((res = mp_add(a, b, c)) != MP_OKAY)
        return res;

    if ((res = mp_mod(c, m, c)) != MP_OKAY)
        return res;

    return MP_OKAY;
}

/* -------------------------- */
/* Compute c = (a - b) mod m */

int mp_submod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c)
{
    int res;

    ARGCHK(a != NULL && b != NULL && m != NULL && c != NULL, MP_BADARG);

    if ((res = mp_sub(a, b, c)) != MP_OKAY)
        return res;

    if ((res = mp_mod(c, m, c)) != MP_OKAY)
        return res;

    return MP_OKAY;
}

/* -------------------------- */
/* Compute c = (a * b) mod m */

int mp_mulmod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c)
{
  int res;

  ARGCHK(a != NULL && b != NULL && m != NULL && c != NULL, MP_BADARG);

  if ((res = mp_mul(a, b, c)) != MP_OKAY)
    return res;
  if ((res = mp_mod(c, m, c)) != MP_OKAY)
    return res;

  return MP_OKAY;
}

/* -------------------------- */

int mp_sqrmod(mp_int_t *a, mp_int_t *m, mp_int_t *c)
{
  int res;

  ARGCHK(a != NULL && m != NULL && c != NULL, MP_BADARG);

  if ((res = mp_sqr(a, c)) != MP_OKAY)
    return res;
  if ((res = mp_mod(c, m, c)) != MP_OKAY)
    return res;

  return MP_OKAY;
}

/* -------------------------- */
/* Compute c = (a ** b) mod m.  Uses a standard square-and-multiply
 * method with modular reductions at each step. (This is basically the
 * same code as mp_expt(), except for the addition of the reductions)
 * The modular reductions are done using Barrett's algorithm (see
 * s_mp_reduce() below for details)
 */

int mp_exptmod(mp_int_t *a, mp_int_t *b, mp_int_t *m, mp_int_t *c)
{
    mp_int_t s, x, mu;
    int res;
    mp_digit_t d, *db = DIGITS(b);
    unsigned int ub = USED(b);
    unsigned int dig, bit;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (mp_cmp_z(b) < 0 || mp_cmp_z(m) <= 0)
        return MP_RANGE;

    if ((res = mp_init(&s)) != MP_OKAY)
        return res;

    if ((res = mp_init_copy(&x, a)) != MP_OKAY)
        shortjmp(X);

    if ((res = mp_mod(&x, m, &x)) != MP_OKAY || (res = mp_init(&mu)) != MP_OKAY)
        shortjmp(MU);

    mp_set_d(&s, 1);

    /* mu = b^2k / m */

    s_mp_add_d(&mu, 1);
    s_mp_lshd(&mu, 2 * USED(m));

    if ((res = mp_div(&mu, m, &mu, NULL)) != MP_OKAY)
        shortjmp(CLEANUP);

    /* Loop over digits of b in ascending order, except highest order */

    for (dig = 0; dig < (ub - 1); dig++)
    {
        d = *db++;

        /* Loop over the bits of the lower-order digits */

        for (bit = 0; bit < DIGIT_BIT; bit++)
        {
            if (d & 1)
            {
                if ((res = s_mp_mul(&s, &x)) != MP_OKAY)
                    shortjmp(CLEANUP);
                if ((res = s_mp_reduce(&s, m, &mu)) != MP_OKAY)
                    shortjmp(CLEANUP);
            }

            d >>= 1;

            if ((res = s_mp_sqr(&x)) != MP_OKAY)
                shortjmp(CLEANUP);

            if ((res = s_mp_reduce(&x, m, &mu)) != MP_OKAY)
                shortjmp(CLEANUP);
        }
    }

    /* Now do the last digit... */

    d = *db;

    while (d)
    {
        if (d & 1)
        {
            if ((res = s_mp_mul(&s, &x)) != MP_OKAY)
                shortjmp(CLEANUP);
            if ((res = s_mp_reduce(&s, m, &mu)) != MP_OKAY)
                shortjmp(CLEANUP);
        }

        d >>= 1;

        if ((res = s_mp_sqr(&x)) != MP_OKAY)
            shortjmp(CLEANUP);

        if ((res = s_mp_reduce(&x, m, &mu)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    s_mp_exch(&s, c);

CLEANUP:
    mp_clear(&mu);
MU:
    mp_clear(&x);
X:
    mp_clear(&s);

    return res;
}

/* -------------------------- */

int mp_exptmod_d(mp_int_t *a, mp_digit_t d, mp_int_t *m, mp_int_t *c)
{
    mp_int_t s, x;
    int res;

    ARGCHK(a != NULL && c != NULL, MP_BADARG);

    if ((res = mp_init(&s)) != MP_OKAY)
        return res;

    if ((res = mp_init_copy(&x, a)) != MP_OKAY)
        shortjmp(X);

    mp_set_d(&s, 1);

    while (d != 0)
    {
        if (d & 1)
        {
            if ((res = s_mp_mul(&s, &x)) != MP_OKAY || (res = mp_mod(&s, m, &s)) != MP_OKAY)
                shortjmp(CLEANUP);
        }

        d /= 2;

        if ((res = s_mp_sqr(&x)) != MP_OKAY || (res = mp_mod(&x, m, &x)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    s_mp_exch(&s, c);

CLEANUP:
    mp_clear(&x);

X:
    mp_clear(&s);

    return res;
}

/* -------------------------- */
/* Compare a <=> 0.  Returns <0 if a<0, 0 if a=0, >0 if a>0.  */

int mp_cmp_z(mp_int_t *a)
{
    if (SIGN(a) == MP_NEG)
        return MP_LT;
    else if (USED(a) == 1 && DIGIT(a, 0) == 0)
        return MP_EQ;
    else
        return MP_GT;
}

/* -------------------------- */
/* Compare a <=> d.  Returns <0 if a<d, 0 if a=d, >0 if a>d */

int mp_cmp_d(mp_int_t *a, mp_digit_t d)
{
  ARGCHK(a != NULL, MP_EQ);

  if (SIGN(a) == MP_NEG)
    return MP_LT;

  return s_mp_cmp_d(a, d);
}

/* -------------------------- */
/* Compare a <=> d.  Returns <0 if a<d, 0 if a=d, >0 if a>d */

int mp_cmp(mp_int_t *a, mp_int_t *b)
{
    ARGCHK(a != NULL && b != NULL, MP_EQ);

    if (SIGN(a) == SIGN(b))
    {
        int mag;

        if ((mag = s_mp_cmp(a, b)) == MP_EQ)
            return MP_EQ;

        if (SIGN(a) == MP_ZPOS)
            return mag;
        else
            return -mag;
    }
    else if (SIGN(a) == MP_ZPOS)
    {
        return MP_GT;
    }
    else
    {
        return MP_LT;
    }
}

/* -------------------------- */
/* Compares |a| <=> |b|, and returns an appropriate comparison result */

int mp_cmp_mag(mp_int_t *a, mp_int_t *b)
{
    ARGCHK(a != NULL && b != NULL, MP_EQ);
    return s_mp_cmp(a, b);
}

/* -------------------------- */
/* This just converts z to an mp_int, and uses the existing comparison
 * routines.  This is sort of inefficient, but it's not clear to me how
 * frequently this wil get used anyway.  For small positive constants,
 * you can always use mp_cmp_d(), and for zero, there is mp_cmp_z().
 */

int mp_cmp_int(mp_int_t *a, long z)
{
    mp_int_t tmp;
    int out;

    ARGCHK(a != NULL, MP_EQ);

    mp_init(&tmp); mp_set_int(&tmp, z);
    out = mp_cmp(a, &tmp);
    mp_clear(&tmp);

    return out;
}

/* -------------------------- */
/* Returns a true (non-zero) value if a is odd, false (zero) otherwise. */

int mp_isodd(mp_int_t *a)
{
    ARGCHK(a != NULL, 0);
    return (DIGIT(a, 0) & 1);
}

/* -------------------------- */
/* Returns a true (non-zero) value if a is even, false (zero) otherwise. */

int mp_iseven(mp_int_t *a)
{
  return !mp_isodd(a);
}

/* -------------------------- */
/* Binary algorithm due to Josef Stein in 1961 (via Knuth). */

int mp_gcd(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res;
    mp_int_t u, v, t;
    mp_digit_t k = 0;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (mp_cmp_z(a) == MP_EQ && mp_cmp_z(b) == MP_EQ)
        return MP_RANGE;

    if (mp_cmp_z(a) == MP_EQ)
    {
        if ((res = mp_copy(b, c)) != MP_OKAY)
            return res;
        SIGN(c) = MP_ZPOS; return MP_OKAY;
    }
    else if (mp_cmp_z(b) == MP_EQ)
    {
        if ((res = mp_copy(a, c)) != MP_OKAY)
            return res;
        SIGN(c) = MP_ZPOS; return MP_OKAY;
    }

    if ((res = mp_init(&t)) != MP_OKAY)
        return res;

    if ((res = mp_init_copy(&u, a)) != MP_OKAY)
        shortjmp(U);

    if ((res = mp_init_copy(&v, b)) != MP_OKAY)
        shortjmp(V);

    SIGN(&u) = MP_ZPOS;
    SIGN(&v) = MP_ZPOS;

    /* Divide out common factors of 2 until at least 1 of a, b is even */

    while (mp_iseven(&u) && mp_iseven(&v))
    {
        s_mp_div_2(&u);
        s_mp_div_2(&v);
        ++k;
    }

    /* Initialize t */

    if (mp_isodd(&u))
    {
        if ((res = mp_copy(&v, &t)) != MP_OKAY)
            shortjmp(CLEANUP);

        /* t = -v */
        if (SIGN(&v) == MP_ZPOS)
            SIGN(&t) = MP_NEG;
        else
            SIGN(&t) = MP_ZPOS;
    }
    else
    {
        if ((res = mp_copy(&u, &t)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    for (EVER)
    {
        while (mp_iseven(&t))
        {
            s_mp_div_2(&t);
        }

        if (mp_cmp_z(&t) == MP_GT)
        {
            if ((res = mp_copy(&t, &u)) != MP_OKAY)
                shortjmp(CLEANUP);
        }
        else
        {
            if ((res = mp_copy(&t, &v)) != MP_OKAY)
                shortjmp(CLEANUP);

            /* v = -t */
            if (SIGN(&t) == MP_ZPOS)
                SIGN(&v) = MP_NEG;
            else
                SIGN(&v) = MP_ZPOS;
        }

        if ((res = mp_sub(&u, &v, &t)) != MP_OKAY)
            shortjmp(CLEANUP);

        if (s_mp_cmp_d(&t, 0) == MP_EQ)
            break;
    }

    s_mp_2expt(&v, k); /* v = 2^k */
    res = mp_mul(&u, &v, c); /* c = u * v */

 CLEANUP:
    mp_clear(&v);
 V:
    mp_clear(&u);
 U:
    mp_clear(&t);

    return res;
}

/* -------------------------- */
/* We compute the least common multiple using the rule:
 *
 *  ab = [a, b](a, b)
 *
 * ... by computing the product, and dividing out the gcd.
 */

int mp_lcm(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
  mp_int_t gcd, prod;
  int res;

  ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

  /* Set up temporaries */
  if ((res = mp_init(&gcd)) != MP_OKAY)
    return res;
  if ((res = mp_init(&prod)) != MP_OKAY)
    shortjmp(GCD);

  if ((res = mp_mul(a, b, &prod)) != MP_OKAY)
    shortjmp(CLEANUP);
  if ((res = mp_gcd(a, b, &gcd)) != MP_OKAY)
    shortjmp(CLEANUP);

  res = mp_div(&prod, &gcd, c, NULL);

 CLEANUP:
  mp_clear(&prod);
 GCD:
  mp_clear(&gcd);

  return res;
}

/* -------------------------- */

/* Compute g = (a, b) and values x and y satisfying Bezout's identity
 * (that is, ax + by = g).  This uses the extended binary GCD algorithm
 * based on the Stein algorithm used for mp_gcd()
 */

int mp_xgcd(mp_int_t *a, mp_int_t *b, mp_int_t *g, mp_int_t *x, mp_int_t *y)
{
    mp_int_t gx, xc, yc, u, v, A, B, C, D;
    mp_int_t *clean[9];
    int res;
    int last = -1;

    if (mp_cmp_z(b) == 0)
       return MP_RANGE;

    /* Initialize all these variables we need */

    if ((res = mp_init(&u)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &u;

    if ((res = mp_init(&v)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &v;

    if ((res = mp_init(&gx)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &gx;

    if ((res = mp_init(&A)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &A;

    if ((res = mp_init(&B)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &B;

    if ((res = mp_init(&C)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &C;

    if ((res = mp_init(&D)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &D;

    if ((res = mp_init_copy(&xc, a)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &xc;

    mp_abs(&xc, &xc);

    if ((res = mp_init_copy(&yc, b)) != MP_OKAY) shortjmp(CLEANUP);
        clean[++last] = &yc;

    mp_abs(&yc, &yc);

    mp_set_d(&gx, 1);

    /* Divide by two until at least one of them is even */

    while (mp_iseven(&xc) && mp_iseven(&yc))
    {
        s_mp_div_2(&xc);
        s_mp_div_2(&yc);
        if ((res = s_mp_mul_2(&gx)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    mp_copy(&xc, &u);
    mp_copy(&yc, &v);
    mp_set_d(&A, 1);
    mp_set_d(&D, 1);

    /* Loop through binary GCD algorithm */

    for (EVER)
    {
        while (mp_iseven(&u))
        {
            s_mp_div_2(&u);

            if (mp_iseven(&A) && mp_iseven(&B))
            {
                s_mp_div_2(&A); s_mp_div_2(&B);
            }
            else
            {
                if ((res = mp_add(&A, &yc, &A)) != MP_OKAY) shortjmp(CLEANUP);
                    s_mp_div_2(&A);

                if ((res = mp_sub(&B, &xc, &B)) != MP_OKAY) shortjmp(CLEANUP);
                    s_mp_div_2(&B);
            }
        }

        while (mp_iseven(&v))
        {
            s_mp_div_2(&v);

            if (mp_iseven(&C) && mp_iseven(&D))
            {
                s_mp_div_2(&C); s_mp_div_2(&D);
            }
            else
            {
                if ((res = mp_add(&C, &yc, &C)) != MP_OKAY) shortjmp(CLEANUP);
                    s_mp_div_2(&C);

                if ((res = mp_sub(&D, &xc, &D)) != MP_OKAY) shortjmp(CLEANUP);
                    s_mp_div_2(&D);
            }
        }

        if (mp_cmp(&u, &v) >= 0)
        {
            if ((res = mp_sub(&u, &v, &u)) != MP_OKAY)
                shortjmp(CLEANUP);

            if ((res = mp_sub(&A, &C, &A)) != MP_OKAY)
                shortjmp(CLEANUP);

            if ((res = mp_sub(&B, &D, &B)) != MP_OKAY)
                shortjmp(CLEANUP);
        }
        else
        {
            if ((res = mp_sub(&v, &u, &v)) != MP_OKAY)
                shortjmp(CLEANUP);

            if ((res = mp_sub(&C, &A, &C)) != MP_OKAY)
                shortjmp(CLEANUP);

            if ((res = mp_sub(&D, &B, &D)) != MP_OKAY)
                shortjmp(CLEANUP);

        }

        /* If we're done, copy results to output */

        if (mp_cmp_z(&u) == 0)
        {
            if (x)
            {
                if ((res = mp_copy(&C, x)) != MP_OKAY)
                    shortjmp(CLEANUP);
            }

            if (y)
            {
                if ((res = mp_copy(&D, y)) != MP_OKAY)
                    shortjmp(CLEANUP);
            }

            if (g)
            {
                if ((res = mp_mul(&gx, &v, g)) != MP_OKAY)
                    shortjmp(CLEANUP);
            }

            break;
        }
    }
CLEANUP:
    while (last >= 0)
        mp_clear(clean[last--]);

    return res;
}

/* -------------------------- */
/* Compute c = a^-1 (mod m), if there is an inverse for a (mod m).
 * This is equivalent to the question of whether (a, m) = 1.  If not,
 * MP_UNDEF is returned, and there is no inverse.
 */

int mp_invmod(mp_int_t *a, mp_int_t *m, mp_int_t *c)
{
    mp_int_t g, x;
    int sa;
    int res;

    ARGCHK(a && m && c, MP_BADARG);

    if (mp_cmp_z(a) == 0 || mp_cmp_z(m) == 0)
        return MP_RANGE;

    sa = SIGN(a);

    if ((res = mp_init(&g)) != MP_OKAY)
        return res;

    if ((res = mp_init(&x)) != MP_OKAY)
        shortjmp(X);

    if ((res = mp_xgcd(a, m, &g, &x, NULL)) != MP_OKAY)
        shortjmp(CLEANUP);

    if (mp_cmp_d(&g, 1) != MP_EQ)
    {
        res = MP_UNDEF;
        shortjmp(CLEANUP);
    }

    res = mp_mod(&x, m, c);

    SIGN(c) = sa;

CLEANUP:
    mp_clear(&x);
X:
    mp_clear(&g);

    return res;
}

/* -------------------------- */
/* Convert a's bit vector to its two's complement, up to the
 * number of words that it contains, storing result in b. The numeric value of
 * this result depends on the size of mpi_digit. This is a building block for
 * handling negative operands in the bit operations.
 */

int mp_2comp(mp_int_t *a, mp_int_t *b, unsigned int dig)
{
    int res;
    unsigned int ix, adig = USED(a);
    mp_digit_t *pa, *pb;
    mp_digit_t padding = ISNEG(a) ? MP_DIGIT_MAX : 0;
    mp_word_t w;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (a != b)
    {
        if ((res = mp_init_size(b, dig)) != MP_OKAY)
            return res;
        SIGN(b) = SIGN(a);
    }
    else
    {
        if ((res = s_mp_pad(b, dig)) != MP_OKAY)
            return res;
    }

    for (pa = DIGITS(a), pb = DIGITS(b), w = 0, ix = 0; ix < dig; ix++)
    {
        w += (ix == 0);
        w += (ix < adig) ? ~pa[ix] : padding;
        pb[ix] = ACCUM(w);
        w = CARRYOUT(w);
    }

    USED(b) = dig;

    return MP_OKAY;
}

/* -------------------------- */

int mp_and(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res = MP_OKAY;
    unsigned int ix, extent = 0;
    mp_digit_t *pa, *pb, *pc;
    mp_int_t tmp_a, tmp_b;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (a == b)
        return mp_copy(a, c);

    if (ISNEG(a))
    {
        extent = USED(b);
        if ((res = mp_2comp(a, &tmp_a, extent)) != MP_OKAY)
            shortjmp(out);
        a = &tmp_a;
    }

    if (ISNEG(b))
    {
        extent = USED(a);
        if ((res = mp_2comp(b, &tmp_b, extent)) != MP_OKAY)
            shortjmp(out);
        b = &tmp_b;
    }

    if (!extent)
        extent = mpi_min(USED(a), USED(b));

    if (c != a && c != b)
    {
        if ((res = mp_init_size(c, extent)) != MP_OKAY)
            shortjmp(out);
    }

    for (pa = DIGITS(a), pb = DIGITS(b), pc = DIGITS(c), ix = 0; ix < extent; ix++)
    {
        pc[ix] = pa[ix] & pb[ix];
    }

    USED(c) = extent;

    if (ISNEG(a) && ISNEG(b))
    {
        mp_2comp(c, c, extent);
        SIGN(c) = MP_NEG;
    }

    s_mp_clamp(c);

out:
    if (ISNEG(a))
        mp_clear(&tmp_a);

    if (ISNEG(b))
        mp_clear(&tmp_b);

    return res;
}

/* -------------------------- */

int mp_or(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res;
    unsigned int ix, extent, mindig;
    mp_digit_t *pa, *pb, *pc;
    mp_int_t tmp_a, tmp_b;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    extent = mpi_max(USED(a), USED(b));
    mindig = mpi_min(USED(a), USED(b));

    if (a == b)
        return mp_copy(a, c);

    if (ISNEG(a))
    {
        if ((res = mp_2comp(a, &tmp_a, extent)) != MP_OKAY)
            shortjmp(out);
        a = &tmp_a;
    }

    if (ISNEG(b))
    {
        if ((res = mp_2comp(b, &tmp_b, extent)) != MP_OKAY)
            shortjmp(out);
        b = &tmp_b;
    }

    if (c != a && c != b)
        res = mp_init_size(c, extent);
    else
        res = s_mp_pad(c, extent);

    if (res != MP_OKAY)
        shortjmp(out);

    for (pa = DIGITS(a), pb = DIGITS(b), pc = DIGITS(c), ix = 0; ix < mindig; ix++)
    {
        pc[ix] = pa[ix] | pb[ix];
    }

    if (ix < USED(a))
        s_mp_copy(pa + ix, pc + ix, USED(a) - ix);
    else if (ix < USED(b))
        s_mp_copy(pb + ix, pc + ix, USED(b) - ix);

    USED(c) = extent;

    if (ISNEG(a) || ISNEG(b))
    {
        mp_2comp(c, c, extent);
        SIGN(c) = MP_NEG;
    }

    s_mp_clamp(c);

out:
    if (ISNEG(a))
        mp_clear(&tmp_a);

    if (ISNEG(b))
        mp_clear(&tmp_b);

    return res;
}

/* -------------------------- */

int mp_xor(mp_int_t *a, mp_int_t *b, mp_int_t *c)
{
    int res;
    unsigned int ix, extent, mindig;
    mp_digit_t *pa, *pb, *pc;
    mp_int_t tmp_a, tmp_b;

    ARGCHK(a != NULL && b != NULL && c != NULL, MP_BADARG);

    if (a == b)
    {
        mp_set_zero(c);
        return MP_OKAY;
    }

    extent = mpi_max(USED(a), USED(b));
    mindig = mpi_min(USED(a), USED(b));

    if (ISNEG(a))
    {
        if ((res = mp_2comp(a, &tmp_a, extent)) != MP_OKAY)
            shortjmp(out);
        a = &tmp_a;
    }

    if (ISNEG(b))
    {
        if ((res = mp_2comp(b, &tmp_b, extent)) != MP_OKAY)
            shortjmp(out);
        b = &tmp_b;
    }

    if (c != a && c != b)
        res = mp_init_size(c, extent);
    else
        res = s_mp_pad(c, extent);

    if (res != MP_OKAY)
        shortjmp(out);

    for (pa = DIGITS(a), pb = DIGITS(b), pc = DIGITS(c), ix = 0; ix < mindig; ix++)
    {
        pc[ix] = pa[ix] ^ pb[ix];
    }

    if (ix < USED(a))
        s_mp_copy(pa + ix, pc + ix, USED(a) - ix);
    else if (ix < USED(b))
        s_mp_copy(pb + ix, pc + ix, USED(b) - ix);

    USED(c) = extent;

    if (ISNEG(a) ^ ISNEG(b))
    {
        mp_2comp(c, c, extent);
        SIGN(c) = MP_NEG;
    }

    s_mp_clamp(c);

out:
    if (ISNEG(a))
        mp_clear(&tmp_a);

    if (ISNEG(b))
        mp_clear(&tmp_b);

    return res;
}

/* -------------------------- */

int mp_comp(mp_int_t *a, mp_int_t *b)
{
    int res;
    unsigned int ix, dig = USED(a);
    mp_digit_t *pa, *pb;
    mp_int_t tmp;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (a != b)
        res = mp_init_size(b, dig);
    else
        res = s_mp_pad(b, dig);

    if (res != MP_OKAY)
        return res;

    if (ISNEG(a))
    {
        if ((res = mp_2comp(a, &tmp, dig)) != MP_OKAY)
            return res;
        a = &tmp;
    }

    for (pa = DIGITS(a), pb = DIGITS(b), ix = 0; ix < dig; ix++)
    {
        pb[ix] = ~pa[ix];
    }

    USED(b) = dig;

    if (ISNEG(a))
    {
        mp_clear(&tmp);
    }
    else
    {
        if ((res = mp_2comp(b, b, dig)) != MP_OKAY)
            return res;
        SIGN(b) = MP_NEG;
    }

    s_mp_clamp(b);
    return MP_OKAY;
}

/* -------------------------- */

int mp_trunc_comp(mp_int_t *a, mp_int_t *b, unsigned int bits)
{
    int res;
    unsigned int ix, dig = bits / DIGIT_BIT, rembits = bits % DIGIT_BIT;
    unsigned int adig = USED(a);
    mp_digit_t padding = ISNEG(a) ? MP_DIGIT_MAX : 0;
    int extra = (rembits != 0);
    mp_digit_t *pa, *pb;
    mp_int_t tmp;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (a != b)
        res = mp_init_size(b, dig + extra);
    else
        res = s_mp_pad(b, dig + extra);

    if (res != MP_OKAY)
        return res;

    if (ISNEG(a))
    {
        if ((res = mp_2comp(a, &tmp, dig + extra)) != MP_OKAY)
            return res;
        a = &tmp;
    }

    for (pa = DIGITS(a), pb = DIGITS(b), ix = 0; ix < dig; ix++)
    {
        pb[ix] = (ix < adig) ? ~pa[ix] : ~padding;
    }

    if (rembits)
    {
        mp_digit_t mask = (MP_DIGIT_MAX >> (DIGIT_BIT - rembits));
        pb[ix] = (((ix < adig) ? pa[ix] : padding) & mask) ^ mask;
    }

    USED(b) = dig + extra;

    if (ISNEG(a))
        mp_clear(&tmp);

    s_mp_clamp(b);
    return MP_OKAY;
}

/* -------------------------- */

int mp_trunc(mp_int_t *a, mp_int_t *b, unsigned int bits)
{
    int res;
    unsigned int ix, dig = bits / DIGIT_BIT, rembits = bits % DIGIT_BIT;
    unsigned int adig = USED(a);
    mp_digit_t padding = ISNEG(a) ? MP_DIGIT_MAX : 0;
    int extra = (rembits != 0);
    mp_digit_t *pa, *pb;
    mp_int_t tmp;

    ARGCHK(a != NULL && b != NULL, MP_BADARG);

    if (a != b)
        res = mp_init_size(b, dig + extra);
    else
        res = s_mp_pad(b, dig + extra);

    if (res != MP_OKAY)
        return res;

    if (ISNEG(a))
    {
        if ((res = mp_2comp(a, &tmp, dig + extra)) != MP_OKAY)
            return res;
        a = &tmp;
    }

    for (pa = DIGITS(a), pb = DIGITS(b), ix = 0; ix < dig; ix++)
    {
        pb[ix] = (ix < adig) ? pa[ix] : padding;
    }

    if (rembits)
    {
        mp_digit_t mask = (MP_DIGIT_MAX >> (DIGIT_BIT - rembits));
        pb[ix] = ((ix < adig) ? pa[ix] : padding) & mask;
    }

    USED(b) = dig + extra;

    if (ISNEG(a))
        mp_clear(&tmp);

    s_mp_clamp(b);
    return MP_OKAY;
}

/* -------------------------- */

int mp_shift(mp_int_t *a, mp_int_t *b, int bits)
{
    mp_int_t tmp;
    int res;
    int a_neg = ISNEG(a);

    if (bits == 0)
        return mp_copy(a, b);

    if (a_neg)
    {
        unsigned int ua = USED(a);
        if ((res = mp_2comp(a, &tmp, ua)) != MP_OKAY)
            return res;
        SIGN(&tmp) = MP_ZPOS;
        a = &tmp;
    }

    if (bits > 0)
        res = mp_mul_2d(a, bits, b);
    else
        res = mp_div_2d(a, -bits, b, NULL);

    if (res != MP_OKAY)
    {
        if (a_neg)
            mp_clear(&tmp);
        return res;
    }

    if (a_neg)
    {
        unsigned int hb, msd;
        mp_digit_t *db;

        mp_clear(&tmp);

        msd = USED(b)-1;
        db = DIGITS(b);
        hb = s_highest_bit(db[msd]);

        if (hb < DIGIT_BIT)
            db[msd] |= MP_DIGIT_MAX << hb;

        if ((res = mp_2comp(b, b, USED(b))) != MP_OKAY)
            return res;

        SIGN(b) = MP_NEG;
        s_mp_clamp(b);
    }

    return MP_OKAY;
}

/* -------------------------- */

int mp_bit(mp_int_t *a, unsigned int bit)
{
    mp_int_t tmp;
    int res;
    int a_neg = ISNEG(a);
    unsigned int digit = bit / MP_DIGIT_BIT;
    mp_digit_t mask = convert(mp_digit_t, 1) << (bit % MP_DIGIT_BIT);

    if (a_neg)
    {
        if ((res = mp_2comp(a, &tmp, bit + 1)) != MP_OKAY)
            return res;
        SIGN(&tmp) = MP_ZPOS;
        a = &tmp;
    }

    res = (digit < USED(a) && (DIGITS(a)[digit] & mask) != 0) ? MP_YES : MP_NO;

    if (a_neg)
        mp_clear(&tmp);

    return res;
}


/* -------------------------- */

int mp_get_bit(mp_int_t *a, unsigned int bit)
{
    return (mp_bit(a,bit) == MP_OKAY) ? 1 : 0;

}

/* -------------------------- */

int mp_set_bit(mp_int_t *a, unsigned int bit,unsigned int value)
{
    return s_mp_set_bit(a,bit,value);
}


/* -------------------------- */

int mp_to_double(mp_int_t *mp, double *d)
{
    unsigned int ix;
    unsigned int used = USED(mp);
    mp_digit_t *dp = DIGITS(mp);
    static double mult;
    double out = dp[used - 1];

    if (!mult)
        mult = pow(2.0, MP_DIGIT_BIT);

    for (ix = used - 2; ix < MP_SIZE_MAX - 1; ix--)
    {
        out = out * mult;
        out += convert(double, dp[ix]);
    }

    if (SIGN(mp) == MP_NEG)
        out = -out;

   *d = out;
    return MP_OKAY;
}

/* -------------------------- */

void mp_print(mp_int_t *mp, int radix, FILE *ofp)
{
    int len;
    unsigned char *str;

    if (mp == NULL || ofp == NULL || radix < 2 || radix > MAX_RADIX)
        return;
    len =  mp_radix_size(mp, radix);
    if(len < 1)
    {
        fputs("0",ofp);
    }
    else
    {
        str = malloc(len);
        if(str)
        {
            mp_toradix_case(mp,str,radix,1);
            fputs((char *)str, ofp);
            free(str);
        }
    }
}

/* -------------------------- */

void mp_show(const char *prefix, mp_int_t *mp)
{
    if (prefix)
        printf("%s = 0x", prefix);

    if (mp)
    {
        unsigned int ix;
        const char *fmt = MP_DIGIT_FMT_1;

        for (ix = USED(mp) - 1; ix < MP_SIZE_MAX; ix--)
        {
            printf(fmt, DIGIT(mp, ix));
            fmt = MP_DIGIT_FMT;
        }

        if(SIGN(mp) == MP_NEG)
            printf(" NEG\n");
        else
            printf("\n");
    }
    else
    {
        printf("NULL\n");
    }
}


/* -------------------------- */

void mp_show_decimal(const char *prefix, mp_int_t *mp)
{
    if (prefix)
        printf("%s ", prefix);

    if(mp)
    {
        int len;
        unsigned char *str;

        len =  mp_radix_size(mp,10);
        if(len < 1)
        {
            printf("0\n");
        }
        else
        {
            str = malloc(len);
            if(str)
            {
                mp_toradix_case(mp,str,10,1);
                printf("%s\n",(char *)str);
                free(str);
            }
        }
    }
    else
    {
        printf("NULL\n");
    }
}


/* -------------------------- */
/* Read in a raw value (base 256) into the given mp_int_t */

int mp_read_signed_bin(mp_int_t *mp, unsigned char *str, size_t len)
{
    int res;

    ARGCHK(mp != NULL && str != NULL && len > 0, MP_BADARG);

    if ((res = mp_read_unsigned_bin(mp, str + 1, len - 1)) == MP_OKAY)
    {
        /* Get sign from first byte */
        if (str[0])
            SIGN(mp) = MP_NEG;
        else
            SIGN(mp) = MP_ZPOS;
    }
    return res;
}

/* -------------------------- */

size_t mp_signed_bin_size(mp_int_t *mp)
{
    ARGCHK(mp != NULL, 0);
    return mp_unsigned_bin_size(mp) + 1;
}

/* -------------------------- */

int mp_to_signed_bin(mp_int_t *mp, unsigned char *str)
{
    ARGCHK(mp != NULL && str != NULL, MP_BADARG);

    /* Caller responsible for allocating enough memory (use mp_raw_size(mp)) */
    str[0] = convert(char, SIGN(mp));

    return mp_to_unsigned_bin(mp, str + 1);
}

/* -------------------------- */
/* Read in an unsigned value (base 256) into the given mp_int_t */

int mp_read_unsigned_bin(mp_int_t *mp, unsigned char *str, size_t len)
{
    unsigned int ix;
    int res;

    ARGCHK(mp != NULL && str != NULL && len > 0, MP_BADARG);

    mp_set_zero(mp);

    for (ix = 0; ix < len; ix++)
    {
        if ((res = s_mp_mul_2d(mp, CHAR_BIT)) != MP_OKAY)
            return res;

        if ((res = mp_add_d(mp, str[ix], mp)) != MP_OKAY)
            return res;
    }

    return MP_OKAY;
}

/* -------------------------- */

size_t mp_unsigned_bin_size(mp_int_t *mp)
{
    mp_digit_t topdig;
    size_t count;

    ARGCHK(mp != NULL, 0);

    /* Special case for the value zero */
    if (USED(mp) == 1 && DIGIT(mp, 0) == 0)
        return 1;

    count = (USED(mp) - 1) * MP_DIGIT_SIZE;
    topdig = DIGIT(mp, USED(mp) - 1);

    while (topdig != 0)
    {
        ++count;
        topdig >>= CHAR_BIT;
    }

    return count;
}

/* -------------------------- */

int mp_to_unsigned_bin(mp_int_t *mp, unsigned char *str)
{
    mp_digit_t *dp, *end, d;
    unsigned char *spos;

    ARGCHK(mp != NULL && str != NULL, MP_BADARG);

    dp = DIGITS(mp);
    end = dp + USED(mp) - 1;
    spos = str;

    /* Special case for zero, quick test */
    if (dp == end && *dp == 0)
    {
       *str = '\0';
        return MP_OKAY;
    }

    /* Generate digits in reverse order */

    while (dp < end)
    {
        size_t i;

        d = *dp;
        for (i = 0; i < MP_DIGIT_SIZE; i++)
        {
            *spos = d & UCHAR_MAX;
            d >>= CHAR_BIT;
            ++spos;
        }

        ++dp;
    }

    /* Now handle last digit specially, high order zeroes are not written */

    d = *end;

    while (d != 0)
    {
        *spos = d & UCHAR_MAX;
        d >>= CHAR_BIT;
        ++spos;
    }

    /* Reverse everything to get digits in the correct order */

    while (--spos > str)
    {
        unsigned char t = *str;

        *str = *spos;
        *spos = t;
        ++str;
    }

    return MP_OKAY;
}

/* -------------------------- */

int mp_to_unsigned_buf(mp_int_t *mp, unsigned char *str, size_t size)
{
    mp_digit_t *dp, *end;
    unsigned char *spos;

    ARGCHK(mp != NULL && str != NULL, MP_BADARG);

    for (spos = str + size, dp = DIGITS(mp), end = dp + USED(mp); dp < end; dp++)
    {
        size_t i;
        mp_digit_t d = *dp;

        for (i = 0; i < MP_DIGIT_SIZE; i++)
        {
            if (dp + 1 == end && d == 0)
                break;
            ARGCHK(spos >= str, MP_RANGE);
            *--spos = d & 0xFF;
          d >>= 8;
        }
    }

    while (spos > str)
        *--spos = 0;

    return MP_OKAY;
}

/* -------------------------- */

unsigned int mp_count_bits(mp_int_t *mp)
{
  ARGCHK(mp != NULL, MP_BADARG);

  return s_highest_bit_mp(mp);
}

/* -------------------------- */

static unsigned int s_mp_count_ones(mp_int_t *mp)
{
    unsigned int ix;
    unsigned int c;
    mp_digit_t *dp = DIGITS(mp);

    for (c = 0, ix = USED(mp) - 1; ix < MP_SIZE_MAX; ix--)
    {
        mp_digit_t d = dp[ix];

        d = ((d & 0xAAAAAAAA) >>  1) + (d & 0x55555555);
        d = ((d & 0xCCCCCCCC) >>  2) + (d & 0x33333333);
        d = ((d & 0xF0F0F0F0) >>  4) + (d & 0x0F0F0F0F);
        d = ((d & 0xFF00FF00) >>  8) + (d & 0x00FF00FF);
        d = ((d & 0xFFFF0000) >> 16) + (d & 0x0000FFFF);
        c += d;
    }

    return c;
}

/* -------------------------- */

int mp_count_ones(mp_int_t *mp)
{
    if (SIGN(mp) == MP_NEG)
    {
        mp_int_t tmp;
        int res;

        if ((res = mp_init_copy(&tmp, mp)) != MP_OKAY)
            return res;

        if ((res = s_mp_sub_d(&tmp, 1) != MP_OKAY))
           return res;

        res = s_mp_count_ones(&tmp);
        mp_clear(&tmp);
        return res;
    }
    return s_mp_count_ones(mp);
}

/* -------------------------- */

unsigned int mp_is_pow_two(mp_int_t *mp)
{
  return s_mp_ispow2(mp) < MP_SIZE_MAX;
}

/* -------------------------- */
/* Read an integer from the given string, and set mp to the resulting
 * value.  The input is presumed to be in base 10.  Leading non-digit
 * characters are ignored, and the function reads until a non-digit
 * character or the end of the string.
 */

int mp_read_radix(mp_int_t *mp, const wchar_t *str, int radix)
{
    size_t ix = 0;
    int val = 0;
    int res;
    int sig = MP_ZPOS;

    ARGCHK(mp != NULL && str != NULL && radix >= 2 && radix <= MAX_RADIX,MP_BADARG);

    mp_set_zero(mp);

    /* Skip leading non-digit characters until a digit or '-' or '+' */

    while (str[ix] && (s_mp_tovalue(str[ix], radix) < 0) && str[ix] != '-' && str[ix] != '+')
    {
        ++ix;
    }

    if (str[ix] == '-')
    {
        sig = MP_NEG;
        ++ix;
    }
    else if (str[ix] == '+')
    {
        sig = MP_ZPOS; /* this is the default anyway... */
        ++ix;
    }

    while ((val = s_mp_tovalue(str[ix], radix)) >= 0)
    {
        if ((res = s_mp_mul_d(mp, radix)) != MP_OKAY)
            return res;

        if ((res = s_mp_add_d(mp, val)) != MP_OKAY)
          return res;
        ++ix;
    }

    if (s_mp_cmp_d(mp, 0) == MP_EQ)
        SIGN(mp) = MP_ZPOS;
    else
        SIGN(mp) = sig;

    return MP_OKAY;
}


/* -------------------------- */

int mp_read_radix_char(mp_int_t *mp, const char *str, int radix)
{
    size_t ix = 0;
    int val = 0;
    int res;
    int sig = MP_ZPOS;

    ARGCHK(mp != NULL && str != NULL && radix >= 2 && radix <= MAX_RADIX,MP_BADARG);

    mp_set_zero(mp);

    /* Skip leading non-digit characters until a digit or '-' or '+' */

    while (str[ix] && (s_mp_tovalue(str[ix], radix) < 0) && str[ix] != '-' && str[ix] != '+')
    {
        ++ix;
    }

    if (str[ix] == '-')
    {
        sig = MP_NEG;
        ++ix;
    }
    else if (str[ix] == '+')
    {
        sig = MP_ZPOS; /* this is the default anyway... */
        ++ix;
    }

    while ((val = s_mp_tovalue(str[ix], radix)) >= 0)
    {
        if ((res = s_mp_mul_d(mp, radix)) != MP_OKAY)
            return res;

        if ((res = s_mp_add_d(mp, val)) != MP_OKAY)
          return res;
        ++ix;
    }

    if (s_mp_cmp_d(mp, 0) == MP_EQ)
        SIGN(mp) = MP_ZPOS;
    else
        SIGN(mp) = sig;

    return MP_OKAY;
}


/* -------------------------- */

unsigned int mp_radix_size(mp_int_t *mp, int radix)
{
    size_t len;
    ARGCHK(mp != NULL, 0);

    len = s_mp_outlen(mp_count_bits(mp), radix) + 1; /* for NUL terminator */

    if (mp_cmp_z(mp) < 0)
        ++len; /* for sign */

    return len;
}

/* -------------------------- */
/* Return the number of digits in the specified radix that would be
 * needed to express 'num' digits of 'qty' bits each.
 */

unsigned int mp_value_radix_size(unsigned int num, unsigned int qty, int radix)
{
  ARGCHK(radix >= 2 && radix <= MAX_RADIX, 0);

  return s_mp_outlen(num * qty, radix);
}

/* -------------------------- */

int mp_toradix(mp_int_t *mp, unsigned char *str, int radix)
{
    return mp_toradix_case(mp, str, radix, 0);
}

/* -------------------------- */

int mp_toradix_case(mp_int_t *mp, unsigned char *str, int radix, int low)
{
    size_t ix, pos = 0;

    ARGCHK(mp != NULL && str != NULL, MP_BADARG);
    ARGCHK(radix > 1 && radix <= MAX_RADIX, MP_RANGE);

    if (mp_cmp_z(mp) == MP_EQ)
    {
        str[0] = '0';
        str[1] = '\0';
    }
    else
    {
        int res;
        mp_int_t tmp;
        int sgn;
        mp_digit_t rem, rdx = convert(mp_digit_t, radix);
        char ch;

        if ((res = mp_init_copy(&tmp, mp)) != MP_OKAY)
            return res;

        /* Save sign for later, and take absolute value */
        sgn = SIGN(&tmp); SIGN(&tmp) = MP_ZPOS;

        /* Generate output digits in reverse order */

        while (mp_cmp_z(&tmp) != 0)
        {
            if ((res = s_mp_div_d(&tmp, rdx, &rem)) != MP_OKAY)
            {
                mp_clear(&tmp);
                return res;
            }

            /* Generate digits, use capital letters */
            ch = s_mp_todigit(rem, radix, low);

            str[pos++] = ch;
        }

        /* Add - sign if original value was negative */

        if (sgn == MP_NEG)
            str[pos++] = '-';

        str[pos--] = '\0';

        /* Reverse the digits and sign indicator */

        ix = 0;
        while (ix < pos)
        {
            unsigned char tmp2 = str[ix];

            str[ix] = str[pos];
            str[pos] = tmp2;
            ++ix;
            --pos;
        }

        mp_clear(&tmp);
    }

    return MP_OKAY;
}

/* -------------------------- */

int mp_toradixw(mp_int_t *mp, wchar_t *str, int radix)
{
    return mp_toradix_casew(mp, str, radix, 0);
}

/* -------------------------- */

int mp_toradix_casew(mp_int_t *mp, wchar_t *str, int radix, int low)
{
    size_t ix, pos = 0;

    ARGCHK(mp != NULL && str != NULL, MP_BADARG);
    ARGCHK(radix > 1 && radix <= MAX_RADIX, MP_RANGE);

    if (mp_cmp_z(mp) == MP_EQ)
    {
        str[0] = L'0';
        str[1] = 0;
    }
    else
    {
        int res;
        mp_int_t tmp;
        int sgn;
        mp_digit_t rem, rdx = convert(mp_digit_t, radix);
        wchar_t ch;

        if ((res = mp_init_copy(&tmp, mp)) != MP_OKAY)
            return res;

        /* Save sign for later, and take absolute value */
        sgn = SIGN(&tmp); SIGN(&tmp) = MP_ZPOS;

        /* Generate output digits in reverse order */

        while (mp_cmp_z(&tmp) != 0)
        {
            if ((res = s_mp_div_d(&tmp, rdx, &rem)) != MP_OKAY)
            {
                mp_clear(&tmp);
                return res;
            }

            /* Generate digits, use capital letters */
            ch = s_mp_todigitw(rem, radix, low);

            str[pos++] = ch;
        }

        /* Add - sign if original value was negative */

        if (sgn == MP_NEG)
            str[pos++] = '-';

        str[pos--] = '\0';

        /* Reverse the digits and sign indicator */

        ix = 0;
        while (ix < pos)
        {
            unsigned char tmp2 = str[ix];

            str[ix] = str[pos];
            str[pos] = tmp2;
            ++ix;
            --pos;
        }

        mp_clear(&tmp);
    }

    return MP_OKAY;
}

/* -------------------------- */

int mp_char2value(char ch, int r)
{
  return s_mp_tovalue(ch, r);
}

/* -------------------------- */
/* Return a string describing the meaning of error code 'ec'.  The
 * string returned is allocated in static memory, so the caller should
 * not attempt to modify or free the memory associated with this
 * string.
 */

const char *mp_strerror(int ec)
{
    int aec = (ec < 0) ? -ec : ec;

    /* Code values are negative, so the senses of these comparisons are accurate */
    if (ec < MP_LAST_CODE || ec > MP_OKAY)
    {
        return int_string[0]; /* unknown error code */
    }
    else
    {
        return int_string[aec + 1];
    }
}

/* -------------------------- */
/* Make sure there are at least 'min' digits allocated to mp */

int s_mp_grow(mp_int_t *mp, unsigned int min)
{
    if (min > MP_MAX_DIGITS)
        return MP_TOOBIG;

    if (min > ALLOC(mp))
    {
        mp_digit_t *tmp;

        /* Set min to next nearest default precision block size */
        min = ((min + (s_mp_defprec - 1)) / s_mp_defprec) * s_mp_defprec;

        if ((tmp = coerce(mp_digit_t *, s_mp_alloc(min, MP_DIGIT_SIZE))) == NULL)
            return MP_MEM;

        s_mp_copy(DIGITS(mp), tmp, USED(mp));
        s_mp_setz(DIGITS(mp), ALLOC(mp));
        s_mp_free(DIGITS(mp));

        DIGITS(mp) = tmp;
        ALLOC(mp) = min;
    }

  return MP_OKAY;
}

/* -------------------------- */
/* Make sure the used size of mp is at least 'min', growing if needed */

int s_mp_pad(mp_int_t *mp, unsigned int min)
{
    if (min > USED(mp))
    {
        int res;

        /* Make sure there is room to increase precision */
        if (min > ALLOC(mp) && (res = s_mp_grow(mp, min)) != MP_OKAY)
            return res;

        /* Increase precision; should already be 0-filled */
        USED(mp) = min;
    }
    return MP_OKAY;
}

/* -------------------------- */
/* Remove leading zeroes from the given value */

void s_mp_clamp(mp_int_t *mp)
{
    unsigned int du = USED(mp);
    mp_digit_t *zp = DIGITS(mp) + du - 1;

    while (du > 1 && !*zp--)
        --du;

    if (du == 1 && *zp == 0)
        SIGN(mp) = MP_ZPOS;

    USED(mp) = du;
}

/* -------------------------- */

static unsigned int s_highest_bit(mp_digit_t n)
{
    if (n & 0xFFFF0000)
    {
        if (n & 0xFF000000)
        {
            if (n & 0xF0000000)
            {
                if (n & 0xC0000000)
                    return (n & 0x80000000) ? 32 : 31;
                else
                    return (n & 0x20000000) ? 30 : 29;
            }
            else
            {
                if (n & 0x0C000000)
                    return (n & 0x08000000) ? 28 : 27;
                else
                    return (n & 0x02000000) ? 26 : 25;
            }
        }
        else
        {
            if (n & 0x00F00000)
            {
                if (n & 0x00C00000)
                    return (n & 0x00800000) ? 24 : 23;
                else
                    return (n & 0x00200000) ? 22 : 21;
            }
            else
            {
                if (n & 0x000C0000)
                    return (n & 0x00080000) ? 20 : 19;
                else
                    return (n & 0x00020000) ? 18 : 17;
            }
        }
    }
    else
    {
        if (n & 0x0000FF00)
        {
            if (n & 0x0000F000)
            {
                if (n & 0x0000C000)
                    return (n & 0x00008000) ? 16 : 15;
                else
                    return (n & 0x00002000) ? 14 : 13;
            }
            else
            {
                if (n & 0x00000C00)
                    return (n & 0x00000800) ? 12 : 11;
                else
                    return (n & 0x00000200) ? 10 : 9;
            }
        }
        else
        {
            if (n & 0x000000F0)
            {
                if (n & 0x000000C0)
                    return (n & 0x00000080) ? 8 : 7;
                else
                    return (n & 0x00000020) ? 6 : 5;
            }
            else
            {
                if (n & 0x0000000C)
                    return (n & 0x00000008) ? 4 : 3;
                else
                    return (n & 0x00000002) ? 2 : (n ? 1 : 0);
            }
        }
    }
}

/* -------------------------- */

unsigned int s_highest_bit_mp(mp_int_t *a)
{
    unsigned int nd1 = USED(a) - 1;
    return s_highest_bit(DIGIT(a, nd1)) + nd1 * MP_DIGIT_BIT;
}

/* -------------------------- */

int s_mp_set_bit(mp_int_t *a, unsigned int bit,unsigned int value)
{
    unsigned int nd = (bit + MP_DIGIT_BIT) / MP_DIGIT_BIT;
    unsigned int nbit = bit - (nd - 1) * MP_DIGIT_BIT;
    unsigned int mask = (convert(mp_digit_t, 1) << nbit);
    int res = MP_OKAY;

    if (nd)
    {
        if(value)
        {
            res = s_mp_pad(a, nd);
            if(res == MP_OKAY)
                DIGIT(a, nd - 1) |= mask;
        }
        else if(DIGIT(a, nd - 1) & mask)
        {
            DIGIT(a, nd - 1) &= ~mask;
            s_mp_clamp(a);
        }
    }
    return res;
}

/* -------------------------- */
/* Exchange the data for a and b; (b, a) = (a, b) */

void s_mp_exch(mp_int_t *a, mp_int_t *b)
{
    mp_int_t tmp;

    tmp = *a;
   *a = *b;
   *b = tmp;
}

/* -------------------------- */
/* Shift mp leftward by p digits, growing if needed, and zero-filling
 * the in-shifted digits at the right end.  This is a convenient
 * alternative to multiplication by powers of the radix
 */

int s_mp_lshd(mp_int_t *mp, unsigned int p)
{
    int res;
    unsigned int pos;
    mp_digit_t *dp;
    unsigned int ix;

    if (p == 0)
        return MP_OKAY;

    if ((res = s_mp_pad(mp, USED(mp) + p)) != MP_OKAY)
        return res;

    pos = USED(mp) - 1;
    dp = DIGITS(mp);

    /* Shift all the significant figures over as needed */

    for (ix = pos - p; ix < MP_SIZE_MAX - p; ix--)
    {
        dp[ix + p] = dp[ix];
    }

    /* Fill the bottom digits with zeroes */

    for (ix = 0; ix < p; ix++)
    {
        dp[ix] = 0;
    }

    return MP_OKAY;
}

/* -------------------------- */
/* Shift mp rightward by p digits.  Maintains the invariant that
 * digits above the precision are all zero.  Digits shifted off the
 * end are lost.  Cannot fail.
 */

void s_mp_rshd(mp_int_t *mp, unsigned int p)
{
    unsigned int ix;
    mp_digit_t *dp;

    if (p == 0)
        return;

    /* Shortcut when all digits are to be shifted off */

    if (p >= USED(mp))
    {
        s_mp_setz(DIGITS(mp), ALLOC(mp));
        USED(mp) = 1;
        SIGN(mp) = MP_ZPOS;
        return;
    }

    /* Shift all the significant figures over as needed */

    dp = DIGITS(mp);

    for (ix = p; ix < USED(mp); ix++)
    {
        dp[ix - p] = dp[ix];
    }

    /* Fill the top digits with zeroes */

    ix -= p;

    while (ix < USED(mp))
    {
        dp[ix++] = 0;
    }

    /* Strip off any leading zeroes */
    s_mp_clamp(mp);
}

/* -------------------------- */
/* Divide by two -- take advantage of radix properties to do it fast */

void s_mp_div_2(mp_int_t *mp)
{
    s_mp_div_2d(mp, 1);
}

/* -------------------------- */

int s_mp_mul_2(mp_int_t *mp)
{
    unsigned int ix;
    mp_digit_t kin = 0, kout, *dp = DIGITS(mp);
    int res;

    /* Shift digits leftward by 1 bit */
    for (ix = 0; ix < USED(mp); ix++)
    {
        kout = (dp[ix] >> (DIGIT_BIT - 1)) & 1;
        dp[ix] = (dp[ix] << 1) | kin;
        kin = kout;
    }

    /* Deal with rollover from last digit */

    if (kin)
    {
        if (ix >= ALLOC(mp))
        {
            if ((res = s_mp_grow(mp, ALLOC(mp) + 1)) != MP_OKAY)
                return res;
            dp = DIGITS(mp);
        }

        dp[ix] = kin;
        USED(mp) += 1;
    }

    return MP_OKAY;
}

/* -------------------------- */
/* Remainder the integer by 2^d, where d is a number of bits.  This
 * amounts to a bitwise AND of the value, and does not require the full
 * division code
 */

void s_mp_mod_2d(mp_int_t *mp, mp_digit_t d)
{
    mp_digit_t ndig = (d / DIGIT_BIT), nbit = (d % DIGIT_BIT);
    unsigned int ix;
    mp_digit_t dmask, *dp = DIGITS(mp);

    if (ndig >= USED(mp))
        return;

    /* Flush all the bits above 2^d in its digit */

    dmask = (convert(mp_digit_t, 1) << nbit) - 1;
    dp[ndig] &= dmask;

    /* Flush all digits above the one with 2^d in it */

    for (ix = ndig + 1; ix < USED(mp); ix++)
    {
        dp[ix] = 0;
    }

    s_mp_clamp(mp);
}

/* -------------------------- */
/* Multiply by the integer 2^d, where d is a number of bits.  This
 * amounts to a bitwise shift of the value, and does not require the
 * full multiplication code.
 */

int s_mp_mul_2d(mp_int_t *mp, mp_digit_t d)
{
    int res;
    mp_digit_t save, next, mask, *dp;
    unsigned int used;
    unsigned int ix;

    if ((res = s_mp_lshd(mp, d / DIGIT_BIT)) != MP_OKAY)
        return res;

    dp = DIGITS(mp); used = USED(mp);

    if ((d %= DIGIT_BIT) != 0)
    {
        mask = (convert(mp_digit_t, 1) << d) - 1;

        /* If the shift requires another digit, make sure we've got one to work with */
        if ((dp[used - 1] >> (DIGIT_BIT - d)) & mask)
        {
            if ((res = s_mp_grow(mp, used + 1)) != MP_OKAY)
                return res;
            dp = DIGITS(mp);
        }

        /* Do the shifting... */

        save = 0;

        for (ix = 0; ix < used; ix++)
        {
            next = (dp[ix] >> (DIGIT_BIT - d)) & mask;
            dp[ix] = (dp[ix] << d) | save;
            save = next;
        }

        /* If, at this point, we have a nonzero carryout into the next
         * digit, we'll increase the size by one digit, and store it...
         */

        if (save)
        {
            dp[used] = save;
            USED(mp) += 1;
        }
    }

    return MP_OKAY;
}

/* -------------------------- */
/* Divide the integer by 2^d, where d is a number of bits.  This
 * amounts to a bitwise shift of the value, and does not require the
 * full division code (used in Barrett reduction, see below)
 */

void s_mp_div_2d(mp_int_t *mp, mp_digit_t d)
{
    unsigned int ix;
    mp_digit_t save, next, mask, *dp = DIGITS(mp);

    s_mp_rshd(mp, d / DIGIT_BIT);
    d %= DIGIT_BIT;

    mask = (convert(mp_digit_t, 1) << d) - 1;

    save = 0;

    for (ix = USED(mp) - 1; ix < MP_SIZE_MAX; ix--)
    {
        next = dp[ix] & mask;
        dp[ix] = (dp[ix] >> d) | (save << (DIGIT_BIT - d));
        save = next;
    }

    s_mp_clamp(mp);
}

/* -------------------------- */
/* Normalize a and b for division, where b is the divisor.  In order
 * that we might make good guesses for quotient digits, we want the
 * leading digit of b to be at least half the radix, which we
 * accomplish by multiplying a and b by a constant.  This constant is
 * returned (so that it can be divided back out of the remainder at the
 * end of the division process).
 * We multiply by the smallest power of 2 that gives us a leading digit
 * at least half the radix.  By choosing a power of 2, we simplify the
 * multiplication and division steps to simple shifts.
 */

mp_digit_t s_mp_norm(mp_int_t *a, mp_int_t *b)
{
    mp_digit_t t, d = 0;

    t = DIGIT(b, USED(b) - 1);

    d = MP_DIGIT_BIT - s_highest_bit(t);
    t <<= d;

    if (d != 0)
    {
        s_mp_mul_2d(a, d);
        s_mp_mul_2d(b, d);
    }

    return d;
}

/* -------------------------- */
/* Add d to |mp| in place */

int s_mp_add_d(mp_int_t *mp, mp_digit_t d) /* unsigned digit addition */
{
    mp_word_t w, k = 0;
    unsigned int ix = 1, used = USED(mp);
    mp_digit_t *dp = DIGITS(mp);

    w = convert(mp_word_t, dp[0]) + d;
    dp[0] = ACCUM(w);
    k = CARRYOUT(w);

    while (ix < used && k)
    {
        w = dp[ix] + k;
        dp[ix] = ACCUM(w);
        k = CARRYOUT(w);
        ++ix;
    }

    if (k != 0)
    {
        int res;

        if ((res = s_mp_pad(mp, USED(mp) + 1)) != MP_OKAY)
            return res;

        DIGIT(mp, ix) = k;
    }
    return MP_OKAY;
}

/* -------------------------- */
/* Subtract d from |mp| in place, assumes |mp| > d */

int s_mp_sub_d(mp_int_t *mp, mp_digit_t d) /* unsigned digit subtract */
{
    mp_word_t w, b = 0;
    unsigned int ix = 1, used = USED(mp);
    mp_digit_t *dp = DIGITS(mp);

    /* Compute initial subtraction */
    w = (RADIX + dp[0]) - d;
    b = CARRYOUT(w) ? 0 : 1;
    dp[0] = ACCUM(w);

    /* Propagate borrows leftward */

    while (b && ix < used)
    {
        w = (RADIX + dp[ix]) - b;
        b = CARRYOUT(w) ? 0 : 1;
        dp[ix] = ACCUM(w);
        ++ix;
    }

    /* Remove leading zeroes */

    s_mp_clamp(mp);

    /* If we have a borrow out, it's a violation of the input invariant */
    if (b)
        return MP_RANGE;
    //else
    return MP_OKAY;
}

/* -------------------------- */
/* Compute a = a * d, single digit multiplication */

int s_mp_mul_d(mp_int_t *a, mp_digit_t d)
{
    mp_word_t w, k = 0;
    unsigned int ix, max;
    int res;
    mp_digit_t *dp = DIGITS(a);

    max = USED(a);

    for (ix = 0; ix < max; ix++)
    {
        w = dp[ix] * convert(mp_word_t, d) + k;
        dp[ix] = ACCUM(w);
        k = CARRYOUT(w);
    }

    /* If there is a carry out, we must ensure
    * we have enough storage for the extra digit.
    * If there is carry, there are no leading zeros
    * don't waste time calling s_mp_clamp.
    */
    if (k)
    {
        if ((res = s_mp_pad(a, max + 1)) != MP_OKAY)
            return res;
        DIGIT(a, max) = k;
        USED(a) = max + 1;
    }
    else
    {
        s_mp_clamp(a);
    }
    return MP_OKAY;
}

/* -------------------------- */
/* Compute the quotient mp = mp / d and remainder r = mp mod d, for a
 * single digit d.  If r is null, the remainder will be discarded.
 */

int s_mp_div_d(mp_int_t *mp, mp_digit_t d, mp_digit_t *r)
{
    mp_word_t w = 0, t;
    mp_int_t quot;
    int res;
    mp_digit_t *dp = DIGITS(mp), *qp;
    unsigned int ix;

    if (d == 0)
        return MP_RANGE;

    /* Make room for the quotient */
    if ((res = mp_init_size(&quot, USED(mp))) != MP_OKAY)
        return res;

    USED(&quot) = USED(mp); /* so clamping will work below */
    qp = DIGITS(&quot);

    /* Divide without subtraction */

    for (ix = USED(mp) - 1; ix < MP_SIZE_MAX; ix--)
    {
        w = (w << DIGIT_BIT) | dp[ix];

        if (w >= d)
        {
            t = w / d;
            w = w % d;
        }
        else
        {
            t = 0;
        }

        //assert (t <= MP_DIGIT_MAX);
        qp[ix] = t;
    }

    /* Deliver the remainder, if desired */

    if (r)
    {
        //assert (w <= MP_DIGIT_MAX);
       *r = w;
    }

    s_mp_clamp(&quot);
    mp_exch(&quot, mp);
    mp_clear(&quot);

    return MP_OKAY;
}

/* -------------------------- */
/* Compute a = |a| + |b| */

int s_mp_add(mp_int_t *a, mp_int_t *b) /* magnitude addition */
{
    mp_word_t w = 0;
    mp_digit_t *pa, *pb;
    unsigned int ix, used = USED(b);
    int res;

    /* Make sure a has enough precision for the output value */
    if ((used > USED(a)) && (res = s_mp_pad(a, used)) != MP_OKAY)
        return res;

  /* Add up all digits up to the precision of b.  If b had initially
   * the same precision as a, or greater, we took care of it by the
   * padding step above, so there is no problem.  If b had initially
   * less precision, we'll have to make sure the carry out is duly
   * propagated upward among the higher-order digits of the sum.
   */

    pa = DIGITS(a);
    pb = DIGITS(b);

    for (ix = 0; ix < used; ++ix)
    {
        w += *pa + convert(mp_word_t, *pb++);
        *pa++ = ACCUM(w);
        w = CARRYOUT(w);
    }

  /* If we run out of 'b' digits before we're actually done, make
   * sure the carries get propagated upward...
   */

    used = USED(a);

    while (w && ix < used)
    {
        w += *pa;
        *pa++ = ACCUM(w);
        w = CARRYOUT(w);
        ++ix;
    }

  /* If there's an overall carry out, increase precision and include
   * it.  We could have done this initially, but why touch the memory
   * allocator unless we're sure we have to?
   */

    if (w)
    {
        if ((res = s_mp_pad(a, used + 1)) != MP_OKAY)
            return res;
        DIGIT(a, ix) = w; /* pa may not be valid after s_mp_pad() call */
    }

    return MP_OKAY;
}

/* -------------------------- */
/* Compute a = |a| - |b|, assumes |a| >= |b| */

int s_mp_sub(mp_int_t *a, mp_int_t *b) /* magnitude subtract */
{
    mp_word_t w = 0;
    mp_digit_t *pa, *pb;
    unsigned int ix, used = USED(b);

  /* Subtract and propagate borrow.  Up to the precision of b, this
   * accounts for the digits of b; after that, we just make sure the
   * carries get to the right place.  This saves having to pad b out to
   * the precision of a just to make the loops work right...
   */

    pa = DIGITS(a);
    pb = DIGITS(b);

    for (ix = 0; ix < used; ++ix)
    {
        w = (RADIX + *pa) - w - *pb++;
       *pa++ = ACCUM(w);
        w = CARRYOUT(w) ? 0 : 1;
    }

    used = USED(a);

    while (ix < used)
    {
        w = RADIX + *pa - w;
       *pa++ = ACCUM(w);
        w = CARRYOUT(w) ? 0 : 1;
        ++ix;
    }

    /* Clobber any leading zeroes we created */

    s_mp_clamp(a);

  /* If there was a borrow out, then |b| > |a| in violation
   * of our input invariant.  We've already done the work,
   * but we'll at least complain about it...
   */
    if (w)
        return MP_RANGE;
    //else
    return MP_OKAY;
}

/* -------------------------- */
/* Compute a = |a| * |b| */

int s_mp_mul(mp_int_t *a, mp_int_t *b)
{
    mp_word_t w, k = 0;
    mp_int_t tmp;
    int res;
    unsigned int ix, jx, ua = USED(a), ub = USED(b);
    mp_digit_t *pa, *pb, *pt, *pbt;

    if ((res = mp_init_size(&tmp, ua + ub)) != MP_OKAY)
        return res;

    /* This has the effect of left-padding with zeroes... */
    USED(&tmp) = ua + ub;

    /* We're going to need the base value each iteration */
    pbt = DIGITS(&tmp);

    /* Outer loop:  Digits of b */

    pb = DIGITS(b);

    for (ix = 0; ix < ub; ++ix, ++pb)
    {
        if (*pb == 0)
            continue;

        /* Inner product:  Digits of a */
        pa = DIGITS(a);

        for (jx = 0; jx < ua; ++jx, ++pa)
        {
            pt = pbt + ix + jx;
            w = *pb * convert(mp_word_t, *pa) + k + *pt;
           *pt = ACCUM(w);
            k = CARRYOUT(w);
        }

        pbt[ix + jx] = k;
        k = 0;
    }

    s_mp_clamp(&tmp);
    s_mp_exch(&tmp, a);

    mp_clear(&tmp);

    return MP_OKAY;
}

/* -------------------------- */
/* Computes the square of a, in place.  This can be done more
 * efficiently than a general multiplication, because many of the
 * computation steps are redundant when squaring.  The inner product
 * step is a bit more complicated, but we save a fair number of
 * iterations of the multiplication loop.
 */

int s_mp_sqr(mp_int_t *a)
{
    mp_word_t w, k = 0;
    mp_int_t tmp;
    int res;
    unsigned int ix, jx, kx, used = USED(a);
    mp_digit_t *pa1, *pa2, *pt, *pbt;

    if ((res = mp_init_size(&tmp, 2 * used)) != MP_OKAY)
        return res;

    /* Left-pad with zeroes */

    USED(&tmp) = 2 * used;

    /* We need the base value each time through the loop */
    pbt = DIGITS(&tmp);

    pa1 = DIGITS(a);

    for (ix = 0; ix < used; ++ix, ++pa1)
    {
        if (*pa1 == 0)
            continue;

        w = DIGIT(&tmp, ix + ix) + *pa1 * convert(mp_word_t, *pa1);

        pbt[ix + ix] = ACCUM(w);
        k = CARRYOUT(w);

    /* The inner product is computed as:
     * (C, S) = t[i,j] + 2 a[i] a[j] + C
     * This can overflow what can be represented in an mp_word_t, and
     * since C arithmetic does not provide any way to check for
     * overflow, we have to check explicitly for overflow conditions
     * before they happen.
     */
        for (jx = ix + 1, pa2 = DIGITS(a) + jx; jx < used; ++jx, ++pa2)
        {
            mp_word_t u = 0, v;

            /* Store this in a temporary to avoid indirections later */
            pt = pbt + ix + jx;

            /* Compute the multiplicative step */
            w = *pa1 * convert(mp_word_t, *pa2);

            /* If w is more than half MP_WORD_MAX, the doubling will
             * overflow, and we need to record a carry out into the next
             * word */
            u = (w >> (MP_WORD_BIT - 1)) & 1;

            /* Double what we've got, overflow will be ignored as defined
             * for C arithmetic (we've already noted if it is to occur)
             */
            w *= 2;

            /* Compute the additive step */
            v = *pt + k;

            /* If we do not already have an overflow carry, check to see
             * if the addition will cause one, and set the carry out if so
             */
            u |= ((MP_WORD_MAX - v) < w);

            /* Add in the rest, again ignoring overflow */
            w += v;

            /* Set the i,j digit of the output */
           *pt = ACCUM(w);

            /* Save carry information for the next iteration of the loop.
             * This is why k must be an mp_word_t, instead of an mp_digit_t */
            k = CARRYOUT(w) | (u << DIGIT_BIT);
        } /* for (jx ...) */

        /* Set the last digit in the cycle and reset the carry */

        k = DIGIT(&tmp, ix + jx) + k;
        pbt[ix + jx] = ACCUM(k);
        k = CARRYOUT(k);

    /* If we are carrying out, propagate the carry to the next digit
     * in the output.  This may cascade, so we have to be somewhat
     * circumspect -- but we will have enough precision in the output
     * that we won't overflow
     */

        kx = 1;

        while (k)
        {
            k = convert(mp_word_t, pbt[ix + jx + kx]) + 1;
            pbt[ix + jx + kx] = ACCUM(k);
            k = CARRYOUT(k);
            ++kx;
        }
    } /* for (ix ...) */

    s_mp_clamp(&tmp);
    s_mp_exch(&tmp, a);

    mp_clear(&tmp);
    return MP_OKAY;
}

/* -------------------------- */
/* Compute a = a / b and b = a mod b.  Assumes b > a.  */

int s_mp_div(mp_int_t *a, mp_int_t *b)
{
    mp_int_t quot, rem, t;
    mp_word_t q;
    int res;
    mp_digit_t d;
    unsigned int ix;

    if (mp_cmp_z(b) == 0)
        return MP_RANGE;

    /* Shortcut if b is power of two */

    if ((ix = s_mp_ispow2(b)) < MP_SIZE_MAX)
    {
        mp_copy(a, b); /* need this for remainder */
        s_mp_div_2d(a, convert(mp_digit_t, ix));
        s_mp_mod_2d(b, convert(mp_digit_t, ix));

        return MP_OKAY;
    }

    /* Allocate space to store the quotient */

    if ((res = mp_init_size(&quot, USED(a))) != MP_OKAY)
        return res;

    /* A working temporary for division */

    if ((res = mp_init_size(&t, USED(a))) != MP_OKAY)
        shortjmp(T);

    /* Allocate space for the remainder */

    if ((res = mp_init_size(&rem, USED(a))) != MP_OKAY)
        shortjmp(REM);

    /* Normalize to optimize guessing */

    d = s_mp_norm(a, b);

    /* Perform the division itself...woo! */

    ix = USED(a) - 1;

    while (ix < MP_SIZE_MAX)
    {
        /* Find a partial substring of a which is at least b */

        while (s_mp_cmp(&rem, b) < 0 && ix < MP_SIZE_MAX)
        {
            if ((res = s_mp_lshd(&rem, 1)) != MP_OKAY)
                shortjmp(CLEANUP);

            if ((res = s_mp_lshd(&quot, 1)) != MP_OKAY)
                shortjmp(CLEANUP);

            DIGIT(&rem, 0) = DIGIT(a, ix);
            s_mp_clamp(&rem);
            --ix;
        }

        /* If we didn't find one, we're finished dividing */

        if (s_mp_cmp(&rem, b) < 0)
            break;

        /* Compute a guess for the next quotient digit */

        q = DIGIT(&rem, USED(&rem) - 1);

        if (q <= DIGIT(b, USED(b) - 1) && USED(&rem) > 1)
            q = (q << DIGIT_BIT) | DIGIT(&rem, USED(&rem) - 2);

        q /= DIGIT(b, USED(b) - 1);

        /* The guess can be as much as RADIX + 1 */

        if (q >= RADIX)
            q = RADIX - 1;

        /* See what that multiplies out to */

        mp_copy(b, &t);

        if ((res = s_mp_mul_d(&t, q)) != MP_OKAY)
            shortjmp(CLEANUP);

        /* If it's too big, back it off.  We should not have to do this
         * more than once, or, in rare cases, twice.  Knuth describes a
         * method by which this could be reduced to a maximum of once, but
         * I didn't implement that here.
         */

        while (s_mp_cmp(&t, &rem) > 0)
        {
            --q;
            s_mp_sub(&t, b);
        }

        /* At this point, q should be the right next digit */

        if ((res = s_mp_sub(&rem, &t)) != MP_OKAY)
            shortjmp(CLEANUP);

        /* Include the digit in the quotient.  We allocated enough memory
         * for any quotient we could ever possibly get, so we should not
         * have to check for failures here
         */
        DIGIT(&quot, 0) = q;
    }

    /* Denormalize remainder */
    if (d != 0)
        s_mp_div_2d(&rem, d);

    s_mp_clamp(&quot);
    s_mp_clamp(&rem);

    /* Copy quotient back to output */

    s_mp_exch(&quot, a);

    /* Copy remainder back to output */

    s_mp_exch(&rem, b);

CLEANUP:
    mp_clear(&rem);

REM:
    mp_clear(&t);
T:
    mp_clear(&quot);

    return res;
}

/* -------------------------- */

int s_mp_2expt(mp_int_t *a, unsigned int k)
{
    int res;
    unsigned int dig, bit;

    dig = k / DIGIT_BIT;
    bit = k % DIGIT_BIT;

    mp_set_zero(a);
    if ((res = s_mp_pad(a, dig + 1)) != MP_OKAY)
        return res;

    DIGIT(a, dig) |= (convert(mp_digit_t, 1) << bit);

    return MP_OKAY;
}

/* -------------------------- */
/* Compute Barrett reduction, x (mod m), given a precomputed value for
 * mu = b^2k / m, where b = RADIX and k = #digits(m).  This should be
 * faster than straight division, when many reductions by the same
 * value of m are required (such as in modular exponentiation).  This
 * can nearly halve the time required to do modular exponentiation,
 * as compared to using the full integer divide to reduce.
 * This algorithm was derived from the _Handbook of Applied
 * Cryptography_ by Menezes, Oorschot and VanStone, Ch. 14,
 * pp. 603-604.
 */
int s_mp_reduce(mp_int_t *x, mp_int_t *m, mp_int_t *mu)
{
    mp_int_t q;
    int res;
    unsigned int um = USED(m);

    if ((res = mp_init_copy(&q, x)) != MP_OKAY)
        return res;

    s_mp_rshd(&q, um - 1); /* q1 = x / b^(k-1) */
    s_mp_mul(&q, mu); /* q2 = q1 * mu */
    s_mp_rshd(&q, um + 1); /* q3 = q2 / b^(k+1) */

    /* x = x mod b^(k+1), quick (no division) */
    s_mp_mod_2d(x, DIGIT_BIT * (um + 1));

    /* q = q * m mod b^(k+1), quick (no division) */
    s_mp_mul(&q, m);
    s_mp_mod_2d(&q, DIGIT_BIT * (um + 1));

    /* x = x - q */
    if ((res = mp_sub(x, &q, x)) != MP_OKAY)
        shortjmp(CLEANUP);

    /* If x < 0, add b^(k+1) to it */
    if (mp_cmp_z(x) < 0)
    {
        mp_set_d(&q, 1);

        if ((res = s_mp_lshd(&q, um + 1)) != MP_OKAY)
            shortjmp(CLEANUP);

        if ((res = mp_add(x, &q, x)) != MP_OKAY)
            shortjmp(CLEANUP);
    }

    /* Back off if it's too big */
    while (mp_cmp(x, m) >= 0)
    {
        if ((res = s_mp_sub(x, m)) != MP_OKAY)
            break;
    }

 CLEANUP:
    mp_clear(&q);

    return res;
}

/* -------------------------- */
/* Compare |a| <=> |b|, return 0 if equal, <0 if a<b, >0 if a>b */

int s_mp_cmp(mp_int_t *a, mp_int_t *b)
{
    unsigned int ua = USED(a), ub = USED(b);

    if(ua == ub)
    {
        unsigned int ix = ua - 1;
        mp_digit_t *ap = DIGITS(a) + ix, *bp = DIGITS(b) + ix;

        for (;; ix--, ap--, bp--)
        {
            if (*ap > *bp)
                return MP_GT;
            else if (*ap < *bp)
                return MP_LT;
            if (ix == 0)
                break;
        }

        return MP_EQ;
    }
    return (ua > ub) ? MP_GT : MP_LT;
}

/* -------------------------- */
/* Compare |a| <=> d, return 0 if equal, <0 if a<d, >0 if a>d */

int s_mp_cmp_d(mp_int_t *a, mp_digit_t d)
{
    unsigned int ua = USED(a);
    mp_digit_t *ap = DIGITS(a);

    if (ua > 1)
        return MP_GT;

    if (*ap < d)
        return MP_LT;
    else if (*ap > d)
        return MP_GT;
    //else
    return MP_EQ;
}

/* -------------------------- */
/* Returns MP_SIZE_MAX if the value is not a power of two; otherwise, it
 * returns k such that v = 2^k, i.e. lg(v).
 */

unsigned int s_mp_ispow2(mp_int_t *v)
{
    mp_digit_t d, *dp;
    unsigned int uv = USED(v);
    unsigned int ix;

    d = DIGIT(v, uv - 1); /* most significant digit of v */

    /* quick test */
    if ((d & (d - 1)) != 0)
        return MP_SIZE_MAX; /* not a power of two */

    if (uv >= 2)
    {
        ix = uv - 2;
        dp = DIGITS(v) + ix;

        for (;; ix--, dp--)
        {
            if (*dp)
                return MP_SIZE_MAX; /* not a power of two */
            if (ix == 0)
                break;
        }
    }
    return ((uv - 1) * DIGIT_BIT) + s_highest_bit(d) - 1;
}

/* -------------------------- */
int s_mp_ispow2d(mp_digit_t d)
{
    /* quick test */
    if ((d & (d - 1)) != 0)
        return -1; /* not a power of two */

    /* If d == 0, s_highest_bit returns 0, thus we return -1. */
    return convert(int, s_highest_bit(d)) - 1;
}

/* -------------------------- */
/* Convert the given character to its digit value, in the given radix.
 * If the given character is not understood in the given radix, -1 is
 * returned.  Otherwise the digit's numeric value is returned.
 * The results will be odd if you use a radix < 2 or > 62, you are
 * expected to know what you're up to.
 */

int s_mp_tovalue(wchar_t ch, int r)
{
    int val, xch;

  /* For bases up to 36, the letters of the alphabet are
     case-insensitive and denote digits valued 10 through 36.
     For bases greater than 36, the lower case letters have
     their own meaning and denote values past 36. */

    if (r <= 36 && ch >= 'a' && ch <= 'z')
        xch = ch - 'a' + 'A';
    else
        xch = ch;

    if (xch >= '0' && xch <= '9')
        val = xch - '0';
    else if (xch >= 'A' && xch <= 'Z')
        val = xch - 'A' + 10;
    else if (xch >= 'a' && xch <= 'z')
        val = xch - 'a' + 36;
    else if (xch == '+')
        val = 62;
    else if (xch == '/')
        val = 63;
    else
        return -1;

    if (val < 0 || val >= r)
        return -1;

    return val;
}

/* -------------------------- */
/* Convert val to a radix-r digit, if possible.  If val is out of range
 * for r, returns zero.  Otherwise, returns an ASCII character denoting
 * the value in the given radix.
 * The results may be odd if you use a radix < 2 or > 64, you are
 * expected to know what you're doing.
 */
char s_mp_todigit(int val, int r, int low)
{
    int ch;

    if (val < 0 || val >= r)
        return 0;

    ch = s_dmap_1[val];

    if (low && val > 9 && r <= 36)
        ch = ch - 'A' + 'a';

    return ch;
}


/* -------------------------- */

wchar_t s_mp_todigitw(int val, int r, int low)
{
    wchar_t ch;

    if (val < 0 || val >= r)
        return 0;

    ch = s_dmap_w[val];

    if (low && val > 9 && r <= 36)
        ch = ch - L'A' + L'a';

    return ch;
}


/* -------------------------- */
/* Return an estimate for how long a string is needed to hold a radix
 * r representation of a number with 'bits' significant bits.
 * Does not include space for a sign or a NUL terminator.
 */

size_t s_mp_outlen(unsigned int bits, int r)
{
    unsigned int units = bits / MP_LOG_SCALE;
    unsigned int rem = bits % MP_LOG_SCALE;
    unsigned int log2 = s_logv_2[r];

    return convert(size_t, units * log2 + (rem * log2 + (MP_LOG_SCALE - 1)) / MP_LOG_SCALE);
}

/* -------------------------- */
/* -------------------------- */

static mp_digit_t bn0  = 0;
static mp_digit_t bn1  = 1;
static mp_digit_t bn2  = 2;
static mp_digit_t bn3  = 3;
static mp_digit_t bn4  = 4;
static mp_digit_t bn5  = 5;
static mp_digit_t bn6  = 6;
static mp_digit_t bn7  = 7;
static mp_digit_t bn8  = 8;
static mp_digit_t bn9  = 9;


static mp_int_t mp0  = {MP_ZPOS, 1, 1, &bn0};
static mp_int_t mp1  = {MP_ZPOS, 1, 1, &bn1};
static mp_int_t mp2  = {MP_ZPOS, 1, 1, &bn2};
static mp_int_t mp3  = {MP_ZPOS, 1, 1, &bn3};
static mp_int_t mp4  = {MP_ZPOS, 1, 1, &bn4};
static mp_int_t mp5  = {MP_ZPOS, 1, 1, &bn5};
static mp_int_t mp6  = {MP_ZPOS, 1, 1, &bn6};
static mp_int_t mp7  = {MP_ZPOS, 1, 1, &bn7};
static mp_int_t mp8  = {MP_ZPOS, 1, 1, &bn8};
static mp_int_t mp9  = {MP_ZPOS, 1, 1, &bn9};


mp_int_t *mp_zero(void)      {return &mp0;}
mp_int_t *mp_one(void)       {return &mp1;}
mp_int_t *mp_two(void)       {return &mp2;}
mp_int_t *mp_three(void)     {return &mp3;}
mp_int_t *mp_four(void)      {return &mp4;}
mp_int_t *mp_five(void)      {return &mp5;}
mp_int_t *mp_six(void)       {return &mp6;}
mp_int_t *mp_seven(void)     {return &mp7;}
mp_int_t *mp_eight(void)     {return &mp8;}
mp_int_t *mp_nine(void)      {return &mp9;}






