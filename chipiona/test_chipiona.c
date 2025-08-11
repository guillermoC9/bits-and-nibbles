/*
    test_chipiona.c

    Test of chipiona cipher routines

    (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Written by Guillermo Amodeo Ojeda.                  
            
                        --oO0Oo--
*/

#include "chipiona.h"

/* --------------- */

static void print_data(char *prefix,void *d,size_t n)
{
    size_t t,w,l = 0;
    unsigned char *dat=(unsigned char *)d;

    if(prefix)
    {
        printf("%s :",prefix);
        l = strlen(prefix) + 2;
    }

    if(d)
    {        
        for(w=t=0;t<n;t++)
        {
            printf("%02x",dat[t]);
            if(++w == 32)
            {
                printf("\n");
                for(w=0;w<l;w++)
                    printf(" ");
                w=0;
            }
            else if (w==16)
                printf(" ");
        }
        printf("\n");
    }
    else
    {
        printf("NULL\n");
    }
}

#ifndef FALSE

#define FALSE 0
#define TRUE  1

#endif

/* --------------- */

static void test_chipiona()
{
    chipiona_t ctx1,ctx2;
    int ret;
    unsigned char cipher[160];
    unsigned char k[64] =
    {
       0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
       0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
       0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    };
    
    unsigned char plain[160] =
    {        
        0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb, 
        0x11, 0x11, 0x11, 0x11, 0xcc, 0xcc, 0xcc, 0xcc, 0x11, 0x11, 0x11, 0x11, 0xdd, 0xdd, 0xdd, 0xdd,
        0x22, 0x22, 0x22, 0x22, 0xaa, 0xaa, 0xaa, 0xaa, 0x22, 0x22, 0x22, 0x22, 0xbb, 0xbb, 0xbb, 0xbb,
        0x22, 0x22, 0x22, 0x22, 0xcc, 0xcc, 0xcc, 0xcc, 0x22, 0x22, 0x22, 0x22, 0xdd, 0xdd, 0xdd, 0xdd,
        0x33, 0x33, 0x33, 0x33, 0xaa, 0xaa, 0xaa, 0xaa, 0x33, 0x33, 0x33, 0x33, 0xbb, 0xbb, 0xbb, 0xbb,
        0x33, 0x33, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xcc, 0x33, 0x33, 0x33, 0x33, 0xdd, 0xdd, 0xdd, 0xdd,
        0x44, 0x44, 0x44, 0x44, 0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 0xbb, 0xbb, 0xbb, 0xbb,
        0x44, 0x44, 0x44, 0x44, 0xcc, 0xcc, 0xcc, 0xcc, 0x44, 0x44, 0x44, 0x44, 0xdd, 0xdd, 0xdd, 0xdd,
        0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xbb, 0xbb, 0xbb, 0xbb, 
        0x55, 0x55, 0x55, 0x55, 0xcc, 0xcc, 0xcc, 0xcc, 0x55, 0x55, 0x55, 0x55, 0xdd, 0xdd, 0xdd, 0xdd         
    };

    printf("*** Chipiona Cipher Test ***\n\n");

    print_data("Key ",k,64);

    printf("\n");

    ret = chipiona_init(&ctx1,k,64);
    if(ret)
    {
        printf("ERROR %d initializing chipiona cipher\n",ret);
        return;
    }
    ret = chipiona_init(&ctx2,k,64);
    if(ret)
    {
        chipiona_end(&ctx1);
        printf("ERROR %d initializing chipiona cipher\n",ret);
        return;
    }

    chipiona_dump_data(&ctx1,stdout,TRUE);    
    chipiona_dump_data(&ctx2,stdout,FALSE);

    printf("\n");
    
    print_data("PlainText1",plain,160);
    chipiona_encode(&ctx1,cipher,plain,160);
    print_data("CipherText",cipher,160);
    chipiona_decode(&ctx2,cipher,cipher,160);
    print_data("PlainText2",cipher,160);

    printf("\n");

    chipiona_dump_data(&ctx1,stdout,FALSE);    
    chipiona_dump_data(&ctx2,stdout,FALSE);

    printf("\n");
    
    memset(cipher,0,160);
    print_data("PlainText1",cipher,73);
    chipiona_encode(&ctx1,cipher,cipher,73);
    print_data("CipherText",cipher,73);
    chipiona_decode(&ctx2,cipher,cipher,73);
    print_data("PlainText2",cipher,73);

    printf("\n");

    chipiona_dump_data(&ctx1,stdout,FALSE);    
    chipiona_dump_data(&ctx2,stdout,FALSE);

    printf("\n");
    chipiona_end(&ctx1);
    chipiona_end(&ctx2);
}



int main()
{    
    test_chipiona();
    return 0;
}
