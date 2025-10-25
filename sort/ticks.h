/*
    ticks.h

    Functions to manage time and ticks

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

		- A WORD ABOUT TICKS...

            ticks_t represents a number of clock 'ticks'.

            The reason why 'ticks' is enclosed in quotes is
            because they do not represent real clock ticks,
            but our interpretation of how often a clock tick
            happens,  which we assume it happens every tenth
            of a microsecond (100 nanoseconds) in order to
            ease mental calculations and improve code
            portability. Actually 100 nanosecods is nothing
            else than the time unit used by Windows since
            the release of Windows NT 3.5

            Its IMPORTANT to understand that ticks_t is a
            SIGNED type, as they are always relative to some
            epoch like 00:00:00 01/01/1970, 00:00:00 1/1/1601
            or the point on time when the computer booted
            so negative values can represent time prior to
            these epochs.

                              --oO0Oo--
*/

#ifndef TICKS_BY_GAO
#define TICKS_BY_GAO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wctype.h>
#include <stddef.h>

#ifndef FALSE

#define FALSE   0
#define TRUE    1

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

#include <sys/time.h>

#endif

/* --------------------------------- *
   We need a 64 bits unsigned int
 * --------------------------------- */

#ifdef FOR_WIN

typedef __int64    ticks_t;

#else 

typedef long long  ticks_t;

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

/* ------------------ *
   Functions
 * ------------------ */

 #ifdef __cplusplus
extern "C" {
#endif

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

/* ------------------------------------------------------ *
    This function returns the time lapse in ticks_t
    between two measures.

    Optionally returns broken down time intended for 
    making sense of the time lapse, which is why the 
    types are integers, which have limitations 
    in the maximums of the returned values:

        'secs' = 68 years in seconds.
        'msec' = 999 milliseconds.
        'usec' = 999 microseconds.

    AGAIN: it is intended to make sense of time lapses 
    measured by the function cpu_ticks().

    If you are not interested on a value, send NULL 
    for it.
 * ------------------------------------------------------ */

ticks_t diff_ticks(ticks_t before,ticks_t after,int *secs,int *msec,int *usec);

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

/* ------------------------------------------------------ *
    Same as the previous but it returns the elapsed time
    in ticks instead seconds, as well as optional broken 
    down time to save a call to diff_ticks()
* ------------------------------------------------------ */

ticks_t stopwatch_ticks(stopwatch_t *sw,int *secs,int *msec,int *usec);

#ifdef __cplusplus
};
#endif

#endif
