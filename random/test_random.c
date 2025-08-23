/*
    test_random.c

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

#include "random.h"

int verbose=FALSE;      

/** Entry on the table */

typedef struct _hte_
{
    unsigned int    num;        /* Random Number */
    unsigned int    rep;        /* Repetitions of this number */
    struct _hte_   *next;       /* Next collision */
} ht_entry_t; 


ht_entry_t **table=NULL;     /* Table of numbers */

unsigned int max=0;

int total=0;        /* Total number of entries in table (counting collissions) */
int repeated=0;     /* Total number of repetions */
int repeated1=0;    /* Number of repetions one */
int repeated2=0;    /* Number of repetions twice */
int repeated3=0;    /* Number of repetions thrice */
int generated=0;    /* Total numbers generated */

/* -------------------------------------- */

int table_set_entry(unsigned int num)
{
    unsigned int pos;
    ht_entry_t *cur,*hdr;
    
    pos = (num % max);
    cur = hdr = table[pos];
    while (cur)
    {
        if(cur->num == num)
        {
            cur->rep++;
            return cur->rep + 1;            
        }
        cur = cur->next;
    }
    cur = (ht_entry_t *)malloc(sizeof(ht_entry_t));
    if(cur)
    {
        cur->num = num;
        cur->rep = 0;
        cur->next = hdr;
        table[pos] = cur;
        total++;
        return 1;
    }
    return 0;
}

/* -------------------------------------- */

void table_show_rep_and_free_collisions(void)
{
    unsigned int pos = 0,show;
    ht_entry_t *cur,*del;
    
    while(pos < max)
    {
        cur = del = table[pos++];
        while (cur)
        {
            show = 0;
            switch(cur->rep)
            {
                case 1: 
                    show = verbose;                    
                    repeated1++;
                    break;
                case 2: 
                    show++;
                    repeated2++;
                    break;
                case 3:    
                    show++;                 
                    repeated3++;                    
                    break;
                default:
                    break;
            }
            if(show)
                printf("0x%08x repeated %d time(s)\n",cur->num,cur->rep);
            del = cur;
            cur = cur->next;
            free(del);
        }        
    }
}

/* -------------------------------------- */

void test_random(int alg,int seed,int count)
{
    rand_t *rc = NULL;
    stopwatch_t sw;
    unsigned int value;
    size_t tmp;    
    double secs;

    printf("\n*** Test RANDOM ***\n\n");

    table = (ht_entry_t **)calloc(sizeof(ht_entry_t),count);
    if(table == NULL)
    {
        printf("Error creating hash table\n");
        return;
    }
    max = (unsigned int) count;
    rc = rand_start(alg,seed);
    if (rc)
    {
        printf("Generate %d numbers using %s\n", count, rand_name(rc));

        stopwatch_start(&sw);

        while (total < count)
        {
            generated++;

            value = rand_unsigned(rc);

            tmp = table_set_entry(value);
            if(tmp == 0)
            {
                printf("Error creating hash table entry\n");
                break;
            }
            else if (tmp > 1)
                repeated++;            
        }

        stopwatch_stop(&sw);

        secs = stopwatch_elapsed(&sw);

        rand_end(rc);

        printf("Done!\nCounting repetitions...\n");        

        table_show_rep_and_free_collisions();

        free(table);

        printf("\nTotal of numbers    : %d\n", total);        
        printf("\nTotal of tries      : %d\n", generated);
        printf("Total of repeats    : %d\n", repeated);   
        printf("Number of 1 repeats : %d\n", repeated1);        
        printf("Number of 2 repeats : %d\n", repeated2);           
        printf("Number of 3 repeats : %d\n", repeated3);           
        printf("Generation time     : %.3f seconds\n\n", secs);
        printf("Note: 1 repeat means the same number was generated twice\n\n");

    }
    else
    {
        printf("Error initializing random context with %d\n",alg);
    }
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

/* ------------------------------------------------------ *
   Official Vectors to Test the TLS PRF
 * ------------------------------------------------------ */

static void test_tls_prf()
{
    unsigned char res[64],
        secr10[48]={REPEAT_32(0xab),REPEAT_16(0xab)},
        seed10[64]={REPEAT_64(0xcd)},
        secr12[16]={0x9b ,0xbe ,0x43 ,0x6b ,0xa9 ,0x40 ,0xf0 ,0x17 ,0xb1 ,0x76 ,0x52 ,0x84 ,0x9a ,0x71 ,0xdb ,0x35},
        seed12[16]={0xa0 ,0xba ,0x9f ,0x93 ,0x6c ,0xda ,0x31 ,0x18 ,0x27 ,0xa6 ,0xf7 ,0x96 ,0xff ,0xd5 ,0x19 ,0x8c};    

    printf("\n*** Test PRF ***\n\n");

    printf("tls_test_prf(REAL) -> TLSv1.0 (MD5^SHA1) = b5baf4722b91851a8816d22ebd8c1d8cc2e94d55\n");

    rand_tls_prf(RAND_TLS_MD5_SHA1,"secret",6,"label","seed",4,NULL,0,res,20);

    print_bytes("tls_test_prf(CALC) -> TLSv1.0 (MD5+SHA1)",res,20);

    rand_tls_prf(RAND_TLS_MD5_SHA1,secr10,48,"PRF Testvector",seed10,64,NULL,0,res,32);

    printf("tls_test_prf(REAL) -> TLSv1.1 (MD5^SHA1) = D3D4D1E349B5D515044666D51DE32BAB258CB521B6B053463E354832FD976754\n");
    print_bytes("tls_test_prf(CALC) -> TLSv1.1 (MD5^SHA1)",res,32);

    rand_tls_prf(RAND_TLS_SHA256,secr12,16,"test label",seed12,16,NULL,0,res,32);

    printf("tls_test_prf(REAL) -> TLSv1.2 (SHA-256)  = e3f229ba727be17b8d122620557cd453c2aab21d07c3d495329b52d4e61edb5a\n");
    print_bytes("tls_test_prf(CALC) -> TLSv1.2 (SHA-256) ",res,32);

    printf("\n");

}

/* -------------------------------------- */

void get_bits_test(int bits)
{
    rand_t *rc = NULL;
    unsigned char bytes[8];
    char ch[66]={0};    
    int bit,rnd,byt,msk;
    u64_t value;
    
    if(bits == 0)
        bits = 53;

    printf("\n*** Test RANDOM bits (seed = 1234) ***\n\n");

    if(bits < 2 || bits > 64)
    {
        bits = 53;
        printf("Bits out of range 2-64. Setting as default 53\n\n");
    }
    else
    {
        printf("Using %d bits\n\n",bits);
    }

    for(rnd=RAND_NONE+1;rnd<RAND_NUM;rnd++)
    {
        rc = rand_start(rnd,1234);
        if (rc)
        {
            rc = rand_start(rnd,1234);
            if (rc)
            {
                memset(bytes,0,8);

                byt = ((bits + 7) / 8);                
                rand_bits(rc,bytes + (8 - byt),bits);                
                
                bit = byt = 0;
                msk = 0x80;
                
                while(bit < 65)
                {                    
                    if(bit==32)
                        ch[bit++] = ' ';
                    ch[bit++] = (bytes[byt] & msk) ? '1' : '0';
                    msk >>= 1;
                    if(msk == 0)
                    {
                        msk=0x80;
                        byt++;
                    }
                }
                value = get_be64(bytes);
                printf("%d bits : %s ",bits,ch); 
                printf("(0x%08x%08x)",((unsigned int)((value >> 32) & 0xffffffff)),((unsigned int)(value & 0xffffffff)));
                printf(" PRF: %s\n",rand_name(rc));            
                rand_end(rc);
            }
            
        }
    }
    printf("\n");
}

/* -------------------------------------- */

#define AUTOID_SIZE     20

void AutoID_test(void)
{
    char id[AUTOID_SIZE + 1];
    int t;

    printf("\n*** JavaScript AutoID Test ***\n\n");
    for(t = 0;t<AUTOID_SIZE;t++)
        printf("firestore_auto_id() = %s\n",firestore_auto_id(id,AUTOID_SIZE + 1));
    printf("\n");
}

/* -------------------------------------- */

void test_entropy(void)
{
    unsigned char data[64];
    int t;
    char str[256];

    printf("**** Test hash_get_entropy() ****\n\n");

    for(t=HASH_NONE+1; t< HASH_NUM_HASHES;t++)
    {        
        snprintf(str,256,"hash_get_entropy(%s)",hash_name(t));
        hash_get_entropy(t,data,64);
        print_bytes(str, data,64);
    }
    printf("\n");
}

/* -------------------------------------- */

void usage(void)
{
    printf("\nTEST RANDOM\n\nuse:\ttest_random [-v] alg seed num\n\n"
           "Generate 'num' different random numbers using 'alg' algorithm\n"
           "counting repetitions and showing the numbers repeated more \n"
           "than once, as well as the total number of tries.\n\n"
           "'seed' is a number used to seed the algorithm. Unless is 0, which\n"
           "makes the algorithm to be seeded with an umpredictable number.\n\n"
           "Algorithms:\n\n\t1 = Mother of George Marsaglia (RAND_GM)\n"
           "\t2 = Mersenne Twister of Matsumoto & Nishimura (RAND_MT)\n"
           "\t3 = Xorshift of George Marsaglia (RAND_GX)\n"
           "\t4 = TLS-PRF-MD5_SHA1 (RAND_TLS_MD5_SHA1)\n"
           "\t5 = TLS-PRF-SHA256 (RAND_TLS_SHA256)\n"
           "\t6 = TLS-PRF-SHA384 (RAND_TLS_SHA384)\n"
           "\t7 = OSSYS of Kenneth MacKay (RAND_OS)\n"
           "\n"
        );
    printf("options:\n\t-v = Verbose. Would also show the numbers repeated once\n\n");
    printf("\tSet 'alg' as 'test' and 'seed' as a number of bits instead for a quick test.\n\n");

}


/* -------------------------------------- */

int main(int argc,char **argv)
{
    int total = 0;
    int arg = 1,alg = 0, seed;    

    if(argc > 1)
    {
        if(!strcmp(argv[arg],"-v"))
        {
            verbose = TRUE;
            arg++;
        }

        if(!strcmp(argv[arg],"test"))
        {
            AutoID_test();
            get_bits_test( (++arg < argc) ? atoi(argv[arg]) : 0);
            test_tls_prf();
            test_entropy();
            return 0;
        }

        if(argc > 3)
        {
            alg = atoi(argv[arg++]);
            seed = atoi(argv[arg++]);
            total = atoi(argv[arg]);
        }
    }

    if(alg < 1 || alg > 7 || total < 1)
        usage();
    else
        test_random(alg,seed,total);

    return 0;
}
