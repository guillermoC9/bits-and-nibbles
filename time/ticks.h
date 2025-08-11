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

/* For add_to_time() */

enum
{
    ADD_UNIT_SECS=0,
    ADD_UNIT_MINS,
    ADD_UNIT_HOURS,
    ADD_UNIT_DAYS,
    ADD_UNIT_WEEKS,
    ADD_UNIT_MONTHS,
    ADD_UNIT_YEARS,
};

#define CALENDAR_SHEET_ENTRIES  (7 * 6 + 2)     /* Elements of the array of calendar sheets */

/* First day of the calendar sheet */

enum 
{
    CAL_START_ON_SUN = 0,  /* Sunday */
    CAL_START_ON_MON,      /* Monday */
    CAL_START_ON_TUE,      /* Tuesday */
    CAL_START_ON_WED,      /* Wednesday */
    CAL_START_ON_THU,      /* Thursday */
    CAL_START_ON_FRI,      /* Friday */
    CAL_START_ON_SAT,      /* Saturday */
};

/* --------------------------------- *
   Structure of a Stopwatch    
 * --------------------------------- */

typedef struct
{
    int      running;	/* FALSE = stop, TRUE = running */
    ticks_t  start;		/* cpu time it started */
    ticks_t  stop;      /* cpu time if stopped or last measure if running */
} stopwatch_t;

/* --------------------------------- *
    Broken down Point on time    
 * --------------------------------- */

typedef struct
{
    int local;  /* TRUE/FALSE time is local time */

    int  year;  /* Full year: 1946, 2006, 2025, etc.. */    
    int  mon;   /* Month: 1 - 12 */
    int  day;   /* Day: 1 - 31 */
    int  hour;  /* Hour: 0 - 23 */
    int  min;   /* Minute: 0 - 59 */
    int  sec;   /* Second: 0 - 59 */
    int  msec;  /* Millisecond: 0 - 999  */
    int  usec;  /* Microsecond: 0 - 999 */    

    int  wday;  /* Day of the week: 0 - 6 (where 0 is Sunday) */
    int  leap;  /* TRUE/FALSE year was a leap year */
} point_on_time_t;

/* ------------------ *
   Functions
 * ------------------ */

 #ifdef __cplusplus
extern "C" {
#endif

#ifdef FOR_WIN

/* ------------------------------------------------------ *
   Windows doesn't have gettimeofday(), so we build one 
   for it to simplify things when writing portable code
 * ------------------------------------------------------ */

struct timezone
{
    int  tz_minuteswest; /* Minutes West of Greenwich */
    int  tz_dsttime;     /* obsolete, added for compatibility */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif

/* -------------------------------------------------------- *
   Return number of seconds, milliseconds and microseconds
   since 00:00:00 1/1/1970 respectively.
    
   Note that current_time() returns the same as time(), so
   you can use the one you please. We provide current_time()
   for code consistency, so all timing functions use the 
   same mechanism.   
 * -------------------------------------------------------- */

#define current_time()    ((time_t)(unix_ticks() / TICKS_PER_SECOND))
#define current_mtime()   (unix_ticks() / TICKS_PER_MSEC)
#define current_utime()   (unix_ticks() / TICKS_PER_USEC)

/* ------------------------------------------------------ *
    This function returns the number of ticks (100 
    nanosecond units) since 00:00:00 1/1/1970, which
    is what Unix like systems know as epoch.

    The resolution of this value depends on the operating
    system. In Windows it should be 100-nanosecond, but on
    other systems is the number of microseconds multiplied
    by 10, so time resolution is microseconds instead of
    100-nanoseconds in these systems.
 * ------------------------------------------------------ */

ticks_t unix_ticks(void);

/* ------------------------------------------------------ *
    This function returns the number of ticks
    (100-nanosecond) since 00:00:00 1/1/1601, which
    is what Windows knows as epoch.

    The resolution of this value depends on the operating
    system. In Windows it should be 100-nanosecond, but on
    other systems is the number of microseconds multiplied
    by 10, so resolution is microseconds instead of
    100-nanoseconds in these systems.
 * ------------------------------------------------------ */

ticks_t windows_ticks(void);

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
    This function separate ticks_t in seconds, 
    milliseconds and microseconds

    This function is intended for making sense of time
    lapses, not points on time; which is why the types
    are integers, and this have IMPORTANT limitations 
    in the maximums of thew returned values:

        'secs' = 68 years in seconds.
        'msec' = 999 milliseconds.
        'usec' = 999 microseconds.

    AGAIN: it is intended to make sense of time lapses 
    measured by the function cpu_ticks(), rather than 
    unix_ticks() or windows_ticks().

    If you are not interested on a value, send NULL 
    for it.
 * ------------------------------------------------------ */

void divide_ticks(ticks_t ticks,int *secs,int *msec,int *usec);

/* ------------------------------------------------------ *
    This function does the opposite of divide_ticks()
    so you can easily build time lapses in ticks.

    Example of loop that waits 10 seconds, 10 milliseconds
    and 10 microseconds for a condition to happen.
        
        ticks_t then,now;

        then  = now = cpu_ticks();
        then += combine_ticks(10,10,10);

        while(now < then && ...)
        {
            ...            
            now = cpu_ticks();
        } 
 * ------------------------------------------------------ */

ticks_t combine_ticks(int secs,int msec,int usec);

/* ------------------------------------------------------ *
    This function returns the time lapse in ticks_t
    between two measures.

    Optionally returns broken down time to save a call 
    to divide_ticks()
 * ------------------------------------------------------ */

ticks_t diff_ticks(ticks_t before,ticks_t after,int *secs,int *msec,int *usec);

/* ------------------------------------------------------ *
   Divide a time_t onto a struct point_on_time_t 'pot'.

    Send 'local' as TRUE to get the time expressed as 
    local time instead of system time.

    It is basically an all-in-one thread-safe version of
    gmtime() and localtime().

    point_on_time_t has some advantages over struct tm
    returned by both gmtime() and localtime():
        
        - Year is fully specified (no calculations needed)
        - Quantities are expressed in human terms (e.g. the
          month goes from 1 to 12 instead from 0 to 11).
        - Can include milliseconds.
        - Can include microseconds.        
        - Includes a field to tell you if time it 
          containss is local or system time.
        - Includes a field to tell if the year was a leap
          year.
        
    Note that this call always return 0 in msec and usec
    because it converts from seconds. Althouh they will 
    be populated when calling mtime_to_pot(), and 
    utime_to_pot().   
 * ------------------------------------------------------ */

void time_to_pot(time_t ts,point_on_time_t *pot,int local);

/* ------------------------------------------------------ *
   Same as previous but from milliseconds so msec would
   be populated
 * ------------------------------------------------------ */

void mtime_to_pot(mtime_t ts,point_on_time_t *pot,int local);

/* ------------------------------------------------------ *
   Same as previous but from miccroseconds so usec would
   be populated
 * ------------------------------------------------------ */

void utime_to_pot(utime_t ts,point_on_time_t *pot,int local);

/* ------------------------------------------------------ *

    Increment or decrement a time_t.

    'tm' is the time to increase/decrease.

    'unit' is what to add to 'tm':
        
        ADD_UNIT_SECS
        ADD_UNIT_MINS
        ADD_UNIT_HOURS
        ADD_UNIT_DAYS
        ADD_UNIT_WEEKS
        ADD_UNIT_MONTHS
        ADD_UNIT_YEARS

    'howmany' is how many units (weeks, days, etc..) to
              increase or decrease if negative. ;-)
 * ------------------------------------------------------ */

time_t add_to_time(time_t tm,int units,int howmany);

/* ------------------------------------------------------ *
   Builds time_t, mtime_t and utime_t from a point on 
   time.
 
   'local' field indicates if the time is expressed in
   local or system time, but the returned time is 
   ALWAYS system time (UTC).

   Field 'wday' is ignored, as 'leap' is also ignored.   

   'msec' and 'usec' would be ignored depending of which
   function you call.
 * ------------------------------------------------------ */

time_t  time_from_pot(point_on_time_t *pot);
mtime_t mtime_from_pot(point_on_time_t *pot);
utime_t utime_from_pot(point_on_time_t *pot);

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


/* ------------------------------------------------------ *
    Returns the day of the week for a given date like
    day, month and year.

    It uses the Greogrian Calendar, so it will return
    0 if a year before 1582 or after 9999 is sent.

    Note that the Gregorian calendar was not imposed in
    UK until 1752 so any date before that will return
    the wrong value for the UK.
 * ------------------------------------------------------ */

int  day_of_week(int day,int mon,int year);

/* ------------------------------------------------------ *
   Determine if the given year is a leap year
 * ------------------------------------------------------ */

int is_leap_year(int year);

/* ------------------------------------------------------ *
    Returns the day and month of easter on a given year
    using the Greogrian Calendar, thus it will return
    0 in the values if a year before 1582 is used.

    Sending a year over 20000 will have the same effect.

    If year is given as 0, it returns the month and
    day of aster for the current year.

 * ------------------------------------------------------ */

void easter_date(int year,int *day,int *month);

/* ------------------------------------------------------ *
    Returns the time stamp of easter on a given year
    from 1970 to 2037 if time_t is 32 bits or from
    1970 to 20000 if time_t is bigger.

    Sending values outside these ranges will return
    0 (1st of January 1970), which was not easter but
    it let's the caller know the call failed.

    If year is 0, it returns the stamp for easter in the 
    current year.
 * ------------------------------------------------------ */

time_t easter_time(int year);

/* ------------------------------------------------------ *
    Returns a sheet of the Gregorian calendar in the form
    of an array of 44 integers:

        [0] = Month of the calendar sheet
        [1] = Year of the calendar sheet

    Rest of the array is 6 rows x 7 columns in which
    each row represents a week starting from the day 
    of the week chosen by the parameter 'first'.
    
    The array is 6 x 7 because there are months which 
    expand for 6 weeks. I am not lying, popular believe 
    is that month spans 4 weeks, but a month can REALLY
    span over the space of 6 weeks, like November 2020, 
    which starts on the Sunday of the 1st week, and ends 
    on the Monday of the last.

    Parameters:

        'resp'  = array of integers, assumed to be able
                  to hold at least 44 entries (set as
                  CALENDAR_SHEET_ENTRIES).

        'month' = Month of the sheet to be returned, if
                  out of range 1-12, the current month 
                  will be used.

        'year'  = Year of the sheet to be returned,
                  if out of the range 1582-20000 the 
                  current year would be the used.

        'full'  = If TRUE, the sheet will be complete.
                  That is containing the days from the
                  previous month at the start, and the
                  days of next month at the end.

                  If FALSE is sent, then the sheet will
                  contain 0 at the start and the end of
                  the array and the days of the month 
                  would populate the start, middle or 
                  end of the array. 

        'first' = First day of the week for the sheet,
                  where 0 is a Sunday, and 6 is Saturday. 
                  Any numer outside that range will make 
                  the sheet to start on a Sunday.

                  We provide macros to make this param
                  more readable: CAL_START_ON_SUN to say 
                  Sunday, up to CAL_START_ON_SAT to say
                  Saturday.

    This function is for programs wanting to draw their own 
    calendars or handling a per day stuff (e.g. a Scheduler).
    
 * ------------------------------------------------------ */

void calendar_sheet(int *resp,int month, int year,int full,int first);


#ifdef __cplusplus
};
#endif

#endif
