/*
    pdf_fnt.h

    Portable Document Format (.PDF) Font Handling

    (CC) Creative Commons 2021-2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Written by Guillermo Amodeo Ojeda using MAKEFONT.PHP by Olivier
          Plathey as basis, on its version 1.82 (Dated 07/12/2019).

        - Core font definitions have been converted from the font definition
          files in the fonts directory that come with MAKEFONT.PHP and
          completed usign the AFM files for the core fonts from Adobe

        - This code forces de internal encoding of the PDF to CP1252 enconding
          (aka Windows 1252) while externally uses wchar_t.

                                --oO0Oo--
*/

#ifndef PDF_FONTS_HANDLING
#define PDF_FONTS_HANDLING

#include "stuff.h"
#include "pdf_defs.h"

/* Font Types */

#define PDF_FNT_CORE        0      /* Core font. No need to embed */
#define PDF_FNT_TTF         1      /* TrueType font*/

/* Font Styles (OR) */

#define PDF_FNT_REGULAR     0x00
#define PDF_FNT_BOLD        0x01
#define PDF_FNT_ITALIC      0x02
#define PDF_FNT_UNDERLINE   0x10

/* Font Encodings */

#define PDF_ENC_CP1252      0       /* CP-1252  (aka Windows-1252) the only one supported by now */

#define PDF_FNT_MAX_CH    256       /* number of characters in CP-1252 */

/*  Unicode mapping table entry */

typedef struct
{
    int type;          /* Type of entry */
    int ch;            /* Char or range char start */
    int che;           /* Range char end*/
    int unicode;       /* Unicode Value */
} pdf_uv_t;

/* Types of entry */

#define PDF_UV_EMPTY    0       /* No value here */
#define PDF_UV_CHAR     1       /* Value is a char mapping */
#define PDF_UV_RANGE    2       /* Value is a range mapping */

typedef struct
{
    int         id;             /* Unique ID of table */
    int         cnt;            /* Number of entries */
    pdf_uv_t lst[];          /* Entries */
} pdf_uv_table_t;

/* Font definition structure */

typedef struct
{
    int      id,n;                  /* ID and object number (filled once loaded into a PDF) */

    char     family[81];            /* Font Family */
    int      style;                 /* Font Style */
    char     key[85];               /* Font Key */
    int      type;                  /* Font Type */
    char     name[81];              /* Font Name */
    int      enc;                   /* Font encoding */

    int      up;                    /* Underline position */
    int      ut;                    /* Underline thickness */

    int      asc;                   /* Ascender */
    int      des;                   /* Descender */
    int      cap;                   /* CapHeight */
    int      tal;                   /* Size of tallest char in the font */
    int      it_ang;                /* ItalicAngle */
    int      is_fix;                /* IsFixedPitch */
    int      xo,yo,xd,yd;           /* Font BBox x org ,y orig,x dest,y dest*/
    int      mis_width;             /* Missing Width */

    int      cw[PDF_FNT_MAX_CH];    /* Characters width table */

    pdf_uv_table_t   *uv;        /* Unicode mapping table */

    /* Embedded fonts */

    void     *file;
    size_t    flen,forg;

    int       used;                 /* Font used (ignored on core fonts *) */

} pdf_font_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Destroy a PDF font freeing all the resources.
   It will recognize core fonts and ignored them.
 * -------------------------------------------------- */

void pdf_free_font(pdf_font_t *fnt);

/* -------------------------------------------------- *
    Returns one of the core fonts
 * -------------------------------------------------- */

pdf_font_t *pdf_get_core_font(const char *family,int style);

/* -------------------------------------------------- *
   Load a TrueType font from a file to embed into
   a document.  If it returns PDF_EACCES it means 
   that the font file doesn't allow embedding.

   Return values:

    PDF_OK         = Success
    PDF_EINVAL     = Bad parameters
    PDF_ENOMEM     = Not enough memory
    PDF_EOPEN      = Cannot open the file
    PDF_EACCES     = File doesn't allow embedding    
    PDF_EINCORRECT = Cannot understand the file format
    PDF_ENOTTF     = Cannot init Freetype
 * -------------------------------------------------- */

int pdf_load_font_file(pdf_font_t **fnt,const wchar_t *font,const char *family);

#endif
