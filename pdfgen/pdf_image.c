/*
    imgutil.c

    Image utilities

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

        - Written by Guillermo Amodeo Ojeda using the info from:

            http://www.w3.org/TR/PNG
            http://en.wikipedia.org/wiki/Portable_Network_Graphics
            http://www.libpng.org/pub/png/spec/1.2/PNG-Decoders.html


        - The CRC-32 code was adapted by Guillermo Amodeo Ojeda from SSHCRC.C 
          that came with PUTTY 0.60

                             PUTTY's LICENSE

        PuTTY is copyright 1997-2007 Simon Tatham.

        Portions copyright Robert de Bath, Joris van Rantwijk, Delian
        Delchev, Andreas Schultz, Jeroen Massar, Wez Furlong, Nicolas Barry,
        Justin Bradford, Ben Harris, Malcolm Smith, Ahmad Khalifa, Markus
        Kuhn, and CORE SDI S.A.

        Permission is hereby granted, free of charge, to any person
        obtaining a copy of this software and associated documentation files
        (the "Software"), to deal in the Software without restriction,
        including without limitation the rights to use, copy, modify, merge,
        publish, distribute, sublicense, and/or sell copies of the Software,
        and to permit persons to whom the Software is furnished to do so,
        subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
        EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
        MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
        NONINFRINGEMENT.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
        FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
        CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
        WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



                                --oO0Oo--
*/


#include "pdf_image.h"

static const unsigned int crc32_table[256]=
{
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/* ------------------------------ */

static void crc32_update(unsigned int *ctx,const void *buf,size_t tam)
{
    unsigned char *ptr=(unsigned char *)buf;
    unsigned int idx,crc;
    size_t t;

    crc=~(*ctx);
    for(t=0;t<tam;t++)
    {
        idx = ptr[t];
        idx^= crc & 0xff;
        crc = (crc >> 8) ^ crc32_table[idx];
    }
   *ctx=~crc;
}

/* ------------------------ *
    Color formats
 * ------------------------ */

enum
{
    FMT_RGB_UNDEF=0, /* Undefined */
    FMT_GRAY8,       /* 256-tone Grayscale (8 BPP) */
    FMT_GRAYA,       /* 256-tone Grayscale with Alpha (16 BPP) */
    FMT_RGB_PAL ,    /* Palette (8 BPP) */
    FMT_RGB15,       /* RGB  5-5-5   (16 BPP) */
    FMT_BGR15,       /* BGR  5-5-5   (16 BPP) */
    FMT_RGB16,       /* RGB  5-6-5   (16 BPP) */
    FMT_BGR16,       /* BGR  5-6-5   (16 BPP) */
    FMT_RGB24,       /* RGB  8-8-8   (24 BPP) */
    FMT_BGR24,       /* BGR  8-8-8   (24 BPP) */
    FMT_RGB32,       /* RGB  8-8-8   (32 BPP) */
    FMT_BGR32,       /* BGR  8-8-8   (32 BPP) */
    FMT_RGBA,        /* RGBA 8-8-8-8 (32 BPP) */
    FMT_ARGB,        /* ARGB 8-8-8-8 (32 BPP) */
    FMT_BGRA,        /* BGRA 8-8-8-8 (32 BPP) */    

    NUM_COLOR_FORMATS
};

/* ------------------------------------------------------ *
   Most color conversion routines have the same prototype
   than memcpy() so we can use a function pointer -that
   we defined below- which can actually point to memcpy()
   if both origin and destination have the same color
   format.

   The functions called pal8_...() have an extra
   parameter which points to the palette, so they have
   to be managed by differently.
 * ------------------------------------------------------ */

typedef void *(*color_fun_t)(void *dest,const void *orig,size_t num);

/* -------------------------------------- */

static void pal8_to_rgb(void *dest,const void *orig,size_t count,rgb_t *pal)
{
    unsigned char *po,*pd;
    size_t i,t,c;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        c=(int)po[t];
        pd[i++]=pal[c].red;
        pd[i++]=pal[c].green;
        pd[i++]=pal[c].blue;
    }
}

/* -------------------------------------- */

#define float_rgb_to_gray8(_r,_g,_b)  (unsigned char) ((((int)_r * 59) + ((int)_g * 150) + ((int)_b * 29))  >> 8)

/* -------------------------------------- */

static void *gray8_to_rgba(void *dest,const void *orig,size_t count)
{
    unsigned char *po,*pd,c;
    size_t t;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;

    for(t=0;t<count;t++)
    {
        c = po[t];
       *pd++=c;
       *pd++=c;
       *pd++=c;
       *pd++=0xFF;
    }
    return dest;
}

/* -------------------------------------- */

static void *gray8_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *po,*pd,c;
    size_t t;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;

    for(t=0;t<count;t++)
    {
        c = po[t];
       *pd++=c;
       *pd++=c;
       *pd++=c;
    }
    return dest;
}

/* -------------------------------------- */

static void *grayA_to_rgba(void *dest,const void *orig,size_t count)
{
    unsigned char *po,*pd;
    size_t t;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;

    for(t=0;t<count;t++)
    {
       *pd++=po[0];
       *pd++=po[0];
       *pd++=po[0];
       *pd++=po[1];
        po+=2;
    }
    return dest;
}

/* -------------------------------------- */

static void *rgb_to_rgba(void *dest,const void *orig,size_t count)
{
    unsigned char *po,*pd;
    size_t t;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;

    for(t=0;t<count;t++)
    {
       *pd++=*po++;
       *pd++=*po++;
       *pd++=*po++;
       *pd++=0xFF;
    }
    return dest;
}


/* -------------------------------------- */

static void *invert_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *po,*pd;
    size_t i,t;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        pd[i++]=po[2];
        pd[i++]=po[1];
        pd[i++]=po[0];
        po+=3;
    }
    return dest;
}

/* -------------------------------------- */

static void *rgb15_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd;
    unsigned short *po,val;
    size_t i,t;

    po=(unsigned short *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        val = po[t];
        pd[i++]=(unsigned char)((val & 0x7C00) >> 7);
        pd[i++]=(unsigned char)((val & 0x03E0) >> 2);
        pd[i++]=(unsigned char)((val & 0x001F) << 3);
    }
    return dest;
}

/* -------------------------------------- */

static void *bgr15_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd;
    unsigned short *po,val;
    size_t i,t;

    po=(unsigned short *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        val = po[t];
        pd[i++]=(unsigned char)((val & 0x001F) << 3);
        pd[i++]=(unsigned char)((val & 0x03E0) >> 2);
        pd[i++]=(unsigned char)((val & 0x7C00) >> 7);
    }
    return dest;
}

/* -------------------------------------- */

static void *rgb16_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd;
    unsigned short *po,val;
    size_t i,t;

    po=(unsigned short *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        val = po[t];
        pd[i++]=(unsigned char)((val & 0xF800) >> 8);
        pd[i++]=(unsigned char)((val & 0x07E0) >> 3);
        pd[i++]=(unsigned char)((val & 0x001F) << 3);
    }
    return dest;
}

/* -------------------------------------- */

static void *bgr16_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd;
    unsigned short *po,val;
    size_t i,t;

    po=(unsigned short *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        val = po[t];
        pd[i++]=(unsigned char)((val & 0x001F) << 3);
        pd[i++]=(unsigned char)((val & 0x07E0) >> 3);
        pd[i++]=(unsigned char)((val & 0x7C00) >> 8);
    }
    return dest;
}

/* -------------------------------------- */

static void *rgb32_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd;
    unsigned int *po,val;
    size_t i,t;

    po=(unsigned int *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        val = po[t];
        pd[i++]=(unsigned char)((val >> 16) & 0xFF);
        pd[i++]=(unsigned char)((val >> 8) & 0xFF);
        pd[i++]=(unsigned char)(val & 0xFF);
    }
    return dest;
}

/* -------------------------------------- */

static void *bgr_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd,*po;
    size_t t;

    po=(unsigned char *)orig;
    pd=(unsigned char *)dest;

    for(t=0;t<count;t++)
    {
        pd[0]=po[2];
        pd[1]=po[1];;
        pd[2]=po[0];;
        po+=3;
        pd+=3;
    }
    return dest;
}

/* -------------------------------------- */

static void *bgr32_to_rgb(void *dest,const void *orig,size_t count)
{
    unsigned char *pd;
    unsigned int *po,val;
    size_t i,t;

    po=(unsigned int *)orig;
    pd=(unsigned char *)dest;
    for(i=t=0;t<count;t++)
    {
        val = po[t];
        pd[i++]=(unsigned char)(val & 0xFF);
        pd[i++]=(unsigned char)((val >> 8) & 0xFF);
        pd[i++]=(unsigned char)((val >> 16) & 0xFF);
    }
    return dest;
}

/* -------------------------------------- *
    Image in Memory
 * -------------------------------------- */

typedef struct
{
    int    width;        /* Width in pixels */
    int    height;       /* Height in pixels */
    int    bpp;          /* Bits per pixel */    
    int    bpl;          /* Bytes per line */
    int    fc;           /* Color format */
    rgb_t  pal[256];     /* Color palette if 8 bits image */
    void  *pixels;       /* Pixels */
} image_t;

/* -------------------------------------- */

static void image_destroy(image_t *img)
{
    if(img->pixels)
    {
        free(img->pixels);
        img->pixels = NULL;
    }
    memset(img,0,sizeof(image_t));    
}

/* -------------------------------------- */

static int image_modify(image_t *img,int width,int height,int fc)
{
    void *mem;
    size_t tam;
    int bpl,bpp;

    if(width < 1)
        width=img->width;

    if(height < 1)
        height=img->height;

    if(fc < 1)
        fc=img->fc;

    if(fc==img->fc && width==img->width && height==img->height)
        return TRUE;

    switch(fc)
    {
        case FMT_RGB_PAL:
        case FMT_GRAY8:
            bpp=8;            
            break;
        case FMT_GRAYA:
        case FMT_RGB15:
        case FMT_BGR15:
        case FMT_RGB16:
        case FMT_BGR16:        
            bpp=16;            
            break;
        case FMT_RGB24:
        case FMT_BGR24:
            bpp=24;            
            break;
        case FMT_RGB32:
        case FMT_BGR32:
        case FMT_RGBA:
        case FMT_ARGB:
        case FMT_BGRA:
            bpp=32;            
            break;
        default:
            return FALSE;
    }

    bpl=((((width) * (bpp) + 31) / 32) * 4);

    tam = (size_t) bpl;
    tam *= height;

    mem=realloc(img->pixels,tam);

    if(mem == NULL)
        return FALSE;

    memset(mem,0,tam);
    
    img->pixels=mem;
    img->width=width;
    img->height=height;
    img->bpp=bpp;
    img->bpl=bpl;    
    img->fc=fc;

    return TRUE;
}

/* ------------------------------------------------- *
    PNG Loading Routines
 * ------------------------------------------------- */

static unsigned char id_png[8]={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A}; /* PNG Identifier */

/*
    Array for loading PNG interlaced with Adam7.

    http://www.libpng.org/pub/png/spec/1.2/PNG-Decoders.html#D.Progressive-display

*/

static int  e_org_x[7]={ 0, 4, 0, 2, 0, 1, 0 };
static int  e_org_y[7]={ 0, 0, 4, 0, 2, 0, 1 };
static int  e_inc_x[7]={ 8, 8, 4, 4, 2, 2, 1 };
static int  e_inc_y[7]={ 8, 8, 8, 4, 4, 2, 2 };

/* Working Context */

typedef struct
{
    int             maxf;   /* Numero de frames de la animacion */
    int             numf;   /* Numero de Fotogramas procesados */    

    int            tengo_idat;    /* El primer IDAT ha sido leido */
    int            hay_trans;     /* Hay transparencia */
    int            compresion;
    int            filtro;
    int            entrelazado;
    image_t       *img;           /* Imagen destino */
    int            depth;
    int            tipo_color;
    unsigned char  trns[256];     /* Paleta de transparencia */
    rgb_t          pal[256];      /* Paleta de colores */
    int            num_pal;

    int             e_bpl[7];      /* Tamaño en bytes de cada linea de los diferentes pases del entrelazado */
    int             e_width[7];    /* Anchura de los diferentes pases del entrelazado */
    int             e_height[7];   /* Altura de los diferentes pases del entrelazado */
    int             e_pase;        /* Pase actual del entrelazado */

    int             linea;         /* Linea que estamos procesando */
    z_stream        z;             /* Contexto ZLib */
    unsigned char  *lineas;        /* Buffer de lineas (para el filtrado) */
    unsigned char  *prev,*actual;  /* Punteros al buffer de lineas */
    int             bpl;           /* Bytes por linea (sin contar el byte de filtro) */
    int             bpp;           /* Bytes por pixel */
    unsigned char  *buf;           /* Buffer para los chunks */
    unsigned char  *png;           /* Buffer PNG que estamos decodificando */
    size_t          tam;           /* tamaño restante en el buffer */
} png_ctx_t;

/* Tipos de Chunk */

enum
{
    PNG_UNKN = -1,  /* Desconocido */
    PNG_IHDR,       /* Cabecera */
    PNG_PLTE,       /* Paleta de Colores */
    PNG_tRNS,       /* Información de transparencia */
    PNG_IDAT,       /* Datos de la Imagen */
    PNG_IEND,       /* Ultimo Chunk de la imagen */
    PNG_PHYS,       /* Dimensiones fisicas de los pixeles */
};

/* -------------------------- */

static int get_chunk_header(png_ctx_t *ctx,int *tipo,unsigned int *tam)
{
    unsigned int tmp;

    /* Ver si hay algun chunk */
    if(ctx->tam < 8)
        return FALSE;
    /* Leer tamaño */
    *tam=get_be32(ctx->png);
    /* Leer tipo */
    tmp=get_be32(ctx->png + 4);
    switch(tmp)
    {
        case 0x49484452:
           *tipo=PNG_IHDR;
            break;
        case 0x74524E53:
           *tipo=PNG_tRNS;
            break;
        case 0x70485973:
           *tipo=PNG_PHYS;
            break;
        case 0x504C5445:
           *tipo=PNG_PLTE;
            break;
        case 0x49444154:
           *tipo=PNG_IDAT;
            break;
        case 0x49454E44:
           *tipo=PNG_IEND;
            break;
        default:
           *tipo=PNG_UNKN;
            break;
    }
    /* Actualizar el contexto */
    ctx->png+=8;
    ctx->tam-=8;
    return TRUE;
}

/* -------------------------- */

static int read_chunk_data(png_ctx_t *ctx,unsigned char *tipo,unsigned int tam)
{
    unsigned int crc1,crc2 = 0;

    if(ctx->tam < (size_t)(tam + 4))
        return FALSE;
    ctx->buf=ctx->png;
    ctx->png+=tam;
    /* Leer CRC */
    crc1=get_be32(ctx->png);
    ctx->png+=4;
    ctx->tam-=(tam + 4);
    /* Comprobar que es correcto */    
    crc32_update(&crc2,tipo,4);
    crc32_update(&crc2,ctx->buf,tam);    
    return (crc2!=crc1) ? FALSE : TRUE;
}

/* -------------------------- */

static int ignore_chunk_png(png_ctx_t *ctx,unsigned int tam)
{
    if(ctx->tam < (size_t)(tam + 4))
        return FALSE;
    ctx->png+=(tam + 4);
    ctx->tam-=(tam + 4);
    return TRUE;
}

/* -------------------------- */

static int read_IHDR_png(png_ctx_t *ctx,unsigned int tam)
{
    unsigned int tmp;
    int width,height;

    /* Comprobar que la header tiene el tamaño adecuado */

    if(tam < 13)
        return FALSE;

    /* Lee el chunk */

    if(!read_chunk_data(ctx,(unsigned char *)"IHDR",tam))    
        return FALSE;

    /* Dimensiones */

    tmp=get_be32(ctx->buf);
    width=(int)tmp;
    tmp=get_be32(ctx->buf + 4);
    height=(int)tmp;
    if(width < 1 || height < 1)
        return FALSE;

    /* Profundidad y tipo de color */

    ctx->depth=ctx->buf[8];
    ctx->tipo_color=ctx->buf[9];

    /* Compresion, Filtro y entrelazado */

    ctx->compresion=ctx->buf[10];
    ctx->filtro=ctx->buf[11];
    ctx->entrelazado=ctx->buf[12];
    /*
        Ahora nos aseguramos de que es un tipo de fichero PNG que podemos leer...

        (Referencia: http://www.w3.org/TR/PNG/#11IHDR )

        Profundidad   : 8 bits
        Tipo de color : 0 (GrayScale), 2 (Truecolor), 3 (Indexed), 4 (GrayScale + Alpha), y 6 (Truecolor + Alpha)
        Compresión    : DEFLATE (0)
        Filtro General: ADAPTIVE (0) - Independiente para cada linea
        Entrelazado   : No entrelazado (0) - Adam7 (1)
    */

    if(ctx->compresion || ctx->filtro || ctx->depth!=8)
        return FALSE;

    /* Determinar informacion de color */

    switch(ctx->tipo_color)
    {
        case 0:
            ctx->bpp=1; /* Escala de Grises */
            break;
        case 2:
            ctx->bpp=3; /* RGB Triple */
            break;
        case 3:
            ctx->bpp=1; /* Indexado (Paleta) */
            break;
        case 4:
            ctx->bpp=2; /* Escala de Grises + Alpha */
            break;
        case 6:
            ctx->bpp=4; /* RGB Triple + Alpha */
            break;
        default:
            return FALSE;
    }

    /* Determinar si esta entrelazado o no */

    if(ctx->entrelazado == 1)
    {
        int t;

        /* Solo permitimos Adam7 */

        if(ctx->entrelazado != 1) /* No es Adam 7 */
            return FALSE;

        /* Calcular los bloques de pixels de cada pase de Adam7 */

        for(t=0;t<7;t++)
        {
            ctx->e_width[t]=((width + e_inc_x[t] - 1 - e_org_x[t]) / e_inc_x[t]);
            ctx->e_height[t]=((height + e_inc_y[t] - 1 - e_org_y[t]) / e_inc_y[t]);
            ctx->e_bpl[t] = (ctx->e_width[t] * ctx->bpp);
        }

        ctx->e_pase = 0;
    }

    /* Siempre convertimos los PNG a RGBA porque es el formato de color que usamos
       para nuestras rutinas de conversion en iconos y bitmaps del sistema.
    */
    if(!image_modify(ctx->img,width,height,FMT_RGBA))    
        return FALSE;    

    /* bytes por linea  */

    ctx->bpl=(ctx->bpp * width);

    /* Crea buffers para lineas (contando el byte de filtro y el offset de filtrado) */

    ctx->lineas=(unsigned char *)calloc(ctx->bpl + 1,2);
    if(ctx->lineas==NULL)
        return FALSE;

    /* Preparar buffers de entrelazado e inicializar descompresion */

    ctx->actual=ctx->lineas;
    ctx->prev=ctx->lineas + (ctx->bpl + 1);
    ctx->linea=0;

    memset(&ctx->z,0,sizeof(ctx->z));
    return (inflateInit(&ctx->z)==Z_OK) ? TRUE : FALSE;
}

/* -------------------------- */

/* Referencia: http://www.gnupdf.org/PNG_and_TIFF_Predictors_Filter */

static int prediccion_paeth(int izda,int arriba,int arriba_izda)
{
    int pa,pb,pc,pp;

    pp=izda + arriba - arriba_izda;
    pa=abs(pp - izda);
    pb=abs(pp - arriba);
    pc=abs(pp - arriba_izda);
    if(pa <= pb && pa <= pc)
        return izda;
    if(pb <= pc)
        return arriba;
    return arriba_izda;
}

/* -------------------------------------- */

static void pal_to_rgba(unsigned char *dest,const unsigned char *orig,size_t cuantos,png_ctx_t *ctx)
{
    size_t t;
    int c;

    for(t=0;t<cuantos;t++)
    {
        c=(int)orig[t];

       *dest++=ctx->pal[c].red;
       *dest++=ctx->pal[c].green;
       *dest++=ctx->pal[c].blue;
       *dest++=ctx->trns[c];
    }
}

/* -------------------------- */

/* Referencia: http://www.w3.org/TR/PNG/#9Filters */

static void process_line(png_ctx_t *ctx)
{
    unsigned char *actual,*arriba,*pix;
    int filtro,a,b,c,x,t,i,bpl;

    bpl = ctx->entrelazado ? ctx->e_bpl[ctx->e_pase] : ctx->bpl;

    /* prepara punteros */

    actual=ctx->actual;
    arriba=ctx->prev + 1;

    /* Leer filtro, ponerlo a 0 y mover 'actual' hasta el primer pixel de la linea */

    filtro=*actual;
   *actual++=0;

    /* Filtrar la linea */

    switch(filtro)
    {
        case 0: /* Sin Filtro */
        default:
            break;

        case 1: /* Sub  */
            for(i=0,t=ctx->bpp;t<bpl;t++,i++)
            {
                x=actual[t];
                a=actual[i];
                actual[t]=(unsigned char)((x + a) & 0xFF);
            }
            break;

        case 2: /* Arriba */

            for(t=0;t<bpl;t++)
            {
                x=actual[t];
                b=arriba[t];
                actual[t]=(unsigned char)((x + b) & 0xFF);
            }
            break;

        case 3: /* Media */

            for(t=0;t<ctx->bpp;t++)
            {
                x=actual[t];
                b=arriba[t];
                actual[t]=(unsigned char)((x + (b / 2)) & 0xFF);
            }

            for(i=0;t<bpl;t++,i++)
            {
                x=actual[t];
                a=actual[i];
                b=arriba[t];
                actual[t]=(unsigned char)((x + ((a + b) / 2)) & 0xFF);
            }
            break;

        case 4: /* Predicción Paeth */

            for(t=0;t<ctx->bpp;t++)
            {
                x=actual[t];
                b=arriba[t];
                actual[t]=(unsigned char)((x + b) & 0xFF);
            }

            for(i=0;t<bpl;t++,i++)
            {
                x=actual[t];
                a=actual[i];
                b=arriba[t];
                c=arriba[i];
                actual[t]=(unsigned char)((x + prediccion_paeth(a,b,c)) & 0xFF) ;
            }
            break;
    }

    /* Copiar la linea en la imagen */

    pix=(unsigned char *)ctx->img->pixels;    

    if(ctx->entrelazado)
    {
        pix+=((e_org_y[ctx->e_pase] + (ctx->linea * e_inc_y[ctx->e_pase])) * ctx->img->bpl);

        a = (e_inc_x[ctx->e_pase] * 4);
        pix+=(e_org_x[ctx->e_pase] * 4);

        if(e_inc_x[ctx->e_pase]==1)
        {
            switch(ctx->tipo_color)
            {
                case 0: /* Escala de Grises */
                    gray8_to_rgba(pix,actual,ctx->e_width[ctx->e_pase]);
                    break;
                case 2: /* FMT_RGB24 */
                    rgb_to_rgba(pix,actual,ctx->e_width[ctx->e_pase]);
                    break;
                case 3: /* FMT_RGB_PAL */
                    pal_to_rgba(pix,actual,ctx->e_width[ctx->e_pase],ctx);
                    break;
                case 4: /* Escala de Grises + Alpha */
                    grayA_to_rgba(pix,actual,ctx->e_width[ctx->e_pase]);
                    break;
                case 6: /* FMT_RGBA */
                    memcpy(pix,actual,ctx->bpl);
                    break;
            }
        }
        else
        {
            for(x = 0; x < ctx->e_width[ctx->e_pase] ; x++)
            {
                switch(ctx->tipo_color)
                {
                    case 0: /* Escala de Grises */
                        pix[0]=actual[x];
                        pix[1]=actual[x];
                        pix[2]=actual[x];
                        pix[3]=0xFF;
                        break;
                    case 2: /* FMT_RGB24 */
                        pix[0]=*actual++;
                        pix[1]=*actual++;
                        pix[2]=*actual++;
                        pix[3]=0xFF;
                        break;
                    case 3: /* FMT_RGB_PAL */
                        b = actual[x];
                        pix[0]=ctx->pal[b].red;
                        pix[1]=ctx->pal[b].green;
                        pix[2]=ctx->pal[b].blue;
                        pix[3]=ctx->trns[b];
                        break;
                    case 4: /* Escala de Grises + Alpha */
                        pix[0]=*actual;
                        pix[1]=*actual;
                        pix[2]=*actual++;
                        pix[3]=*actual++;
                        break;
                    case 6: /* FMT_RGBA */
                        pix[0]=*actual++;
                        pix[1]=*actual++;
                        pix[2]=*actual++;
                        pix[3]=*actual++;
                        break;
                }
                pix+=a;
            }
        }
    }
    else
    {
        pix+=(ctx->linea * ctx->img->bpl);
        switch(ctx->tipo_color)
        {
            case 0: /* Escala de Grises */
                gray8_to_rgba(pix,actual,ctx->img->width);
                break;
            case 2: /* FMT_RGB24 */
                rgb_to_rgba(pix,actual,ctx->img->width);
                break;
            case 3: /* FMT_RGB_PAL */
                pal_to_rgba(pix,actual,ctx->img->width,ctx);
                break;
            case 4: /* Escala de Grises + Alpha */
                grayA_to_rgba(pix,actual,ctx->img->width);
                break;
            case 6: /* FMT_RGBA */
                memcpy(pix,actual,ctx->bpl);
                break;
        }
    }

    ctx->linea++;

    if(ctx->entrelazado)
    {
        if(ctx->linea == ctx->e_height[ctx->e_pase])
        {
            ctx->linea = 0;
            ctx->e_pase++;
        }
    }

    /* Rotar buffers de filtrado */

    if(ctx->actual==ctx->lineas)
    {
        ctx->actual=ctx->prev;
        ctx->prev=ctx->lineas;
    }
    else
    {
        ctx->prev=ctx->actual;
        ctx->actual=ctx->lineas;
    }
}

/* -------------------------- */

static int read_IDAT_png(png_ctx_t *ctx,unsigned int tam)
{
    int ret,terminado=0;    

    /* Segun el estandard PNG es legal tener paquetes IDAT vacios,
       no se que utilidad tendran, pero bueno, aqui les damos
       su sitio como manda el estandard. :-)
    */

    if(tam == 0)
        return TRUE;

    if(tam > 0x7FFFFFFF)
        return FALSE;

    if(!read_chunk_data(ctx,(unsigned char *)"IDAT",tam))
        return FALSE;

    /* Prepara Descompresión */

    ctx->z.next_in=(Bytef*)ctx->buf;
    ctx->z.avail_in=(uInt)tam;

    if(!ctx->z.avail_out)
    {
        ctx->z.next_out =(Bytef *)ctx->actual;

        if(ctx->entrelazado)
            ctx->z.avail_out = (uInt)(ctx->e_bpl[ctx->e_pase] + 1);
        else
            ctx->z.avail_out = (uInt)(ctx->bpl + 1);
    }

    /* Descomprime... */

    while(!terminado && ctx->z.avail_in)
    {
        ret=inflate(&ctx->z,Z_PARTIAL_FLUSH);
        switch(ret)
        {
            case Z_STREAM_END:
                terminado++;
            case Z_OK:
                break;
            //case Z_BUF_ERROR:
            default:
                return FALSE;
        }
        if(!ctx->z.avail_out)
        {
            process_line(ctx);
            ctx->z.next_out =(Bytef*)ctx->actual;
            if(ctx->entrelazado)
            {
                if(ctx->e_pase == 7)
                    terminado++;
                else
                    ctx->z.avail_out = (uInt)(1 + ctx->e_bpl[ctx->e_pase]);
            }
            else
            {
                if(ctx->linea < ctx->img->height)
                    ctx->z.avail_out = (uInt)(ctx->bpl + 1);
                else
                    terminado++;
            }
        }
    }
    return TRUE;
}

/* -------------------------- */

static int read_PLTE_png(png_ctx_t *ctx,unsigned int tam)
{
    int col,pos;

    if(tam > 2)
    {
        if(read_chunk_data(ctx,(unsigned char *)"PLTE",tam))
        {
            for(pos=col=0;col<256 && tam > 2;col++)
            {
                ctx->pal[col].red=ctx->buf[pos++];
                ctx->pal[col].green=ctx->buf[pos++];
                ctx->pal[col].blue=ctx->buf[pos++];
                tam-=3;
            }
            ctx->num_pal=col;
            return TRUE;
        }
    }
    return FALSE;
}

/* -------------------------- */

static int read_tRNS_png(png_ctx_t *ctx,unsigned int tam)
{
    if(tam > 0)
    {
        if(read_chunk_data(ctx,(unsigned char *)"tRNS",tam))
        {        
            ctx->hay_trans=TRUE;
            if(tam > 256)
                tam = 256;
            memcpy(ctx->trns,ctx->buf,tam);
            return TRUE;
        }
    }
    return FALSE;
}

/* -------------------------- */

static int read_png(image_t *img,const void *buf,size_t btam)
{
    int ret=TRUE,tengo_ihdr;
    int tipo;
    png_ctx_t ctx;
    unsigned int tam;    

    if(btam < 8)
        return FALSE;

    ctx.png=(unsigned char *)buf;
    ctx.tam=btam;
    ctx.num_pal=0;
    ctx.buf=NULL;
    ctx.img=img;
    ctx.lineas=NULL;
    ctx.prev=NULL;
    ctx.actual=NULL;
    ctx.maxf = 0;
    ctx.numf = 0;
    ctx.tengo_idat=FALSE;
    ctx.entrelazado = 0;
    ctx.e_pase = 0;
    memset(ctx.trns,0xff,256);

    /* chequear identidad */
    if(memcmp(id_png,ctx.png,8))
        return FALSE;
    ctx.png+=8;
    ctx.tam-=8;

    tengo_ihdr=FALSE;

    /* correcta, asi que vamos a cargar el PNG */
    while(ret && ctx.tam)
    {
        ret=get_chunk_header(&ctx,&tipo,&tam);
        if(!ret)
            continue;
        switch(tipo)
        {
            case PNG_IHDR: /* Cabecera */
                if(tengo_ihdr)
                {
                    /* Ya la hemos leido! */
                    ret=FALSE;
                }
                else
                {
                    ret=read_IHDR_png(&ctx,tam);
                    if(ret)
                        tengo_ihdr = TRUE;
                }
                break;
            case PNG_PLTE: /* Paleta */
                if(!tengo_ihdr)
                {
                    /* No tenemos header! */
                    ret=FALSE;
                }
                else
                {
                    ret=read_PLTE_png(&ctx,tam);
                }
                break;
            case PNG_tRNS: /* Transparencia */
                if(!tengo_ihdr)
                {
                    /* No tenemos header! */
                    ret=FALSE;
                }
                else
                {
                    ret=read_tRNS_png(&ctx,tam);
                }
                break;            
            case PNG_IDAT: /* Datos */
                if(!tengo_ihdr)
                {
                    ret=FALSE;
                }
                else if((ctx.entrelazado && ctx.e_pase < 7) || ctx.linea < ctx.img->height)
                {
                    ret=read_IDAT_png(&ctx,tam);
                    if(ret)
                        ctx.tengo_idat=TRUE;
                }
                break;
            case PNG_IEND: /* ultimo chunk */
                ctx.tam=0;
                break;            
            default: /* Ignorar chunks que no usamos */
                ret=ignore_chunk_png(&ctx,tam);
                break;
        }
    }
    if(tengo_ihdr)
        inflateEnd(&ctx.z);
    if(ctx.lineas)
        free(ctx.lineas);
    if(ret && !ctx.tengo_idat)
        ret=FALSE;
    else if(!ret && ctx.tengo_idat)
        ret=TRUE;
    return ret;
}

/* -------------------------- *
    BMP Loading Routines
 * -------------------------- */

/* Referencia: https://en.wikipedia.org/wiki/BMP_file_format */

/* Working Context */

typedef struct
{
    unsigned int     offset;        /* Offset to the bits */
    unsigned int     width;
    unsigned int     height;
    unsigned short   planos;
    unsigned short   bpp;           /* bits per pixel */
    unsigned int     compresion;    /* compresion (0=BI_RGB, 1=BI_RLE8, 2=BI_RLE4) */
    unsigned int     tam;           /* Tamaño en bytes del bitmap */
    rgb_t            pal[256];      /* Paleta */
} bmp_ctx_t;

/* -------------------------- */

static int read_header_bmp(unsigned char *cab,int tam,bmp_ctx_t *bm)
{
    if(tam < 54 || cab[0]!='B' || cab[1]!='M')
        return FALSE;
    bm->offset=get_le32(cab + 10);
    bm->width=get_le32(cab + 18);
    bm->height=get_le32(cab + 22);
    bm->planos=get_le16(cab + 26);
    bm->bpp=get_le16(cab + 28);
    bm->compresion=get_le32(cab + 30);

    if(bm->bpp!=8 && bm->bpp!=24)
        return FALSE;

    if(bm->planos!=1 || bm->compresion)
        return FALSE;

    if(bm->offset >= (unsigned int)tam)
        return FALSE;

    /* Leer La paleta si es necesario */

    if(bm->bpp==8)
    {
        int t,i;

        for(i=54,t=0;t<256;t++)
        {
            bm->pal[t].blue=cab[i++];
            bm->pal[t].green=cab[i++];
            bm->pal[t].red=cab[i++];
            i++; /* ignora byte reservado */
        }
    }
    return TRUE;
}

/* -------------------------- */

static int read_bmp(image_t *img,const void *buf,size_t tam)
{
    int t;
    bmp_ctx_t cabi;
    unsigned char *bits,*ptr;
    color_fun_t procesa=NULL;
    size_t width;

    ptr=(unsigned char *)buf;
    if(!read_header_bmp(ptr,tam,&cabi))    
        return FALSE;

    if(!image_modify(img,(int)cabi.width,(int)cabi.height,(cabi.bpp==8) ? FMT_RGB_PAL : FMT_RGB24))    
        return FALSE;
    width=img->width;
    if(cabi.bpp==8)
    {
        memcpy(img->pal,cabi.pal,sizeof(rgb_t)*256);
        procesa=(color_fun_t)memcpy;
    }
    else
    {
        procesa=(color_fun_t)invert_rgb;
    }
    ptr+=cabi.offset;
    tam=(((cabi.width * cabi.bpp + 31) / 32) * 4);
    /*
        Los bitmaps de windows vienen cabeza abajo,
        asi que hay que leer del final hacia atras.
    */
    bits=(unsigned char *)img->pixels;
    bits+=(img->bpl * (img->height-1));
    for(t=0;t<img->height;t++)
    {
        procesa(bits,ptr,width);
        ptr+=tam;
        bits-=img->bpl;
    }
    return TRUE;
}

/* -------------------------------------------------- *
    JPEG support

    Read the width (ww), height (hh), the number of 
    channels (ch) and the bits per channel (bc) from
    s JPEG file
 * -------------------------------------------------- */

static int info_from_jpeg(int *ww,int *hh,int *ch,int *bc,const void *buffer,size_t tam)
{
	unsigned char *buf;
    size_t pos = 2;
    unsigned short len;

    buf=(unsigned char *)buffer;

    if(tam > 127 && buf[0]==0xff && buf[1]==0xd8 && buf[tam-2]==0xff && buf[tam-1]==0xd9)
    {
        tam-=2;

        while(pos < tam)
        {
            if(buf[pos++] == 0xff)
            {                       
                len = 0;
                switch(buf[pos++])
                {
                    case 0xd8:/* This should not happen as this is the start of a JPEG, but...*/
                    case 0xd9:
                    case 0xda:
                        /* 
                            We are reaching either the start or 
                            the end of image. Stop searching 
                        */
                        return FALSE;
                    case 0xc0:
                    case 0xc1:
                    case 0xc2:
                    case 0xc3:
                    case 0xc5:
                    case 0xc6:
                    case 0xc7:
                    case 0xc9:
                    case 0xca:
                    case 0xcb:
                    case 0xcd:
                    case 0xce:
                    case 0xcf:
                        /* This is a SOF, so jump the length */                                  
                        pos += 2;
                       *bc = (int) buf[pos++];
                       *hh = (int) get_be16(buf + pos);	
                       *ww = (int) get_be16(buf + pos + 2);	
                       *ch = (int) buf[pos + 4];
                        return TRUE;
                    default:
                        len = get_be16(buf + pos);                        
                        break;                    
                }
                if(len > 2) 
                    pos += (len - 2);                
            }            
        }
    }
    return FALSE;    
}

/* ------------------------------------------------------ *
   Separate the RGB Channels from the Alpha channel and
   return pointers to them. Note that the image would be
   converted to RGB if in another format (ej Indexed) and
   that if the alpha channel is missing or its either full
   of 0 or full of 0xff, the mask will be returned as NULL
   (empty) as its application will not have any effect on
   the RGB pixels.
 * ------------------------------------------------------ */

static int get_rgb_and_mask(image_t *img,void **data,size_t *dlen,void **mask,size_t *mlen)
{
    unsigned char *prgb,*pmsk,*tmp,*org,val = 0;
    size_t lrgb,crgb,cmsk,lmsk;
    int i,t,c,same = TRUE;

    if(img && img->pixels && img->height > 0 && img->width > 0 && data && dlen && mask && mlen)
    {
        lrgb = img->width * 3;
        lmsk = img->width;

        prgb = (unsigned char *) calloc(lrgb,img->height);
        if(!prgb)
            return FALSE;

        pmsk = (unsigned char *) calloc(lmsk,img->height);
        if(!pmsk)
        {
            free(prgb);
            return FALSE;
        }
        cmsk = crgb = 0;
       *data = prgb;
       *mask = pmsk;
        tmp = (unsigned char *)img->pixels;
        for(i =0; i < img->height; i++)
        {
            switch(img->fc)
            {
                case FMT_GRAY8:
                    gray8_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_RGB_PAL:
                    pal8_to_rgb(prgb,tmp,img->width,img->pal);
                    break;                
                case FMT_RGB15:
                    rgb15_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_BGR15:
                    bgr15_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_RGB16:
                    rgb16_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_BGR16:
                    bgr16_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_RGB24:
                    memcpy(prgb,tmp,img->width);
                    break;
                case FMT_BGR24:
                    bgr_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_RGB32:
                    rgb32_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_BGR32:
                    bgr32_to_rgb(prgb,tmp,img->width);
                    break;
                case FMT_RGBA:
                    org = tmp;
                    if(i == 0)
                        val = org[3];
                    for(c = t=0;t<img->width;t++)
                    {
                        prgb[c++]=org[0];
                        prgb[c++]=org[1];
                        prgb[c++]=org[2];
                        if(val !=  org[3])
                            same = FALSE;
                        pmsk[t] = org[3];
                        org+=4;
                    }
                    pmsk += lmsk;
                    cmsk += lmsk;
                    break;
                case FMT_ARGB:
                    org = tmp;
                    if(i == 0)
                        val = org[0];
                    for(c=t=0;t<img->width;t++)
                    {
                        prgb[c++]=org[1];
                        prgb[c++]=org[2];
                        prgb[c++]=org[3];
                        if(val !=  org[0])
                            same = FALSE;
                        pmsk[t++] = org[0];
                        org+=4;
                    }
                    pmsk += lmsk;
                    cmsk += lmsk;
                    break;
                case FMT_BGRA:
                    org = tmp;
                    if(i == 0)
                        val = org[3];
                    for(c=t=0;t<img->width;t++)
                    {
                        prgb[c++]=org[2];
                        prgb[c++]=org[1];
                        prgb[c++]=org[0];
                        if(val !=  org[3])
                            same = FALSE;
                        pmsk[t]= org[3];
                        org+=4;
                    }
                    pmsk += lmsk;
                    cmsk += lmsk;
                    break;
                default:
                    free(*data);
                    free(*mask);
                   *dlen = *mlen = 0;
                   *data = *mask = NULL;
                    return FALSE;                    
            }
            tmp  += img->bpl;
            prgb += lrgb;
            crgb += lrgb;
        }
       *dlen = crgb;
       *mlen = cmsk;
        if(same == TRUE && (val == 0 || val == 0xff))
        {
            free(*mask);
           *mask = NULL;
           *mlen = 0;
        }
        return TRUE;
    }
    return FALSE;
}

/* -------------------------------------- */

static int image_from_data(image_t *img,const void *buf,size_t tam)
{
    if(img && buf && tam > 127)
    {
        if(read_bmp(img,buf,tam))
            return TRUE;
        return read_png(img,buf,tam);
    }
    return FALSE;
}

/* -------------------------------------------------- */

int pdf_load_an_image(pdf_image_t *img,wchar_t *file)
{
    int ch,found = FALSE;
    image_t imagen = {0};
    void  *buf;
    size_t len;        
    int ret = PDF_EINVAL;

    if(img && file)
    {
        ret = PDF_ENOMEM;
        buf = read_whole_filew(file,&len);
        if(buf)
        {        
            img->used  = FALSE;
            img->data  = img->mask =  NULL;
            img->dlen  = img->mlen  = 0;
            img->zdata = img->zmask = FALSE;            
            img->n = 0;

            ret = PDF_EINCORRECT;

            if(info_from_jpeg(&(img->w),&(img->h),&ch,&(img->bpc),buf,len))            
            {
                img->type = PDF_IMG_JPG;
                if(ch == 3)
                {
                    img->cs = "DeviceRGB";
                    found = TRUE;
                }
                else if(ch == 4)
                {
                    img->cs = "DeviceCMYK";
                    found = TRUE;
                }
                else if(ch == 1)
                {
                    img->cs = "DeviceGray";
                    found = TRUE;
                }

                if(found)
                {
                    img->data = buf;
                    img->dlen = len;                    
                    return PDF_OK;
                }
            }

            if(image_from_data(&imagen,buf,len))
            {                
                if(get_rgb_and_mask(&imagen,&(img->data),&(img->dlen),&(img->mask),&(img->mlen)))
                {
                    void *zdata,*zmask;
                    size_t zdlen,zmlen;

                    img->w = imagen.width;
                    img->h = imagen.height;
                    img->type = PDF_IMG_RGB;
                    img->bpc = 8;
                    img->cs = "DeviceRGB";

                    zdata = zcompress(img->data,img->dlen,&zdlen,0);
                    if(zdata)
                    {
                        free(img->data);
                        img->data = zdata;
                        img->dlen = zdlen;
                        img->zdata = TRUE;
                    }
                    zmask = zcompress(img->mask,img->mlen,&zmlen,0);
                    if(zmask)
                    {
                        free(img->mask);
                        img->mask = zmask;
                        img->mlen = zmlen;
                        img->zmask = TRUE;
                    }                              
                    image_destroy(&imagen);                            
                    free(buf);
                    return PDF_OK;
                }
                image_destroy(&imagen);        
            }
            free(buf);
        }
    }
    return ret;
}

