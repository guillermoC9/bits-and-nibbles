/*
    pdf_defs.h

    Gloabl Definitions for Portable Document Format generation engine

    (CC) Creative Commons 2021-2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

                                --oO0Oo--
*/

#ifndef PDF_DOCUMENT_GENERATION_ENGINE_DEFINES
#define PDF_DOCUMENT_GENERATION_ENGINE_DEFINES

/* Macros */

#define PDF_DOC_VERSION       1.4     /* Version of the PDF documents */
#define PDF_ENGINE_VERSION    1.0     /* Version of the engine */

/* Limits */

#define PDF_MAX_TEMP         2048     /* Temporary buffer for conversions */
#define PDF_MAX_URI           512     /* Maximum chars in a URI */

/* Document's orientation */

#define PDF_PORTRAIT            0     /* Vertical (default) */
#define PDF_LANDSCAPE           1     /* Horizontal */

/* Document's page sizes */

enum 
{
    PDF_SIZE_A4  = 0,   /* A4 (default) */
    PDF_SIZE_A3,        /* A3 */
    PDF_SIZE_A5,        /* A5 */
    PDF_SIZE_LETTER,    /* Letter */
    PDF_SIZE_LEGAL,     /* Legal */
    
    PDF_NUM_PAGE_SIZES
};

/* Document's units */

enum 
{
    PDF_UNIT_MM = 0,    /* Millimeters (default) */
    PDF_UNIT_CM,        /* Centimeters */
    PDF_UNIT_IN,        /* Inches */
    PDF_UNIT_PT,        /* Points per inch  */

    PDF_NUM_UNITS
};

/* Document's Zoom Modes */

enum 
{
    PDF_ZOOM_DEFAULT = 0,   /* Default */
    PDF_ZOOM_FULLPAGE,      /* Full Page */
    PDF_ZOOM_WIDTH,         /* Full eidth */
    PDF_ZOOM_ACTUAL,        /* Actual size */

    PDF_ZOOM_NUM_MODES
};

/* Document's Zoom Factors (Range))*/

#define PDF_ZOOM_MIN         4      /* Minimum Zoom */
#define PDF_ZOOM_MAX      3000      /* Maximum Zoom Factor */

/* Document's Page Layout */

enum 
{
    PDF_PAGE_DEFAULT = 0,   /* Default */
    PDF_PAGE_ROLL,          /* Full eidth */
    PDF_PAGE_SINGLE,        /* Single Page */
    PDF_PAGE_DUAL_LEFT,     /* Dual Page odd in left */
    PDF_PAGE_DUAL_RIGHT,    /* Dual Page odd in right */

    PDF_NUM_LAYOUTS
};

/* Document's Metadata */

enum 
{
    PDF_DOC_TITLE = 0,      /* Document's Title */
    PDF_DOC_AUTHOR,         /* Document's Author */
    PDF_DOC_SUBJECT,        /* Document's Subject */
    PDF_DOC_KEYWORDS,       /* Document's Keywords */
    PDF_DOC_PRODUCER,       /* Document's Producer (default GAO-PDF) */
    PDF_DOC_CREATOR,        /* Document's Creator */
    PDF_DOC_CREATION_DATE,  /* Document's Creation Date (default automatic) */

    PDF_MAX_MD_FIELDS
};

/* Document's status */

enum 
{
    PDF_STATE_EMPTY = 0,    /* Document is empty */
    PDF_STATE_PAGE_CLOSED,  /* Page is closed */
    PDF_STATE_PAGE_READY,   /* Page is ready for writing */
    PDF_STATE_DOC_CLOSED,   /* Document is closed */

    PDF_NUM_DOC_STATES
};

/* Line Types */

enum 
{
    PDF_SOLID_LINE = 0,     /* Solid line */
    PDF_DOTTED_LINE,        /* Dotted line */
    PDF_DASHED_LINE,        /* Dashed line */
    PDF_MORSE_LINE,         /* Dot-Dash-Dot-Dash line */
    PDF_DOTGAP_LINE,        /* Dotted line with bigger gaps*/
    PDF_DASHGAP_LINE,       /* Dashed line with bigger gaps */

    PDF_NUM_LINE_TYPES
};

/* Drawing Attributes */

#define PDF_A_NONE          0x0000000   /* No border, No fill, feed right */

#define PDF_A_LFT_B         0x0000001   /* Left border */
#define PDF_A_TOP_B         0x0000002   /* Top border */
#define PDF_A_RGT_B         0x0000004   /* Right border*/
#define PDF_A_BOT_B         0x0000008   /* Bottom border */
#define PDF_A_FULL_B        0x000000F   /* Full border */

#define PDF_A_RND_TL        0x0000010   /* Round Top Left corner or draw Top Left segment */
#define PDF_A_RND_BL        0x0000020   /* Round Bottom Left corner or draw Bottom Left segment  */
#define PDF_A_RND_TR        0x0000040   /* Round Top Right corner  or draw Top Right  segment  */
#define PDF_A_RND_BR        0x0000080   /* Round Bottom Right corner  or draw Bottom Right segment  */
#define PDF_A_RND_F         0x00000F0   /* Round all borders or draw full border (circle) */

#define PDF_A_RIGHT_F       0x0000100   /* Feed Right */
#define PDF_A_LINE_F        0x0000200   /* Line feed  */
#define PDF_A_BELOW_F       0x0000400   /* Below feed */

#define PDF_A_LEFT_J        0x0000000   /* Left Justified  */
#define PDF_A_RIGHT_J       0x0001000   /* Right Justified */
#define PDF_A_CENTRE_J      0x0002000   /* Center Justified */
#define PDF_A_FULL_J        0x0004000   /* Full Justified */

#define PDF_A_FILLED        0x0010000   /* Fill the cell */
#define PDF_A_FIT_TEXT      0x0080000   /* Fit text in the cell */

#define PDF_A_BORDER(a)     ((a) & 0x000000f)
#define PDF_A_ROUND(a)      ((a) & 0x00000f0)
#define PDF_A_FEED(a)       ((a) & 0x0000f00)
#define PDF_A_JUST(a)       ((a) & 0x000f000)
#define PDF_A_EXTRA(a)      ((a) & 0x00f0000)

/* List of currencies */

enum 
{
    PDF_CURR_NONE = 0,
    PDF_CURR_DOLLAR,     /* $ */
    PDF_CURR_POUND,      /* £ */
    PDF_CURR_EURO,       /* € */
    PDF_CURR_YEN,        /* ¥ */
    PDF_MAX_CURR
};


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

#define PDF_OK            0     /* Zero its mostly 'Ok' int the standard C library */
#define PDF_EINVAL       -1     /* Invalid argument passed */
#define PDF_ETIMEDOUT    -2     /* Timed out */
#define PDF_ENOMATCH     -3     /* Values do not match */
#define PDF_ENOMEM       -4     /* No enough memory */
#define PDF_EINTR        -5     /* Interrupted operation */
#define PDF_ERANGE       -6     /* Range error */
#define PDF_EFAULT       -7     /* Bad address (segmentation fault) */
#define PDF_E2BIG        -8     /* Result or argument is too big */
#define PDF_EAGAIN       -9     /* Temporarily busy, try again later*/
#define PDF_EACCES       -10    /* Access denied */
#define PDF_EFIND        -11    /* Not found */
#define PDF_ECREAT       -12    /* Creation error */
#define PDF_EOPEN        -13    /* Open error */
#define PDF_EREAD        -14    /* Read error */
#define PDF_EWRITE       -15    /* Write error */
#define PDF_EBUG         -16    /* POSSIBLE BUG */
#define PDF_ESYNTAX      -17    /* Syntax error */
#define PDF_EMISSING     -18    /* Something is Missing */
#define PDF_ECORRUPT     -19    /* Corrupted data */
#define PDF_EINCORRECT   -20    /* Somethign is incorrect */
#define PDF_ENOIMPL      -21    /* Not implemented */
#define PDF_ENODATA      -22    /* No data to return */
#define PDF_EFULL        -23    /* It is complete */
#define PDF_EPROTOCOL    -24    /* Protocol violation. */
#define PDF_EZLIB        -25    /* Internal zLib error */
#define PDF_ENOTTF       -26    /* Cannot initialize TTF engine */

#endif
