/*
    pdf_images.h

    PDF Image utilities

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

        - Originally written by Guillermo Amodeo Ojeda

                                --oO0Oo--
*/

#ifndef PDF_IMAGE_UTILITIES
#define PDF_IMAGE_UTILITIES

#include "stuff.h"
#include "pdf_defs.h"

/* Image */

#define PDF_IMG_RGB     0       /* True Color Image */
#define PDF_IMG_JPG     1       /* JPEG Image */

typedef struct
{
    int         w,h;    /* Dimensions */
    int         type;   /* type of image (see above macros) */
    int         bpc;    /* Bits per color */
    const char *cs;     /* Color Space */

    int         used;   /* TRUE/FALSE has been used */
    int         n;      /* Object number of the image */

    void       *data;   /* Image data */
    size_t      dlen;   /* data len */
    int         zdata;  /* TRUE/FALSE deflated data (Ignored for PDF_IMG_JPG) */

    void       *mask;   /* Mask/Alpha data */
    size_t      mlen;   /* mask len */
    int         zmask;  /* TRUE/FALSE deflated mask (Ignored for PDF_IMG_JPG) */
} pdf_image_t;

/* Palette entry */

typedef struct
{
    unsigned char red,green,blue;
} rgb_t;


#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------ *
   Load a JPG, PNG or BMP image into a pdf_image_t 
     
   Return values:

    PDF_OK         = Success
    PDF_EINVAL     = Bad parameters
    PDF_ENOMEM     = Not enough memory
    PDF_EOPEN      = Cannot open the file
    PDF_EINCORRECT = Cannot understand the file format   
 * ------------------------------------------------------ */

int pdf_load_an_image(pdf_image_t *img,wchar_t *file);

#ifdef __cplusplus
};
#endif

#endif


