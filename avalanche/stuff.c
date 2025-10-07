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
