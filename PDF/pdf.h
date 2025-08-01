/*
    pdf.h

    Portable Document Format generation engine

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

        - Written by Guillermo Amodeo Ojeda using FDPF.PHP by Olivier
          Plathey as guide (version 1.82 dated 07/12/2019) and some
          scripts form his site (http://www.fpdf.org) by Olivier himself
          and other people like: Christophe Prugnaud,

        - The generated PDF 'code' have been fine tuned using PDF
          specs and what I have been learning about PDF lingo.

        - This code forces de internal encoding of the PDF to CP1252 enconding
          (aka Windows 1252) while externally uses wchar_t to ease the font
          handling.

        - This has been done like that so in the future we can change the internals
          to support languages not supported by CP1252 without having to change the
          public API functions.

                                --oO0Oo--
*/

#ifndef PDF_DOCUMENT_GENERATION_ENGINE
#define PDF_DOCUMENT_GENERATION_ENGINE

#include "stuff.h"
#include "barcode.h"
#include "pdf_defs.h"
#include "pdf_font.h"
#include "pdf_image.h"

/* Target Link  */

typedef struct
{
    int     local;              /* TRUE/FALSE is a local link */
    int     page;               /* Page Number to go */
    float   y;                  /* Postition where to land */
    char    uri[PDF_MAX_URI];   /* Link's uri */
} pdf_tlnk_t;

/* Origin Link  */

typedef struct
{
    float   x,y;                /* Position of the link */
    float   w,h;                /* Dimensions */
    int     id;                 /* Target link ID */
} pdf_olnk_t;

/* PDF Page Info */

typedef struct
{
    pdf_olnk_t  *lnk_lst;                /* List of origin links in the page */
    int             lnk_cnt;                /* Number of links in the list */
    int             lnk_max;                /* Capacity of the list of links */
    int             n;
    unsigned char  *content;                /* Content of the page */
    size_t          size;                   /* Content size */
    size_t          max;                    /* Content capacity */
    int             replaced;               /* Aliases have been replaced */
} pdf_page_t;


/* User defined functions (for header and footer */

typedef int (*pdf_user_func_t)(void *pdf);

/* Control structure */

typedef struct
{
    float           version;                      /* PDF Version */
    wchar_t        *metadata[PDF_MAX_MD_FIELDS];  /* Metadata  Fields */
    int             state;                        /* Document state */

    long           *offsets;                      /* Object's offsets */
    int             obj_num;                      /* Number of objects */
    int             obj_max;                      /* Max Number of objects */

    pdf_tlnk_t  *lnk_lst;                      /* List of target links in the document */
    int             lnk_cnt;                      /* Number of links in the list */
    int             lnk_max;                      /* Capacity of the list of links */

    int             lnk_border;                   /* TRUE/FALSE link's borders are visible */

    pdf_page_t  *page_lst;                     /* List of pages */
    int             page_cnt;                     /* Number of pages */
    int             page_max;                     /* Capacity of page's list */
    int             page_num;                     /* Current page number (-1 none) */
    char           *page_nb_alias;                /* Alias of number of pages */

    pdf_font_t **fnt_lst;                      /* List of fonts */
    int             fnt_cnt;                      /* Number of fonts */
    int             fnt_max;                      /* capacity of font's list */

    pdf_font_t  *fnt_cur;                      /* Current font */
    float           fnt_pt;                       /* Font Size in Points */
    float           fnt_size;                     /* Font size in user units */
    int             fnt_under;                    /* TRUE/FALSE underline is active */

    float           txt_line;                     /* Height of a text line in units */

    pdf_image_t    *img_lst;                      /* List of images */
    int             img_cnt;                      /* Number of images*/
    int             img_max;                      /* capacity of images list */


    int             orientation;                  /* Default orientation */
    int             rotation;                     /* Page rotation */
    int             size;                         /* Default size */
    int             zoom;                         /* Default zoom */
    int             layout;                       /* Default layout */
    int             unit;                         /* Document measure unit  */
    float           scale;                        /* Scale Factor (points per user unit)  */
    float           size_h;                       /* Horizontal size (portrait) */
    float           size_v;                       /* Vertical size (portrait) */
    float           ws;                           /* Word Spacing */
    float           x,y;                          /* Current position in user units */
    float           width,height;                 /* Dimensions of current page in user units */
    float           width_pt,height_pt;           /* Dimensions of current page in Points */
    float           dmargin,cmargin;              /* Document and cell margins */
    float           lmargin,rmargin;              /* Left and right margins */
    float           tmargin,bmargin;              /* Top and page break margins */
    float           pageabt;                      /* Auto Page Break trigger */

    float           lasth;                        /* Height of last printed cell */

    int             autopb;                       /* TRUE/FALSE automatic page breaking active */
    int             alpha;                        /* TRUE/FALSE alpha blending is active */

    float           line_width;                   /* Line Width in user units */
    int             line_style;                   /* Style of Line */

    rgb_t        draw_color;                   /* Drawing color */
    rgb_t        fill_color;                   /* Filling color */
    rgb_t        text_color;                   /* Text color */
    int             color_flag;                   /* Fill and text colors are different */

    int                 inuser_func;              /* TRUE/FALSE user functon being processed */
    pdf_user_func_t  header_func;              /* User defined function for header */
    pdf_user_func_t  footer_func;              /* User defined function for footer */
    pdf_user_func_t  accept_func;              /* User defined function for accecpt auto page break */

    int            deflate_it;                    /* Compress PDF output (do not affect images) */

    char           tmp[PDF_MAX_TEMP];             /* For internal conversions */

    FILE          *fp;                            /* Output FILE */

    int            error;                         /* Last error in the document */
} pdf_t;

/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
   Convert a time_t to a PDF date string:

    YYYYMMDDddhhss

   CAREFUL:

        It returns a pointer to a read-only buffer
        that will change as soon as someone else call
        the function.

    Example:

        pdf_set_metadata(pdf,PDF_DOC_CREATION_DATE,pdf_date(time(NULL)));

 * -------------------------------------------------- */

const char *pdf_datetime(time_t t);

/* ------------------------------------------------------------  *
   Format a number using the given format, that is:

   tsep = Thousands separator (tipically a , . or space)
   dsep = Decimal separator (tipically a , or .)
   ndec = Number of decimals
   num  = Number to format
   symb = Currency symbol. If not 0 shows the currency symbol
          in the appropiate position: before the number in 
          pounds, dollars and yen, and after in euros and
          other currencies not handled.

    Examples of formats:

      1234567    = 1.234.567 (Spanish)
                   1,234,567 (English)

      1234567.89 = 1.234.567,89 (Spanish)
                   1,234,567.89 (English)

    If curr (currency) is other than 0 (PDF_CURR_NONE), it 
    formats with the given currency symbol.

      1.234.567,89€  or  1,234,567.89€  (PDF_CURR_EURO)
      £1.234.567,89  or  £1,234,567.89  (PDF_CURR_POUND)
      $1.234.567,89  or  $1,234,567.89  (PDF_CURR_DOLLAR)
 * ------------------------------------------------------------ */

 wchar_t *pdf_format_num(wchar_t *dest,size_t max,wchar_t tsep,wchar_t dsep,int ndec,double num,int curr);

/* -------------------------------------------------- *
   Create a context for creating PDF files using the
   default values:
 * -------------------------------------------------- */

pdf_t *pdf_create(int orientation,int unit,int size);

/* -------------------------------------------------- *
   Destroy a PDF context freeing all the resources
   and closing the file if open.
 * -------------------------------------------------- */

void pdf_destroy(pdf_t *pdf);

/* -------------------------------------------------- *
   Closes a PDF document so updates cannot be done to
   it except for calling some functions like:

   pdf_output(), pdf_set_compression(),
   pdf_set_display_mode(),
 * -------------------------------------------------- */

int pdf_close(pdf_t *pdf);

/* -------------------------------------------------- *
   Output a PDF to a file or screen (file == NULL)
   It closes the document if not closed
 * -------------------------------------------------- */

int pdf_output(pdf_t *pdf,const char *file);

/* -------------------------------------------------- *
   Return the dimensions and the position of the
   abscissa (x) and the ordinate (y)
 * -------------------------------------------------- */

float pdf_get_x(pdf_t *pdf);
float pdf_get_y(pdf_t *pdf);
float pdf_get_width(pdf_t *pdf);
float pdf_get_height(pdf_t *pdf);

/* -------------------------------------------------- *
    Set x and/or y.
    when setting y, set resetx as TRUE to also
    reset x to the start of y
 * -------------------------------------------------- */

void pdf_set_x(pdf_t *pdf,float x);
void pdf_set_y(pdf_t *pdf,float y,int resetx);

/* -------------------------------------------------- *
    pdf_line_feed() resets the abscissa (x) to
    the left margin and advance the ordinate (y)
    the size of the last drawed cell (0 if no cell
    has been drawn)

    pdf_line_break() is the same but it advances
    the ordinate h units instead.
 * -------------------------------------------------- */

void pdf_line_feed(pdf_t *pdf);
void pdf_line_break(pdf_t *pdf,float h);

float pdf_get_line_height(pdf_t *pdf);
float pdf_get_cell_height(pdf_t *pdf);

/* -------------------------------------------------- *
   Return the name of the current font, its size in
   points, family and style
 * -------------------------------------------------- */

const char *pdf_get_font_name(pdf_t *pdf);
int pdf_get_font_size(pdf_t *pdf);

const char *pdf_get_font_family(pdf_t *pdf);
int pdf_get_font_style(pdf_t *pdf);


/* -------------------------------------------------- *
   Return the orientation, size and unit
   of the current page or the document if pages has
   not been added.
 * -------------------------------------------------- */

int pdf_get_orientation(pdf_t *pdf);
int pdf_get_size(pdf_t *pdf);
int pdf_get_unit(pdf_t *pdf);

/* -------------------------------------------------- *
   Same as previous but instead it returns a char
   or wchar_t string
 * -------------------------------------------------- */

const char    *pdf_get_orientationc(pdf_t *pdf);
const wchar_t *pdf_get_orientationw(pdf_t *pdf);

const char    *pdf_get_sizec(pdf_t *pdf);
const wchar_t *pdf_get_sizew(pdf_t *pdf);

const char    *pdf_get_unitc(pdf_t *pdf);
const wchar_t *pdf_get_unitw(pdf_t *pdf);

/* -------------------------------------------------- *

    Set a document's metadata field.
    Possible values of which:

    PDF_DOC_TITLE          = Document's Title
    PDF_DOC_AUTHOR         = Document's Author
    PDF_DOC_SUBJECT        = Document's Subject
    PDF_DOC_KEYWORDS       = Document's Keywords
    PDF_DOC_PRODUCER       = Document's Producer
    PDF_DOC_CREATOR        = Document's Creator
    PDF_DOC_CREATION_DATE  = Document's creation Date

 * -------------------------------------------------- */

int pdf_set_metadata(pdf_t *pdf,int which,const wchar_t *fmt,...);

wchar_t *pdf_get_metadata(pdf_t *pdf,int which);


/* -------------------------------------------------- *
    Set if compression should be used with stream
    objects in the PDF. Default is TRUE.

    It DOES NOT have an effect on images, which are
    always compressed.
 * -------------------------------------------------- */

void pdf_set_compression(pdf_t *pdf,int on);

/* -------------------------------------------------- *
   Defines the left, top and right margins.
   By default, they equal 1 cm. Call this functions
   to change them.

   When changing the left margin, if the abcissa gets
   out of margin is brought back.
 * -------------------------------------------------- */

void pdf_set_top_margin(pdf_t *pdf,float mar);
void pdf_set_left_margin(pdf_t *pdf,float mar);
void pdf_set_right_margin(pdf_t *pdf,float mar);
void pdf_set_bottom_margin(pdf_t *pdf,float mar);
void pdf_set_margins(pdf_t *pdf,float left,float top,float right,float bottom);

/* -------------------------------------------------- *
   Set left and right margins of cells to be drawn
   from now on. It also resets the last cell height
 * -------------------------------------------------- */

void pdf_set_cell_margin(pdf_t *pdf,float mar);

/* -------------------------------------------------- *
   Read the left, top and right margins.
   By default, they equal 1 cm. Call this functions
   to change them.

   When changing the left margin, if the abcissa gets
   out of margin is brought back.
 * -------------------------------------------------- */

float pdf_get_top_margin(pdf_t *pdf);
float pdf_get_left_margin(pdf_t *pdf);
float pdf_get_right_margin(pdf_t *pdf);
float pdf_get_bottom_margin(pdf_t *pdf);

/* -------------------------------------------------- *
   Set the font, style and size to be used when
    writing text. to change things separately,
    send whatever you do not want to change as
    NULL for the family, -1 for style and -1 for size

   'family' is a text string with the family to use
   and 'style' is a bitmask that can be one or a
   combination of these:

           PDF_FNT_REGULAR
           PDF_FNT_ITALIC
           PDF_FNT_BOLD
           PDF_FNT_UNDERLINE

   The following font families are internally defined
   so no need to load a file associated to them:

        "helvetica" ("arial" works too)
        "courier"
        "times"
        "symbol"
        "zapfdingbats"

 * -------------------------------------------------- */

int pdf_set_font(pdf_t *pdf,const char *family,int style,float size);

/* -------------------------------------------------- *
   Load a TTF font from a file to embed in the PDF
   document and assign a family name or use the real
   family name from the file by sending family as
   NULL.

   Note that if the call is succesful (returns OK)
   the font will be set as the current font, thus you
   can actually find out the family name and style
   by calling pdf_get_font_family() and/or
    pdf_get_font_style() just after this call
 * -------------------------------------------------- */

int pdf_load_font(pdf_t *pdf,const char *family,const wchar_t *file);

/* -------------------------------------------------- *
    Set the style and size of the current font

    pdf_set_font_size() only changes size
    pdf_set_font_style() only changes  style
 * -------------------------------------------------- */

int pdf_set_font_size(pdf_t *pdf,float size);
int pdf_set_font_style(pdf_t *pdf,int style);

/* -------------------------------------------------- *
    Add a page to the end of the document or insert
    one after the current one (of course if you
    insert from the last one you will be adding it)
 * -------------------------------------------------- */

int pdf_insert_page(pdf_t *pdf);
int pdf_add_page(pdf_t *pdf);

/* -------------------------------------------------- *
   Return the number of pages and the current one
   (0 means no pages)
 * -------------------------------------------------- */

int pdf_total_pages(pdf_t *pdf);
int pdf_page_number(pdf_t *pdf);

/* -------------------------------------------------- *
   Set page n as the current page so you can write on
   it. It also moves the abcissa (x) and the  ordinate
   (y) to the left and top margin respectively.

   This function has not effect inside the user
   defined functions for footer, header or accept
   break;

   Despite that, you should use this with caution, as
   if you move to a page and auto page break is on, you
   may insert pages inadvertely just by generating
   output.

   Note that you can add a page right after the current
   one by calling pdf_insert_page() instead of
   pdf_add_page() which always add it at the end.

   if you set n as minor than 1 it will go page 1,
   and if you try to set as current a page beyond the
   last one, the last one is selected.
 * -------------------------------------------------- */

int pdf_go_to_page(pdf_t *pdf,int n);

/* -------------------------------------------------- *
    Enables or disables the automatic page breaking
    mode. When autob is TRUE, the second parameter
    is the distance from the bottom of the page that
    triggers a page break. By default, the mode is
    on and the margin is 2 cm.

    if mar is 0, then it enables disables de auto
    without changing hte margin
 * -------------------------------------------------- */

void pdf_set_auto_page_break(pdf_t *pdf,int autob,float mar);

/* -------------------------------------------------- *
    Determine the way the document is going to be
    shown to the user (see macros above).

    zoom   = a defined mode like PDF_PAGE_FULL for
             full page or a zooming factor expressed
             in percentage between  5 to 3000

    layout = is the page layout like PDF_PAGE_SINGLE
             for a single page or any of the others
 * -------------------------------------------------- */

void pdf_set_display_mode(pdf_t *pdf,int zoom,int layout);

/* -------------------------------------------------- *
   Sets a user function which is called whenever a
   page break condition is met, and the break is
   issued or not depending on the returned TRUE/FALSE
   value.

   The default function returns a value according to
   the mode selected by pdf_set_auto_page_break()
 * -------------------------------------------------- */

int pdf_set_accept_auto_page_break(pdf_t *pdf,pdf_user_func_t func);

/* -------------------------------------------------- *
   Set user functions to be called to render the page
   header and footer. They are automatically called
   when adding a new page and/or closing the document

   The default functions are empty, so you have to
   use these if you want a specific processing of
   header and footers.
 * -------------------------------------------------- */

int pdf_set_header_function(pdf_t *pdf,pdf_user_func_t func);
int pdf_set_footer_function(pdf_t *pdf,pdf_user_func_t func);

/* -------------------------------------------------- *
   Sets a string to be replaced for the number of
   pages in the document so for example a footer
   like:

   int pn = pdf_page_number(pdf);

   pdf_cell(pdf,100,100,"Page %d/{nb}",pn);

   The default function returns a value according to
   the mode selected by pdf_set_auto_page_break()
 * -------------------------------------------------- */

void pdf_set_page_number_alias(pdf_t *pdf,const wchar_t *alias,...);

/* -------------------------------------------------- *
   Set the current drawing, fill and text colors
 * -------------------------------------------------- */

void pdf_set_text_color(pdf_t *pdf,unsigned char r, unsigned char g, unsigned char b);
void pdf_set_fill_color(pdf_t *pdf,unsigned char r, unsigned char g, unsigned char b);
void pdf_set_draw_color(pdf_t *pdf,unsigned char r, unsigned char g, unsigned char b);

/* -------------------------------------------------- *
   Set the line drawing width (in units) as well as
   the line style of the available:

    PDF_SOLID_LINE      Solid line
    PDF_DOTTED_LINE     Dotted line
    PDF_DASHED_LINE     Dashed line
    PDF_MORSE_LINE      Dot-Dash-Dot-Dash line
    PDF_DOTGAP_LINE     Dotted line with bigger gap
    PDF_DASHGAP_LINE    Dashed line with bigger gap

   Note that if the style doens't exist it selects
   solid.
 * -------------------------------------------------- */

void pdf_set_line_width(pdf_t *pdf,float w);
void pdf_set_line_style(pdf_t *pdf,int what);

/* -------------------------------------------------- *
   Return the dimensions of a string in user units,
   the measure is based on the current font family,
   style and size.
 * -------------------------------------------------- */

float pdf_get_text_width(pdf_t *pdf,const wchar_t *fmt,...);

void pdf_get_text_rect(pdf_t *pdf,int *w,int *h,const wchar_t *fmt,...);

/* -------------------------------------------------- *
   Return the font size needed for a string to fit
   in 'width' or 'height' document units using the
   current font family and style.

   pdf_get_font_size_for_rect() tries to find a
   font size that will fit both, but the result will
   depend of the shape of the rectangle.
 * -------------------------------------------------- */

float pdf_get_font_size_for_width(pdf_t *pdf,float width,const wchar_t *fmt,...);
float pdf_get_font_size_for_height(pdf_t *pdf,float height);

float pdf_get_font_size_for_rect(pdf_t *pdf,float w,float h,const wchar_t *fmt,...);


/* -------------------------------------------------- *
    Create a link space for the current document.

    The functión creates an entry for a link and
    returns its id to be used to set a destination
    with either pdf_set_link_local_dest() or
    pdf_set_link_uri_dest() and later passed to
    the functions that draw elements for them to
    become the origin of the link when clicked.

    This two-step method to build links allows for
    multiple elements to be the origin for the same
    link target by simply giving them the link id
    returned by this function.

    IMPORTANT: when defining elements as origin you
    must remember that the links are bound to areas
    not shapes, so when selecting an element as
    origin, you are in fact defining as origin the
    area they occupy. This means you should avoid
    nesting clickable elements to avoid confusing
    the readers.
 * -------------------------------------------------- */

int pdf_add_target_link(pdf_t *pdf);

/* -------------------------------------------------- *
    Set destination of a target link to either a
    page of the current document or to a URI.

    id  = Link ID returned by pdf_add_target_link()
    page = destination page (0 for current)
       y = destiantion y postion (-1 for current)
    uri  = destination URI
 * -------------------------------------------------- */

int pdf_set_target_link_as_local(pdf_t *pdf,int id,int page, float y);
int pdf_set_target_link_as_uri(pdf_t *pdf,int id,const char *uri);

/* -------------------------------------------------- *
   Define if links borders are drawn.

   This works even for a closed document the same as
   pdf_set_compression().

   Therefore you could output a document with no
   borders in the links and another one with them.

 * -------------------------------------------------- */

void pdf_set_links_border(pdf_t *pdf,int on);


/* -------------------------------------------------- *
    Load an image file into a document and returns
    an id that can be used with pdf_draw_image()
    to draw it as many times as you want on the
    document or -1 if it could not load the image

    Formats: PCX, PNG, JPEG, BMP, GIF, CEL & DEGAS.
 * -------------------------------------------------- */

int pdf_load_image(pdf_t *pdf,wchar_t *file);

/* -------------------------------------------------- *
    Add image loaded with pdg_load_image().

    It also allows an optional link id returned by
    pdf_add_target_link() so the image becomes the
    origin of that link.

    Send 'lnk' as 0 to ignore.
 * -------------------------------------------------- */

int pdf_draw_image(pdf_t *pdf,float x,float y,float w,float h,int id,int lnk);


/* -------------------------------------------------- *
    Draw a line from dx,dy to hx,hy (This DO NOT
    update the abcissa (x) nor the coordinate (y)

    send any of the coordinates as -1 to use the
    current abscissa or coordinate.
 * -------------------------------------------------- */

void pdf_draw_line(pdf_t *pdf,float dx,float dy,float hx,float hy);


/* -------------------------------------------------- *
    Move the abcissa (x) and the coordinate (y) of
    the document to (nx,ny).

    send any of the coordinates as -1 to leave the
    abscissa or coordinate in its current place.

    Therefore, doing:

        pdf_move_to(pdf,-1,-1);

    has not effect whatsoever
 * -------------------------------------------------- */

void pdf_move_to(pdf_t *pdf,float nx,float ny);

/* -------------------------------------------------- *
   Draw a line from the current abcissa (x) and
   coordinte (y) to x,y, which become the new
   abcissa and coordinate.
 * -------------------------------------------------- */

void pdf_line_to(pdf_t *pdf,float x,float y);

/* -------------------------------------------------- *
    Draw a line from dx,dy to hx,hy.

    Send any of the coordinates as -1 to use the
    current abscissa or coordinate.
 * -------------------------------------------------- */

void pdf_draw_line(pdf_t *pdf,float dx,float dy,float hx,float hy);

/* -------------------------------------------------- *

    Draw a rectangle of w x h units at x,y using
    'attr', which is a bitmask that defines the
    drawing attributtes:

          PDF_A_FILLED = Fill the background

          PDF_A_FULL_B = Draw a full border

          PDF_A_LFT_B  = Draw a left border
          PDF_A_TOP_B  = Draw a top border
          PDF_A_RGT_B  = Draw a right border
          PDF_A_BOT_B  = Draw a bottom border

          PDF_A_RND_B  = Round all borders

          PDF_A_RND_TL = Round Top Left corner
          PDF_A_RND_BL = Round Bottom Left corner
          PDF_A_RND_TR = Round Top Right corner
          PDF_A_RND_BR = Round Bottom Right corner

    It also allows an optional link id returned by
    pdf_add_target_link() so the rectangle
    becomes the origin of that link.

    Send 'lnk' as 0 to ignore.
 * -------------------------------------------------- */

void pdf_draw_rectangle(pdf_t *pdf,float x,float y,float w,float h,int attr,int lnk);


/* -------------------------------------------------- *
    Draw ellipses and circles using the same attr
    than the rectangles, exceptfor the case in which
    you want to draw just segments of the circle
    or ellipse. Then you need to use the rounded
    corners flags:

          PDF_A_RND_F  = Draw a full border

          PDF_A_RND_TL = Round Top Left corner
          PDF_A_RND_BL = Round Bottom Left corner
          PDF_A_RND_TR = Round Top Right corner
          PDF_A_RND_BR = Round Bottom Right corner

    The rectangular ones like PDF_A_LFT_B will not
    work, although PDF_A_FULL_B will work just
    like PDF_A_RND_F and draw a full border. We let
    this happen because it allows a programmer to set
    attributes that will work with both shapes at the
    same time.

    They also allow an optional link id returned by
    pdf_add_target_link() so the circle/ellipse
    becomes the origin of that link.

    Send 'lnk' as 0 to ignore.

    Its important to remember that links are built
    from rectangular areas, so the area outside the
    round areas but inside the rectangle are active
 * -------------------------------------------------- */

void pdf_draw_ellipse(pdf_t *pdf,float x,float y,float rx,float ry,int attr,int lnk);
void pdf_draw_circle(pdf_t *pdf,float x,float y,float r,int attr,int lnk);

/* -------------------------------------------------- *
    Draw text at abscissa (x) and ordinate (y) with
    current attributes: color, etc.
    It uses the same format as printf()
 * -------------------------------------------------- */

void pdf_draw_text(pdf_t *pdf,float x,float y,const wchar_t *fmt,...);

/* -------------------------------------------------- *
    Draw text in flowing mode at abscissa (x) and
    ordinate (y) with current attributes: color, etc.
    as pdf_text() does. It also uses the same format
    as printf().

    Flowing mode means that it draws the text with
    automatic or explicit line feeds.

    It also allows an optional link id returned by
    pdf_add_target_link() so the text becomes the
    origin of that link. Send 'lnk' as 0 to ignore.
 * -------------------------------------------------- */

void pdf_write_text(pdf_t *pdf,int lnk,const wchar_t *fmt,...);

/* -------------------------------------------------- *
    Renders HTML in the best possible way given the
    limitations of a rudimentary HTML parser.
 * -------------------------------------------------- */

void pdf_write_html(pdf_t *pdf,const wchar_t *fmt,...);

/* -------------------------------------------------- *
    Draw a cell of w,h at current abscissa (x) and
    ordinate (y) using the given attributes.

    Send fmt as NULL if you do not want to draw text
    inside the cell, note that text is truncated if
    doesn't fit in the cell

    'attr' its a bitmask that defines the drawing
    attributtes of the cell, which are the same
    as pdf_draw_rectangle() plus these:

       PDF_A_FIT_TEXT = Fit text horizontally
                        instead of truncate

       PDF_A_LEFT_J   = Draw text justified left
       PDF_A_RIGHT_J  = Draw text justified right
       PDF_A_CENTRE_J = Draw text centered
       PDF_A_FULL_J   = Draw text fully justified

    as well as the way to move the abscissa (x) and/or
    ordinate (y) after the cell is drawn:

       PDF_A_RIGHT_F = Move abscissa to the right

       PDF_A_LINE_F  = Line Feed. Advance ordinate
                       the height of a cell and the
                       abscissa to the left margin

       PDF_A_BELOW_F = Mode abscissa and coordinate
                       to inmediate below this one.
                       That is to same abcissa as
                       before drawing and the
                       ordinate the height of the
                       cell.

    If you send attr as 0 it uses the default
    values: no border, no fill, feed right.

    It also allows an optional link id returned by
    pdf_add_target_link() so the text becomes the
    origin of that link. Send 'lnk' as 0 to ignore.

    multi_cell() is the same as pdf_cell() but
    it draws the text with automatic or explicit
    line feeds in the fashion of pdf_write_text()
    as well as ignoring the attributes for moving the
    abcissa and ordinate, as it just does PDF_A_FEED
    after drawing the cells.

    multi_cell() doesn't accept links
 * -------------------------------------------------- */

void pdf_draw_cell(pdf_t *pdf,float w,float h,int attr,int lnk,const wchar_t *fmt,...);
void pdf_draw_multi_cell(pdf_t *pdf,float w,float h,int attr,const wchar_t *fmt,...);

/* -------------------------------------------------- *
    Draws a ruler measuring millimeters

    x,y is the origin of the ruler in current units,
    -1 in any of them means current abcsisa (x) or
    ordinate (y) accordingly.

    w is the lengh of the ruler in millimeters.

    n is the first number to put in the ruler

    top is TRUE/FALSE draw horizontal ruler with the
    marks on the top rather than bottom

    left is TRUE/FALSE draw vertical ruler with the
    marks to the left rather than to the right.

 * -------------------------------------------------- */

void pdf_draw_horz_mm_ruler(pdf_t *pdf,float x,float y,float w,int n,int top);
void pdf_draw_vert_mm_ruler(pdf_t *pdf,float x,float y,float w,int n,int left);

/* -------------------------------------------------- *
    Draw a barcode at x,y using the appropiate format
    which dependes on the lenght of the code:

    13 Digits for EAN-13
    12 Digits for UPC-A with a 0 prefix
     8 Digits for EAN-8
     5 Digits for EAN-5
     2 Digits for EAN-2

    x == -1 to use current abscissa (x) and y == -1
    to use current ordinate (y).

    w is barcode width (0 is default for the type)

    It also allows an optional link id returned by
    pdf_add_target_link() so the text becomes the
    origin of that link. Send 'lnk' as 0 to ignore.

    bkg is TRUE/FALSE draw the white background.

    Composited Codes:

    EAN-13+5 (with format XXXXXXXXXXXX+XXXXX) is an
    EAN-13 barcode with an accompaning EAN-5 barcode
    usually used to put a price tag to an EAN-13.

    EAN-13+2 (with format XXXXXXXXXXXX+XX) is an
    EAN-13 barcode with an accompaning EAN-2 barcode
    usually used to put an issue number to an EAN-13.

    EAN-13-5 and EAN-13-2 (using a minus char instead
    a plus on the format) is the same as the previous
    but the EAN-2 and EAN-5 show the legend at the
    bottom rather than at the top of the barcode.

    With this we aim to cover all barcode needs that
    document writers may need. Eg for printing labels 
    or book covers/backs that show a barcode. 
 * -------------------------------------------------- */

int pdf_draw_barcode(pdf_t *pdf,float x, float y, float w,int lnk,int bkg,const wchar_t *fmt,...);

#endif
