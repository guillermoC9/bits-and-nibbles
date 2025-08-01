# Checksum 

## Luhn's Check

Luhn's check if a CRC calculation which generates a single digit from an array of numbers. That single digit allows to check the correctness of the array of numbers by just adding the number to the end of the array, and repeating the CRC calculation, which then will result in a 0. That is a very powerfull way to check if the numbers are correct before checking it, for example after a user has typed it.

It is widely used in the industry since Hans Peter Luhn from IBM invented the algorithm on the 80's. For example is in every credit or debit card number, bank account numbers and even in the OTP numbers, which are these 6 digit numbers you get in your device or by email, sent from Apple, Google, Amazon or Microsoft to verify that you are yourself. ;-) The 6th digit is the Luhn's digit of the other 5, like is always the last on the credit card or account number. 

If you do the calculation of the whole number, 0 should be the result; and if you remove the last number before doing the calculation,  the result will be that number that you removed.

If you want to learn more about it go to wikipedia, which is what I read to write the code:

    https://en.wikipedia.org/wiki/Luhn_algorithm

## Spanish's control letter

Spain has two documents used to identify people:

- DNI: Which is an anagram for 'Documento Nacional de Identidad' (Identity Document of National)
- NIE: Which is an anagram for 'Numero de Identidad de Extranjero' (Identity Number of Foreigner)

These two documents use a 'number' that finish with a letter which is used in the exact same way of the Luhn's check: to allow determine if the other digits of the 'number' are correct.  The reson why I enclose 'number' in quotes, is because they are technically numbers, but in practice they include letters. In the case of the DNI the check on last position; but in the case of NIE, there is also a letter at the start -which are always X,Y or Z- and the check one last. Note that teh initial letter is not included in the calculation of the control letter:

        - DNI format is XXXXXXXXC  where: X is a number and C the control letter.
        - NIE format is LXXXXXXXC  where: L is a letter (X, Y or Z), X is a number and C is the control letter.

You can find details for all these -which I read to write the code- at (**Warning: pages are in Spanish**) :

    https://www.interior.gob.es/opencms/ca/servicios-al-ciudadano/tramites-y-gestiones/dni/calculo-del-digito-de-control-del-nif-nie/









