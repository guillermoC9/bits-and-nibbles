/*
    oid.h

    Routines to handle OID, X.500 and X.509 fields

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

        - Written by Guillermo Amodeo Ojeda using info from:

            http://www.ietf.org/rfc/rfc2253.txt
            http://www.ietf.org/rfc/rfc4519.txt
            http://www.ietf.org/rfc/rfc3280.txt
            http://www.iana.org/assignments/ldap-parameters/ldap-parameters.xhtml
            http://www.ietf.org/rfc/rfc4013.txt

                                --oO0Oo--
*/

#ifndef OID_MANAGEMENT
#define OID_MANAGEMENT

#include "stuff.h"

/* ------------------------------------------------------ *

    Enums for easy handling of fields X.500/X.509

    Note that they are alphabetically ordered by
    their textual names so they can be binary
    searched. Look at oid.c for details if you
    need to add new fields.

    We have created aliases for 3 fields so they are
    easier to handle by our code:

        1.3.6.1.4.1.311.60.2.1.1 - jurisdictionOfIncorporationLocalityName
        1.3.6.1.4.1.311.60.2.1.2 - jurisdictionOfIncorporationStateOrProvinceName
        1.3.6.1.4.1.311.60.2.1.3 - jurisdictionOfIncorporationCountryName

    These are the fields that provide Extended Validation (EV) to X509 certificates,
    and we have createad the following aliases for them:

        ev_l  = jurisdictionOfIncorporationLocalityName
        ev_st = jurisdictionOfIncorporationStateOrProvinceName
        ev_c  = jurisdictionOfIncorporationCountryName

    The EV are in extreme fashion these days as they are suppossedly more secure than
    other certificates because the Certification Authority garantees that they can
    PHISICALLY locate the subject of the certificate using these 3 fields.

    It is up to you to believe or not if this makes an EV certificate more secure
    than one which does not include these fields. I off course don't think they are
    more secure as is upto you to believe they hold that info, although browsers
    seem to assure their users that EV are more secure by changing their to green
    so they feel more protected.

    The reality is that certificates are as trusthworthy as the certification
    authority that produce them is, which basically means they are as trusted
    as you want them to be.

 * ------------------------------------------------------ */

enum
{
    X500_BCAT=0,     /** businessCategory - eg. 'Banking', 'IT', 'Hospitality', 'Food Industry',.. */
    X500_C,          /** c - Country. Usually expresed in two letters GB, ES or FR. */
    X500_CN,         /** cn - Common Name. Like: 'myserver.mydomain.com', 'IT Services' or 'T. S. Eliot'*/
    X500_DC,         /** dc - Domain Component. A component of any domain, e.g.  'com' or 'example' from 'example.com' */
    X500_DESC,       /** description - eg. 'This is an extended validation certificate'  */
    X500_DN ,        /** dn - Distinguished Name. eg. 'CN=T.S. Eliot, O=Eliot\, Haigh-Wood and Russell, C=GB' */
    X500_DNQ,        /** dnQualifier - Distinguished Name Qualifier. eg. 'Company Entry' or 'Telephone Listing' */
    X500_EMAIL,      /** emailAddress - eg. ts@eliot.com */
    X500_C_EV,       /** Country for Extended Validation */
    X500_L_EV,       /** Locality for Extended Validation */
    X500_ST_EV,      /** State or Province for Extended Validation */
    X500_GENQ,       /** generationQualifier - eg. 'III', 'Jr.', 'Sr.', 'Elder' ... */
    X500_GN,         /** gn - Given Name. eg.  'Guillermo', 'Christopher', 'Antonio', ... */
    X500_INI,        /** initials. Like 'T. S.' in 'T. S. Elliot') */
    X500_JOI_C,      /** jurisdictionOfIncorporationCountryName - Country for Extended Validation */
    X500_JOI_L,      /** jurisdictionOfIncorporationLocalityName - Locality for Extended Validation */
    X500_JOI_ST,     /** jurisdictionOfIncorporationStateOrProvinceName - State or Province for Extended Validation */
    X500_L,          /** l - Locality. eg. Largs, Glasgow, Sevilla */
    X500_O,          /** o - Organization. eg. 'cloud-nueve uk ltd.'  */
    X500_OU,         /** ou - Organization Unit. eg. 'IT Services' */
    X500_POSTADDR,   /** postalAddress - eg. 'Greenock Road, PA16 0AH - Greenock'  */
    X500_POSTCODE,   /** postalCode - eg. 'PA16 0AH' */
    X500_POBOX,      /** postOfficeBox - eg. '16' on 'PO Box 16' */
    X500_PSEUDO,     /** pseudonym - eg. 'The Big Yin' or 'Guillermo's corner' */
    X500_SERIAL,     /** serialNumber - eg. 001153  */
    X500_SN,         /** sn - Surname. eg.  'Amodeo', 'Turcksin' or 'Mamani' */
    X500_ST,         /** st -  State or Province. eg. 'North Ayrshire', 'Andalucia' or 'Ohio' */
    X500_STREET,     /** street - Street part of an address. eg. 'Flat 1-1 of 22 Union Street' */
    X500_TITLE,      /** title - eg. 'Dr.', 'Mr.', 'Sir', 'Dame' ... */
    X500_UID,        /** userid - eg. 'gamodeo' or  'turksin' */
    X500_PASSWD,     /** userPassword - eg. 'ThisIsASecret' */

    /** Always last */

    X500_NUM_FIELDS
};


/* ------------------------------------------------------ *
    All functions here ignore case for the name of
    x500/x509 fields, as advised by all the RFCs we
    used as reference.

    WARNING: 

        NONE of them accept that origin and destination
        pointers point to the same buffer.
 * ------------------------------------------------------ */


#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------------------------------ *
   Convert an OID to the its field name in x500/x509
   and viceversa.

    Example:


        oid_to_x500(...,"2.5.4.3") == "CN"
        x500_to_oid(...,"cn") == "2.5.4.3"

* ------------------------------------------------------ */

char    *oid_to_x500(char *resp,size_t len,const char *oid);
wchar_t *oid_to_x500w(wchar_t *resp,size_t len,const wchar_t *oid);

char    *x500_to_oid(char *resp,size_t len,const char *x500);
wchar_t *x500_to_oidw(wchar_t *resp,size_t len,const wchar_t *x500);

/* ------------------------------------------------------ *
   Convert an OID or field name of x500/x509 to our
   quick access macros and viceversa.

    Example:

        field_to_x500(...,X500_CN) == "CN"
        field_to_oid(...,X500_CN) == "2.5.4.3"

        oid_to_field(...,"2.5.4.3") == X500_CN
        x500_to_field(...,"cN") == X500_CN
* ------------------------------------------------------ */

char    *field_to_x500(char *resp,size_t max,int field);
wchar_t *field_to_x500w(wchar_t *resp,size_t max,int field);

int x500_to_field(const char *x500);
int x500_to_fieldw(const wchar_t *x500);

char    *field_to_oid(char *resp,size_t max,int field);
wchar_t *field_to_oidw(wchar_t *resp,size_t max,int field);

int oid_to_field(const char *oid);
int oid_to_fieldw(const wchar_t *oid);

/* ------------------------------------------------------ *
    Convert an OID in string format (char/wchar) to
    an OID in ASN.1 and return the length of the ASN.1
    string.

    WARNING:

        The returned ASN.1 contain just the data, not
        the header that identifies the data as an OID.
 * ------------------------------------------------------ */

size_t oid_asn1(unsigned char *asn1,size_t max,const char *oid);
size_t oid_asn1w(unsigned char *asn1,size_t max,const wchar_t *oid);

/**  These do the opposite and return the len of the OID string */

size_t asn1_oid(char *oid,size_t max,const unsigned char *asn1,size_t len);
size_t asn1_oidw(wchar_t *oid,size_t max,const unsigned char *asn1,size_t len);

/* ------------------------------------------------------ *
    Convert an OID encoded in ASN1 format to an array 
    of int  values with the numbers of the OID, and 
    return the length of the array upto a max elements.

    This is like ans1_oid() but returns a list of 
    ints so they can be checked easier than to use strings
    eg. if the oid in string would be: 2.5.29.19, the
    function will return 4 and the array:

        oid[0] = 2
        oid[1] = 5
        oid[2] = 29
        oid[3] = 19

        max is the maximum number of entries that the
        array oid will accept. A goog choice for the
        maximum is 25,  which we use internally here
        because the longest I have seen is 

        oid_ints_to_asn1() does the opposite
 * ------------------------------------------------------ */

size_t asn1_oid_to_ints(unsigned int *oid,size_t max,const unsigned char *asn1,size_t len);
size_t oid_ints_to_asn1(unsigned char *asn1,size_t max,const unsigned int *oid,size_t len);


size_t oid_to_ints(unsigned int *dest,size_t max,const char *oid);
size_t oid_to_intsw(unsigned int *dest,size_t max,const wchar_t *oid);


/* ------------------------------------------------------ *
    Convert an OID encoded in ASN.1 to a x500 field name.

    The functions assume that the ASN.1 strings contain
    just data, so do not send a type header indentifying
    the data as an OID.
 * ------------------------------------------------------ */

size_t asn1_oid_to_x500(char *x500,size_t max,const unsigned char *oid,size_t len);
size_t asn1_oid_to_x500w(wchar_t *x500,size_t max,const unsigned char *oid,size_t len);

/* ------------------------------------------------------ *
   These do the opposite and the returned ASN.1 is just
   data, so it doesn't contain the type header.
 * ------------------------------------------------------ */

size_t x500_to_oid_asn1(unsigned char *oid,size_t max,const char *x500);
size_t x500_to_oid_asn1w(unsigned char *oid,size_t max,const wchar_t *x500);


/* ------------------------------------------------------ *
   Escapes a string as specified in section 2.4 of
   RFC-2253 and returns the number of copied characters
   or 0 if it doesn't fit

   You can call it with 'dest' as NULL if you want to find out
   the size needed for the escaped string.
 * ------------------------------------------------------ */

size_t escape_string_rfc2253(char *dest,const char *orig, size_t max);

#ifdef __cplusplus
};
#endif


#endif
