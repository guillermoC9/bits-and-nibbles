/*
    test_csv.c

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

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "csv.h"

#define PC_MAX_CHARS   8

/* -------------------------------------- */

typedef struct 
{
    char pc[PC_MAX_CHARS + 1];
    double lat,lon;    
} pc_t;

/* -------------------------------------- */

static pc_t **pc_lst = NULL;
static int    pc_cnt = 0;
static int    pc_max = 0;

/* -------------------------------------- */

static int add_to_pc_db(const char *pc,const char *lon,const char *lat)
{
    /* Increase the database of postcodes */
    if(pc_cnt == pc_max)
    {
        pc_t **tmp = (pc_t **)realloc(pc_lst,sizeof(pc_t *) * (pc_max + 1024));
        if(tmp == NULL)
            return FALSE;
        pc_lst = tmp;
        pc_max += 1024;
    }

    pc_lst[pc_cnt] = (pc_t *)calloc(sizeof(pc_t),1);
    if(pc_lst[pc_cnt] == NULL)
        return FALSE;

    strncpy(pc_lst[pc_cnt]->pc,pc,PC_MAX_CHARS);
    pc_lst[pc_cnt]->lat = atof(lat);
    pc_lst[pc_cnt]->lon = atof(lon);
    pc_cnt++;
    return TRUE;
}

/* -------------------------------------- */

static void free_pc_db(void)
{
    int t;

    if(pc_lst)
    {
        for(t=0;t<pc_cnt;t++)
        {
            free(pc_lst[t]);
            pc_lst[t] = NULL;
        }
        free(pc_lst);
        pc_lst = NULL;
        pc_cnt = 0;
        pc_max = 0;
    }
}


/* -------------------------- */

static int  strempty(const char *s)
{
    if(s)
    {
        while(*s)
        {
            if(!isspace(*s++))
                return FALSE;
        }
    }
    return TRUE;
}

/* -------------------------- */

static void strupper(char *s)
{
    if(s)
    {
        char *d = s;
        while(*s)
           *d++ = toupper(*s++);
    }
}

/* -------------------------------------- */

static void test_postcodes(void)
{
   
    int t,cnt,ret,pos[3]={-1,-1,-1},got;
    csv_t *csv;
    const char **row;
    char pc[1025] = {0},*ptr;

    printf("\n*** Test of POSTCODES ***\n\n");

    csv = csv_create();
    if(csv == NULL)
    {
        printf("ERROR: cannot create CSV context\n");
        return;
    }
    
    /* Open the file */

    ret = csv_openw(csv,1,L"uk_postcodes.csv");
    if(ret != CSV_OK)
    {
        printf("ERROR %d loading postcodes file uk_postcodes.csv\n",ret);
        csv_destroy(csv);
        return;
    }
    
    /* Find out if we got the rows we need, and in which position they are */

    got = 0;    
    row = csv_header_list(csv,&cnt);
    for(t=0;t<cnt && got < 3;t++)
    {                
        if(!strcmp("Postcode",row[t]))
            ret = 0;
        else if(!strcmp("Longitude",row[t]))
            ret = 1;
        else if(!strcmp("Latitude",row[t]))
            ret = 2;
        else
            ret = -1;

        if(ret > -1 && pos[ret]==-1)    
        {
            pos[ret]=t;
            got++;
        }
    }

    if(got != 3) 
    {
        printf("ERROR: Missing column. File needs colums: Postcode, Longitude and Latitude in any order\n");
        csv_destroy(csv);
        return;
    }

    printf("Loading postcodes from uk_postcodes.csv...\n");
        
    while(csv_read_next(csv))
    {
        row = csv_row_list(csv,NULL);
        if(row && !add_to_pc_db(row[ pos[0] ],row[ pos[1] ],row[ pos[2] ]))
        {
            printf("ERROR: There is not enough memory to read more rows\n");
            break;
        }
    }

    printf("Loaded %d postcode entries from %d rows\n\n",pc_cnt,csv_row_count(csv) );
    csv_destroy(csv);
    
    do {        
        printf("Write a uk postcode or part of it (empty to end):\n");
        if(fgets(pc,1024,stdin) && !strempty(pc))
        {
            ptr=strpbrk(pc,"\r\n");
            if(ptr)
                *ptr='\0';
            strupper(pc);
            got = strlen(pc);
            ret = -1;
            for(t=0; ret==-1 && t<pc_cnt; t++)
            {
                if(!strncmp(pc,pc_lst[t]->pc,got))
                    ret = t;
            }
            if(ret == -1)
                printf("Could not find postcode '%s'\n",pc);
            else    
                printf("Postcode '%s' is at lat=%.6f lon=%.6f\n",pc_lst[ret]->pc,pc_lst[ret]->lat,pc_lst[ret]->lon);            
        }
    } while(!strempty(pc));

    printf("That's all folks!!\n");

    free_pc_db();
}

/* -------------------------------------- */

int separator = ',';
int encoding  = CSV_ENC_UTF8;
int quotes    = '"';

/* -------------------------------------- */

static void test_csv(const char *filename)
{
    int t,ret;
    csv_t *csv;

    printf("\n*** Test of CSV ***\n\n");

    csv = csv_create();
    if(csv == NULL)
    {
        printf("ERROR: cannot create CSV context\n");
        return;
    }

    csv_set_options(csv,separator,quotes,encoding);  

    ret = csv_open(csv,1,filename);
    if(ret == CSV_OK)
    {
        int count = csv_header_count(csv);
        int max_rows = 0;

        if(count)
        {
            printf("Read %d Headers from '%s'\n\n",count,filename);
            for(t=0 ; t < count; t++)
                printf("%d: %s\n",t+1,csv_header_name(csv,t));
            printf("\n");
        }
        count = 0;
        while(csv_read_next(csv))
        {
            if(csv_row_cols(csv) > max_rows)
                max_rows = csv_row_cols(csv);
            count++;
        }
        printf("Read %d rows of a max of %d cols (%d reported internally)\n",count,max_rows,csv_row_count(csv));
    }

    if(ret != CSV_OK)
        printf("ERROR %d testing CSV file %s\n",ret,filename);

    csv_destroy(csv);

    printf("\n");
}

/* -------------------------------------- */

static int read_options(int argc,char **argv)
{
    int pos = 0;
    char *ptr;

    while(++pos < argc && argv[pos][0] == '-')
    {
        ptr = &(argv[pos][1]);
        if(!strcmp(ptr,"w"))
            encoding = CSV_ENC_W1252;
        else if(!strcmp(ptr,"t"))
            separator = '\t';
        else if(!strcmp(ptr,"s"))
            separator = ';';
        else if(!strcmp(ptr,"p"))
            separator = '|';
        else if(!strcmp(ptr,"q"))
            quotes = '\'';        
    }

    return pos;
}

/* ------------------------------- */

static void set_console_utf8()
{
#if defined(_WIN32)
    SetConsoleOutputCP(65001);
#else
    setlocale(LC_CTYPE,"UTF-8");
#endif
}

/* -------------------------------------- */

int main(int argc,char **argv)
{
    int t,first;
    
    if(argc > 1)
    {
        set_console_utf8();

        first = read_options(argc,argv);

        for(t=first; t<argc; t++ )
        {
            if(!strcmp(argv[t],"postcodes"))
                test_postcodes();
            else
                test_csv(argv[t]);
        }
    }
    else
    {
        printf("\nusage: %s  [-w -t -s -p -q] csv_file_1 [csv_file_2 ...]\n\n",argv[0]);
        printf("    -w = use Windows 1252 encoding for the file instead UTF-8\n");
        printf("    -t = use tabulator ('\\t') as separator instead comma (,) \n");
        printf("    -s = use semicolon (;) as separator instead comma (,) \n");
        printf("    -p = use pipe ('|') as separator instead comma (,) \n");        
        printf("    -q = use single quote (') as string delimiters instead double quote (\")\n\n");        
        printf("POSTCODES: Use the word 'postcode' instead of a filename to run the postcodes test\n\n");
    }
    return 0;
}
