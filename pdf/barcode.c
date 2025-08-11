/*
    barcode.c

    Barcode Generation Routines

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

        - Written by Guillermo Amodeo Ojeda using the info from:

          https://en.wikipedia.org/wiki/International_Article_Number

        - This code allows to build a string of bits that can be used to draw
          a barcode either in text or graphics

                                --oO0Oo--

*/


#include "barcode.h"

/* L,G and R codes for EAN-12, EAN-8 and EAN-5 */

static int L_Code[10] =
    {
        0x0D, /* 0 = 0001101 */
        0x19, /* 1 = 0011001 */
        0x13, /* 2 = 0010011 */
        0x3D, /* 3 = 0111101 */
        0x23, /* 4 = 0100011 */
        0x31, /* 5 = 0110001 */
        0x2F, /* 6 = 0101111 */
        0x3B, /* 7 = 0111011 */
        0x37, /* 8 = 0110111 */
        0x0B  /* 9 = 0001011 */
    };

static int G_Code[10] = /* Reversed version of R_Codes */
    {
        0x27, /* 0 = 0100111 */
        0x33, /* 1 = 0110011 */
        0x1B, /* 2 = 0011011 */
        0x21, /* 3 = 0100001 */
        0x1D, /* 4 = 0011101 */
        0x39, /* 5 = 0111001 */
        0x05, /* 6 = 0000101 */
        0x11, /* 7 = 0010001 */
        0x09, /* 8 = 0001001 */
        0x17  /* 9 = 0010111 */
    };

static int R_Code[10] = /* NOT of L_Codes */
    {
        0x72, /* 0 = 1110010 */
        0x66, /* 1 = 1100110 */
        0x6C, /* 2 = 1101100 */
        0x42, /* 3 = 1000010 */
        0x5C, /* 4 = 1011100 */
        0x4E, /* 5 = 1001110 */
        0x50, /* 6 = 1010000 */
        0x44, /* 7 = 1000100 */
        0x48, /* 8 = 1001000 */
        0x74  /* 9 = 1110100 */
    };

/* Pattern strings */

static char EAN13_pattern[10][7] =
    {
        "LLLLLL",
        "LLGLGG",
        "LLGGLG",
        "LLGGGL",
        "LGLLGG",
        "LGGLLG",
        "LGGGLL",
        "LGLGLG",
        "LGLGGL",
        "LGGLGL"
    };

static char EAN5_pattern[10][6] =
    {
        "GGLLL",
        "GLGLL",
        "GLLGL",
        "GLLLG",
        "LGGLL",
        "LLGGL",
        "LLLGG",
        "LGLGL",
        "LGLLG",
        "LLGLG"
    };

static char EAN2_pattern[4][3] =
    {
        "LL",
        "LG",
        "GL",
        "GG",
    };


/* ------------------------------ */

static int check_digit(const char *bc,int len,int check)
{
    int t,sum = 0,val;

    for(t=0;t<len;t++)
    {
        if(t % 2)
            val = (3 * ((int)(bc[t] - '0')));
        else
            val = (int)(bc[t] - '0');
        sum += val;
    }
    val = (sum % 10);
    if(val)
        val = (10 - val);

    if(check)
        return (val == ((int)(bc[len] - '0'))) ? 0 : -1;
    return val;
}

/* ------------------------------ */

int format_ean_13(char *resp,size_t max,const char *bc)
{
    int digits,t,st;

    if(!resp || !bc || max < 14)
        return -1;
    
    digits = strlen(bc);
    if(digits > 12)
    {
        strlcpy(resp,bc,14);
        if(check_digit(resp,12,TRUE))
            return -3;
    }
    else
    {
        st = 12 - digits;
        for(t=0;t<st;t++)
            resp[t]='0';
        memcpy(resp + t,bc,digits);
        resp[12] = (char)('0' + check_digit(resp,12,FALSE));
        resp[13] = 0;
    }
    return 0;
}

/* ------------------------------ */

int get_ean_13(unsigned char *resp,size_t max,const char *bc)
{
    char code[14];
    int *dig,t,ret,pos,i,msk,val;
    char *pattern;

    if(max < 12)
        return -1;

    ret = format_ean_13(code,14,bc);
    if(ret)
        return ret;

    memset(resp,0,12);

    /* Prefix */

    mem_set_bit(resp,12,94,1);
    mem_set_bit(resp,12,92,1);

    /* Separator */

    mem_set_bit(resp,12,48,1);
    mem_set_bit(resp,12,46,1);

    /* Suffix */

    mem_set_bit(resp,12,2,1);
    mem_set_bit(resp,12,0,1);

    pattern = EAN13_pattern[(int)(code[0] - '0')];

   t=i=0;
   pos=91;
   while(t < 6)
   {
        dig = (pattern[t] == 'L') ? L_Code : G_Code;
        val = dig[(code[++t] - '0')];
        msk = 0x40;
        while(msk)
        {
            if(val & msk)
                mem_set_bit(resp,12,pos,1);
            msk >>= 1;
            pos--;
        }
    }

   for(pos=44,i=0;i < 6;i++)
   {
        val = R_Code[(code[++t] - '0')];
        msk = 0x40;
        while(msk)
        {
            if(val & msk)
                mem_set_bit(resp,12,pos,1);
            msk >>= 1;
            pos--;
        }
    }

    return 0;
}



/* ------------------------------ */

int format_ean_8(char *resp,size_t max,const char *bc)
{
    int digits,t,st;

    if(!resp || !bc || max < 9)
        return -1;

    digits = strlen(bc);
    if(digits > 7)
    {
        strlcpy(resp,bc,9);
        if(check_digit(resp,7,TRUE))
            return -3;
    }
    else
    {
        st = 7 - digits;
        for(t=0;t<st;t++)
            resp[t]='0';
        memcpy(resp + t,bc,digits);
        resp[7] = (char)('0' + check_digit(resp,7,FALSE));
        resp[8] = 0;
    }
    return 0;
}

/* ------------------------------ */

int get_ean_8(unsigned char *resp,size_t max,const char *bc)
{
    char code[9];
    int ret,t,pos,msk,val;

    if(max < 9)
        return -1;

    ret = format_ean_8(code,9,bc);
    if(ret)
        return ret;

    memset(resp,0,9);

    /* Prefix */

    mem_set_bit(resp,9,66,1);
    mem_set_bit(resp,9,64,1);

    /* Separator */

    mem_set_bit(resp,9,34,1);
    mem_set_bit(resp,9,32,1);

    /* Suffix */

    mem_set_bit(resp,9,2,1);
    mem_set_bit(resp,9,0,1);


   t=0;
   pos=63;
   while(t < 4)
   {
        val = L_Code[(code[t++] - '0')];
        msk = 0x40;
        while(msk)
        {
            if(val & msk)
                mem_set_bit(resp,9,pos,1);
            msk >>= 1;
            pos--;
        }
    }

    pos=30;
    while(t < 8)
    {
        val = R_Code[(code[t++] - '0')];
        msk = 0x40;
        while(msk)
        {
            if(val & msk)
                mem_set_bit(resp,9,pos,1);
            msk >>= 1;
            pos--;
        }
    }

    return 0;
}

/* ------------------------------ */

int get_ean_5(unsigned char *resp,size_t max,const char *bc)
{
    int *dig,t,sum,pos,val,msk;
    int bits[5] = {42,33,24,15,6};
    char *pattern;

    if(max < 6 || strlen(bc) < 5)
        return -1;

    for(t=0;t<5;t++)
    {
        if(!isdigit(bc[t]))
            return -3;
    }

    memset(resp,0,6);

    /* Prefix */

    mem_set_bit(resp,6,46,1);
    mem_set_bit(resp,6,44,1);
    mem_set_bit(resp,6,43,1);

    /* Separators */

    mem_set_bit(resp,6,34,1);
    mem_set_bit(resp,6,25,1);
    mem_set_bit(resp,6,16,1);
    mem_set_bit(resp,6,7,1);

    sum =  3 * (bc[0] - '0');
    sum += 9 * (bc[1] - '0');
    sum += 3 * (bc[2] - '0');
    sum += 9 * (bc[3] - '0');
    sum += 3 * (bc[4] - '0');

    pattern = EAN5_pattern[(sum % 10)];

    for(t=0;t<5;t++)
    {
        dig = (pattern[t] == 'L') ? L_Code : G_Code;
        val = dig[(bc[t] - '0')];
        pos = bits[t];
        msk = 0x40;
        while(msk)
        {
            if(val & msk)
                mem_set_bit(resp,6,pos,1);
            msk >>= 1;
            pos--;
        }
    }
    return 0;
}

/* ------------------------------ */

int get_ean_2(unsigned char *resp,size_t max,const char *bc)
{
    int *dig,t,pos,val,msk;
    char *pattern;

    if(max < 3 || strlen(bc) < 2 || !isdigit(bc[0]) || !isdigit(bc[1]))
        return -1;

    pattern = EAN2_pattern[(atoi(bc) % 4)];

    memset(resp,0,3);

    /* Prefix */

    mem_set_bit(resp,3,19,1);
    mem_set_bit(resp,3,17,1);
    mem_set_bit(resp,3,16,1);

    /* Separator */

    mem_set_bit(resp,3,7,1);

    for(t=0;t<2;t++)
    {
        dig = (pattern[t] == 'L') ? L_Code : G_Code;
        val = dig[(bc[t] - '0')];
        pos = (t) ? 6 : 15;
        msk = 0x40;
        while(msk)
        {
            if(val & msk)
                mem_set_bit(resp,3,pos,1);
            msk >>= 1;
            pos--;
        }
    }
    return 0;
}



