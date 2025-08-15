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

char *strtrim(char *str)
{
    char *dst=str;
    char *ori=str;

    if(str)
    {
        while(isspace(*ori))
            ori++;

        while(*ori)
            *dst++=*ori++;

        while(dst > str )
        {
            if(!isspace(*(dst-1)))
                break;
            dst--;
        }
       *dst='\0';
    }
    return str;
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

/* ------------------------------- */

int hex_to_wchar(wchar_t *dest,size_t max,const void *buf,size_t tam,char sep)
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
            dest[ret++]=(wchar_t)int_to_hex(ptr[i] >> 4);
            if(--max == 0)
                break;
            dest[ret++]=(wchar_t)int_to_hex(ptr[i] & 0xf);
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


/* ------------------------------- */

static int hex_to_int(wchar_t hex)
{
    switch(hex)
    {
        case L'0':  return 0x00;
        case L'1':  return 0x01;
        case L'2':  return 0x02;
        case L'3':  return 0x03;
        case L'4':  return 0x04;
        case L'5':  return 0x05;
        case L'6':  return 0x06;
        case L'7':  return 0x07;
        case L'8':  return 0x08;
        case L'9':  return 0x09;
        case L'a':
        case L'A':  return 0x0A;
        case L'b':
        case L'B':  return 0x0B;
        case L'c':
        case L'C':  return 0x0C;
        case L'd':
        case L'D':  return 0x0D;
        case L'e':
        case L'E':  return 0x0E;
        case L'f':
        case L'F':  return 0x0F;
    }
    return -1;
}


/* ------------------------------- */

size_t char_to_hex(void *dest,size_t max,const char *buf,size_t tam, char sep)
{
    unsigned char *ptr=(unsigned char *)dest,val=0;
    size_t i,ret=0;
    int cnt=0,end=0;

    if(dest && buf)
    {
        if(tam == 0)
            tam = strlen(buf);

        for(i=0;!end && i<tam && ret < max;i++)
        {
            if(isxdigit(buf[i]))
            {
                val<<=4;
                val|=(unsigned char)hex_to_int((wchar_t)buf[i]);
                cnt++;
            }
            else if(buf[i]==' ' || buf[i]==':' || buf[i]==sep)
            {
                if(cnt == 1)
                    cnt++;
            }
            else
            {
                end++;
            }
            if(cnt > 1)
            {
                ptr[ret++]=val;
                val=0;
                cnt=0;
            }
        }
    }
    return ret;
}

/* ------------------------------- */

size_t wchar_to_hex(void *dest,size_t max,const wchar_t *buf,size_t tam,wchar_t sep)
{
    unsigned char *ptr=(unsigned char *)dest,val=0;
    size_t i,ret=0;
    int cnt=0,end=0;

    if(dest && buf)
    {
        if(tam == 0)
            tam = wcslen(buf);

        for(i=0;!end && i<tam && ret < max;i++)
        {
            if(iswxdigit(buf[i]))
            {
                val<<=4;
                val|=(unsigned char)hex_to_int(buf[i]);
                cnt++;
            }
            else if(buf[i]==L' ' || buf[i]==L':' || buf[i]==sep)
            {
                if(cnt == 1)
                    cnt++;
            }
            else
                end++;
            if(cnt > 1)
            {
                ptr[ret++]=val;
                val=0;
                cnt=0;
            }
        }
    }
    return ret;

}

/* --------------------------------------------------- */
/* --------------------------------------------------- */

void time_to_pot(time_t ts,point_on_time_t *pot,int local)
{
    struct tm *rt,td;
#ifdef FOR_WIN   
    /* 
       Microsoft C for Windows uses local thread
       memory for localtime() and gmtime(), so
       they can safely be called from different
       threads.

    */
    rt=(local) ? localtime(&ts) : gmtime(&ts);    
#else
    /* 
        In other systems we just use the reentrant 
        version 
    */
    rt = (local) ? localtime_r(&ts,&td) : gmtime_r(&ts,&td);
#endif    
    if(rt)    
    {        
        pot->local=(local) ? TRUE : FALSE;
        pot->day=td.tm_mday;
        pot->mon=td.tm_mon + 1;
        pot->year=td.tm_year + 1900;
        pot->hour=td.tm_hour;
        pot->min=td.tm_min;
        pot->sec=(td.tm_sec % 60);
        pot->msec=0;
        pot->usec=0;
        pot->wday=td.tm_wday;        
        pot->leap = ((pot->year % 4 == 0 && pot->year % 100) || pot->year % 400 == 0);
    }
}

/* ----------------------------- */

time_t time_from_pot(point_on_time_t *pot)
{    
    struct tm td;

    if(!pot)
        return 0;

    memset(&td,0,sizeof(td));
    td.tm_mday=(pot->day > 0) ? pot->day : 0;
    td.tm_mon=(pot->mon > 0) ? (pot->mon - 1) : 0;
    td.tm_year=(pot->year > 0) ? (pot->year - 1900) : 0;
    td.tm_hour=(pot->hour% 24);
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

char *time_to_stamp(char *pot,size_t max,time_t t,int local)
{
    point_on_time_t pt;    
        
    if(pot)
    {
        time_to_pot(t,&pt,local);        
        snprintf(pot,max,"%04d%02d%02d%02d%02d%02d",pt.year,pt.mon,pt.day,pt.hour,pt.min,pt.sec);        
    }
    return pot;
}

/* ----------------------------- */

wchar_t *time_to_stampw(wchar_t *pot,size_t max,time_t t,int local)
{
    point_on_time_t pt;    
        
    if(pot)
    {
        time_to_pot(t,&pt,local);                
        swprintf(pot,max,L"%04d%02d%02d%02d%02d%02d",pt.year,pt.mon,pt.day,pt.hour,pt.min,pt.sec);        
    }
    return pot;
}

/* ------------------------------------------------------ *

    This returns the size of an UTF-8
    sequence by analizing the first
    byte.

	The maximum length of any sequence is
	4 bytes, which is the maximum you need
	to encode any unicode value from 0 to
	0x10ffff.

	if the value is over the 4 bytes limit
	(e.g. pre-2003 sequences) it will return 0

* ------------------------------------------------------ */

static int utf8seq(unsigned char ch)
{
	if(ch < 192)
		return 1;
	if(ch < 224)
		return 2;
	if(ch < 240)
		return 3;
	if(ch < 248)
		return 4;
	return 0;
}

/* ------------------------------- */

static unsigned char mask[7]={0x00,0x7f,0x1f,0x0f,0x07,0x03,0x01};

/* ------------------------------- */

int utf8_to_ucs4(unsigned int *resp,const unsigned char *utf8,size_t max)
{
	int t, num;
	unsigned int ucs4 = 0;

	/* Determine how many chars to process */

	num = utf8seq(utf8[0]);
	if(num > max)
	{
	   *resp=0;
		return 0;
	}

	ucs4=(utf8[0] & mask[num]);

	for (t=1;t<num;t++)
	{
		ucs4 <<= 6;
		ucs4 |= (utf8[t] & 0x3F);
	}

	/* Detect wrong sequences and characters */

	if(ucs4 < 0x80)
	{
		if(num > 1)
			ucs4 = WCHAR_REPLACEMENT;

		if(ucs4 == 0)
			num=0;
	}
	else if(ucs4 < 0x800)
	{
		if(num > 2)
			ucs4 = WCHAR_REPLACEMENT;
	}
	else if(ucs4 < 0x10000)
	{
		if(num > 3 || (ucs4 > 0xd7ff && ucs4 < 0xe000)) /* Is it surrogate? */
			ucs4 = WCHAR_REPLACEMENT;
	}
	else if (ucs4 > 0x10ffff)
	{
		ucs4 = WCHAR_REPLACEMENT;
	}

   *resp = ucs4;
    return num;
}

/* ------------------------------- */

int ucs4_to_utf8(unsigned char *resp,unsigned int ucs4)
{
	if(ucs4 > 0x10ffff || (ucs4 > 0xcfff && ucs4 < 0xe000))
	{
		/* WCHAR_REPLACEMENT in UTF-8 */

		resp[0]=0xcf;
		resp[1]=0xbf;
		resp[2]=0xbd;

		return 3;
	}

    if(ucs4 < 0x80)
    {
        resp[0]=(unsigned char)ucs4;
        return 1;
    }

    if(ucs4 < 0x800)
    {
        resp[0]=(unsigned char)(0xc0 | (ucs4 >> 6));
        resp[1]=(unsigned char)(0x80 | (ucs4 & 0x3f));
        return 2;
    }

    if(ucs4 < 0x10000)
    {

        resp[0]=(unsigned char)(0xe0 | ( ucs4 >> 12));
        resp[1]=(unsigned char)(0x80 | ((ucs4 >> 6) & 0x3f));
        resp[2]=(unsigned char)(0x80 | ( ucs4 & 0x3f));

        return 3;
    }

    resp[0]=(unsigned char)(0xf0 | ( ucs4 >> 18));
    resp[1]=(unsigned char)(0x80 | ((ucs4 >> 12) & 0x3f));
    resp[2]=(unsigned char)(0x80 | ((ucs4 >> 6) & 0x3f));
    resp[3]=(unsigned char)(0x80 | ( ucs4 & 0x3f));

    return 4;
}

/* -------------------------------------- */

size_t utf8_to_wchar(wchar_t *wch,size_t max,const void *orig,size_t total)
{
	int num;
	unsigned int ch;
	const unsigned char *utf8=(const unsigned char *)orig;
	size_t ret = 0;


  	if(!wch || !utf8 || max < 1)
		return 0;

	if(total == 0)
	    total =  strlen(orig);

	max--;

	while(total > 0 && ret < max)
	{
		num = utf8_to_ucs4(&ch,utf8,total);
		if(num < 1)
			break;
		utf8+=num;
		total-=num;
#ifdef WCHAR_UTF_16
		if(ch > 0xffff)
		{
			if( max < 2)
				break;
            ch -= 0x10000;
            wch[ret++]=(wchar_t)(0xd800 | ((ch >> 10) & 0x3ff));
            wch[ret++]=(wchar_t)(0xdc00 | (ch & 0x3ff));
            max-=2;
		}
		else
		{
		    wch[ret++]=(wchar_t)ch;
		    max--;
	    }
#else
	    wch[ret++]=(wchar_t)ch;
	    max--;
#endif
    }
    wch[ret] = 0;
    return ret;
}


/* ------------------------------- */

int wchar_to_ucs4(unsigned int *resp,const wchar_t *wch,size_t max)
{
	unsigned int ch = 0, tmp;
	int ret = 0;

	ch = wch[ret];

	if(ch > 0)
	{
		ret++;

        if((ch > 0xd7ff && ch < 0xe000)) /* Is it surrogate? */		
		{
			/* Possible surrogate code, if the sequence is correct we
			   processit, but if incorrect we put the replacemente
			   character and return 1
			*/

			if( max == 1)
			{
			   *resp = WCHAR_REPLACEMENT;
			}
			else
			{
				/* Check next */

				tmp = wch[ret];

				/* If not correct return and the caller can decide */

				if(ch > 0xdbff || tmp < 0xdc00 || tmp > 0xdfff)
				{
				   *resp = WCHAR_REPLACEMENT;
				}
				else
				{
					/* Build char from the surrogates */

					ch-=0xd800;
					ch<<=10;
					ch|= (tmp - 0xdc00);

					ret++;
				}
			}
		}
	}

   *resp = ch;
	return ret;
}

/* -------------------------------------- */

size_t wchar_to_utf8(unsigned char *utf8,size_t max,const wchar_t *wch,size_t total)
{
	int num;
	unsigned int ch;
	size_t ret = 0;
	unsigned char utmp[4];
    
    if(!wch || !utf8 || !max)
		return 0;

    if(total == 0)
        total = wcslen(wch);

	max--;

	while(ret < max && total > 0)
	{
		num = wchar_to_ucs4(&ch,wch,total);
		if(num < 1)
			break;

		wch+=num;
		total-=num;

		num = ucs4_to_utf8(utmp,ch);
		if (ret + num > max)
			break;
		memcpy(utf8,utmp,num);
		utf8+=num;
		ret+=num;
    }
   *utf8 = 0;
    return ret;
}
