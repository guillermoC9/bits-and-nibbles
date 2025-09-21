# JSON
## JavaScript Object Notation

JSON was invented by Douglas Crockford as a way to give Smalltalk-like persistence to JavaScript, although it has evolved to become a de-facto replacement for XML as well as a kind of standard for data interchange and storage -some NoSQL databases use JSON as its base- as is very easy for humans to read and for software to parse. As said, it has replaced XML and similars in many uses because is more space efficient and as good as them keeping data hierarchy.  

JSON is also surprisingly small and efficient (find its complete definition at http://www.json.org/json-en.html) although if you think about it, it was created to store and recover the state of the objects in Javascript's memory, so a big and complex structure would have introduced risk of data corruption. Its simplicity, flexibility and unambiguity are really its strengths.

The library we present here allows not only parsing of JSON files, but also modification, handling and even saving, although they are stored all in memory until freed after processing. Therefore, the hierarchy in memory is exactly the same as it will be in the file, which is not the fastest or lightest way to handle data, especially for large datasets.

Basically, this library should be used to read a file, process it and free it; or to create a dataset in memory to be stored in a JSON file and freed after saving.  This is much easier than save or load JSON files bit by bit while processing the info, as this forces you to make that process in the correct order for the hierarchy of the file to be correct.

With this library you load the file and then access the info in the order you want. Same for saving, you can build the data hierarchy in any order and the file will be saved correctly. I actually created this library to use JSON to define X509 certificates to be created, as well as enabling JSON files to be used as config files, as JSON files are easier to manage than classical INI or CONF files, especially when it comes to grouping and managing hierarchical configuration settings. This is why the library support things like setting big numbers using [**mpint**](https://github.com/guillermoC9/bits-and-nibbles/edit/main/mpint), base64 data, time/date formats, bytes in hexa, etc.  If you do not need that stuff you could remove it to get rid of the dependancy in other files an make it sef-contained.

The library uses wchar_t for the data and paths so it can handle any language without changes. JSON specs state that files have to be encoded in UTF-8, and the unicode escape sequences in string must be encoded with UTF-16. IMHO this is messy and make handling JSON files a bit complex, so the library translate everything to wchar_t in memory in order to simplify the code to handle the info. Simplicity bears less points of possible failure.

Note that the library aims for data integrity, so -unlike JavScript- if two indentical key names are found in the same object, the second produces an error and the file will not be loaded. I think this is better than silently replace the first value with the second as JavaScript does, mainly because a duplicate key in the object is clearly an error, most likely produced by a manual modification going wrong, or by a buggy piece of code creating the JSON. Nobody will create a key to be later replaced by something different before having a chance to process it.

The library takes two approaches to key duplication: 

    1st.- Duplicate keys are NOT allowed when loading files or when adding existing objects into other objects for the reason explained above. Although they are actually allowed for arrays, as key names are ignored in array members and will be lost if saving the file, effectively avoiding the duplication.

    2nd.- Duplicate keys ARE allowed to be used when setting values or creating objects and arrays in memory, as the set functions will actually replace existing values of the object with the new ones, or will return the existing object if an existing path is especified to a creation function. 

The second approach mimics JavaScript, where  ``` let b = { obj : { x : 23, y : 22, x : 11 } }; ``` will end up with ```obj.x``` having a value of ```11```.  

The test code loads a couple of JSON files, detach them from their root, fusion them together on another tree, and then adds/deletes few branches and things in order to demonstrate how to use the functions of the library. It then saves a third file with the results of the tests.

## To build it:

For Microsoft C/C++ for Windows (Visual Studio C++) use:  

```
nmake /f msc.mak test
```

For Macintosh, Linux and others using GNU C:

```
make -f gcc.mak test
```
### To clean the builds:

Do the same that for build it but use *cleanit* instead of *test* as the target.

## Example output from the test program:

```

$./test_json

== JSON TEST START 

*** Test JSON ***
ROOT
--- START trace_json() ---
''={1,JSON_OBJECT}
   'stuff'={1,JSON_OBJECT}
      'test_1'={1,JSON_NULL,null}
      'test_2'={0,JSON_NULL,null}
--- END trace_json()---
AS FILE
--- START trace_json() ---
''={1,JSON_OBJECT}
   'stuff'={1,JSON_OBJECT}
      'test_1'={1,JSON_OBJECT}
         'certificate'={5,JSON_OBJECT}
            'version'={1,JSON_NUMBER,3}
            'serial'={0,JSON_NULL,null}
            'validity'={2,JSON_OBJECT}
               'notBefore'={0,JSON_NULL,null}
               'notAfter'={14,JSON_STRING,'20260610042531'}
            'subject'={1,JSON_OBJECT}
               'dn'={10,JSON_OBJECT}
                  'cn'={35,JSON_STRING,'cloud-nueve uk RSA Test Certificate'}
                  'c'={2,JSON_STRING,'GB'}
                  'l'={5,JSON_STRING,'Largs'}
                  'o'={19,JSON_STRING,'cloud-nueve uk ltd.'}
                  'ou'={17,JSON_STRING,'Server Operations'}
                  'st'={14,JSON_STRING,'North Ayrshire'}
                  'emailAddress'={21,JSON_STRING,'server@cloud-nueve.uk'}
                  'ev_c'={2,JSON_STRING,'GB'}
                  'jurisdictionOfIncorporationLocalityName'={5,JSON_STRING,'Largs'}
                  'ev_st'={14,JSON_STRING,'North Ayrshire'}
            'extensions'={4,JSON_OBJECT}
               'basicConstrains'={2,JSON_OBJECT}
                  'critical'={4,JSON_BOOLEAN,true}
                  'isCA'={5,JSON_BOOLEAN,false}
               'keyUsage'={2,JSON_OBJECT}
                  'critical'={4,JSON_BOOLEAN,true}
                  'uses'={2,JSON_ARRAY}
                     'uses[0]'={16,JSON_STRING,'digitalSignature'}
                     'uses[1]'={15,JSON_STRING,'keyEncipherment'}
               'extKeyUsage'={2,JSON_OBJECT}
                  'critical'={5,JSON_BOOLEAN,false}
                  'uses'={2,JSON_ARRAY}
                     'uses[0]'={10,JSON_STRING,'serverAuth'}
                     'uses[1]'={10,JSON_STRING,'clientAuth'}
               'subjectAltName'={2,JSON_OBJECT}
                  'critical'={5,JSON_BOOLEAN,false}
                  'names'={2,JSON_OBJECT}
                     'DNS'={18,JSON_STRING,'www.cloud-nueve.uk'}
                     'URI'={22,JSON_STRING,'https://cloud-nueve.uk'}
      'test_2'={1,JSON_OBJECT}
         'recipes'={3,JSON_ARRAY}
            'recipes[0]'={5,JSON_OBJECT}
               'name'={16,JSON_STRING,'Spanish Omelette'}
               'price'={2,}
               'ingredients'={3,JSON_ARRAY}
                  'ingredients[0]'={4,JSON_OBJECT}
                     'name'={8,JSON_STRING,'potatoes'}
                     'amount'={3,JSON_NUMBER,250}
                     'unit'={1,JSON_STRING,'g'}
                     'notes'={25,JSON_STRING,'We recommend old potatoes'}
                  'ingredients[1]'={4,JSON_OBJECT}
                     'name'={4,JSON_STRING,'egss'}
                     'amount'={1,JSON_NUMBER,4}
                     'unit'={5,JSON_STRING,'units'}
                     'notes'={0,JSON_STRING,''}
                  'ingredients[2]'={4,JSON_OBJECT}
                     'name'={5,JSON_STRING,'onion'}
                     'amount (g)'={1,JSON_NUMBER,1}
                     'unit'={4,JSON_STRING,'unit'}
                     'notes'={0,JSON_STRING,''}
               'instructions'={284,JSON_STRING,'Cut the potatoes in slices then fry them in sunflower or oliva oil but not too fried, then drain them in aplat with a paper towel, bater the eggs with some salt in a bowl and put the potatoes in. Mix it and the put on a flat frying pan with few drops of oliva oil to cook the omelette'}
               'attributes'={4,JSON_ARRAY}
                  'attributes[0]'={2,JSON_OBJECT}
                     'name'={5,JSON_STRING,'salty'}
                     'value'={4,JSON_BOOLEAN,true}
                  'attributes[1]'={2,JSON_OBJECT}
                     'name'={5,JSON_STRING,'fresh'}
                     'value'={5,JSON_BOOLEAN,false}
                  'attributes[2]'={2,JSON_OBJECT}
                     'name'={4,JSON_STRING,'sour'}
                     'value'={5,JSON_BOOLEAN,false}
                  'attributes[3]'={2,JSON_OBJECT}
                     'name'={6,JSON_STRING,'bitter'}
                     'value'={5,JSON_BOOLEAN,false}
            'recipes[1]'={5,JSON_OBJECT}
               'name'={7,JSON_STRING,'Ceviche'}
               'price'={3,}
               'ingredients'={3,JSON_ARRAY}
                  'ingredients[0]'={4,JSON_OBJECT}
                     'name'={4,JSON_STRING,'fish'}
                     'amount'={3,JSON_NUMBER,250}
                     'unit'={1,JSON_STRING,'g'}
                     'notes'={22,JSON_STRING,'We recommend Sole fish'}
                  'ingredients[1]'={4,JSON_OBJECT}
                     'name'={12,JSON_STRING,'green lemmon'}
                     'amount'={2,JSON_NUMBER,50}
                     'unit'={2,JSON_STRING,'ml'}
                     'notes'={46,JSON_STRING,'In spain, this kind of lemmon is called 'lima''}
                  'ingredients[2]'={4,JSON_OBJECT}
                     'name'={9,JSON_STRING,'red onion'}
                     'amount (g)'={2,JSON_NUMBER,50}
                     'unit'={1,JSON_STRING,'g'}
                     'notes'={0,JSON_STRING,''}
               'instructions'={80,JSON_STRING,'This is a recipe where raw fish is seasoned with the lemmon and the onion on top'}
               'attributes'={6,JSON_ARRAY}
                  'attributes[0]'={2,JSON_OBJECT}
                     'name'={5,JSON_STRING,'sweet'}
                     'value'={4,JSON_BOOLEAN,true}
                  'attributes[1]'={2,JSON_OBJECT}
                     'name'={5,JSON_STRING,'salty'}
                     'value'={4,JSON_BOOLEAN,true}
                  'attributes[2]'={2,JSON_OBJECT}
                     'name'={6,JSON_STRING,'bitter'}
                     'value'={5,JSON_BOOLEAN,false}
                  'attributes[3]'={2,JSON_OBJECT}
                     'name'={5,JSON_STRING,'fresh'}
                     'value'={4,JSON_BOOLEAN,true}
                  'attributes[4]'={2,JSON_OBJECT}
                     'name'={4,JSON_STRING,'sour'}
                     'value'={4,JSON_BOOLEAN,true}
                  'attributes[5]'={2,JSON_OBJECT}
                     'name'={4,JSON_STRING,'acid'}
                     'value'={5,JSON_BOOLEAN,false}
            'recipes[2]'={3,JSON_OBJECT}
               'name'={12,JSON_STRING,'Not a recipe'}
               '      '={52,JSON_STRING,'This field is named with a string filled with spaces'}
               'number'={13,JSON_NUMBER,18.234567e+12}
--- END trace_json()---
It didn't exist
Which is an array of numbers
--- START trace_json() ---
'Numbers'={7,JSON_ARRAY}
   'Numbers[0]'={10,JSON_NUMBER,-1234.5678}
   'Numbers[1]'={11,JSON_NUMBER,12345678790}
   'Numbers[2]'={13,JSON_NUMBER,987654321.009}
   'Numbers[3]'={24,JSON_NUMBER,9.223372036854775807e+18}
   'Numbers[4]'={19,JSON_NUMBER,9223372036854775807}
   'Numbers[5]'={25,JSON_NUMBER,1.8446744073709551615e+19}
   'Numbers[6]'={20,JSON_NUMBER,18446744073709551615}
--- END trace_json()---
Numbers have 7 elements
Numbers[0]=-1234.5678
Numbers[1]=12345678790.0000
Numbers[2]=987654321.0090
Numbers[3]=9223372036854775808.0000
Numbers[4]=9223372036854775808.0000
Numbers[5]=18446744073709551616.0000
Numbers[6]=18446744073709551616.0000
- Showing as different numbers
  Numbers[0] is -1234.5678
    as number = -1.234568e+03
    as short  = -1234
    as ushort = 64302
    as int    = -1234
    as uint   = 4294966062
    as long   = -1234
    as ulong  = 18446744073709550382
  Numbers[1] is 12345678790
    as number = 1.234568e+10
    as short  = 7110
    as ushort = 7110
    as int    = -539223098
    as uint   = 3755744198
    as long   = 12345678790
    as ulong  = 12345678790
  Numbers[2] is 987654321.009
    as number = 9.876543e+08
    as short  = 26801
    as ushort = 26801
    as int    = 987654321
    as uint   = 987654321
    as long   = 987654321
    as ulong  = 987654321
  Numbers[3] is 9.223372036854775807e+18
    as number = 9.223372e+18
    as short  = -1
    as ushort = 65535
    as int    = -1
    as uint   = 4294967295
    as long   = 9223372036854775807
    as ulong  = 9223372036854775807
  Numbers[4] is 9223372036854775807
    as number = 9.223372e+18
    as short  = -1
    as ushort = 65535
    as int    = -1
    as uint   = 4294967295
    as long   = 9223372036854775807
    as ulong  = 9223372036854775807
  Numbers[5] is 1.8446744073709551615e+19
    as number = 1.844674e+19
    as short  = -1
    as ushort = 65535
    as int    = -1
    as uint   = 4294967295
    as long   = -1
    as ulong  = 18446744073709551615
  Numbers[6] is 18446744073709551615
    as number = 1.844674e+19
    as short  = -1
    as ushort = 65535
    as int    = -1
    as uint   = 4294967295
    as long   = -1
    as ulong  = 18446744073709551615
-oOo- Path with Spaces -oOo-
json_get_string('stuff.test_2.recipes[2].      ')='This field is named with a string filled with spaces'
-oOo- Archive -oOo-

{
   "stuff" : 
   {
      "test_1" : 
      {
         "certificate" : 
         {
            "version" : 3,
            "serial" : null,
            "validity" : 
            {
               "notBefore" : null,
               "notAfter" : "20260610042531"
            },
            "subject" : 
            {
               "dn" : 
               {
                  "cn" : "cloud-nueve uk RSA Test Certificate",
                  "c" : "GB",
                  "l" : "Largs",
                  "o" : "cloud-nueve uk ltd.",
                  "ou" : "Server Operations",
                  "st" : "North Ayrshire",
                  "emailAddress" : "server@cloud-nueve.uk",
                  "ev_c" : "GB",
                  "jurisdictionOfIncorporationLocalityName" : "Largs",
                  "ev_st" : "North Ayrshire"
               }
            },
            "extensions" : 
            {
               "basicConstrains" : 
               {
                  "critical" : true,
                  "isCA" : false
               },
               "keyUsage" : 
               {
                  "critical" : true,
                  "uses" : 
                  [
                     "digitalSignature",
                     "keyEncipherment"
                  ]
               },
               "extKeyUsage" : 
               {
                  "critical" : false,
                  "uses" : 
                  [
                     "serverAuth",
                     "clientAuth"
                  ]
               },
               "subjectAltName" : 
               {
                  "critical" : false,
                  "names" : 
                  {
                     "DNS" : "www.cloud-nueve.uk",
                     "URI" : "https:\/\/cloud-nueve.uk"
                  }
               }
            }
         }
      },
      "test_2" : 
      {
         "recipes" : 
         [
            {
               "name" : "Spanish Omelette",
               "price" : "5\u20ac",
               "ingredients" : 
               [
                  {
                     "name" : "potatoes",
                     "amount" : 250,
                     "unit" : "g",
                     "notes" : "We recommend old potatoes"
                  },
                  {
                     "name" : "egss",
                     "amount" : 4,
                     "unit" : "units",
                     "notes" : ""
                  },
                  {
                     "name" : "onion",
                     "amount (g)" : 1,
                     "unit" : "unit",
                     "notes" : ""
                  }
               ],
               "instructions" : "Cut the potatoes in slices then fry them in sunflower or oliva oil but not too fried, then drain them in aplat with a paper towel, bater the eggs with some salt in a bowl and put the potatoes in. Mix it and the put on a flat frying pan with few drops of oliva oil to cook the omelette",
               "attributes" : 
               [
                  {
                     "name" : "salty",
                     "value" : true
                  },
                  {
                     "name" : "fresh",
                     "value" : false
                  },
                  {
                     "name" : "sour",
                     "value" : false
                  },
                  {
                     "name" : "bitter",
                     "value" : false
                  }
               ]
            },
            {
               "name" : "Ceviche",
               "price" : "25\u20ac",
               "ingredients" : 
               [
                  {
                     "name" : "fish",
                     "amount" : 250,
                     "unit" : "g",
                     "notes" : "We recommend Sole fish"
                  },
                  {
                     "name" : "green lemmon",
                     "amount" : 50,
                     "unit" : "ml",
                     "notes" : "In spain, this kind of lemmon is called 'lima'"
                  },
                  {
                     "name" : "red onion",
                     "amount (g)" : 50,
                     "unit" : "g",
                     "notes" : ""
                  }
               ],
               "instructions" : "This is a recipe where raw fish is seasoned with the lemmon and the onion on top",
               "attributes" : 
               [
                  {
                     "name" : "sweet",
                     "value" : true
                  },
                  {
                     "name" : "salty",
                     "value" : true
                  },
                  {
                     "name" : "bitter",
                     "value" : false
                  },
                  {
                     "name" : "fresh",
                     "value" : true
                  },
                  {
                     "name" : "sour",
                     "value" : true
                  },
                  {
                     "name" : "acid",
                     "value" : false
                  }
               ]
            },
            {
               "name" : "Not a recipe",
               "      " : "This field is named with a string filled with spaces",
               "number" : 18.234567e+12
            }
         ]
      },
      "Numbers" : 
      [
         -1234.5678,
         12345678790,
         987654321.009,
         9.223372036854775807e+18,
         9223372036854775807,
         1.8446744073709551615e+19,
         18446744073709551615
      ]
   },
   "obj0" : 
   {
      "tmp" : 
      {
         "obj1" : null,
         "array1" : 
         [
            null,
            null,
            null,
            {
               "last" : null
            }
         ]
      }
   },
   "response" : 
   {
      "result" : 
      [
         {
            "list" : 
            [
               null,
               null,
               null,
               null,
               {
                  "characters" : 
                  [
                     null,
                     null,
                     null,
                     null,
                     {
                        "manuel" : 
                        {
                           "films" : 
                           [
                              null,
                              null,
                              null,
                              null,
                              null,
                              "Faulty Towers"
                           ]
                        }
                     }
                  ]
               }
            ]
         }
      ],
      "messages" : 
      [
         "No messages"
      ],
      "normal_array" : 
      [
         {
            "child" : null
         }
      ],
      "drinks" : 
      [
         "Juice",
         "Meod",
         "Cuba Libre",
         "Wine"
      ],
      "     " : 
      [
         "My dad is an array,",
         "whose name is a string",
         "filled with spaces.",
         "And IT WORKS!"
      ]
   }
}
-oOo- Complete -oOo-
-oOo- Saved -oOo-
-oOo- Deleted -oOo-

== JSON TEST END

```




