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

/* --------------------------------------------------- */

int gettimeofday(struct timeval *tv,struct timezone *tz)
{
    FILETIME ft;
    ULARGE_INTEGER f;
    __int64 usec;

    if(!tv) 
        return -1;

    GetSystemTimeAsFileTime(&ft);

    f.LowPart=ft.dwLowDateTime;
    f.HighPart=ft.dwHighDateTime;
    f.QuadPart/=10;
    f.QuadPart-=0x295e9648864000ULL;

    usec=(__int64)f.QuadPart;

    tv->tv_sec=(time_t)(usec / 1000000);
    tv->tv_usec=(int)(usec % 1000000);

    if(tz)
    {
        TIME_ZONE_INFORMATION tzi;

        if(GetTimeZoneInformation(&tzi)==TIME_ZONE_ID_INVALID)
            tz->tz_minuteswest=0;
        else
            tz->tz_minuteswest=(int)tzi.Bias;
        tz->tz_dsttime=0;
    }
    return 0;
}

#elif defined(FOR_MAC)

#include <sys/sysctl.h>
#include <mach/mach_time.h>

#endif 

/* --------------------------------------------------- */

ticks_t windows_ticks()
{
    ticks_t ret;
#ifdef FOR_WIN
    FILETIME ft;
    LARGE_INTEGER f;

    GetSystemTimeAsFileTime(&ft);

    f.LowPart=ft.dwLowDateTime;
    f.HighPart=ft.dwHighDateTime;
    ret=(ticks_t)f.QuadPart;
#else
    struct timeval tv;

    gettimeofday(&tv,NULL);
    ret=(ticks_t)tv.tv_sec;
    ret*=1000000;
    ret+=tv.tv_usec;
    ret*=10;
    ret+=0x19db1ded53e8000ULL;  /* Difference between 1970 and 1601 */
#endif
    return ret;
}

/* --------------------------------------------------- */

ticks_t unix_ticks(void)
{
    ticks_t ret;
#ifdef FOR_WIN
    FILETIME ft;
    LARGE_INTEGER f;

    GetSystemTimeAsFileTime(&ft);

    f.LowPart=ft.dwLowDateTime;
    f.HighPart=ft.dwHighDateTime;
    f.QuadPart-=0x19db1ded53e8000ULL;   /* Difference between 1970 and 1601 */
    ret=(ticks_t)f.QuadPart;
#else
    struct timeval tv;

    gettimeofday(&tv,NULL);
    ret=(ticks_t)tv.tv_sec;
    ret*=1000000;
    ret+=tv.tv_usec;
    ret*=10;
#endif
    return ret;
}

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

void divide_ticks(ticks_t ticks,int *segs,int *mseg,int *useg)
{
    ticks /= 10;
    if(useg)
        *useg=(int)(ticks % 1000);

    ticks /= 1000;
    if(mseg)
        *mseg=(int)(ticks % 1000);
    
    if(segs)
        *segs=(int)(ticks / 1000);
}

/* --------------------------------------------------- */

ticks_t combine_ticks(int segs,int mseg,int useg)
{
    ticks_t val;

    val=segs;
    val*=1000;
    val+=mseg;
    val*=1000;
    val+=useg;
    val*=10;
    return val;
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

void time_to_pot(time_t ts,point_on_time_t *pot,int local)
{    
    struct tm *rt;
#ifdef FOR_WIN       
    /* 
       Microsoft C for Windows uses local thread
       memory for localtime() and gmtime(), so
       they can safely be called from different
       threads.

    */
    rt=(local) ? localtime(&ts) : gmtime(&ts);    
#else
    struct tm td;
    /* 
        In other systems we just use the reentrant 
        version 
    */
    rt = (local) ? localtime_r(&ts,&td) : gmtime_r(&ts,&td);
#endif    
    if(rt)    
    {        
        pot->local=(local) ? TRUE : FALSE;
        pot->day=rt->tm_mday;
        pot->mon=rt->tm_mon + 1;
        pot->year=rt->tm_year + 1900;
        pot->hour=rt->tm_hour;
        pot->min=rt->tm_min;
        pot->sec=(rt->tm_sec % 60);
        pot->msec=0;
        pot->usec=0;
        pot->wday=rt->tm_wday;        
        pot->leap = ((pot->year % 4 == 0 && pot->year % 100) || pot->year % 400 == 0);
    }
}


/* ----------------------------- */

void mtime_to_pot(mtime_t mt,point_on_time_t *pot,int local)
{
    time_to_pot((time_t)( mt / 1000) ,pot,local);
    pot->msec =(int)(mt % 1000);
}

/* ----------------------------- */

void utime_to_pot(utime_t ut,point_on_time_t *pot,int local)
{
    mtime_to_pot( ut / 1000 ,pot,local);
    pot->usec=(int)(ut % 1000);
}

/* ----------------------------- */

time_t add_to_time(time_t tm,int unit,int howmany)
{    
    point_on_time_t pot;    
    ticks_t total = (ticks_t) tm;

    if(unit < ADD_UNIT_SECS || unit > ADD_UNIT_YEARS)
        return tm;

    switch(unit)
    {
        case ADD_UNIT_SECS:
            break;
        case ADD_UNIT_MINS:
            howmany *= 60;
            break;
        case ADD_UNIT_HOURS:
            howmany *= 3600;
            break;
        case ADD_UNIT_DAYS:
            howmany *= 86400;
            break;
        case ADD_UNIT_WEEKS:
            howmany *= 604800;
            break;
        case ADD_UNIT_MONTHS:
            time_to_pot(tm,&pot,FALSE);
                            
            pot.year += (howmany / 12);
            pot.mon  += (howmany % 12);

            if( pot.mon > 12)
            {
                pot.year++;
                pot.mon -= 12;
            }
            else if (pot.mon < 0)
            {
                pot.year--;
                pot.mon += 12;
            }
            return time_from_pot(&pot);                        
        case ADD_UNIT_YEARS:
            time_to_pot(tm,&pot,FALSE);                
            pot.year += howmany;
            return time_from_pot(&pot);
        default:
            break;
    }

    total += howmany;
    return (time_t) total;
}


/* ----------------------------- */

time_t time_from_pot(point_on_time_t *pot)
{    
    struct tm td;

    if(!pot)
        return 0;

    memset(&td,0,sizeof(td));
    td.tm_mday=(pot->day > 0 && pot->day < 32) ? pot->day : 1;
    td.tm_mon=(pot->mon > 0 && pot->mon < 13) ? (pot->mon  - 1) : 1;
    td.tm_year=(pot->year > 1900 && pot->year < 20001) ? (pot->year - 1900) : 70;
    td.tm_hour=(pot->hour % 24);
    td.tm_min=(pot->min % 60);
    td.tm_sec=(pot->sec % 60);
    
    if(pot->local)
        return mktime(&td);
#ifdef FOR_WIN
    return _mkgmtime(&td);
#else
    return timegm(&td);
#endif
}

/* ----------------------------- */

mtime_t mtime_from_pot(point_on_time_t *pot)
{
    mtime_t ret = time_from_pot(pot);
    
    if(pot)
    {
        ret *= 1000;
        ret += (pot->msec % 1000);
    }
    return ret;
}

/* ----------------------------- */

mtime_t utime_from_pot(point_on_time_t *pot)
{
    utime_t ret = mtime_from_pot(pot);
    
    if(pot)
    {
        ret *= 1000;
        ret += (pot->usec % 1000);
    }
    return ret;
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


/* ----------------------------------------------------------------------- *
     https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
 * ----------------------------------------------------------------------- */

int day_of_week(int d,int m, int y)
{
    static int mt[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    if(y < 10000 && y > 1581)
    {
        if (m < 3 )
            y -= 1;
        return (y + y/4 - y/100 + y/400 + mt[--m] + d) % 7;
    }
    return 0;
}

/* --------------------------------------------------- *
   Determine if 'year' is a leap year 
   (page 111 of K&R 2nd edition) 
 * --------------------------------------------------- */

int is_leap_year(int year)
{
    return ((year % 4 == 0 && year % 100) || year % 400 == 0) ? TRUE : FALSE;
}

/* --------------------------------------------------- *
    https://almanac.oremus.org/easter/computus/

    Number of days to easter after 21 of March 
 * --------------------------------------------------- */

static int easter_days(int year)
{
    int g,c,p,d;

    if(year > 1581 && year < 20001)
    {
        /* 
            Calculate the date of the full moon p 
            from March 21st to April 18th.

            0  = March 21st
            28 = April 18th            
        */     

        g = (year % 19) + 1;
        c = ((year - 1600) / 100) - ( (year - 1600) / 400);
        c -= (((year - 1400) / 100) * 8) / 25;

        p = (3 - (11 * g) + c) % 30;
        if(p < 0)
            p += 30;

        if( p == 29 || (p == 28 && g > 11))
            p--;

        /* Ok, got the full moon, now calculate 
           easter, which is the following Sunday */

        d = (year + (year / 4) - (year / 100) + (year / 400)) % 7;
        if(d < 0)
            d += 7;

        c = (4 - p - d) % 7;
        if( c < 0)
            c += 7;

        p += c;
        return ++p;
    }
    return -1;
}

/* --------------------------------- */

void easter_date(int year,int *day,int *mon)
 {
    int m,d,easter;

    m = 0;
    d = 0;

    if(!year)
    {
        point_on_time_t ts;

        time_to_pot(current_time(),&ts,FALSE);
        year = (int) ts.year;
    }

    easter = easter_days(year);    
    if(easter > -1)
    {
        if(easter < 11)
        {
            m = 3;
            d = easter + 21;
        }
        else
        {
            m = 4;
            d = easter - 10;
        }
    }

    if(day)
        *day = d;

    if(mon)
        *mon = m;
}

/* --------------------------------- */

time_t easter_time(int year)
{
    point_on_time_t ts;
    int day,mon;

    if(year < 1970)
    {
        if(year)
            return 0;
        time_to_pot(current_time(),&ts,FALSE);
        year = (int) ts.year;
    }
    else if(year > 2037)
    {
        if(sizeof(time_t) == 4 || year > 20000)
            return 0;
    }

    easter_date(year,&day,&mon);

    memset(&ts,0,sizeof(ts));
    ts.local = FALSE;
	ts.day   = day;
    ts.mon  = mon;
    ts.year = year;
 
    return time_from_pot(&ts);
}

/* --------------------------------- */

void calendar_sheet(int *resp,int m, int y,int full,int first)
{
    point_on_time_t pot;
    int inicial=0,dm,dd,tmp,t,i;
    int days[12]={31,28,31,30,31,30,31,31,30,31,30,31};

    if(!resp)
        return;
         
    time_to_pot(current_time(),&pot,FALSE);

    if(y < 1582 || y > 9999)
        y = pot.year;

    if(m < 1 || m > 12)
        m = pot.mon;

    if(m==2 && is_leap_year(y))
        dm = 29;
    else
        dm = days[m-1];

    inicial = day_of_week(1,m,y);

    memset(resp,0,sizeof(int) * CALENDAR_SHEET_ENTRIES);

    if(first > 0 && first < 7)
    {
        if(inicial > first)
            inicial -= first;
        else if (inicial == first)
            inicial = 0;
        else
            inicial += (7 - first);        
    }

    resp[0] = m;
    resp[1] = y;

    if(full && inicial > 0)
    {
        tmp = ((m == 1) ? 31 : days[m - 2]) - (inicial - 1);
        for(i=2,t=0;t<inicial;t++)
            resp[i++] = tmp++;
    }

    inicial+=2;

    for (dd = 1; dd <= dm; dd++)
        resp[inicial++] = dd;

    if(full)
    {
        for(dd=1;inicial < CALENDAR_SHEET_ENTRIES;dd++)
            resp[inicial++] = dd;
    }
}
