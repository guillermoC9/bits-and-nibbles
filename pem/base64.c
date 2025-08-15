/*
    base64.c

    Base64 encoding/decoding

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
#include "base64.h"

/* -------------------- */


static char *chr_base64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char *chr_safe64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

static int val_base64[256]=
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


/* -------------------- */

size_t bytes_to_base64(char *dest,const void *origin,size_t num,int safe)
{
    unsigned char *buf=(unsigned char *)origin;
    size_t tam=0,tmp = 0;
    char *codigos_base64 = (safe) ? chr_safe64 : chr_base64;

    while(tam < num)
    {
        switch(tam)
        {
            case 0:
                dest[0]=codigos_base64[(buf[tam] >> 2)];
                tmp = (buf[tam++] & 0x03) << 4;
                break;
            case 1:
                tmp |= (buf[tam] >> 4);
                dest[1]=codigos_base64[tmp];
                tmp = (buf[tam++] & 0x0F) << 2;
                break;
            case 2:
                tmp |= (buf[tam] >> 6);
                dest[2]=codigos_base64[tmp];
                dest[3]=codigos_base64[(buf[tam] & 0x3F)];
                return 3;
        }
    }

    if(tam == 2)
    {
        dest[2]=codigos_base64[tmp];
        dest[3]='=';
    }
    else if(tam==1)
    {
        dest[1]=codigos_base64[tmp];
        dest[2]='=';
        dest[3]='=';
    }
    return tam;
}

/* -------------------- */

size_t base64_to_bytes(void *dest,const char *origin)
{
    int val,cnt;
    size_t tam = 0;
    unsigned char *buf=(unsigned char *)dest,tmp[4];

    for(cnt=0;cnt<4;cnt++)
    {
        val=val_base64[(int)origin[cnt]];
        if(val==-1)
            break;
        tmp[cnt]=(unsigned char)(val & 0xff);
    }

    switch(cnt)
    {
        case 4:
            buf[2]=(unsigned char)((tmp[2] << 6) | tmp[3]);
            tam++;
        case 3:
            buf[1]=(unsigned char)((tmp[1] << 4) | (tmp[2] >> 2));
            tam++;
        case 2:
            buf[0]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
            tam++;
        default:
            break;
    }
    return tam;
}

/* -------------------- */


size_t base64_encode(char *dest,size_t max,const void *origin,size_t tam,int safe)
{
    size_t num = 0 , cnt = 0;
    unsigned char *buf=(unsigned char *)origin;
    int blk=0,tmp = 0;
    char *codigos_base64 = (safe) ? chr_safe64 : chr_base64;

    if(max==0)
        return 0;

    while( num < tam && (max - cnt) > 4 )
    {
        for(blk=0; num < tam && blk < 3; blk++)
        {
            switch(blk)
            {
                case 0:
                    dest[0]=codigos_base64[(buf[num] >> 2)];
                    tmp = (buf[num++] & 0x03) << 4;
                    break;
                case 1:
                    tmp |= (buf[num] >> 4);
                    dest[1]=codigos_base64[tmp];
                    tmp = (buf[num++] & 0x0F) << 2;
                    break;
                case 2:
                    tmp |= (buf[num] >> 6);
                    dest[2]=codigos_base64[tmp];
                    dest[3]=codigos_base64[(buf[num++] & 0x3F)];
                    break;
            }
        }
        if(blk == 2)
        {
            dest[2]=codigos_base64[tmp];
            dest[3]='=';
        }
        else if(blk==1)
        {
            dest[1]=codigos_base64[tmp];
            dest[2]='=';
            dest[3]='=';
        }
        dest+=4;
        cnt+=4;
    }
   *dest='\0';
    return cnt;
}

/* -------------------- */

size_t base64_decode(void *dest,size_t max,const char *origin,size_t tam,int ignore)
{
    int valor,cnt;
    unsigned char tmp[4];
    size_t num;
    unsigned char *buf=(unsigned char *)dest;

    if(!tam)
        tam=strlen(origin);
    num=0;
    cnt=0;
    while(*origin && tam > 0  && max > 2)
    {
        valor=val_base64[(int)*origin++];
        if(valor==-1)
        {
            if(!ignore)
                break;
        }
        else
        {
            tmp[cnt++]=(unsigned char)valor;
            if(cnt==4)
            {
                buf[num++]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
                buf[num++]=(unsigned char)((tmp[1] << 4) | (tmp[2] >> 2));
                buf[num++]=(unsigned char)((tmp[2] << 6) | tmp[3]);
                max-=3;
                cnt=0;
            }
        }
        tam--;
    }
    switch(cnt)
    {
        case 3:
            if(max > 1)
            {
                buf[num++]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
                buf[num++]=(unsigned char)((tmp[1] << 4) | (tmp[2] >> 2));
            }
            break;
        case 2:
            if(max > 0)
            {
                buf[num++]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
            }
            break;
        default:
            break;
    }
    return num;
}

/* -------------------- */


size_t base64_encodew(wchar_t *dest,size_t max,const void *origin,size_t tam,int safe)
{
    size_t num = 0 , cnt = 0;
    unsigned char *buf=(unsigned char *)origin;
    int blk=0,tmp = 0;
    char *codigos_base64 = (safe) ? chr_safe64 : chr_base64;

    if(max==0)
        return 0;

    while( num < tam && (max - cnt) > 4 )
    {
        for(blk=0; num < tam && blk < 3; blk++)
        {
            switch(blk)
            {
                case 0:
                    dest[0]=(wchar_t)codigos_base64[(buf[num] >> 2)];
                    tmp = (buf[num++] & 0x03) << 4;
                    break;
                case 1:
                    tmp |= (buf[num] >> 4);
                    dest[1]=(wchar_t)codigos_base64[tmp];
                    tmp = (buf[num++] & 0x0F) << 2;
                    break;
                case 2:
                    tmp |= (buf[num] >> 6);
                    dest[2]=(wchar_t)codigos_base64[tmp];
                    dest[3]=(wchar_t)codigos_base64[(buf[num++] & 0x3F)];
                    break;
            }
        }
        if(blk == 2)
        {
            dest[2]=(wchar_t)codigos_base64[tmp];
            dest[3]=L'=';
        }
        else if(blk==1)
        {
            dest[1]=(wchar_t)codigos_base64[tmp];
            dest[2]=L'=';
            dest[3]=L'=';
        }
        dest+=4;
        cnt+=4;
    }
   *dest='\0';
    return cnt;
}

/* -------------------- */

size_t base64_decodew(void *dest,size_t max,const wchar_t *origin,size_t tam,int ignore)
{
    int valor,cnt;
    unsigned char tmp[4];
    size_t num;
    unsigned char *buf=(unsigned char *)dest;
    wint_t ch;

    if(!tam)
        tam=wcslen(origin);
    num=0;
    cnt=0;
    while(*origin && tam > 0  && max > 2)
    {
        ch = *origin++;
        valor=(ch < 0 || ch > 127) ? -1 : val_base64[ch];
        if(valor==-1)
        {
            if(!ignore)
                break;
        }
        else
        {
            tmp[cnt++]=(unsigned char)valor;
            if(cnt==4)
            {
                buf[num++]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
                buf[num++]=(unsigned char)((tmp[1] << 4) | (tmp[2] >> 2));
                buf[num++]=(unsigned char)((tmp[2] << 6) | tmp[3]);
                max-=3;
                cnt=0;
            }
        }
        tam--;
    }
    switch(cnt)
    {
        case 3:
            if(max > 1)
            {
                buf[num++]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
                buf[num++]=(unsigned char)((tmp[1] << 4) | (tmp[2] >> 2));
            }
            break;
        case 2:
            if(max > 0)
            {
                buf[num++]=(unsigned char)((tmp[0] << 2) | (tmp[1] >> 4));
            }
            break;
        default:
            break;
    }
    return num;
}

/* -------------------- */

int base64_save(FILE *fp,size_t width,const void *buffer,size_t tam,int safe)
{
    int pos=0,num;
    unsigned char *buf=(unsigned char *)buffer;
    char tmp[4];

    if(!fp)
        return -1;

    if(width < tam)
    {
        if(width == 0)
            width = tam;
        else
            width = (((width + 3) / 4) * 4);
    }

    while(tam > 0)
    {
        num=bytes_to_base64(tmp,buf,tam,safe);
        fprintf(fp,"%c%c%c%c",tmp[0],tmp[1],tmp[2],tmp[3]);
        if(ferror(fp))
            return -1;
        tam-=num;
        buf+=num;
        pos += 4;
        if(pos == width)
        {
            fprintf(fp,"\n");
            if(ferror(fp))
                return -1;
            pos=0;
        }

    }
    if(pos > 0 && width > 0)
    {
        fprintf(fp,"\n");
        if(ferror(fp))
            return -1;
    }
    return 0;
}



/* */
