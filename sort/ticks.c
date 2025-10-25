/*
    ticks.c

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

                                --oO0Oo--

*/

#include "ticks.h"

#ifdef FOR_WIN

#include <windows.h>

#elif defined(FOR_MAC)

#include <sys/sysctl.h>
#include <mach/mach_time.h>

#endif 

/* --------------------------------------------------- */

ticks_t cpu_ticks(void)
{
    ticks_t ret;

#if defined(FOR_WIN)
    double factor = 0.0;
	LARGE_INTEGER cont,freq;
	double tmp;

    QueryPerformanceFrequency(&freq);

    factor  = (double) freq.QuadPart;
    factor /= 10000000;

    QueryPerformanceCounter(&cont);

	tmp = (double) cont.QuadPart;
	tmp/= factor;

    ret = (ticks_t)tmp;

#elif defined(FOR_MAC)

    mach_timebase_info_data_t ti;

    mach_timebase_info(&ti);

    ret = (ticks_t)((mach_absolute_time() / 100) * ti.numer / ti.denom);

#else

    struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	ret = (ticks_t)((ts.tv_sec * TICKS_PER_SECOND) + (ts.tv_nsec / 100));

#endif

    return ret;
}


/* --------------------------------------------------- */

ticks_t diff_ticks(ticks_t before,ticks_t after,int *segs,int *mseg,int *useg)
{
    ticks_t tmp;

    after -= before;
    before=(after / 10);

    tmp=(before / 1000);

    if(useg)
        *useg=(int)(before % 1000);

    if(mseg)
        *mseg=(int)(tmp % 1000);

    if(segs)
        *segs=(int)(tmp / 1000);

    return after;
}

/* --------------------------------------------------- */
/* --------------------------------------------------- */

void stopwatch_start(stopwatch_t *sw)
{
    if(sw)
    {
        sw->running = TRUE;
        sw->start = cpu_ticks();
        sw->stop = sw->start;
    }
}

/* --------------------------------------------------- */

void stopwatch_stop(stopwatch_t *sw)
{
    if(sw && sw->running)
    {
        sw->running = FALSE;
        sw->stop = cpu_ticks();
    }
}


/* --------------------------------------------------- */

void stopwatch_resume(stopwatch_t *sw)
{
    if(sw && !sw->running)
    {
        ticks_t now;

        sw->running = TRUE;

        now = cpu_ticks();

        sw->start += (now - sw->stop);
        sw->stop = now;
    }
}


/* --------------------------------------------------- */

double stopwatch_elapsed(stopwatch_t *sw)
{
    double ret = 0.0;
    if(sw)
    {
        ticks_t start;

        if(sw->running)
        {
            start = sw->stop;
            sw->stop = cpu_ticks();
        }
        else
        {
            start = sw->start;
        }
        ret = (double) (sw->stop - start);
        ret /= TICKS_PER_SECOND;
    }
    return ret;
}

/* --------------------------------------------------- */

ticks_t stopwatch_ticks(stopwatch_t *sw,int *secs,int *msec,int *usec)
{
    if(sw)
    {
        ticks_t start;

        if(sw->running)
        {
            start = sw->stop;
            sw->stop = cpu_ticks();
        }
        else
        {
            start = sw->start;
        }
        return diff_ticks(start,sw->stop,secs,msec,usec);
    }
    return 0;
}
