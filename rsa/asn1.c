/*
    asn1.c

    ASN.1 routines

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


#include "asn1.h"

/* ------------------------ */

int asn1_read_type_and_len(int *type,int *len,const void *buf,int max)
{
    int t,pos=0,bytes = 1;
    const unsigned char *ptr = (const unsigned char *)buf;

   *type = ptr[pos++];
    if(pos >= max)
        return -1;
    if(ptr[pos] & 0x80)
    {
        bytes = (ptr[pos++] & 0x7F);
        if(pos >= max)
            return -1;
    }
   *len = 0;
    for(t=0;t<bytes;t++)
    {
       *len = ((*len << 8) | ptr[pos++]);
        if(pos > max)
            return -1;
    }
    return pos;
}

/* ------------------------ */

int asn1_read(asn1_t *asn,const void *data,int len)
{
    int num;

    asn->max = 0;
    asn->dyn = FALSE;

    asn->start = (unsigned char *)data;

    num = asn1_read_type_and_len(&asn->type,&asn->len,asn->start,len);

    if(num > 0)
    {
        asn->data = asn->start + num;

        num += asn->len;

        asn->total = num;

    }
    return num;
}

/* ------------------------------- */

void asn1_seq_free(asn1_seq_t *seq)
{
    if(seq->lst)
    {
        free(seq->lst);
        seq->lst = NULL;
    }
    seq->num = 0;
}

/* ------------------------------- */

int asn1_seq_read(asn1_seq_t *seq,const void *data,int len)
{
    asn1_t *tmp;
    int cnt,num,max;
    unsigned char *ptr=(unsigned char *)data;

    seq->num = 0;
    seq->lst = NULL;
    num=max=0;
    while(len > 0)
    {
        if(num == max)
        {
            tmp=(asn1_t *)realloc(seq->lst,sizeof(asn1_t) * (max + 10));
            if(!tmp)
            {
                asn1_seq_free(seq);
                return -1;
            }
            seq->lst = tmp;
            max += 10;
        }
        cnt=asn1_read(&(seq->lst[num]),ptr,len);
        if(cnt <  1)
            return cnt;

        seq->num = ++num;

        len-=cnt;
        ptr+=cnt;
    }
    return 0;
}

/* ------------------------------- */

int asn1_to_time(asn1_t *asn,time_t *val)
{
    char zone;
    point_on_time_t pot = {0};

    switch(asn->type)
    {
        case ASN1_UTC:
            if(asn->len!=13)
                return -1;
            sscanf((char *)asn->data,"%02d%02d%02d%02d%02d%02d%c",
                                     &pot.year,&pot.mon,&pot.day,&pot.hour,&pot.min,&pot.sec,&zone);
            /*
                Correct two digit dates as section 4.1.2.5.1 of RFC-3280 requires
            */
            if(pot.year < 50)
                pot.year+=2000;
            else
                pot.year+=1900;
            break;
        case ASN1_TIME:
            if(asn->len!=15)
                return -1;
            sscanf((char *)asn->data,"%04d%02d%02d%02d%02d%02d%c",
                                     &pot.year,&pot.mon,&pot.day,&pot.hour,&pot.min,&pot.sec,&zone);
            break;
        default:
            return -1;
    }
   *val=time_from_pot(&pot);
    return 0;
}

/* ------------------------------- */

int asn1_to_bool(asn1_t *asn,int *val)
{
    if(asn->type==ASN1_BOOL)
    {
       *val = (asn->data[0]) ? TRUE : FALSE;
        return 0;
    }
    return -1;
}

/* ------------------------ */

int asn1_to_bignum(mp_int_t *mp,asn1_t *asn)
{
    int ret = -1;

    switch(asn->type)
    {
        case ASN1_ENUM:
        case ASN1_INT:
        case ASN1_BYTES:
            ret = (mp_set_bytes(mp,asn->data,asn->len) == MP_OKAY) ? 0 : -1;
            break;
        case ASN1_BITS:
            ret = (mp_set_bytes(mp,asn->data + 1,asn->len - 1) == MP_OKAY) ? 0 : -1;
            if(ret == 0 && asn->data[0])
            {
                int bits = (int)asn->data[0];
                mp_shift(mp,mp,-bits);
            }
            break;
        default:
            break;
    }
    return ret;
}

/* ------------------------ */

int asn1_to_uint(asn1_t *asn,unsigned int *val)
{
    int len,dsp;
    unsigned char *ptr;

    switch(asn->type)
    {
        case ASN1_ENUM:
        case ASN1_INT:
        case ASN1_BYTES:
            ptr=asn->data;
            len=asn->len;
            dsp=0;
            break;
        case ASN1_BITS:
            ptr=asn->data + 1;
            len=asn->len - 1;
            dsp=asn->data[0];
            break;
        default:
            return -1;
    }
    if(len > sizeof(unsigned int))
        return -1;
   *val = 0;
    while(0 < len--)
        *val = ((*val << 8) | *ptr++);

    if(dsp)
        *val = (*val >> dsp);

    return 0;
}

/* ------------------------ */

int asn1_to_oid(asn1_t *asn,char *oid,int max)
{
    if(asn->type==ASN1_OID)
    {
        if(asn1_oid(oid,max,asn->data,asn->len) > 0)
            return 0;
    }
    return -1;
}

/* ------------------------ */

int asn1_to_oidw(asn1_t *asn,wchar_t *oid,int max)
{
    if(asn->type==ASN1_OID)
    {
        if(asn1_oidw(oid,max,asn->data,asn->len) > 0)
            return 0;
    }
    return -1;
}

/* ------------------------ */

#define ANS1_MAX_STR      1581

/* ------------------------ */

int asn1_to_string(asn1_t *asn,char *dest,int max)
{
    int t;
    time_t val;

    switch(asn->type)
    {
        case ASN1_UTF8:
        case ASN1_PRINTABLE:
        case ASN1_TELETEXT:
        case ASN1_IA5_STR:
            max--;
            for(t=0;t<asn->len && t<max;t++)
                dest[t]=asn->data[t];
            dest[t]=0;
            return t;
        case ASN1_OID:
            return asn1_to_oid(asn,dest,max);
        case ASN1_TIME:
        case ASN1_UTC:
            asn1_to_time(asn,&val);
            time_to_stamp(dest,max,val,FALSE);
            return (int)strlen(dest);
        case ASN1_BMP_STR:
            {
                wchar_t tmp[ANS1_MAX_STR];
                int i=0;

                t=0;
                while(t<asn->len && i<(ANS1_MAX_STR - 1))
                {
                    tmp[i]=asn->data[t];
                    if(++t < asn->len)
                    {
                        tmp[i]=(dest[i] << 8);
                        tmp[i++]=asn->data[t++];
                    }
                }
                tmp[i] = 0;
                wcstombs(dest,tmp,max);
            }
            break;
        case ASN1_INT:
            if(asn->len < 5)
            {
                unsigned int i;

                asn1_to_uint(asn,&i);
                return (int)snprintf(dest,max,"%u",i);
            }
            else
            {
                mp_int_t bn;

                if(mp_init(&bn) == MP_OKAY)
                {
                    if(asn1_to_bignum(&bn,asn) == 0)
                    {
                        unsigned int count = 0;
                        unsigned char *tmp;

                        tmp = mp_get_bytes(&bn,&count);
                        if(tmp)
                        {
                            count = (unsigned int) hex_to_char(dest,max,tmp,count,FALSE);
                            free(tmp);
                        }
                        mp_clear(&bn);
                        return (int)count;
                    }
                    mp_clear(&bn);
                    return -1;
                }
            }
            /* Fall over to next if bn == NULL */
        case ASN1_NULL:
            return (int) strlcpy(dest,"NULL",max);
        case ASN1_BOOL:
            return (int) snprintf(dest,max,"%s",(asn->data[0]) ? "true" : "false");
        default:
            break;
    }
    return asn1_to_rfc2253(asn,dest,max);
}

/* ------------------------ */

int asn1_to_wcs(asn1_t *asn,wchar_t *dest,int max)
{
    int t,i;
    time_t val;

    switch(asn->type)
    {
        case ASN1_UTF8:
            if(utf8_to_wchar(dest,max,asn->data,asn->len) > 0)
                return (int)wcslen(dest);
            break;
        case ASN1_PRINTABLE:
        case ASN1_TELETEXT:
        case ASN1_IA5_STR:
            max--;
            for(t=0;t<asn->len && t<max;t++)
                dest[t]=(wchar_t)asn->data[t];
            dest[t]=0;
            return t;
        case ASN1_BMP_STR:
            i=t=0;
            max--;
            while(t<asn->len && i<max)
            {
                dest[i]=asn->data[t];
                if(++t < asn->len)
                {
                    dest[i]=(dest[i] << 8);
                    dest[i++]=asn->data[t++];
                }
            }
            dest[i] = 0;
            break;
        case ASN1_OID:
            return asn1_to_oidw(asn,dest,max);
        case ASN1_TIME:
        case ASN1_UTC:
            asn1_to_time(asn,&val);
            time_to_stampw(dest,max,val,FALSE);
            return (int)wcslen(dest);
        case ASN1_INT:
            if(asn->len < 5)
            {
                unsigned int i;

                asn1_to_uint(asn,&i);
                return swprintf(dest,max,L"%u",i);
            }
            else
            {
                mp_int_t bn;

                if(mp_init(&bn) == MP_OKAY)
                {
                    if(asn1_to_bignum(&bn,asn)==0)
                    {
                        unsigned int count = 0;
                        unsigned char *tmp;

                        tmp = mp_get_bytes(&bn,&count);
                        if(tmp)
                        {
                            count = (unsigned int) hex_to_wchar(dest,max,tmp,count,FALSE);
                            free(tmp);
                        }
                        mp_clear(&bn);
                        return (int)count;
                    }
                    mp_clear(&bn);
                }
                return -1;
            }
            /* Fall over to next if bn == NULL */
        case ASN1_NULL:
            return (int)swprintf(dest,max,L"NULL");
        case ASN1_BOOL:
            return (int)swprintf(dest,max,L"%ls",(asn->data[0]) ? L"Yes" : L"No");
        default:
            break;
    }
    return asn1_to_rfc2253w(asn,dest,max);
}

/* ------------------------ */

int asn1_to_rfc2253(asn1_t *asn,char *dest,int max)
{
    int t,len = (asn->total * 2) + 2;


    if(len <= max)
    {
       *dest++='#';
        for(t=0;t<asn->total;t++)
        {
            snprintf(dest,3,"%02x",asn->start[t]);
            dest+=2;
        }
       *dest='\0';
        return len;
    }
    return 0;
}

/* ------------------------ */

int asn1_to_rfc2253w(asn1_t *asn,wchar_t *dest,int max)
{
    int t,len = (asn->total * 2) + 2;

    if(len <= max)
    {
       *dest++=L'#';
        for(t=0;t<asn->total;t++)
        {
            swprintf(dest,3,L"%02x",asn->start[t]);
            dest+=2;
        }
       *dest=L'\0';
        return len;
    }
    return 0;
}

/* ------------------------ */

int asn1_to_whole_attribute(asn1_t *asn,char *val,int max)
{
    int len,ret=0;
    asn1_seq_t set;
    asn1_t seq;

    if(!val)
        return 0;

    memset(val,0,max);

    if(asn->type==ASN1_SET && asn1_read(&seq,asn->data,asn->len) > 0 && !asn1_seq_read(&set,seq.data,seq.len))
    {
        if(set.num == 2 && set.lst[0].type==ASN1_OID)
        {
            len = asn1_oid_to_x500(val,max,set.lst[0].data,set.lst[0].len);
            if(len < 1)
            {
                asn1_seq_free(&set);
                return 0;
            }
            val+=len;
            max-=len;
           *val++='=';
            if(++len >= max)
            {
                asn1_seq_free(&set);
                return -1;
            }
            ret+=len;
            ret+=asn1_to_string(&(set.lst[1]),val,--max);
        }
        asn1_seq_free(&set);
    }
    return ret;
}

/* ------------------------ */

int asn1_to_whole_attributew(asn1_t *asn,wchar_t *val,int max)
{
    int len,ret=0;
    asn1_seq_t set;
    asn1_t seq;

    if(!val)
        return 0;

    memset(val,0,sizeof(wchar_t) * max);

    if(asn->type==ASN1_SET && asn1_read(&seq,asn->data,asn->len) > 0 && !asn1_seq_read(&set,seq.data,seq.len))
    {
        if(set.num == 2 && set.lst[0].type==ASN1_OID)
        {
            len = asn1_oid_to_x500w(val,max,set.lst[0].data,set.lst[0].len);
            if(len < 1)
            {
                asn1_seq_free(&set);
                return 0;
            }
            val+=len;
            max-=len;
           *val++=L'=';
            if(++len >= max)
            {
                asn1_seq_free(&set);
                return -1;
            }
            ret+=len;
            ret+=asn1_to_wcs(&(set.lst[1]),val,--max);
        }
        asn1_seq_free(&set);
    }
    return ret;
}

/* ------------------------ */

int asn1_to_attribute(asn1_t *asn,char *oid,int omax,char *val,int vmax)
{
    asn1_seq_t set;
    asn1_t seq;

    if(asn->type==ASN1_SET && asn1_read(&seq,asn->data,asn->len) > 0 && !asn1_seq_read(&set,seq.data,seq.len))
    {
        if(set.num == 2 && set.lst[0].type==ASN1_OID)
        {
            if(oid)
            {
                if(asn1_oid_to_x500(oid,omax,set.lst[0].data,set.lst[0].len) < 1)
                {
                    asn1_seq_free(&set);
                    return -1;
                }
            }

            if(val)
                asn1_to_string(&(set.lst[1]),val,vmax);
        }
        asn1_seq_free(&set);
        return 0;
    }
    return -1;
}

/* ------------------------ */

int asn1_to_attributew(asn1_t *asn,wchar_t *oid,int omax,wchar_t *val,int vmax)
{
    asn1_seq_t set;
    asn1_t seq;

    if(asn->type==ASN1_SET && asn1_read(&seq,asn->data,asn->len) > 0 && !asn1_seq_read(&set,seq.data,seq.len))
    {
        if(set.num == 2 && set.lst[0].type==ASN1_OID)
        {
            if(oid)
            {
                if(asn1_oid_to_x500w(oid,omax,set.lst[0].data,set.lst[0].len) < 1)
                {
                    asn1_seq_free(&set);
                    return -1;
                }
            }

            if(val)
                asn1_to_wcs(&(set.lst[1]),val,vmax);
        }
        asn1_seq_free(&set);
        return 0;
    }
    return -1;
}

/* ------------------------ */
/* ------------------------ */

#define print_level(_f,_n)     {int _t=_n; for(_t=0;_t<_n;_t++) fprintf(_f,"    ");}

static int ASN_WIDE = 32;

/* ------------------------ */

static void asn1_list_to_text(FILE *fp,int level,asn1_t *asn,int num)
{
    int t,cnt,n,last;
    int bl;
    unsigned int ui;
    asn1_seq_t seq;
    char oid[81];
    wchar_t tmpw[ANS1_MAX_STR];
    char    tmpc[ANS1_MAX_STR];

    while( 0 < num-- )
    {
        print_level(fp,level);
        last = asn->len - 1;
        switch(asn->type)
        {
            case ASN1_END:
                fprintf(fp,"END\n");
                break;
            case ASN1_BOOL:
                asn1_to_bool(asn,&bl);
                fprintf(fp,"BOOLEAN %s,\n",bl ? "true" : "false");
                break;
            case ASN1_INT:
                fprintf(fp,"INTEGER");
                if(asn1_to_uint(asn,&ui)==0)
                {
                    fprintf(fp," %d (decimal)\n",(int)ui);
                }
                else
                {
                    for(n=t=0;t<asn->len;t++)
                    {
                        fprintf(fp," %02x",asn->data[t]);
                        if(++n==ASN_WIDE && t < last)
                        {
                            n=0;
                            fprintf(fp,"\n       ");
                            print_level(fp,level);
                        }
                    }
                    if(n)
                        fprintf(fp,"\n");
                }
                break;
            case ASN1_BITS:
                fprintf(fp,"BITS");
                for(n=t=0;t<asn->len;t++)
                {
                    fprintf(fp," %02x",asn->data[t]);
                    if(++n==ASN_WIDE && t < last)
                    {
                        n=0;
                        fprintf(fp,"\n    ");
                        print_level(fp,level);
                    }
                }
                if(n)
                    fprintf(fp,"\n");
                break;
            case ASN1_BYTES:
                fprintf(fp,"BYTES");
                for(n=t=0;t<asn->len;t++)
                {
                    fprintf(fp," %02x",asn->data[t]);
                    if(++n==ASN_WIDE && t < last)
                    {
                        n=0;
                        fprintf(fp,"\n     ");
                        print_level(fp,level);
                    }
                }
                if(n)
                    fprintf(fp,"\n");
                break;
            case ASN1_NULL:
                fprintf(fp,"NULL\n");
                break;
            case ASN1_OID:
                fprintf(fp,"OBJECT IDENTIFIER ");
                if(!asn1_to_oid(asn,oid,81))
                    fprintf(fp,"%s\n",oid);
                else
                    fprintf(fp,"(can't read it)\n");
                break;
            case ASN1_UTF8:      /* UTF8String */
                fprintf(fp,"UTF8 STRING \"");
                if(asn1_to_wcs(asn,tmpw,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%ls",tmpw);
                fprintf(fp,"\"\n");
                break;
            case ASN1_PRINTABLE: /* PrintableString */
                fprintf(fp,"PRINTABLE STRING \"");
                if(asn1_to_string(asn,tmpc,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%s",tmpc);
                fprintf(fp,"\"\n");
                break;
            case ASN1_TELETEXT: /* TeletextString */
                fprintf(fp,"TELETEXT STRING \"");
                if(asn1_to_string(asn,tmpc,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%s",tmpc);
                fprintf(fp,"\"\n");
                break;
            case ASN1_IA5_STR:   /* IA5String */
                fprintf(fp,"IA5 STRING \"");
                if(asn1_to_wcs(asn,tmpw,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%ls",tmpw);
                fprintf(fp,"\"\n");
                break;
            case ASN1_UTC:       /* UTC */
                fprintf(fp,"UTC ");
                if(asn1_to_string(asn,tmpc,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%s\n",tmpc);
                else
                    fprintf(fp,"(can't read)\n");
                break;
            case ASN1_TIME:       /* Time */
                fprintf(fp,"TIME ");
                if(asn1_to_string(asn,tmpc,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%s\n",tmpc);
                else
                    fprintf(fp,"(can't read)\n");
                break;
            case ASN1_BMP_STR:   /* BMPString (Unicode) */
                fprintf(fp,"BMP STRING \"");
                if(asn1_to_wcs(asn,tmpw,ANS1_MAX_STR) > 0)
                    fprintf(fp,"%ls",tmpw);
                else
                    fprintf(fp,"(can't read)");
                fprintf(fp,"\",\n");
                break;
            case ASN1_SEQ:
                fprintf(fp,"SEQUENCE {\n");
                cnt=asn1_seq_read(&seq,asn->data,asn->len);
                if(cnt)
                {
                    print_level(fp,level+1);
                    fprintf(fp,"(error %d)\n",cnt);
                }
                else
                {
                    asn1_list_to_text(fp,level+1,seq.lst,seq.num);
                    asn1_seq_free(&seq);
                }
                print_level(fp,level);
                fprintf(fp,"}\n");
                break;
            case ASN1_SET:
                fprintf(fp,"SET {\n");
                cnt=asn1_to_text(fp,level+1,asn->data,asn->len);
                if(cnt < 0)
                {
                    print_level(fp,level+1);
                    fprintf(fp,"(error %d)\n",cnt);
                }
                print_level(fp,level);
                fprintf(fp,"}\n");
                break;
            default:
                if((cnt=asn1_explicit(asn))!=-1)
                {
                    fprintf(fp,"EXPLICIT [%d] {\n",cnt);

                    cnt=asn1_seq_read(&seq,asn->data,asn->len);
                    if(cnt)
                    {
                        print_level(fp,level+1);
                        fprintf(fp,"(error %d)\n",cnt);
                    }
                    else
                    {
                        asn1_list_to_text(fp,level+1,seq.lst,seq.num);
                        asn1_seq_free(&seq);
                    }
                    print_level(fp,level);
                    fprintf(fp,"}\n");
                }
                else
                {
                    if((cnt=asn1_implicit(asn))!=-1)
                        fprintf(fp,"IMPLICIT [%d]\n",cnt);
                    else
                        fprintf(fp,"UNKNOWN TYPE %02x\n",asn->type);
                    print_level(fp,level + 1);
                    fprintf(fp,"%02x",asn->data[0]);
                    for(n=t=1;t<asn->len;t++)
                    {
                        fprintf(fp," %02x",asn->data[t]);
                        if(++n==ASN_WIDE && t < last)
                        {
                            n=0;
                            fprintf(fp,"\n");
                            print_level(fp,level+1);
                        }
                    }
                    if(n)
                        fprintf(fp,"\n");
                }
                break;
        }
        asn++;
    }
}

/* ------------------------ */

int asn1_to_text(FILE *fp,int level,const void *buf,int len)
{
    asn1_t asn;
    int cnt;
    unsigned char *data=(unsigned char *)buf;

    while(len > 0)
    {
        cnt=asn1_read(&asn,data,len);
        if(cnt <  1)
            return cnt;
        asn1_list_to_text(fp,level,&asn,1);
        len-=cnt;
        data+=cnt;
    }
    return 0;
}

/* ------------------------ */

int asn1_bytes_of_len(unsigned int len)
{
    if(len < 0x80)
        return 1;

    if(len < 0x100)
        return 2;

    if(len < 0x10000)
        return 3;

    if(len < 0x1000000)
        return 4;

    return 5;
}

/* ------------------------ */

int asn1_put_len(unsigned int len,void *buf)
{
    unsigned char *dest=(unsigned char *)buf;

    if(len < 0x80)
    {
        dest[0]=(unsigned char)len;
        return 1;
    }


    if(len < 0x100)
    {
        dest[0]=0x81;
        dest[1]=(unsigned char)len;
        return 2;
    }

    if(len < 0x10000)
    {
        dest[0]=0x82;
        dest[1]=(unsigned char)((len >> 8) & 0xff);
        dest[2]=(unsigned char)(len & 0xff);
        return 3;
    }

    if(len < 0x1000000)
    {
        dest[0]=0x83;
        dest[1]=(unsigned char)((len >> 16) & 0xff);
        dest[2]=(unsigned char)((len >> 8) & 0xff);
        dest[3]=(unsigned char)(len & 0xFF);
        return 4;
    }

    dest[0]=0x84;
    dest[1]=(unsigned char)((len >> 24) & 0xff);
    dest[2]=(unsigned char)((len >> 16) & 0xff);
    dest[3]=(unsigned char)((len >>  8) & 0xff);
    dest[4]=(unsigned char)(len & 0xff);

    return 5;
}


/* ------------------------ */

int asn1_calc_len(int type,unsigned int len,int *data)
{
    switch(type)
    {
        case ASN1_END:
            if(data)
                *data = 0;
            return 1;

        case ASN1_NULL:
            if(data)
                *data = 0;
            return 2;

        case ASN1_BOOL:
            if(data)
                *data = 1;
            return 3;

        case ASN1_BITS:
            len  = ((len + 7) / 8);
            len++;
            break;

        case ASN1_UTC:
            if(data)
                *data = 13;
            return 15;      /* type + 1 len + 13 time */

        case ASN1_TIME:
            if(data)
                *data = 15;
            return 17;      /* type + 1 len + 15 time */

        case ASN1_BMP_STR:
            len*=2;         /* Unicode 2 bytes per character */
            break;

        case ASN1_INT:
        case ASN1_BYTES:
        case ASN1_OID:
        case ASN1_ENUM:
        case ASN1_UTF8:
        case ASN1_PRINTABLE:
        case ASN1_TELETEXT:
        case ASN1_IA5_STR:
        case ASN1_SEQ:
        case ASN1_SET:
            break;

        default:
             if(!ASN1_EXPLICIT_TYPE(type))
                 return -1;
            break;
    }

    if(data)
        *data = (int)len;


    if(len < 0x80)
        return (len + 2);

    if(len < 0x100)
        return (len + 3);

    if(len < 0x10000)
        return (len + 4);

    if(len < 0x1000000)
        return (len + 5);

    return (len + 6);
}

/* ------------------------ */

void asn1_free(asn1_t *asn)
{
    if(asn)
    {
        if(asn->max && asn->data)
            free(asn->start);
        if(asn->dyn)
            free(asn);
    }
}

/* ------------------------ */

asn1_t *asn1_create(int type,int len)
{
    asn1_t *asn;

    asn = (asn1_t *)malloc(sizeof(asn1_t));
    if(asn)
    {
        asn->dyn = TRUE;
        asn->type = type;
        asn->max = asn1_calc_len(type,len,&asn->len);
        if(asn->max > 0)
        {
            asn->start = (unsigned char *)malloc(asn->max);
            if(asn->start)
            {
                asn->start[0] = (unsigned char) type;
                asn1_put_len(asn->len,asn->start + 1);
                asn->total = asn->max;
                asn->data = asn->start + (asn->total - asn->len);
                return asn;
            }
        }
        free(asn);
    }
    return NULL;
}

/* ------------------------ */

unsigned char *asn1_make_space(asn1_t *asn,int len)
{
    int max,cbt,nbt;
    unsigned char *ret=NULL;

    if(asn)
    {
        max = asn->len + len;
        nbt = asn1_bytes_of_len(max);
        max+=nbt;
        max++;

        if(max >= asn->max)
        {
            ret = (unsigned char *)realloc(asn->start,max + 64);
            if(ret == NULL)
                return NULL;
            asn->start = ret;
            asn->data = asn->start + (asn->total - asn->len);
            asn->max = max + 64;
        }

        cbt = asn1_bytes_of_len(asn->len);

        if(nbt == cbt)
        {
            ret = asn->data;
        }
        else if(asn->len > 0)
        {
            ret = asn->start + 1 + nbt;
            memmove(ret,asn->data,asn->len);
            asn->data = ret;
            asn->total = 1 + nbt + asn->len;
        }

        ret += asn->len;
        asn->len+=len;
        asn->total+=len;
        asn1_put_len(asn->len,asn->start + 1);
    }
    return ret;
}


/* ------------------------ */

int asn1_add_data(asn1_t *asn,const void *dat,int len)
{
    unsigned char *ptr;

    ptr=asn1_make_space(asn,len);
    if(ptr)
    {
        memcpy(ptr,dat,len);
        return 0;
    }
    return -1;
}

/* ------------------------ */

asn1_t *null_asn1(void)
{
    return asn1_create(ASN1_NULL,0);
}

/* ------------------------ */

asn1_t *asn1_create_explicit(int num,const void *bytes,int len)
{
    asn1_t *asn;

    asn=asn1_create(ASN1_EXPLICIT(num),len);
    if(asn && bytes)
        memcpy(asn->data,bytes,len);
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_bytes(const void *bytes,int len)
{
    asn1_t *asn;

    asn=asn1_create(ASN1_BYTES,len);
    if(asn)
        memcpy(asn->data,bytes,len);
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_bits(const void *bits,int numbits)
{
    int rem,len;
    asn1_t *asn;

    asn=asn1_create(ASN1_BITS,numbits);
    if(asn)
    {
        len = (numbits / 8);
        rem = (numbits % 8);

        if(rem)
            len++;

        asn->data[0]=(rem) ? 8 - rem : 0;
        memcpy(asn->data + 1,bits,len);
    }
    return asn;
}

/* ------------------------------- */

asn1_t *asn1_create_time(int type,time_t val)
{
    asn1_t *asn = NULL;
    point_on_time_t pet;

    time_to_pot(val,&pet,FALSE);
    switch(type)
    {
        case ASN1_UTC:
            asn=asn1_create(ASN1_UTC,0);
            if(asn)
            {
                /* two digit date correction as per section 4.1.2.5.1  of RFC-3280 */

                if(pet.year > 1999)
                    pet.year-=2000;
                else if(pet.year > 1899)
                    pet.year-=1900;
                else
                    pet.year = 0;   /* prior to 1900 converted to 1900 */
                asn->type = ASN1_UTC;

                sprintf((char *)asn->data,"%02d%02d%02d%02d%02d%02dZ",pet.year,pet.mon,pet.day,pet.hour,pet.min,pet.sec);
            }
            break;
        case ASN1_TIME:
            asn=asn1_create(ASN1_TIME,0);
            if(asn)
            {
                if(pet.year > 9999)
                    pet.year=9999;
                sprintf((char *)asn->data,"%04d%02d%02d%02d%02d%02dZ",pet.year,pet.mon,pet.day,pet.hour,pet.min,pet.sec);
            }
            break;
        default:
            return NULL;
    }
    return asn;
}

/* ------------------------ */

#if defined(ILP64)

#error "asn1_create_uint() Does not support 'int' being 64 bits."

#endif


int asn1_encode_uint(void *buf,int max,unsigned int val)
{
    int pos = 0, len = 4;
    unsigned int mask = val;
    unsigned char tmp[4],*ret = (unsigned char *) buf;

    /*  ASN1/DER requires that enconded ints occupy the smallest possible */

    tmp[3] = (unsigned char)((mask >>  0) & 0xff);
    tmp[2] = (unsigned char)((mask >>  8) & 0xff);
    tmp[1] = (unsigned char)((mask >> 16) & 0xff);
    tmp[0] = (unsigned char)((mask >> 24) & 0xff);

    while(pos < 3 && tmp[pos] == 0)
    {
        len--;
        pos++;
    }

    /*  If top bit is 1 we add a leading 0 to avoid negatives */

    if (tmp[pos] & 0x80)
    {
        len++;
        if(ret && max > 0)
        {
           *ret++ = 0;
            max--;
        }
    }

    if(ret)
    {
        if(max < len)
            return -1;
        memcpy(ret, tmp + pos,len);
    }

    return len;
}

/* ------------------------ */

int asn1_encode_uint_bits(void *buf,int max,unsigned int val)
{
    int len,bits,shf;
    unsigned int mask;
    unsigned char *tmp = (unsigned char *)buf;

    /*  ASN1/DER requires that enconded bits occupy the smallest possible */

    bits = 0;
    mask = val;

    while (mask)
    {
        bits++;
        mask >>= 1;
    }

    len = (bits + 7) / 8;
    shf = (len * 8) - bits;
    val <<= shf;

    if(tmp)
    {
        if(max <= len)
            return -1;

        tmp [0] = (unsigned char ) ( shf & 0xff);

        shf = 1;

        switch(len)
        {
            case  3: tmp[shf++]=(unsigned char)((val >> 16) & 0xff);
            case  2: tmp[shf++]=(unsigned char)((val >>  8) & 0xff);
            case  1: tmp[shf++]=(unsigned char)(val & 0xff);
            default: break;
        }
    }
    return len + 1;
}

/* ------------------------ */

asn1_t *asn1_create_uint(unsigned int val)
{
    asn1_t *asn;

    asn=asn1_create(ASN1_INT,asn1_encode_uint(NULL,0,val));
    if(asn)
        asn1_encode_uint(asn->data,asn->len,val);

    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_int_from_bytes(const void *buf,int num)
{
    asn1_t *asn=NULL;
    unsigned char *bytes = (unsigned char *)buf;

    if(bytes && num > 0)
    {
        while(*bytes==0 && num > 1)
        {
            bytes++;
            num--;
        }
        asn=asn1_create(ASN1_INT,num);
        if(asn)
            memcpy(asn->data,bytes,num);
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_uint_from_bytes(const void *buf,int num)
{
    asn1_t *asn=NULL;
    unsigned char *bytes = (unsigned char *)buf;

    if(bytes && num > 0)
    {
        int top;

        /*  ASN1/DER requires that enconded ints occupy the
            smallest possible, so we remove leading 0s
        */

        while(*bytes==0 && num > 1)
        {
            bytes++;
            num--;
        }

        /*  If top bit is 1 we add a leading 0 to avoid negatives */

        top = (bytes[0] & 0x80) ? 1 : 0;

        asn=asn1_create(ASN1_INT, num + top);
        if(asn)
        {
            if(top)
                asn->data[0] = 0;
            memcpy(asn->data + top,bytes,num);
        }
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_bignum(mp_int_t *bn)
{
    asn1_t *asn = NULL;
    unsigned char *data;
    unsigned int len;

    data=mp_get_bytes(bn,&len);
    if(data)
    {
        asn=asn1_create_uint_from_bytes(data,len);
        free(data);
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_bignum_bits(mp_int_t *bn)
{
    asn1_t *asn = NULL;
    unsigned char *data;
    unsigned int len;

    data=mp_get_bytes(bn,&len);
    if(data)
    {
        asn=asn1_create_bits(data,len * 8);
        free(data);
    }
    return asn;
}


/* ------------------------ */

asn1_t *asn1_create_int(const char *num)
{
    asn1_t *ret = NULL;

    if(num)
    {
        int base = 10;
        mp_int_t bn;

        if(num[0]=='0' && num[1]=='x')
        {
            base = 16;
            num+=2;
        }

        if(mp_init_set_string(&bn,num,base) == MP_OKAY)
        {
            ret = asn1_create_bignum(&bn);
            mp_clear(&bn);
        }
    }
    return ret;
}

/* ------------------------ */

asn1_t *asn1_create_intw(const wchar_t *num)
{
    asn1_t *ret = NULL;

    if(num)
    {
        int base = 10;
        mp_int_t bn;

        if(num[0]==L'0' && num[1]==L'x')
        {
            base = 16;
            num+=2;
        }
        if(mp_init_set_wide_string(&bn,num,base) == MP_OKAY)
        {
            ret = asn1_create_bignum(&bn);
            mp_clear(&bn);
        }
    }
    return ret;
}

/* ------------------------ */

asn1_t *asn1_create_utf8(const char *cad)
{
    asn1_t *asn=NULL;

    if(cad)
    {
        int len=strlen(cad);
        asn=asn1_create(ASN1_UTF8,len);
        if(asn)
            memcpy(asn->data,cad,len);
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_rfc2253(const char *cad)
{
    asn1_t *asn=NULL;
    int len,bytes,t;

    if(cad && *cad++=='#')
    {
        len = strlen(cad);
        if(len > 3)
        {
            asn = (asn1_t *)malloc(sizeof(asn1_t));
            if(asn)
            {
                asn->dyn = TRUE;
                asn->max = len;
                if(asn->max > 0)
                {
                    asn->start = (unsigned char *)malloc(asn->max);
                    if(asn->start)
                    {
                        asn->total = 0;
                        for(t=0;t<len;t+=2)
                        {
                            asn->start[asn->total++]=(unsigned char)(strtol(cad,NULL,16) & 0xff);
                            cad+=2;
                        }
                        bytes = asn1_read_type_and_len(&asn->type,&asn->len,asn->start,asn->total);
                        if(bytes > 0)
                        {
                            bytes+=asn->len;
                            if(bytes == asn->total)
                            {
                                asn->data = asn->start + (asn->total - asn->len);
                                return asn;
                            }
                        }
                        free(asn->start);
                    }
                }
                free(asn);
            }
        }
    }
    return NULL;
}

/* ------------------------ */

asn1_t *asn1_create_rfc2253w(const wchar_t *cad)
{
    asn1_t *asn=NULL;
    int len,bytes,t;

    if(cad && *cad++=='#')
    {
        len = wcslen(cad);
        if(len > 3)
        {
            asn = (asn1_t *)malloc(sizeof(asn1_t));
            if(asn)
            {
                asn->dyn = TRUE;
                asn->max = len;
                if(asn->max > 0)
                {
                    asn->start = (unsigned char *)malloc(asn->max);
                    if(asn->start)
                    {
                        asn->total = 0;
                        for(t=0;t<len;t+=2)
                        {
                            asn->start[asn->total++]=(unsigned char)(wcstol(cad,NULL,16) & 0xff);
                            cad+=2;
                        }
                        bytes = asn1_read_type_and_len(&asn->type,&asn->len,asn->start,asn->total);
                        if(bytes > 0)
                        {
                            bytes+=asn->len;
                            if(bytes == asn->total)
                            {
                                asn->data = asn->start + (asn->total - asn->len);
                                return asn;
                            }
                        }
                        free(asn->start);
                    }
                }
                free(asn);
            }
        }
    }
    return NULL;
}

/* ------------------------ */

asn1_t *asn1_create_utf8w(const wchar_t *cad)
{
    asn1_t *asn=NULL;
    int len, max;

    if(cad)
    {
        len = wcslen(cad);
        max = len * 4 + 1;
        asn=asn1_create(ASN1_UTF8,max);
        if(asn)
        {
            asn->len = wchar_to_utf8(asn->start,max,cad,len);
            len = asn1_bytes_of_len(asn->len);
            asn->data = (asn->start + 1 + len);
            memmove(asn->data,asn->start,asn->len);
            asn->start[0]=ASN1_UTF8;
            asn1_put_len(asn->len,asn->start + 1);
            asn->total = 1 + len + asn->len;
        }
    }
    return asn;
}


/* ------------------------ */

asn1_t *asn1_create_oid(const char *oid)
{
    unsigned char asn1[128];
    asn1_t *asn = NULL;
    int len;

    len = oid_asn1(asn1,128,oid);
    if(len > 0)
    {
        asn=asn1_create(ASN1_OID,len);
        if(asn)
            memcpy(asn->data,asn1,len);
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_oidw(const wchar_t *oid)
{
    unsigned char asn1[128];
    asn1_t *asn = NULL;
    int len;

    len = oid_asn1w(asn1,128,oid);
    if(len > 0)
    {
        asn=asn1_create(ASN1_OID,len);
        if(asn)
            memcpy(asn->data,asn1,len);
    }
    return asn;
}

/* ------------------------ */

static asn1_t *crea_atributo_asn1_set(asn1_t *o,asn1_t *v)
{
    asn1_t *asn=NULL;
    int seq,set;


    seq=(o->total + v->total);
    set=1 + asn1_bytes_of_len(seq) + seq;

    asn = asn1_create(ASN1_SET,set);
    if(asn)
    {
        asn->data[0] = ASN1_SEQ;
        set=1 + asn1_put_len(seq,asn->data + 1);
        memcpy(asn->data + set,o->start,o->total);
        set+=o->total;
        memcpy(asn->data + set,v->start,v->total);
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_attr_set(const char *oid,const char *val)
{
    asn1_t *asn=NULL,*o,*v;

    o = asn1_create_oid(oid);
    if(o)
    {
        if(val)
        {
            if(*val == '#')
                v = asn1_create_rfc2253(val);
            else
                v = asn1_create_utf8(val);
        }
        else
        {
            v = null_asn1();
        }

        if(v)
        {
            asn = crea_atributo_asn1_set(o,v);
            free(v);
        }
        free(o);
    }
    return asn;
}

/* ------------------------ */

asn1_t *asn1_create_attr_setw(const wchar_t *oid,const wchar_t *val)
{
    asn1_t *asn=NULL,*o,*v;

    o = asn1_create_oidw(oid);
    if(o)
    {
        if(val)
        {
            if(*val == L'#')
                v = asn1_create_rfc2253w(val);
            else
            v = asn1_create_utf8w(val);
        }
        else
        {
            v = null_asn1();
        }

        if(v)
        {
            asn = crea_atributo_asn1_set(o,v);
            free(v);
        }
        free(o);
    }
    return asn;
}

/* ------------------------ */

char *asn1_one_line_rdn(unsigned char *asn,int len)
{
    asn1_seq_t set;
    int t,num=0,max=0,cntv,cnto,cnt;
    char *ret=NULL,oid[256],tmp[256],*ptr;

    if(asn)
    {
        if(!asn1_seq_read(&set,asn,len))
        {
            if(set.num > 0)
            {
                num = 0;

                for(t=0;t<set.num;t++)
                {
                    if(!asn1_to_attribute(&(set.lst[t]),oid,256,tmp,256))
                    {
                        cnto=strlen(oid);
                        cntv=escape_string_rfc2253(NULL,tmp,0);
                        if(cntv < 1)
                        {
                            if(ret)
                                free(ret);
                            return NULL;
                        }

                        /* Incluir el igual y la coma */

                        cnt=cntv + cnto + 2;

                        if((max - num) <= cnt)
                        {
                            ptr=(char *)realloc(ret,max + cnt + 256);
                            if(ptr==NULL)
                            {
                                if(ret)
                                    free(ret);
                                return NULL;
                            }
                            ret = ptr;
                            max+=(cnt + 256);
                        }
                        if(num > 0)
                            ret[num++]=',';
                        memcpy(ret + num,oid,cnto);
                        num+=cnto;
                        ret[num++]='=';
                        escape_string_rfc2253(ret+num,tmp,cntv);
                        num+=cntv;
                        ret[num]='\0';
                    }
                }
                if(num==0)
                {
                    free(ret);
                    ret=NULL;
                }
            }
            asn1_seq_free(&set);
        }
    }
    return ret;
}



