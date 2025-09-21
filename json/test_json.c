/*
    test_json.c

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

#include "json.h"

/* ------------------------------- */

static int show_object(json_obj_t *obj,int index,json_obj_t *par,int level,void *opaque)
{
    if(obj)
    {
        int t;

        for(t=0;t<level;t++)
            printf("   ");
        
        if(index > -1)
            printf("'%ls[%d]",(par) ? par->name : L"NULL",index);
        else            
            printf("'%ls",obj->name);

        printf("'={%d,",obj->len);

        switch(obj->type)
        {
            case JSON_NULL:
                printf("JSON_NULL,null");
                break;
            case JSON_STRING:
                printf("JSON_STRING,'%ls'",(wchar_t *)obj->value);                    
                break;
            case JSON_NUMBER:
                printf("JSON_NUMBER,%ls",(wchar_t *)obj->value);
                break;
            case JSON_BOOLEAN:
                printf("JSON_BOOLEAN,%ls",(wchar_t *)obj->value);
                break;
            case JSON_OBJECT:
                printf("JSON_OBJECT");                    
                break;
            case JSON_ARRAY:
                printf("JSON_ARRAY");                    
                break;
            default:
                printf("UNKNOWN type %d",obj->type);
                break;
        }
        printf("}\n");            
    }   
    return FALSE; 
}
   
/* -------------------------------------- */

static void trace_json(const wchar_t *title,json_obj_t *root)
{
    if(title)
        printf("%ls\n",title);
    printf("--- START trace_json() ---\n");
    json_traverse_obj(root,(func_json_t)show_object,NULL);
    printf("--- END trace_json()---\n");
}


/* ------------------------------- */

void test_json()
{
    json_obj_t *root, *tmp, *obj, *daddy;
    int ret;
    json_pos_t pos;    
    
    printf("*** Test JSON ***\n");

    root = json_create_obj(NULL,NULL);
    obj = json_create_obj_path(root, L"stuff.test_1",&daddy);   
    tmp = json_create_obj(daddy,L"test_2");      

    trace_json(L"ROOT", root);

    tmp = json_parse_file(L"test_1.json", &pos, &ret);
    if (tmp)
    {
        obj = json_detach_obj(tmp,json_find_obj(tmp,L"certificate",NULL));

        json_delete_obj(tmp);

        if(obj)
        {

            ret = json_add_obj(json_find_obj(root,L"test_1",NULL),obj);
            if(ret==JSON_OK)
            {
                tmp = json_parse_file(L"test_2.json", &pos, &ret);
                if (tmp)
                {
                    obj = json_detach_obj(tmp,json_find_obj(tmp,L"recipes",NULL));

                    json_delete_obj(tmp);

                    ret = json_add_obj(json_find_obj(root,L"test_2",NULL),obj);
                    if(ret == JSON_OK)
                        trace_json(L"AS FILE", root);
                    else 
                        printf("Could not detach 'recipes' (%d) from the file, so cannot add it to 'stuff.test2'\n",ret);           
                    
                }
                else 
                {
                    printf("Error %d in row %d,col %d on JSON file test_2.json\n", ret, pos.row, pos.col);
                }
            }
        }
        else 
        {
            printf("Could not detach 'certificates' (%d)from the file, so cannot add it to 'root'\n",ret);
        }
    }
    else    
    {
        printf("Error %d in row %d,col %d on JSON file test_1.json\n", ret, pos.row, pos.col);
    }    

    daddy = json_create_obj_path(NULL, L"obj0.tmp.obj1",&tmp);
    if (daddy)
    {
        json_add_obj(root, tmp);
        obj = json_create_obj_path(root, L"obj0.tmp.array1[3].last",NULL);               
    }
    else
    {
        printf("Error creating DADDY\n");
    }

    obj = json_create_obj_path(root, L"response.result[0].list[4].characters[4].manuel.films[5]",NULL);

    json_set_string(obj,L"Faulty Towers");
    
    json_assign_string(root, L"response.messages[0]", L"No messages");
    json_assign_string(root, L"response.messages[4]", L"Nothing more to show");

    json_create_obj_path(root, L"response.normal_array[].child",NULL);

    daddy = json_create_obj_path(root, L"stuff.Numbers",NULL);    

    json_insert_obj(daddy, json_create_number_str(NULL, L"-1234.5678"),0);
    json_insert_obj(daddy, json_create_number_str(NULL, L"12345678790"),1);
    json_insert_obj(daddy, json_create_number_str(NULL, L"987654321.009"),2);
    json_insert_obj(daddy, json_create_number_str(NULL, L"9.223372036854775807e+18"),3);
    json_insert_obj(daddy, json_create_number_str(NULL, L"9223372036854775807"),4);
    json_insert_obj(daddy, json_create_number_str(NULL, L"1.8446744073709551615e+19"),5);
    json_insert_obj(daddy, json_create_number_str(NULL, L"18446744073709551615"),6);
    

    daddy = json_create_obj_path(root, L"response[drinks]",NULL);

    json_insert_obj(daddy, json_create_string(L"Drink", L"Cuba Libre"), 2);
    json_insert_obj(daddy, json_create_string(L"Drink", L"Juice"), -1);
    json_insert_obj(daddy, json_create_string(L"Drink", L"Wine"), 8);
    json_insert_obj(daddy, json_create_string(L"Drink", L"Meod"), 1);
   
    obj = json_find_obj(root, L"response.drinks[1]",NULL);
    if (obj)
    {
        printf("Drink == MEOD EXIST\n");        
        trace_json(L"DRINK", obj);
    }
    else
    {
        printf("It didn't exist\n");
    }
    
    obj = json_create_obj_path(root, L"response.     [0]",&daddy);

    json_insert_obj(daddy, json_create_string(NULL, L"My dad is an array,"), 0);
    json_insert_obj(daddy, json_create_string(NULL, L"whose name is a string"), 1);
    json_insert_obj(daddy, json_create_string(NULL, L"filled with spaces."), 2);

    json_set_string(obj,L"And IT WORKS!");



    tmp = json_get_obj(root,L"stuff.Numbers",NULL);
    //last = json_find_obj(root,L"Numbers",NULL);    
    if(tmp)
    {
        int t,num;
        wchar_t tpath[81];
        json_obj_t *prev;

        trace_json(L"Which is an array of numbers",tmp);
        num = json_obj_elements(tmp);
        printf("Numbers have %d elements\n",num);
        for(t=0;t<num;t++)
        {
            swprintf(tpath,81,L"stuff.Numbers[%d]",t);
            printf("Numbers[%d]=%.4f\n",t,json_get_number(root,tpath));
        }
        printf("- Showing as different numbers\n");
        for(t=0;t<num;t++)
        {
            prev = json_get_element(tmp,t);            
            if(prev)
            {       
                printf("  Numbers[%d] is %ls\n",t,json_get_string(prev,NULL));     
                printf("    as number = %e\n",json_get_number(prev,NULL));                
                printf("    as short  = %d\n",json_get_short(prev,NULL));
                printf("    as ushort = %u\n",json_get_ushort(prev,NULL));
                printf("    as int    = %d\n",json_get_int(prev,NULL));
                printf("    as uint   = %u\n",json_get_uint(prev,NULL));
                printf("    as long   = %ld\n",json_get_long(prev,NULL));
                printf("    as ulong  = %lu\n",json_get_ulong(prev,NULL));                
            }
            else
            {
                printf("Numbers[%d]=NULL\n",t);                
            }            
        }
    }
    printf("-oOo- Path with Spaces -oOo-\n");
    printf("json_get_string('stuff.test_2.recipes[2].      ')='%ls'\n",json_get_string(root,L"stuff.test_2.recipes[2].      "));
    printf("-oOo- Archive -oOo-\n");
    json_show_obj(root, stdout);
    printf("-oOo- Complete -oOo-\n");
    if (!json_save(root, L"test_3.json"))
        printf("-oOo- Saved -oOo-\n");
    json_delete_obj(root);
   printf("-oOo- Deleted -oOo-\n");
}

/* -------------------------------------- */

int main(void)
{
    fprintf(stdout,"\n== JSON TEST START \n\n");

    test_json();

    fprintf(stdout,"\n== JSON TEST END\n\n");
}

