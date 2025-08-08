/*
    pdf.c

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

#include "pdf.h"

/* -------------------------------------------------- *
    Internal variables, global to this file
 * -------------------------------------------------- */

static const float  pdf_units[PDF_NUM_UNITS]       = { 72.00 / 25.4, 72.00 / 2.54, 72.00, 1.00  };
static const float  pdf_size_h[PDF_NUM_PAGE_SIZES] = { 595.28,  841.89, 420.94, 612.00,  612.00 };
static const float  pdf_size_v[PDF_NUM_PAGE_SIZES] = { 841.89, 1190.55, 595.28, 792.00, 1008.00 };

static const char  *meta_fields[PDF_MAX_MD_FIELDS] = {"Title","Author","Subject","Keywords","Producer","Creator","CreationDate"};

/* -------------------------------------------------- *
    Internal functions
 * -------------------------------------------------- */

#define pdf_free(_b)  do { if(_b) free(_b); } while(0)

/* -------------------------------------------------- */

static char *pdf_wchar_to_cp1252(size_t *rlen,const wchar_t *fmt,va_list vl,int norr)
{
    char *ret = NULL;
    size_t t,i,len = 0;
    wchar_t *orig;

    orig = wcsallocfv(&len,fmt,vl);
    if(orig)
    {
        ret = (char *)orig;
        for(t=i=0; t < len; t++)
        {
            if(norr && orig[t] == L'\r')
                continue;
            ret[i++] = wchar_to_win1252(orig[t]);
        }
        ret[i] = 0;
        len = i;
    }
    if(rlen)
        *rlen = len;
    return ret;
}

/* -------------------------------------------------- */

static void pdf_replace_nb_alias(pdf_t *pdf,int n)
{
    size_t *places,alen,plen,dlen,pos;
    int  t,count,max,found;
    unsigned char *data;
    char pnum[28];

    data = pdf->page_lst[n].content;
    if(data == NULL)
    {
        pdf->error = PDF_ENOMEM;
        return;
    }

    dlen = pdf->page_lst[n].size;
    alen = strlen(pdf->page_nb_alias);
    plen = snprintf(pnum,28,"%d",pdf->page_cnt);

    /* Find all the occurences of the alias, and their location  */

    places = NULL;
    count = 0;
    max   = 0;
    pos   = 0;

    while(pos < dlen)
    {
        found = FALSE;

        if(data[pos] == pdf->page_nb_alias[0])
        {
            if((dlen - pos) < alen)
                break;

            if(!memcmp( data + pos, pdf->page_nb_alias,alen))
            {
                if(count == max)
                {
                    size_t *nlst = (size_t *)realloc(places,(max + 10) * sizeof(size_t));

                    if(nlst == NULL)
                    {
                        if(places)
                            pdf_free(places);
                        pdf->error = PDF_ENOMEM;
                        return;
                    }
                    places = nlst;
                    max += 10;
                }
                places[count++] = pos;
                found = TRUE;
            }
        }
        pos += (found) ? alen : 1;
    }

    /* Now do the replacement if any */

    if(count)
    {
        size_t nlen,dif,prev;
        unsigned char *ndat;

        /* Find out new size */

        if(alen < plen)
        {
            dif  = (plen - alen);
            nlen = dlen + (dif * count);
        }
        else
        {
            dif  = (alen - plen);
            nlen = dlen - (dif * count);
        }

        ndat = (unsigned char *)malloc(nlen);
        if(ndat == NULL)
        {
            free(places);
            pdf->error = PDF_ENOMEM;
            return;
        }

        pdf->page_lst[n].content = ndat;
        pdf->page_lst[n].size    = nlen;
        pdf->page_lst[n].max     = nlen;

        pos  = 0;
        prev = 0;
        nlen = 0;

        for(t=0;t<count;t++)
        {
            pos = places[t];
            dif = pos - prev;

            memcpy(ndat + nlen,data + prev, dif);
            nlen += dif;

            memcpy(ndat + nlen,pnum,plen);
            nlen += plen;
            prev = pos + alen;
        }

        if(dlen > prev)
        {
            dif = dlen - prev;
            memcpy(ndat + nlen,data + prev, dif);
            nlen += dif;
        }

        free(places);
        free(data);
    }
}

/* -------------------------------------------------- */

static void pdf_add_data_to_page(pdf_t *pdf,const void *data,size_t dsize)
{
    unsigned char *ndat;
    size_t left,size;

    ndat = pdf->page_lst[pdf->page_num].content;
    size = pdf->page_lst[pdf->page_num].size;
    left = pdf->page_lst[pdf->page_num].max - size;

    if(left < dsize)
    {
        left = pdf->page_lst[pdf->page_num].max + dsize + 128;
        if(left < size)  /* Overflow? This is a huge page!!! */
        {
            pdf->error = PDF_E2BIG;
            return;
        }
        ndat = (unsigned char *)realloc(pdf->page_lst[pdf->page_num].content,left);
        if(!ndat)
        {
            pdf->error = PDF_ENOMEM;
            return;
        }
        pdf->page_lst[pdf->page_num].content = ndat;
        pdf->page_lst[pdf->page_num].max = left;
    }
    pdf->page_lst[pdf->page_num].size += dsize;
    memcpy(ndat + size, data, dsize);
}


/* -------------------------------------------------- */

static void pdf_out_string(pdf_t *pdf,const char *str,size_t len)
{
    if(pdf->state == PDF_STATE_PAGE_READY)
    {
        size_t count = 0,pos = 0;

        while(!pdf->error && pos < len)
        {
            switch(str[pos])
            {
                case '\\':
                    pdf_add_data_to_page(pdf,pdf->tmp,count);
                    pdf_add_data_to_page(pdf,"\\\\",2);
                    count = 0;
                    break;
                case '(':
                    pdf_add_data_to_page(pdf,pdf->tmp,count);
                    pdf_add_data_to_page(pdf,"\\(",2);
                    count = 0;
                    break;
                case ')':
                    pdf_add_data_to_page(pdf,pdf->tmp,count);
                    pdf_add_data_to_page(pdf,"\\)",2);
                    count = 0;
                    break;
                case '\r':
                    pdf_add_data_to_page(pdf,pdf->tmp,count);
                    pdf_add_data_to_page(pdf,"\\r",2);
                    count = 0;
                    break;
                default:
                    if(count == PDF_MAX_TEMP)
                    {
                        pdf_add_data_to_page(pdf,pdf->tmp,count);
                        count = 0;
                    }
                    pdf->tmp[count++] = str[pos];
                    break;
            }
            pos++;
        }
        if(count > 0)
            pdf_add_data_to_page(pdf,pdf->tmp,count);
    }
}

/* -------------------------------------------------- */

static void pdf_out(pdf_t *pdf,const char *fmt,...)
{
    if(!pdf->error && pdf->state == PDF_STATE_PAGE_READY)
    {
        va_list vl;
        size_t len;

        va_start(vl,fmt);
        len = vsnprintf(pdf->tmp,PDF_MAX_TEMP,fmt,vl);
        va_end(vl);

        if(len >= PDF_MAX_TEMP)
            len = PDF_MAX_TEMP - 1;

        pdf_add_data_to_page(pdf,pdf->tmp,len);
    }
}

/* -------------------------------------------------- */

static void pdf_out_color(pdf_t *pdf,rgb_t *c,int draw)
{
    if(c->red == c->green && c->green == c->blue)
    {
        if(c->green == 0)
            pdf_out(pdf,"0 %s\n",(draw) ? "G" : "g");
        else
            pdf_out(pdf,"%.3f %s\n",c->green / 255.0,(draw) ? "G" : "g");
    }
    else
    {
        pdf_out(pdf,"%.3f %.3f %.3f %s\n",c->red / 255.0, c->green / 255.0, c->blue / 255.0,(draw) ? "RG" : "rg");
    }
}

/* -------------------------------------------------- */

static void pdf_put_wcs(pdf_t *pdf,wchar_t *str)
{
    char ch;


    while(*str)
    {
        ch = wchar_to_win1252(*str++);
        switch(ch)
        {
            case '\\':
                fprintf(pdf->fp,"\\\\");
                break;
            case '(':
                fprintf(pdf->fp,"\\(");
                break;
            case ')':
                fprintf(pdf->fp,"\\)");
                break;
            case '\r':
                fprintf(pdf->fp,"\\r");
                break;
            case '\n':
                fprintf(pdf->fp,"\\n");
                break;
            case '\t':
                fprintf(pdf->fp,"\\t");
                break;
            default:
                if(ch < 0)
                    fprintf(pdf->fp,"\\%o",(unsigned char)ch);
                else
                    fwrite(&ch,1,1,pdf->fp);
                break;
        }
        if(ferror(pdf->fp))
        {
            pdf->error = PDF_EWRITE;
            break;
        }
    }
}

/* -------------------------------------------------- */

static void pdf_put_string(pdf_t *pdf,const char *str)
{
    char ch;


    while(*str)
    {
        ch = *str++;
        switch(ch)
        {
            case '\\':
                fprintf(pdf->fp,"\\\\");
                break;
            case '(':
                fprintf(pdf->fp,"\\(");
                break;
            case ')':
                fprintf(pdf->fp,"\\)");
                break;
            case '\r':
                fprintf(pdf->fp,"\\r");
                break;
            case '\n':
                fprintf(pdf->fp,"\\n");
                break;
            case '\t':
                fprintf(pdf->fp,"\\t");
                break;
            default:
                if(ch < 0)
                    fprintf(pdf->fp,"\\%o",(unsigned char)ch);
                else
                    fwrite(&ch,1,1,pdf->fp);
                break;
        }
        if(ferror(pdf->fp))
        {
            pdf->error = PDF_EWRITE;
            break;
        }
    }
}


/* -------------------------------------------------- */

static void pdf_put(pdf_t *pdf,const char *fmt,...)
{
    if(pdf->fp && !pdf->error)
    {
        va_list vl;

        if(ferror(pdf->fp))
        {
            pdf->error = PDF_EWRITE;
        }
        else
        {
            va_start(vl,fmt);
            vfprintf(pdf->fp,fmt,vl);
            va_end(vl);
        }
    }
}

/* -------------------------------------------------- */

static void pdf_new_obj(pdf_t *pdf,int n)
{
    if(pdf->error == PDF_OK)
    {
        if(n < 1)
        {
            if(pdf->obj_max == pdf->obj_num)
            {
                size_t nsize;
                long *nobjl;

                nsize = sizeof(long) * (pdf->obj_max + 10);
                nobjl = (long *)realloc(pdf->offsets,nsize);
                if(nobjl == NULL)
                {
                    pdf->error = PDF_ENOMEM;
                    return;
                }
                pdf->offsets = nobjl;
                pdf->obj_max += 10;
            }
            pdf->obj_num++;
            n = pdf->obj_num;
        }
        pdf->offsets[n - 1] = ftell(pdf->fp);
        pdf_put(pdf,"%d 0 obj\n",n);
    }
}

/* -------------------------------------------------- */

static void pdf_put_stream_obj(pdf_t *pdf,const void *data,size_t len)
{
    pdf_new_obj(pdf,0);

    if(!pdf->error)
    {
        if(pdf->deflate_it && len > 0)
        {
            size_t out;
            void *ptr;

            ptr = zcompress(data,len,&out,0);
            if(ptr == NULL)
            {
                pdf->error = PDF_ENOMEM;
                return;
            }
            pdf_put(pdf,"<</Filter /FlateDecode /Length %lu>>\nstream\n",(unsigned long)out);
            if(len > 0)
            {
                if(fwrite(ptr,out,1,pdf->fp) != 1)
                    pdf->error = PDF_EWRITE;
            }
            fprintf(pdf->fp,"\nendstream\n");
            pdf_free(ptr);
        }
        else
        {
            pdf_put(pdf,"<</Length %lu>>\nstream\n",(unsigned long)len);
            if(len > 0)
            {
                if(fwrite(data,len,1,pdf->fp) != 1)
                    pdf->error = PDF_EWRITE;
            }
            fprintf(pdf->fp,"\nendstream\n");
        }
        pdf_put(pdf,"endobj\n");
	}
}


/* -------------------------------------------------- */

static void pdf_put_stream(pdf_t *pdf,const void *data,size_t len)
{
    if(!pdf->error)
    {
        pdf_put(pdf,"stream\n");
        if(len > 0)
        {
            if(fwrite(data,len,1,pdf->fp) != 1)
                pdf->error = PDF_EWRITE;
        }
        fprintf(pdf->fp,"\nendstream\n");
	}
}

/* -------------------------------------------------- */

static void pdf_put_page(pdf_t *pdf,int n)
{
    pdf_new_obj(pdf,0);
    pdf_put(pdf,"<</Type /Page\n/Parent 1 0 R\n");

    /*
    pdf_put(pdf,"/MediaBox [0 0 %.2f %.2f]\n",pdf->width,pdf->page_lst[n].height);

    if(pdf->page_lst[n].rotation > 0)
       pdf_put(pdf,"/Rotate %d\n",pdf->page_lst[n].rotation);
    */

    pdf_put(pdf,"/Resources  2 0 R\n");

    if(pdf->page_lst[n].lnk_cnt > 0)
    {
        int i;
        pdf_tlnk_t  *tlnk;
        pdf_olnk_t  *olnk;

		pdf_put(pdf,"/Annots [\n");

		for(i = 0; i < pdf->page_lst[n].lnk_cnt; i++)
		{
		    olnk = &(pdf->page_lst[n].lnk_lst[i]);
            tlnk = &(pdf->lnk_lst[olnk->id]);

			pdf_put(pdf,"<</Type /Annot /Subtype /Link /Rect [");
			pdf_put(pdf,"%.2f %.2f %.2f %.2f",olnk->x,olnk->y,olnk->x + olnk->w,olnk->y - olnk->h);
			if(pdf->lnk_border)
    			pdf_put(pdf,"] /Border [0 0 %.3f] ",0.567 / pdf->scale);
    		else
    		    pdf_put(pdf,"] /Border [0 0 0] ");
			if(tlnk->local == FALSE)
			{
				pdf_put(pdf,"/A <</S /URI /URI (");
				pdf_put_string(pdf,tlnk->uri);
				pdf_put(pdf,")>>>>\n");
            }
            else
			{
				pdf_put(pdf,"/Dest [%d 0 R /XYZ 0 ",pdf->page_lst[tlnk->page].n);
				pdf_put(pdf,"%.2f null]>>\n",tlnk->y);
			}
		}
		pdf_put(pdf,"]\n");
    }
    if(pdf->alpha == TRUE)
        pdf_put(pdf,"/Group <</Type /Group /S /Transparency /CS /DeviceRGB>>\n");
    pdf_put(pdf,"/Contents %d 0 R>>\nendobj\n",pdf->obj_num + 1);
    if(pdf->page_nb_alias && !pdf->page_lst[n].replaced)
    {
        pdf_replace_nb_alias(pdf,n);
        pdf->page_lst[n].replaced = TRUE;
    }
    pdf_put_stream_obj(pdf,pdf->page_lst[n].content,pdf->page_lst[n].size);
}


/* -------------------------------------------------- */

static void pdf_put_pages(pdf_t *pdf)
{
    int n;

	for(n=0;n < pdf->page_cnt; n++)
		pdf->page_lst[n].n = pdf->obj_num + 1 + 2 * n;

    for(n=0;n < pdf->page_cnt; n++)
		pdf_put_page(pdf,n);

	// Pages root

	pdf_new_obj(pdf,1);
	pdf_put(pdf,"<</Type /Pages\n/Kids [ ");

	for(n=0;n < pdf->page_cnt; n++)
		pdf_put(pdf,"%d 0 R ",pdf->page_lst[n].n);

	pdf_put(pdf,"]\n/Count %d\n",pdf->page_cnt);
	pdf_put(pdf,"/MediaBox [0 0 %.2f %.2f]\n>>\nendobj\n",pdf->width_pt,pdf->height_pt);
}

/* -------------------------------------------------- */

static char *pdf_get_unicode_cmap(pdf_font_t *fnt,size_t *len)
{
    const char *prefix = "/CIDInit /ProcSet findresource begin\n12 dict begin\nbegincmap\n/CIDSystemInfo\n<</Registry (Adobe)\n"
                         "/Ordering (UCS)\n/Supplement 0\n>> def\n/CMapName /Adobe-Identity-UCS def\n/CMapType 2 def\n1 "
                         "begincodespacerange\n<00> <FF>\nendcodespacerange\n",
               *suffix = "endcmap\nCMapName currentdict /CMap defineresource pop\nend\nend";
    char *ranges = NULL,*chars = NULL,*tmp,buf[81],*ret;
    int i, rlen = 0, clen = 0;

    for(i=0;i < fnt->uv->cnt; i++)
    {
        tmp = buf; /* Snitch (Chivato) */
		if(fnt->uv->lst[i].type == PDF_UV_RANGE)
		{
		    snprintf(buf,81,"<%02X> <%02X> <%04X>\n",fnt->uv->lst[i].ch,fnt->uv->lst[i].che,fnt->uv->lst[i].unicode);
            tmp = realloc_strcat(ranges,buf,0,0,len);
            if(tmp != NULL)
            {
                ranges = tmp;
                rlen++;
            }
		}
		else if(fnt->uv->lst[i].type == PDF_UV_CHAR)
		{
			snprintf(buf,81,"<%02X> <%04X>\n",fnt->uv->lst[i].ch,fnt->uv->lst[i].unicode);
            tmp = realloc_strcat(chars,buf,0,0,len);
            if(tmp != NULL)
            {
                chars = tmp;
                clen++;
            }
		}

		if(tmp == NULL)
		{
            pdf_free(ranges);
	        pdf_free(chars);
		    return NULL;
		}
	}

    ret = strdup(prefix);
    if(rlen > 0)
    {
        if(ret)
        {
            snprintf(buf,81,"%d beginbfrange\n",rlen);
            tmp = realloc_strcat(ret,buf,0,0,len);
            if(tmp == NULL)
            {
                pdf_free(ret);
                ret = NULL;
            }
            else
            {
                ret = realloc_strcat(tmp,ranges,0,0,len);
                if(ret)
                {
                    tmp = realloc_strcat(ret,"endbfrange\n",0,0,len);
                    if(tmp)
                    {
                        ret = tmp;
                        tmp = NULL;
                    }
                }
                pdf_free(tmp);
            }
        }
        pdf_free(ranges);
    }

    if(clen > 0)
    {
        if(ret)
        {
            snprintf(buf,81,"%d beginbfchar\n",clen);
            tmp = realloc_strcat(ret,buf,0,0,len);
            if(tmp == NULL)
            {
                pdf_free(ret);
                ret = NULL;
            }
            else
            {
                ret = realloc_strcat(tmp,chars,0,0,len);
                if(ret)
                {
                    tmp = realloc_strcat(ret,"endbfchar\n",0,0,len);
                    if(tmp)
                    {
                        ret = tmp;
                        tmp = NULL;
                    }
                }
                pdf_free(tmp);
            }
        }
        pdf_free(chars);
    }
    if(ret)
    {
        tmp = realloc_strcat(ret,suffix,0,0,len);
        if(tmp == NULL)
            pdf_free(ret);
        ret = tmp;
	}
    return ret;
}

/* -------------------------------------------------- */

static void pdf_put_fonts(pdf_t *pdf)
{
    int i,cmapn,coremapn = 0;
    char *cmap = NULL;
    size_t len = 0;
    pdf_font_t *fnt;

    for(i=0;i < pdf->fnt_cnt; i++)
	{
    	fnt = pdf->fnt_lst[i];
	    cmapn = 0;
		if(fnt->uv)
		{
		    int id = fnt->uv->id;

		    if(id!=0x1AAC || coremapn == 0)
            {
			    cmap = pdf_get_unicode_cmap(pdf->fnt_lst[i],&len);
                if(cmap == NULL)
                {
                    pdf->error = PDF_ENOMEM;
                    return;
                }
                pdf_put_stream_obj(pdf,cmap,len);
			    pdf_free(cmap);
			    cmapn = pdf->obj_num;
			    if(id == 0x1AAC)
			        coremapn = cmapn;
			}
			else
			{
			    cmapn = coremapn;
			}

		}
		// Font object
		if(fnt->type == PDF_FNT_CORE)
		{
    		fnt->n = pdf->obj_num + 1;
        	pdf_new_obj(pdf,0);
			pdf_put(pdf,"<</Type /Font\n/BaseFont /%s\n/Subtype /Type1\n",fnt->name);
			if(strcmp("Symbol",fnt->name) && strcmp("ZapfDingbats",fnt->name))
				pdf_put(pdf,"/Encoding /WinAnsiEncoding\n");
			if(cmapn > 0)
				pdf_put(pdf,"/ToUnicode %d 0 R\n",cmapn);
			pdf_put(pdf,">>\nendobj\n");
		}
		else if(fnt->type == PDF_FNT_TTF && fnt->used == TRUE)
		{
		    int nf,fd,ws,t,flags = 0x20;

            nf = pdf->obj_num + 1;
		    pdf_new_obj(pdf,0);
			pdf_put(pdf,"<</Length %lu /Length1 %lu /Filter /FlateDecode\n",(unsigned long)fnt->flen,(unsigned long)fnt->forg);
			pdf_put(pdf,">>\n");
			pdf_put_stream(pdf,fnt->file,fnt->flen);
			pdf_put(pdf,"endobj\n");

            ws = pdf->obj_num + 1;
            pdf_new_obj(pdf,0);
            pdf_put(pdf,"[ ");
            for(t=32;t<256;t++)
                pdf_put(pdf,"%d ",fnt->cw[t]);
            pdf_put(pdf,"]\nendobj\n");

            if(fnt->is_fix)
                flags|=0x01;
            if(fnt->it_ang)
                flags|=0x40;

            fd = pdf->obj_num + 1;
            pdf_new_obj(pdf,0);
            pdf_put(pdf,"<</Type /FontDescriptor /FontName /%s\n",fnt->name);
            pdf_put(pdf,"/Ascent %d /Descent %d /CapHeight %d /Flags %d\n",fnt->asc,fnt->des,(fnt->cap) ? fnt->cap : fnt->asc,flags);
            pdf_put(pdf,"/FontBBox [%d %d %d %d] ",fnt->xo,fnt->yo,fnt->xd,fnt->yd);
            pdf_put(pdf,"/StemV %d /MissingWidth %d ",(fnt->style & PDF_FNT_BOLD) ? 120 : 70 ,fnt->mis_width);
            if(fnt->it_ang)
                pdf_put(pdf,"/ItalicAngle %d ",fnt->it_ang);
            pdf_put(pdf,"/FontFile2 %d 0 R>>\nendobj\n",nf);

            fnt->n = pdf->obj_num + 1;
            pdf_new_obj(pdf,0);
			pdf_put(pdf,"<</Type /Font\n/BaseFont /%s\n/Subtype /TrueType\n",fnt->name);
			pdf_put(pdf,"/FirstChar 32 /LastChar 255 /Encoding /WinAnsiEncoding\n");
			if(cmapn > 0)
				pdf_put(pdf,"/ToUnicode %d 0 R\n",cmapn);
			pdf_put(pdf,"/Widths %d 0 R\n/FontDescriptor %d 0 R\n",ws,fd);
			pdf_put(pdf,">>\nendobj\n");

		}
	}
}

/* -------------------------------------------------- */

static void pdf_put_images(pdf_t *pdf)
{
    int i;
    pdf_image_t *img;

    for(i=0;i < pdf->img_cnt; i++)
	{
	    img = &(pdf->img_lst[i]);
	    if(img->used == TRUE)
	    {
	     	img->n = pdf->obj_num + 1;
    	    pdf_new_obj(pdf,0);
			pdf_put(pdf,"<</Type /XObject\n/Subtype /Image\n/Width %d\n/Height %d\n",img->w,img->h);
  		    pdf_put(pdf,"/ColorSpace /%s\n",img->cs);
			if(!strcmp(img->cs,"DeviceCMYK"))
			    pdf_put(pdf,"/Decode [1 0 1 0 1 0 1 0]\n");
       		pdf_put(pdf,"/BitsPerComponent %d\n",img->bpc);
    		if(img->type==PDF_IMG_JPG)
    		{
        		pdf_put(pdf,"/Filter /DCTDecode\n");
    		}
    		else if(img->zdata)
        	    pdf_put(pdf,"/Filter /FlateDecode\n/DecodeParms <</Predictor 1>>\n");
    		if(img->mask)
    		{
    		    pdf_put(pdf,"/SMask %d 0 R\n",pdf->obj_num + 1);
    		}
            pdf_put(pdf,"/Length %lu>>\n",(unsigned long)img->dlen);
            pdf_put_stream(pdf,img->data,img->dlen);
            pdf_put(pdf,"endobj\n");

            if(img->mask)
            {
                pdf_new_obj(pdf,0);
			    pdf_put(pdf,"<</Type /XObject\n/Subtype /Image\n/Width %d\n/Height %d\n",img->w,img->h);
  		        pdf_put(pdf,"/ColorSpace /DeviceGray\n/BitsPerComponent 8\n");
    		    if(img->zmask)
        		    pdf_put(pdf,"/Filter /FlateDecode\n/DecodeParms <</Predictor 1>>\n");
    		    pdf_put(pdf,"/Length %lu>>\n",(unsigned long)img->mlen);
                pdf_put_stream(pdf,img->mask,img->mlen);
                pdf_put(pdf,"endobj\n");
            }
	    }
	}
}

/* -------------------------------------------------- */

static void pdf_put_catalog(pdf_t *pdf)
{
    int n = pdf->page_lst[0].n;

	pdf_put(pdf,"/Type /Catalog\n/Pages 1 0 R\n");

	switch(pdf->zoom)
	{
	    case PDF_ZOOM_FULLPAGE:
    		pdf_put(pdf,"/OpenAction [%d 0 R /Fit]\n",n);
    		break;
    	case PDF_ZOOM_WIDTH:
    		pdf_put(pdf,"/OpenAction [%d 0 R /FitH null]\n",n);
    		break;
    	case PDF_ZOOM_ACTUAL:
    		pdf_put(pdf,"/OpenAction [%d 0 R /XYZ null null 1]\n",n);
    		break;
    	default:
    	    if(pdf->zoom >= PDF_ZOOM_MIN && pdf->zoom <= PDF_ZOOM_MAX)
        		pdf_put(pdf,"/OpenAction [%d 0 R /XYZ null null %.2f]\n",n,pdf->zoom / 100.0);
    		break;
    }

	switch(pdf->layout)
	{
	    case PDF_PAGE_SINGLE:
    		pdf_put(pdf,"/PageLayout /SinglePage\n",n);
    		break;
        case PDF_PAGE_ROLL:
		    pdf_put(pdf,"/PageLayout /OneColumn\n",n);
		    break;
	    case PDF_PAGE_DUAL_LEFT:
    		pdf_put(pdf,"/PageLayout /TwoColumnLeft\n",n);
    		break;
	    case PDF_PAGE_DUAL_RIGHT:
    		pdf_put(pdf,"/PageLayout /TwoColumnRight\n",n);
    		break;
    	default:
    	    break;
    }
}

/* -------------------------------------------------- */

static void pdf_put_document(pdf_t *pdf)
{
    int i,prev_n;
    long offset = 0;

    prev_n = pdf->obj_num;

    pdf_put(pdf,"%%PDF-%.1f\n",pdf->version);
    pdf_put_pages(pdf);

    pdf_put_fonts(pdf);
	pdf_put_images(pdf);

	/* Resource dictionary */
    pdf_new_obj(pdf,2);
	pdf_put(pdf,"<<\n");
    pdf_put(pdf,"/ProcSet [/PDF /Text /ImageB /ImageC /ImageI]\n");
	pdf_put(pdf,"/Font <<\n");
    for(i=0;i < pdf->fnt_cnt; i++)
		pdf_put(pdf,"/F%d %d 0 R\n",pdf->fnt_lst[i]->id,pdf->fnt_lst[i]->n);

	pdf_put(pdf,">>\n/XObject <<\n");
    for(i=0;i < pdf->img_cnt; i++)
    {
        if(pdf->img_lst[i].used == TRUE)
    	    pdf_put(pdf,"/I%d %d 0 R\n",i+1,pdf->img_lst[i].n);
    }
	pdf_put(pdf,">>\n>>\nendobj\n");

	/* Info */

	pdf_new_obj(pdf,0);
	pdf_put(pdf,"<<\n");

    for(i = 0; i < PDF_MAX_MD_FIELDS; i++)
    {
        if(pdf->metadata[i] == NULL)
        {
            if(i == PDF_DOC_PRODUCER)
                pdf_put(pdf,"/Producer (GAO-PDF %.1f)\n",PDF_ENGINE_VERSION);
            else if(i == PDF_DOC_CREATION_DATE)
                pdf_put(pdf,"/CreationDate (D:%s)\n",pdf_datetime(time(NULL)));
        }
        else
        {
            pdf_put(pdf,"/%s (",meta_fields[i]);
            pdf_put_wcs(pdf,pdf->metadata[i]);
            pdf_put(pdf,")\n");
        }
    }

	pdf_put(pdf,">>\nendobj\n");

	/* Catalog */

	pdf_new_obj(pdf,0);
	pdf_put(pdf,"<<\n");
	pdf_put_catalog(pdf);
	pdf_put(pdf,">>\nendobj\n");

	/* Cross-ref */

    if(!pdf->error)
	    offset = ftell(pdf->fp);

    pdf_put(pdf,"xref\n0 %d\n0000000000 65535 f \n",pdf->obj_num + 1);

    for(i=0; i<= pdf->obj_num; i++)
        pdf_put(pdf,"%010ld 00000 n \n",pdf->offsets[i]);

	/* Trailer */

    pdf_put(pdf,"trailer\n<<\n/Size %d\n/Root %d 0 R\n/Info %d 0 R\n>>\n",pdf->obj_num + 1,pdf->obj_num,pdf->obj_num - 1);

	pdf_put(pdf,"startxref\n%lu\n%%EOF\n",(unsigned long)offset);

    pdf->obj_num = prev_n;

}


/* -------------------------------------------------- *
    Public functions
 * -------------------------------------------------- */

const char *pdf_datetime(time_t t)
{
    struct tm *td;
    static char buffer[128];

    td = gmtime(&t);
    snprintf(buffer,128,"%04d%02d%02d%02d%02d%02d",(td->tm_year % 256) + 1900,td->tm_mon+1,td->tm_mday,td->tm_hour,td->tm_min,td->tm_sec);
    return (const char *)buffer;
}

/* -------------------------------------- */

static int is_sep_before(int curr,wchar_t *symb)
{
     switch(curr)
    {
        case PDF_CURR_POUND:
           *symb = L'£';
            return TRUE;
            break;
        case PDF_CURR_DOLLAR:
            *symb = L'$';
            return TRUE;            
        case PDF_CURR_YEN:
            *symb = L'¥';
            return TRUE;            
        case PDF_CURR_EURO:
            *symb= L'€';
             break;
        default:   
            *symb= ' ';          
             break;
    }
    return FALSE;
}

/* -------------------------------------- */

wchar_t *pdf_format_num(wchar_t *dest,size_t max,wchar_t tsep,wchar_t dsep,int ndec,double num,int curr)
{
    wchar_t tmp[201]={0},*src,*dst,symb;
    float scale = 1;
    int d,len,grp,lft,before,zero = FALSE, minus = FALSE;
    size_t total = 0;

    if(dest)
    {
        before=is_sep_before(curr,&symb);
        if(ndec < 0 )
            ndec = 0;
        else if(ndec > 15)
            ndec = 15;

        for(d=0;d < ndec;d++)
            scale = (scale * 10);

        num *= scale;
        len = swprintf(tmp,200,L"%.0f",num);
        if(len < 0)
        {
            wcsncpy(dest,L"???",max);            
            return dest;
        }

        src = tmp;
        dst = dest;

        if(*src=='-')
        {
            src++;
            minus = TRUE;
            len--;
            total++;
        }

        if(len <= ndec)
        {
            zero = TRUE;
            len++;
        }

        grp = ((len - ndec) / 3);
        lft = ((len - ndec) % 3);

        total += len + grp;

        if(curr)
            total++;

        if(ndec)
            total++;

        if(total >= max)
        {
            wcsncpy(dest,L"???",max);
            return dest;
        }

        if(curr && before)
            *dst++ = symb;

        if(minus)
            *dst++ = L'-';

        if(zero)
        {
            *dst++ = L'0';
            if(ndec > 0 && num == 0)
            {
                *dst++ = dsep;
                while(--ndec > -1)
                    *dst++ = L'0';
            }
        }
        else
        {
            for(d=0;d < lft;d++)
                *dst++ = *src++;

            while(--grp > -1)
            {
                if(lft > 0)
                    *dst++ = tsep;
                *dst++ = *src++;
                *dst++ = *src++;
                *dst++ = *src++;
                 lft++;
            }
        }

        if(ndec > 0)
        {
           *dst++ = dsep;
            while(--ndec > -1)
                *dst++ = *src++;
        }

        if(curr && !before)
            *dst++ = symb;
       *dst = 0;
    }
    return dest;
}

/* -------------------------------------------------- */

#define PDF_LINE_RATIO  1.12

/* -------------------------------------------------- */

pdf_t *pdf_create(int orientation,int unit,int size)
{
    pdf_t *pdf;

    pdf = (pdf_t *)malloc(sizeof(pdf_t));
    if(pdf)
    {
        pdf->version = PDF_DOC_VERSION;
        pdf->state = PDF_STATE_EMPTY;

        /* Metadata */

        memset(pdf->metadata,0,sizeof(pdf->metadata));

        /* Object list */

        pdf->offsets = (long *)calloc(sizeof(long),10);
        if(pdf->offsets == NULL)
        {
            free(pdf);
            return NULL;
        };
        pdf->obj_max = 10;
        pdf->obj_num = 2;

        /* Local links */

        pdf->lnk_lst = NULL;
        pdf->lnk_max = 0;
        pdf->lnk_cnt = 0;

        pdf->lnk_border = FALSE;

         /* Images */

        pdf->img_lst = NULL;
        pdf->img_max = 0;
        pdf->img_cnt = 0;

        /* Page stuff */

        pdf->page_lst = NULL;
        pdf->page_max  = 0;
        pdf->page_cnt  = 0;
        pdf->page_num  = -1;
        pdf->page_nb_alias = NULL;

        /* Default unit to Points */

        pdf->unit = (unit >= PDF_UNIT_MM && unit < PDF_NUM_UNITS) ? unit : PDF_UNIT_MM;
        pdf->scale    = pdf_units[pdf->unit];

        /* Default to A4 */

        pdf->size   = (size >= PDF_SIZE_A4 && size < PDF_NUM_PAGE_SIZES) ? size : PDF_SIZE_A4;

        pdf->size_h = pdf_size_h[size] / pdf->scale;
        pdf->size_v = pdf_size_v[size] / pdf->scale;

        if(orientation == PDF_LANDSCAPE)
        {
            pdf->orientation = PDF_LANDSCAPE;
            pdf->width  = pdf->size_v;
            pdf->height = pdf->size_h;
        }
        else
        {
            pdf->orientation = PDF_PORTRAIT;
            pdf->width  = pdf->size_h;
            pdf->height = pdf->size_v;
        }

        pdf->width_pt  = pdf->width  * pdf->scale;
        pdf->height_pt = pdf->height * pdf->scale;

        pdf->ws = 0;
        pdf->lasth = 0;

        pdf->layout = PDF_PAGE_DEFAULT;
        pdf->zoom   = PDF_ZOOM_DEFAULT;

        pdf->rotation = 0;

        /* Set margins to 1 cm */

        pdf->tmargin = 28.346 / pdf->scale;
        pdf->lmargin = pdf->rmargin = pdf->tmargin;

        pdf->autopb = TRUE;
        pdf->bmargin = pdf->tmargin * 2;
        pdf->pageabt = pdf->height - pdf->bmargin;

        /* Cell margins to 1 mm and line width to 0.2 mm */

        pdf->cmargin    = pdf->tmargin / 10;
        pdf->line_width = 0.567 / pdf->scale;

        pdf->x  =  pdf->lmargin;
        pdf->y  =  pdf->tmargin;

        /* Font stuff */

        pdf->fnt_lst = (pdf_font_t **)malloc(sizeof(pdf_font_t *) * 5);
        if(pdf->fnt_lst == NULL)
        {
            free(pdf->offsets);
            free(pdf);
            return NULL;
        }
        pdf->fnt_max = 5;

        pdf->fnt_lst[0] = pdf_get_core_font("Helvetica",PDF_FNT_REGULAR);
        if(pdf->fnt_lst[0] == NULL)
        {
            free(pdf->fnt_lst);
            free(pdf->offsets);
            free(pdf);
            return NULL;
        }

        pdf->fnt_cnt =  1;
        pdf->fnt_cur =  pdf->fnt_lst[0];

        pdf->fnt_cur->id = pdf->fnt_cnt;

        pdf->fnt_pt     = 12;
        pdf->fnt_size   = 12 / pdf->scale;
        pdf->fnt_under  = FALSE;

        pdf->txt_line   = (pdf->fnt_size * PDF_LINE_RATIO);
        pdf->txt_line   = pdf->fnt_size;

        pdf->alpha = FALSE;

        pdf->draw_color.red   = 0;
        pdf->draw_color.green = 0;
        pdf->draw_color.blue  = 0;

        pdf->fill_color.red   = 0;
        pdf->fill_color.green = 0;
        pdf->fill_color.blue  = 0;

        pdf->text_color.red   = 0;
        pdf->text_color.green = 0;
        pdf->text_color.blue  = 0;

        pdf->color_flag = FALSE;

        pdf->inuser_func = FALSE;

        pdf->header_func = NULL;
        pdf->footer_func = NULL;
        pdf->accept_func = NULL;

        pdf->deflate_it = TRUE;

        pdf->fp = NULL;
        pdf->error = PDF_OK;
    }
    return pdf;
}


/* -------------------------------------------------- */

void pdf_destroy(pdf_t *pdf)
{
    if(pdf)
    {
        int t;

        for(t=0; t < PDF_MAX_MD_FIELDS; t++)
            pdf_free(pdf->metadata[t]);

        pdf_free(pdf->offsets);
        pdf_free(pdf->lnk_lst);

        if(pdf->page_lst)
        {
            for(t=0; t < pdf->page_cnt; t++)
            {
                pdf_free(pdf->page_lst[t].lnk_lst);
                pdf_free(pdf->page_lst[t].content);
            }

            free(pdf->page_lst);
        }

        if(pdf->fnt_lst)
        {
            for(t=0; t < pdf->fnt_cnt; t++)
                 pdf_free_font(pdf->fnt_lst[t]);
            free(pdf->fnt_lst);
        }

        if(pdf->img_lst)
        {
            for(t=0; t < pdf->img_cnt; t++)
            {
                pdf_free(pdf->img_lst[t].data);
                pdf_free(pdf->img_lst[t].mask);
            }
            free(pdf->img_lst);
        }

        if(pdf->fp && pdf->fp!=stdout)
        {
            fflush(pdf->fp);
            fclose(pdf->fp);
        }

        memset(pdf,0,sizeof(pdf_t));

        free(pdf);
    }
}

/* -------------------------------------------------- */

int pdf_close(pdf_t *pdf)
{
	if(pdf && !pdf->inuser_func)
	{
	    if(pdf->state != PDF_STATE_DOC_CLOSED)
	    {
	        if(pdf->page_num == -1)
                pdf_add_page(pdf);

            if(pdf->footer_func)
            {
                pdf->inuser_func = TRUE;
                pdf->footer_func(pdf);
                pdf->inuser_func = FALSE;
            }

            pdf->state = PDF_STATE_PAGE_CLOSED;
        }
        return pdf->error;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_output(pdf_t *pdf,const char *file)
{
	if(pdf && pdf_close(pdf) == PDF_OK)
	{
	    pdf->fp = stdout;

	    if(file)
	    {            
	        pdf->fp = fopen(file,"wb");
	        if(pdf->fp == NULL)
	        {                
	            pdf->error = PDF_ECREAT;
	            return pdf->error;
	        }
	    }

        pdf_put_document(pdf);

        if(pdf->fp != stdout)
        {
            fflush(pdf->fp);
            fclose(pdf->fp);
        }

	    pdf->fp = NULL;
        return pdf->error;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

static int pdf_add_font_to_list(pdf_t *pdf,pdf_font_t *fnt)
{
    int id;

    if(pdf->fnt_cnt == pdf->fnt_max)
    {
        pdf_font_t **nfnt;

        nfnt = (pdf_font_t **)realloc(pdf->fnt_lst,sizeof(pdf_font_t *) * (pdf->fnt_max + 5));
        if(nfnt == NULL)
            return PDF_ENOMEM;

        pdf->fnt_lst = nfnt;
        pdf->fnt_max += 5;
    }
    id = pdf->fnt_cnt++;
    fnt->id = pdf->fnt_cnt;
    pdf->fnt_lst[id]= fnt;
    return id;
}

/* -------------------------------------------------- */

int pdf_load_font(pdf_t *pdf,const char *family,const wchar_t *file)
{
    int ret = PDF_EINVAL;

    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED && file)
    {
        int t,id = PDF_EINVAL;
        pdf_font_t *fnt = NULL;        

        ret = pdf_load_font_file(&fnt,file,family);
        if(!ret)
        {
            for(t = 0; t < pdf->fnt_cnt; t++)
            {
                if(!strcmp(pdf->fnt_lst[t]->family,fnt->family) && pdf->fnt_lst[t]->style == fnt->style)
                {
                    id = t;
                    pdf_free_font(fnt);
                    fnt = NULL;
                    break;
                }
            }

            if(id == PDF_EINVAL)
            {
                id = pdf_add_font_to_list(pdf,fnt);
                if(id == PDF_ENOMEM)
                {
                    pdf_free_font(fnt);
                    return PDF_ENOMEM;
                }
                id++;
            }
            if(id != pdf->fnt_cur->id)
            {
                pdf->fnt_cur = pdf->fnt_lst[id - 1];
                pdf_out(pdf,"BT /F%d %.2f Tf ET\n",pdf->fnt_cur->id,pdf->fnt_pt);
            }
        }
    }
    return ret;
}

/* -------------------------------------------------- */

static int pdf_add_font(pdf_t *pdf,const char *family,int style)
{
    int id = PDF_EINVAL;
    int t,sty = 0;

    if (strcasecmp(family,"symbol") && strcasecmp(family,"zapfdingbats"))
    {
        if(!strcasecmp(family,"arial"))
            family = "helvetica";

        if(style & PDF_FNT_BOLD)
            sty|= PDF_FNT_BOLD;

        if(style & PDF_FNT_ITALIC)
            sty|= PDF_FNT_ITALIC;
    }
    for(t = 0; t < pdf->fnt_cnt; t++)
    {
        if(!strcasecmp(pdf->fnt_lst[t]->family,family) && pdf->fnt_lst[t]->style == sty)
        {
            id = t;
            break;
        }
    }
    if(id == PDF_EINVAL)
    {
        pdf_font_t *fnt;

        fnt = pdf_get_core_font(family,style);
        if(fnt == NULL)
            id = PDF_EFIND;
        else
            id = pdf_add_font_to_list(pdf,fnt);
    }
    return id;
}

/* -------------------------------------------------- */

int pdf_set_font(pdf_t *pdf,const char *family,int style,float size)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        int id,prev,change=TRUE;

        prev = pdf->fnt_cur->id  - 1;

        if(family == NULL)
            family = (const char *) pdf->fnt_cur->family;

        id = pdf_add_font( pdf,family, style);
        if(id < 0)
            return id;

        if(id != prev)
            pdf->fnt_cur = pdf->fnt_lst[id];
        else
            change = FALSE;

        if(size > 0.0 && size < 10001.0)
        {
            if(pdf->fnt_pt != size)
            {
                pdf->fnt_pt   = size;
                pdf->fnt_size = size / pdf->scale;
                pdf->txt_line = (pdf->fnt_size * PDF_LINE_RATIO);
                change = TRUE;
            }
        }
        pdf->fnt_under = (style & PDF_FNT_UNDERLINE) ? TRUE : FALSE;
        if(change)
            pdf_out(pdf,"BT /F%d %.2f Tf ET\n",pdf->fnt_cur->id,pdf->fnt_pt);
        return pdf->error;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_set_font_size(pdf_t *pdf,float size)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED && size > 0.0 && size < 10001.0)
    {
        if(pdf->fnt_pt != size)
        {
            pdf->fnt_pt   = size;
            pdf->fnt_size = size / pdf->scale;
            pdf->txt_line   = (pdf->fnt_size * PDF_LINE_RATIO);
            pdf_out(pdf,"BT /F%d %.2f Tf ET\n",pdf->fnt_cur->id,pdf->fnt_pt);
        }
        return pdf->error;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_set_font_style(pdf_t *pdf,int style)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        int id,sty;

        id = pdf->fnt_cur->id;
        sty = (style & ~PDF_FNT_UNDERLINE);
        if(sty != pdf->fnt_cur->style)
        {
            id = pdf_add_font(pdf,pdf->fnt_cur->family,sty);
            if(id < 0)
                return id;
            id++;
        }
        pdf->fnt_under = (style & PDF_FNT_UNDERLINE) ? TRUE : FALSE;
        if(id != pdf->fnt_cur->id)
        {
            pdf->fnt_cur = pdf->fnt_lst[id - 1];
            pdf_out(pdf,"BT /F%d %.2f Tf ET\n",pdf->fnt_cur->id,pdf->fnt_pt);
        }
        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_total_pages(pdf_t *pdf)
{
    int ret = 0;

    if(pdf)
        ret = pdf->page_cnt;
    return ret;
}

/* -------------------------------------------------- */

int pdf_page_number(pdf_t *pdf)
{
    int ret = 0;

    if(pdf && pdf->page_num != -1)
        ret = pdf->page_num  + 1;
    return ret;
}


/* -------------------------------------------------- */

static int pdf_accept_auto_break(pdf_t *pdf)
{
    int ret = pdf->autopb;

    if(pdf->accept_func && !pdf->inuser_func)
    {
        pdf->inuser_func = TRUE;
        ret=pdf->accept_func(pdf);
        pdf->inuser_func = FALSE;
    }
    return ret;
}

/* -------------------------------------------------- */

static void pdf_advance_page_if_needed(pdf_t *pdf,float h,float ws)
{
    if((pdf->y + h) > pdf->pageabt)
    {
        if(!pdf->inuser_func && pdf_accept_auto_break(pdf))
        {
            float x = pdf->x;

            if(ws > 0)
            {
                pdf->ws=0;
                pdf_out(pdf,"0 Tw\n");
            }
            pdf_add_page(pdf);
            pdf->x = x;
            if(ws > 0)
            {
                pdf->ws = ws;
                pdf_out(pdf,"%.3f Tw\n",ws);
            }
        }
    }
}

/* -------------------------------------------------- */

static int pdf_add_link_to_page(pdf_t *pdf,int id,float x, float y, float w,float h,int is_text)
{
    if(pdf->page_num != -1 && id > 0 && id <= pdf->lnk_cnt )
    {
        pdf_page_t *page;

        page = &(pdf->page_lst[pdf->page_num]);

        if(page->lnk_max == page->lnk_cnt)
        {
            size_t nsize;
            pdf_olnk_t *nllnk;

            nsize = sizeof(pdf_olnk_t) * (page->lnk_max + 5);
            nllnk = (pdf_olnk_t *)realloc(page->lnk_lst,nsize);
            if(nllnk == NULL)
                return PDF_ENOMEM;
            page->lnk_lst = nllnk;
            page->lnk_max += 5;
        }

        if(is_text)
            y += (h * 0.7);

        page->lnk_lst[page->lnk_cnt].id = id - 1;
        page->lnk_lst[page->lnk_cnt].x = x;
        page->lnk_lst[page->lnk_cnt].y = y;
        page->lnk_lst[page->lnk_cnt].w = w;
        page->lnk_lst[page->lnk_cnt].h = h;

        page->lnk_cnt++;

        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_add_target_link(pdf_t *pdf)
{
    int id = PDF_EINVAL;

    if(pdf && pdf->error == PDF_OK)
    {
        if(pdf->lnk_max == pdf->lnk_cnt)
        {
            size_t nsize;
            pdf_tlnk_t *nllnk;

            nsize = sizeof(pdf_tlnk_t) * (pdf->lnk_max + 5);
            nllnk = (pdf_tlnk_t *)realloc(pdf->lnk_lst,nsize);
            if(nllnk == NULL)
                return PDF_ENOMEM;
            pdf->lnk_lst = nllnk;
            pdf->lnk_max += 5;
        }
        id = pdf->lnk_cnt++;
        memset(&(pdf->lnk_lst[id]),0,sizeof(pdf_tlnk_t));
        id++;
    }
    return id;
}

/* -------------------------------------------------- */

int pdf_set_target_link_as_local(pdf_t *pdf,int id,int page, float y)
{
    if(pdf && pdf->error == PDF_OK && id > 0 && id <= pdf->lnk_cnt)
    {
        if(page < 1 || page > pdf->page_cnt)
            page = pdf->page_num;
        else
            page--;

        if(y == -1)
            y = pdf->y;

        id--;

        pdf->lnk_lst[id].local = TRUE;
        pdf->lnk_lst[id].page  = page;
        pdf->lnk_lst[id].y     = (pdf->height - y) * pdf->scale;

        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_set_target_link_as_uri(pdf_t *pdf,int id,const char *uri)
{
    if(pdf && pdf->error == PDF_OK && id > 0 && id <= pdf->lnk_cnt)
    {
        id--;
        pdf->lnk_lst[id].local = FALSE;
        strlcpy(pdf->lnk_lst[id].uri,uri,PDF_MAX_URI);
        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

void pdf_set_links_border(pdf_t *pdf,int on)
{
    if(pdf)
        pdf->lnk_border = (on) ? TRUE : FALSE;
}

/* -------------------------------------------------- */

int pdf_load_image(pdf_t *pdf,wchar_t *file)
{
    int ret;

    if(!pdf || pdf->state == PDF_STATE_DOC_CLOSED)
        return PDF_EINVAL;

    if(pdf->img_cnt == pdf->img_max)
    {
        pdf_image_t *nimg = (pdf_image_t *)realloc(pdf->img_lst,sizeof(pdf_image_t) * (pdf->img_max + 5));
        if(nimg == NULL)
            return PDF_ENOMEM;                    
        pdf->img_lst  = nimg;
        pdf->img_max += 5;
    }        

    ret = pdf_load_an_image(&(pdf->img_lst[pdf->img_cnt]),file);
    if(ret == PDF_OK)
    {
        pdf->img_cnt++;
        return pdf->img_cnt;                
    }
    return ret;
}

/* -------------------------------------------------- */

int pdf_draw_image(pdf_t *pdf,float x,float y,float w,float h,int id,int lnk)
{
    pdf_image_t *img;

    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        if(id > 0 && id <= pdf->img_cnt)
        {
            float ly = pdf->y;

            img = &(pdf->img_lst[id - 1]);
            if(img)
            {
                float tmp;

                if(w == 0 && h == 0)
                {
                    w = -96;
                    h = -96;
                }

                if(w < 0)
                {
                    tmp = w;
            		w  = ((img->w * -1.0) * 72.0);
            		w /= tmp;
            		w /= pdf->scale;
            	}
            	if(h < 0)
                {
                    tmp = h;
            		h  = ((img->h * -1.0) * 72.0);
            		h /= tmp;
            		h /= pdf->scale;
            	}

                if(w==0)
                    w = (h * img->w) / img->h;

                if(h==0)
                    h = (w * img->h) / img->w;

                if(y == -1)
                {
                    pdf_advance_page_if_needed(pdf,h,0);
                    y = pdf->y;
                    if(lnk)
                        ly = y;
                    pdf->y += h;
                }
                else if(lnk)
                {
                    ly = y;
                }
                if(x == -1)
                    x = pdf->x;

                y  = (pdf->height - (y + h)) * pdf->scale;
                ly = (pdf->height - (ly + h)) * pdf->scale;

                w *= pdf->scale;
                h *= pdf->scale;
                x *= pdf->scale;

                pdf_out(pdf,"q %.2f 0 0 %.2f %.2f %.2f cm /I%d Do Q\n",w,h,x,y,id);

                if(lnk > 0)
                    pdf_add_link_to_page(pdf,lnk,x,ly,w,-h,FALSE);

                if(img->mlen > 0)
                    pdf->alpha = TRUE;


                img->used = TRUE;

                return PDF_OK;
            }
            return PDF_EFIND;
        }
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */
/* -------------------------------------------------- */

typedef struct
{
    pdf_font_t  *fnt_cur;
    float           fnt_pt;
    float           fnt_size;
    int             fnt_under;

    int             txt_line;

    float           ws;
    float           line_width;
    int             line_style;

    rgb_t        draw_color;
    rgb_t        fill_color;
    rgb_t        text_color;
    int             color_flag;

    float           dmargin,cmargin;
    float           lmargin,rmargin;
    float           tmargin,bmargin;

    float           pageabt;
    int             autopb;

} pdf_state_t;

/* -------------------------------------------------- */

static void pdf_save_state(pdf_t *pdf,pdf_state_t *st)
{

    st->fnt_cur    = pdf->fnt_cur;
    st->fnt_pt     = pdf->fnt_pt;
    st->fnt_size   = pdf->fnt_size;
    st->fnt_under  = pdf->fnt_under;

    st->txt_line   = pdf->txt_line;

    st->ws         = pdf->ws;
    st->line_width = pdf->line_width;
    st->line_style = pdf->line_style;

    st->draw_color = pdf->draw_color;
    st->fill_color = pdf->fill_color;
    st->text_color = pdf->text_color;

    st->color_flag = pdf->color_flag;

    st->dmargin    =  pdf->dmargin;
    st->cmargin    =  pdf->cmargin;
    st->lmargin    =  pdf->lmargin;
    st->rmargin    =  pdf->rmargin;
    st->tmargin    =  pdf->tmargin;
    st->bmargin    =  pdf->bmargin;

    st->pageabt    =  pdf->pageabt;
    st->autopb     =  pdf->autopb;
}

/* -------------------------------------------------- */

static void pdf_restore_state(pdf_t *pdf,pdf_state_t *st)
{
    pdf->fnt_cur    = st->fnt_cur;
    pdf->fnt_pt     = st->fnt_pt;
    pdf->fnt_size   = st->fnt_size;
    pdf->fnt_under  = st->fnt_under;

    pdf->txt_line   = st->txt_line;

    pdf->ws         = st->ws;
    pdf->line_width = st->line_width;
    pdf->line_style = st->line_style;

    pdf->draw_color = st->draw_color;
    pdf->fill_color = st->fill_color;
    pdf->text_color = st->text_color;
    pdf->color_flag = st->color_flag;

    pdf->dmargin   =  st->dmargin;
    pdf->cmargin   =  st->cmargin;
    pdf->rmargin   =  st->rmargin;
    pdf->lmargin   =  st->lmargin;
    pdf->tmargin   =  st->tmargin;
    pdf->bmargin   =  st->bmargin;

    pdf->pageabt   =  st->pageabt;
    pdf->autopb    =  st->autopb;
}

/* -------------------------------------------------- */

int pdf_add_page(pdf_t *pdf)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED && !pdf->error && !pdf->inuser_func)
    {
        pdf_page_t *page;
        pdf_state_t st;

        if(pdf->page_cnt == pdf->page_max)
        {
            pdf_page_t *npage = (pdf_page_t *)realloc(pdf->page_lst,sizeof(pdf_page_t) * (pdf->page_max + 5));
            if(npage == NULL)
            {
                pdf->error = PDF_ENOMEM;
                return pdf->error;
            }
            pdf->page_lst  = npage;
            pdf->page_max += 5;
        }
        pdf_save_state(pdf,&st);

        if(pdf->page_num != -1)
        {
            if(pdf->footer_func)
            {
                pdf->inuser_func = TRUE;
                pdf->footer_func(pdf);
                pdf->inuser_func = FALSE;
            }
            pdf_restore_state(pdf,&st);
        }

	    /* Start new page */

        pdf->page_num = pdf->page_cnt++;

        page = &(pdf->page_lst[pdf->page_num]);

        page->lnk_lst = NULL;
        page->lnk_cnt = 0;
        page->lnk_max = 0;

        page->content = NULL;
        page->size    = 0;
        page->max     = 0;

        page->replaced = FALSE;

        pdf->state = PDF_STATE_PAGE_READY;

        pdf->x = pdf->lmargin;
        pdf->y = pdf->tmargin;

        pdf->pageabt = pdf->height - pdf->bmargin;

    	/* Set line cap style to square */

        pdf_out(pdf,"%.2f w\n",pdf->line_width * pdf->scale);
        pdf_out_color(pdf,&(pdf->draw_color),TRUE);
        pdf_out_color(pdf,&(pdf->fill_color),FALSE);
        pdf_out(pdf,"BT /F%d %.2f Tf ET\n",pdf->fnt_cur->id,pdf->fnt_pt);

        pdf->line_style = -1;
        pdf_set_line_style(pdf,st.line_style);

        if(pdf->header_func)
        {
            pdf_out(pdf,"2 J\n");

            pdf->inuser_func = TRUE;
            pdf->header_func(pdf);
            pdf->inuser_func = FALSE;

            pdf_out(pdf,"2 J\n");
            if(st.line_width != pdf->line_width)
            {
                pdf->line_width = st.line_width;
                pdf_out(pdf,"%.2f w\n",pdf->line_width * pdf->scale);
            }

            pdf_set_line_style(pdf,st.line_style);

            if(memcmp(&(pdf->draw_color),&(st.draw_color),sizeof(rgb_t)))
                pdf_set_draw_color(pdf,st.draw_color.red,st.draw_color.green,st.draw_color.blue);

            if(memcmp(&(pdf->fill_color),&(st.fill_color),sizeof(rgb_t)))
                pdf_set_fill_color(pdf,st.fill_color.red,st.fill_color.green,st.fill_color.blue);

            if(memcmp(&(pdf->text_color),&(st.text_color),sizeof(rgb_t)))
                pdf_set_text_color(pdf,st.text_color.red,st.text_color.green,st.text_color.blue);

            if(st.fnt_cur != pdf->fnt_cur || st.fnt_pt != pdf->fnt_pt)
                pdf_set_font(pdf,st.fnt_cur->family,st.fnt_cur->style,st.fnt_pt);

            pdf->fnt_under = st.fnt_under;
        }

        return pdf->error;
    }
    return PDF_EINVAL;
}


/* -------------------------------------------------- */

void pdf_set_compression(pdf_t *pdf,int on)
{
    if(pdf)
        pdf->deflate_it = (on) ? TRUE : FALSE;
}

/* -------------------------------------------------- */

int pdf_get_orientation(pdf_t *pdf)
{
    if(pdf)
        return pdf->orientation;
    return 0;
}

/* -------------------------------------------------- */

const char *pdf_get_orientationc(pdf_t *pdf)
{
    if(pdf)
    {
        switch(pdf->orientation)
        {
            case PDF_PORTRAIT:
                return "Portrait";
            case PDF_LANDSCAPE:
            default:
                return "Landscape";

        }
    }
    return "None";
}

/* -------------------------------------------------- */

const wchar_t *pdf_get_orientationw(pdf_t *pdf)
{
    if(pdf)
    {
        switch(pdf->orientation)
        {
            case PDF_PORTRAIT:
                return L"Portrait";
            case PDF_LANDSCAPE:
            default:
                return L"Landscape";

        }
    }
    return L"None";
}

/* -------------------------------------------------- */

int pdf_get_size(pdf_t *pdf)
{
    if(pdf)
        return pdf->size;
    return 0;
}

/* -------------------------------------------------- */

const char *pdf_get_sizec(pdf_t *pdf)
{
    if(pdf)
    {
        switch(pdf->size)
        {
            case PDF_SIZE_A3:
                return "A3";
            case PDF_SIZE_A5:
                return "A5";
            case PDF_SIZE_LETTER:
                return "Letter";
            case PDF_SIZE_LEGAL:
                return "Legal";
            case PDF_SIZE_A4:
            default:
                return "A4";

        }
    }
    return "None";
}

/* -------------------------------------------------- */

const wchar_t *pdf_get_sizew(pdf_t *pdf)
{
    if(pdf)
    {
        switch(pdf->size)
        {
            case PDF_SIZE_A3:
                return L"A3";
            case PDF_SIZE_A5:
                return L"A5";
            case PDF_SIZE_LETTER:
                return L"Letter";
            case PDF_SIZE_LEGAL:
                return L"Legal";
            case PDF_SIZE_A4:
            default:
                return L"A4";

        }
    }
    return L"None";
}

/* -------------------------------------------------- */

int pdf_get_unit(pdf_t *pdf)
{
    if(pdf)
        return pdf->unit;
    return 0;
}

/* -------------------------------------------------- */

const char  *pdf_get_unitc(pdf_t *pdf)
{
    if(pdf)
    {
        switch(pdf->unit)
        {
            case PDF_UNIT_CM:
                return "cm";
            case PDF_UNIT_MM:
                return "mm";
            case PDF_UNIT_IN:
                return "inches";
            case PDF_UNIT_PT:
            default:
                return "pt";
        }
    }
    return "None";
}

/* -------------------------------------------------- */

const wchar_t *pdf_get_unitw(pdf_t *pdf)
{
    if(pdf)
    {
        switch(pdf->unit)
        {
            case PDF_UNIT_CM:
                return L"cm";
            case PDF_UNIT_MM:
                return L"mm";
            case PDF_UNIT_IN:
                return L"inches";
            case PDF_UNIT_PT:
            default:
                return L"pt";
        }
    }
    return L"None";
}

/* -------------------------------------------------- */

float pdf_get_x(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->x;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_y(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->y;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_width(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->width;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_height(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->height;
    return ret;
}

/* -------------------------------------------------- */

const char *pdf_get_font_family(pdf_t *pdf)
{
    if(pdf)
        return (const char *)pdf->fnt_cur->family;
    return "none";
}

/* -------------------------------------------------- */

int pdf_get_font_style(pdf_t *pdf)
{
    int ret = PDF_FNT_REGULAR;

    if(pdf)
    {
        ret = pdf->fnt_cur->style;
        if(pdf->fnt_under)
            ret |= PDF_FNT_UNDERLINE;
    }
    return ret;
}


/* -------------------------------------------------- */

const char *pdf_get_font_name(pdf_t *pdf)
{
    if(pdf)
        return (const char *)pdf->fnt_cur->name;
    return "none";
}

/* -------------------------------------------------- */

int pdf_get_font_size(pdf_t *pdf)
{
    if(pdf)
        return pdf->fnt_pt;
    return 0;
}

/* -------------------------------------------------- */

wchar_t *pdf_get_metadata(pdf_t *pdf,int which)
{
    wchar_t *ret = L"";
    if(pdf && which > -1 && which < PDF_MAX_MD_FIELDS)
    {
        if(pdf->metadata[which])
            ret = pdf->metadata[which];
    }
    return ret;
}

/* -------------------------------------------------- */

int pdf_set_metadata(pdf_t *pdf,int which,const wchar_t *fmt,...)
{
    if(pdf && fmt && which > -1 && which < PDF_MAX_MD_FIELDS)
    {
        va_list vl;
        wchar_t *ret;

        va_start(vl,fmt);
        ret = wcsallocf(NULL,fmt,vl);
        va_end(vl);

        if(ret)
        {
            pdf_free(pdf->metadata[which]);
            pdf->metadata[which] = ret;
            return PDF_OK;
        }

        return PDF_ENOMEM;
    }

    return PDF_EINVAL;
}

/* -------------------------------------------------- */

void pdf_set_x(pdf_t *pdf,float x)
{
    if(pdf && pdf->page_num != -1)
    {
        if(x >= 0)
            pdf->x = x;
        else
            pdf->x = (pdf->width + x);
    }
}

/* -------------------------------------------------- */

void pdf_set_y(pdf_t *pdf,float y,int resetx)
{
    if(pdf && pdf->page_num != -1)
    {
        if(y >= 0)
            pdf->y = y;
        else
            pdf->y = (pdf->height + y);

        if(resetx)
            pdf->x = pdf->lmargin;
    }
}

/* -------------------------------------------------- */

void pdf_line_feed(pdf_t *pdf)
{
    if(pdf  &&  pdf->page_num != -1)
    {
        pdf->x = pdf->lmargin;
        if(pdf->lasth == 0.0)
            pdf->y+=pdf->txt_line;
        else
            pdf->y += pdf->lasth;
    }
}

/* -------------------------------------------------- */

void pdf_line_break(pdf_t *pdf,float h)
{
    if(pdf &&  pdf->page_num != -1)
    {
        pdf->x = pdf->lmargin;
        if(h == 0.0)
            pdf->y+=pdf->txt_line;
        else
            pdf->y += h;
    }
}

/* -------------------------------------------------- */

float pdf_get_line_height(pdf_t *pdf)
{
    float ret = 0;
    if(pdf)
        ret = pdf->txt_line;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_cell_height(pdf_t *pdf)
{
    float ret = 0;
    if(pdf)
        ret = (pdf->lasth) ? pdf->lasth : pdf->txt_line;
    return ret;
}


/* -------------------------------------------------- */

void pdf_set_cell_margin(pdf_t *pdf,float mar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf->cmargin = (mar < 0.001) ? 0.0 : mar;
        pdf->lasth   = 0.0;
    }
}


/* -------------------------------------------------- */

void pdf_set_top_margin(pdf_t *pdf,float mar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf->tmargin = (mar < 0.001) ? 0.0 : mar;
        if(pdf->page_num != -1 && pdf->y < pdf->tmargin)
    		pdf->y = pdf->tmargin;
    }
}

/* -------------------------------------------------- */

void pdf_set_left_margin(pdf_t *pdf,float mar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf->lmargin = (mar < 0.001) ? 0.0 : mar;
        if(pdf->page_num != -1 && pdf->x < pdf->lmargin)
    		pdf->x = pdf->lmargin;
    }
}

/* -------------------------------------------------- */

void pdf_set_right_margin(pdf_t *pdf,float mar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
        pdf->rmargin = (mar < 0.001) ? 0.0 : mar;
}

/* -------------------------------------------------- */

void pdf_set_bottom_margin(pdf_t *pdf,float mar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf->bmargin = (mar < 0.001) ? 0.0 : mar;
        pdf->pageabt = pdf->height - pdf->bmargin;
    }
}

/* -------------------------------------------------- */

void pdf_set_margins(pdf_t *pdf,float lmar,float tmar,float rmar,float bmar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf->rmargin = (rmar < 0.001) ? 0.0 : rmar;
        pdf->lmargin = (lmar < 0.001) ? 0.0 : lmar;
        pdf->tmargin = (tmar < 0.001) ? 0.0 : tmar;
        pdf->bmargin = (bmar < 0.001) ? 0.0 : bmar;

        pdf->pageabt = pdf->height - pdf->bmargin;

        if(pdf->page_num != -1)
        {
            if(pdf->x < pdf->lmargin)
    		    pdf->x = pdf->lmargin;

    		if(pdf->y < pdf->tmargin)
        		pdf->y = pdf->tmargin;
        }
    }
}

/* -------------------------------------------------- */

float pdf_get_left_margin(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->lmargin;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_right_margin(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->rmargin;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_top_margin(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->tmargin;
    return ret;
}

/* -------------------------------------------------- */

float pdf_get_bottom_margin(pdf_t *pdf)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf->bmargin;
    return ret;
}

/* -------------------------------------------------- */

void pdf_set_auto_page_break(pdf_t *pdf,int autob,float mar)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf->autopb = (autob) ? TRUE : FALSE;
        pdf->pageabt = pdf->height - pdf->bmargin;
    }
}

/* -------------------------------------------------- */

void pdf_set_page_number_alias(pdf_t *pdf,const wchar_t *alias,...)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        pdf_free(pdf->page_nb_alias);
        pdf->page_nb_alias = NULL;

        if(alias)
        {
            va_list vl;

            va_start(vl,alias);
            pdf->page_nb_alias = pdf_wchar_to_cp1252(NULL,alias,vl,FALSE);
            va_end(vl);
        }
    }
}

/* -------------------------------------------------- */

void pdf_set_display_mode(pdf_t *pdf,int zoom,int layout)
{
    if(pdf)
    {
        if(zoom < PDF_ZOOM_DEFAULT)
            pdf->zoom = PDF_ZOOM_DEFAULT;
        else if(zoom > PDF_ZOOM_MAX)
            pdf->zoom = PDF_ZOOM_MAX;
        else
            pdf->zoom = zoom;

        if(layout < PDF_PAGE_DEFAULT)
            pdf->layout = PDF_PAGE_DEFAULT;
        else if(layout > PDF_PAGE_ROLL)
            pdf->layout = PDF_PAGE_ROLL;
        else
            pdf->layout = layout;
	}
}

/* -------------------------------------------------- */

int pdf_set_header_function(pdf_t *pdf,pdf_user_func_t func)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED && !pdf->inuser_func)
    {
        pdf->header_func = func;
        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_set_footer_function(pdf_t *pdf,pdf_user_func_t func)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED && !pdf->inuser_func)
    {
        pdf->footer_func = func;
        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

int pdf_set_accept_auto_page_break(pdf_t *pdf,pdf_user_func_t func)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED && !pdf->inuser_func)
    {
        pdf->accept_func = func;
        return PDF_OK;
    }
    return PDF_EINVAL;
}

/* -------------------------------------------------- */

void pdf_set_draw_color(pdf_t *pdf,unsigned char r, unsigned char g, unsigned char b)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        if(r != pdf->draw_color.red || g != pdf->draw_color.green || b != pdf->draw_color.blue)
        {
            pdf->draw_color.red = r;
            pdf->draw_color.green = g;
            pdf->draw_color.blue = b;

            pdf_out_color(pdf,&(pdf->draw_color),TRUE);
		}
	}
}

/* -------------------------------------------------- */

void pdf_set_fill_color(pdf_t *pdf,unsigned char r, unsigned char g, unsigned char b)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        if(r != pdf->fill_color.red || g != pdf->fill_color.green || b != pdf->fill_color.blue)
        {
            pdf->fill_color.red = r;
            pdf->fill_color.green = g;
            pdf->fill_color.blue = b;

            if(memcmp(&(pdf->fill_color),&(pdf->text_color),sizeof(rgb_t)))
                pdf->color_flag = TRUE;
            else
                pdf->color_flag = FALSE;

            pdf_out_color(pdf,&(pdf->fill_color),FALSE);
		}
	}
}

/* -------------------------------------------------- */

void pdf_set_text_color(pdf_t *pdf,unsigned char r, unsigned char g, unsigned char b)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        if(r != pdf->text_color.red || g != pdf->text_color.green || b != pdf->text_color.blue)
        {
            pdf->text_color.red = r;
            pdf->text_color.green = g;
            pdf->text_color.blue = b;

            if(memcmp(&(pdf->fill_color),&(pdf->text_color),sizeof(rgb_t)))
                pdf->color_flag = TRUE;
            else
                pdf->color_flag = FALSE;
		}
	}
}

/* -------------------------------------------------- */

void pdf_set_line_width(pdf_t *pdf,float w)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        if(pdf->line_width != w)
        {
            pdf->line_width = w;
            pdf_out(pdf,"%.2f w\n",pdf->line_width * pdf->scale);
		}
	}
}

/* -------------------------------------------------- */

void pdf_set_line_style(pdf_t *pdf,int what)
{
    if(pdf && pdf->state != PDF_STATE_DOC_CLOSED)
    {
        if(pdf->line_style != what)
        {
            const char *style = "[] 0";

            switch(what)
            {
                case PDF_DOTTED_LINE:   /* Dotted line */
                    style = "[1 2] 0";
                    break;
                case PDF_DASHED_LINE:   /* Dashed line */
                    style = "[3 3] 0";
                    break;
                case PDF_MORSE_LINE:    /* Dot-Dash-Dot-Dash line */
                    style = "[1 3 3 3] 0";
                    break;
                case PDF_DOTGAP_LINE:   /* Dotted line with bigger gaps*/
                    style = "[1 4] 0";
                    break;
                case PDF_DASHGAP_LINE:  /* Dashed line with bigger gaps */
                    style = "[3 6] 0";
                    break;
                case PDF_SOLID_LINE:    /* Solid line */
                default:
                    break;
            }
            pdf->line_style = what;
            pdf_out(pdf,"%s d\n",style);
		}
	}
}


/* -------------------------------------------------- */

static float pdf_font_size_for_width(pdf_t *pdf,pdf_font_t  *fnt,float width,void *orig,size_t len)
{
    float lst,w,tmp,pt;
    size_t t;
    unsigned char *str = (unsigned char *)orig;

    w = 0;
    for(t=0;t<len;t++)
        w += fnt->cw[ str[t] ];
    w /= 1000;

    pt = 8.0;
    tmp = w * (pt / pdf->scale);

    if(width > tmp) /* We need to go up */
    {
        do {
            pt += 0.01;
            tmp = w * (pt / pdf->scale) ;
        } while(tmp < width);
    }
    else if (width < tmp)
    {
        lst = 0;
        do {
            lst = pt;
            pt -= 0.01;
            tmp = w * (pt / pdf->scale);
        } while (tmp > width);

        if(tmp < width)
            pt = lst;
    }
    return pt;
}

/* -------------------------------------------------- */

float pdf_get_font_size_for_width(pdf_t *pdf,float width,const wchar_t *fmt,...)
{
    float ret = 0.0;

    if(pdf && fmt)
    {
        va_list vl;
        char *cp1252;
        size_t len;

        va_start(vl,fmt);
        cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,FALSE);
        va_end(vl);

        if(cp1252)
        {
            ret = pdf_font_size_for_width(pdf,pdf->fnt_cur,width,cp1252,len);
            free(cp1252);
        }
    }
    return ret;
}


/* -------------------------------------------------- */

static float pdf_font_size_for_height(pdf_t *pdf,pdf_font_t  *fnt,float height)
{
    float lst,h,tmp,pt;

    h = fnt->tal / 1000;
    pt = 8.0;
    tmp = h * (pt / pdf->scale);

    if(height > tmp) /* We need to go up */
    {
        do {
            pt += 0.01;
            tmp = h * (pt / pdf->scale) ;
        } while(tmp < height);
    }
    else if (height < tmp)
    {
        lst = 0;
        do {
            lst = pt;
            pt -= 0.01;
            tmp = h * (pt / pdf->scale);
        } while (tmp > height);

        if(tmp < height)
            pt = lst;
    }
    return pt;
}

/* -------------------------------------------------- */

float pdf_get_font_size_for_height(pdf_t *pdf,float height)
{
    float ret = 0.0;

    if(pdf)
        ret = pdf_font_size_for_height(pdf,pdf->fnt_cur,height);
    return ret;
}


/* -------------------------------------------------- */

static float pdf_font_size_for_rect(pdf_t *pdf,pdf_font_t  *fnt,float w,float h,void *orig,size_t len)
{
    float ptw,pth;

    ptw = pdf_font_size_for_width(pdf,fnt,w,orig,len);
    pth = pdf_font_size_for_height(pdf,fnt,h);

    return (ptw < pth) ? ptw : pth;
}

/* -------------------------------------------------- */

float pdf_get_font_size_for_rect(pdf_t *pdf,float w,float h,const wchar_t *fmt,...)
{
    float ret = 0.0;

    if(pdf && fmt)
    {
        va_list vl;
        char *cp1252;
        size_t len;

        va_start(vl,fmt);
        cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,FALSE);
        va_end(vl);

        if(cp1252)
        {
            ret = pdf_font_size_for_rect(pdf,pdf->fnt_cur,w,h,cp1252,len);
            free(cp1252);
        }
    }
    return ret;
}


/* -------------------------------------------------- */

static float do_pdf_get_text_width(pdf_t *pdf,void *orig,size_t len)
{
    float ret = 0.0;
    size_t t;
    unsigned char *str = (unsigned char *)orig;

    for(t=0;t<len;t++)
        ret += pdf->fnt_cur->cw[ str[t] ];
    return ret * pdf->fnt_size / 1000.0;
}

/* -------------------------------------------------- */

float pdf_get_text_width(pdf_t *pdf,const wchar_t *fmt,...)
{
    float ret = 0.0;

    if(pdf && fmt)
    {
        va_list vl;
        char *cp1252;
        size_t len;

        va_start(vl,fmt);
        cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,FALSE);
        va_end(vl);

        if(cp1252)
        {
            ret = do_pdf_get_text_width(pdf,cp1252,len);
            free(cp1252);
        }
    }
    return ret;
}

/* -------------------------------------------------- */

static void do_pdf_get_text_rect(pdf_t *pdf,int *w,int *h,void *orig,size_t len)
{
    float rw = 0;
    size_t t;
    unsigned char *str = (unsigned char *)orig;

    for(t=0;t<len;t++)
        rw += pdf->fnt_cur->cw[ str[t] ];
    *w = rw * pdf->fnt_size / 1000.0;
    *h = (pdf->fnt_cur->tal * pdf->fnt_size / 1000.0);
}

/* -------------------------------------------------- */

void pdf_get_text_rect(pdf_t *pdf,int *w,int *h,const wchar_t *fmt,...)
{
    if(w)
        *w = 0;
    if(h)
        *h = 0;

    if(pdf && fmt)
    {
        va_list vl;
        char *cp1252;
        size_t len;

        va_start(vl,fmt);
        cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,FALSE);
        va_end(vl);

        if(cp1252 && w && h)
        {
            do_pdf_get_text_rect(pdf,w,h,cp1252,len);
            free(cp1252);
        }
    }
}

/* -------------------------------------------------- */

void pdf_move_to(pdf_t *pdf,float nx,float ny)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
        if(nx > -1)
            pdf->x = (nx < pdf->width) ? nx : pdf->width;

        if(ny > -1)
            pdf->y = (ny < pdf->height) ? ny : pdf->height;
	}
}

/* -------------------------------------------------- */

void pdf_line_to(pdf_t *pdf,float nx,float ny)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
        float dx,dy;

        dx = pdf->x;
        dy = pdf->y;

        if(nx < 0)
            nx = pdf->x;
        else if(nx > pdf->width)
            nx = pdf->width;

        if(ny < 0)
            ny = pdf->y;
        else if(ny > pdf->height)
            nx = pdf->height;

        if(nx!=dx || ny!=dy)
        {
            pdf->x = nx;
            pdf->y = ny;

            dx *= pdf->scale;
            dy = (pdf->height - dy) * pdf->scale;

            nx *= pdf->scale;
            ny = (pdf->height - ny) * pdf->scale;

            pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",dx,dy,nx,ny);
        }
	}
}

/* -------------------------------------------------- */

void pdf_draw_line(pdf_t *pdf,float dx,float dy,float hx,float hy)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {

        if(dx < 0)
            dx = pdf->x;
        else if(dx > pdf->width)
            dx = pdf->width;

        if(dy < 0)
            dy = pdf->y;
        else if(dy > pdf->height)
            dx = pdf->height;

        if(hx < 0)
            hx = pdf->x;
        else if(hx > pdf->width)
            hx = pdf->width;

        if(hy < 0)
            hy = pdf->y;
        else if(hy > pdf->height)
            hx = pdf->height;

        if(hx!=dx || hy!=dy)
        {
            dx *= pdf->scale;
            dy = (pdf->height - dy) * pdf->scale;
            hx *= pdf->scale;
            hy = (pdf->height - hy) * pdf->scale;

            pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",dx,dy,hx,hy);
        }
	}
}

/* -------------------------------------------------- */

#define PS_KAPPA 0.5522847498

static void do_pdf_draw_rectangle(pdf_t *pdf,float x,float y,float w,float h,int attr)
{
    int brd = (attr & PDF_A_FULL_B);
    int rnd = (attr & PDF_A_RND_F);
    float k,hh;

    k  = pdf->scale;
    hh = pdf->height;

    if(brd == PDF_A_FULL_B)
    {
        const char *op = (attr & PDF_A_FILLED) ? "B" : "S";

        if(rnd)
        {
            float xc,yc,r,rr;

            r  = 3.5;
            rr = PS_KAPPA * r;

            if (rnd & PDF_A_RND_TL)
                pdf_out(pdf,"%.2f %.2f m\n",(x + r) * k,(pdf->height-y) *k);
            else
                pdf_out(pdf,"%.2f %.2f m\n",x * k,(pdf->height-y) *k);

            if (rnd & PDF_A_RND_TR)
            {
                xc = x + w - r;
                yc = y + r;
                pdf_out(pdf,"%.2f %.2f l ", xc * k,(pdf->height-y) * k );
                pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(xc+rr)*k,(hh-(yc-r))*k,(xc+r)*k,(hh-(yc-rr))*k,(xc + r)*k,(hh-yc)*k);
            }
            else
            {
                pdf_out(pdf,"%.2f %.2f l ", (x + w) * k,(pdf->height-y) * k );
            }

            if (rnd & PDF_A_RND_BR)
            {
                xc = x + w - r;
                yc = y + h - r;
                pdf_out(pdf,"%.2f %.2f l ",(x+w)*k,(pdf->height-yc)*k);
                pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(xc+r)*k,(hh-(yc+rr))*k,(xc+rr)*k,(hh-(yc+r))*k,xc*k,(hh-(yc+r))*k);
            }
            else
            {
                pdf_out(pdf,"%.2f %.2f l ", (x + w) * k,(pdf->height-(y + h)) * k );
            }


            if (rnd & PDF_A_RND_BL)
            {
                xc =  x + r;
                yc =  y + h - r;
                pdf_out(pdf,"%.2f %.2f l\n",xc * k,(pdf->height-(y+h))*k);
                pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(xc-rr)*k,(hh-(yc+r))*k,(xc-r)*k,(hh-(yc+rr))*k,(xc-r)*k,(hh-yc)*k);
            }
            else
            {
                pdf_out(pdf,"%.2f %.2f l ", x * k,(pdf->height-(y + h)) * k );
            }


            if (rnd & PDF_A_RND_TL)
            {
                xc = x + r ;
                yc = y + r;
                pdf_out(pdf,"%.2f %.2f l\n",x*k,(pdf->height-yc)*k );
                pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(xc-r)*k,(hh-(yc-rr))*k,(xc-rr)*k,(hh-(yc-r))*k,xc*k,(hh-(yc-r))*k);
            }
            else
            {
                pdf_out(pdf,"%.2f %.2f l ", x * k,(pdf->height-y) * k );
            }

            pdf_out(pdf,"%s\n",op);
        }
        else
        {

            pdf_out(pdf,"%.2f %.2f %.2f %.2f re %s\n",x * k,(hh - y) * k,w * k, -h * k,op);
        }
    }
    else
    {
        x *= k;
        y = (hh - y) * k;
        w *= k;
        h =  -h * k;

        if(attr & PDF_A_FILLED)
            pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n",x,y,w,h);

        if(brd)
        {
            if(attr & PDF_A_LFT_B)
                pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",x, y , x, y + h);

            if(attr &  PDF_A_TOP_B)
                pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",x, y , x + w, y);

            if(attr & PDF_A_RGT_B)
                pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",x + w, y, x + w , y + h);

            if(attr & PDF_A_BOT_B)
                pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",x, y + h , x + w, y + h);
        }
    }
}

/* -------------------------------------------------- */

void pdf_draw_rectangle(pdf_t *pdf,float dx,float dy,float w,float h,int attr,int lnk)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
        if(dx < 0)
            dx = pdf->x;
        else if(dx > pdf->width)
            dx = pdf->width;

        if(dy < 0)
            dy = pdf->y;
        else if(dy > pdf->height)
            dx = pdf->height;

        do_pdf_draw_rectangle(pdf,dx,dy,w,h,attr);

        if(lnk > 0)
            pdf_add_link_to_page(pdf,lnk,dx*pdf->scale,(pdf->height - dy)*pdf->scale,w*pdf->scale,h*pdf->scale,FALSE);
    }
}

/* -------------------------------------------------- */

void pdf_draw_circle(pdf_t *pdf,float x,float y,float r,int attr,int lnk)
{
    pdf_draw_ellipse(pdf,x,y,r,r,attr,lnk);
}

/* -------------------------------------------------- */

void pdf_draw_ellipse(pdf_t *pdf,float x,float y,float rx,float ry,int attr,int lnk)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
        const char *op;
        float lx,ly,k,h;

        if((attr & PDF_A_FULL_B) == PDF_A_FULL_B)
            attr |= PDF_A_RND_F;

        if(attr & PDF_A_FILLED)
            op = (attr & PDF_A_RND_F) ? "B" : "f";
        else
            op = "S";

        lx = PS_KAPPA * rx;
        ly = PS_KAPPA * ry;

        k = pdf->scale;
        h = pdf->height;

        pdf_out(pdf,"%.2f %.2f m\n",(x+rx)*k,(h-y)*k);
        pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(x+rx)*k,(h-(y-ly))*k,(x+lx)*k,(h-(y-ry))*k,x*k,(h-(y-ry))*k);
        pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(x-lx)*k,(h-(y-ry))*k,(x-rx)*k,(h-(y-ly))*k,(x-rx)*k,(h-y)*k);
        pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c\n",(x-rx)*k,(h-(y+ly))*k,(x-lx)*k,(h-(y+ry))*k, x*k,(h-(y+ry))*k);
        pdf_out(pdf,"%.2f %.2f %.2f %.2f %.2f %.2f c %s\n",(x+lx)*k,(h-(y+ry))*k,(x+rx)*k,(h-(y+ly))*k,(x+rx)*k,(h-y)*k,op);

        if(lnk > 0)
            pdf_add_link_to_page(pdf,lnk,(x - rx) * k ,(h - (y - ry)) * k, rx * 2 * k,ry * 2 * k,FALSE);

    }
}


/* -------------------------------------------------- */

static int count_spaces(const char *str)
{
    int cnt = 0;

    while(*str)
    {
        if(*str++ == ' ')
            cnt++;
    }
    return cnt;
}

/* -------------------------------------------------- */

void pdf_draw_text(pdf_t *pdf,float x,float y,const wchar_t *fmt,...)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY && fmt)
    {
        char *cp1252;
        size_t len;
        va_list vl;

        va_start(vl,fmt);
        cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,FALSE);
        va_end(vl);

        if(cp1252 == NULL)
        {
            pdf->error = PDF_ENOMEM;
            return;
        }

        if(pdf->color_flag)
        {
            pdf_out(pdf,"q ");
            pdf_out_color(pdf,&(pdf->text_color),FALSE);
        }

        pdf->fnt_cur->used = TRUE;

        pdf_out(pdf,"BT %.2f %.2f Td (",x * pdf->scale,(pdf->height - y) * pdf->scale);
        pdf_out_string(pdf,cp1252,len);
        pdf_out(pdf,") Tj ET\n");

        if(pdf->fnt_under)
        {
            float up,ut,w;

            up = pdf->fnt_cur->up;
            ut = pdf->fnt_cur->ut;

            w  = do_pdf_get_text_width(pdf,cp1252,len);
            w += pdf->ws * count_spaces(cp1252);

            y = (pdf->height - (y - up / 1000.0 * pdf->fnt_size)) * pdf->scale;

            pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n",x * pdf->scale,y,w * pdf->scale,(-ut / 1000.0 * pdf->fnt_pt));
        }

        free(cp1252);

        if(pdf->color_flag)
            pdf_out(pdf,"Q\n");

	}
}

/* -------------------------------------------------- */

static void do_pdf_draw_cell(pdf_t *pdf,float w,float h,int attr,int lnk,char *text,size_t len)
{
    float k,pt = 0;
    int feed,fit;


    pdf_advance_page_if_needed(pdf,h,pdf->ws);

    if(w == 0.0)
        w = (pdf->width - pdf->rmargin - pdf->x);

    if(h == 0.0)
        h = (pdf->lasth == 0.0) ? pdf->txt_line : pdf->lasth;

    pdf->lasth = h;

    k = pdf->scale;

    feed   = PDF_A_FEED(attr);
    fit    = (attr & PDF_A_FIT_TEXT) ? TRUE : FALSE;

    if(attr & (PDF_A_FULL_B|PDF_A_FILLED))
        do_pdf_draw_rectangle(pdf,pdf->x,pdf->y,w,h,attr);

    if(lnk > 0)
        pdf_add_link_to_page(pdf,lnk,pdf->x * k, (pdf->height - pdf->y) * k ,w * k, (h * k),FALSE);

    if(text && len > 0)
    {
        float ws,dx = pdf->cmargin;

        pdf_out(pdf,"q\n");

        if(pdf->color_flag)
            pdf_out_color(pdf,&(pdf->text_color),FALSE);

        /* Make sure it fits */

        ws = do_pdf_get_text_width(pdf,text,len);

        if(fit && ws + (pdf->cmargin * 2) > w)
        {
            ws = w - (pdf->cmargin * 2);
            pt = pdf_font_size_for_width(pdf,pdf->fnt_cur,ws,text,len);
            if(pt < pdf->fnt_pt)
            {
                float tmp = pdf->fnt_pt;

                pdf_out(pdf,"BT /F%d %.2f Tf ET \n",pdf->fnt_cur->id,pt);
                pdf->fnt_pt = pt;
                pdf->fnt_size = pt / k;
                pdf->txt_line = (pdf->fnt_size * PDF_LINE_RATIO);
                pt = tmp;
            }
        }
        else
        {
            while(ws + (pdf->cmargin * 2) > w && len > 1)
                ws = do_pdf_get_text_width(pdf,text,--len);
        }
        if(attr & PDF_A_RIGHT_J)
            dx = (w - pdf->cmargin - ws);
        else if(attr & PDF_A_CENTRE_J)
            dx = (w - ws) / 2;
        //else if(attr & PDF_A_FULL_J)
        //    dx = pdf->cmargin;

        pdf->fnt_cur->used = TRUE;

        pdf_out(pdf,"BT %.2f %.2f Td (",(dx + pdf->x) * k,(pdf->height - (pdf->y + 0.5 * h + 0.25 * pdf->fnt_size)) * k);
        pdf_out_string(pdf,text,len);
        pdf_out(pdf,") Tj ET\n");

        if(pdf->fnt_under)
        {
            float y = (pdf->y + 0.5 * h + 0.25 * pdf->fnt_size);
            float up,ut;

            up = pdf->fnt_cur->up;
            ut = pdf->fnt_cur->ut;

            ws += pdf->ws * count_spaces(text);

            y = (pdf->height - (y - up / 1000.0 * pdf->fnt_size)) * k;

            pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n",(dx + pdf->x) * k,y,ws * k,(-ut / 1000.0 * pdf->fnt_pt));
        }

        pdf_out(pdf,"Q\n");

        if(pt != 0)
        {
            pdf->fnt_pt = pt;
            pdf->fnt_size = pt / k;
            pdf->txt_line = (pdf->fnt_size * PDF_LINE_RATIO);
        }
    }

    switch(feed)
    {
        case PDF_A_LINE_F:
            pdf->y += h;
            pdf->x = pdf->lmargin;
            break;
        case PDF_A_BELOW_F:
            pdf->y += h;
            break;
        case PDF_A_RIGHT_F:
        default:
            pdf->x += w;
            break;
    }
}

/* -------------------------------------------------- */

void pdf_draw_cell(pdf_t *pdf,float w,float h,int attr,int lnk,const wchar_t *fmt,...)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
       char *cp1252 = NULL;
       size_t len = 0;


        if(fmt)
        {
            va_list vl;

            va_start(vl,fmt);
            cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,FALSE);
            va_end(vl);

            if(cp1252 == NULL)
            {
                pdf->error = PDF_ENOMEM;
                return;
            }
        }

        do_pdf_draw_cell(pdf,w,h,attr,lnk,cp1252,len);

        if(cp1252)
            free(cp1252);
    }
}

/* -------------------------------------------------- */

void pdf_draw_multi_cell(pdf_t *pdf,float w,float h,int attr,const wchar_t *fmt,...)
{
    char *cp1252 = NULL;
    size_t len = 0,pos,cur,sep;
    va_list vl;
    pdf_font_t *fnt = pdf->fnt_cur;
    float l,ls,max,ws;
    int border,nl,ns;

    if(!pdf || pdf->state != PDF_STATE_PAGE_READY)
        return;

    if(fmt==NULL)
    {
        pdf_draw_cell(pdf,w,h,attr,0,NULL);
        return;
    }

    va_start(vl,fmt);
    cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,TRUE);
    va_end(vl);

    if(cp1252 == NULL)
    {
        pdf->error = PDF_ENOMEM;
        return;
    }
    else if(len == 0)
    {
        free(cp1252);
        do_pdf_draw_cell(pdf,w,h,attr,0,NULL,len);
        return;
    }

    /* Getting ready... */

    if(w == 0.0)
        w = (pdf->width - pdf->rmargin - pdf->x);

    max =  (w - 2 * pdf->cmargin) * 1000 / pdf->fnt_size;

    if(cp1252[len-1]=='\n')
        len--;

    /* Prepare attributes */

    border = PDF_A_BORDER(attr);

    attr &= 0xff00;         /* Remove feed and border */
    attr |= PDF_A_BELOW_F ;   /* Force Below Feed */

    if(border)
    {
        if(border == PDF_A_FULL_B)
        {
            attr |= (PDF_A_LFT_B | PDF_A_RGT_B | PDF_A_TOP_B);
        }
        else
        {
            if(border & PDF_A_RGT_B)
                attr |= PDF_A_RGT_B;
            if(border & PDF_A_LFT_B)
                attr |= PDF_A_LFT_B;
            if(border & PDF_A_TOP_B)
                attr |= PDF_A_TOP_B;
        }
    }

    ws = pdf->ws;

    pos = cur = 0;
    sep = 0xffffffff;
    ns = 0;
    nl = 1;
    ls = l = 0;

    while(cur < len)
    {
        if(cp1252[cur] == '\n')
        {
            if(pdf->ws > 0)
            {
                pdf->ws = 0;
                pdf_out(pdf,"0 Tw\n");
            }
            do_pdf_draw_cell(pdf,w,h,attr,0,cp1252 + pos,cur - pos);
            cur++;
            sep = 0xffffffff;
            pos = cur;
            ns = 0;
            nl++;
            l = 0;
            if(border && nl == 2)
                attr &= ~PDF_A_TOP_B;
            continue;
        }

        if(cp1252[cur]==' ')
        {
            sep = cur;
            ls = l;
            ns++;
        }
        l += fnt->cw[ (int)((unsigned char)cp1252[cur]) ];
        if(l > max)
        {
            if(sep == 0xffffffff)
            {
                if(cur == pos)
                    cur++;

                if(pdf->ws > 0)
                {
                    pdf->ws = 0;
                    pdf_out(pdf,"0 Tw\n");
                }
                do_pdf_draw_cell(pdf,w,h,attr,0,cp1252 + pos,cur - pos);
            }
            else
            {
                if(attr & PDF_A_FULL_J)
                {
                    pdf->ws = (ns > 1) ? (max - ls) / 1000.0 * pdf->fnt_size / (ns - 1) : 0;
					pdf_out(pdf,"%.3f Tw\n",pdf->ws * pdf->scale);
                }
                do_pdf_draw_cell(pdf,w,h,attr,0,cp1252 + pos,sep - pos);
                cur = ++sep;
            }
            sep = 0xffffffff;
            pos = cur;
            l = 0;
            ns = 0;
            nl++;
            if(border && nl == 2)
                attr &= ~PDF_A_TOP_B;
        }
        else
        {
            cur++;
        }
    }
    if(pdf->ws > 0)
    {
        pdf->ws = 0;
        pdf_out(pdf,"0 Tw\n");
    }
    if(border & PDF_A_BOT_B)
        attr |= PDF_A_BOT_B;
    do_pdf_draw_cell(pdf,w,h,attr,0,cp1252 + pos,cur - pos);
    pdf->x = pdf->lmargin;

    free(cp1252);

    if(ws != pdf->ws)
    {
        pdf->ws = ws;
        pdf_out(pdf,"%.3f Tw",pdf->ws * pdf->scale);
    }

}

/* -------------------------------------------------- */

static void pdf_write_string(pdf_t *pdf,float w,char *str,size_t len)
{
    float y;

    if(len == 0)
        return;

    if(pdf->color_flag)
    {
        pdf_out(pdf,"q ");
        pdf_out_color(pdf,&(pdf->text_color),FALSE);
    }

    pdf->fnt_cur->used = TRUE;

    pdf_out(pdf,"BT %.2f %.2f Td (", pdf->x * pdf->scale,(pdf->height - pdf->y) * pdf->scale);
    pdf_out_string(pdf,str,len);
    pdf_out(pdf,") Tj ET\n");

    if(pdf->fnt_under)
    {
        float up,ut;

        up = pdf->fnt_cur->up;
        ut = pdf->fnt_cur->ut;

        y = (pdf->height - (pdf->y - up / 1000.0 * pdf->fnt_size)) * pdf->scale;

        pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n",pdf->x * pdf->scale,y,w * pdf->scale,(-ut / 1000.0 * pdf->fnt_pt));
    }

    if(pdf->color_flag)
        pdf_out(pdf,"Q\n");

    pdf->x += w;
}

/* -------------------------------------------------- */

static void pdf_write(pdf_t *pdf,int lnk,char *cp1252,size_t len)
{
    size_t pos,cur,sep;
    pdf_font_t *fnt = pdf->fnt_cur;
    float ws,w,max,h,f;

    h   = pdf->txt_line;

    pdf_advance_page_if_needed(pdf,h,0);

    f   = (pdf->fnt_size / 1000.0);
    max = (pdf->width - pdf->rmargin - pdf->x);

    pos = cur = 0;
    sep = 0xffff;
    w = ws = 0;

    while(cur < len)
    {
        if(cp1252[cur] == '\n')
        {
            if(lnk > 0)
                pdf_add_link_to_page(pdf,lnk,pdf->x * pdf->scale,(pdf->height - pdf->y) * pdf->scale,w * pdf->scale,(h  * pdf->scale),TRUE);
            pdf_write_string(pdf,w ,cp1252 + pos,cur - pos);
            pdf->y += h;
            pdf->x = pdf->lmargin;
            cur++;
            sep = 0xffff;
            pos = cur;
            w = 0;
            max = (pdf->width - pdf->rmargin - pdf->x);
            continue;
        }

        if(cp1252[cur] == ' ')
        {
            sep = cur;
            ws  = w;
        }
        w += fnt->cw[ (int)((unsigned char)cp1252[cur]) ] * f;
        if(w > max)
        {
            if(sep == 0xffff)
            {
                if(pdf->x > pdf->lmargin)
                {
                    pdf->y += h;
                    pdf->x = pdf->lmargin;
                    cur = pos;
                    w = 0;
                    max = (pdf->width - pdf->rmargin - pdf->x);
                    continue;
                }
                if(cur == pos)
                    cur++;
                if(lnk > 0)
                    pdf_add_link_to_page(pdf,lnk,pdf->x * pdf->scale,(pdf->height - pdf->y) * pdf->scale,w * pdf->scale,(h  * pdf->scale),TRUE);
                pdf_write_string(pdf,w,cp1252 + pos,cur - pos);
            }
            else
            {
                if(lnk > 0)
                    pdf_add_link_to_page(pdf,lnk,pdf->x * pdf->scale,(pdf->height - pdf->y) * pdf->scale,ws * pdf->scale,(h  * pdf->scale),TRUE);
                pdf_write_string(pdf,ws,cp1252 + pos,sep - pos);
                cur = sep + 1;
            }
            pdf->y += h;
            pdf->x = pdf->lmargin;
            sep = 0xffff;
            pos = cur;
            w = 0;
            max = (pdf->width - pdf->rmargin - pdf->x);
        }
        else
        {
            cur++;
        }
    }

    if(cur != pos)
    {
        if(lnk > 0)
            pdf_add_link_to_page(pdf,lnk,pdf->x * pdf->scale,(pdf->height - pdf->y) * pdf->scale,w * pdf->scale,(h  * pdf->scale),TRUE);
        pdf_write_string(pdf,w,cp1252 + pos,cur - pos);
    }
}

/* -------------------------------------------------- */

void pdf_write_text(pdf_t *pdf,int lnk,const wchar_t *fmt,...)
{
    char *cp1252 = NULL;
    size_t len = 0;
    va_list vl;

    if(!pdf || pdf->state != PDF_STATE_PAGE_READY || fmt==NULL)
        return;

    va_start(vl,fmt);
    cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,TRUE);
    va_end(vl);

    if(cp1252 == NULL || len == 0)
    {
        if(cp1252)
            free(cp1252);
        return;
    }

    pdf_write(pdf,lnk,cp1252,len);

    free(cp1252);
}

/* -------------------------------------------------- */
/* -------------------------------------------------- */

typedef struct
{
    char  *name;
    char  *value;
} html_attr_t;

typedef struct
{
    html_attr_t  attr_lst[82];
    size_t          attr_cnt;
    char           *name;
    int             closing;
} html_tag_t;

typedef struct
{
    char   *href;
    int     bold;
    int     italic;
    int     under;
} html_state_t;

/* -------------------------------------------------- */

static char *html_find_attr(html_tag_t *tag,const char *attr)
{
    int t;

    for(t=0;t<tag->attr_cnt;t++)
    {
        if(!strcasecmp(tag->attr_lst[t].name,attr))
            return tag->attr_lst[t].value;
    }
    return NULL;
}

/* -------------------------------------------------- */

static void html_open_tag(pdf_t *pdf,html_tag_t *tag,html_state_t *st)
{
    if(!strcasecmp(tag->name,"br"))
    {
        pdf->y += pdf->txt_line;
        pdf->x = pdf->lmargin;
        return;
    }

    if(!strcasecmp(tag->name,"a"))
    {
        st->href = html_find_attr(tag,"href");
        return;
    }

    if(!strcasecmp(tag->name,"b"))
        st->bold = PDF_FNT_BOLD;
    else if(!strcasecmp(tag->name,"i"))
        st->italic = PDF_FNT_ITALIC;
    else if(!strcasecmp(tag->name,"u"))
        st->under = PDF_FNT_UNDERLINE;

    pdf_set_font_style(pdf,st->bold | st->italic | st->under);
}

/* -------------------------------------------------- */

static void html_close_tag(pdf_t *pdf,html_tag_t *tag,html_state_t *st)
{
    if(!strcasecmp(tag->name,"a"))
    {
        st->href = NULL;
        return;
    }

    if(!strcasecmp(tag->name,"b"))
        st->bold = 0;
    else if(!strcasecmp(tag->name,"i"))
        st->italic = 0;
    else if(!strcasecmp(tag->name,"u"))
        st->under = 0;

    pdf_set_font_style(pdf,st->bold | st->italic | st->under);
}


/* -------------------------------------------------- */

static char **html_split(char *text,int *rcnt)
{
    char **ret,**tmp;
    int  max,cnt;

    max = cnt = 0;
    ret = NULL;
    while(*text)
    {
        if(cnt == max)
        {
            tmp = (char **)realloc(ret, sizeof(char *) * (max + 10));
            if(tmp == NULL)
            {
                pdf_free(ret);
                return NULL;
            }
            ret = tmp;
            max += 10;
        }
        ret[cnt++] = text;
        while(*text && *text!='<')
            text++;
        if(*text == 0)
            break;
       *text++ = 0;
        ret[cnt++] = text;
        while(*text && *text!='>')
            text++;
        if(*text == 0)
            break;
        *text++ = 0;
    }

    *rcnt = cnt;

    return ret;
}

/* -------------------------------------------------- */

static int html_read_tag(char *text,html_tag_t *tag)
{
    char quote = 0;

    tag->attr_cnt = 0;
    tag->closing = FALSE;

    if(*text == 0)
        return FALSE;

    if(*text == '/')
    {
        text++;
        tag->closing = TRUE;
    }

    /* Read the tag */

    tag->name = text;
    while(*text)
    {
        if(*text==' ')
        {
           *text++=0;
            break;
        }
        text++;
    }

    /* Read attributes if they exist */

    while(*text && tag->attr_cnt < 80)
    {
        while(*text==' ')
            text++;

        if(*text == 0)
            break;

        tag->attr_lst[tag->attr_cnt].name = text;
        while(*text)
        {
            if(*text == '=')
            {
                *text++=0;
                break;
            }
            text++;
        }

        strtrim(tag->attr_lst[tag->attr_cnt].name);

        while(*text==' ')
            text++;

        if(*text=='\"' || *text=='\'')
            quote = *text++;

        tag->attr_lst[tag->attr_cnt++].value = text;

        while(*text)
        {
            if(*text == quote || (!quote && *text==' '))
            {
                *text++=0;
                break;
            }
            text++;
        }
    }
    return TRUE;
}

/* -------------------------------------------------- */

void pdf_write_html(pdf_t *pdf,const wchar_t *fmt,...)
{
    char *cp1252 = NULL;
    size_t len = 0;
    va_list vl;
    html_tag_t tag = {0};
    html_state_t st = {0};
    int t,cnt;
    char **ret;

    if(!pdf || pdf->state != PDF_STATE_PAGE_READY || fmt==NULL)
        return;

    va_start(vl,fmt);
    cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,TRUE);
    va_end(vl);

    if(cp1252 == NULL || len == 0)
    {
        if(cp1252)
            free(cp1252);
        return;
    }


    cnt = 0;
    ret = html_split(cp1252,&cnt);

    for(t=0;t<cnt;t++)
    {
        if(t % 2)
        {
            if(html_read_tag(ret[t],&tag))
            {
                if(tag.closing)
                    html_close_tag(pdf,&tag,&st);
                else
                    html_open_tag(pdf,&tag,&st);
            }
        }
        else
        {
            int id = 0,under;
            rgb_t old = {0,0,0};

            if(st.href)
            {
                id = pdf_add_target_link(pdf);
                if(id > 0)
                {
                    under = st.under;
                    old = pdf->text_color;

                    pdf_set_target_link_as_uri(pdf,id,st.href);
                    pdf_set_text_color(pdf,0,0,255);
                    if(!under)
                    {
                        st.under = PDF_FNT_UNDERLINE;
                        pdf_set_font_style(pdf,st.bold | st.italic | st.under);
                    }
                }
            }

            pdf_write(pdf,id,ret[t],strlen(ret[t]));
            if(st.href)
            {
                pdf_set_text_color(pdf,old.red,old.green,old.blue);
                if(!under)
                {
                    st.under = 0;
                    pdf_set_font_style(pdf,st.bold | st.italic | st.under);
                }
            }
        }
    }

    free(cp1252);
}

/* -------------------------------------------------- */

void pdf_draw_horz_mm_ruler(pdf_t *pdf,float x,float y,float w,int n,int top)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
        float dx,dy,sy,fy,hy,my,t,step,w1,w2,hh,nn;
        int c = 0,id;
        float k = 2.834645;  /* scale to millimeters */

        if(x < 0)
            x = pdf->x;

        if(y < 0)
            y = pdf->y;

        if(n < 0)
            n = 0;

        dx = x;
        dy = (pdf->height - y);

        step = 1.0  * k;
        w1   = 0.08 * k;
        w2   = 0.15 * k;

        pdf_out(pdf,"q\n [] 0 d 0 G 0 g\n");

        if(w < 1)
            w = (pdf->width_pt / k);

        if(n > 9)
            nn  = (2.00 * k);
        else
            nn  = (1.00 * k);

        dx *= pdf->scale;
        dy *= pdf->scale;

        if(top)
        {
            sy = dy + (2.0 * k);
            my = dy + (3.0 * k);
            hy = dy + (4.0 * k);
            fy = dy + (5.0 * k);
        }
        else
        {
            sy = dy - (2.0 * k);
            my = dy - (3.0 * k);
            hy = dy - (4.0 * k);
            fy = dy - (7.0 * k);
        }

        id = pdf_add_font(pdf,"courier",PDF_FNT_REGULAR);
        if(id < 0)
            id=1;
        else
            id++;

        pdf_out(pdf,"BT /F%d 9.45 Tf ET\n",id);
        hh = my;
        for(t=0;t<=w;t++)
        {
            if(c == 0 || c == 5)
            {
                pdf_out(pdf,"%.2f w\n",w2);
                if(c == 0)
                {
                    hh = hy;
                    pdf_out(pdf,"BT %.2f %.2f Td (%d) Tj ET\n",dx - nn, fy,n);
                }
                else
                {
                   hh = my;
                }
            }
            pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",dx,dy,dx,hh);
            dx += step;
            if(++c == 10)
            {
                c = 0;
                if(++n == 10)
                    nn = 2.00 * k;
            }
            else if(c == 1 || c == 6)
            {
                pdf_out(pdf,"%.2f w\n",w1);
                hh = sy;
            }
        }
        pdf_out(pdf,"Q\n");
    }
}

/* -------------------------------------------------- */

void pdf_draw_vert_mm_ruler(pdf_t *pdf,float x,float y,float w,int n,int left)
{
    if(pdf && pdf->state == PDF_STATE_PAGE_READY)
    {
        float dx,dy,sx,fx,hx,mx,t,step,w1,w2,xx,nn;
        int c = 0,id;
        float k = 2.834645;  /* scale to millimeters */

        if(x < 0)
            x = pdf->x;

        if(y < 0)
            y = pdf->y;

        if(n < 0)
            n = 0;

        dy = (pdf->height - y) * pdf->scale;
        nn  = (1.00 * k);

        step = 1.0  * k;
        w1   = 0.08 * k;
        w2   = 0.15 * k;

        pdf_out(pdf,"q\n [] 0 d 0 G 0 g\n");

        if(w < 1)
            w = (pdf->height_pt / k);

        dx = (x * pdf->scale);

        if(left)
        {
            sx = dx - (2.0 * k);
            mx = dx - (3.0 * k);
            hx = dx - (4.0 * k);
            if(n > 9)
                fx = dx - (9.0 * k);
            else
                fx = dx - (7.0 * k);
        }
        else
        {
            sx = dx + (2.0 * k);
            mx = dx + (3.0 * k);
            hx = dx + (4.0 * k);
            fx = dx + (5.0 * k);
        }

        id = pdf_add_font(pdf,"courier",PDF_FNT_REGULAR);
        if(id < 0)
            id=1;
        else
            id++;

        pdf_out(pdf,"BT /F%d 9.45 Tf ET\n",id);
        xx = mx;
        for(t=0;t<=w;t++)
        {
            if(c == 0 || c == 5)
            {
                pdf_out(pdf,"%.2f w\n",w2);
                if(c == 0)
                {
                    xx = hx;
                    pdf_out(pdf,"BT %.2f %.2f Td (%d) Tj ET\n",fx, dy - nn ,n);
                }
                else
                {
                   xx = mx;
                }
            }
            pdf_out(pdf,"%.2f %.2f m %.2f %.2f l S\n",dx,dy,xx,dy);
            dy -= step;
            if(++c == 10)
            {
                c = 0;
                if(++n == 10 && left)
                    fx -= 2.00 * k;
            }
            else if(c == 1 || c == 6)
            {
                pdf_out(pdf,"%.2f w\n",w1);
                xx = sx;
            }
        }
        pdf_out(pdf,"Q\n");
    }
}


/* -------------------------------------------------- */

int pdf_draw_barcode(pdf_t *pdf,float x, float y, float w,int lnk,int bkg,const wchar_t *fmt,...)
{
    unsigned char resp[EAN_13_BYTES],extra[EAN_5_BYTES];
    int t,bit=0,bytes,ret,ebytes,ebit=0,gap;
    char *cp1252 = NULL;
    const char *top = NULL;
    size_t len = 0, elen = 0;
    va_list vl;

    if(!pdf || pdf->state != PDF_STATE_PAGE_READY || fmt==NULL)
        return PDF_EINVAL;

    va_start(vl,fmt);
    cp1252 = pdf_wchar_to_cp1252(&len,fmt,vl,TRUE);
    va_end(vl);

    ret = PDF_ENODATA;

    if(cp1252 && len > 0)
    {
        float hh,h,k,f,ex=0,ey=0;
        int et=FALSE;

        if (x == -1)
            x = pdf->x;

        if (y == -1)
            y = pdf->y;

        ret = PDF_ENOIMPL;
        switch(len)
        {
            case 16:
                len  = 13;
                elen = 2;
                ret = get_ean_13(resp,EAN_13_BYTES,cp1252);
                if(ret == PDF_OK)
                {
                    if(cp1252[13]=='+')
                        et = TRUE;
                    else if(cp1252[13]!='-')
                    {
                        free(cp1252);
                        return -3;
                    }
                    ret = get_ean_2(extra,EAN_2_BYTES,cp1252 + 14);
                    if(ret == PDF_OK)
                    {
                        bytes  = EAN_13_BYTES;
                        bit    = 95;
                        gap    = 8;
                        ebytes = EAN_2_BYTES;
                        ebit   = 21;
                        if(w == 0)
                        {
                            w = (33.25 + 2.8 + 7.35) / (95+8+21);
                            h  = 14;
                        }
                        else
                        {
                            w = w / (95 + 8 + 21);
                            h = (13 * w ) / ((33.25 + 2.8 + 7.35) / (95 + 8 + 21));
                        }
                        x += w * 9;
                    }
                }
                break;
            case 19:
                len  = 13;
                elen = 5;
                ret = get_ean_13(resp,EAN_13_BYTES,cp1252);
                if(ret == PDF_OK)
                {
                    if(cp1252[13]=='+')
                        et = TRUE;
                    else if(cp1252[13]!='-')
                    {
                        free(cp1252);
                        return PDF_EINCORRECT;
                    }
                    ret = get_ean_5(extra,EAN_5_BYTES,cp1252 + 14);
                    if(ret == PDF_OK)
                    {
                        bytes  = EAN_13_BYTES;
                        bit    = 95;
                        gap    = 8;
                        ebytes = EAN_5_BYTES;
                        ebit   = 48;
                        if(w == 0)
                        {
                            w = (33.25 + 2.8 + 16.80) / (95+8+48);
                            h  = 14;
                        }
                        else
                        {
                            w = w / (95 + 8 + 48);
                            h = (13 * w ) / ((33.25 + 2.8 + 16.80) / (95 + 8 + 48));
                        }
                        x += w * 9;
                    }
                }
                break;
            case 12:
                top = "0";
                len++;
            case 13:
                bytes = EAN_13_BYTES;
                bit   = 95;
                if(w == 0)
                {
                    w = 33.25 / 95;
                    h  = 14;
                }
                else
                {
                    w = w / 95;
                    h = (13 * w ) / (33.25 / 95);
                }
                x += w * 9;
                ret = get_ean_13(resp,EAN_13_BYTES,cp1252);
                break;
            case 8:
                bytes = EAN_8_BYTES;
                bit   = 67;
                if(w == 0)
                {
                    w = 23.45 / 67;
                    h  = 14;
                }
                else
                {
                    w = w / 67;
                    h = (13 * w ) / (23.45 / 67);
                }
                ret = get_ean_8(resp,EAN_8_BYTES,cp1252);
                break;
            case 5:
                bytes = EAN_5_BYTES;
                bit   = 48;
                if(w == 0)
                {
                    w = 16.80 / 48;
                    h  = 14;
                }
                else
                {
                    w = w / 48;
                    h = (13 * w ) / (16.80 / 48);
                }
                ret = get_ean_5(resp,EAN_5_BYTES,cp1252);
                break;
            case 2:
                bytes = EAN_2_BYTES;
                bit   = 21;
                if(w == 0)
                {
                    w = 7.35 / 21;
                    h  = 14;
                }
                else
                {
                    w = w / 21;
                    h = (13 * w ) / (7.35 / 21);
                }
                ret = get_ean_2(resp,EAN_2_BYTES,cp1252);
                break;
        }

        k = pdf->scale;

        if(ret == PDF_OK)
        {
            pdf_font_t  *fnt;
            int id;

            pdf_out(pdf,"\nq\n 1 g ");

            id = pdf_add_font(pdf,"courier",PDF_FNT_REGULAR);
            if(id > 0)
                fnt = pdf->fnt_lst[id];
            else
                fnt = pdf->fnt_lst[0];  /* Helvetica Regular is always ID 1 */

            fnt->used = TRUE;       /* make sure is delivered on output */

            if(len == 5 || len== 2)
            {
                f = pdf_font_size_for_width(pdf,fnt,w * (7 * len),cp1252,len);
                if(bkg)
                    pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n ",(x-w*3)*k,(pdf->height-(y-(f/k)))*k, w*(bit+6)*k ,-(h+(f/k)+w*6)*k);

                if(lnk > 0)
                    pdf_add_link_to_page(pdf,lnk,(x-w*3)*k,(pdf->height-(y-(f/k)))*k, w*(bit+6)*k ,(h+(f/k)+w*6)*k,FALSE);

                pdf_out(pdf,"0 g BT /F%d %.2f Tf ET \n",fnt->id,f);
                pdf_out(pdf,"BT %.2f %.2f Td (",(x + w * 6.5) * k,(pdf->height - y) * k);
                pdf_out_string(pdf,cp1252,len);
                pdf_out(pdf,") Tj ET\n");
                y += (w * 2);
                for(t=0; bit > 0; t++)
                {
                    if(mem_get_bit(resp,bytes,--bit))
                        do_pdf_draw_rectangle(pdf,x + (t * w), y , w, h,PDF_A_FILLED);
                }
            }
            else
            {
                if(len == 13)
                {
                    f = pdf_font_size_for_width(pdf,fnt,w * 40,cp1252 + (( top ) ? 0 : 1),6);

                    if(bkg)
                    {
                        if(elen == 0)
                            pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n ",(x-w*12)*k,(pdf->height-(y - w * 3))*k, w*111*k ,-(h+(f/k)+w*6)*k);
                        else
                            pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n ",(x-w*12)*k,(pdf->height-(y - w * 3))*k, w*(111+(gap+ebit))*k ,-(h+(f/k)+w*6)*k);
                    }
                    if(lnk > 0)
                    {
                        if(elen == 0)
                            pdf_add_link_to_page(pdf,lnk,(x-w*12)*k,(pdf->height-(y - w * 3))*k, w*111*k ,(h+(f/k)+w*6)*k,FALSE);
                        else
                            pdf_add_link_to_page(pdf,lnk,(x-w*12)*k,(pdf->height-(y - w * 3))*k, w*(111+(gap+ebit))*k ,(h+(f/k)+w*6)*k,FALSE);
                    }

                }
                else
                {
                    f = pdf_font_size_for_width(pdf,fnt,w * 28,cp1252,4);

                    if(bkg)
                        pdf_out(pdf,"%.2f %.2f %.2f %.2f re f\n ",(x-w*3)*k,(pdf->height-(y - w * 3))*k, w*73*k ,-(h+(f/k)+w*6)*k);

                    if(lnk > 0)
                        pdf_add_link_to_page(pdf,lnk,(x-w*3)*k,(pdf->height-(y - w * 3))*k, w*73*k ,(h+(f/k)+w*6)*k,FALSE);

                }

                pdf_out(pdf,"0 g BT /F%d %.2f Tf ET \n",fnt->id,f);


                for(t=0; bit > 0; t++)
                {
                    if(len == 13 && (bit==95 || bit==93 || bit==49 || bit==47 || bit==3|| bit==1))
                        hh = ((f / k) / 2);
                    else if(len == 8 && (bit==67 || bit==65 || bit==35 || bit==33 || bit==3 || bit==1))
                        hh = ((f / k) / 2);
                    else
                        hh = 0;

                    if(mem_get_bit(resp,bytes,--bit))
                        do_pdf_draw_rectangle(pdf,x + (t * w), y , w, h + hh,PDF_A_FILLED);
                }

                if(len == 8)
                {
                    y += (h + (f / k) - w);
                    x += (w * 4.5);
                    pdf_out(pdf,"BT %.2f %.2f Td (",x * k,(pdf->height - y) * k);
                    pdf_out_string(pdf,cp1252,4);
                    pdf_out(pdf,") Tj ET\n");
                    x += (w * 31.5);
                    pdf_out(pdf,"BT %.2f %.2f Td (",x * k,(pdf->height - y) * k);
                    pdf_out_string(pdf,cp1252 + 4,4);
                    pdf_out(pdf,") Tj ET\n");

                }
                else
                {
                    if(elen)
                    {
                        ex = x + ((95 + gap) * w);
                        ey = y;
                    }

                    if(et)
                    {
                        ey += ((f / k) / 3) * 2;
                        pdf_out(pdf,"0 g BT /F%d %.2f f Tf ET \n",fnt->id,f);
                        pdf_out(pdf,"BT %.2f %.2f Td (",(ex + w * 6.5) * k,(pdf->height - ey) * k);
                        pdf_out_string(pdf,cp1252 + 14,elen);
                        pdf_out(pdf,") Tj ET\n");
                        ey += (w * 2);
                    }

                    if(elen)
                    {
                        for(t=0; ebit > 0; t++)
                        {
                            if(mem_get_bit(extra,ebytes,--ebit))
                                do_pdf_draw_rectangle(pdf,ex + (t * w), ey , w, h,PDF_A_FILLED);
                        }
                        if(et==FALSE)
                            ey += (h + (f / k) - w);
                    }

                    y += (h + (f / k) - w);

                    pdf_out(pdf,"BT %.2f %.2f Td (",(x - w * 9) * k,(pdf->height - y) * k);
                    pdf_out_string(pdf,(top) ? top : cp1252,1);
                    pdf_out(pdf,") Tj ET\n");

                    x += (w * 5);

                    pdf_out(pdf,"BT %.2f %.2f Td (",x * k,(pdf->height - y) * k);
                    pdf_out_string(pdf,cp1252 + (( top ) ? 0 : 1),6);
                    pdf_out(pdf,") Tj ET\n");

                    x += (w * 45);

                    pdf_out(pdf,"BT %.2f %.2f Td (",x * k,(pdf->height - y) * k);
                    pdf_out_string(pdf,cp1252 + 6 + (( top ) ? 0 : 1),6);
                    pdf_out(pdf,") Tj ET\n");

                    if(elen && !et)
                    {
                        pdf_out(pdf,"0 g BT /F%d %.2f Tf ET \n",fnt->id,f);
                        pdf_out(pdf,"BT %.2f %.2f Td (",(ex + w * 6.5) * k,(pdf->height - ey) * k);
                        pdf_out_string(pdf,cp1252 + 14,elen);
                        pdf_out(pdf,") Tj ET\n");
                    }
                }
            }

            pdf_out(pdf,"Q\n");
        }

        free(cp1252);
    }
    return ret;
}
