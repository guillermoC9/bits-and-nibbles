/*
    test_pem.c

    Test of entropy routines

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

#include "pem.h"
#include <termios.h>

size_t read_pass(char *passwd, size_t max)
{
    size_t len = 0;
    if(passwd)
    {
        if(max > 1)
        {
            int fd,ch;
            struct termios ot,nt;

            fd = fileno (stdin);

            /* Turn echoing off and fail if we can't. */
            if (!tcgetattr (fd, &ot))
            {
                nt = ot;
                nt.c_lflag &= ~ECHO;
                if (!tcsetattr (fd,TCSAFLUSH, &nt))
                {
                   /* Read the password. */
                   max--;
                   while(len < max)
                   {
                        ch = fgetc(stdin);
                        if(ch=='\n' || ch=='\r')
                            break;
                        passwd[len++]=(char)ch;
                   }
                }
                tcsetattr(fd,TCSAFLUSH,&ot);
            }
        }
        passwd[len]=0;
    }
    return len;
}

/* -------------------------------------- */

void print_bytes(char *prefix, void *d, size_t n)
{
    size_t t;
    unsigned char *dat;

    dat = (unsigned char *)d;

    if (prefix)
        printf("%s=", prefix);

    if (d)
    {
        for (t = 0; t < n; t++)
            printf("%02x", dat[t]);
        printf("\n");
    }
    else
    {
        printf("NULL\n");
    }
}

/* -------------------------------------- */

static void pem_to_text(char *filename)
{
    pem_elem_t  *elem;
    pem_file_t  *pm;
    int ret;
    char passwd[128],*pwd = NULL;

    ret = pem_open(&pm,filename);
    if(ret)
    {
        printf("Cannot load: %s (errro %d)\n",filename,ret);
        return;
    }

    printf("\n*** READING PEM %s ***\n",filename);

    elem = pem_get_first_element(pm);
    while(elem)
    {
        printf("\n----- %s -----\n\n",pem_element_name(elem->elem));

        if(elem->elem==PEM_ENC_PRIVATE_KEY || elem->flags & PEM_F_CRYPT)
        {
            printf("Encrypted, please type a password:\n");                
            if(read_pass(passwd,128)==0)
            {
                printf("You need a password for decrypting %s. Aborting",filename);                
                return;
            }
            pwd = passwd;
        }

        ret = pem_decode_element(elem,pwd);
        if(ret == 0 && elem->flags & PEM_F_ASN1)
            asn1_to_text(stdout,0,elem->data,elem->size);
        else
            printf("error %d decoding PEM Element\n",ret);
        elem = pem_get_next_element(pm);
    }
    pem_close(pm);
}

/* -------------------------------------- */

int main(int argc, char **argv)
{
    int t;

    if(argc < 1)
    {
        printf("\nusage: test_pem file1 [file2] [file3]\n\n"
               "    fileX       PEM file encapsulating ASN.1 to show\n\n"                     
              );   
    }
    else
    {
        for(t=1;t<argc;t++)
            pem_to_text(argv[t]);
    }

    return 0;
}