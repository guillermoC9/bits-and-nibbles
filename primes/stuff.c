/*
    stuff.c

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

#include "stuff.h"

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


/* ----------------------------- */
/* ----------------------------- */

void *memxor(void *b,int c,size_t len)
{
    if(b)
    {
        size_t t,blk,rem;
        unsigned long *dblk,val;

        memset(&val,c,sizeof(val));
    
        blk = (len / sizeof(unsigned long));
        rem = (len % sizeof(unsigned long));
    
        dblk=(unsigned long *)b;

        for(t = 0;t < blk; t++)
            dblk[t] ^= val;
    
        if (rem) 
        {
            unsigned char ch,*dest;
            

            dest=(unsigned char *)&(dblk[t]);
            ch = (unsigned char)c;
            for(t=0; t< rem; t++)
                dest[t] ^= ch;
        }
    }
    return b;
}

/* -------------------------------------- */

void *mem_xor(void *dst,const void *src,size_t len)
{
    size_t t,blk,rem;
    unsigned long *dblk,*sblk;
    
    blk = (len / sizeof(unsigned long));
    rem = (len % sizeof(unsigned long));

    sblk=(unsigned long *)src;
    dblk=(unsigned long *)dst;

    for(t = 0;t < blk; t++)
        dblk[t] ^= sblk[t];
    
    if (rem) 
    {
        unsigned char *orig=(unsigned char *)&(sblk[t]);
        unsigned char *dest=(unsigned char *)&(dblk[t]);

    	for(t=0; t< rem; t++)
            dest[t] ^= orig[t];
    }
    return dst;
}

/* ----------------------------- */
/* ----------------------------- */

unsigned int get_le32(const void *bufi)
{
	const unsigned char *buf = (const unsigned char *)bufi;

    return (unsigned int)((buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0]);
}

/* ----------------------------- */

void put_le32(void *bufo,unsigned int val)
{
	unsigned char *buf = (unsigned char *)bufo;

    buf[0]=(unsigned char)(val & 0xFF);
    buf[1]=(unsigned char)((val >> 8)& 0xFF);
    buf[2]=(unsigned char)((val >> 16)& 0xFF);
    buf[3]=(unsigned char)(val >> 24);
}

/* ----------------------------- */

unsigned int get_be32(const void *bufi)
{

	const unsigned char *buf = (const unsigned char *)bufi;

    return (unsigned int)((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
}

/* ----------------------------- */

void put_be32(void *bufo,unsigned int val)
{
	unsigned char *buf = (unsigned char *)bufo;

    buf[0]=(unsigned char)(val >> 24);
    buf[1]=(unsigned char)((val >> 16)& 0xFF);
    buf[2]=(unsigned char)((val >> 8) & 0xFF);
    buf[3]=(unsigned char)(val & 0xFF);
}

/* ----------------------------- */

u64_t get_le64(const void *bufi)
{
    int t;
    u64_t val=0;
	const unsigned char *buf = (const unsigned char *)bufi;

    for(t=7;t>-1;t--)
    {
        val<<=8;
        val|=buf[t];
    }
    return val;
}

/* ----------------------------- */

void put_le64(void *bufo,u64_t val)
{
    int t;
   	unsigned char *buf = (unsigned char *)bufo;

    for(t=0;t<8;t++)
    {
        buf[t]=(unsigned char)(val & 0xFF);
        val>>=8;
    }
}

/* ----------------------------- */

u64_t get_be64(const void *bufi)
{
    int t;
    u64_t val=0;
	const unsigned char *buf = (const unsigned char *)bufi;

    for(t=0;t<8;t++)
    {
        val<<=8;
        val|=buf[t];
    }
    return val;
}

/* ----------------------------- */

void put_be64(void *bufo,u64_t val)
{
    int t;

	unsigned char *buf = (unsigned char *)bufo;

    for(t=7;t>-1;t--)
    {
        buf[t]=(unsigned char)(val & 0xFF);
        val>>=8;
    }
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
