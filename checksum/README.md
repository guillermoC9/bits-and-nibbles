# Checksum 

## Luhn's Check

Luhn's check if a checksum calculation which generates a single digit from an array of numbers. That single digit allows to check the correctness of the array of numbers by just adding the number to the end of the array, and repeating the CRC calculation, which then will result in a 0. That is a very powerfull way to check if the numbers are correct before checking it, for example after a user has typed it.

It is widely used in the industry since Hans Peter Luhn from IBM invented the algorithm on the 80's. For example is in every credit or debit card number, bank account numbers and even in the OTP numbers, which are these 6 digit numbers you get in your device or by email, sent from Apple, Google, Amazon or Microsoft to verify that you are yourself. ;-) The 6th digit is the Luhn's digit of the other 5, like is always the last on the credit card or account number. 

If you do the calculation of the whole number, 0 should be the result; and if you remove the last number before doing the calculation,  the result will be that number that you removed.

If you want to learn more about it go to wikipedia, which is what I read to write the code:

- https://en.wikipedia.org/wiki/Luhn_algorithm

## Spanish's control letter

Spain has two documents used to identify people:

- DNI: Which is an anagram for 'Documento Nacional de Identidad' (Identity Document of National)
- NIE: Which is an anagram for 'Numero de Identidad de Extranjero' (Identity Number of Foreigner)

These two documents use a 'number' that finish with a letter which is used in the exact same way of the Luhn's check: to allow determine if the other digits of the 'number' are correct.  The reson why I enclose 'number' in quotes, is because they are technically numbers, but in practice they include letters. In the case of the DNI the check on last position; but in the case of NIE, there is also a letter at the start -which are always X,Y or Z- and the check one last. Note that the initial letter has a special value in the calculation of the control letter in a NIE:

        - DNI format is XXXXXXXXC  where: X is a number and C the control letter.
        - NIE format is LXXXXXXXC  where: L is a letter (X, Y or Z), X is a number and C is the control letter.

**Note** that the DNI is also called NIF: Número de Identificación Fiscal (Taxpayer Identification Number).

You can find details **-in Spanish-** for all these at:

- https://www.interior.gob.es/opencms/ca/servicios-al-ciudadano/tramites-y-gestiones/dni/calculo-del-digito-de-control-del-nif-nie/

### Translation of the page:

Calculation of the control letter for NIF/NIE, as per article of 12 of Royal Decree-law 255/2025, from 1st of april, which establishes that the Identitity Document of National, will include the number and control letter for the unequivocal verification of the DNI.

To verify the DNI/NIF of Spanish residents, the algorithm to calculate the control letter would be as follows:

```

Divide the number by 23 and the remainder (0-22) would be replaced for a letter from the following table:

REMAINDER	0	1	2	3	4	5	6	7	8	9	10	11
LETTER        	T	R	W	A	G	M	Y	F	P	D	X	B
 

REMAINDER	12	13	14	15	16	17	18	19	20	21	22
LETTER        	N	J	Z	S	Q	V	H	L	C	K	E

```

So if the number is 12345678, divided by 23 will have a remainder of 14, so the letter would be Z, completing the number to 12345678Z.

The NIE for foreigners resident in Spain starts by one of the letters: X, Y, Z and is followed by 7 letters and the control letter.
For the calculation of the control letter, the first letter take the following digit values: 

```
X = 0
Y = 1
Z = 2
```
Then is divided by 23 and the digit is taken from the same table as the DNI/NIF.

This means that X1234567 becomes 01234567 and gives a remainder of 19 (letter L) and Z3456789 becomes 23456789, giving remainder 9 (letter D).

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

*** Test DNI/NIE's check ***

Write a DNI or NIE (empty to end):
X1234567
The control letter for 'X1234567' is 'L', so the one given is Wrong
Write a DNI or NIE (empty to end):
X1234567L
The control letter for 'X1234567L' is 'L', so the one given is Ok
Write a DNI or NIE (empty to end):
12345678z
The control letter for '12345678z' is 'Z', so the one given is Ok
Write a DNI or NIE (empty to end):

calc_dni_letterw(12345678)   = 'Z'
check_dni_letterw(12345678Z) = Ok
calc_dni_letterw(2564589)    = 'T'
check_dni_letterw(2564589C)  = Ok
calc_dni_letterw(X1234567)   = 'L'
check_dni_letterw(X1234567L) = Ok
calc_dni_letterw(Z3456789)   = 'D'
check_dni_letterw(X3456789T) = Bad (it should be D)


*** Test Luhn's CRC ***

calc_luhnc('7992739871 3',0,TRUE,TRUE) = '0'. Must be '0'
calc_luhnc('7992739871 3',0,FALSE,FALSE) = '3'. Must be '3'
calc_luhnc('7992739871 3',0,TRUE,TRUE) = '0'. Must be '0'
calc_luhnc('7992-7398-71',0,FALSE,TRUE) = '3'. Must be '3'
calc_luhnc('7992-7398-71-3',0,TRUE,TRUE) = '0'. Must be '0'
calc_luhnc('7992739871',0,FALSE,FALSE) = '3'. Must be '3'
calc_luhnc('7992739871 3',0,TRUE,TRUE) = '0'. Must be '0'
calc_luhnc('7992739878',0,FALSE,FALSE) = '8'. Must be '8'
calc_luhnc('7992739878 8',0,TRUE,TRUE) = '0'. Must be '0'
calc_luhnc('7992739878 3',0,TRUE,TRUE) = ''. Must be ''
calc_luhnc('502071110577440',0,FALSE,FALSE) = '7'. Must be '7'
calc_luhnc('5020711105774407',0,TRUE,FALSE) = '0'. Must be '0'
calc_luhnc('502071110577440',0,FALSE,FALSE) = '7'. Must be '7'
calc_luhnc('5020 7111 0577 4407',0,TRUE,TRUE) = '0'. Must be '0'
calc_luhnc('411111111111111',0,FALSE,FALSE) = '1'. Must be '1'
calc_luhnc('4111111111111111',0,TRUE,FALSE) = '0'. Must be '0'
calc_luhnc('4111111112111111',0,TRUE,FALSE) = ''. Must be ''
calc_luhnw('555544443333111 ',0,FALSE,FALSE) = '1'. Must be '1'
calc_luhnw('5555444433331111',0,TRUE,FALSE) = '0'. Must be '0'


```





