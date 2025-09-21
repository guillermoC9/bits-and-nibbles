/*
    json.c

    JSON (JavaScript Object Notation) support

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

  		- Originally written by Guillermo Amodeo Ojeda

        - JSON was invented by Douglas Crockford as a way to give
          persistence to JavaScript objects, although it has evolved
          to become a de-facto standard for data interchange, as it
          is easy for humans to read and for software to parse.
          Its also much more space efficient than XML and similars,
          and as good as them keeping data hierarchy.

          JSON is also very small, you can find its complete definition
          at http://www.json.org/json-en.html, which is the guide Guillermo
          followed to write the code here.

        - Guillermo also made sure the format managed by this code is
          compliant with the guidelines given in RFC-4627, the guide that
          defines JSON for its use in JavScript.

                  http://www.ietf.org/rfc/rfc4627.txt
                  

        - IMPORTANT:

            The most imortant thing to understand with JSON is that 
            despite the multiple ways you can reference elements in 
            JavaScript, ALL elements are really objects, and they 
            are grouped in arrays. The difference is that in a 
            JavaScript object, the contained elements are named, 
            whereas in an array, the elements are unnamed.

            That is why in JavaScript you can reference the member 
            'val' from an object 'obj' like: 
            
                obj.val 
                obj["val"].

            In our paths here we do not enclose the named indexes in
            quotes because there are no vars, so here you do should do
            this for the above example: 
                
                obj[val]

            Although I would not use named indexes unless you really 
            want to pretend that obj is an array of named objects, 
            which I personally find confusing and ambiguous, and we 
            all know that ambiguity causes risk in programming.
            It is also important to understand that when saving and 
            loading JSON objects, the only element that preserve its 
            order are arrays (JSON_ARRAY), the object members can be 
            in any order, so you should not rely on their cardinal
            position.     

            Note that in the code we sometimes call the elements 
            'objects' as well as elements. They are just that as we 
            said above.

            Arrays and Objects are just groups of objects that can
            contain any type of object.
            
            It is important to understand WHY the value NULL is 
            VERY IMPORTANT in JSON:

            By setting a member to NULL, you acknolwledge that even 
            if it has no value, the element MUST exists. This will 
            allow the element to be loaded and saved, as well as 
            found when referenced. This is very important in many 
            uses like documental and NoSQL databases, as well as 
            for code integrity, where a member could be referenced 
            by some code, which will fail if the actual member was 
            not preserved for being NULL.


        - All functions use wide char (wchar_t) to avoid language
          problems, as JSON spec states it should be stored using 
          UTF-8 enconding, and it seemed natural to translate it
          to wchar_t  so handing them would be easier than handling
          UTF-8.

                                --oO0Oo--
*/

#include "json.h"
#include <stdarg.h>

/* ------------------------------- *
        Utility functions
 * ------------------------------- */

static int json_puts(const char *s,FILE *fp)
{
    while(*s)
    {
        if(putc_unlocked(*s++,fp)==EOF)
            return EOF;
    }
    return 0;
}

/* ------------------------------- */

static int json_putw(wchar_t *wcad,int quotes,int escape,FILE *fp)
{
    unsigned int t,num;
    unsigned char ch[16];
	unsigned int val;
	int cnt;

	num=0;

    if(quotes)
        ch[num++]='\"';

    do
    {
		cnt = wchar_to_ucs4(&val,wcad,2);
		if(cnt == 0)
		{
			if(quotes)
	            ch[num++]='\"';
		}
		else
		{
			wcad+=cnt;
			if(escape)
			{
				if(val > 0xffff)
				{

					/*
					    Secton 2.5 of RFC-4627 says that JSON uses UTF-16
					    to represent unicode chars over 0xffff, so we
					    generate surrogate codes for anything over that
					    code point (e.g. modern emojis)
	    	        */

					unsigned short tmp;

					val -= 0x10000;

					tmp = (unsigned short)(0xd800 | ((val >> 10) & 0x3ff));

					ch[num++]='\\';
					ch[num++]='u';
					ch[num++]=int_to_hex( (tmp >> 12) & 0x0f );
					ch[num++]=int_to_hex( (tmp >> 8) & 0x0f );
					ch[num++]=int_to_hex( (tmp >> 4) & 0x0f );
					ch[num++]=int_to_hex( tmp & 0x0f );

					tmp = (unsigned short)(0xdc00 | (val & 0x3ff));

					ch[num++]='\\';
					ch[num++]='u';
					ch[num++]=int_to_hex( (tmp >> 12) & 0x0f );
					ch[num++]=int_to_hex( (tmp >> 8) & 0x0f );
					ch[num++]=int_to_hex( (tmp >> 4) & 0x0f );
					ch[num++]=int_to_hex( tmp & 0x0f );

					val+= 0x10000;

				}
				else if(val > 0x80)
				{
				    /* Unicode 0x80 - 0xffff */
					ch[num++]='\\';
					ch[num++]='u';
					ch[num++]=int_to_hex( (val >> 12) & 0x0f );
					ch[num++]=int_to_hex( (val >> 8) & 0x0f );
					ch[num++]=int_to_hex( (val >> 4) & 0x0f );
					ch[num++]=int_to_hex( val & 0x0f );
				}
				else
				{
					switch(val)
					{
						case L'\"':
							ch[num++]='\\';
							ch[num++]='"';
							break;
						case L'\\':
							ch[num++]='\\';
							ch[num++]='\\';
							break;
						case L'/' :
							ch[num++]='\\';
							ch[num++]='/';
							break;
						case L'\b' :
							ch[num++]='\\';
							ch[num++]='b';
							break;
						case L'\f' :
							ch[num++]='\\';
							ch[num++]='f';
							break;
						case L'\n' :
							ch[num++]='\\';
							ch[num++]='n';
							break;
						case L'\r' :
							ch[num++]='\\';
							ch[num++]='r';
							break;
						case L'\t' :
							ch[num++]='\\';
							ch[num++]='t';
							break;
						default:
							ch[num++]=(unsigned char)val;
							break;
					}
				}
            }
			else if(val < 0x80)
			{
				ch[num++]=(unsigned char)val;
			}
			else if(val < 0x800)
			{
				ch[num++]=(unsigned char)(0xc0 | (val >> 6));
				ch[num++]=(unsigned char)(0x80 | (val & 0x3f));
			}
			else if(val < 0x10000)
			{
				ch[num++]=(unsigned char)(0xf0 | ( val >> 12));
				ch[num++]=(unsigned char)(0x80 | ((val >> 6) & 0x3f));
				ch[num++]=(unsigned char)(0x80 | ( val & 0x3f));
			}
			else
			{
				ch[num++]=(unsigned char)(0xf0 | ( val >> 18));
				ch[num++]=(unsigned char)(0x80 | ((val >> 12) & 0x3f));
				ch[num++]=(unsigned char)(0x80 | ((val >> 6) & 0x3f));
				ch[num++]=(unsigned char)(0x80 | ( val & 0x3f));
			}
        }
        for(t=0;t<num;t++)
        {
            if(putc_unlocked(ch[t],fp)==EOF)
                return EOF;
        }
        num=0;
    } while(val);
    return 0;
}

 /* ------------------------------- */

static void write_spaces(FILE *fp,int level)
{
    while(level > 0)
    {
   	    json_puts("   ",fp);
   	    level--;
   	 }
}

/* ------------------------------- */

static void write_object(FILE *fp,json_obj_t *obj,int level,int keep_going,int in_array)
{
    if (!obj)
        return;

    write_spaces(fp,level);

    /* 
        Show name only if not in an array (where names doesn't count),
        its not an object, or if it is the name is not empty. 
    */

    if(!in_array && (obj->type != JSON_OBJECT || obj->name[0]))
    {
    	json_putw(obj->name,TRUE,TRUE,fp);
    	json_puts(" : ",fp);
    }

    switch(obj->type)
    {
        case JSON_OBJECT:
            if(!in_array)
            {
            	json_puts("\n",fp);
                write_spaces(fp,level);
            }
          	json_puts("{\n",fp);
            write_object(fp,(json_obj_t *)obj->value,level + 1,TRUE,FALSE);
		    write_spaces(fp,level);
           	json_puts("}",fp);
            break;
        case JSON_STRING:
            json_putw((wchar_t *)obj->value,TRUE,TRUE,fp);
            break;
        case JSON_NUMBER:
        case JSON_BOOLEAN:
            json_putw((wchar_t *)obj->value,FALSE,FALSE,fp);
            break;
        case JSON_NULL:
           	json_puts("null",fp);
            break;
        case JSON_ARRAY:
            if(!in_array)
            {
                json_puts("\n",fp);
                write_spaces(fp,level);
            }
            json_puts("[\n",fp);
            write_object(fp,(json_obj_t *)obj->value,level + 1,TRUE,TRUE);
            write_spaces(fp,level);
            json_puts("]",fp);
            break;
        default:
            fprintf(fp,"\"Unknown object type %d \"\n",obj->type);
            break;
    }
    json_puts((obj->next && keep_going) ? ",\n" : "\n",fp);
    if(keep_going)
        write_object(fp,obj->next,level,TRUE,in_array);
}

/* ----------------------------------------- *
   Chars not allowed in an object name
   (aka key names).

   Note that we extend the invalid chars
   given by the specs to include chars 
   that are part of JSON syntax. 
   
   Not because is invalid JSON, but 
   because they may cause problems when
   handling paths. 

   Note that we allow other special chars: 
    
    # @ % $ £ + | * ~ / ^ ± § < > ? ! ( ) 

   and even spaces.
* ----------------------------------------- */

 #define NOT_ALLOWED_CHARS L".[]{},;:'\\\"=\b\r\t\f\n"

/* --------------------------------- *
   Creates an object making sure
   the object's name is OK
   
   New object is always a JSON_NULL
 * --------------------------------- */

static int json_create_object(const wchar_t *name,json_obj_t **ret)
{
    json_obj_t *obj;
    size_t len=0;

    if(name)
    {                
        if(wcspbrk(name,NOT_ALLOWED_CHARS))
            return JSON_EINCORRECT;

        len=(wcslen(name) * sizeof(wchar_t));
    }    

    obj=(json_obj_t *)calloc(sizeof(json_obj_t) + len,1);
	if(obj)
	{
	    obj->type = JSON_NULL;
        if(name)
        	wcscpy(obj->name,name);        
       *ret = obj;
        return JSON_OK; 
    }
   	return JSON_ENOMEM;
}

/* ------------------------------- *
    Find an object named 'name'
    among the children of an 
    object
 * ------------------------------- */

static json_obj_t *json_find_named_child(json_obj_t *obj,const wchar_t *name)
{
    json_obj_t *cual=(json_obj_t *)obj->value;
    
    while(cual)
    {       
        if(!wcscmp(cual->name,name))
            return cual;
        cual=cual->next;
    }
    return NULL;
}
           
/* ------------------------------- *
    Attach 'child' at the end
    of the 'parent' children list

    if JSON_NULL, then parent is
    converted to JSON_OBJECT.

    Otherwise it will fail
 * ------------------------------- */

int json_attach_object(json_obj_t *parent,json_obj_t *child)
{
    switch(parent->type)
    {
        case JSON_NULL:
            parent->type=JSON_OBJECT;
            parent->value=child;
            parent->len = 1;
            return JSON_OK;
        case JSON_OBJECT:                                                    
        case JSON_ARRAY:                      
            if(parent->value)
            {
                json_obj_t *pre;
                
                pre = (json_obj_t *)parent->value;
                while(pre->next)
                    pre = pre->next;
                pre->next = child;
                pre->len++;
            }
            else 
            {
                parent->value=child;
                parent->len = 1;
            }
            return JSON_OK;
        default:
            break;
    }    
    return JSON_EINCORRECT;
}


/* ------------------------------- */

static int json_detach_object(json_obj_t *parent,json_obj_t *child)
{
    json_obj_t *pre, *obj;
    
    pre = NULL;
    obj = (json_obj_t *)parent->value;

    while(obj)
    {        
        if(obj==child)
        {
            if(pre)                    
                pre->next=obj->next;
            else
                parent->value=obj->next;
            parent->len--;
            child->next = NULL;
            return TRUE;
        }
        if(obj->value && (obj->type == JSON_OBJECT || obj->type == JSON_ARRAY))
        {
            if(json_detach_object(obj,child))
                return TRUE;
        }
        pre=obj;
        obj=obj->next;
    }    
    return FALSE;
}

/* ------------------------------- */

static void json_delete_object_list(json_obj_t *obj)
{
    json_obj_t *del;

    while(obj)
    {                
        del=obj;
        obj=obj->next;     
        
        if(del->value)
        {
            if(del->type==JSON_OBJECT || del->type==JSON_ARRAY)
            {
    	        json_delete_object_list((json_obj_t *)del->value);                
            }
            else
            {
                memset(del->value,0,sizeof(wchar_t) * del->len);
                free(del->value);
            }
        }
        memset(del,0,sizeof(json_obj_t));
	    free(del);
    }
}

/* ------------------------------- *
        JSON PARSER STUFF
 * ------------------------------- */

typedef struct
{
    FILE *fp;
    int   ch;           /** Last read char */
    unsigned char  mask[7];

	const char *doc;    /* document */
    size_t      len;    /* size in bytes */
	size_t      cur;    /* Current position being parsed */

    wchar_t    *buf;    /* Buffer */
    size_t      tam;    /* Size of data in the buffer */
    size_t      max;    /* Size of 'buf' */

	json_pos_t 	pos;	/* current position */

	int         err;    /* Last error */
} tmp_json_t;


#define TMP_JSON_INIT	{NULL,0,{0x00,0x7f,0x1f,0x0f,0x07,0x03,0x01},NULL,0,0,NULL,0,0,{0,0},-1}

/* ------------------------------- */

static char json_next_char(tmp_json_t *json)
{
    if(json->fp)
    {
        if(json->ch != EOF)
            return (char)json->ch;
    }
    else
    {
        if(json->cur < json->len)
    	    return json->doc[json->cur];
    }
    return 0;
}

/* ------------------------------- */

static void json_jump_char(tmp_json_t *json)
{
    if(json->fp)
    {
        json->ch=getc_unlocked(json->fp);
        json->pos.col++;
    }
    else
    {
        if(json->cur < json->len)
        {
	        json->pos.col++;
        	json->cur++;
        }
    }
}

/* ------------------------------- */

static char json_jump_blanks(tmp_json_t *json)
{
	char ch;
	int cr = 0;

    do {
    	ch=json_next_char(json);
    	if(!isspace(ch))
    	{
    		if(cr)
    		{
    			json->pos.row++;
				json->pos.col=0;
			}
    		return ch;
    	}
    	else if(ch == '\r')
    	{
    		if(cr)
    		{
    			json->pos.row++;
				json->pos.col=0;
    		}
    		else
	    		cr=1;
    	}
    	else if(ch == '\n' || cr)
    	{
    		cr = 0;
    		json->pos.row++;
    		json->pos.col=0;
    	}
        json_jump_char(json);
    } while(ch);
    return 0;
}

/* ------------------------------- */

static int json_put_wchar(tmp_json_t *json,wchar_t ch)
{
    if(json->tam == json->max)
    {
        wchar_t *tmp;
        size_t tam;

        tam=(json->tam + 512) * sizeof(wchar_t);
        tmp=(wchar_t *)realloc(json->buf,tam);
        if(!tmp)
            return FALSE;
        json->buf=tmp;
        json->max+=511;
    }

    json->buf[json->tam++]=ch;
    json->buf[json->tam]=0;

  	return TRUE;
}

/* ------------------------------- */

static wchar_t json_get_wchar(tmp_json_t *json)
{
    unsigned char ch;
	int num,i;
	unsigned int ucs4=0;

    ch=(unsigned char)json_next_char(json);
    if(ch=='\\')
    {
        json_jump_char(json);
        ch=(unsigned char)json_next_char(json);
        switch(ch)
       	{
        	case '\"':
           	    json_jump_char(json);
        	    return L'\"';
	        case '\\':
           	    json_jump_char(json);
        	    return L'\\';
            case '/' :
           	    json_jump_char(json);
        	    return L'/';
    		case 'b' :
           	    json_jump_char(json);
        	    return L'\b';
    		case 'f' :
           	    json_jump_char(json);
        	    return L'\f';
    		case 'n' :
           	    json_jump_char(json);
        	    return L'\n';
    		case 'r' :
           	    json_jump_char(json);
        	    return L'\r';
    		case 't' :
           	    json_jump_char(json);
        	    return L'\t';
    		case 'u' :
    		    i=0;
    		    do
    		    {
            	    json_jump_char(json);
                    ch=(unsigned char)json_next_char(json);
                    if(!isxdigit(ch))
                        return 0;
    				ucs4<<=4;
    			    ucs4|=whex_to_int((wchar_t)ch);
   				} while(++i < 4);
           	    json_jump_char(json);
                if(!ucs4)
                {
                    /*
                        I discovered that many software stacks and
                        databases based on JavaScript (e.g. Angular
                        and Firebase Database) allow JSON strings
                        with embedded nulls (\u0000) because they
                        just ignore  the nulls. Not sure who to blame
                        for this nosense allowance (the string producer
                        I guess). It seems now that is common practice 
                        to allow spurious NULL chars inside strings. 
                        It is so widespread that even SQLite allows it.
                        Although they warn people about this dangerous 
                        practice:

                             https://sqlite.org/nulinstr.html

                        After many JSON documents coming from those products
                        failed to load in this engine due to spuriois \u0000
                        in strings, I decided just to ignore them too, but
                        decided to also convert them to the character '0'
                        in order to make the caller aware of the presence
                        of the spurious NULL, actually hoping it will be 
                        noticed and corrected. 
                        
                        I also hope that I won't have to regret my decision.
                    */
                    ucs4='0';
                }
   				return (wchar_t)ucs4;
   			case 0:
	        default:
       	        return 0;
    	}
    }

	/* How many chars to read? */

	if(ch < 192)
		num=1;
	else if(ch < 224)
		num=2;
	else if(ch < 240)
		num=3;
	else if(ch < 248)
		num=4;
	else
	{
	    json->err=JSON_ECORRUPT; /* Not an UTF-8 file */
        return 0;
	}

	/* Read them */

	ucs4=(unsigned int)(ch & json->mask[num]);

	for(i=1;i<num;i++)
	{
		json_jump_char(json);
		ch=(unsigned char)json_next_char(json);
		if(!ch)
		{
    	    json->err=JSON_ECORRUPT; /* Not an UTF-8 file */
			return 0;
		}
		ucs4<<=6;
		ucs4|=(ch & 0x3f);
	}

	/* Correct incorrect chars and overlong sequences */

	if(ucs4 < 0x80)
	{
		if(num > 1)
			ucs4 = WCHAR_REPLACEMENT;
	}
	else if(ucs4 < 0x800)
	{
		if(num > 2)
			ucs4 = WCHAR_REPLACEMENT;
	}
	else if(ucs4 < 0x10000)
	{
		if(num > 3 || (ucs4 > 0xd7ff && ucs4 < 0xe000))
			ucs4 = WCHAR_REPLACEMENT;
	}
	else if (ucs4 > 0x10ffff)
	{
		ucs4 = WCHAR_REPLACEMENT;
	}

    json_jump_char(json);
    return (wchar_t)ucs4;
}


/* ------------------------------- */

static int json_read_string(tmp_json_t *json)
{
    wchar_t wch;
    char ch;

    json->tam=0;

    if(json_next_char(json)!='\"')
        return JSON_ESYNTAX;

    json_jump_char(json);

    ch=json_next_char(json);

    while(ch)
    {
        switch(ch)
        {
    		case '\b':
    		case '\f':
    		case '\t':
            case '\r':
            case '\n':
            case '\0':
                return JSON_ESYNTAX;
            case '\"':
                json_jump_char(json);
                if(!json_put_wchar(json,0))
                    return JSON_ENOMEM;
                return JSON_OK;
    	    default:
    	        wch=json_get_wchar(json);
    	        if(!wch)
    	            return JSON_ESYNTAX;
#ifndef WCHAR_UTF_16
                /* 
                    JSON uses UTF-16 (Sección 2.5 of RFC-4627).

                    So in systems that do not use UTF-16 we convert
                    surrogates onto UTF-32 or the replacement char
                    if wrong.
                */
                if(wch > 0xc7ff && wch < 0xe000)
                {
                    if(wch < 0xdc00)
                    {
                        wchar_t tmp = json_get_wchar(json);

                        if(tmp > 0xdbff && tmp < 0xe000)
                        {
                            wch -= 0xd800;
                            wch <<= 10;
                            wch |= (tmp - 0xdc00);
                        }
                        else
                        {
                            if(!json_put_wchar(json,WCHAR_REPLACEMENT))
                                return JSON_ENOMEM;
                            wch = tmp;
                        }
                    }
                    else
                    {
                        wch=WCHAR_REPLACEMENT;
                    }
                }
#endif
                if(!json_put_wchar(json,wch))
                    return JSON_ENOMEM;
          		break;
        }
        ch=json_next_char(json);
    }
	return JSON_ESYNTAX;
}

/* ------------------------------- */

static int json_read_number(tmp_json_t *json)
{
    char ch;
    int val;
    wchar_t *num=L"0123456789";

    json->tam=0;

    /* Sign */

    ch=json_next_char(json);
    if(ch=='-' || ch=='+')
    {
        if(ch=='-')
        {
            if(!json_put_wchar(json,L'-'))
                return JSON_ENOMEM;            
        }
        json_jump_char(json);
        ch=json_next_char(json);
    }

    if(!isdigit(ch))
        return JSON_ESYNTAX;

    /* Integer part (characteristic) */

    do {
        val=(ch - '0');

        if(!json_put_wchar(json,num[val]))
            return JSON_ENOMEM;

        json_jump_char(json);
        ch=json_next_char(json);

    } while(isdigit(ch));

    /* Fractional part (mantissa) */

    if(ch=='.')
    {
        if(!json_put_wchar(json,L'.'))
            return JSON_ENOMEM;
        json_jump_char(json);
        ch=json_next_char(json);
        while(isdigit(ch))
        {
            val=(ch - '0');

            if(!json_put_wchar(json,num[val]))            
                return JSON_ENOMEM;

            json_jump_char(json);
            ch=json_next_char(json);
        }
    }

    /* Exponent */

    if(ch=='e' || ch=='E')
    {
        if(!json_put_wchar(json,L'e'))        
            return JSON_ENOMEM;
        
        json_jump_char(json);
        ch=json_next_char(json);
        if(ch=='-')
        {
            if(!json_put_wchar(json,L'-'))            
                return JSON_ENOMEM;
            
            json_jump_char(json);
            ch=json_next_char(json);
        }
        else if(ch=='+')
        {
            if(!json_put_wchar(json,L'+'))
                return JSON_ENOMEM;
            
            json_jump_char(json);
            ch=json_next_char(json);
        }
        else if(!isdigit(ch))
            return JSON_ESYNTAX;

        do {
            val=(ch - '0');
            if(!json_put_wchar(json,num[val]))            
                return JSON_ENOMEM;
            
            json_jump_char(json);
            ch=json_next_char(json);
        } while(isdigit(ch));
    }

    if(!json_put_wchar(json,0))    
        return JSON_ENOMEM;
    
    return JSON_OK;
}

/* ------------------------------- */

static int json_read_true(tmp_json_t *json)
{
    char ch;

    json->tam=0;

    ch=json_next_char(json);
    if(ch=='t')
    {
        if(!json_put_wchar(json,L't'))
            return JSON_ENOMEM;
        
        json_jump_char(json);
        ch=json_next_char(json);
        if(ch=='r')
        {
            if(!json_put_wchar(json,L'r'))
                return JSON_ENOMEM;
            
            json_jump_char(json);
            ch=json_next_char(json);
            if(ch=='u')
            {
                if(!json_put_wchar(json,L'u'))
                    return JSON_ENOMEM;

                json_jump_char(json);
                ch=json_next_char(json);
                if(ch=='e')
                {
                    if(!json_put_wchar(json,L'e'))
                        return JSON_ENOMEM;
                    
                    json_jump_char(json);
                    ch=json_next_char(json);
                    if(isspace(ch) || strchr(",}]",ch))
                    {
                        if(!json_put_wchar(json,0))
                            return JSON_ENOMEM;
                        
                        return JSON_OK;
                    }
                }
            }
        }
    }
    return JSON_ESYNTAX;
}

/* ------------------------------- */

static int json_read_false(tmp_json_t *json)
{
    char ch;

    json->tam=0;

    ch=json_next_char(json);
    if(ch=='f')
    {
        if(!json_put_wchar(json,L'f'))
            return JSON_ENOMEM;
        
        json_jump_char(json);
        ch=json_next_char(json);
        if(ch=='a')
        {
            if(!json_put_wchar(json,L'a'))
                return JSON_ENOMEM;
            
            json_jump_char(json);
            ch=json_next_char(json);
            if(ch=='l')
            {
                if(!json_put_wchar(json,L'l'))
                    return JSON_ENOMEM;
                
                json_jump_char(json);
                ch=json_next_char(json);
                if(ch=='s')
                {
                    if(!json_put_wchar(json,L's'))
                        return JSON_ENOMEM;
                
                    json_jump_char(json);
                    ch=json_next_char(json);
                    if(ch=='e')
                    {
                        if(!json_put_wchar(json,L'e'))
                            return JSON_ENOMEM;
                        
                        json_jump_char(json);
                        ch=json_next_char(json);
                        if(isspace(ch) || strchr(",}]",ch))
                        {
                            if(!json_put_wchar(json,0))
                                return JSON_ENOMEM;
                            
                            return JSON_OK;
                        }
                    }
                }
            }
        }
    }
    return JSON_ESYNTAX;
}

/* ------------------------------- */

static int json_read_null(tmp_json_t *json)
{
    char ch;

    json->tam=0;

    ch=json_next_char(json);
    if(ch=='n')
    {
        json_jump_char(json);
        ch=json_next_char(json);
        if(ch=='u')
        {
            json_jump_char(json);
            ch=json_next_char(json);
            if(ch=='l')
            {
                json_jump_char(json);
                ch=json_next_char(json);
                if(ch=='l')
                {
                    json_jump_char(json);
                    ch=json_next_char(json);
                    if(isspace(ch) || strchr(",}]",ch))
                        return JSON_OK;
                }
            }
        }
    }
    return JSON_ESYNTAX;
}

/* ------------------------------- */

static int parse_value_json(tmp_json_t *json,json_obj_t *obj);

/* ------------------------------- */

static int parse_array_json(tmp_json_t *json,json_obj_t *obj)
{
    json_obj_t *tmp,*last=NULL;
    int ret;

    obj->type=JSON_ARRAY;

    if(json_next_char(json)!='[')
        return JSON_ESYNTAX;

    json_jump_char(json);

	while(json_jump_blanks(json)!=']')
	{
		ret=json_create_object(NULL,&tmp);
		if(ret)
		    return ret;

   		ret=parse_value_json(json,tmp);
   		if(ret)
   		{
   			free(tmp);
   			return ret;
   		}
        
   		if(last)
            last->next=tmp;
   		else
   		    obj->value=tmp;
	    last=tmp;
        obj->len++;        

		/* Check for more objects */
		
        if(json_jump_blanks(json)!=',')
            break;

        /* Ignore comma */

        json_jump_char(json);
   	}

   	if(json_next_char(json)!=']')
   		return JSON_ESYNTAX;
   	

    json_jump_char(json);
   	return JSON_OK;
}


/* ------------------------------- */

static int parse_object_json(tmp_json_t *json,json_obj_t *obj)
{
    json_obj_t *tmp,*last=NULL;
    int ret;

    obj->type=JSON_OBJECT;

    if(json_next_char(json)!='{')
        return JSON_ESYNTAX;

    json_jump_char(json);

	while(json_jump_blanks(json)=='\"')
	{
	    /* Read name and move to the colon */

		ret=json_read_string(json);
		if(!ret && json_jump_blanks(json)!=':')
			ret=JSON_ESYNTAX;

		if(ret)
			return ret;
		
	    /* Create new element */
        
        ret=json_create_object((json->tam) ? json->buf : NULL,&tmp);
		if(ret)
			return ret;

        /* Avoiding duplication */

        if(json_find_named_child(obj,tmp->name))
        {
            free(tmp);
			return JSON_EEXIST;
        }

        /* Ignore colon and read value  */

        json_jump_char(json);
   		ret=parse_value_json(json,tmp);
   		if(ret)
   		{
   			free(tmp);
			return ret;
   		}

        /* Add new element to the object avoiding duplication */

        if(last)
           last->next=tmp;
        else
            obj->value=tmp;
        last=tmp;
        obj->len++;

        /* See if there are more elements */

		if(json_jump_blanks(json)!=',')
            break;

        /* Ignore comma */

        json_jump_char(json);
   	}

   	if(json_next_char(json)!='}')
   	    return JSON_ESYNTAX;
   	
    json_jump_char(json);

   	return JSON_OK;
}

/* ------------------------------- */

int parse_value_json(tmp_json_t *json,json_obj_t *obj)
{
    int ret = JSON_ESYNTAX;

	switch(json_jump_blanks(json))
	{
		case '{':
		    ret = parse_object_json(json,obj);
            break;
		case '[':
			ret = parse_array_json(json,obj);
            break;
		case '\"':
			ret = json_read_string(json);
			if(ret == JSON_OK)
            {                
           		obj->type=JSON_STRING;
			    obj->value=wcsdupr(json->buf,NULL,&(obj->len));
			    if(!obj->value)
			        ret = JSON_ENOMEM;			    
           	}
           	break;
		case '-':
		case '+':
			ret = json_read_number(json);
			if(ret == JSON_OK)
			{
        		obj->type=JSON_NUMBER;			    
                obj->value=wcsdupr(json->buf,NULL,&(obj->len));
			    if(!obj->value)
			        ret=JSON_ENOMEM;			    
           	}
            break;
		case 'f':
			ret=json_read_false(json);
			if(ret == JSON_OK)
			{
        		obj->type=JSON_BOOLEAN;
			    obj->value=wcsdupr(json->buf,NULL,&(obj->len));
			    if(!obj->value)			    
			        ret=JSON_ENOMEM;			    
           	}
            break;
		case 't':
			ret=json_read_true(json);
			if(ret == JSON_OK)
			{
        		obj->type=JSON_BOOLEAN;
			    obj->value=wcsdupr(json->buf,NULL,&(obj->len));
			    if(!obj->value)
			        ret=JSON_ENOMEM;
           	}
            break;
		case 'n':
			ret=json_read_null(json);
			if(!ret)
			{
        		obj->type=JSON_NULL;
                obj->value=NULL;
                obj->len=0;
           	}
            break;
		default:
		    if(isdigit(json_next_char(json)))
		    {
                ret=json_read_number(json);
                if(!ret)
                {
                    obj->type=JSON_NUMBER;
                    obj->value=wcsdupr(json->buf,NULL,&(obj->len));
                    if(!obj->value)
                        ret=JSON_ENOMEM;
                }
			}
			break;
	}
	return ret;
}

/* ------------------------------- */

json_obj_t *json_parse_FILE(FILE *fp,json_pos_t *p,int *err)
{
    tmp_json_t tmp = TMP_JSON_INIT;
    json_obj_t *obj=NULL;
    int ret;

    if(fp)
    {
        tmp.fp=fp;

        flockfile(fp);

		ret=json_create_object(NULL,&obj);
		if(ret == JSON_OK)
		{
            

			json_jump_char(&tmp);  /* On file mode, start reading */
			json_jump_blanks(&tmp);

			tmp.err=parse_object_json(&tmp,obj);        

			if(tmp.err)
				json_delete_obj(obj);			
			
            if(tmp.buf)
				free(tmp.buf);
		}
		else
		{            
    		tmp.err=ret;
    	}

        funlockfile(fp);
    }
    if(p)
    {
        p->row=tmp.pos.row;
        p->col=tmp.pos.col;
    }
    if(err)
	    *err=tmp.err;
    return obj;
}

/* ------------------------------- */

json_obj_t *json_parse_file(const wchar_t *filename,json_pos_t *p,int *err)
{
    json_obj_t *ret=NULL;

    if(filename)
    {
        FILE *fp=fopenw(filename,"r");
        if(fp)
        {
            flockfile(fp);
	        ret=json_parse_FILE(fp,p,err);
            funlockfile(fp);
            fclose(fp);
        }
    }
    return ret;
}

/* ------------------------------- */

json_obj_t *json_parse(const void *doc,size_t tam,json_pos_t *p,int *err)
{
    tmp_json_t tmp = TMP_JSON_INIT;
    json_obj_t *obj=NULL;
    int ret;

    if(doc)
    {
        tmp.doc=(const char *)doc;
        tmp.len=(tam) ? tam : strlen(doc);
        if(tmp.len)
        {
            ret=json_create_object(NULL,&obj);
            if(ret == JSON_OK)
            {
                json_jump_blanks(&tmp);

                tmp.err=parse_object_json(&tmp,obj);
                if(tmp.err)
                {
                    json_delete_obj(obj);
                    obj=NULL;
                }
                if(tmp.buf)
                    free(tmp.buf);
            }
            else
            {
                tmp.err=ret;
            }
        }
    }
    if(p)
    {
        p->row=tmp.pos.row;
        p->col=tmp.pos.col;
    }
    if(err)
	    *err=tmp.err;
    return obj;
}

/* ------------------------------- *
        Saving Functions
 * ------------------------------- */

int json_save(json_obj_t *obj,const wchar_t *name)
{
    FILE *fp;

    if(obj && name)
    {
        fp=fopenw(name,"wb");
        if(fp)
        {
            flockfile(fp);           
            write_object(fp,obj,0,FALSE,(obj->type==JSON_ARRAY) ? TRUE : FALSE);            
            funlockfile(fp);
            fflush(fp);            
            fclose(fp);
            return JSON_OK;
        }
        return JSON_ECREAT;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_save_FILE(json_obj_t *obj,FILE *fp)
{
    if(obj && fp)
    {
        flockfile(fp);       
        write_object(fp,obj,0,FALSE,(obj->type==JSON_ARRAY) ? TRUE : FALSE);
        funlockfile(fp);
        fflush(fp);        
        return JSON_OK;
    }
    return JSON_EINVAL;
}

/* ------------------------------- *
      Functions to get stuff
 * ------------------------------- */


json_obj_t *json_get_element(json_obj_t *obj,int pos)
{
    json_obj_t *cual;

    if(obj && pos > -1 && (obj->type==JSON_ARRAY || obj->type==JSON_OBJECT))
    {
        int cur=0;

        cual=json_obj_first_element(obj);
        while(cual)
        {
            if(pos==cur++)
                return cual;
            cual=cual->next;
        }
    }
    return NULL;
}

/* ------------------------------- */
/* ------------------------------- */

typedef struct 
{    
    int     idx;
    wchar_t name[JSON_MAX_NAME + 2];
} json_name_t;

typedef struct 
{
    json_name_t *lst;
    int          cnt;    
    int          max;    
} json_name_list_t;

/* ---------------------------------------------- *
    Reads the name of an element -and its index 
    if present- from a path and adds it to the 
    name list. 

    Returns the number of chars read from the 
    path (including  brackets and index if any)

    Note that a spurious point, closing bracket 
    or invalid character in the name will make 
    it return JSON_SYNTAX, and if an element 
    name is too long will return JSON_EFULL.

    It handles using keys as array indexes, so
    an entry like 'obj[name]' will add two entries
    to the name list: {obj,-1},{name,-1} just like 
    it will if the path was: 'obj.name'

    IMPORTANT: 

    The index is 0 based so if no index is 
    specified in the element, 'idx' will be set 
    as -1.
 * ---------------------------------------------- */

static int json_get_name_and_index(const wchar_t *path,json_name_list_t *nl)
{
    int nlen, ilen, inb, cnt,pos;    
    json_name_t *cur;
        
    /* Make sure there are at least two entries */

    if((nl->max - nl->cnt) < 2)
    {
        json_name_t *tmp;
        
        tmp = (json_name_t *)realloc(nl->lst,sizeof(json_name_t) * (nl->max + 10));
        if(tmp == NULL)
            return JSON_ENOMEM;
        nl->lst = tmp;
        nl->max += 10;
    } 
    
    /* Let's go... */
    
    cur = nl->lst + nl->cnt;

    memset(cur->name,0,sizeof(cur->name));
    cur->idx = -1;

    ilen = nlen = cnt = 0;        
    inb = FALSE;        
    
    while(*path)
    {
        if(*path==L'.') 
        {
            /* Spurious point */

            if(nlen == 0) 
                return JSON_ESYNTAX;                 
            ilen++;                  
            break;
        }            
        
        /* Check for closing brackets on text indexed brackets */        

        if(*path==L']') 
        {
            /* Spurious braket */            
            if(inb == FALSE) 
                return JSON_ESYNTAX; 
            path++;    
            inb = FALSE;
            ilen++;                  
            if(*path==L'.')
                ilen++;

            pos = nlen;
            while(pos > 0 && iswspace(cur->name[--pos]))
                cur->name[pos]=0;            
            break;
        }

        /* Read index if there */

        if(*path=='[')
        {            
            /* Spurious bracket */

            if(nlen == 0)   
                return JSON_ESYNTAX;                                        

            do {
                path++;
                ilen++;
            } while(iswblank(*path));

            if(iswdigit(*path) || *path == L']')
            {
                cur->idx = 0 ;
                while(iswdigit(*path))
                {
                    cur->idx *= 10;
                    cur->idx += (int) (*path++ - L'0');
                    ilen++;
                }

                while(iswblank(*path))
                {
                    ilen++;
                    path++;
                }

                if(*path != L']')   /* Missing bracket */
                    return JSON_ESYNTAX;

                ilen++;
                path++;       

                if(*path)
                { 
                    if(*path != L'.') 
                        return JSON_ESYNTAX;
                    ilen++;
                }                    
                break;
            }

            /* Possible string index */

            inb = TRUE;
            nl->cnt++;
            cnt += nlen;
            nlen = 0;
            cur++;  
            memset(cur->name,0,sizeof(cur->name));
            cur->idx = -1;
        }  

        /* Symbols used in JSON are not allowed in a name */

        if(wcschr(NOT_ALLOWED_CHARS,*path))
            return JSON_ESYNTAX;        

        cur->name[nlen] = *path++;                
        if(++nlen == JSON_MAX_NAME + 1)
            return JSON_EFULL;
    }

    /* Missing bracket */

    if(inb == TRUE)
        return JSON_ESYNTAX;

    nl->cnt++;
    cnt += nlen;

    return (cnt + ilen);
}

/* ------------------------------- */

static void json_free_split_path(json_name_list_t *nl)
{
    if(nl)
    {
        if(nl->lst)
            free(nl->lst);
        nl->lst = NULL;
        nl->cnt = 0;
        nl->max = 0;
    }
}

/* -------------------------------------------------------- *
    This function splits an object path like:

        object1.array1[3].object2[childofit].last[5]

    in an array of of json_name_t elements and returns
    in 'retval'. In this example, 'retval' will have a 
    an array like this:

        {"object1",-1}, 
        {"array1",3},
        {"object2",-1},
        {"childofit",-1},
        {"last",5}

    You have to free 'retval' with json_free_split_path()
 * -------------------------------------------------------- */

static int json_split_path(const wchar_t *path,json_name_list_t *ret)
{
    int cnt;

    while(*path)
    {        
        cnt = json_get_name_and_index(path,ret);
        if(cnt < 1)
        {
            json_free_split_path(ret);
            if(cnt == 0)
                break;            
            return cnt;
        }                
        path+=cnt;
    }

    if(ret->cnt == 0)
    {
        json_free_split_path(ret);
        return JSON_ENODATA;
    }
    return JSON_OK;
}

/* -------------------------------------------------------- *
    This function checks a path to be sure it refers to a
    single object, which's name it returns and also the 
    index if a cardinal one was specified.  

    If a path with more than a member is specified, then
    it will return JSON_EINCORRECT

    Correct examples:

        json_object_name(L"object_name",name,256,&idx);
        json_object_name(L"array_name[3],name,256,&idx);

    Incorrect examples:

        json_object_name(L"object.name",name,256,&idx);
        json_object_name(L"array[name]",name,256,&idx)
 * -------------------------------------------------------- */

static int json_get_object_name(const wchar_t *path,wchar_t *name,int len,int *idx)
{
    int ret;
    json_name_list_t list = {NULL,0,0};

    ret = json_split_path(path,&list);
    if(ret == JSON_OK) 
    {
        if(list.cnt == 1)
        {
            wcslcpy(name,list.lst[0].name,len);
            *idx = list.lst[0].idx;
        }
        else
        {
            ret = JSON_EINCORRECT;
        }
        json_free_split_path(&list);        
    }        
    return ret;
}

/* -------------------------------------- *
   This function creates a named object,
   attachs it to its parent (if sent) and
   returns it on 'robj'.

   If idx is not -1, it makes the object
   an array and creates idx+1 child null
   objects, then return the last child in 
   'robj' and the object in 'parent'. 

    Unless is 0 where it doesn't add 
    any objects

   This is to handle the creation of 
   objects named like 'name[idx]'.
 * -------------------------------------- */

static int json_create_named_object(const wchar_t *name, int idx,json_obj_t **parent,json_obj_t **robj)
{
    int t,ret;
    json_obj_t *obj,*child,*last = NULL;

    ret=json_create_object(name,&obj);
    if(ret)    
        return ret;
        
    if(idx > -1)
    {
        obj->type=JSON_ARRAY;        
        for( t = -1; t < idx; t++)
        {
            ret=json_create_object(NULL,&child);
            if(ret)
            {
                json_delete_obj(obj);
                return ret;
            }            
            if(last)
                last->next = child;
            else
                obj->value = child;
            obj->len++;
            last = child;
        }
    }

    if(*parent)
        json_attach_object(*parent,obj);

    if(last)
    {
        *parent = obj;
        *robj = last;
    }
    else 
    {
        *robj = obj; 
    }
    return JSON_OK;
}


/* -------------------------------------------------------- *
   This function creates all objects in the given 
   array of json_name_t.

   It then return the root object in 'rret' (if it 
   was NULL to start with), the last object in 'oret' 
   and the  parent object of 'oret' in 'pret'.

   Depending on the path sent, you could end up with 
   two situations that you need to handle when calling 
   this function:

        1.- rret == oret 
        2.- rret == pret         

    1.- happens when rret was originally NULL and one 
        objects is created (so pret will be NULL on 
        return)

    2.- happens when rret was originally NULL and two 
        object are created, OR when rret is not originally 
        NULL and only one object is created
        
    On the rest of ocasions the 3 values will be different
 * -------------------------------------------------------- */


static int json_create_object_path(json_name_list_t *path,int pos,json_obj_t **rret,json_obj_t **pret,json_obj_t **oret)
{    
    int err;
    json_obj_t *obj,*root,*prev;
    
    root = prev = *rret;
    obj  = NULL;       

    while(pos < path->cnt)
    {               
        err = json_create_named_object(path->lst[pos].name,path->lst[pos].idx,&prev,&obj);        
        if(err)
        {
            if(root != *rret)
                json_delete_obj(root);            
            return err;     
        }
            
        /* 
            First round root == obj
            Second round root == prev 

            Rest of rounds root, prev 
            and obj will all be 
            different.
        */

        if(root == NULL) 
            root = (prev) ? prev: obj;

        pos++;

        if(pos < path->cnt)
        {
            obj->type = JSON_OBJECT;
            if(root == obj)
                root = NULL;
            prev = obj;
        }            
    }

   *rret = root; 
   *pret = prev; 
   *oret = obj;

    return JSON_OK;
} 

/* -------------------------------------------------------------- *
   This function is the behind-scenes for json_get_obj() as well 
   as for json_create_obj_path().

   Basically gets the object on a path that has 'obj' as root, 
   and if the path is not complete, but 'create' is TRUE, it 
   will create the rest of the path.

   The last object of the path (created or found) would be 
   returned in 'cual' (means 'which one' in Spanish) and its
   parent will be returned in 'padre' (which means 'father' in
   Spanish).  

   Note that if only one object is created, 'padre' will point 
   to 'obj'.

   IMPORTANT: the siblings of 'obj' are not scanned.
 * -------------------------------------------------------------- */

static int json_get_object(json_obj_t *obj,const wchar_t *path,json_obj_t **padre,json_obj_t **cual,int create)
{    
    int pos,err;
    json_obj_t *prev,*tmp;
    json_name_list_t nlst = {NULL,0,0};

    prev = NULL;    

    if(!path)
    {
        if(create)
        {
            err = json_create_object(NULL,cual);
            if(err == JSON_OK)
            {
               *padre = NULL;
                if(obj)
                {
                    err = json_attach_object(obj,*cual);
                    if(err==JSON_OK)
                    {
                        *padre = obj;
                    }
                    else 
                    {
                        json_delete_obj(*cual);
                       *cual = NULL; 
                    }                
                }
            }
            return err;
        }        
    }
    else
    {
        err = json_split_path(path,&nlst);    
        if(err != JSON_OK)
            return err;

        pos = 0;

        if(obj == NULL)
        {        
            if(!create) 
            {    
                json_free_split_path(&nlst);
                return JSON_EFIND;
            }
           *padre = NULL;

            err =  json_create_object_path(&nlst,pos,padre,&prev,cual);            
            json_free_split_path(&nlst);
            return err;
        }

        while(pos < nlst.cnt)
        {
            switch(obj->type)
            {
                case JSON_OBJECT:
                case JSON_ARRAY:
                    break;
                case JSON_NULL:
                    if(create)
                    {
                       *padre = obj;
                        prev = obj;
                        err =  json_create_object_path(&nlst,pos,&prev,padre,cual);            
                        json_free_split_path(&nlst);
                        return err;
                    }                
                default:
                    json_free_split_path(&nlst);
                    return (create) ? JSON_EINCORRECT : JSON_EFIND;
            }

            prev = obj;

            tmp = json_find_named_child(obj,nlst.lst[pos].name);
            if(tmp == NULL)
            {
                if(create)
                {          
                    err =  json_create_object_path(&nlst,pos,&prev,&tmp,cual);
                    if(err == JSON_OK)
                        *padre = tmp;
                }
                else
                {
                    err = JSON_EFIND;
                }
                json_free_split_path(&nlst);
                return err;
            }    
            else if(nlst.lst[pos].idx > -1)
            {                    
                prev = tmp;
                tmp = json_get_element(prev,nlst.lst[pos].idx);
                if(!tmp)
                {
                    json_free_split_path(&nlst);
                    return JSON_EFIND;
                }                
            }                                                
            obj = tmp;
            pos++;            
        }
        
        /* Found */

        json_free_split_path(&nlst);

       *padre = prev;
       *cual  = obj;
        return JSON_OK;
    }
    return JSON_EFIND;
}

/* ------------------------------- */

json_obj_t *json_get_obj(json_obj_t *obj,const wchar_t *path,json_obj_t **parret)
{
    json_obj_t *parent,*child;
    int ret;

    ret = json_get_object(obj,path,&parent,&child,FALSE);
    if(ret==JSON_OK)
    {
        if(parret)
            *parret = parent;
        return child;
    }
    return NULL;
}

/* ---------------------------------------- *
    This function finds a object named
    'name' with index 'idx' (if 'idx' is 
    not -1) and then returns it.

    It also returns the parent object  in
    'padre'
 * ---------------------------------------- */

static json_obj_t *json_find_object(json_obj_t *obj,const wchar_t *name,int idx,json_obj_t *par,json_obj_t **padre,int in_array)
{
    json_obj_t *cual;
        
    while(obj)
    {
        if(!in_array && !wcscmp(obj->name,name))
        {                
            if(idx > -1)
            {                
                cual = json_get_element(obj,idx);
                if(cual == NULL)
                    return NULL;
                if(padre)
                   *padre = obj;                
            }
            else 
            {
                if(padre)
                   *padre = par;
                cual  = obj;
            }            
            return cual;
        }
        
        if(obj->value)
        {
            int inarr = FALSE;
            switch(obj->type)
            {
                case JSON_ARRAY:
                    inarr = TRUE;
                case JSON_OBJECT:                                
                    cual = json_find_object((json_obj_t *)obj->value,name,idx,obj,padre,inarr);
                    if(cual)
                        return cual;                                     
                    break;
                default:
                    break;
            }
        }        
        obj=obj->next;
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_find_obj(json_obj_t *obj,const wchar_t *tofind,json_obj_t **parent)
{
    int idx = 0,ret;    
    wchar_t name[256];
    json_obj_t *cual = NULL;
    
    if(obj && tofind) 
    {        
        ret = json_get_object_name(tofind,name,256,&idx);
        if(ret)
            return NULL;

        if(!wcscmp(obj->name,name))
        {                
            if(idx > -1)
            {
                cual = json_get_element(obj,idx);
                if(cual == NULL)
                    return NULL;
                if(parent)
                    *parent = obj;
            }
            else
            {
                if(parent)
                    *parent = NULL;
                cual = obj;
            }                                    
        }
        else if(obj->type == JSON_ARRAY || obj->type == JSON_OBJECT)
        {
            int inarr = (obj->type == JSON_ARRAY) ? TRUE : FALSE;
            cual = json_find_object((json_obj_t *)obj->value,name,idx,obj,parent,inarr);                        
        }        
    }
    return cual;
}

/* ------------------------------- */

typedef struct 
{    
    func_json_t func;   
    void *opaque;    
} traverse_t;

/* ------------------------------- */

static json_obj_t *json_traverse_object(json_obj_t *obj,int index,json_obj_t *par,traverse_t *tra,int level)
{
    json_obj_t *cual;    

    while(obj)
    {      
        if(tra->func(obj,index,par,level,tra->opaque))
            return obj;
            
        switch(obj->type)
        {
            case JSON_ARRAY:                
                if(obj->value)
                {
                    int idx = 0;
                    json_obj_t *chld = (json_obj_t *)obj->value;

                    do {
                        if(tra->func(chld,idx++,obj,level + 1,tra->opaque))
                            return chld;

                        if(chld->value && (chld->type == JSON_ARRAY || chld->type==JSON_OBJECT))
                        {                            
                            cual = json_traverse_object((json_obj_t *)chld->value,-1,chld,tra,level + 2);
                            if(cual)
                                return cual;
                        }
                        chld = chld->next; 
                    } while(chld);
                }
                break;
            case JSON_OBJECT:                    
                if(obj->value)
                {
                    cual = json_traverse_object((json_obj_t *)obj->value,-1,obj,tra,level + 1);
                    if(cual)
                        return cual;
                }
                break;                
            default:
                break;
        }
        obj = (level) ? obj->next : NULL;        
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_traverse_obj(json_obj_t *obj,func_json_t func,void *opaque)
{
    if(func && obj) 
    {
        traverse_t tra;        

        tra.func = func;
        tra.opaque = opaque;
        
        return json_traverse_object(obj,-1,NULL,&tra,0);
    }
    return NULL;
}

/* ------------------------------- */

int json_existing_objs(json_obj_t *obj,int num,...)
{
    va_list vl;
    int t;
    const wchar_t *name;

    if(obj && num > 0)
    {
        va_start(vl,num);
        for(t=0;t<num;t++)
        {
            name = va_arg(vl,const wchar_t *);
            if(!json_get_obj(obj,name,NULL))
                return FALSE;
        }
        va_end(vl);
        return TRUE;
    }
    return FALSE;
}

/* ------------------------------- */

wchar_t *json_get_string(json_obj_t *obj,const wchar_t *path)
{
    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_NUMBER:
            case JSON_STRING:
            case JSON_BOOLEAN:
                return (wchar_t *)obj->value;
            /*
            case JSON_NULL:
            case JSON_ARRAY:
            case JSON_OBJECT:
            */
            default:
                break;
        }
    }
    return NULL;
}

/* ------------------------------- */

time_t json_get_time(json_obj_t *obj,const wchar_t *path)
{            
    point_on_time_t pot = {0};

    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj)
    {
        wchar_t *value = (wchar_t *)obj->value;

        switch(obj->type)
        {
            case JSON_STRING:                
                if(obj->len == 15 && value[14]=='Z')
                {                    
                    swscanf(value,L"%04d%02d%02d%02d%02d%02dZ",&pot.year,&pot.mon,&pot.day,&pot.hour,&pot.min,&pot.sec);
                    return time_from_pot(&pot);
                }
                else if(obj->len == 19 && wcschr(value,':') && wcschr(value,'-') && wcschr(value,' '))
                {
                    swscanf(value,L"%04d-%02d-%02d %02d:%02d:%02d",&pot.year,&pot.mon,&pot.day,&pot.hour,&pot.min,&pot.sec);
                    return time_from_pot(&pot);
                }
            case JSON_NUMBER:            
                return (time_t)wcstol(value,NULL,10);                
            default:
                break;
        }
    }
    return 0;
}


/* ------------------------------- */

int json_get_boolean(json_obj_t *obj,const wchar_t *path)
{
    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj && obj->value)
    {
        wchar_t *value = (wchar_t *)obj->value;

        switch(obj->type)
        {
            case JSON_NUMBER:
                if(wcstod(value,NULL) != 0.0)
                    return TRUE;
                break;
            case JSON_STRING:
                if(!wcsempty(value))
                    return TRUE;
                break;
            case JSON_BOOLEAN:
                if(wcscmp(value,L"false"))
                    return TRUE;
                break;
            case JSON_NULL:
                return FALSE;
            default:
                break;
        }
    }
    return FALSE;
}

/* ------------------------------- */

double json_get_number(json_obj_t *obj,const wchar_t *path)
{
    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj && obj->value)
    {
        wchar_t *value = (wchar_t *)obj->value;

        switch(obj->type)
        {
            case JSON_BOOLEAN:
                if(wcscmp(value,L"false"))
                    return 1.0;
                break;
            case JSON_STRING:
            case JSON_NUMBER:
                return wcstod(value,NULL);
            case JSON_NULL:
                return 0.0;
            default:
                break;
        }
    }
    return 0.0;
}

/* ------------------------------- */

unsigned long int json_get_ulong(json_obj_t *obj,const wchar_t *path)
{
    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj && obj->value)
    {
        wchar_t *value = (wchar_t *)obj->value;

        switch(obj->type)
        {
            case JSON_BOOLEAN:
                if(wcscmp(value,L"false"))
                    return 1;
                break;
            case JSON_STRING:
            case JSON_NUMBER:
                if(wcsstr(value,L"e+") || wcsstr(value,L"e-") || wcsstr(value,L"E+") || wcsstr(value,L"E-"))                
                     return wcstold(value,NULL);
                return wcstoul(value,NULL,10);
            case JSON_NULL:
                return 0;
            default:
                break;
        }
    }
    return 0;
}


/* ------------------------------- */

long int json_get_long(json_obj_t *obj,const wchar_t *path)
{
    return (long int)json_get_ulong(obj,path);
}

/* ------------------------------- */

short int json_get_short(json_obj_t *obj,const wchar_t *path)
{
    long li = json_get_long(obj,path);
    return (short int)(li & 0xffff);
}

/* ------------------------------- */

unsigned short int json_get_ushort(json_obj_t *obj,const wchar_t *path)
{
    unsigned long lu = json_get_ulong(obj,path);
    return (unsigned short int)(lu & 0xffff);
}

/* ------------------------------- */

int json_get_int(json_obj_t *obj,const wchar_t *path)
{
    long li = json_get_long(obj,path);
    return (int)(li & 0xffffffff);
}

/* ------------------------------- */

unsigned int json_get_uint(json_obj_t *obj,const wchar_t *path)
{
    unsigned long lu = json_get_ulong(obj,path);
    return (unsigned int)(lu & 0xffffffff);
}

/* ------------------------------- */

int json_get_bytes(json_obj_t *obj,const wchar_t *path,void **ret)
{
    int num,max,dig,pushit;
    unsigned char *buf,val;
    wchar_t *ptr;

    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj && obj->value && obj->type==JSON_STRING)
    {        
        max = (obj->len / 2) + 8; /* A good guess with a small safety space */
        
       *ret = (unsigned char *)malloc(max);
        if(*ret == NULL)
            return JSON_ENOMEM;

        num = dig = 0;
        buf = *ret;
        ptr = (wchar_t *)obj->value;
        val = 0;
        
        pushit=FALSE;
    
        while(*ptr && num < max)
        {
            if(iswxdigit(*ptr))
            {
                if (dig)
                {
                    val <<= 4;
                    val |= (unsigned char) whex_to_int(*ptr);
                    pushit=TRUE;
                }
                else
                {
                    dig++;
                    val=(unsigned char)whex_to_int(*ptr);
                }
            }
            else if(*ptr==L' ' || *ptr==L':' || *ptr==L',')
            {
                val <<= 4;
                pushit=TRUE;
            }
            else
            {
                val <<= 4;
                break;
            }
            if(pushit)
            {
                buf[num++]=val;
                dig=0;
                val=0;
                pushit=FALSE;
            }
            ptr++;
        }

        if(dig && num < max)
            buf[num++]=val;
        
        /* 
            If num == max it means the string obj->value is 
            NOT nul terminated (which can only mean a memory 
            leak). Any other combination of the string will 
            end up as being the double of chars than bytes.
            As we add extra 8 bytes to max, then 'num' should 
            never catch up 'max'.

            Therefore we prefer to fail the function on this
            case than risk telling the user that the data is
            OK and cause anohter problem somewhere else, as
            we do not know what this data is.
        */

        if(num == max)
        {
            free(*ret);
           *ret = NULL;
            return JSON_EBUG; 
        }

        return num;

    }
    return JSON_EMISSING;
}

/* ------------------------------- */

mp_int_t *json_get_bignum(json_obj_t *obj,const wchar_t *path)
{
    void *ret=NULL;
    mp_int_t *bn=NULL;
    int len;

    len=json_get_bytes(obj,path,&ret);
    if(len > 0)
    {
        bn = mp_create();
        mp_set_bytes(bn,ret,len);
        free(ret);
    }
    return bn;
}

/* ------------------------------- */

int json_get_bytes_base64(json_obj_t *obj,const wchar_t *path,void **ret)
{
    int num=0;

    if(path)
        obj=json_get_obj(obj,path,NULL);

    if(obj && obj->value && obj->type==JSON_STRING)
    {
        if(ret==NULL)
            return base64_size_dec(obj->len);

       *ret = malloc(obj->len);
        if(*ret)
        {
            num=(int)base64_decodew(*ret,obj->len,(wchar_t *)obj->value,obj->len,TRUE);
            if(num < 1)
            {
                free(*ret);
               *ret = NULL;
            }
            return num;
        }
        return JSON_ENOMEM;
    }
    return JSON_EINCORRECT;
}

/* ------------------------------- */

mp_int_t *json_get_bignum_base64(json_obj_t *obj,const wchar_t *path)
{
    void *ret=NULL;
    mp_int_t *bn=NULL;
    int len;

    len=json_get_bytes_base64(obj,path,&ret);
    if(len > 0)
    {
        bn = mp_create();
        mp_set_bytes(bn,ret,len);
        free(ret);
    }
    return bn;
}

/* ------------------------------- *
    Object manipulation functions
 * ------------------------------- */

void json_delete_obj(json_obj_t *obj)
{
    if(obj)
    {
        if(obj->value)
        {
            if(obj->type==JSON_OBJECT || obj->type==JSON_ARRAY)
            {
    	        json_delete_object_list((json_obj_t *)obj->value);                
            }
            else
            {
                memset(obj->value,0,sizeof(wchar_t) * obj->len);
                free(obj->value);
            }
        }
        memset(obj,0,sizeof(json_obj_t));
    	free(obj);
    }
}

/* ------------------------------- */

json_obj_t *json_detach_obj(json_obj_t *parent,json_obj_t *child)
{
    if(parent && child && parent != child)
    {
        if(parent->type==JSON_OBJECT || parent->type==JSON_ARRAY)
        {
            if(json_detach_object(parent,child))
                return child;
        }
    }
    return NULL;
}

/* ------------------------------- */

int json_add_obj(json_obj_t *parent,json_obj_t *child)
{
    if(parent && child && parent != child)
    {
        if(parent->type == JSON_OBJECT && json_find_named_child(parent,child->name))
            return JSON_EEXIST;
        return json_attach_object(parent,child); 
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_insert_obj(json_obj_t *parent,json_obj_t *child,int pos)
{
    json_obj_t *obj,*pre;
    int t;

    if(parent && child && child != parent)
    {
        switch(parent->type)
        {
            case JSON_NULL:
                parent->type=JSON_ARRAY;
                parent->value=child;
                parent->len = 1;
                return JSON_OK;            
            case JSON_OBJECT:  
                if(json_find_named_child(parent,child->name))
                    return JSON_EEXIST;              
                pos = parent->len;                
            case JSON_ARRAY:                    
                if(pos < 1)
                {
                    child->next=(json_obj_t *)parent->value;
                    parent->value=child;                                        
                }
                else 
                {
                    pre =(json_obj_t *)parent->value;
                    obj = pre->next;
                    for(t=1;t<pos && obj;t++)
                    {
                        pre=obj;
                        obj=obj->next;
                    }
                    pre->next=child;
                    child->next=obj;
                }
                parent->len++;
                return JSON_OK;
            default:
                break;
        }
    }
    return JSON_EINVAL;

}

/* ------------------------------- */

json_obj_t *json_create_obj(json_obj_t *parent,const wchar_t *name)
{
    json_obj_t *obj;
    int ret;
    
    if(parent)
    {
        switch(parent->type)
        {
            case JSON_OBJECT:            
                obj=json_find_named_child(parent,name);
                if(obj)
                    return obj;
            case JSON_ARRAY:                    
            case JSON_NULL:
                break;
            default:
                return NULL;
        }
        
    }

    ret=json_create_object(name,&obj);
    if(ret == JSON_OK)
    {
        if(parent && json_attach_object(parent,obj))            
        {
            free(obj);
            return NULL;
        }
        return obj;
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_create_array(json_obj_t *parent,const wchar_t *name)
{
    json_obj_t *ret=json_create_obj(parent,name);
    if(ret)
        ret->type=JSON_ARRAY;
    return ret;
}

/* ------------------------------- */

json_obj_t *json_create_obj_path(json_obj_t *obj,const wchar_t *path,json_obj_t **parret)
{
    json_obj_t *parent,*child;
    int ret;

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_OBJECT:
            case JSON_ARRAY:
            case JSON_NULL:
                break;
            case JSON_NUMBER:            
                if(obj->value)
                    free(obj->value);
            default:
                obj->type=JSON_OBJECT; 
                obj->len = 0;
                obj->value = NULL;            
                break;
        }
    }
    ret = json_get_object(obj,path,&parent,&child,TRUE);
    if(ret==JSON_OK)
    {
        if(parret)
            *parret = parent;
        return child;
    }
    return NULL;

}

/* ------------------------------- *
        Set values functions
 * ------------------------------- */

int json_set_string(json_obj_t *obj,const wchar_t *value)
{
    wchar_t *val=NULL;

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_BOOLEAN:
                val=wcsdupr((value && !wcscasecmp(value,L"true")) ? L"true" : L"false",(wchar_t *)obj->value,&(obj->len));
                if(val)
                {                   
                    obj->value=val;
                    return JSON_OK;
                }                
                return JSON_ENOMEM;
            case JSON_NULL:
                obj->type=JSON_STRING;
            case JSON_STRING:
                if(!value)
                    value=L"";
                val=wcsdupr(value,(wchar_t *)obj->value,&(obj->len));
                if(val)
                {
                    obj->value=val;
                    return JSON_OK;
                }
                return JSON_ENOMEM;
            case JSON_NUMBER:
                if(!value)
                    value=L"0";
                val=(wchar_t *)calloc(sizeof(wchar_t),wcslen(value) + 1);
                if(val)
                {
                    if(obj->value)
                        free(obj->value);

                    obj->value=val;
                    obj->len=1;

                    if(*value==L'-')
                    {
                        *val++=*value++;
                         obj->len++;
                    }
                    else if(*value==L'+')
                    {
                        value++;
                    }

                    while(iswdigit(*value))
                    {
                        *val++=*value++;
                         obj->len++;
                    }

                    if(*value==L'.')
                    {
                        *val++=*value++;
                        obj->len++;
                        while(iswdigit(*value))
                        {
                            *val++=*value++;
                            obj->len++;
                        }
                    }

                    /* Exponente */

                    if(*value==L'e' || *value==L'E')
                    {
                        *val++=*value++;
                         obj->len++;

                        if(*value==L'-' || *value==L'+')
                        {
                            *val++=*value++;
                             obj->len++;
                        }

                        while(iswdigit(*value))
                        {                            
                            *val++=*value++;
                            obj->len++;
                        }
                    }

                    if(val == obj->value)
                        *val=L'0';
                    return JSON_OK;
                }                
                return JSON_ENOMEM;
            default:
                return JSON_EINCORRECT;
        }
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_utf8(json_obj_t *obj,const char *value)
{
    wchar_t *val=NULL;

    if(obj)
    {
        size_t max;

        switch(obj->type)
        {
            case JSON_BOOLEAN:
                val=wcsdupr((value && !strcasecmp(value,"true")) ? L"true" : L"false",(wchar_t *)obj->value,&(obj->len));
                if(val)
                {                    
                    obj->value=val;
                    return JSON_OK;
                }                
                return JSON_ENOMEM;
            case JSON_NULL:
                obj->type=JSON_STRING;
            case JSON_STRING:
                if(!value)
                    max = 1;
                else
                    max = strlen(value) + 1;
                val=(wchar_t *)calloc(sizeof(wchar_t),max);
                if(val)
                {
                    obj->len=1;
                    if(obj->value)
                        free(obj->value);
                    obj->value=val;
                    if(max > 1)
                    {
                        utf8_to_wchar(val,max,(unsigned char *)value,max);                    
                        obj->len = wcslen(val);
                    }
                    return JSON_OK;
                }                
                return JSON_ENOMEM;
            case JSON_NUMBER:
                if(!value)
                    max = 2;
                else
                    max = strlen(value) + 1;
                val=(wchar_t *)calloc(sizeof(wchar_t),max);
                if(val)
                {
                    if(obj->value)
                        free(obj->value);

                    obj->value=val;
                    obj->len=1;

                    if(*value=='-')
                    {
                       *val++=L'-';
                        value++;
                        obj->len++;
                    }
                    else if(*value=='+')
                    {
                        value++;
                    }

                    while(isdigit(*value))
                    {
                        *val++=(wchar_t)*value++;
                        obj->len++;
                    }

                    if(*value==L'.')
                    {
                       *val++=L'.';                       
                        value++;
                        obj->len++;
                        while(isdigit(*value))
                        {
                            *val++=(wchar_t)*value++;
                            obj->len++;
                        }
                    }

                    /* Exponent */

                    if(*value=='e' || *value=='E')
                    {
                       *val++=L'E';
                        value++;
                        obj->len++;

                        if(*value=='-')
                        {
                           *val++=L'-';
                            value++;
                            obj->len++;
                        }
                        else if(*value=='+')
                            value++;

                        while(isdigit(*value))
                        {
                            *val++=(wchar_t)*value++;
                            obj->len++;
                        }
                    }

                    if(val == obj->value)
                        *val=L'0';
                    return JSON_OK;
                }                
                return JSON_ENOMEM;
            default:
                return JSON_EINCORRECT;
        }
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_time(json_obj_t *obj,time_t value,int fmt)
{
    wchar_t *val,tmp[256];

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_NULL:                
            case JSON_NUMBER:                
            case JSON_STRING:                
                if(fmt != JSON_UNIX_TIME)
                {
                    point_on_time_t pot;

                    obj->type=JSON_STRING;
                    time_to_pot(value,&pot,FALSE);
                    if(fmt == JSON_TIME_DB)
                        swprintf(tmp,256,L"%04d-%02d-%02d %02d:%02d:%02d",pot.year,pot.mon,pot.day,pot.hour,pot.min,pot.sec);
                    else
                        swprintf(tmp,256,L"%04d%02d%02d%02d%02d%02dZ",pot.year,pot.mon,pot.day,pot.hour,pot.min,pot.sec);
                }
                else
                {
                    obj->type=JSON_NUMBER;
                    swprintf(tmp,256,L"%lld",(long long)value);
                }                
                break;
            case JSON_BOOLEAN:
                wcslcpy(tmp,(value==0) ? L"false" : L"true",256);
                break;
            default:
                return JSON_EINCORRECT;
        }
        val=wcsdupr(tmp,(wchar_t *)obj->value,&(obj->len));
        if(val)
        {            
            obj->value=val;
            return JSON_OK;
        }
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}


/* ------------------------------- */

static int copy_number(wchar_t *dest,const wchar_t *number)
{    
    int len = 0;    

    while(iswspace(*number))
        number++;
    
    if(*number == L'+') 
        number++;       

    if(*number == L'-')
        dest[len++] = *number++;

    while(iswspace(*number))
        number++;

    while(iswdigit(*number))
        dest[len++] = *number++;
        
    /* Fractional part (mantissa) */

    if(*number==L'.')
    {
        dest[len++]=*number++;
        while(iswdigit(*number))
            dest[len++]=*number++;
    }

    /* Exponent */

    if(*number==L'e' || *number==L'E')
    {
        dest[len++]=*number++;        
        switch(*number)
        {
            case L'-':            
            case L'+':
                dest[len++]=*number++;
                break;            
            default:
                break;
        }
        
        if(iswdigit(*number))
        {    
            do {
                dest[len++]=*number++;
            } while(iswdigit(*number));
        }
        else 
        {
            dest[len++]=L'0';
        }
    }
    
    return len;
}

/* ------------------------------- */

int json_set_number_str(json_obj_t *obj,const wchar_t *value)
{
    wchar_t *val;
    int len;

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_NULL:
                obj->type=JSON_NUMBER;
            case JSON_NUMBER:
            case JSON_STRING:
                break;
            case JSON_BOOLEAN:
                value = wcsempty(value) ? L"false" : L"true";
                break;
            default:
                return JSON_EINCORRECT;
        }
        len = (int) wcslen(value);
        val=(wchar_t *)realloc(obj->value,len * sizeof(wchar_t) + 4);
        if(val)
        {
            obj->value=val;
            obj->len = copy_number(val,value);
            return JSON_OK;
        }
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_number(json_obj_t *obj,double value)
{
    wchar_t *val,tmp[256];
    double entero,fraccion;

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_NULL:
                obj->type=JSON_NUMBER;
            case JSON_NUMBER:
            case JSON_STRING:
                fraccion = modf(value,&entero);
                if(fraccion == 0.0)
                    swprintf(tmp,256,L"%.0f",entero);
                else
                    swprintf(tmp,256,L"%E",value);
                break;
                break;
            case JSON_BOOLEAN:
                wcslcpy(tmp,(value==0.0) ? L"false" : L"true",256);
                break;
            default:
                return JSON_EINCORRECT;
        }
        val=wcsdupr(tmp,(wchar_t *)obj->value,&(obj->len));
        if(val)
        {
            obj->value=val;            
            return JSON_OK;
        }
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_boolean(json_obj_t *obj,int value)
{
    wchar_t *val,*tmp;

    if(obj)
    {
        switch(obj->type)
        {
            case JSON_NUMBER:
                tmp=(value) ? L"1" : L"0";
                break;
            case JSON_NULL:
                obj->type=JSON_BOOLEAN;
            case JSON_STRING:
            case JSON_BOOLEAN:
                tmp=(value) ? L"true" : L"false";
                break;
            default:
                return JSON_EINCORRECT;
        }
        val=wcsdupr(tmp,(wchar_t *)obj->value,&(obj->len));
        if(val)
        {            
            obj->value=val;
            return JSON_OK;
        }
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_null(json_obj_t *obj)
{
    if(obj)
    {
        json_delete_object_list((json_obj_t *)obj->value);
        obj->value = NULL;
        obj->type=JSON_NULL;
        obj->len=0;
        return JSON_OK;
    }
    return JSON_EINVAL;
}


/* ------------------------------- */

int json_set_base64(json_obj_t *obj,const void *value,size_t num)
{
    if(obj && num > 0)
    {
        size_t tam;

        if(obj->type==JSON_ARRAY || obj->type==JSON_OBJECT)
            return JSON_EINCORRECT;

        tam=base64_size_enc(num) + 1;
        obj->value=calloc(sizeof(wchar_t),tam);
        if(obj->value)
        {
            obj->type=JSON_STRING;
            base64_encodew(obj->value,tam,value,num,FALSE);
            obj->len = (int)wcslen((wchar_t *)obj->value);
            return JSON_OK;
        }
        obj->type=JSON_NULL;
        obj->len=0;        
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_bytes(json_obj_t *obj,const void *value,size_t num)
{
    if(obj && num > 0)
    {
        size_t tam;

        if(obj->type==JSON_ARRAY || obj->type==JSON_OBJECT)
            return JSON_EINCORRECT;

        tam=(num * 2) + 1;
        obj->value=calloc(sizeof(wchar_t),tam);
        if(obj->value)
        {
            obj->type=JSON_STRING;            
            hex_to_wchar((wchar_t *)obj->value,tam,value,num,FALSE);
            obj->len = (int)wcslen((wchar_t *)obj->value);
            return JSON_OK;
        }
        obj->type=JSON_NULL;
        obj->len=0;        
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}


/* ------------------------------- */

int json_set_hexa(json_obj_t *obj,const void *value,size_t num)
{
    if(obj && num > 0)
    {
        size_t tam;

        if(obj->type==JSON_ARRAY || obj->type==JSON_OBJECT)
            return JSON_EINCORRECT;

        tam=(num * 3) + 1;
        obj->value=calloc(sizeof(wchar_t),tam);
        if(obj->value)
        {
            obj->type=JSON_STRING;            
            hex_to_wchar((wchar_t *)obj->value,tam,value,num,L':');
            obj->len = (int)wcslen((wchar_t *)obj->value);
            return JSON_OK;
        }
        obj->type=JSON_NULL;
        obj->len=0;
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_bignum(json_obj_t *obj,mp_int_t *bn)
{
    if(obj && bn)
    {
        if(obj->type==JSON_ARRAY || obj->type==JSON_OBJECT)
            return JSON_EINCORRECT;

        obj->value=mp_get_wide_string(bn);
        if(obj->value)
        {
            obj->type=JSON_STRING;
            obj->len = (int)wcslen((wchar_t *)obj->value);
            return JSON_OK;
        }
        obj->type=JSON_NULL;
        obj->len=0;
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- */

int json_set_big_int(json_obj_t *obj,mp_int_t *bn)
{
    if(obj && bn)
    {
        if(obj->type==JSON_ARRAY || obj->type==JSON_OBJECT)
            return JSON_EINCORRECT;

        obj->value=mp_get_wide_string_decimal(bn);
        if(obj->value)
        {
            obj->type=JSON_STRING;
            obj->len = (int)wcslen((wchar_t *)obj->value);
            return JSON_OK;
        }
        obj->type=JSON_NULL;
        obj->len=0;
        return JSON_ENOMEM;
    }
    return JSON_EINVAL;
}

/* ------------------------------- *
        Assignment functions
 * ------------------------------- */

json_obj_t *json_assign_string(json_obj_t *parent,const wchar_t *path,const wchar_t *value)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_string(obj,value)==JSON_OK)
            return obj;    
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_utf8(json_obj_t *parent,const wchar_t *path,const char *value)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_utf8(obj,value)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_number(json_obj_t *parent,const wchar_t *path,double value)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_number(obj,value)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_number_str(json_obj_t *parent,const wchar_t *path,const wchar_t *str)
{
    json_obj_t *par,*obj;
    int ret=0;
        
    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_number_str(obj,str)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }    
    return NULL;
}


/* ------------------------------- */

json_obj_t *json_assign_boolean(json_obj_t *parent,const wchar_t *path,int value)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_boolean(obj,value)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }        
    }
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_null(json_obj_t *parent,const wchar_t *path)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_null(obj)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }   
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_base64(json_obj_t *parent,const wchar_t *path,const void *value,size_t tam)
{    
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_base64(obj,value,tam)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_bytes(json_obj_t *parent,const wchar_t *path,const void *value,size_t tam)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_bytes(obj,value,tam)==JSON_OK)
            return obj;                
        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }    
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_hexa(json_obj_t *parent,const wchar_t *path,const void *value,size_t num)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_hexa(obj,value,num)==JSON_OK)
            return obj;                
            if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }
    return NULL;
}

/* ------------------------------- */

json_obj_t *json_assign_bignum(json_obj_t *parent,const wchar_t *path,mp_int_t *bn)
{
    json_obj_t *par,*obj;
    int ret=0;

    ret = json_get_object(parent,path,&par,&obj,TRUE);
    if(ret==JSON_OK)
    {
        if(json_set_bignum(obj,bn)==JSON_OK)
            return obj;       

        if(parent)            
        {
            json_detach_obj(parent,obj);        
            json_delete_obj(obj);
        }
        else
        {
            json_delete_obj((par) ? par : obj);
        }
    }
    return NULL;
}

