/*
    test_sort.c

    Test of sorting and searching routines

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

#include "ticks.h"

#define MAX_WORDS   4000

/* --------------- */

typedef struct {
    int   len;
    char  str[1];
} word_t;


word_t *words_list[MAX_WORDS];
int     words_count=0;

word_t *words_halved[MAX_WORDS];
word_t *words_reversed[MAX_WORDS];
word_t *words_unsorted[MAX_WORDS];

word_t *words_tmp[MAX_WORDS];

/* --------------- */

#define copy_words(_f)    memcpy(words_tmp,_f,sizeof(words_tmp));

/* --------------- */

static void save_words(const char *name,word_t *words[MAX_WORDS])
{
    int i;
    FILE *fp;

    fp = fopen(name,"w");
    if(fp)
    {
        for(i=0;i<words_count;i++)
            fprintf(fp,"%s\n",words[i]->str);
        fflush(fp);
        fclose(fp);
    }    
}

/* --------------- */

static void free_words(void)
{
    int t;

    for(t=0;t<words_count;t++)
        free(words_list[t]);
    words_count = 0;
}

/* --------------- */

static int add_word(char *word)
{
    size_t len = 0;
    char *ptr; 

    /* Jump over spaces */   

    while(isspace(*word))
        word++;

    /* Count size */

    for(ptr=word;!isspace(*ptr);ptr++)
        len++;

    if(len)
    {
        words_list[words_count] = (word_t *)calloc(sizeof(word_t) + len,1);
        if(words_list[words_count]==NULL)
            return 0; /* Memory error is same as full list */
        words_list[words_count]->len = len;
        memcpy(words_list[words_count]->str,word,len);
        if(++words_count == MAX_WORDS)
            return 0;  /* Full list */
    }
    return 1;
}

/* --------------- */

static void load_words(void)
{
    char word[1024];
    FILE *fp;
    int i,t;

    fp = fopen("words.db","r");
    if(fp == NULL) 
    {
        printf("ERROR: Cannot read words database\n");
        return;
    }
    while(fgets(word,1023,fp))
    {
        if(!add_word(word))
        {
            printf("ERROR: %s\n",(words_count == MAX_WORDS) ? "List full" : "Not enough memory");
            break;
        }
    }
    fclose(fp);
    printf("Loaded %d words\n",words_count);

    save_words("words_list.txt",words_list);

    /* Build halfed list */

    t=words_count/2;
    for(i=0;t<words_count;i++)
        words_halved[i] = words_list[t++];       

    t=0;
    while(t<words_count/2)
        words_halved[i++] = words_list[t++];       

    save_words("words_halfed.txt",words_halved);

    /* Build reversed list */

    t=words_count;
    for(i=0;i<words_count;i++)
        words_reversed[i] = words_list[--t];       
    
    save_words("words_reversed.txt",words_reversed);

    /* Build unsorted list */

    memset(words_unsorted,0,sizeof(char *) * words_count);

    for(i=0;i<words_count;i++)
    {        
        t = (rand() % words_count);        
        while(words_unsorted[t])
        {
            if(t < words_count / 2)
            {
                if(--t < 0)
                    t = (rand() % words_count);        
            }
            else
            {
                if(++t >= words_count)
                    t = (rand() % words_count); 
            }
        }
        words_unsorted[t] = words_list[i];    
    }    

    save_words("words_unsorted.txt",words_unsorted);

    return;
}

/* -------------------------------------- */
/* -------------------------------------- */

int changes = 0;
int compares = 0;

/* -------------------------------------- *
   Returns if a word is alphabetically 
   smaller, equal or bigger than another 
   if bigger then a change is counted as
   it will happen
 * -------------------------------------- */

int compare_str(const void *a,const void *b)
{
    word_t *wa,*wb;

    wa = *((word_t **)a);
    wb = *((word_t **)b);

    compares++;

    return strcmp(wa->str,wb->str);
}

/* -------------------------------------- *
   Returns if a word is longer than the
   other, and if equal, then it returns
   if alphabetically smaller, equal or 
   bigger than the other.

   if bigger then a change is counted as
   it will happen

   This allows for a list where shorter
   words are first, but alphabetically
   sorted among the same size. 
 * -------------------------------------- */


int compare_len(const void *a,const void *b)
{
    word_t *wa,*wb;
    int ret;

    wa = *((word_t **)a);
    wb = *((word_t **)b);

    compares++;

    ret = wa->len - wb->len;
    if(!ret)
        return strcmp(wa->str,wb->str);
    return ret;
}

/* ------------------------------------------- */
/* ------------------------------------------- */

typedef int (*compare_func_t)(const void *a,const void *b);

/* ------------------------------------------- *
  Bubble Sort bubbles up valuse to their final 
  position
 * ------------------------------------------- */

void bubble_sort(word_t *list[MAX_WORDS],int max,compare_func_t compare_func)
{
    int swapped,a,b;
    word_t *tmp;

    if (max > 1 && compare_func)
    {
        do {
            swapped = 0;

            for (a=0,b=1; b < max; b++, a++)
            {
                if (compare_func(&(list[a]),&(list[b])) > 0)
                {
                    tmp=list[b];
                    list[b]=list[a];
                    list[a]=tmp; 
                    swapped++;                    
                    changes++;
                }
            }

        } while(swapped);
    }    
};



/* ------------------------------------------- *
  Bubble Sort with optimization to solve the
  Rabbits and Turtles problem (which greatly
  improves the algorithm's speed) also called
  sinking and bubbling, as values sink down or
  bubble up to their final position
 * ------------------------------------------- */

void cocktail_shaker(word_t *list[MAX_WORDS],int max,compare_func_t compare_func)
{
    int swapped,a,b;
    word_t *tmp;
    
    if (max > 1 && compare_func)
    {
        do {
            swapped = 0;

            /* Sinking */

            for (a=0,b=1; b < max; b++, a++)
            {
                if (compare_func(&(list[a]),&(list[b])) > 0)
                {
                    tmp=list[b];
                    list[b]=list[a];
                    list[a]=tmp; 
                    swapped++;         
                    changes++;           
                }
            }

            /* Bubbling */

            if (swapped) 
            {
                swapped = 0;
                
                for (b=max-1,a=max-2; b > 0; a--, b--)
                {
                    if (compare_func(&(list[a]),&(list[b])) > 0)
                    {
                        tmp=list[b];
                        list[b]=list[a];
                        list[a]=tmp; 
                        swapped++;    
                        changes++;                    
                    }
                }
            }
        } while(swapped);
    }
};

/* --------------- */

void print_ticks(char *prefix,ticks_t ticks)
{
    char *cad,tmp[300]={0};
    int sign=0;

    if(ticks < 0)
    {
        sign++;
        ticks*=-1;
    }
    cad=tmp + 299;
    while(ticks > 0)
    {
        cad--;
       *cad='0' + (char)(ticks % 10);
        ticks/=10;
    }
    if(sign)
    {
        cad--;
       *cad='-';
    }
    printf("%s=%s\n",prefix,cad);
}

/* -------------------------------------- */

void test_sorting(void)
{
    stopwatch_t sw1,sw2;    
    
    printf("\n*** Test Sort ***\n\n");

    stopwatch_start(&sw1);    

    printf("Stop watch started...\n");

    load_words();

    printf("It took %.6f seconds to Load, Half, Reverse and Unsort the words\n\n",stopwatch_elapsed(&sw1));

    /* Already sorted */

    copy_words(words_list);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    bubble_sort(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort sorted using bubble_sort() \n",stopwatch_elapsed(&sw2),changes,compares);
    
    copy_words(words_list);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    cocktail_shaker(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort sorted using cocktail_shaker()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_list);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    qsort(words_tmp,words_count,sizeof(word_t *),compare_str);   
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort sorted using qsort()\n\n",stopwatch_elapsed(&sw2),changes,compares);

    /* Halved */

    copy_words(words_halved);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    bubble_sort(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort halved using bubble_sort()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_halved);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    cocktail_shaker(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort halved using cocktail_shaker()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_halved);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    qsort(words_tmp,words_count,sizeof(word_t *),compare_str);   
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort halved using qsort()\n\n",stopwatch_elapsed(&sw2),changes,compares);

    /* Reversed */

    copy_words(words_reversed);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    bubble_sort(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort reversed using bubble_sort()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_reversed);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    cocktail_shaker(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort reversed using cocktail_shaker()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_reversed);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    qsort(words_tmp,words_count,sizeof(word_t *),compare_str);   
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort reversed using qsort()\n\n",stopwatch_elapsed(&sw2),changes,compares);

    /* Unsorted */

    copy_words(words_unsorted);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    bubble_sort(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort unsorted using bubble_sort()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_unsorted);    
    stopwatch_start(&sw2);
    changes=compares=0;    
    cocktail_shaker(words_tmp,words_count,compare_str);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort unsorted using cocktail_shaker()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_unsorted);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    qsort(words_tmp,words_count,sizeof(word_t *),compare_str);   
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort unsorted using qsort()\n\n",stopwatch_elapsed(&sw2),changes,compares);

    /* Sort by len */

    copy_words(words_unsorted);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    bubble_sort(words_tmp,words_count,compare_len);
    stopwatch_stop(&sw2);
    printf("It took %.3f seconds, %d changes and %d compares to sort by len using bubble_sort()\n",stopwatch_elapsed(&sw2),changes,compares);

    save_words("words_by_len.txt",words_tmp);

    copy_words(words_unsorted);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    cocktail_shaker(words_tmp,words_count,compare_len);
    stopwatch_stop(&sw2);

    printf("It took %.3f seconds, %d changes and %d compares to sort by len using cocktail_shaker()\n",stopwatch_elapsed(&sw2),changes,compares);

    copy_words(words_unsorted);    
    stopwatch_start(&sw2);    
    changes=compares=0;
    qsort(words_tmp,words_count,sizeof(word_t *),compare_len);   
    stopwatch_stop(&sw2);
    
    printf("It took %.3f seconds, %d changes and %d compares to sort by len using qsort()\n\n",stopwatch_elapsed(&sw2),changes,compares);


    stopwatch_stop(&sw1);

    printf("Total execution time was %.3f seconds\n\n",stopwatch_elapsed(&sw1));

    free_words();
}

/* -------------------------------------------------- *
   This waste cycles to make sure searches takes some
   longer time and the measures are visible (if we 
   do not use this, all will take 0.000 secs)
 * -------------------------------------------------- */

static ticks_t waste_cycles(void)
{
    int t;
    ticks_t ret = 0;

    for(t=0;t<10000;t++)
        ret+=cpu_ticks();
    return ret;
}

/* -------------------------------------------------- */

int find_word_bin(const char *word)
{
    int ret, med, min, max, found = 0;
    
    min=0;
    max=words_count - 1;        
    
    /* Efficient search (binary) */

    compares=0;

    while(max >= min && !found)
    {
        med = ((max - min) / 2) + min;            
        ret = strcmp(words_list[med]->str,word);
        if (ret < 0)
        {
            min = med + 1;                
        }
        else if(ret > 0)
            max = med - 1;
        else
            found++;            
        compares++;
        waste_cycles();
    }
    
    return found;
}

/* -------------------------------------------------- */

int find_word_seq(const char *word)
{
    int t, found = 0;
    
    /* Unefficient search (sequential) */

    compares = 0;

    for(t=0;!found && t<words_count;t++)
    {
        if(!strcmp(words_list[t]->str,word))
            found++;
        compares++;
        waste_cycles();
    }
    
    return found;
}

/* -------------------------------------------------- */

void test_searching(void)
{
    stopwatch_t sw1,sw2;    
    int found,t;
    char *word_to_find[] = {"pm","job","bird","marry","worker","forward","terrible","tablespoon","significant","surprisingly","zone","violence","writing","yourself","YouCannotFindMe","not_here",NULL};
    
    printf("\n*** Test Search ***\n\n");

    stopwatch_start(&sw1);    

    printf("Stop watch started...\n");

    load_words();

    printf("It took %.6f seconds to Load, Half, Reverse and Unsort the words\n\n",stopwatch_elapsed(&sw1));

    for(t=0;word_to_find[t];t++)
    {
        /* Sequential search */

        stopwatch_start(&sw2);            
        found = find_word_seq(word_to_find[t]);
        stopwatch_stop(&sw2);
        printf("It took %.3f seconds, to sequentially search '%s' and it was %s after %d compares\n",stopwatch_elapsed(&sw2),word_to_find[t],(found) ? "FOUND" : "NOT FOUND",compares);

        /* Binary search */

        stopwatch_start(&sw2);            
        found = find_word_bin(word_to_find[t]);
        stopwatch_stop(&sw2);
        printf("It took %.3f seconds, to binary search '%s' and it was %s after %d compares\n\n",stopwatch_elapsed(&sw2),word_to_find[t],(found) ? "FOUND" : "NOT FOUND",compares);

    }

    stopwatch_stop(&sw1);

    printf("Total execution time was %.3f seconds\n\n",stopwatch_elapsed(&sw1));

    free_words();
}

/* -------------------------------------- */

int main(void)
{
    test_sorting();
    test_searching();    
    return 0;
}
