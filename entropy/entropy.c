/*
    entropy.c

    Entropy generation routines

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

        - Designed and written by Guillermo Amodeo Ojeda.                  
            
                        --oO0Oo--
*/

/* Cabeceras */

#include "entropy.h"

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

typedef unsigned __int64    u64_t;

#else 

typedef unsigned long long  u64_t;

#endif

/* --------------------------------- *
   Include specific headers for each
   operating system.
 * --------------------------------- */

#ifdef FOR_WIN

#include <windows.h>

#else 

#include <sys/time.h>

#ifdef FOR_MAC

#include <mach/mach_time.h>

#endif

#endif

/* ------------------------------------------------------------- *
    Return the high resolution tick counter of the CPU.

    In linux/unix it has a 10 microsecond resolution, but is 
    enough for its use here.
 * ------------------------------------------------------------- */

static u64_t cpu_ticks(void)
{
    u64_t ret;

#if defined(FOR_WIN)
	LARGE_INTEGER cont;

    QueryPerformanceCounter(&cont);
	ret = (u64_t) cont.QuadPart;

#elif defined(FOR_MAC)

    ret = (u64_t)mach_absolute_time();

#else

    struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	ret = (u64_t)ts.tv_sec;
	ret*= 1000000000;
	ret+= ts.tv_nsec;

#endif

    return ret;
}

/* -------------------------------------------- */

unsigned int unpredictable_seed(void)
{
    unsigned int seed,tam,t;
    u64_t a[5];
    static u64_t sclk = 0;
    char *b;

    /*
        Get the current count of cpu time 
        using a system-dependant call so any 
        try to reproduce this would be really 
        difficult.
    */

    a[0]=cpu_ticks();

     /*
        Now we allocate up to 1023 bytes of
        memory in order to hash the memory
        address and its content more or
        less quickly using DJB. The way in
        which we do it should facilitate
        for the function to take different 
        execution time and thus affect the 
        values returned by cpu_ticks(), as 
        well as to give different addresses 
        and content if somethign happens 
        between calls to this function.
    */

    tam = (unsigned int)(a[0] & 0xff) | 0x300;
    b = (char *)malloc(tam);
    if(b)
    {
        a[1] = 5381;
        for(t = 0;t < tam ; t++)
        {
            a[1] *= 33;
            a[1] += b[t];
        }
        a[1]<<=32;
        a[1] |= (u64_t)((size_t)b);
        a[1] |= tam;
        free(b);        
    }
    else
    {
        a[1] = ~a[0];
    }

    /*  
        We initialize the sequence number
        (the counter) if this is the 
        first call. 

        We do it here to make sure that
        time is a bit more disturbed than 
        if this was done at the start of 
        the function.
    */

    if(!sclk)
        sclk = cpu_ticks();
    
    /*
        Feed the array with an amalgamation
        of everything so we have some more
        data to feed to the DJB hash.
    */

    a[2]=~sclk;
    a[3]=++sclk;
    a[4]=(a[0] ^ a[1] ^ a[2] ^ a[3]);

    /* 
        Hash everything using DJB in order
        to build the final seed.
    */

    b = (char *)a;

    for(seed=5381,t=0;t < sizeof(a);t++)
        seed = (seed << 5) + seed + b[t];

    return seed;
}
   
/* --------------------------------- *
   This function is written to make
   sure the compiler doesn't optimize
   the loop so it will run 1 to 64 
   times. 
   Which why we check if 'ret' is 0
   when we know this will happen 
   either never or almost never. 
   Although the compiler doesn't 
   know that... ;-)
  * ------------------------------- */

unsigned int unpredictable_seed_non_linear(void)
{
    volatile unsigned int num,ret;

    ret = unpredictable_seed();

    num = 1 + (ret % 64);

    do {
        ret = unpredictable_seed();
        if(ret)
            num--;
    } while (num > 0);
    return ret;
}

/* ------------------------------- */

void get_entropy(void *dest,size_t num)
{
    unsigned char *buf;
    size_t cnt = 0;
    unsigned int left = 0;

    buf = (unsigned char *)dest;
    while(cnt < num)
    {
        left >>= 8;
        if(left == 0)
            left = unpredictable_seed();
        buf[cnt++] = (unsigned char)(left & 0xff);
    }
}

/* -------------------------------- */

void get_entropy_non_linear(void *dest,size_t num)
{
    unsigned char *buf;
    size_t cnt = 0;
    unsigned int left = 0;

    buf = (unsigned char *)dest;
    while(cnt < num)
    {
        left >>= 8;
        if(left == 0)
            left = unpredictable_seed_non_linear();
        buf[cnt++] = (unsigned char)(left & 0xff);
    }
}

/* -------------------------------- */




