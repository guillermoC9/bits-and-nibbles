/*
    csv.c

    Comma Separated Value (.CSV) format

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

        - Originally written by Guillermo Amodeo Ojeda using these references:

          * https://en.wikipedia.org/wiki/Tab-separated_values
          * https://en.wikipedia.org/wiki/Comma-separated_values

        - This code is used to read documents in any delimiter-separated value
          format. Clearly dominated by CSV as popularized by Microsoft Excel and
          Microsoft Access.

        - Files can be encoded either in Microsoft's Windows 1252 codepage or
          UTF-8 but are internally converted to UTF-8 when read which is the
          enconding used in memory.

                                --oO0Oo--
*/

#include "csv.h"

#if defined(_WIN32)
    #define fseeko  fseek
    #define ftello  ftell
#endif


/* --------------------------------------------------- *
    Reference:

        https://en.wikipedia.org/wiki/Windows-1252

 * --------------------------------------------------- */

static unsigned int win1252_to_ucs4(char cual)
{
    unsigned char ch = (unsigned char) cual;

    if(ch > 0x7f && ch < 0xa0)
    {
        switch(ch)
        {
            case 0x80: return 0x20ac;  /*  € */
            case 0x81: return 0xfffd; /* WCHAR_REPLACEMENT */
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
            case 0x8d: return 0xfffd; /* WCHAR_REPLACEMENT */
            case 0x8e: return 0x017d;  /*  Ž */
            case 0x8f: return 0xfffd; /* WCHAR_REPLACEMENT */
            case 0x90: return 0xfffd; /* WCHAR_REPLACEMENT */
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
            case 0x9d: return 0xfffd; /* WCHAR_REPLACEMENT */
            case 0x9e: return 0x017e;  /*  ž */
            case 0x9f: return 0x0178;  /*  Ÿ */
            default:
                break;
        };
    }

    return (wchar_t)ch;
}


/* -------------------------------------------------- */

static int win1252_to_utf8_seq(unsigned char *resp,char w1252)
{
    unsigned int ucs4;
    
    ucs4 = win1252_to_ucs4(w1252);
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


/* -------------------------------------------------- */

FILE *csv_file_desc(csv_t *csv)
{
    return (csv) ? csv->fp : NULL;
}

/* -------------------------------------------------- */

int csv_separator(csv_t *csv)
{
    return (csv) ? csv->sep : 0;
}

/* -------------------------------------------------- */

int csv_quote(csv_t *csv)
{
    return (csv) ? csv->quo : 0;
}

/* -------------------------------------------------- */

int csv_encoding(csv_t *csv)
{
    return (csv) ? csv->enc : 0;
}

/* -------------------------------------------------- */

const char **csv_header_list(csv_t *csv,int *count)
{
    if(csv) 
    {
        if(count)
            *count = csv->hdr_cols;
        return (const char **)csv->hdr_lst;
    }
    if(count)
        *count = 0;
    return NULL;
}

/* -------------------------------------------------- */

int csv_header_count(csv_t *csv)
{
    return (csv) ? csv->hdr_cols : 0;
}

/* -------------------------------------------------- */

const char *csv_header_name(csv_t *csv,int num)
{
    if(csv && csv->hdr_lst && num > -1 && num < csv->hdr_cols)
         return csv->hdr_lst[num];    
    return "";
}

/* -------------------------------------------------- */

int csv_row_cols(csv_t *csv)
{
    return (csv) ? csv->row_cols : 0;
}

/* -------------------------------------------------- */

const char **csv_row_list(csv_t *csv,int *count)
{
    if(csv) 
    {
        if(count)
            *count = csv->row_cols;
        return (const char **)csv->row_lst;
    }
    if(count)
        *count = 0;
    return NULL;
}

/* -------------------------------------------------- */

const char *csv_row_col_data(csv_t *csv,int num)
{
    if(csv && csv->row_lst && num > -1 && num < csv->row_cols)
         return csv->row_lst[num];    
    return "";
}

/* -------------------------------------------------- */

int csv_row_count(csv_t *csv)
{
    return (csv) ? csv->rows : 0;
}

/* -------------------------------------------------- */

static void csv_free_row(csv_t *csv)
{
    if(csv->row_lst)
    {
        int t;

        for(t=0;t<csv->row_cols;t++)
        {
            if(csv->row_lst[t])
            {
                free(csv->row_lst[t]);
                csv->row_lst[t] = NULL;
            }
        }
        free(csv->row_lst);
        csv->row_lst = NULL;
    }
    if(csv->row_len)
    {
        free(csv->row_len);
        csv->row_len = NULL;
    }
    csv->row_cols = 0;
    csv->row_max = 0;
}

/* -------------------------------------------------- */

static void csv_free_headers(csv_t *csv)
{
    if(csv->hdr_lst)
    {
        int t;

        for(t=0;t<csv->hdr_cols;t++)
        {
            if(csv->hdr_lst[t])
            {
                free(csv->hdr_lst[t]);
                csv->hdr_lst[t] = NULL;
            }
        }
        free(csv->hdr_lst);
        csv->hdr_lst = NULL;
    }
    csv->hdr_cols = 0;
}

/* -------------------------------------------------- */

void csv_destroy(csv_t *csv)
{
    if(csv)
    {
        if(csv->fp)
        {
            fclose(csv->fp);
            csv->fp = NULL;
        }

        csv_free_row(csv);
        csv_free_headers(csv);

        csv->pos  = 0;
        csv->rows = 0;
        csv->sep  = 0;
        csv->quo  = 0;
        csv->enc  = 0;

        free(csv);
    }
}

/* -------------------------------------------------- */

csv_t *csv_create()
{
    csv_t *csv;

    csv = (csv_t *)malloc(sizeof(csv_t) + CSV_MAX_BYTES + 4);
    if(csv)
    {
        memset(csv,0,sizeof(csv_t));
        csv->sep = ',';
        csv->quo = '\"';
        csv->enc = CSV_ENC_UTF8;
    }
    return csv;
}

/* -------------------------------------------------- */

int csv_set_options(csv_t *csv,char sep,char quo,int enc)
{
    if(csv && csv->fp == NULL)
    {
        int sep2,quo2;

        sep2 = (sep) ? sep : csv->sep;
        quo2 = (quo) ? quo : csv->quo;

        if(sep2 == quo2)
            return CSV_EBUG;

        csv->sep = sep2;
        csv->quo = quo2;

        switch(enc)
        {
            case CSV_ENC_UTF8:
            case CSV_ENC_W1252:
                csv->enc = enc;
                break;
            default:
                return CSV_EINVAL;
        }

        return CSV_OK;
    }
    return CSV_EINVAL;
}

/* -------------------------- */
/* -------------------------- */

static int csv_add_col(csv_t *csv,int tam)
{
    csv->field_buf[tam++]=0;

    if(csv->row_cols == csv->row_max)
    {
        char **dcols;
        int  *scols, ncols = 10;

        dcols = (char **)realloc(csv->row_lst,(csv->row_max + ncols) * sizeof(char **));
        if(dcols == NULL)
            return CSV_ENOMEM;

        scols = (int *)realloc(csv->row_len,(csv->row_max + ncols) * sizeof(int));
        if(scols == NULL)
        {
            free(dcols);
            return CSV_ENOMEM;
        }

        csv->row_lst = dcols;
        csv->row_len = scols;

        while(ncols > 0)
        {
            dcols[csv->row_max]=NULL;
            scols[csv->row_max++]=0;
            ncols--;
        }
    }

    if(tam > csv->row_len[csv->row_cols])
    {
        char *tmp;

        tmp = (char *)realloc(csv->row_lst[csv->row_cols],tam);
        if(tmp == NULL)
            return CSV_ENOMEM;

        csv->row_lst[csv->row_cols] = tmp;
        csv->row_len[csv->row_cols] = tam;
    }

    strcpy(csv->row_lst[csv->row_cols++],(char *)csv->field_buf);
    return CSV_OK;
}

/* -------------------------------------------------- */

static int csv_read_row(csv_t *csv,int maxsize)
{
    int ch, quoted,ignore,lf;
    int count,last,first,cr,prev;

    /* ignore controls if columns or entire lines have to be ignored
       because either the max number of columns or lines have been
       surpassed.
    */

    quoted = ignore = cr = FALSE;
    count = lf = 0;

    /* Automatic trimming */

    first = TRUE;  /* The first character has to be read */
    last = 0;      /* Where was the last separator seen (0 no separator seen) */
    prev = FALSE;  /* Was the previous char a quote? (for quote escaping) */

    /* Let's go... */

    csv->row_cols = 0;

    while(!feof(csv->fp))
    {
        ch = fgetc(csv->fp);
        if(ch == EOF)
        {
            if(!first)
            {
                if(last != 0)
                    count = last;
                return csv_add_col(csv,count);
            }
            return CSV_ENODATA;
        }

        /* Handle return carriage (allow \r\n and only \r as line feed) */

        if(ch == '\r')
        {
            prev = FALSE;
            if(cr)
            {
                ch = '\n';
                cr = FALSE;
            }
            else
            {
                cr = TRUE;
                continue;
            }
        }

        /* Handle new line */

        if(ch == '\n')
        {
            prev = FALSE;
            if(quoted)
            {
                if(!ignore)
                {
                    csv->field_buf[count]=(unsigned char)'\n';
                    if(++count == maxsize || ++lf == CSV_MAX_LN)
                        ignore=TRUE;
                }
                continue;
            }
            if(!first)
            {
                if(last != 0)
                    count = last;
               return csv_add_col(csv,count);
            }
            return CSV_OK;
        }

        /* Handle space when not a separator */

        if((ch == ' ' && csv->sep != ' ') || (ch == '\t' && csv->sep != '\t'))
        {
            prev = FALSE;
            if(!first && !ignore)
            {
                if(last == 0)
                    last = count;
                csv->field_buf[count]=(unsigned char)(ch & 0xff);
                if(++count == maxsize)
                    ignore=TRUE;
            }
            continue;
        }

        /* Handle quote */

        if(ch == csv->quo)
        {
            if(first)
            {
                first = FALSE;
                quoted = TRUE;
                continue;
            }

            if(quoted)
            {
                quoted = FALSE;
                prev   = TRUE;
                continue;
            }

            /* quote == FALSE */

            quoted = TRUE;

            if(prev == FALSE)
            {
                prev = TRUE;
                continue;
            }

            prev = FALSE;

            if(!ignore)
            {
                if(csv->enc == CSV_ENC_UTF8)
                {
                    csv->field_buf[count]=(unsigned char)(ch & 0xff);
                    if(++count == maxsize)
                        ignore=TRUE;
                    last = 0;
                }
                else if(csv->enc == CSV_ENC_W1252)
                {
                    count += win1252_to_utf8_seq(csv->field_buf + count,(char)(ch & 0xff));
                    if(count >= maxsize)
                        ignore=TRUE;
                    last = 0;
                }
            }
            continue;
        }

        /* Handle separator */

        if(ch == csv->sep)
        {
            if(!quoted)
            {
                int ret;

                if(last != 0)
                    count = last;
                ret = csv_add_col(csv,count);
                if( ret)
                    return ret;
                last = 0;
                first = TRUE;
                count = 0;
            }
            else if(!ignore)
            {
                if(csv->enc == CSV_ENC_UTF8)
                {
                    csv->field_buf[count]=(unsigned char)(ch & 0xff);
                    if(++count == maxsize)
                        ignore=TRUE;
                }
                else if(csv->enc == CSV_ENC_W1252)
                {
                    count += win1252_to_utf8_seq(csv->field_buf + count,(char)(ch & 0xff));
                    if(count >= maxsize)
                        ignore=TRUE;
                }
            }
            continue;
        }

        /* Handle the rest */
        if(!ignore)
        {
            if(csv->enc == CSV_ENC_UTF8)
            {
                csv->field_buf[count]=(unsigned char)(ch & 0xff);
                if(++count == maxsize)
                    ignore=TRUE;
            }
            else if(csv->enc == CSV_ENC_W1252)
            {
                count += win1252_to_utf8_seq(csv->field_buf + count,(char)(ch & 0xff));
                if(count >= maxsize)
                    ignore=TRUE;
            }
            first = FALSE;
            last = 0;
        }
    }

    return CSV_ENODATA;
}

/* -------------------------------------------------- */

static int csv_do_opening(csv_t *csv,int hdr)
{
    int ret = CSV_OK;
    unsigned char tmp[3];

    /* Check the BOM */

    if(fread(tmp,3,1,csv->fp)==1)
    {
        /* If there is a BOM, then encoding is UTF-8 */

        if(tmp[0]==0xef && tmp[1]==0xbb && tmp[2]==0xbf)
            csv->enc = CSV_ENC_UTF8;
        else
            fseek(csv->fp,-3,SEEK_CUR);
    }
    else
    {
        ret = CSV_EREAD;
    }

    while(ret == CSV_OK && hdr > 0)
    {
        ret = csv_read_row(csv,CSV_MAX_HEADER);
        if(ret == CSV_OK && --hdr == 0)
        {
            csv->hdr_lst = (char **)calloc(sizeof(char **),csv->row_cols);
            if(csv->hdr_lst)
            {
                int t;

                for(t=0;t < csv->row_cols && ret == CSV_OK;t++)
                {
                    csv->hdr_lst[t] = (char *)malloc(csv->row_len[t] + 1);
                    if(csv->hdr_lst[t])
                        strcpy(csv->hdr_lst[csv->hdr_cols++],csv->row_lst[t]);
                    else
                        ret = CSV_ENOMEM;
                }
            }
        }
    }

    if(ret != CSV_OK)
    {
        csv_close(csv);
        return ret;
    }

    csv->pos = ftello(csv->fp);
    return CSV_OK;
}

/* -------------------------------------------------- */

char **csv_read_next(csv_t *csv)
{
    if(csv && csv->fp)
    {
        int ret;

        ret = csv_read_row(csv,CSV_MAX_BYTES);
        if(ret == CSV_OK)
        {
            csv->rows++;
            return csv->row_lst;
        }
    }
    return NULL;
}

/* -------------------------------------------------- */

char **csv_rewind(csv_t *csv)
{
    if(csv && csv->fp)
    {
        fseeko(csv->fp,csv->pos,SEEK_SET);
        csv->rows = 0;
    }
    return csv_read_next(csv);
}

/* -------------------------------------------------- */

int csv_open(csv_t *csv,int hdr,const char *filename)
{
    if(filename && csv)
    {
        if(csv->fp)
            return CSV_EEXIST;
        csv->fp = fopen(filename,"r");
        if(csv->fp)
            return csv_do_opening(csv,hdr);
        return CSV_EOPEN;
    }
    return CSV_EINVAL;
}

/* -------------------------------------------------- */

int csv_openw(csv_t *csv,int hdr,const wchar_t *filename)
{
    if(filename && csv)
    {
        char tmp[1384] = {0};
    
        if(csv->fp)
            return CSV_EEXIST;
    printf("Aqui 1\n");
        if( wcstombs(tmp,filename,1383) > 0)
        {
            csv->fp = fopen(tmp,"r");
printf("Aqui 2\n");            
            if(csv->fp)
                return csv_do_opening(csv,hdr);
printf("Aqui 3\n");                
        }
        return CSV_EOPEN;
    }
    return CSV_EINVAL;
}

/* -------------------------------------------------- */

void csv_close(csv_t *csv)
{
    if(csv)
    {
        if(csv->fp)
        {
            fclose(csv->fp);
            csv->fp = NULL;
        }

        csv->pos = 0;
        csv->rows = 0;

        csv_free_headers(csv);

        csv->row_cols = 0;
    }
}

