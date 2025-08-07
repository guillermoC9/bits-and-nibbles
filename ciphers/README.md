# Ciphers
## Symmetric criptographic primitives

What is criptography without ciphers? Ciphers are systems designed to offuscate information in order to make it impossible to obtain unless you are the intended recipient and have the means to de-offuscate it. That is why this process is called cipher/decipher information. 

Unlike criptographic hashes, ciphers were well in use long before computers came to life. In the book by Simon Singh 'The Code Book', you can read an interesting recall of the history of criptography. which is as old as information itself. There and in other places around internet is where you should learn about criptography and their uses.

Here, we are presenting a set of symmetric ciphers together with a way to use them easily, much like the [**Hashes and HMAC**](https://github.com/guillermoC9/bits-and-nibbles/edit/main/hashes) stuff in this same repository.

When is said *symmetric cipher*, what is meant is that the same key -because all ciphers have keys- is used to cipher and decipher the information, which bears a big disadvantage. That is that if I am Alice -which is what the industry likes to call the sender of information- I would need to distribute the key to the information's recipient -that's Bob-, and there is a danger of the key actually ending up in the hands of an unwanted recipient -which is usually called Eve for *eavesdropper*- whoh can then decipher the data as well as Bob. 

Is to solve this problem that *asymmetric ciphers* were created -being RSA the most famous-. They are cipher systems that use two keys: a public key and a private one. Being called like that is just because one is made public, and the other one you keep for yourself. Whatever is ciphered by one key, canonly be deciphered with the other.

Now I can give Bob my public key and he can give me his. Whatever he ciphers with it, can only be deciphered by my private key, and likewise I can do the same with his. Eve would not be able to do obtain any info from us. Let's at this point repeat that asymmetric ciphers were created to solve the **key distribution problem**. I say this because they have been for long time *wrongly* used to authenticate and sign stuff, and now with the arrival of Quantum computers and the infamous -but great- Shor's algorithm, Eve could generate a private key from a public one, and of course impersonate either Bob, me or both of us.

This is not the place to discuss that -specially because this is the space of symmetric ciphers-, but let me say that when I said *wronlgy* I meant it, as all the protection you can have to avoid someone impersonating you, was to keep your key hidden. That is so dangerous, as it will be a **disaster**. Unlike with symmetric keys, were getting hold of a key exposes just your info, a private key is also used to prove that it was you who signed something, so someone with your private key can be yourself, and someone with a Quantum computer can become *anyone* for whom the public key can be obtained. Welcome to chaos, as most authentication systems use private keys to identify people, a thing invented to solve the distribution problem, not the identity one.    

Anyway, that is for another day, here we put at your dispossal the following symmetric cipher primitives: AES, ARIA, BLOWFISH, CAMELLIA, CHACHA, CHACHA+POLY, DES, RC4, SALSA, TWOFISH and XTEA, and the way you distribute our keys is your problme for the moment. ;-)

The cipher engine in cipher.c does support different cipher types like stream or block ciphers, as well as different modes like: ECB, CBC, CTR, GCM and AED. It does also support few padding schemes for block ciphers. Note that this is implemented by the cipher engine itself, and not the cipher primitives themselves. This methodology allows to add new ciphers by just developing the initialization and basic encoding/decoding functions.

Just have a look at the code and the examples. :-)

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

$ ./test_ciphers


                     
```




