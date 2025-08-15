/*
    oid.c

    Routines to handle OID, X.500 and X.509 fields

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

        - Written by Guillermo Amodeo Ojeda using info from:

            http://www.ietf.org/rfc/rfc2253.txt
            http://www.ietf.org/rfc/rfc4519.txt
            http://www.ietf.org/rfc/rfc3280.txt
            http://www.iana.org/assignments/ldap-parameters/ldap-parameters.xhtml
            http://www.ietf.org/rfc/rfc4013.txt

                                --oO0Oo--
*/

#include "oid.h"

/*
    VERY IMPORTANT:

    When you add new fields to the list, you have to put them on
    ALPHABETIC order by the attribute name in 'field'.

    Then you have to add the enum in the same position in oid.h
    and add code to search a OID using the ASN.1 encoded value
    in find_oid().

    You can use the utility 'oid_asn1' in order to encode an
    OID in ASN.1.

    Remember that field names are case insensitive and that if you
    do not follow the described protocol when adding new entries,
    you will be violating the universal law of programmers, causing
    the end of the modern world as we know it today.

    Or maybe the search algorithm will just stop working. ;-)
*/

struct _oid_
{
    char          *field;    /* Field name (RFC-4519 and RFC-2253) */
    unsigned char  oid[11];  /* OID encoded in ASN.1 */
    size_t         len;      /* Size in bytes of 'oid' */
}
lista[X500_NUM_FIELDS] =
{
    {"businessCategory",{0x55,0x04,15,0,0,0,0,0,0,0,0},3},                        /* 2.5.4.15 */
    {"c",{0x55,0x04,6,0,0,0,0,0,0,0,0},3},                                        /* 2.5.4.6 */
    {"cn",{0x55,0x04,3,0,0,0,0,0,0,0,0},3},                                       /* 2.5.4.3 */
    {"dc",{0x09,0x92,0x26,0x89,0x93,0xf2,0x2c,0x64,0x01,0x19,0},10},              /* 0.9.2342.19200300.100.1.25 */
    {"description",{0x55,0x04,13,0,0,0,0,0,0,0,0},3},                             /* 2.5.4.13 */
    {"dn",{0x55,0x04,49,0,0,0,0,0,0,0,0},3},                                      /* 2.5.4.49 */
    {"dnQualifier",{0x55,0x04,46,0,0,0,0,0,0,0,0},3},                             /* 2.5.4.46 */
    {"emailAddress",{0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x09,0x01,0,0},9},        /* 1.2.840.113549.1.9.1 */
    {"ev_c",{0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x3c,0x02,0x01,0x03},11},         /* 1.3.6.1.4.1.311.60.2.1.3 */
    {"ev_l",{0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x3c,0x02,0x01,0x01},11},         /* 1.3.6.1.4.1.311.60.2.1.1 */
    {"ev_st",{0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x3c,0x02,0x01,0x02},11},        /* 1.3.6.1.4.1.311.60.2.1.2 */
    {"generationQualifier",{0x55,0x04,44,0,0,0,0,0,0,0,0},3},                     /* 2.5.4.44" */
    {"gn",{0x55,0x04,42,0,0,0,0,0,0,0,0},3},                                      /* 2.5.4.42 */
    {"initials",{0x55,0x04,43,0,0,0,0,0,0,0,0},3},                                /* 2.5.4.43 */
    {"jurisdictionOfIncorporationCountryName",{0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x3c,0x02,0x01,0x03},11},         /* as ev_c */
    {"jurisdictionOfIncorporationLocalityName",{0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x3c,0x02,0x01,0x01},11},        /* as ev_l */
    {"jurisdictionOfIncorporationStateOrProvinceName",{0x2b,0x06,0x01,0x04,0x01,0x82,0x37,0x3c,0x02,0x01,0x02},11}, /* as ev_st */
    {"l",{0x55,0x04,7,0,0,0,0,0,0,0,0},3},                                        /* 2.5.4.7 */
    {"o",{0x55,0x04,10,0,0,0,0,0,0,0,0},3},                                       /* 2.5.4.10 */
    {"ou",{0x55,0x04,11,0,0,0,0,0,0,0,0},3},                                      /* 2.5.4.11 */
    {"postalAddress",{0x55,0x04,16,0,0,0,0,0,0,0,0},3},                           /* 2.5.4.16 */
    {"postalCode",{0x55,0x04,17,0,0,0,0,0,0,0,0},3},                              /* 2.5.4.17 */
    {"postOfficeBox",{0x55,0x04,18,0,0,0,0,0,0,0,0},3},                           /* 2.5.4.18 */
    {"pseudonym",{0x55,0x04,65,0,0,0,0,0,0,0,0},3},                               /* 2.5.4.65 */
    {"serialNumber",{0x55,0x04,5,0,0,0,0,0,0,0,0},3},                             /* 2.5.4.5 */
    {"sn",{0x55,0x04,4,0,0,0,0,0,0,0,0},3},                                       /* 2.5.4.4 */
    {"st",{0x55,0x04,8,0,0,0,0,0,0,0,0},3},                                       /* 2.5.4.8 */
    {"street",{0x55,0x04,9,0,0,0,0,0,0,0,0},3},                                   /* 2.5.4.9 */
    {"title",{0x55,0x04,12,0,0,0,0,0,0,0,0},3},                                   /* 2.5.4.12 */
    {"uid",{0x09,0x92,0x26,0x89,0x93,0xf2,0x2c,0x64,0x01,0x01,0},10},             /* 0.9.2342.19200300.100.1.1 */
    {"userPassword",{0x2b,0x06,0x01,0x04,0x01,0x8b,0x3a,0x73,0x79,0x01,0x28},11}, /* 1.3.6.1.4.1.1466.115.121.1.40  */
};

/* ------------------------- */

static int find_oid(const unsigned char *oid,size_t len)
{
    switch(len)
    {
        case 3:
            if(oid[0]==0x55 && oid[1]==0x04)
            {
                switch(oid[2])
                {
                    case  3: return X500_CN;
                    case  4: return X500_SN;
                    case  5: return X500_SERIAL;
                    case  6: return X500_C;
                    case  7: return X500_L;
                    case  8: return X500_ST;
                    case  9: return X500_STREET;
                    case 10: return X500_O;
                    case 11: return X500_OU;
                    case 12: return X500_TITLE;
                    case 13: return X500_DESC;
                    case 15: return X500_BCAT;
                    case 16: return X500_POSTADDR;
                    case 17: return X500_POSTCODE;
                    case 18: return X500_POBOX;
                    case 42: return X500_GN;
                    case 43: return X500_INI;
                    case 44: return X500_GENQ;
                    case 46: return X500_DNQ;
                    case 49: return X500_DN;
                    case 65: return X500_PSEUDO;
                }
            }
            break;
        case 9:
            if(!memcmp(oid,lista[X500_EMAIL].oid,9))
                return X500_EMAIL;
            break;
        case 10:
            if(!memcmp(oid,lista[X500_DC].oid,9))
            {
                switch(oid[9])
                {
                    case 1:
                        return X500_UID;
                    case 25:
                        return X500_DC;
                }
            }
            break;
        case 11:
            if(!memcmp(oid,lista[X500_C_EV].oid,10))
            {
                switch(oid[10])
                {
                    case 1:
                        return X500_L_EV;
                    case 2:
                        return X500_ST_EV;
                    case 3:
                        return X500_C_EV;
                }
            }
            else if(!memcmp(oid,lista[X500_PASSWD].oid,11))
            {
                return X500_PASSWD;
            }
            break;
    }
    return -1;
}

/* ------------------------- */

static int find_name(const char *tosearch)
{
    int min,max,med,res;

    min=0;
    max=X500_NUM_FIELDS - 1;
    while(max >= min)
    {
        med = ((max - min) / 2) + min;
        res=strcasecmp(tosearch,lista[med].field);
        if(!res)
            return med;
        else if(res > 0)
            min = med + 1;
        else //if (res < 0)
            max = med - 1;
    }
    return -1;
}

/* --------------------------------------- */
/* --------------------------------------- */

char *oid_to_x500(char *resp,size_t max,const char *oid)
{
    return field_to_x500(resp,max,oid_to_field(oid));
}

/* --------------------------------------- */

wchar_t *oid_to_x500w(wchar_t *resp,size_t max,const wchar_t *oid)
{
    return field_to_x500w(resp,max,oid_to_fieldw(oid));
}

/* --------------------------------------- */

char *x500_to_oid(char *resp,size_t max,const char *x500)
{

    return field_to_oid(resp,max,find_name(x500));
}


/* --------------------------------------- */

wchar_t *x500_to_oidw(wchar_t *resp,size_t max,const wchar_t *x500)
{
    char tosearch[64];
    int t;

    if(resp && x500)
    {
        /* Convertir a char (los fields x500/x509 son todos ascii) */

        for(t=0;x500[t] && t<63;t++)
            tosearch[t] = (char)(x500[t] & 0x7f);
        tosearch[t]=0;
        return field_to_oidw(resp,max,find_name(tosearch));
    }
    return NULL;
}

/* --------------------------------------- */

char *field_to_x500(char *resp,size_t max,int field)
{
    if(resp && field > -1 && field < X500_NUM_FIELDS)
    {
        strlcpy(resp,lista[field].field,max);
        return resp;
    }
    return NULL;
}

/* --------------------------------------- */

wchar_t *field_to_x500w(wchar_t *resp,size_t max,int field)
{
    char *ptr;
    int t;

    if(resp && field > -1 && field < X500_NUM_FIELDS)
    {
        ptr = lista[field].field;
        for(t=0;ptr[t] && t < max;t++)
            resp[t] = (wchar_t)ptr[t];
        resp[t] = 0;
        return resp;
    }
    return NULL;
}

/* --------------------------------------- */

int x500_to_field(const char *x500)
{
    return find_name(x500);
}

/* --------------------------------------- */

int x500_to_fieldw(const wchar_t *x500)
{
    char tosearch[64];
    int t;

    if(x500)
    {
        for(t=0;x500[t] && t<63;t++)
            tosearch[t] = (char)(x500[t] & 0x7f);
        tosearch[t]=0;
        return find_name(tosearch);
    }
    return -1;
}

/* --------------------------------------- */

char *field_to_oid(char *resp,size_t max,int field)
{
    if(resp && field > -1 && field < X500_NUM_FIELDS)
    {
        asn1_oid(resp,max,lista[field].oid,lista[field].len);
        return resp;
    }
    return NULL;
}

/* --------------------------------------- */

wchar_t *field_to_oidw(wchar_t *resp,size_t max,int field)
{
    if(resp && field > -1 && field < X500_NUM_FIELDS)
    {
        asn1_oidw(resp,max,lista[field].oid,lista[field].len);
        return resp;
    }
    return NULL;
}

/* --------------------------------------- */

int oid_to_field(const char *oid)
{
    unsigned char asn1[128];
    size_t len;

    len = oid_asn1(asn1,128,oid);
    if( len > 0)
        return find_oid(asn1,len);
    return -1;
}

/* --------------------------------------- */

int oid_to_fieldw(const wchar_t *oid)
{
    unsigned char asn1[128];
    int len;

    len = oid_asn1w(asn1,128,oid);
    if( len > 0)
        return find_oid(asn1,len);
    return -1;
}

/* --------------------------------------- */

size_t oid_asn1(unsigned char *asn1,size_t max,const char *oid)
{
    size_t num = 0;
    unsigned int val[128];

    if(oid && asn1)
    {
        num = oid_to_ints(val,128,oid);
        return oid_ints_to_asn1(asn1,max,val,num);
    }
    return 0;
}

/* --------------------------------------- */

size_t oid_asn1w(unsigned char *asn1,size_t max,const wchar_t *oid)
{    
    size_t num = 0;
    unsigned int val[128];

    if(oid && asn1)
    {
        num = oid_to_intsw(val,128,oid);
        return oid_ints_to_asn1(asn1,max,val,num);
    }
    return 0;
}

/* --------------------------------------- */

size_t asn1_oid(char *oid,size_t max,const unsigned char *asn,size_t len)
{
    size_t total,t,ret;
    unsigned int tmp;
    
    total=snprintf(oid,max,"%u.%u",(asn[0] / 40),(asn[0] % 40));
    if(total >= max)
        return 0;

    oid+=total;
    max-=total;

    for(tmp=0,t=1;t<len && max > 0;t++)
    {
        tmp<<=7;
        if(asn[t] & 0x80)
        {
            tmp|=(asn[t] & 0x7F);
        }
        else
        {
            tmp|=(asn[t]);
            ret=snprintf(oid,max,".%u",tmp);
            if(ret >= max)
                return -1;
            oid+=ret;
            max-=ret;
            total+=ret;
            tmp=0;
        }
    }
    return total;
}

/* --------------------------------------- */

size_t asn1_oidw(wchar_t *oid,size_t max,const unsigned char *asn,size_t len)
{
    size_t ret=0;
    char tmp[1581] = {0};
    
    asn1_oid(tmp,1580,asn,len);
    ret = mbstowcs(oid,tmp,max);
    if(ret == (size_t) -1)
        ret = 0;
    return ret;
}

/* --------------------------------------- */

size_t asn1_oid_to_ints(unsigned int *oid,size_t max,const unsigned char *asn,size_t len)
{
    size_t t,ret;
    unsigned int tmp;

    if(max < 2)
        return 0;

    oid[0] = (unsigned int) (asn[0] / 40);
    oid[1] = (unsigned int) (asn[0] % 40);
    
    ret = 2;
    
    for(tmp=0,t=1;t<len && ret < max;t++)
    {
        tmp<<=7;
        if(asn[t] & 0x80)
        {
            tmp|=(asn[t] & 0x7F);
        }
        else
        {
            tmp|=(asn[t]);
            oid[ret++]= tmp;
            tmp=0;
        }
    }

    return ret;
}

/* --------------------------------------- */

size_t oid_ints_to_asn1(unsigned char *asn,size_t max,const unsigned int *v,size_t num)
{
    size_t val,tam=0,t,i;
    unsigned char tmp[6];


    if (max == 0)
        return 0;

    /* First value is (v0 * 40) + v1 */

    asn[tam++] = (unsigned char)((v[0] * 40 + v[1]) & 0xff);

    /* Now add the rest of values */

    for(t=2;t<num && tam < max;t++)
    {
        val = v[t];
        if(!val)
        {
            asn[tam++]=0;
        }
        else
        {
            for(i=0;val;i++)
            {
                tmp[i] = (unsigned char)(val & 0x7f);
                val>>=7;
            }
            while(i > 0 && tam < max)
            {
                if(--i > 0)
                    tmp[i]|=0x80;
                asn[tam++]=(unsigned char)tmp[i];
            }
        }
    }
    return tam;
}


/* --------------------------------------- */

static size_t do_oid_to_ints(unsigned int *dest,size_t max,const void *orig,int wide)
{
    size_t pos, num = 0;
    unsigned int val;
    int quedan = FALSE;
    const wchar_t *oidw = (const wchar_t *)orig;
    const char *oid = (const char *)orig;
    wchar_t ch;

    if(oid && dest)
    {
        num = pos = 0;
        val = 0;
        while(num < max && oid[pos])
        {
            if(wide)
                ch = oidw[pos];
            else    
                ch = (wchar_t)oid[pos];
            switch(ch)
            {
                case L'0':
                case L'1':
                case L'2':
                case L'3':
                case L'4':
                case L'5':
                case L'6':
                case L'7':
                case L'8':
                case L'9':
                    val *=10;
                    val += (unsigned int) (oid[pos++] - L'0');
                    quedan = TRUE;
                    break;
                case L'.':                    
                    dest[num++] = val;
                    val = 0;
                    quedan = FALSE;
                    pos++;
                    break;
                default:
                    if(quedan)
                        dest[num++] = val;
                    return num;                    
            }            
        }        
        if(quedan)
            dest[num++] = val;
        return num;
    }
    return 0;
}

/* --------------------------------------- */

size_t oid_to_ints(unsigned int *dest,size_t max,const char *oid)
{    
    return do_oid_to_ints(dest,max,oid,FALSE);
}

/* --------------------------------------- */

size_t oid_to_intsw(unsigned int *dest,size_t max,const wchar_t *oid)
{    
    return do_oid_to_ints(dest,max,oid,TRUE);
}

/* --------------------------------------- */

size_t asn1_oid_to_x500(char *x500,size_t max,const unsigned char *oid,size_t len)
{
    int cual;

    if(x500)
    {
        cual = find_oid(oid,len);
        if(cual == -1)
            return asn1_oid(x500,max,oid,len);
        return strlcpy(x500,lista[cual].field,max);
    }
    return 0;
}

/* --------------------------------------- */

size_t asn1_oid_to_x500w(wchar_t *x500,size_t max,const unsigned char *oid,size_t len)
{
    size_t cual,t;
    char *ptr;

    if(x500)
    {
        cual = find_oid(oid,len);
        if(cual == -1)
            return asn1_oidw(x500,max,oid,len);

        ptr = lista[cual].field;
        for(t=0;ptr[t] && t < max;t++)
            x500[t] = (wchar_t)ptr[t];
        x500[t] = 0;

        return t;
    }
    return 0;
}

/* --------------------------------------- */

size_t x500_to_oid_asn1(unsigned char *oid,size_t max,const char *x500)
{
    int field = x500_to_field(x500);

    if(oid)
    {
        if(field > -1 && field < X500_NUM_FIELDS)
        {
            if(max > lista[field].len)
                max = lista[field].len;
            memcpy(oid,lista[field].oid,max);
            return max;
        }
        return oid_asn1(oid,max,x500);
    }
    return 0;
}

/* --------------------------------------- */

size_t x500_to_oid_asn1w(unsigned char *oid,size_t max,const wchar_t *x500)
{
    int field = x500_to_fieldw(x500);

    if(oid)
    {
        if(field > -1 && field < X500_NUM_FIELDS)
        {
            if(max > lista[field].len)
                max = lista[field].len;
            memcpy(oid,lista[field].oid,max);
            return max;
        }
        return oid_asn1w(oid,max,x500);
    }
    return 0;
}


/* --------------------------------------- */

size_t escape_string_rfc2253(char *dest,const char *orig, size_t max)
{
    size_t cnt = 0, quoted = 0;

    if(dest)
        memset(dest,0,max);

    switch(*orig)
    {
        case '"':
            quoted++;
            if(dest)
            {
                if(cnt == max)
                    return 0;
                dest[cnt]=*orig;
            }
            orig++;
            cnt++;
            break;
        case '#':
        case ' ':
            if(dest)
            {
                if(cnt + 1 >= max)
                    return 0;
                dest[cnt] = '\\';
                dest[cnt+1] = *orig;
            }
            orig++;
            cnt+=2;
        default:
            break;
    }

    while(*orig)
    {
        switch(*orig)
        {
            case ' ':
                if(!quoted && !orig[1])
                {
                    if(dest)
                    {
                        if(cnt == max)
                            return 0;
                        dest[cnt]='\\';
                    }
                    cnt++;
                }
                break;
            case '"':
                if(quoted)
                    quoted=0;
            case ',':
            case '+':
            case '\\':
            case '<':
            case '>':
            case ';':
                if(!quoted)
                {
                    if(dest)
                    {
                        if(cnt == max)
                            return 0;
                        dest[cnt]='\\';
                    }
                    cnt++;
                }
            default:
                break;
        }
        if(dest)
        {
            if(cnt == max)
                return 0;
            dest[cnt]=*orig;
        }
        orig++;
        cnt++;
    }
    return cnt;
}
