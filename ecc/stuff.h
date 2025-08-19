/*
    stuff.h

    Assorted stuff to support the PDF code

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

	  - Originally written by Guillermo Amodeo Ojeda

     
                                --oO0Oo--
*/

#ifndef EXTENSIONS_OF_STRING_H
#define EXTENSIONS_OF_STRING_H

/** Essential headers */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <wchar.h>
#include <time.h>
#include <locale.h>

#ifndef FALSE

#define FALSE   0
#define TRUE    1

#endif


/* --------------------------------- *
   Make sure unsigned int is at 
   least 32 bits long 
 * --------------------------------- */

 #if UINT_MAX <  0xffffffffu
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

#ifdef FOR_WIN

typedef __int64             ticks_t;
typedef unsigned __int64    u64_t;

/* Microsoft C has different names than the rest for useful functions */

#define snprintf        _snprintf
#define strcasecmp      _stricmp
#define strncasecmp     _strnicmp

#else 

typedef long long           ticks_t;
typedef unsigned long long  u64_t;

#endif

typedef ticks_t    mtime_t;    /* Milliseconds since 00:00:00 01/01/1970 */
typedef ticks_t    utime_t;    /* Microseconds since 00:00:00 01/01/1970 */

/* --------------------------------- *
    Macros
 * --------------------------------- */

#define TICKS_PER_USEC      10          /* Number of ticks in a microsecond */
#define TICKS_PER_MSEC      10000       /* Number of ticks in a millisecond */
#define TICKS_PER_SECOND    10000000    /* Number of ticks in a second */
#define USECS_PER_SECOND    1000000     /* Number of microseconds in a second */
#define MSECS_PER_SECOND    1000        /* Number of milliseconds in a second */

/* --------------------------------- *
   Structure of a Stopwatch    
 * --------------------------------- */

typedef struct
{
    int      running;	/* FALSE = stop, TRUE = running */
    ticks_t  start;		/* cpu time it started */
    ticks_t  stop;      /* cpu time if stopped or last measure if running */
} stopwatch_t;

/*
    Repetition of elements to initialize things, eg:

    char string_100[101]={REPEAT_64('x'),REPEAT_32('o'),REPEAT_4('r'),0};

*/

#define REPEAT_2(_a)    _a,_a
#define REPEAT_4(_a)    REPEAT_2(_a),REPEAT_2(_a)
#define REPEAT_8(_a)    REPEAT_4(_a),REPEAT_4(_a)
#define REPEAT_16(_a)   REPEAT_8(_a),REPEAT_8(_a)
#define REPEAT_32(_a)   REPEAT_16(_a),REPEAT_16(_a)
#define REPEAT_64(_a)   REPEAT_32(_a),REPEAT_32(_a)
#define REPEAT_128(_a)  REPEAT_64(_a),REPEAT_64(_a)
#define REPEAT_256(_a)  REPEAT_128(_a),REPEAT_128(_a)

#define REPEAT_127(_a)  REPEAT_64(_a),REPEAT_32(_a),REPEAT_16(_a),REPEAT_8(_a),REPEAT_4(_a),REPEAT_2(_a),_a

/* Functions*/

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Same as memset() but instead of setting each byte 
   to 'c', it XOR, OR or AND c with each of the 'len'
   bytes of 'buf'.
 * -------------------------------------------------- */

void *memxor(void *b,int c,size_t len);

/* -------------------------------------------------- *
   Same as memmove() but instead copy it makes XOR
   of 'len' bytes of 'src' with the same number of bytes
   in 'dst' and stores them in 'dest'.

        'dst' = ('dst' XOR 'src')

    If 'dst' and 'src' point to the same buffer, all bits will become 0.

    return 'dst'.
 * -------------------------------------------------- */

void *mem_xor(void *dst,const void *src,size_t len);

 /* -------------------------------------------------- *
   These functions are used to conert values from/to
   varios sizes an endians.

   The format to recognize the functions are:
   
   get = read, put = write.
   be = Big Endian, le = Little Endian.

   and then the number of bits, so:

    get_be32() will read 4 bytes in big endian 
    format and conver it to a 32 bits value in the 
    endian of the current machine.

    put_be32() will do the opposite, writting a 32
    bit value in the local endian to 4 bytes in
    big endian.
 * -------------------------------------------------- */

/* 32 bits big endian */

unsigned int get_be32(const void *buf);
void         put_be32(void *buf,unsigned int val);

/** 32 bits little endian */

unsigned int get_le32(const void *buf);
void         put_le32(void *buf,unsigned int val);

/** 64 bits little endian */

u64_t get_le64(const void *buf);
void  put_le64(void *buf,u64_t val);

/** 64 bits big endian */

u64_t get_be64(const void *buf);
void  put_be64(void *buf,u64_t val);

/* -------------------------------------------------------- *
    This function returns the tick counter from the CPU.

    As opposed to unix_ticks() or windows_ticks() this 
    function does not return an specific point on time 
    from an epoch, but it returns the CPU's high resolution
    counter of cycles from an undeterminated point on time, 
    scaled to 100-nanosecond.

    Usually this counter reset itself to 0 when the system
    starts and in some cases even when the process starts.

    This is intended for measuring the time lapsed between 
    two points on time because it is not affected by changes 
    on the system clock, so the measurement will always be 
    correct.

    This will illustrate the point:

        then1 = cpu_ticks();
        then2 = cpu_ticks();

    Let's assume system clock is moved 10 seconds backwards
    after 1 second pass.

        now1 = cpu_ticks();
        now2 = cpu_ticks();

        lapse1 = (now1 - then1) / TICKS_PER_SECOND;
        lapse2 = (now2 - then2) / TICKS_PER_SECOND;

    At this point lapse1 would be -9 and lapse2 would
    be 1, which is the correct number of seconds passed.

    That is why unix_ticks() or windows_ticks() should be 
    used to store specific points on time, and cpu_ticks() 
    to measure time lapses.

 * ------------------------------------------------------ */

ticks_t cpu_ticks(void);

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

/* ------------------------------------------------------ *
    This function initializes and start a stopwatch
    (previous results are lost)

    A stopwatch is intended for measuring lapses of
    time, so it uses cpu ticks to not be affected by
    changes on system time.
 * ------------------------------------------------------ */

void stopwatch_start(stopwatch_t *sw);

/* ------------------------------------------------------ *
    This function stops a stopwatch.

    After calling this function the elapsed time will
    not change and correspond to when this function was
    called.
* ------------------------------------------------------ */

void stopwatch_stop(stopwatch_t *sw);

/* ------------------------------------------------------ *
    This function resumes a stopwatch.

    If a stopwatch is stopped, it is started again, but
    discounting the elapsed ticks since it was stopped
    until was started again by this funcion so the 
    stopped time will not be counted as running.

* ------------------------------------------------------ */

void stopwatch_resume(stopwatch_t *sw);

/* ------------------------------------------------------ *
    If the stopwatch is stopped, this function returns
    the elapsed time in seconds since the stopwatch was
    started until it was stopped.

    If the stopwatch is running, it will return the time
    in seconds since the last call to this function or
    the function stopwatch_ticks() but it will not stop 
    the stopwatch.
* ------------------------------------------------------ */

double stopwatch_elapsed(stopwatch_t *sw);

#ifdef __cplusplus
};
#endif



#endif
