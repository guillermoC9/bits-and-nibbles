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

int mem_get_bit(void *b,size_t len,int x)
{
    unsigned char *ptr,val=0,mask;
    size_t bits,pos;

    ptr = (unsigned char *)b;
    if(len > 0)
    {
        mask = 0x01;
        bits = 8 * len--;
        if(x > -1 && x < bits)
        {
            pos = len - (x / 8);
            mask <<= (x % 8);
            val = (ptr[pos] & mask);
        }
    }
    return (val) ? 1 : 0;
}

/* -------------------------------------- */

void mem_set_bit(void *b,size_t len,int x,int v)
{
    unsigned char *ptr,mask;
    size_t bits,pos;

    ptr = (unsigned char *)b;
    if(len > 0)
    {
        mask = 0x01;
        bits = 8 * len--;
        if(x > -1 && x < bits)
        {
            pos = len - (x / 8);
            mask <<= (x % 8);
            if(v)
                ptr[pos] |= mask;
            else
                ptr[pos] &= ~mask;
        }
    }
}

/* -------------------------------------- */

int mem_bits(void *buf,size_t len)
{
    size_t cnt,t;
    unsigned char msk,tmp,*src;

    cnt = len * 8;
    src = (unsigned char *)buf;

    for(t=0;t<len;t++)
    {
        tmp = src[t];
        msk = 0x80;
        while(msk)
        {
            if(tmp & msk)
                return (int)cnt;
            msk>>=1;
            cnt--;
        }
    }
    return 0;
}

/* ------------------------------------------------------ *
   These macros detect if a value is inside the unicode
   space or not
 * ------------------------------------------------------ */

#define is_unicode(_0)		((_0) < 0xd800 || ((_0) > 0xdfff && (_0) < 0x110000))
#define is_not_unicode(_0)	((_0) > 0x10ffff || ((_0) > 0xd7ff && (_0) < 0xe000))

/* ------------------------------------------------------ *
   Detect if a value is a surrogate code (hi o lo)
 * ------------------------------------------------------ */

#define is_surrogate(_0) ((_0) > 0xd7ff && (_0) < 0xe000)

/* ------------------------------- */

size_t utf8len(const unsigned char *ch,size_t max)
{
    size_t ret=0,num=0;

    while(ch[num] && num < max)
    {
	    if(ch[num] < 192)
	        num++;
	    else if(ch[num] < 224)
		    num+=2;
	    else if(ch[num] < 240)
    		num+=3;
	    else if(ch[num] < 248)
		    num+=4;
		else
        	break;
        ret++;
    }
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

		if(is_surrogate(ch))
		{
			/* Possible surrogate code, if the sequence is correct we
			   process it, but if incorrect we put the replacemente
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

/* ------------------------------- */

int ucs4_to_wchar(wchar_t *resp,unsigned int ucs4)
{
	int ret = 0;

	if(ucs4)
	{
		ret++;
		if (is_not_unicode(ucs4))
		{
		   *resp = WCHAR_REPLACEMENT;
			return 1;
		}
#ifdef WCHAR_UTF_16
        /* Handle surrogates */
		if(ucs4 > 0xffff) 
		{
			ucs4 -= 0x10000;
			resp[0]=(wchar_t)(0xd800 | ((ucs4 >> 10) & 0x3ff));
			resp[1]=(wchar_t)(0xdc00 | (ucs4 & 0x3ff));
			return 2;
		}
#endif
	}
   *resp=(wchar_t)ucs4;
	return ret;
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

int utf16_to_ucs4(unsigned int *resp,const unsigned short *utf16,size_t max)
{
	unsigned int ch = 0, tmp;
	int ret = 0;

	ch = utf16[ret];

	if(ch > 0)
	{
		ret++;

		if(is_surrogate(ch))
		{
            /* Possible surrogate code, if the sequence is correct we
			   process it, but if incorrect we put the replacemente
			   character before keep processing.
			*/

			if( max == 1) /* No more chars */
			{
			   *resp = WCHAR_REPLACEMENT;
			}
			else
			{
				/* Check next */

				tmp = utf16[ret];

				if(ch > 0xdbff || tmp < 0xdc00 || tmp > 0xdfff)
				{
			   		*resp = WCHAR_REPLACEMENT;
				}
				else
				{
					ch -= 0xd800;
					ch <<= 10;
					ch |= (tmp - 0xdc00);

					ret++;
				}
			}
		}
	}

   *resp = ch;
	return ret;
}

/* ------------------------------- */

int ucs4_to_utf16(unsigned short *resp,unsigned int ucs4)
{
	int ret = 0;

	if(ucs4)
	{
		ret++;

		if (is_not_unicode(ucs4))
		{
			ucs4 = WCHAR_REPLACEMENT;
		}
		else if(ucs4 > 0xffff)
		{
			ucs4 -= 0x10000;
			resp[0]=(wchar_t)(0xd800 | ((ucs4 >> 10) & 0x3ff));
			resp[1]=(wchar_t)(0xdc00 | (ucs4 & 0x3ff));
			return 2;
		}
	}

   *resp=(unsigned short)ucs4;
	return ret;
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
		if(num > 3 || is_surrogate(ucs4))
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

/* --------------------------------------------------- *
    Reference:

        https://en.wikipedia.org/wiki/Windows-1252
 * --------------------------------------------------- */

wchar_t win1252_to_wchar(char cual)
{
    unsigned char ch = (unsigned char) cual;

    if(ch > 0x7f && ch < 0xa0)
    {
        switch(ch)
        {
            case 0x80: return 0x20ac;  /*  € */
            case 0x81: return WCHAR_REPLACEMENT;
            case 0x82: return 0x201a;  /*  ‚ */
            case 0x83: return 0x0192;  /*  ƒ */
            case 0x84: return 0x201e;  /*  „ */
            case 0x85: return 0x2026;  /*  … */
            case 0x86: return 0x2020;  /*  † */
            case 0x87: return 0x2021;  /*  ‡ */
            case 0x88: return 0x02c6;  /*  ˆ */
            case 0x89: return 0x2030;  /*  ‰ */
            case 0x8a: return 0x0160;  /*  Š */
            case 0x8b: return 0x2039;  /*  ‹ */
            case 0x8c: return 0x0152;  /*  Œ */
            case 0x8d: return WCHAR_REPLACEMENT;
            case 0x8e: return 0x017d;  /*  Ž */
            case 0x8f: return WCHAR_REPLACEMENT;
            case 0x90: return WCHAR_REPLACEMENT;
            case 0x91: return 0x2018;  /*  ‘ */
            case 0x92: return 0x2019;  /*  ’ */
            case 0x93: return 0x201c;  /*  “ */
            case 0x94: return 0x201d;  /*  ” */
            case 0x95: return 0x2022;  /*  • */
            case 0x96: return 0x2013;  /*  – */
            case 0x97: return 0x2014;  /*  — */
            case 0x98: return 0x02dc;  /*  ˜ */
            case 0x99: return 0x2122;  /*  ™ */
            case 0x9a: return 0x0161;  /*  š */
            case 0x9b: return 0x203a;  /*  › */
            case 0x9c: return 0x0153;  /*  œ */
            case 0x9d: return WCHAR_REPLACEMENT;
            case 0x9e: return 0x017e;  /*  ž */
            case 0x9f: return 0x0178;  /*  Ÿ */
            default:
                break;
        };
    }

    return (wchar_t)ch;
}

/* -------------------------------------- */

char wchar_to_win1252(wchar_t cual)
{
    int wc = (int)cual;

    switch(wc)
    {
        case 0x20ac: return 0x80;   /* € */
        case 0x201a: return 0x82;   /* ‚ */
        case 0x0192: return 0x83;   /* ƒ */
        case 0x201e: return 0x84;   /* „ */
        case 0x2026: return 0x85;   /* … */
        case 0x2020: return 0x86;   /* † */
        case 0x2021: return 0x87;   /* ‡ */
        case 0x02c6: return 0x88;   /* ˆ */
        case 0x2030: return 0x89;   /* ‰ */
        case 0x0160: return 0x8a;   /* Š */
        case 0x2039: return 0x8b;   /* ‹ */
        case 0x0152: return 0x8c;   /* Œ */
        case 0x017d: return 0x8e;   /* Ž */
        case 0x2018: return 0x91;   /* ‘ */
        case 0x2019: return 0x92;   /* ’ */
        case 0x201c: return 0x93;   /* “ */
        case 0x201d: return 0x94;   /* ” */
        case 0x2022: return 0x95;   /* • */
        case 0x2013: return 0x96;   /* – */
        case 0x2014: return 0x97;   /* — */
        case 0x02dc: return 0x98;   /* ˜ */
        case 0x2122: return 0x99;   /* ™ */
        case 0x0161: return 0x9a;   /* š */
        case 0x203a: return 0x9b;   /* › */
        case 0x0153: return 0x9c;   /* œ */
        case 0x017e: return 0x9e;   /* ž */
        case 0x0178: return 0x9f;   /* Ÿ */

        case WCHAR_REPLACEMENT:
            return '?';

        default:
            if(wc > 0xff)
                return '?';
            break;
    }
    return (char) (wc & 0xff);
}

/* -------------------------------------- */

size_t utf8_to_utf16(unsigned short *utf16,size_t max,const unsigned char *orig,size_t total)
{
	int num;
	unsigned int ch;
	const unsigned char *utf8=(const unsigned char *)orig;
	size_t ret = 0;

	if(!utf16 || !utf8 || max < 1)
		return 0;

	max--;

	while(ret < max && total > 0)
	{
		num = utf8_to_ucs4(&ch,utf8,total);
		if(num < 1)
			break;
		utf8+=num;
		total-=num;

		if(ch > 0xffff)
		{
			if( max - ret < 2)
				break;

            ch-=0x10000;
            utf16[ret++]=(unsigned short)(0xd800 | ((ch >> 10) & 0x3ff));
            utf16[ret++]=(unsigned short)(0xdc00 | (ch & 0x3ff));
		}
		else
		{
	        utf16[ret++]=(unsigned short)ch;
	    }
    }
    utf16[ret] = 0;
    return ret;
}


/* -------------------------------------- */

size_t utf16_to_utf8(unsigned char *utf8,size_t max,const unsigned short *utf16,size_t total)
{
	int num;
	unsigned int ch;
	size_t ret = 0;
	unsigned char utmp[4];

	if(!utf16 || !utf8 || max < 1)
		return 0;

	max--;

	while(ret < max && total > 0)
	{
		num = utf16_to_ucs4(&ch,utf16,total);
		if(num < 1)
			break;
		utf16+=num;
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

/* -------------------------------------- */

static void *read_whole(FILE *fp,size_t *tam)
{
    void *ret = NULL;

    if(fp)
    {
        size_t pos;

        fseek(fp,0,SEEK_END);
        pos=ftell(fp);
        fseek(fp,0,SEEK_SET);
        ret=calloc(pos+1,1);
        if(ret)
        {
            if(fread(ret,pos,1,fp)==1)
            {
                if(tam)
                   *tam=pos;
            }
            else
            {
                free(ret);
                ret=NULL;
            }
        }
    }
    return ret;
}

/* -------------------------------------- */

void *read_whole_file(const char *file,size_t *tam)
{
    void *ret = NULL;

    if(file)
    {
        FILE *fp=fopen(file,"rb");
        if(fp)
        {
            ret = read_whole(fp,tam);
            fclose(fp);
        }
    }
    return ret;
}

/* -------------------------------------- */

void *read_whole_filew(const wchar_t *file,size_t *tam)
{
    void *ret = NULL;
    char *tmp;
    
    tmp = wcstostr(file,0);
    if(tmp)
    {
        FILE *fp = fopen(tmp,"rb");
        if(fp)
        {
            ret = read_whole(fp,tam);
            fclose(fp);
        }
        free(tmp);
    }
    return ret;
}

/* --------------------------------------- */

#define ZIP_BUFFER   2048

/* --------------------------------------- */

void *do_read_compressed(FILE *fp,size_t *tam,size_t *orig)
{
    void *zbuf=NULL;
    z_stream z={0};
    size_t tamf;
    uInt tmp,quedan;
    char buf[ZIP_BUFFER];

    fseek(fp,0,SEEK_END);
    tamf=ftell(fp);
    fseek(fp,0,SEEK_SET);

    if(tamf < UINT_MAX && deflateInit(&z,Z_DEFAULT_COMPRESSION)==Z_OK)
    {
        quedan = (uInt)tamf;
        zbuf=malloc(tamf);
        if(zbuf)
        {
            if(orig)
               *orig = tamf;

            z.next_out=zbuf;
            z.avail_out=quedan;

            while(quedan > 0)
            {
                tmp = (quedan < ZIP_BUFFER) ? quedan : ZIP_BUFFER;

                if(fread(buf,tmp,1,fp)!=1)
                    break;

                z.next_in=(Bytef *)buf;
                z.avail_in=tmp;

                if(deflate(&z,Z_NO_FLUSH)==Z_STREAM_ERROR || z.avail_out==0)
                    break;

                quedan-=tmp;
            }

            deflate(&z,Z_FINISH);
            deflateEnd(&z);

            if(quedan)
            {
                free(zbuf);
                zbuf=NULL;
            }
            else
            {
                void *buf=realloc(zbuf,z.total_out);

                if(!buf)
                    free(zbuf);
                zbuf=buf;
                if(tam)
                   *tam = (size_t) z.total_out;
            }
        }

    }
    return zbuf;
}

/* -------------------------------------- */

void *read_compressed(const char *file,size_t *tam,size_t *orig)
{
    void *ret = NULL;

    if(file)
    {
        FILE *fp=fopen(file,"rb");
        if(fp)
        {
            ret = do_read_compressed(fp,tam,orig);
            fclose(fp);
        }
    }
    return ret;
}

/* -------------------------------------- */

void *read_compressedw(const wchar_t *file,size_t *tam,size_t *orig)
{
    void *ret = NULL;
    char *tmp;
    
    tmp = wcstostr(file,0);
    if(tmp)
    {
        FILE *fp=fopen(tmp,"rb");
        if(fp)
        {
            ret = do_read_compressed(fp,tam,orig);
            fclose(fp);
        }
        free(tmp);
    }
    return ret;
}

/* -------------------------------------- */

void *zcompress(const void *data,size_t tam,size_t *sret,int level)
{
    void *ztmp,*zbuf = NULL;
    size_t zmax,zlen;
    int ret;


    if(level < 1)
        level = Z_DEFAULT_COMPRESSION;
    else if(level > Z_BEST_COMPRESSION)
        level = Z_BEST_COMPRESSION;

    if(sret)
        *sret = 0;

    if(data && tam > 0)
    {
        z_stream z = {0};

        ret=deflateInit(&z,level);
        if(ret != Z_OK)
        {
            free(zbuf);
            return NULL;
        }
        zmax = (size_t )deflateBound(&z,tam);
        zbuf = (unsigned char *)malloc(zmax);
        if(zbuf == NULL)
        {
            deflateEnd(&z);
            return NULL;
        }

        z.next_in  = (Bytef *)data;
        z.avail_in = (uLongf) tam;
        z.next_out = (Bytef *)zbuf;
        z.avail_out = (uLongf)zmax;

        ret = deflate(&z,Z_FINISH);

        if(ret != Z_STREAM_END)
        {
            free(zbuf);
            return NULL;
        }

        zlen = zmax - (size_t)z.avail_out;

        deflateEnd(&z);

        if(zmax > zlen)
        {
            ztmp = realloc(zbuf,zlen);
            if(ztmp != NULL)
                zbuf = ztmp;
        }
        if(sret)
            *sret = zlen;
        return zbuf;
    }
    return NULL;
}

/* -------------------------------------- */

#define ZIP_TMP_BUFFER  65535

void *zuncompress(const void *data,size_t tam,size_t *ret)
{
    Bytef *outBuf;
    void *zbuf = NULL;
    int error=0;
    z_stream z={0};
    size_t outLen=0;

    if(data == NULL || tam==0 || inflateInit(&z)!=Z_OK)
        return NULL;

    outBuf = (Bytef *)malloc(ZIP_TMP_BUFFER);
    if(outBuf == NULL)
        return NULL;

    zbuf = outBuf;
    outLen = 0;

    z.next_in=(Bytef *)data;
    z.avail_in=(uInt)tam;
    z.next_out=outBuf;
    z.avail_out=(uInt)ZIP_TMP_BUFFER;

    while(!error && z.avail_in)
    {
        switch(inflate(&z,Z_PARTIAL_FLUSH))
        {
            case Z_STREAM_END:
                z.avail_in=0;
                break;
            case Z_OK:
                break;
            //case Z_BUF_ERROR:
            default:
                error++;
                break;
        }

        if(!error)
        {
            uInt outted = (ZIP_TMP_BUFFER - z.avail_out);

            outLen += outted;
            if(z.avail_in > 0)
            {
                outBuf = (Bytef *)realloc(zbuf,outLen + ZIP_TMP_BUFFER);
                if(outBuf == NULL)
                {
                    if(zbuf)
                        free(zbuf);
                    inflateEnd(&z);
                    return NULL;
                }
                zbuf = outBuf;
                outBuf += outLen;
                z.next_out=outBuf;
                z.avail_out=(uInt)ZIP_TMP_BUFFER;
            }
        }
    }
    inflateEnd(&z);

    if(!error)
    {
        void *def = realloc(zbuf,outLen);
        if(def)
        {
            if(ret)
                *ret = (size_t) outLen;
             return def;
        }
    }

    free(zbuf);
    return NULL;
}

/* ----------------------------- */
/* ----------------------------- */

unsigned short int get_le16(const void *bufi)
{
	const unsigned char *buf = (const unsigned char *)bufi;

    return (unsigned short int)((buf[1] << 8) | buf[0]);
}

/* ----------------------------- */

void put_le16(void *bufo,unsigned short int val)
{
	unsigned char *buf = (unsigned char *)bufo;

    buf[0]=(unsigned char)(val & 0xFF);
    buf[1]=(unsigned char)(val >> 8);
}

/* ----------------------------- */

unsigned short int get_be16(const void *bufi)
{
	const unsigned char *buf = (const unsigned char *)bufi;

    return (unsigned short int)((buf[0] << 8) | buf[1]);
}

/* ----------------------------- */

void put_be16(void *bufo,unsigned short int val)
{
	unsigned char *buf = (unsigned char *)bufo;

    buf[0]=(unsigned char)(val >> 8);
    buf[1]=(unsigned char)(val & 0xFF);
}

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

/* ------------------------------- */
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

size_t strlower(char *dest,const char *orig,size_t max)
{
	size_t n = 0;
	int ch;

	if(max > 0)
	{
		while(orig[n] && n<max)
		{
            ch=tolower(orig[n]);
	        dest[n++]=(char)ch;
	    }
	}
	else
	{
    	while(orig[n])
		{
            ch=tolower(orig[n]);
	        dest[n++]=(char)ch;
	    }
	}
    dest[n] = 0;
    return n;
}

/* ------------------------------- */

size_t strupper(char *dest,const char *orig,size_t max)
{
	size_t n = 0;
	int ch;

	if(max > 0)
	{
		while(orig[n] && n<max)
		{
            ch=toupper(orig[n]);
	        dest[n++]=(char)ch;
	    }
	}
	else
	{
    	while(orig[n])
		{
            ch=toupper(orig[n]);
	        dest[n++]=(char)ch;
	    }
	}
	dest[n] = 0;
    return n;
}

/* -------------------------- */

char *realloc_strcat(char *orig,const char *cat,size_t ctam,char sep,size_t *nlen)
{
    size_t len,otam;
    char *ret;

    otam = (orig) ? strlen(orig) : 0;
    len  = otam;

    if(cat)
    {
        if(ctam == 0)
            ctam=strlen(cat);

        len += ctam;
        if(sep)
            len++;
        ret=(char *)realloc(orig,len + 3);
        if(ret)
        {
            orig=ret;
            if(otam)
            {
                ret+=otam;
                if(sep)
                   *ret++=sep;
            }
            strcpy(ret,cat);
        }
    }
    if(nlen)
        *nlen = len;
    return orig;
}

/* ----------------------------- */

wchar_t *wcsallocfv(size_t *size,const wchar_t *fmt,va_list vl)
{
    wchar_t *ret = NULL, *tmp;
    int len = 0;

    if(fmt)
    {
        /* We have to do this because swprintf()
           doesn't work like snprintf() and return
           -1 if the buffer doesn't suffice. Thus
           we cannot calculate what we need.
        */
        tmp = (wchar_t *)malloc(32728 * sizeof(wchar_t));
        if(tmp)
        {
            len = vswprintf(tmp,32728,fmt,vl);
            if(len > 0)
            {
                ret = realloc(tmp,(len + 1) * sizeof(wchar_t));
                if(ret)
                    tmp = NULL;
            }
            if(tmp)
                free(tmp);
        }
    }
    if(size)
        *size = (size_t)len;
    return ret;

}

/* ----------------------------- */


#if defined(FOR_WIN) && !defined(va_copy)
#define va_copy(d,s)    ((d) = (s))
#endif

char *strallocfv(size_t *size,const char *fmt,va_list vl)
{
    char *ret = NULL,tmp[2];
    int len = 0,max;

    if(fmt)
    {
        va_list v2;

        va_copy(v2,vl);
        max = snprintf(tmp,2,fmt,v2);
        va_end(v2);
        if(max > 0)
        {
            max++;
            ret = (char *)malloc(max);
            if(ret)
                len = vsnprintf(ret,max,fmt,vl);
        }
    }
    if(size)
        *size = (size_t)len;
    return ret;
}

/* ----------------------------- */

wchar_t *wcsallocf(size_t *size,const wchar_t *fmt,...)
{
    va_list vl;
    wchar_t *ret = NULL;

    if(fmt)
    {
        va_start(vl,fmt);
        ret=wcsallocfv(size,fmt,vl);
        va_end(vl);
    }
    return ret;
}

/* ----------------------------- */

char *strallocf(size_t *size,const char *fmt,...)
{
    va_list vl;
    char *ret = NULL;

    if(fmt)
    {
        va_start(vl,fmt);
        ret=strallocfv(size,fmt,vl);
        va_end(vl);
    }
    return ret;
}

/* ----------------------------- */

wchar_t *strtowcs(const char *str,size_t len)
{
    wchar_t *ret = NULL,*cur;
    const unsigned char * ptr;
    int cnt;
    unsigned int ch;
    if(str)
    {
        if(len == 0)
            len = strlen(str);
        ret = (wchar_t *)calloc(sizeof(wchar_t),len);
        if(ret)
        {
            ptr = (const unsigned char *)str;
            cur = ret;
            while(*ptr && len > 0)
            {
                cnt = utf8_to_ucs4(&ch,ptr,len);
                if(cnt == 0)
                    break;

                len -= cnt;
                ptr += cnt;

                cur += ucs4_to_wchar(cur,ch);                
            }
        }
    }
    return ret;
}

/* ----------------------------- */

char *wcstostr(const wchar_t *str,size_t len)
{
    char *ret = NULL;
    unsigned char * ptr;
    int cnt;
    unsigned int ch;
    if(str)
    {
        if(len == 0)
            len = wcslen(str);
        len *= 4;
        ret = (char *)calloc(len,1);
        if(ret)
        {
            ptr = (unsigned char *)ret;
            while(*str && len > 0)
            {
                cnt = wchar_to_ucs4(&ch,str,len);
                if(cnt == 0)
                    break;
                str += cnt;
                cnt = ucs4_to_utf8(ptr,ch);                
                len -= cnt;
                ptr +=cnt;
            }
        }
    }
    return ret;
}



#ifndef FOR_MAC

/* ------------------------------- */

size_t strlcpy(char *s1, const char *s2, size_t max)
{
    size_t t=0;

    if(max > 0)
    {
        while(s2[t] && --max > 0)
        {
            s1[t] = s2[t];
            t++;
        }
        s1[t]=0;
    }
    return t;
}

#endif
