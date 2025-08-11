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

/* -------------------------------------- */

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

/* -------------------------------------- */

void *mem_shift(void *buf,int len,int dir)
{
    unsigned char *dest = (unsigned char *)buf;
    int pos,pre;

    if(len > 0)
    {
        if(dir == S9_RSHIFT)
        {
            pos = len - 1;
            pre = pos - 1;

            while(pos > 0)
            {
                dest[pos]  = (dest[pos] >> 1) | (dest[pre--] <<  7);
                pos--;
            }

            dest[0] >>= 1;
        }
        else if(dir == S9_LSHIFT)
        {
            pos = pre = 0;
            while(++pre < len)
            {
                dest[pos]  = (dest[pos] << 1) | (dest[pre] >> 7);
                pos++;
            }
            dest[pos] <<= 1;
        }
    }
    return buf;
}

/* -------------------------------------- */
/* -------------------------------------- */

void gcm_mult(unsigned char *z, const unsigned char *x,const unsigned char *y,size_t blksize)
{
    size_t cur_byte = 0,minus_one;
    unsigned char res[GCM_MAX_BLK_SIZE],tmp[GCM_MAX_BLK_SIZE],cur_mask;
    int bit;

    if (blksize > GCM_MAX_BLK_SIZE)
        blksize = GCM_MAX_BLK_SIZE;

    minus_one = blksize - 1;

    memset(res, 0, blksize);
    memcpy(tmp, y, blksize);

    cur_byte=0;
    cur_mask=0x80;

    while(cur_byte < blksize)
    {
       if(x[cur_byte] & cur_mask)
          mem_xor(res, tmp, blksize);

        bit = (tmp[minus_one] & 0x01);
        mem_shift(tmp,blksize,0);
        if(bit)
            tmp[0] ^= 0xE1;
        cur_mask>>=1;
        if(!cur_mask)
        {
            cur_mask=0x80;
            cur_byte++;
        }
    }
    memcpy(z,res,blksize);
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


/* ------------------------------- */

static char int_to_hex(int val)
{
    static char *hex="0123456789abcdef";

    if(val < 0 || val > 15)
        val = 0;
    return hex[val];
}

/* ------------------------------- */

int hex_to_char(char *dest,size_t max,const void *buf,size_t tam,char sep)
{
    unsigned char *ptr=(unsigned char *)buf;
    size_t i,ns=0;
    int ret=0;

    if(max > 0)
    {
        max--;

        if(sep)
            ns = (tam - 1);

        for(i=0; i < tam && max > 0; i++)
        {
            dest[ret++]=int_to_hex(ptr[i] >> 4);
            if(--max == 0)
                break;
            dest[ret++]=int_to_hex(ptr[i] & 0xf);
            max--;
            if(ns)
            {
                if(max == 0)
                    break;
                dest[ret++]=sep;
                ns--;
                max--;
            }
        }
        dest[ret]=0;
    }
    return ret;
}