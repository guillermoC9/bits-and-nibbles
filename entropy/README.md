# Entropy 
## Unpredictable entropy generator
### Designed and written by Guillermo Amodeo Ojeda.

This is a very simple unpredictable entropy generator, intended for seeding pseudo-random generators easily but effectively.

Although it is often assumed that 'unpredictable' and 'random' have the same meaning, when it comes to the code written here, these two words have a subtle -but important- difference.

An unpredictable number is a number that cannot be predicted, which means that there is no way of knowing what number will be generated even if you know how/when its going to be generated.

On the other hand, a random number can be predicted if you know how/when is going to be generated, and this is an important characteristic of random generators, because you can put two generators at the same state and both will be generating the same sequence of numbers. Two generators of unpredictable numbers will produce different sequences even if they are put in the exact same state at any point in time.

A sequence of random numbers have also other characteristics like certain distribution and periodicity; both in the order in which the numbers are generated as well as their bit distribution; both characteristics are expected to be of certain quality depending on their final use.

These characteristics and properties of random numbers are not expected to be found in unpredictable numbers, which are expected to have just that characteristic: be unpredictable.

Knowing the difference is essential in order to use the code here, as its intended to be used to create unpredictable numbers so unpredictable entropy can be generated from them, and in turn they can be used to whatever task you want them for. Like seeding or shuffling proper random number generators.

The unpredictable numbers are known as true-random numbers, and the random numbers are known as pseudo-random numbers.

**WARNING:** As said, the code here generates **unpredictable data** intended to seed pseudo-random number generators, so even if data generated with them may look like random data itself, **it isn't** so do not use it as such unless you know what you are doing, and therefore you measured the risk to use it. **Consider yourself warned**.

## unpredictable_seed()

You will find here a function called **unpredictable_seed()** which returns a 32 bit unsigned int every time is called, even if called many times in a row. This function was created by me in order to seed pseudo-random generators -like mother- that took a 32 bit integer as seed. Why?

Well, most seed generation techniques I had looked at the time were using some sort of time based formula to generate the seeds, which meant that if you called the function -lets say- within the same millisecond, the seed would be the same as the previous, effectively weakening a generator that otherwise would be strong.

Therefore, my first goal was to make sure that the function would return a different seed every time was called regardless of how small was the lapse from one call to another.

In order to do that I use a static counter and some other techniques to obtain some data which I hash with Daniel J Berstein's DJB hash to make sure the number is not a small one. I know this hash can generate a lot of collisions, but the data we feed into it is small enough to ensure it doesn't happen often. Actually, in my tests it only happened few hundred times in 15 million tries, and the collision never came within less than few thousand calls.

## unpredictable_seed_non_linear()

Here is an interesting function not only intended for generating unpredictable data, but also to make sure it takes an unpredictable amount of time to do it.

There are far too many feasible timing attacks in the world. Actually, so many that they neutralised few algorithms an ciphering modes like CBC. 

Also, the new 'brat in the neighbourhood' -Quantum Computers- use probablistics in order to find answers to questions like: *Which are the two prime factors of a modulus in a RSA key?* So timing is **really important** for the security of algorithms in both Classic and Quantum Computers.

Now, people make huge efforts to make sure algorithms are **time constant** in order to beat timing attacks. I think that its much better to make the algorithms **time unpredictable**, which means that the algorithm will take different timing to do things even if they are in the same state or fed the same input. This will make impossible to attack algorithms by feeding then with some particular input and time their behaviour in order to gather time information to guess things. Disturbing time unpredictibly will effectively neutralize timing attacks.

This is the intented use of this function, which you can use to disturb exectution time, even if you do not need an unpredictable seed, calling it for example like this:

```
    
    if(condition && unpredictable_seed_non_linear())
    {
        ...
    }
```

Which plays with the knowledge that ```unpredictable_seed_non_linear()``` never returns 0 -somethign the compiler doesn't know-

The function **get_entropy_non_linear()** uses it to make sure its execution time is unpredictable.  

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

*** Entropy Test ***

unpredictable_seed()=f7bd9601
unpredictable_seed()=9905cb19
unpredictable_seed()=716c973f
unpredictable_seed()=c07d6893
unpredictable_seed()=61fae427
unpredictable_seed()=5c6dd3c3
unpredictable_seed()=2dccefd
unpredictable_seed()=6ec00111
unpredictable_seed()=6b66aa1f
unpredictable_seed()=63b67db7
unpredictable_seed()=8546b03b
unpredictable_seed()=d559dedb
unpredictable_seed()=b0923b9d
unpredictable_seed()=41c111cb
unpredictable_seed()=fbae96ed
unpredictable_seed()=48d79eb3

unpredictable_seed_no_linear()=5ae8fdb5
unpredictable_seed_no_linear()=66bedb87
unpredictable_seed_no_linear()=f1384831
unpredictable_seed_no_linear()=2ffc3907
unpredictable_seed_no_linear()=78572dab
unpredictable_seed_no_linear()=bdb96997
unpredictable_seed_no_linear()=d0c0ad87
unpredictable_seed_no_linear()=8de012f9
unpredictable_seed_no_linear()=688cfcfd
unpredictable_seed_no_linear()=c82298a9
unpredictable_seed_no_linear()=3ca0c7f9
unpredictable_seed_no_linear()=1a905e4f
unpredictable_seed_no_linear()=12af1107
unpredictable_seed_no_linear()=e64caeb7
unpredictable_seed_no_linear()=dad87d9b
unpredictable_seed_no_linear()=9288a8e1

get_entropy()=4b66f1b0f16bad2acf15d5ffa94b6a9ee1a489810b9f832c816c07e911c708422fc03a1a25678fcfdb68ce0deff9b69529acd0ffed40c50623e1d5930d281c7c
get_entropy()=33151eeeb3598b7a45b73e81d18fbc709f572b925d315f8b395063da950ec7298f1a79c7cbd459772b967c9445f037f6376837e18b0f2f960ff54bba59d33091
get_entropy()=279863c7b78736afd940a0e275eaca75dd57b1c62d1f79734d6710ef565781dd20390d3d572c8e53ccefc7e3d76e2e9b4a24b2db530d299f56baed3d762e2b45
get_entropy()=dbba37b4637786d485bedcd5dd64d965ab64b7a2196859e0f1f130a739d3511fa1c4c0d2c30371986b046249cd45db5cd7843dd073a4d3dc6b8cb8b14be0da92
get_entropy()=0ded02d849de4179f9cf0705dd7d209dc9a166a44ff647591b2ebf4855bc1b6091acd2fa971d4e6bcf7eaa41ff607da92f663824a9ddc24a8114bb57a3461280
get_entropy()=45f6692127606afda1312caaed26ec9b83bfe7d51bc5dced95b235a8cf929fe9f166ec7a295af729173f68d25bfdd84ed12d96ea53f718ca113ecb4faf759306
get_entropy()=617b4143099c8f13b7611845cb968fb917f9e1fa23c1f9d7599e2626df0c3fff43c706ca4f94f63269d3372deb7c58c0294aff19d5a87875dffbd8d7558ad377
get_entropy()=9d42404647b7593cc9e4723683d50b10e30bff90693e63ddd9e8ab0ca713c3ed6594a40f25a34bbbf78a0d05c95f82c4538ce585531bbcfd59612b90db986ab9
get_entropy()=cf2547742515c1c15d78b2c383050f8ed38c9aec7ff2d95fe7f62e76e901a50f8d3954f2e31917ee3f73ae58439eea33c54fa91405331d8c133c26d8db2c5a24
get_entropy()=47558598a561125d33cce73a9f59dcf6b9e26bd1cb0a0f2e07044e8deb5378ae8709854e3d9f96adbfc7857db52656e9f9bbee6f870ec897e93de5d97766e887
get_entropy()=c97ca5ca4fe271bdfd768be275bdbd2f5d678c2b8742eb6bbfe6f0027975af2577a20774116207e6b1686328b9a6e9a7f3af287345183f1b37c4ce121d3b4a96
get_entropy()=67dd05570f18417a2f78ae1ad9530c71f54caf42c5e4d153b1babe93b36e8c220feea99737d4bc32391ce403f10f4ffe5d24bd17353db2c05bbf595f0b9b5525
get_entropy()=194c999bef738d94f5077d42cfbc7923f960ca1635627b82a59064ed95768ca4d7555a12bd4d39ac5554c956dfd6bdcc77cbef3437bbbb9cbf26f44e3fc9c5a1
get_entropy()=7fc9bf862fc346a331ac4211ed206c60d9b9272983a52a48e9cbf668ebfcae764917640dd95fe27abb17d88acdb053f1cb0bf8662f8fc2a32139545347966dd3
get_entropy()=93c15ed877771c503f3409500ffcc6cdc188756db5d58230c72b08d05781b8bf932360f64996d5d5517e36b6dd6f0bcf0f1242e3a377bd4071edaa63275a38f8
get_entropy()=4905f1ec3d5106ea254e9ba273e12c1bed7e4beda316af4edd586a79e579d12e175f67a89bbcbbdce7e542dd3fd872d957c7d0f765ec6f83236d0f41432a761d

get_entropy_no_linear()=0d8da3646d002c1d4daa4d4f075e621ef1f19a53c1793b649b0a0e466bdbccddc1d5c0fd4b5b35bd13ec3e06bb892f5a2b680b5e6172166a83b004ad49b85ab4
get_entropy_no_linear()=516b62dcadbffc5be7ce0af27f14025a1f38180a55a8026403781b647f451ad3f54cf4f7cf7a67df11ecc810f7a36afcd1903767852f008a4f42e46eeb29cb07
get_entropy_no_linear()=bda3f73831184e30ed8dc9a46752f05ad530997f83224878e366736695eb439c27a5cc606512180139b8062f9748e3cceb07a60b1b75110d5fd08dda35420701
get_entropy_no_linear()=5fc2f376edc252df9b39afc413946e0471c6b9668376362ad9175f042105b4cb414a74d14b651d76e55a29d0b78964f183a5fc14fd3fcf7af556159c6fb73fb4
get_entropy_no_linear()=49727cc62b81feebfd5028c85991ad442753c0cdd5ac6d5eb7a9fca973e14390d5eed8a6b1095846a9665ee2df716a8869fa936f2b7729a1c9162e8cebf9e41d
get_entropy_no_linear()=03f3d7fbf1b97b2663c47c5e77b9af19737b1189fb87233073810206fb237f5aab9fecbb9dbaeceda13df7b2af30afabefca7108e71f639eb36be017e5120fad
get_entropy_no_linear()=fbfa2e593d4638ba3d31066aef9e576e6f048e4c95f01f6a554003075114b8ec41ee02e64971d2d8dff8b56dc5362e5f8d9e1761b3ce4adb9d4ec65c79135697
get_entropy_no_linear()=1f4075ddb15b18e2abe8f9c56f4107faab00ee36f91dadca6b85b40dcb8de122d5558241eb19d9c1bf9b9c09bb59bee5f7c7d8a857fd7ea52fa5e42725f8b80d
get_entropy_no_linear()=5d42b79b3da8a8ef21baca01656e680f97ceb41e25176517576d2cac8326ff5be395380b23eb0af1ed06e0a21bcc57c915f3023bfdbb429bcfe3fff135a6c5c7
get_entropy_no_linear()=1f88eb32358f13e0919c5dccd70dc1f3593852709986b36e8dc4020a0783a7fa7b4e8389cfed4bcb6166ec6ef5938e444f0a25e4d96cc8de6d4e60e3d5663bec
get_entropy_no_linear()=0b20ba9fd5dea78587be8106431a08d05d5ba70d8744c4a02317d0b87b9728feb93020d023da96cb5f9375c747dd485a7f0e6c74c5e38aeb132b509d4166dbda
get_entropy_no_linear()=075de314f70d36a657b64af705aaa835a584ea95a7c7c0b56d05959ec1cb9bbfd7b1ebbc8b0acdad1df8c1daa7aa9fdc57499448b94ad37f853d4de053951a2d
get_entropy_no_linear()=79a528b37be254e2812cb466cb858506c51451ac395729d62931ab8e314b38a66912e43f49f31bf5e97a21c3959846907d15cb65a3e9ba02f33e36664b41fb4f
get_entropy_no_linear()=51354fd1f161a7dc63de220df3bee45b67ba557c21c6103f8bc4dbef499cac1eab35284a19cf1a671907c23857a71e60d5bd90691f1685abe5819ae025fc87d0
get_entropy_no_linear()=a34ad86ebdf321e35f290c31178d3ecf7bf5fc8cef10c5948fa5e23313788bdeb7bab2ca051fec06b9e5682727e1bee69da26f57a9188f4d9dea0096e1e3abe5
get_entropy_no_linear()=f96d4139ed2390643fddd6507d53ab2965d6165778af6111fc1a39d940ab763b7c68ce6b692a89a1970c7fd7fff486137f390e59772efbb7edf4c6c9c036c633

```




