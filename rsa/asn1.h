/*
    asn1.h

    ASN.1 routines

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


        ABOUT THE IMPLEMENTATION...

           I originally wrote these routines to handle X509
           certificates encoded using DER (Distinguished
           Encoding Rules), but then learned that LDAP uses
           a subset of BER (Basic Encoding Rules) compatible
           with DER, so now I also use some of them for handing
           LDAP.

           References:

               http://luca.ntop.org/Teaching/Appunti/asn1.html
               http://www.vijaymukhi.com/vmis/ber.htm


          Please note that in here we use int instead size_t
          for the sizes because it will unnecesarily increase
          the sizes of ASN.1 structs and data as these days
          size_t is mostly compiled as 64 bits an the ASN.1
          is a byte oriented encoding.

                                --oO0Oo--


*/

#ifndef ASN_1_ROUTINES
#define ASN_1_ROUTINES

#include "ticks.h"
#include "oid.h"
#include "mpint.h"

#define ASN1_END         0x00    /** Final */
#define ASN1_BOOL        0x01    /** Boolean */
#define ASN1_INT         0x02    /** Integer */
#define ASN1_BITS        0x03    /** String of bits */
#define ASN1_BYTES       0x04    /** Strign of bytes */
#define ASN1_NULL        0x05    /** a NULL element */
#define ASN1_OID         0x06    /** Object Identifier */
#define ASN1_ENUM        0x0a    /** Enumeration */
#define ASN1_UTF8        0x0C    /** UTF8String */
#define ASN1_PRINTABLE   0x13    /** PrintableString */
#define ASN1_TELETEXT    0x14    /** TeletextString */
#define ASN1_IA5_STR     0x16    /** IA5String */
#define ASN1_UTC         0x17    /** UTC */
#define ASN1_TIME        0x18    /** Generic Time */
#define ASN1_BMP_STR     0x1E    /** BMPString (Unicode String 0-ffff) */
#define ASN1_SEQ         0x30    /** Ordered Sequence */
#define ASN1_SET         0x31    /** Un-orderer Sequence */

/** Bits of the BER/DER identifier */

#define ASN1_BER_ID(_x)      ((_x) & 0xC0)
#define ASN1_BER_TYPE(_x)    ((_x) & 0x20)
#define ASN1_BER_ID_TYPE(_x) ((_x) & 0xE0)

#define ASN1_UNIVERSAL(_x)   (ASN1_BER_ID(_x) == 0x00)
#define ASN1_APPLICATION(_x) (ASN1_BER_ID(_x) == 0x40)
#define ASN1_CONTEXT(_x)     (ASN1_BER_ID(_x) == 0x80)
#define ASN1_PRIVATE(_x)     (ASN1_BER_ID(_x) == 0xC0)

#define asn1_implicit(_x)    ((ASN1_BER_ID_TYPE((_x)->type) == 0x80) ? ((_x)->type & 0x1F) : -1)
#define asn1_explicit(_x)    ((ASN1_BER_ID_TYPE((_x)->type) == 0xA0) ? ((_x)->type & 0x1F) : -1)

#define asn1_choice(_x)      ((_x) & 0x1F)
#define asn1_optional(_x)    ((_x) & 0x1F)

#define ASN1_IMPLICIT(_x)    (unsigned char)(0x80|((_x) & 0x1F))
#define ASN1_EXPLICIT(_x)    (unsigned char)(0xA0|((_x) & 0x1F))

#define ASN1_IMPLICIT_TYPE(_x) (ASN1_BER_ID_TYPE(_x) == 0x80)
#define ASN1_EXPLICIT_TYPE(_x) (ASN1_BER_ID_TYPE(_x) == 0xA0)


/* -------------------------------------------------- *

  A CONFUSING SUBJECT...

    Implicit and explicit fields are used to determine which fields are
    being handled when dealing with optional fields.

    Implicit fields have the context bit (ASN1_CONTEXT) active so they
    can be identified.

    For example, a bit string is defined like this:

        bits  ::=  BIT STRING

    But an implicit bit string is like this:

        bits   ::= [1] IMPLICIT BIT STRING

    The difference between the two is that the first one would have a tag
    ASN1_BITS and the second a tag ASN1_IMPLICIT(1). But both of
    them has to be read as bit strings.

    The reason why they are defined differently is to allow us to tell one
    field from another when dealing with optional fields. For example:

        versionNUM    ::= INTEGER        
        subjectUID    ::= [1] IMPLICIT BIT STRING OPTIONAL
        somethingElse ::= [2] IMPLICIT BIT STRING OPTIONAL
        extensions    ::= [3] IMPLICIT SEQUENCE { BIT STRING } OPTIONAL

    If after reading the versionNUM -which is compulsory- we find a tag
    ASN1_IMPLICIT(1), we will know this field is subjectUID, but if we
    find a ASN1_IMPLICIT(2) then we will know that it is somethingElse
    and not subjectUID.

    It would not be possible to find out which one is which if they were
    both defined like this:

        versionNUM    ::= INTEGER
        subjectUID    ::= BIT STRING OPTIONAL
        somethingElse ::= BIT STRING OPTIONAL
        extensions    ::= SEQUENCE { BIT STRING } OPTIONAL

    Explicit is used like implicit to assign a number to a field so it
    can be explicitly recognized when made optional, while having a
    default value if its not present. 
    
    Therefore this:

        versionNUM    ::= INTEGER
        subjectUID    ::= [1] IMPLICIT BIT STRING OPTIONAL
        somethingElse ::= [2] IMPLICIT BIT STRING OPTIONAL
        extensions    ::= [3] IMPLICIT SEQUENCE { BIT STRING } OPTIONAL

    Would become this if you want versionNUM to be optional:

        versionNUM  ::= [0] EXPLICIT INTEGER DEFAULT 0,
        issuerUID   ::= [1] IMPLICIT BIT STRING OPTIONAL
        subjectUID  ::= [2] IMPLICIT BIT STRING OPTIONAL
        extensions  ::= [3] EXPLICIT BIT STRING OPTIONAL

    Explicit encapsulates the value or values in a sequence so the
    encoder does not have to specify a sequence (ASN1_SEQ) but
    use a tag ASN1_EXPLICIT(x) instead simplifying the specs.

    This is particularly good if the only element is in fact a sequence,
    as it will save putting two ASN1_SEQ tags in a row like happens
    in real life x509 certificates, which have this as part of their
    definition:

        TBSCertificate  ::=  SEQUENCE
        {
            version         [0]  EXPLICIT Version DEFAULT v1,

            serialNumber         CertificateSerialNumber,
            signature            AlgorithmIdentifier,
            issuer               Name,
            validity             Validity,
            subject              Name,
            subjectPublicKeyInfo SubjectPublicKeyInfo,

            issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
                                 -- If present, version MUST be v2 or v3

            subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
                                 -- If present, version MUST be v2 or v3

            extensions      [3]  EXPLICIT Extensions OPTIONAL
                                 -- If present, version MUST be v3
        }

    Like 'extensions' is defined as sequence of sequences, you know
    you have to handle the sequences directly instead guessing.

    Extensions is actually defined like this:

        Extensions  ::=  SEQUENCE SIZE (1..MAX) OF Extension

        Extension  ::=  SEQUENCE
        {
            extnID      OBJECT IDENTIFIER,
            critical    OPTIONAL BOOLEAN DEFAULT FALSE,
            extnValue   OCTET STRING
        }

    And version identifier like:

        Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }

    So in this case you know that if you read a ASN1_EXPLICIT(0) tag
    you are dealing with the version information and if you read something
    else the version is not there and the version is 0 (v1).

    And if you get a tag ASN1_IMPLICIT(1) or ASN1_IMPLICIT(2) you know
    they are the unique identifiers and that if you get ASN1_EXPLICIT(3)
    are the extensions, even if you did not get any of the previous 3.

    If instead they were specified as BIT STRING or SEQUENCE or whatever
    they are, you will not be able to tell them apart and they will all have
    to be specified.

    I am really grateful to Thomas Pornin for explaining the concept of 
    explicit and implicit to somebody else in the Google mailing list:

           https://groups.google.com/forum/#!topic/sci.crypt/oTBrXmWrvIY

    This made me understand the concept and I then could implement my own x509
    certificates parsing routines.

    Free ASN.1 book -> http://www.oss.com/asn1/resources/books-whitepapers-pubs/asn1-books.html
    
 * -------------------------------------------------- */

/** Element ASN.1 */

typedef struct
{
    int            type;   /* Type of element */
    unsigned char *data;   /* Pointer to the data of the element */
    int            len;    /* Lenght of 'data' in bytes */

    unsigned char *start;  /* Pointer to the start of the element */
    int            total;  /* Lenght in bytes of the whole element */

    int             max;    /* Max number of bytes of the buffer
                              (0 means that the buffer is static) */
    int             dyn;    /* TRUE/FALSE the struct us dynamic */
} asn1_t;


#define ASN1_INIT    {ASN1_NULL,NULL,0,NULL,0,0,NO}

/** Sequence */

typedef struct
{
    int         num;    /* Number of elements in the sequence */
    asn1_t  *lst;    /* List of elements in the sequence */
} asn1_seq_t;


/** Functions */

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- *
    Encode an integer in the range 0-0xffffffff in
    ASN.1 format, storing it in 'buf' and returning
    the number of bytes used to codify it.

    If 'max' is less than 5 (máximum number of bytes
    needed to codify an integer) then the function 
    returns -1

    If 'buf' is NULL returns the number of bytes needed
    to codify the number
 * -------------------------------------------------- */

int asn1_encode_uint(void *buf,int max,unsigned int val);

/* -------------------------------------------------- *
    Same as previous but the format is actually that
    of a BIT STRING (instead INT) returning the number 
    of bytes used to codify it.

    If 'max' is less than the number of bytes needed 
    to codify the integer as BIT STRING it returns -1

    If 'buf' is NULL returns the number of bytes needed
    to codify the number
 * -------------------------------------------------- */

int asn1_encode_uint_bits(void *buf,int max,unsigned int val);

/* -------------------------------------------------- *
   Read the type and size of an ASN.1 element and
   return the number of bytes read.
 * -------------------------------------------------- */

int asn1_read_type_and_len(int *type,int *len,const void *ptr,int max);

/* -------------------------------------------------- *
    Return the number of bytes needed to write the
    lenght of an ASN.1 element in the correct
    encoding as per the specs

   'len' is the size in bytes of the element
* -------------------------------------------------- */

int asn1_bytes_of_len(unsigned int len);

/* -------------------------------------------------- *
    Write the size of anASN.1 element using the
    correct encoding as per the specs

    Make sure that 'dest' is not NULL and can hold
    at least 5 bytes.
 * -------------------------------------------------- */

int asn1_put_len(unsigned int len,void *dest);

/* -------------------------------------------------- *

    Returns the correct size for writing an ASN.1
    element in the correct format as per specs.

    'type' is type of  element.
    'len'  is the size of the element or ignored if irrelevant.

    Here is the list:

        ASN1_END         ('len' is ignored and use 0 as this element has no data)
        ASN1_BOOL        ('len' is ignored and we use 1)
        ASN1_INT         ('len' the number of bytes of the integer)
        ASN1_BITS        ('len' the number of BITS of the integer)
        ASN1_BYTES       ('len' is the size in bytes)
        ASN1_NULL        ('len' is ignored like ASN1_END)
        ASN1_OID         ('len' is the size in bytes of oid)
        ASN1_ENUM        ('len' is the size in bytes of a enum)
        ASN1_UTF8        ('len' is the size in bytes of a UTF-8 string)
        ASN1_PRINTABLE   ('len' is the size in bytes of the string)
        ASN1_TELETEXT    ('len' is the size in bytes of the string)
        ASN1_IA5_STR     ('len' is the size in bytes of the string)
        ASN1_UTC         ('len' is ignored and 13 bytes used)
        ASN1_TIME        ('len' is ignored and 15 bytes used)
        ASN1_BMP_STR     ('len' is the size in bytes of the string)
        ASN1_SEQ         ('len' is the size in bytes of the sequence)
        ASN1_SET         ('len' is the size in bytes of the set)

    If 'data' is not NULL then it will return the len of the data
    excluding the type and len bytes.

 * -------------------------------------------------- */

int asn1_calc_len(int type,unsigned int len,int *data);

/* -------------------------------------------------- *
    Read an ASN.1 element encoded in DER from a
    given buffer and return the number of bytes
    read from the buffer.

    WARNING:

       This function expects an uninitalized static
       asn1_t structure and makes the pointers
       inside the struct to point to bytes inside
       'data' so there isn't need to call asn1_free()
       to free the data on the struct.

       Just make sure 'data' still exists and have 
       not changed when you use the struct.
       .
 * -------------------------------------------------- */

int asn1_read(asn1_t *asn,const void *data,int len);

/* -------------------------------------------------- *
   Read a sequence (ASN1_SEQ o ASN1_SET) of
   ASN.1 elements.

   This function reinitializes 'seq' and allocates
   memory for the sequence, so call asn1_seq_free()
   when you do not need it.

 * -------------------------------------------------- */

int asn1_seq_read(asn1_seq_t *seq,const void *data,int len);

/* -------------------------------------------------- *
   Cleans up 'seq' and free any resources it have
   allocated
 * -------------------------------------------------- */

void asn1_seq_free(asn1_seq_t *seq);

/** ---------------------------------------------- *
             Convert from ASN.1
 * ---------------------------------------------- */

/* Convert ASN1_INT, ASN1_BITS o ASN1_BYTES to unsigned int  */

int asn1_to_uint(asn1_t *asn, unsigned int *val);

/* Convert a time element (ASN1_UTC o ASN1_TIME) to time_t */

int asn1_to_time(asn1_t *asn, time_t *val);

/* Convert ASN1_BOOL to int */

int asn1_to_bool(asn1_t *asn, int *val);

/* Convert ASN1_INT, ASN1_BITS o ASN1_BYTES to a large integer */

int asn1_to_bignum(mp_int_t *mp,asn1_t *asn);

/* Convert an ASN1_OID to char */

int asn1_to_oid(asn1_t *asn, char *oid, int max);

/* Convert an ASN1_OID to wchar */

int asn1_to_oidw(asn1_t *asn, wchar_t *oid, int max);

/* -------------------------------------------------- *
   Converts an ASN.1 block of len bytes to the ASN.1
   descriptive text format.

   'level' is the indentation level so this function
   can be used to convert a whole ASN.1 into a
   document by calling itself recursively
 * -------------------------------------------------- */

 int asn1_to_text(FILE *fp,int level,const void *data,int len);


/* -------------------------------------------------- *
   Read an ASN.1 element in a string.

   If the element is not any kind of string, it does
   the following conversion:

    ASN1_BOOL = "true" or "false"
    ASN1_INT  = Decimal representation of the
                   number or its hexadecimal one
                   if the number is too long to
                   fit in the buffer.

    ASN1_NULL = "NULL"
    ASN1_OID  = String versions of the OID
    ASN1_UTC  = "YYYYMMDDHHMMSS" in UTC
    ASN1_TIME = "YYYYMMDDHHMMSS" in UTC


    ASN1_UTF8, ASN1_PRINTABLE, 
    ASN1_TELETEXT, ASN1_IA5_STR
    and ASN1_BMP_STR are converted to string 

    Any other would be converted to ASN.1 codes in
    numeric format as RFC-2253 dictates, that is '#'
    followed by the codes in hexa:

        #0500   - This is an ASN1_NULL
        #040100 - This is an ASN1_BYTES with
                  a single by with 0 value
 * -------------------------------------------------- */

int asn1_to_string(asn1_t *asn,char *dest,int max);

/* Same as previous but wchar_t */

int asn1_to_wcs(asn1_t *asn,wchar_t *dest,int max);

/** Read a ASN1 element in the numeric format of rfc2253 #xxxxxxxx */

int asn1_to_rfc2253(asn1_t *asn,char *dest,int max);

/** Same as previous but wchar_t */

int asn1_to_rfc2253w(asn1_t *asn,wchar_t *dest,int max);

/* -------------------------------------------------- *
   Read an key = value pair from an element 'asn'
   which MUST be of type ASN1_SET.
 * -------------------------------------------------- */

int asn1_to_attribute(asn1_t *asn,char *oid,int omax,char *val,int vmax);
int asn1_to_attributew(asn1_t *asn,wchar_t *oid,int omax,wchar_t *val,int vmax);

/* -------------------------------------------------- *
   Same as previous but uses just one string buffer
   and the returned string will be like "key = value"
   As liek the previous 'asn' MUST be ASN1_SET
 * -------------------------------------------------- */

int asn1_to_whole_attribute(asn1_t *asn,char *val,int max);
int asn1_to_whole_attributew(asn1_t *asn,wchar_t *val,int max);

/* -------------------------------------------------- *
    CONVERT TO A ASN.1
   -------------------------------------------------- *

    Creates a ASN.1 buffer of type 'type' and 'len'
    bytes of size.

    IMPORTANT: 'len' really depends on 'type' so
    look at the description of asn1_calc_len()
    above to know more details.

    Basically this function creates an ASN.1 struct
    already formated with enough space to copy
    'len' bytes on it by putting them into ans->data
    the data for that type of struct.

    You can see examples of it in ans1.c, in
    functions like asn1_create_bignum() and
    asn1_create_uint().

    Use asn1_free() to free it when not needed
*/

asn1_t *asn1_create(int type,int len);

/* -------------------------------------------------- *
   increase the space in the data buffer of an ASN.1
   struct by adding 'len' to the size of the data
   and returning a pointer where this new data can
   be copied to abe added to the previous.
 * -------------------------------------------------- */

unsigned char *asn1_make_space(asn1_t *asn,int len);

/* -------------------------------------------------- *
   Adds data to a data buffer of an ASN.1 struct.

   Basically is like calling the previous function
   followed of a memcpy() but  alittle faster.
 * -------------------------------------------------- */

int asn1_add_data(asn1_t *asn,const void *dat,int len);

/* -------------------------------------------------- *
   Frees any data of the struct and the struct itself
   if its dynamically obtaned with asn1_create()
 * -------------------------------------------------- */

void asn1_free(asn1_t *asn);

/* -------------------------------------------------- *
   Creates an ASN1_NULL element
 * -------------------------------------------------- */

asn1_t *asn1_create_null(void);

/* -------------------------------------------------- *
  Creates a ASN1_EXPLICIT(n) 
* -------------------------------------------------- */

asn1_t *asn1_create_explicit(int num,const void *bytes,int len);

/* -------------------------------------------------- *
   Converts the data en 'bytes' to an ASN1_BYTES
   element
 * -------------------------------------------------- */

asn1_t *asn1_create_bytes(const void *bytes,int len);

/* -------------------------------------------------- *
   Converts the data in 'bits' to an ASN1_BITS
   element

    IMPORTANT: 'numbits' its the size of 'bits' in
               BITS not bytes as the struct store
               bit sequences that are not multiple
               of bytes.
 * -------------------------------------------------- */

asn1_t *asn1_create_bits(const void *bits,int numbits);

/* -------------------------------------------------- *
   Converts a time_t to an ASN1_UTC or
   ASN1_TIME depending on 'type'.
 * -------------------------------------------------- */

asn1_t *asn1_create_time(int type,time_t val);

/* -------------------------------------------------- *
  Convierts a mp_int_t to ASN1_INT
 * -------------------------------------------------- */

asn1_t *asn1_create_bignum(mp_int_t *bn);

/* -------------------------------------------------- *
  Convierts a mp_int_t to ASN1_BITS
 * -------------------------------------------------- */

asn1_t *asn1_create_bignum_bits(mp_int_t *bn);

/* -------------------------------------------------- *
   Converts the unsigned int 'val' to ASN1_INT
 * -------------------------------------------------- */

asn1_t *asn1_create_uint(unsigned int val);

/* -------------------------------------------------- *
  Creates an ASN1_INT from a byte buffer
 * -------------------------------------------------- */

asn1_t *asn1_create_uint_from_bytes(const void *buf,int num);
asn1_t *asn1_create_int_from_bytes(const void *buf,int num);

/* -------------------------------------------------- *
   Converts a numeric string to ASN1_INT.

   Format can be decimal or hexa (prefixed by 0x)
 * -------------------------------------------------- */

asn1_t *asn1_create_int(const char *);

/* -------------------------------------------------- *
   Same as previous but wchar_t
 * -------------------------------------------------- */

asn1_t *asn1_create_intw(const wchar_t *);

/* -------------------------------------------------- *
   Converts a string in ASN1_UTF8
 * -------------------------------------------------- */

asn1_t *asn1_create_utf8(const char *);

/* -------------------------------------------------- *
   Same as previous but wchar_t
 * -------------------------------------------------- */

asn1_t *asn1_create_utf8w(const wchar_t *);

/* -------------------------------------------------- *
   Converts an element ASN.1 in numeric RFC-2253
   format to a real ASN.1 enconded struct
 * -------------------------------------------------- */

asn1_t *asn1_create_rfc2253(const char *);

/* ----------------------------------------------------- *
   Converts the string version of an OID to ASN1_OID.
 * ----------------------------------------------------- */

asn1_t *asn1_create_oid(const char *oid);

/* -------------------------------------------------- *
   Same as previous but wchar_t
 * -------------------------------------------------- */

asn1_t *asn1_create_oidw(const wchar_t *oid);

/* -------------------------------------------------- *
   Converts an attribute of type 'oid = valor' to
   an ASN1_SET that contains the attribute and
   the value which is coded as UTF-8 string
 * -------------------------------------------------- */

asn1_t *asn1_create_attr_set(const char *oid,const char *val);

/* -------------------------------------------------- *
   Same as previous but wchar_t
 * -------------------------------------------------- */

asn1_t *asn1_create_attr_setw(const wchar_t *oid,const wchar_t *val);

/* -------------------------------------------------- *
   Returns a RDN encoded in ASN.1 as an one-line
   string of the format described in RFC-2253.

    Use free() on the returned string when you do not
    need it any more.

    A RDN is a Relative Distinguished Name and it is
    described in the X509 specification that can be
    found here:

    https://hexdocs.pm/x509/X509.RDNSequence.html

 * -------------------------------------------------- */

char *asn1_one_line_rdn(unsigned char *asn1,int len);

#ifdef __cplusplus
};
#endif


#endif




