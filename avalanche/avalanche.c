/*
    avalanche.c

    Pa55w0rd Avalanche algorithm

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

        - Designed and written by Guillermo Amodeo Ojeda.


        - Pa55w0rd Avalanche is an algorithm I invented to convert a small 
          word or sentence to a huge string of random characters in order 
          to be able to use passwords that you can easily remember instead
          of using random chars strings, which you have to write down and
          hide somewhere. To me this is the problem with password these 
          days: THEY ARE TOO COMPLEX TO REMEMBER.
          
        - My algorithm works by generating an avalanche of bits from hashing 
          64 bytes of random data generated using a pseudo-random generator,
          chosen by the first four bytes of the key, and then using these 
          same four bytes to seed the generator. 

          This 64 bytes are then treated as the first temporary hash, which 
          the algorithm repeatedly hash together with the original word and
          convert the result to chars in the 'avalanche alphabet'. It then 
          repeats this process until enough chars are obtained, reusing the 
          64 bytes of the resulting hash as the temporary hash for the next
          step of the process.
          
          If you feed a word like 'test' to this algorithm, you would obtain 
          the following passwords depending on the required size, which is 
          shown beside them:

            12 p=t32_C!)HI!#
            20 P3#b^w$kGRa7VPoox@tPZ
            35 No8R=#HQHr@DoM^!~2cx9[$Td|_vwoxd{N4A
            55 VgkoxBH)d12}r@J}M#Y/tREXU+Iu5t!B#G{!6oP(QtNIeeG2f(&jeDb@

          The generated passwords not only fit all the chars requirements, 
          they can be as long as you want to increase the difficulty of 
          a brute-force attack.
          
          The algorithm is deterministic, so the same word/sentence will 
          generate the same long password, but only for the same ouput size. 
          If you change the size, the password will also change.
          
          Therefore, an attacker could try a brute-force attack on your long 
          password using the algorithm to generate them, for example trying 
          by avalanching all the words of a dictionary. The thing is that they
          cannot be sure of which is the correct password so easily, as there
          is an extra variable in there: you can choose the length of the 
          output, which will output a different password from he same word, 
          so the attacker also needs to find out what is the size you used in 
          order to find out which of the generated passwords is the correct.                    


                                --oO0Oo--
*/

#include "avalanche.h"

/* ------------------------------- */
/* ------------------------------- */

static char *chr_avalanche="ABCDEFGHIJKLMNOPQRSTUVWXYZ/^+[]{}()#@0123456789$!@#^!&x_-=~!|abcdefghijklmnoprstwuvwxyz@$";

char *pa55w0rd_avalanche(const char *orig,char *dest,size_t max)
{
    unsigned int seed;
    size_t t,len,pos,cnt;
    rand_t *rc;
    sha512_t ctx;
    unsigned char tmp[SHA512_SIZE];
    int prg[3]={RAND_MT,RAND_GX,RAND_GM};

    if(!orig || !dest)
        return NULL;

    if(max > 0)
        max--;
    
    /* Use the first 4 bytes of the original password to select and seed the PRG */

    len = strlen(orig);
    if(len > 1)
        seed = orig[0];

    for(t=1; t<4 && t<len; t++)
    {
        seed <<= 8;
        seed |= orig[t];
    }

    rc = rand_start(prg[ seed % 3 ],seed);
    if(!rc)
        return NULL;

    /* Start the avalanche */

    for(pos=t=0;t<(SHA512_SIZE / 4);t++)
    {   
        do {     
           seed = rand_unsigned(rc);
        } while(!seed);        

        tmp[pos++] = (seed >> 24) & 0xff;
        tmp[pos++] = (seed >> 16) & 0xff;
        tmp[pos++] = (seed >>  8) & 0xff;
        tmp[pos++] = (seed >>  0) & 0xff;
    }         

    rand_end(rc);    

    cnt = 0;
    while(cnt < max)
    {
        /* Generate the SHA-512 base password */

        sha512_init(&ctx);
        sha512_update(&ctx,orig,len);
        sha512_update(&ctx,tmp,SHA512_SIZE);
        sha512_update(&ctx,tmp,(max % SHA512_SIZE));
        sha512_final(&ctx,tmp);

        /* Now convert it to the avalanche alphabet */
        
        pos = 0;
        while (cnt < max && pos < SHA512_SIZE)
            dest[cnt++] = chr_avalanche[ tmp[pos++] % 89 ];
    }
    dest[cnt] = 0;
    return dest;
}
