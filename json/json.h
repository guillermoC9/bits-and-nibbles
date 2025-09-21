/*
    json.h

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

#ifndef GAO_JSON
#define GAO_JSON

#include "stuff.h"
#include "base64.h"
#include "mpint.h"

#include <float.h>
#include <math.h>

/* ----------------------- *
    JSON Object types 
 * ----------------------- */

enum
{
    JSON_NULL=0,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
};

/* ----------------------- *
    JSON_OBJECTS struct
 * ----------------------- */

typedef struct _jsonO
{
    struct _jsonO *next;       /* Next object in this level */

    int            type;       /* Type of object */
    int		         len;	       /* Lenght of value: (0 if value is NULL, number of chars or number of objects if array or object */
    void          *value;      /* Pointer to the value of the object */
    wchar_t        name[1];    /* Name of object */
} json_obj_t;

/* -------------------------------------- *
   Macros to access object members
 * -------------------------------------- */

#define json_obj_type(_o)            ((_o)->type)
#define json_obj_name(_o)            ((_o)->name)
#define json_obj_elements(_o)        ((_o)->len)
#define json_obj_first_element(_o)   (json_obj_t *)((_o)->value)
#define json_obj_next_element(_o)    ((_o)->next)

/* ------------------------------------------------------------- *
   User function used by the function json_traverse_obj().

   This function would be called once for each of the objects
   enumerated by json_traverse_obj(), and if the function 
   returns a value different from FALSE (0), json_traverse_obj()
   will stop its enumeration and return the object passed to 
   this user function when the function returned non-zero.

  The parameters are:

    'obj'    - The current object being traversed        

    'index'  - The 0-based index of the object being traversed 
               in its parent array. So this will be -1 if not 
               an  array element and 0 or more if it is. 

    'par'    - Parent of the object being traversed    

    'level'  - Object level relative to the object sent to
               json_traverse_obj(): 0=root, 1=child, 
               2=grandchild, 3=great-grandchild, 4,5,6,...
               (really added so you could implement indent 
               and  other stuff like it).

    'opaque' - This pointer is passed untoched to the user
               user function by json_traverse_obj()    

    REFRAIN FROM MODIFIYING 'obj' beyond settign values if
    obj->type is JSON_NULL, JSON_BOOLEAN, JSON_STRING or 
    JSON_NUMBER. Doing somethign else is *VERY* dangerous 
    unless you  *REALLY* know what you are doing. 
    A little cat will cry if you do somethign wrong here.    
 * ------------------------------------------------------------- */

typedef int (*func_json_t)(json_obj_t *obj,int index,json_obj_t *par,int level, void *opaque);

/* Structure of row and column positions */

typedef struct
{
	unsigned int row;	/* Row */
	unsigned int col;	/* Column */
} json_pos_t;

/* --------------------------------------- *
   Max number of chars in a name 
   JSON specs do not set a limit on them
   but we do as there has to be a limit
   that make things maneagebla... ;-)
 * --------------------------------------- */

#define JSON_MAX_NAME   255

/* ----------------- *
      Functions 
* ------------------ */

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------- *
    Parses a JSON encoded with UTF-8 and build the object
    tree, returning the root object.

    If there are parsing errors, the parsing will stop in the
    first one of them and 'pos' will contain the row and
    column in the document where the error was found
    (or the last parsed position if parsing succeeded).

    The first char of the document is at position 0,0.

    Note that if there are errors, the object tree will be
    deleted and NULL returned.

    Use json_delete_obj() on the root object to
    delete the whole tree.

    'doc' is the document to parse.

    'len' is the size in bytes of the document.

    'pos' is an optional (but highly recommended) struct
          pos where the function will return the position
          on the doc where the error -if any- happened

    'err' is an optional (but highly recommended) value that
          returns JSON_OK if worked or the error.

    IMPORTANT

    tabs (both vertical and horizontal) are counted as 1 space
    to avoid misleading results as tab size varies and can be
    transalated to 2, 4, 8 or other number of spaces depending 
    on the software used and the user preferences.
 * ------------------------------------------------------------- */

json_obj_t *json_parse(const void *doc,size_t tam,json_pos_t *p,int *err);


/* ------------------------------------------------------------- *
   Reads a file and calls json_parse() 
 * ------------------------------------------------------------- */

json_obj_t *json_parse_file(const wchar_t *filename,json_pos_t *p,int *err);


/* --------------------------------------------------------------- *
    Parses the content of a file for a complete JSON object tree
    (JSON always describe one object that contains other objects),

    as the previous it returns NULL on errors, but in this case,
    'pos' is relative to the position of the file pointer of 'fp'
    when the function was called.

    WARNING: 

        If errors are found the file pointer will be left on the 
        place where the error was determined (not the position 
        of 'pos').
 * --------------------------------------------------------------- */

json_obj_t *json_parse_FILE(FILE *fp,json_pos_t *p,int *err);

/* ------------------------------------------------------------- *
    Saves the object 'obj' and all its descendants in JSON 
    format encoded in UTF-8 in a file called 'name'.

    return 0 on success or negative or error.
 * ------------------------------------------------------------- */

int json_save(json_obj_t *obj,const wchar_t *name);

/* ------------------------------------------------------------- *
   Same as previous but in a given FILE descriptor 
 * ------------------------------------------------------------- */

int json_save_FILE(json_obj_t *obj,FILE *fp);

/* ------------------------------------------------------------- *
    Macro to simulate a show function in 
 * ------------------------------------------------------------- */

#define json_show_obj(_o,_f)  json_save_FILE(_o,_f)


/* ------------------------------------------------------------- *
    Returns a JSON object given its path, which is relative to 
    the object given.

    This means that if the path is 'config.profiles[3].name' 
    the object 'config' must be a child of the object 'obj',
    an in turn, is an object that has an element called 
    'profiles', which is an array where the fourth element 
    (indexes are 0 based) is also an object that has a child
    element called 'name'.

    Note that the only difference between objects and arrays 
    in JSON (and hence JavaScript) is that object elements 
    have names, whereas array members do not usually have 
    them. 
    
    Basically an array is an object with unnamed children.  

    When I said 'usually they don't have names', I meant that
    you can create a named object and insert it on an array, 
    and the name will be there, although ignored while a
    member of the array. Note that the name will be lost in
    the file when the object is saved.

    If my explanation is not rubbish, and you use a couple of 
    your few thousand brain cells, you'll probably deduct that 
    doing:

          json_get_obj(obj,L"config.profiles[name]",NULL);

    It is the same than doing:

          json_get_obj(obj,L"config.profiles.name",NULL);

    To be honest I prefer to use the second form as the first
    is a way to simulate arrays with named keys, but we all
    know that arrays in JSON have unnamed children, but again,
    that is just MY preference.

    Also, if you omit the cardinal in an array, 0 is assumed and
    this:

        json_get_obj(obj,L"config.profiles[].name",NULL);

    Is the same than this:

        json_get_obj(obj,L"config.profiles[0].name",NULL);

    if 'parent' is not NULL, it receives a pointer to the 
    parent of the returned object.

    This same rules apply to all functions that uses paths 
    in this library.
    
 * ------------------------------------------------------------- */

json_obj_t *json_get_obj(json_obj_t *obj,const wchar_t *path,json_obj_t **parent);

/* ------------------------------------------------------------- *
   Return element in position 'pos' from a JSON object or array 
 * ------------------------------------------------------------- */

json_obj_t *json_get_element(json_obj_t *obj,int pos);

/* ------------------------------------------------------------- *
   This function finds the first object called 'name' in the 
   object 'obj' and all its descendants.

    DO NOT USE PATHS with this function, it will fail and 
    return NULL.

    INCORRECT

        json_find_obj(obj,L"object01.array02.tofind",NULL);

    CORRECT

        json_find_obj(obj,L"tofind",NULL);


    Of course it only works if 'obj' is an array or object.

    if 'parent' is not NULL, it receives a pointer to the 
    parent of the returned object.

* ------------------------------------------------------------- */

json_obj_t *json_find_obj(json_obj_t *obj,const wchar_t *name,json_obj_t **parent);

/* ------------------------------------------------------------- *
   Traverse 'obj' and all its descendants calling 'func' for 
   each of them

   If 'func' returns non-zero, the function will stop the 
   traversing and return the object passed as parameter to 
   'func' when  it returned non-zero.
   
   'opaque' is a pointer that json_traverse_obj() will 
   simply pass to 'func' in order to allow parameter passing
   to 'func'
* ------------------------------------------------------------- */


json_obj_t *json_traverse_obj(json_obj_t *obj,func_json_t func,void *opaque);


/* ------------------------------------------------------------------------ *
    This function returns TRUE if ALL of the given paths exist.

    All the paths are relative to 'obj'.

    'num' is the number of paths given:

    names must be wchar_t * or const wchar_t *

    E.g:

    json_existing_objs(obj,3,L"user.name",L"user.addr",L"user.phone");

    json_existing_objs(json_get_obj(L"user"),3,L"name",L"addr",L"phone");

* ------------------------------------------------------------------------ */

int json_existing_objs(json_obj_t *obj,int num,...);

/* ----------------------------------------------------- *
    Return the value of a JSON object as a string

    If you send 'path' as NULL it will return the 
    value of 'obj' as a string.

    The result varies if object is not a JSON_STRING:

        - JSON_NULL, JSON_OBJECT and JSON_ARRAY will 
        return NULL.

        - JSON_NUMBER will return the original string 
        of the number.

        - JSON_BOOLEAN will return 'false' o 'true'

* ----------------------------------------------------- */

wchar_t *json_get_string(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
    Return the value of a JSON object as a real number

    If you send 'path' as NULL it will return
    the value of 'obj' as a number.

    The result varies if object is not JSON_NUMBER:

        - JSON_NULL, JSON_OBJECT and JSON_ARRAY will 
          return 0.0

        - JSON_STRING works like strtod().

        - JSON_BOOLEAN will return 0.0 for 'false' 
          and 1.0 for 'true'
* ----------------------------------------------------- */

double json_get_number(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
    Same as previous but with various types. 

    Note than if the value is bigger than the returned
    type, it is truncated to fit the size to return, so
    be careful.

    Also remember that JSON numbers do not use octal or
    hexa, so a string contaning a number in any of these 
    formats (not a decimal or real) will result in the 
    incorrect value for the number.    
 * ----------------------------------------------------- */

short int json_get_short(json_obj_t *obj,const wchar_t *path);
unsigned short int json_get_ushort(json_obj_t *obj,const wchar_t *path);

int json_get_int(json_obj_t *obj,const wchar_t *path);
unsigned int json_get_uint(json_obj_t *obj,const wchar_t *path);

long int json_get_long(json_obj_t *obj,const wchar_t *path);
unsigned long int json_get_ulong(json_obj_t *obj,const wchar_t *path);

/* Free result with mp_clear() */

mp_int_t *json_get_big_int(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
   Returns a time_t from a JSON_NUMBER or a JSON_STRING 
   with a number or time format. 
   
   For other types applies the same rules as previous 
   function.

    If it is a JSON_STRING, one of three options are 
    available to define a time stamp:

        - The string has the format 'YYYY-MM-DD HH:MM:SS'
        - The string has the format 'YYYYMMDDHHMMSSZ'
        - The string has a decimal number which is the
          number of seconds since 1970/01/01 00:00:00

    Note that time_t is system time and thus assumed 
    to be UTC. Therefore, the time captured by this 
    functions is assumed to be UTC.
 * ----------------------------------------------------- */

time_t json_get_time(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
    Return the value of a JSON object as a boolean

    If you send 'path' as NULL it will return
    the value of 'obj' as a boolean.

    The result varies if object is not JSON_BOOLEAN:

        - JSON_NULL, JSON_OBJECT and JSON_ARRAY will 
          return FALSE

        - JSON_STRING will return FALSE for an empty 
          string or TRUE if not empty.

        - JSON_NUMBER will return FALSE for 0 or 0.0 
          and TRUE for any other value
 * ----------------------------------------------------- */

int json_get_boolean(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
    Return the value of a JSON_STRING object as an 
    array of bytes

    If you send 'path' as NULL it will return the value 
    of 'obj' as an array of bytes.

    The string must be in one of these hexadecimal 
    formats:

        xxxxxxxx
        xx:xx:xx:xx
        xx,xx, xx,xx
        xx xx xx xx

    It allocates memory for it, put the pointer in 'ret' 
    and return the number of bytes or error (negative).

    'ret' must be freed using free() if not NULL     
 * ----------------------------------------------------- */

int json_get_bytes(json_obj_t *obj,const wchar_t *path,void **ret);

/* ----------------------------------------------------- *
   Returns a bigint from an hexadecimal string in the 
   same formats accepted by json_get_bytes().

   Free it with mp_clear()
 * ----------------------------------------------------- */

mp_int_t *json_get_bignum(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
   Same as previous but using base64 instead of 
   hexadecimal. Note
 * ----------------------------------------------------- **/

int json_get_bytes_base64(json_obj_t *obj,const wchar_t *path,void **ret);

mp_int_t *json_get_bignum_base64(json_obj_t *obj,const wchar_t *path);

/* ----------------------------------------------------- *
    Deletes an object and all its children and 
    descendants.

    WARNING

    This is intended for root objects, so be very 
    CAREFUL if you use it with other objects, as if 
    you  delete an object and its parent has a reference 
    to it, you may end up in DEEP TROUBLE.
* ----------------------------------------------------- */

void json_delete_obj(json_obj_t *obj);


/* ----------------------------------------------------- *
    Detach object 'child' from 'obj' if 'obj' is an
    ancestor of 'child'.

    Basically removes 'child' from wherever is in obj's
    tree. 
    
    IMPORTANT: if 'child' is detached from 'obj', all 
    its descendants are bound to it, so they are all 
    detached from the tree as well, making 'child' a 
    JSON tree of its own.

    If successful ('child' is detached from 'obj') it
    will return 'child'. If not, it will return NULL.

    Correctly deleting a branch:

    child = json_get_obj(root,L"object0.array1[2].config",&parent);
    if(child)
    {
        if(json_detach_obj(parent,child))        
            json_delete_obj(child);
    }  

    Lazy version:

    child = json_get_obj(root,L"object0.array1[2].config",NULL);    
    if(json_detach_obj(root,child))        
        json_delete_obj(child);

 * ----------------------------------------------------- */

json_obj_t *json_detach_obj(json_obj_t *parent,json_obj_t *child);

/* ----------------------------------------------------- *
    Add an object to another object, array or NULL 
    object.

    'parent' is the object to which add 'child', and 
    like the rest, the function behaves differently 
    depending on the type of object of it.

    If 'parent' is a JSON_ARRAY, the call would be 
    like calling json_insert_obj(parent,child,parent->len)        
    
    If it is a JSON_OBJECT then 'child' is added to 
    the end of the children list EXCEPT if there is 
    another child object with the same name, in which
    case the function returns JSON_EEXIST.

    If it is JSON_NULL, it will be converted to a type 
    JSON_OBJECT and treated as such.    

    If 'parent' is something else, 'child' will not be
    added and the function will return JSON_EINVAL.

    'parent' cannot be NULL and the function will 
    return JSON_EINVAL if it is.    
* ----------------------------------------------------- */

int json_add_obj(json_obj_t *parent,json_obj_t *child);

/* ----------------------------------------------------- *
    Inserts a JSON object in a given position in an 
    array, object or NULL

    If 'parent' is JSON_ARRAY, 'child' will be inserted 
    in the position 'pos' unless 'pos' is out of range.   
      
    In this case it will be inserted at the start of
    the array if 'pos' is less than 1 (remember arrays
    are zero based), or added to the end if 'pos'
    is bigger than the array's range.

    If it is a JSON_NULL, it will be converted to
    JSON_ARRAY and treated as such. 

    It it is JSON_OBJECT it will work like a call to 
    json_add_obj(), which will involve a check to see
    if an object with the name of 'child' exists among
    the children of 'parent' and fail if does.

    If 'parent' is something else is not inserted and
    will return JSON_EINVAL.

    'parent' cannot be NULL and the function will 
    return JSON_EINVAL if it is.

    VERY IMPORTANT!!!!

    The name of the object 'child' will not be erased
    but will be ignored as is the case for all children 
    of arrays. 
    
    Therefore, two objects with the same name will be 
    added to a JSON_ARRAY, but will make the function 
    fail if 'parent' is a JSON_OBJECT  
* ----------------------------------------------------- */

int json_insert_obj(json_obj_t *parent,json_obj_t *child,int pos);


/* ----------------------------------------------------- *
   Create JSON objects and arrays (do not use paths 
   for 'name', as they will fail) and add them to 
   'parent'.

   Send 'parent' to NULL to create a standalone 
   object or array

   If 'parent' already has an object of any kind called 
   'name', that object is returned instead of a new 
   one created. 

   Therefore you should check the type of the returned 
   object to make sure it is what you need.

   This behaviour is different to the functions 
   json_parse(), json_add_obj() and json_insert_obj() 
   which all fail for duplicate names in objects.

   The reason for such difference is due to the fact
   that in the former, allowing duplicates to replace
   existent objects will cause data loss (the previous
   values or children), whereas here nothing is lost as 
   the existent object will be returned not replaced,
   and if it was created, the new object will be empty
   anyway.
* ----------------------------------------------------- */

json_obj_t *json_create_obj(json_obj_t *parent,const wchar_t *name);
json_obj_t *json_create_array(json_obj_t *parent,const wchar_t *name);


/* --------------------------------------------------------------------- *
    Creates all the objects in a path if they do not 
    exist and return the last one. Any intermediate 
    object would be created as JSON_OBJECT except if 
    the name includes brackets, where it would be 
    created as an array with the needed number of 
    elements to make the  path.

    Eg.

    json_create_obj_path(root,'obj.array1[3].last',&parent);

    will create a child of root as JSON_OBJECT called 
    'obj', that will have just one child which is an 
    array with 4 elements, where elements 0, 1 and 2 
    would be JSON_NULL and element 3 will be JSON_OBJECT,
    an intermediate for 'last', which will be created as 
    JSON_NULL and returned. 

    If not NULL, 'parent' will contain 'obj.array1[3]'.

    It will return NULL if there is not memory, there 
    is a syntax error in the path or simply because one 
    of the intermediate elements in the path exists but 
    it cannot have children (e.g. is a JSON_STRING).

    If the whole path exist, this function behaves just 
    like a call to json_get_obj() and return the object
    and its parent -if 'parent' is not NULL.

    WARNING !!! 

    If you send 'obj' as NULL, it will create the full
    path and return the root object in 'parent' instead 
    of the parent object, so make sure you send it or 
    memory leak will occur as you will not have a way
    to recover the object tree just created, only the 
    last object, and only if it was specified too.
           
    Examples:

    obj1 = json_create_obj_path(NULL,'obj0.tmp.obj1',&root);

        - Will create all of them, return 'obj1' and put 
         'obj0' (root) in 'root'.

        - The resulting tree for call will be:

            obj0 -> tmp -> obj1

    last = json_create_obj_path(root,'obj0.tmp.array1[3].last',&parent);

        - Will create everything from 'array[3]' as child of 
         'tmp' (created in the previous call) and return 'last'. 
         'parent' will contain  a pointer to 'array[3]'.

        - The resulting tree for this call will be:

          root -> obj0 -> tmp -> obj1
                                 array[0]-> null
                                      [1]-> null
                                      [2]-> null
                                      [3]-> last

 * --------------------------------------------------------------------- */

json_obj_t *json_create_obj_path(json_obj_t *obj,const wchar_t *path,json_obj_t **parent);

/* ----------------------------------------------------- *
    Set the value of an object from a string. 
    
    It behaves differently depending on the type of 
    object of 'obj':

        - JSON_STRING will take any string (NULL will 
          assign "").

        - JSON_NUMBER behaves like strtod() (anything 
          not a number generate string "0").

        - JSON_BOOLEAN will generate 'true' or 'false'. 
          Any case insesitive 'true' will be accepted, 
          and NULL or anything other than 'true' will 
          generate 'false'.

    If 'obj' is JSON_NULL, it becomes JSON_STRING.

    If 'obj' is JSON_ARRAY or JSON_OBJECT, it does 
    nothing and returns JSON_EINCORRECT.

    will return JSON_EINVAL if bad parameters or 
    JSON_ENOMEM for memory errors

 * ----------------------------------------------------- */

int json_set_string(json_obj_t *obj,const wchar_t *value);

/* ----------------------------------------------------- *
    Set the value of an object from an UTF-8 string. 

    It behaves differently depending on the type of 
    object of 'obj':

        - JSON_STRING Will take any string (NULL will 
          assign "").

        - JSON_NUMBER Behaves like strtod() (anything 
          not a number generate string "0").

        - JSON_BOOLEAN will generate 'true' or 'false'. 
          Any case insesitive 'true' will be accepted, 
          and NULL or anything other than 'true' will 
          generate 'false'.

    If 'obj' is JSON_NULL, it becomes JSON_STRING.

    If 'obj' is JSON_ARRAY o JSON_OBJECT, it does nothing 
    and returns JSON_EINCORRECT.

    will return JSON_EINVAL if bad parameters or 
    JSON_ENOMEM for memory errors

 * ----------------------------------------------------- */

int json_set_utf8(json_obj_t *obj,const char *value);

/* ----------------------------------------------------- *
    Set the value of an object from a number. 
    
    It behaves differently depending on the type of 
    object of 'obj':

        - JSON_NUMBER and JSON_STRING Will convert the 
          number to string JSON_BOOLEAN will generate 
          'false' for 0,0 or 'true' for anything else

    If JSON_NULL, it will become JSON_NUMBER.

    If 'obj' is JSON_ARRAY o JSON_OBJECT, it does 
    nothing and returns JSON_EINCORRECT.

    will return JSON_EINVAL if bad parameters or 
    JSON_ENOMEM for memory errors
 * ----------------------------------------------------- */

int json_set_number(json_obj_t *obj,double value);

/* ----------------------------------------------------- *
   Set a time_t value using the same rules for the 
   previous function but with one variant:
   
   'fmt' define the resulting object:

        2 - JSON_STRING like 'YYYY-MM-DD HH:MM:SS'
        1 - JSON_STRING like 'YYYYMMDDHHMMSSZ'
        0 - JSON_NUMBER with seconds since Unix epoch       

    The macros ease the choice

 * ----------------------------------------------------- */

#define JSON_UNIX_TIME     0
#define JSON_TIME_STAMP    1
#define JSON_TIME_DB       2

int json_set_time(json_obj_t *obj,time_t value,int fmt);

/* ----------------------------------------------------- *
    Set the value of an object from a boolean. 
    It behaves differently depending on the type of 
    object of 'obj':

        - JSON_BOOLEAN will take the boolean value

        - JSON_STRING will take 'true' or 'false' like 
          json_set_string()

        - JSON_NUMBER Will generate 0 for FALSE and 
          1 for TRUE.

    If 'obj' is JSON_NULL, it becomes JSON_BOOLEAN.

    If 'obj' is JSON_ARRAY o JSON_OBJECT, it does nothing 
    and returns JSON_EINCORRECT.

    will return JSON_EINVAL if bad parameters or 
    JSON_ENOMEM for memory errors
* ----------------------------------------------------- */

int json_set_boolean(json_obj_t *obj,int value);

/* ----------------------------------------------------- *
    BE CAREFUL with this!!!!!

    Convert ANY object to JSON_NULL deleting its value.

    If 'obj' is JSON_ARRAY or JSON_OBJECT, it will delete
    all children and descendants -if any- by calling
    json_delete_obj() in all of them.

    will return JSON_EINVAL if bad parameters or 
    JSON_ENOMEM for memory errors
* ----------------------------------------------------- */

int json_set_null(json_obj_t *obj);

/* ----------------------------------------------------- *
    Set a binary group of bytes as a base64 string.
    The 'obj' ends as JSON_STRING if anything
    other than JSON_OBJECT or JSON_ARRAY, which
    will return JSON_EINCORRECT.
 * ----------------------------------------------------- */

int json_set_base64(json_obj_t *parent,const void *value,size_t tam);

/* ----------------------------------------------------- *
    Set a binary group of bytes as an hexa string 
    like 2fb00...
    
    The 'obj' ends up as JSON_STRING if it is anything
    other than JSON_OBJECT or JSON_ARRAY.
    
    The function will return JSON_EINCORRECT if it is
    one fo them.
 * ----------------------------------------------------- */

int json_set_bytes(json_obj_t *parent,const void *value,size_t tam);

/* ----------------------------------------------------- *
    Set a binary group of bytes as an hexa string 
    like xx:xx:xx:xx
    
    The 'obj' ends as JSON_STRING if anything
    other than JSON_OBJECT or JSON_ARRAY.

    The function will return JSON_EINCORRECT if it is
    one fo them.
 * ----------------------------------------------------- */

int json_set_hexa(json_obj_t *parent,const void *value,size_t num);

/* ----------------------------------------------------- *
    Set a bignum as an hexa string like xx:xx:xx:xx
    The 'obj' ends as JSON_STRING if anything
    other than JSON_OBJECT or JSON_ARRAY. 
    
    The function will return JSON_EINCORRECT if it is
    one fo them.
 * ----------------------------------------------------- */

int json_set_bignum(json_obj_t *parent,mp_int_t *bn);

/* ----------------------------------------------------- *
    Set a bignum as a big number in decimal stored as 
    a string

    The 'obj' ends as JSON_STRING if anything
    other than JSON_OBJECT or JSON_ARRAY. 
    
    The function will return JSON_EINCORRECT if it is
    one fo them.
 * ----------------------------------------------------- */

 int json_set_big_int(json_obj_t *parent,mp_int_t *bn);

/* ----------------------------------------------------- *
    Add/assign values of various types to and object
    path relative to 'obj', which CAN be NULL if you 
    want to create a standalone object.

    If the object exist, they set the value and return 
    the object or NULL for errors.

    If it doesn't, the functions will create it and
    return a pointer to it.
    
    The reality is that they set a value to an object 
    pointed to by 'path', creating all the objects in 
    the path if they do not exist. 

    It returns a pointer to the newly created object 
    or NULL if an error occurs.

    IMPORTANT: 

    The new object path will be added to obj's  child
    list, but only if 'obj' is JSON_OBJECT, JSON_ARRAY
    or NULL. Otherwise will not create the object and 
    return NULL.    

    All json_assign...() will create a standlone 'obj' 
    if NULL 
        
    Although they use more efficient code, they just 
    basically do this:

        obj = json_create_obj_path(obj,name,NULL);
        if(obj)
            json_set...(obj,value);
        return obj;

    BE EXTRA CAREFUL with json_assign_null() to avoid 
    deleting existing stuff. look json_set_null() for    
    details. 
 * ----------------------------------------------------- */

json_obj_t *json_assign_string(json_obj_t *obj,const wchar_t *path,const wchar_t *value);
json_obj_t *json_assign_utf8(json_obj_t *obj,const wchar_t *path,const char *value);
json_obj_t *json_assign_number_str(json_obj_t *obj,const wchar_t *path,const wchar_t *value);
json_obj_t *json_assign_number(json_obj_t *obj,const wchar_t *path,double value);
json_obj_t *json_assign_boolean(json_obj_t *obj,const wchar_t *path,int value);
json_obj_t *json_assign_null(json_obj_t *obj,const wchar_t *path);
json_obj_t *json_assign_base64(json_obj_t *obj,const wchar_t *path,const void *value,size_t tam);
json_obj_t *json_assign_bytes(json_obj_t *obj,const wchar_t *path,const void *value,size_t tam);
json_obj_t *json_assign_hexa(json_obj_t *obj,const wchar_t *path,const void *value,size_t num);
json_obj_t *json_assign_bignum(json_obj_t *obj,const wchar_t *path,mp_int_t *bn);

/* ----------------------------------------------------- *
   These are more readble than json_assign...(NULL,...) 
 * ----------------------------------------------------- */

#define json_create_string(_n,_v)        json_assign_string(NULL,_n,_v)
#define json_create_utf8(_n,_v)          json_assign_utf8(NULL,_n,_v)
#define json_create_number(_n,_v)        json_assign_number(NULL,_n,_v)
#define json_create_number_str(_n,_v)    json_assign_number_str(NULL,_n,_v)
#define json_create_boolean(_n,_v)       json_assign_boolean(NULL,_n,_v)
#define json_create_null(_n)             json_assign_null(NULL,_n)
#define json_create_base64(_n,_v,_l)     json_assign_base64(NULL,_n,_v,_l)
#define json_create_bytes(_n,_v,_l)      json_assign_bytes(NULL,_n,_v,_l)
#define json_create_hexa(_n,_v,_l)       json_assign_hexa(NULL,_n,_v,_l)
#define json_create_bignum(_n,_v,_l)     json_assign_bignum(NULL,_n,_v,_l)


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

#define JSON_OK             0    /* Zero its mostly 'Ok' int the standard C library */
#define JSON_EINVAL        -1    /* Invalid argument passed */
#define JSON_ENOMEM        -2    /* No enough memory */
#define JSON_ERANGE        -3    /* Range error */
#define JSON_EFIND         -4    /* Not found */
#define JSON_ECREAT        -5    /* Creation error */
#define JSON_EOPEN         -6    /* Open error */
#define JSON_EREAD         -7    /* Read error */
#define JSON_EWRITE        -8    /* Write error */
#define JSON_EBUG          -9    /* POSSIBLE BUG */
#define JSON_ESYNTAX      -10    /* Syntax error */
#define JSON_EMISSING     -11    /* Something is Missing */
#define JSON_ECORRUPT     -12    /* Corrupted data */
#define JSON_EINCORRECT   -13    /* Object is of incorrect type */
#define JSON_ENOIMPL      -14    /* Not implemented */
#define JSON_ENODATA      -15    /* No data to return */
#define JSON_EFULL        -16    /* It is complete */
#define JSON_EEXIST       -17    /* It already exists */


#ifdef __cplusplus
};
#endif



#endif
