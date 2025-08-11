# CSV files
## Comma Separated Value files

Here is a very simple way to import CSV files one line at the time.

Actually, it can be used not only to read Comma Spearated Values documents, but documents in any delimiter-separated value format. Although they are called CSV because that space is clearly dominated by CSV as popularized by Microsoft Excel and Microsoft Access.

These files are usually encoded using Microsoft Windows 1252 codepage, but they can also be encoded in UTF-8. Here we manage both encodings, although internally we convert Windows 1252 encoding to UTF-8 when reading the files, which is the enconding used in memory.

Limits in the CSV are actually the same that in Microsoft Excel, which can be read here:

- https://support.office.com/en-us/article/Excel-specifications-and-limits-1672b34d-7043-467e-8e27-269d656771c3

Note that although we respect the limit of 16,384 columns (cells) imposed by Microsoft Excel, our code here can be used to read over their limit of 1,048,576 rows, as rows here are returned one at the time, so that limit is informative only, because the limit could really be imposed by the caller. In practive we inforce a limit of 134,217,728 rows because there has to be a limit to avoid integer overflow. Yeah we could have used a bigger type, but it made not sense to set a bigger one, as our practical limit is already 128 times bigger than the official limit.

In fact our example uses a CSV with more than 2 million entries. To be exact: 2,683,735 UK Postcodes with its latitude and longitude.

Note that this code is intended for importing or tansforming, as CSV is not an optimized format to manage data, just to move it beween apps. Therefore this code is a bit slow, taking 5-10 seconds to handle the 2.6 million entries.

### Size Warning:

The test file uk_postcodes.zip is 25 Mb in size, and when unzipped -to uk_postcodes.csv- for running the tests, it becomes 75 Mb. It will be deleted when cleaning the build. Sorry for the size but wanted to use a file at least twice long as the limit of over 1 million rows.

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

$ make -f gcc.mak  test
gcc -c -o csv.o csv.c
gcc -o test_csv test_csv.c csv.o

You need to unzip uk_postcodes.zip to obtain uk_postcodes.csv
WARNING: The result file would be ~75 MB in size

$ unzip uk_postcodes.zip 
Archive:  uk_postcodes.zip
  inflating: uk_postcodes.csv        

$ ./test_csv uk_postcodes.csv  postcodes

*** Test of CSV ***

Read 3 Headers from 'uk_postcodes.csv'

1: Postcode
2: Latitude
3: Longitude

Read 2683735 rows of a max of 3 cols (2683735 reported internally)


*** Test of POSTCODES ***

Loading postcodes from uk_postcodes.csv...
Loaded 2683735 postcode entries from 2683735 rows

Write a uk postcode or part of it (empty to end):
ka30
Postcode 'KA30 0AA' is at lat=99.999999 lon=0.000000
Write a uk postcode or part of it (empty to end):
ka30 9hf
Postcode 'KA30 9HF' is at lat=55.800539 lon=-4.856999
Write a uk postcode or part of it (empty to end):
ka30 9aa
Postcode 'KA30 9AA' is at lat=55.796571 lon=-4.865847
Write a uk postcode or part of it (empty to end):
sw1p 3pa
Postcode 'SW1P 3PA' is at lat=51.498762 lon=-0.128107
Write a uk postcode or part of it (empty to end):
sw1a 2aa
Postcode 'SW1A 2AA' is at lat=51.503540 lon=-0.127695
Write a uk postcode or part of it (empty to end):
sw1a 1aa
Postcode 'SW1A 1AA' is at lat=51.501009 lon=-0.141588
Write a uk postcode or part of it (empty to end):

That's all folks!!


```




