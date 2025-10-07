/*
    test_ticks.c

    Test of timing routines

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

#include "ticks.h"


/* --------------- */

void print_ticks(char *prefix,ticks_t ticks)
{
    char *cad,tmp[300]={0};
    int sign=0;

    if(ticks < 0)
    {
        sign++;
        ticks*=-1;
    }
    cad=tmp + 299;
    while(ticks > 0)
    {
        cad--;
       *cad='0' + (char)(ticks % 10);
        ticks/=10;
    }
    if(sign)
    {
        cad--;
       *cad='-';
    }
    printf("%s=%s\n",prefix,cad);
}

/* -------------------------------------- */

static volatile unsigned int flag = 0;  /* Fake flag to trick the compular to not remove the useless loop */

static int waste_some_time(void)
{
    int counter = 0;

    while(++counter !=  0x7ffffff)
    {
        if(flag)
            break;
    }
    return counter;
}

/* --------------------------- */

void test_ticks(void)
{
    ticks_t ta,tb,t1,t2;
    int secs,msec,usec;
    stopwatch_t sw1,sw2;

    printf("*** Test Ticks ***\n\n");

    stopwatch_start(&sw1);    

    printf("Stop watch started...\n");

    msec = waste_some_time();

    printf("It took %.6f seconds to excute the loop %d times\n\n",stopwatch_elapsed(&sw1),msec);

    ta=cpu_ticks();
    t1=unix_ticks();

    printf("Time has been taken and one of the stop watches started\n\n");

    stopwatch_start(&sw2);    

    printf("Please move the system clock back some time and press enter:\n");
    getchar();

    stopwatch_stop(&sw2);
    printf("You took %.3f seconds to change time and press the key\n\n",stopwatch_elapsed(&sw2));

    tb=cpu_ticks();
    t2=unix_ticks();

    diff_ticks(ta,tb,&secs,&msec,&usec);
    print_ticks("Start CPU  ",ta);
    print_ticks("End CPU    ",tb);
    print_ticks("Difference ",tb);
    printf(     "Elapsed %d secs %d msecs %d usecs\n\n",secs,msec,usec);

    diff_ticks(t1,t2,&secs,&msec,&usec);
    print_ticks("Start UNIX ",ta);
    print_ticks("End UNIX   ",tb);
    print_ticks("Difference ",tb);
    printf(     "Elapsed %d secs %d msecs %d usecs\n\n",secs,msec,usec);

    stopwatch_resume(&sw2);

    printf("Put back the correct time and press enter again:\n");
    getchar();
    
    printf("You took %.3f seconds to change time and press the second key key\n\n",stopwatch_elapsed(&sw2));

    stopwatch_stop(&sw2);

    tb=cpu_ticks();
    t2=unix_ticks();

    diff_ticks(ta,tb,&secs,&msec,&usec);
    print_ticks("Start CPU  ",ta);
    print_ticks("End CPU    ",tb);
    print_ticks("Difference ",tb);
    printf(     "Elapsed %d secs %d msecs %d usecs\n\n",secs,msec,usec);

    diff_ticks(t1,t2,&secs,&msec,&usec);
    print_ticks("Start UNIX ",ta);
    print_ticks("End UNIX   ",tb);
    print_ticks("Difference ",tb);
    printf(     "Elapsed %d secs %d msecs %d usecs\n\n",secs,msec,usec);

    stopwatch_stop(&sw1);

    printf("Total execution time was %.3f seconds (%.3f of them changing time)\n\n",stopwatch_elapsed(&sw1),stopwatch_elapsed(&sw2));


}


void test_time(void)
{
    time_t t1,t2,t3;
    mtime_t mt;
    utime_t ut;
    point_on_time_t pot;
    char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct","Nov","Dic"};
    char *suffix[10] = {"th","st","nd","rd","th","th","th","th","th","th"};

    t1 = time(NULL);
    t2 = current_time();
    t3 = unix_ticks();
    mt = current_mtime();
    ut = current_utime();

    printf("*** Time test ***\n\n");

    printf("unix_ticks()    = %llu \n",(unsigned long long)t3);
    printf("current_time()  = %lu (%lu from time(NULL)\n",(unsigned long)t1,(unsigned long)t2);
    printf("current_mtime() = %llu \n",(unsigned long long)mt);
    printf("current_utime() = %llu \n\n",(unsigned long long)ut);
    
    time_to_pot(t2,&pot,TRUE);
    printf("time_to_pot(%ld,LOCAL) = %s, %s %d%s %d%sat %02d:%02d:%02d.%d-%d\n",(long)t2,day[pot.wday],mon[pot.mon-1],
           pot.day,suffix[pot.day % 10],pot.year,(pot.leap) ? " (leap) " : " ",pot.hour,pot.min,pot.sec,pot.msec,pot.usec);
    
    t2 = time_from_pot(&pot);
    printf("time_from_pot(LOCAL) = %ld\n\n",(long)t2);           

    mtime_to_pot(mt,&pot,TRUE);
    printf("mtime_to_pot(%lld,LOCAL) = %s, %s %d%s %d%sat %02d:%02d:%02d.%d-%d\n",(long long)mt,day[pot.wday],mon[pot.mon-1],
           pot.day,suffix[pot.day % 10],pot.year,(pot.leap) ? " (leap) " : " ",pot.hour,pot.min,pot.sec,pot.msec,pot.usec);
    
    mt = mtime_from_pot(&pot);
    printf("mtime_from_pot(LOCAL) = %lld\n\n",(long long)mt);           

    utime_to_pot(ut,&pot,TRUE);
    printf("utime_to_pot(%lld,LOCAL) = %s, %s %d%s %d%sat %02d:%02d:%02d.%d-%d\n",(long long)mt,day[pot.wday],mon[pot.mon-1],
           pot.day,suffix[pot.day % 10],pot.year,(pot.leap) ? " (leap) " : " ",pot.hour,pot.min,pot.sec,pot.msec,pot.usec);

    ut = utime_from_pot(&pot);
    printf("utime_from_pot(LOCAL) = %llu\n\n",(long long)ut);           


    printf("\n");

}

/* --------------- */

void test_calendar_sheet(int m, int y, int full,int first)
{
    int tmp[CALENDAR_SHEET_ENTRIES], d, f, c;
    char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
    if(first < CAL_START_ON_SUN || first > CAL_START_ON_SAT)
        first = CAL_START_ON_SUN;

    calendar_sheet(tmp, m, y, full,first);

    printf("calendar_sheet(%d,%d) Returned %d of %d\n", m, y, tmp[0], tmp[1]);
    easter_date(tmp[1],&d,&c);
    printf("Easter falls on Sunday %02d/%02d/%d\n\n",d,c,tmp[1]);

    for (d = 0; d < 7; d++)
    {
        printf("%s ", day[first]);
        if(++first > CAL_START_ON_SAT)
            first = CAL_START_ON_SUN;
    }

    printf("\n");

    for (d = 2, f = 0; f < 6; f++)
    {
        for (c = 0; c < 7; c++)
        {
            if (tmp[d] > 0)
                printf("%2d  ", tmp[d]);
            else
                printf("    ");
            d++;
        }
        printf("\n");
    }
    printf("\n");
}

/* --------------- */

void test_calendar(void)
{
    printf("*** Test Calendar ***\n\n");

    printf("---> Today starting on Sunday\n");
    test_calendar_sheet(0, 0, TRUE,CAL_START_ON_SUN);

    printf("---> Today starting on Monday\n");
    test_calendar_sheet(1000, 999999, TRUE,CAL_START_ON_MON);    
    
    printf("---> April 1968 starting on Tuesday (only this month days)\n");
    test_calendar_sheet(4, 1968, FALSE,CAL_START_ON_TUE);

    printf("---> August 2025 starting on Wednesday (only this month days)\n");
    test_calendar_sheet(8, 2025, FALSE,CAL_START_ON_WED);

    printf("---> June 2006 starting on Thursday (only this month days)\n");
    test_calendar_sheet(6, 2006, FALSE,CAL_START_ON_THU);

    printf("---> February 2000 (leap) starting on Friday (only this month days)\n");
    test_calendar_sheet(2, 2000, FALSE,CAL_START_ON_FRI);    
    
    printf("---> April 1601 starting on Saturday (only this month days)\n");
    test_calendar_sheet(4, 1601, FALSE,CAL_START_ON_SAT);
    
    printf("---> April 4601 starting on Sunday\n");
    test_calendar_sheet(4, 4601, FALSE,CAL_START_ON_SUN);    

    printf("---> April 14601 starting on Monday\n");
    test_calendar_sheet(4, 14601, FALSE,CAL_START_ON_MON);    
}



/* -------------------------------------- */

int main(void)
{
    test_ticks();    
    test_time();    
    test_calendar();
    return 0;
}
