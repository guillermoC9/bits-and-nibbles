/*
    csv.h

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

#ifndef CSV_FILES_BY_GAO
#define CSV_FILES_BY_GAO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef FALSE
   #define FALSE   0
   #define TRUE    1
#endif

#if defined(_WIN32)
   typedef long off_t; 
#endif

/* --------------------------------- *
   Make sure unsigned int is at 
   least 32 bits long 
 * --------------------------------- */

 #if UINT_MAX <  0xffffffffu
   #error "unsigned int is expected to have at least 32 bits"
#endif

/*
    Limits in the CSV are actually the same that in Microsoft Excel, which can be read here:

    https://support.office.com/en-us/article/Excel-specifications-and-limits-1672b34d-7043-467e-8e27-269d656771c3

    Note that although we respect the limit of 16,384 columns (cells) imposed by Microsoft Excel,
    our code here can be used to read over their limit of 1,048,576 rows, as rows here are returned
    one at the time, so that limit is informative only because the limit will really be imposed by
    the use of the rows by the caller. Although in practive we have a limit of 134,217,728 rows
    because there has to be a limit to avoid integer overflow. Yeah we could have used a bigger type,
    but it made not sense as our practical limit is already 128 times bigger than the official.
*/

#define CSV_MAX_EXCEL_ROWS  0x100000    /*  1,048,576 rows maximum in a file produced by excel */
#define CSV_MAX_COLS        0x4000      /* 16,384 fields (columns) maximum in a row  */
#define CSV_MAX_CHARS       0x7fff      /* 32,767 characters maximum in a field (cell) */
#define CSV_MAX_HEADER      0xff        /* 255 characters máximum in a header name */
#define CSV_MAX_LN          0xfd        /* 253 new lines inside a quoted field (line feeds per cell) */

/* Our limits */

#define CSV_MAX_ROWS        0x8000000   /* 134,217,728 rows maximum in a file read by us */
#define CSV_MAX_BYTES       0x8fff      /* 36,863 bytes maximum in a field of our own */

/* Control structure */

typedef struct _csv_t_
{
    FILE    *fp;           /* File being read (they are open read only) */
    off_t    pos;          /* Position of the first row */
    int      rows;         /* Number of rows read so far (csv_rewind() reset this value) */

    int      sep;          /* Separator */
    int      quo;          /* Quote */
    int      enc;          /* Encoding */

    char   **hdr_lst;      /* Array with headers (NULL terminated) */
    int      hdr_cols;     /* number of populated columns in the header */

    char   **row_lst;      /* Array with last read row (NULL terminated) */
    int     *row_len;      /* size of columns in the row */
    int      row_cols;     /* number of populated columns in the last read row */
    int      row_max;      /* number of total columns in the row */

    unsigned char  field_buf[];  /*  buffer for readig fields. CSV_MAX_BYTES in size */
} csv_t;

/** Macros */

#define CSV_ENC_UTF8     1      /* File is encoded in UTF-8 */
#define CSV_ENC_W1252    2      /* File is encoded in Windows 1252 codepage */

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Create a context for parsing CSV files using the
   default values:

    - Separator is , (comma)
    - Quote char is " (double quote)
    - Encoding is UTF-8
 * -------------------------------------------------- */

csv_t *csv_create(void);

/* -------------------------------------------------- *
   Destroy a CSV parsing context freeing all the
   resources and closing the file if open.
 * -------------------------------------------------- */

void csv_destroy(csv_t *csv);

/* -------------------------------------------------- *
   Change options for parsing of a CSV as far as a
   file is not currently open:

   'sep' is the field separator
   'quo' is the quote character
   'enc' is the encoding of the file

    Anything you do or want to change send as 0

    Examples:

    csv_set_options(csv,0,0,CSV_ENC_W1252);  // Encode using win1252
    csv_set_options(csv,'\t','\'',0);        // TSV file in UTF-8 with single quote
    csv_set_options(csv,'|','\'',0);         // Pipe delimited in UTF-8 with single quote

    It returns:

        CSV_EINVAL   Parameters are missing or wrong
        CSV_EBUG     separator and quote cannot be the same

 * -------------------------------------------------- */

int csv_set_options(csv_t *csv,char sep,char quo,int enc);

/* -------------------------------------------------- *

   Starts the parsing of a CSV from a given file
   pointer.

    'csv' is the context
    'hdr' is the position of the header where:

         0 = No header exist
         1 = header is first line of the file
         2 = header is second line (1st is ignored)
         3 = header is third line (1st and 2nd ignored)
         4 = header is fourth line (1, 2 and 3 ignored)
         ...
         n = header is nth line (1 to n-1 ignored)

    'filename' is the file to open

 * -------------------------------------------------- */

int csv_open(csv_t *csv,int hdr,const char *filename);
int csv_openw(csv_t *csv,int hdr,const wchar_t *filename);

/* -------------------------------------------------- *
   Return the FILE descriptor of the open FILE in 
   case you want to manupulate it, which it is not
   recommended as you can mess up the work of the 
   function here.
 * -------------------------------------------------- */

FILE *csv_file_desc(csv_t *csv);

/* -------------------------------------------------- *
   These return the characters used as separator and 
   quote for columns.
   csv_encoding() return the encoding of the file:
   CVS_ENC_UTF8 or CVS_ENC_W1252
 * -------------------------------------------------- */

int   csv_separator(csv_t *csv);
int   csv_quote(csv_t *csv);
int   csv_encoding(csv_t *csv);

/* -------------------------------------------------- *
   Return information on the headers    
  
   The headers are taking from the first row of the 
   file unless you set 'hdr' to 0 in csv_open()
   
   number of cols will never be smaller than the
   number of headers, but it could be BIGGER. This is
   due to the nature of the CSV itself

   the 'count' parameter is and optional recipient for
   the number of headers, just in case you want to get 
   both things at the same time and nto having to call
   csv_header_count().

   s9_csv_header_name() returns the name of the header
   in position num. Range is 0 to s9_csv_header_count()
 * -------------------------------------------------- */

const char **csv_header_list(csv_t *csv,int *count);
const char  *csv_header_name(csv_t *csv,int num);

int csv_header_count(csv_t *csv);

/* -------------------------------------------------- *
   Return information on the curren loaded row    
   
   One returns an array with the cols of the row
   and the other the number of them.

   REMEMBER: 
   
   The number of cols will never be smaller than the
   number of headers, but it could be BIGGER. This is
   due to the nature of the CSV itself

   the 'cols' parameter is and optional recipient for
   the number of cols, just in case you want to get 
   both things at the same time. 

   s9_csv_row_col_data() returns the data of the col
   in position num from the current row. 
   Range is 0 to s9_csv_row_cols() - 1.
 * -------------------------------------------------- */

const char **csv_row_list(csv_t *csv,int *cols);
const char  *csv_row_col_data(csv_t *csv,int num);

int csv_row_cols(csv_t *csv);


/* -------------------------------------------------- *
   Returns the number of processed rows (including 
   the current one)
 * -------------------------------------------------- */

int csv_row_count(csv_t *csv);

/* -------------------------------------------------- *
   Ends the parsing of a CSV freeing all resources
   in the csv related to the file like closing the
   file, freeing row and headers and reseting the
   row counter.
 * -------------------------------------------------- */

void csv_close(csv_t *csv);

/* -------------------------------------------------- *
   Reads and return the next row of the file.

   The returned array of strings is null-terminated
   and has csv_row_cols() number of cols.

   This number will never be smaller than the number
   of cols of the header, but it can be larger
   (although never bigger than the limits). If the
   number of cols in the row is smaller than the
   number of headers, the row is padded with empty
   strings in each col to make the caller's code
   for row handling simpler.

   It returns NULL when more rows cannot be read,
   i.e. EOF is reached or 134,217,728 rows have
   been read.

   Note that this function's limit is 128 times bigger
   than the official limit of 1,048,576.

   As said above, that limit is informative only as
   the rows are returned one by one. :-)

 * -------------------------------------------------- */

char **csv_read_next(csv_t *csv);

/* -------------------------------------------------- *
    This function goes back and reads the first row
    after the header if any.

    IMPORTANT: This resets the counter of read rows
    to avoid problems
 * -------------------------------------------------- */

char **csv_rewind(csv_t *csv);

#ifdef __cplusplus
};
#endif


/* -------------------------------------------------- *
    Return values for the functions of the engine
    They mostly take their names from the macros in
    errno.h. We redefine them because errno values are
    positive and we use negative values so we can return
    errors from functions that return positive numbers
    without having to modify errno, as to do so may mess
    up analysis of errors set by the standard lib 
    functions which in this way can be checked later
 * -------------------------------------------------- */

#define CSV_OK            0     /* Zero its mostly 'Ok' int the standard C library */
#define CSV_EINVAL       -1     /* Invalid argument passed */
#define CSV_ENOMEM       -4     /* No enough memory */
#define CSV_EEXIST       -12    /* Creation error */
#define CSV_ECREAT       -12    /* Creation error */
#define CSV_EOPEN        -13    /* Open error */
#define CSV_EREAD        -14    /* Read error */
#define CSV_EBUG         -16    /* POSSIBLE BUG */
#define CSV_ENODATA      -22    /* No data to return */


#endif
