# Time and Ticks
## Appropiate time management

Time is **really important**, so we have defineded a type called ```ticks_t``` that represents a number of clock 'ticks' which happen every 100 nanoseconds. Why?

Well, we need a portable way for measuring time homogeneously in every system that our software runs on, as each system has a different method and time basic unit to keep track of time. Unix, Linux and similars use ```time_t```, which is measured in seconds, and Windows use ```FILETIME```, which holds units of 100 nanoseconds each.  This is the unit we chose for our ```ticks_t```. Note that because the existance of POSIX, Windows also has ```time_t```. 

One important reason to choose 100 nanseconds unit for ```ticks_t``` was the need to ease mental calculations when programming. Managing computer times is quite complex in human terms, so simplify the measures is always a good idea. Let's have a look at time and explain myself:

  - 1 Day has 24 hours (86400 seconds).
  - 1 Hour has 60 minutes (3600 seconds).
  - 1 Minute has 60 seconds.
  - 1 Second has 1000 milliseconds.
  - 1 Millisecond has 1000 microseconds.
  - 1 Microsecond has 1000 nanoseconds.
  - 1 Nanosecond has 1000 picoseconds.

Computer's CPU generate clock ticks every picosecond or less, and being a picosecond the millionth of a microsecond -actually the millionth of a millionth of a millisecond-, it complicates mental calculations quite a lot when programming. Therefore we simplify things by using a tenth of microsecond (100 nanoseconds) as our minimum division of time. It is small enough (ten millionth of a second) to hold accurate timing, and big enough to do mental math with them. Not counting the fact that is a 'standard' measure of time in the industry,  used not only by Windows, but also in other industry standards (e.g. in the industry's GUID, which version 1 is described by RFC-4122).

So once we have established ```ticks_t``` as a good minimum time unit for portability and ease of use, is when we talk about the other time units, but before I should highlight that ```ticks_t``` is a **signed type** like all time units shold always be. This is becasue **time is always relative to some epoch** which will have the unit value of 0 to represent the epoch itlsef. Then, positive numbers represent the elapsed time units since that epoch, and negative numbers represent time units before that epoch. 

Probably the most famous of all epochs is the birth of Christ, which is so important that many countries measures time since that epoch, which is called Year 0 AD ('Anno Domini' or 'Year of the Lord)') which seems to have happened some 2025 years ago as per today. A difference of that epoch with most of the others, is that negative years are not expressed as negative, but appended BC (Before Christ) intead. The notation when using this epoch, is that the year 2025 is 2025-AD and -10 would be 10-BC. 

Most people live with epochs not knowing they are using them. For example your age is the number of years passed since the epoch of your own birth. Another common epoch is the start of a century, were people often will know that 10/06/06 means 10th of June 2006, as 06 is  the number of years since epoch year 2000. When I was at school, 10/06/06 would mean 10 of June 1906, as the epoch year was 1900.

So now that you know that there is not such thing as **absolute time**, we must clarify that even if ```ticks_t``` is like other units a counter of elapsed time units, it has no specific epoch associated to it. We just need it for measuring elapsed time acurately in very small units of time, but the epoch is irrelevant becuse it will depend on what are you measuring. 

The other mentioned units all have an specific epoch. For example, ```time_t``` holds the number of seconds since 1st of January 1970 (00:00:00 01/01/1970), and is also signed, as negative values represent time prior to the epoch. ```FILETIME``` from Windows holds tenth of microseconds since 1st of January 1601 (00:00:00 1/1/1601) and is actually **unsigned**, so time prior the epoch cannot be represented with the type itself. In fact, the function GetSystemTime() only return years since 1601 up to 30827. To be honest I think that representing time before 1601 wold be more useful than represent time after 29226, so I would have set the type as signed, like we did with ```ticks_t```. 
 
As a personal comment, I must say that I think that using that date as epoch was one of Dave Cutler's excentricities during the development of Windows NT. Although Raymond Chen has a blog entry that says it was a mathematical decision.

I say it was an excentricity because the Gregorian calendar actually starts on Friday 15th of October 1582, so Why not use that? Also, Dave Cutler came from the Unix world, where epoch -as said- is 1st of January 1970 because limitations on space and clock oscillators at the time. Therefore, I always found the choice curious, instead of using -for example- tenth of microseconds since Unix epoch. Anyway, here is the blog post from Raymond Chen about why Dave Cutler (NT's Chief Architect) chose 1601 as epoch:

- ```The FILETIME structure records time in the form of 100-nanosecond intervals since January 1, 1601. Why was that date chosen?  The Gregorian calendar operates on a 400-year cycle, and 1601 is the first year of the cycle that was active at the time Windows NT was being designed. In other words, it was chosen to make the math come out nicely.  I actually have the email from Dave Cutler confirming this.```

You can find this explanation here: https://devblogs.microsoft.com/oldnewthing/20090306-00/?p=18913

Anyway, the thing is that since Windows NT chose that unit, loads of stuff in the industry has chosen this model too for measuring time because it holds upto 59,000 years (59k) worth of 100-nanoseconds, as opposed to 137 years worth of seconds held by a 32 bits ```time_t```. Thus -as ```FILETIME```-, ```ticks_t``` can also hold 59k worth of 100-nanosencods units, but being signed ranges from -29.2k years to +29.2k.

## Issues with ```time_t``` 

Note that ```time_t``` varies in size depending on system and target data size, so you should not assume ```time_t``` is one size or the other to avoid issues. For example in Windows, since the release of Windows XP, the size of ```time_t``` is always 64 bits instead 32, while in Linux and macOS it depends on the compilation target (32 in ILP32 and 64 bits in LP64). So make sure your code does not assume ```time_t``` is one size or another. this is why we made ```ticks_t``` 64 bits in size, to minimize issues related with type size.

Another issue is that people doesn't understand that ```time_t``` represents System Time, which is **always** expressed as UTC and not local time. Let's look at this:

There are two types of time:

- **System Time**: Which refers to Coordinated Universal Time or UTC, and its always the same no mattter where about in the planet is the computer located.

- **Local Time**: Which referes to the time in the place where the computer belives it is located.

UTC time is always the same everywhere, but Local time changes as we move throught meridians on multiples of 1 hour in time or 15 degrees in longitude -note that 15 deegrees multiplied by 24 hours equals to 360 degrees-. In practice this offset is only theoretical, as the timezones are never regular because the countries borders; where some are in a timezones that doesn't match their geographical location -for example Spain-. The solution of this is resort to more than 24 timezones, which some of them having a difference of 30 to 45 minutes with the next, instead of a difference of one hour (15 degrees).

Local time is calculated like this:  ```local = (system + zone + saving)```

Where:

- **system** represents System Time (UTC).

- **zone** is the time offset from **system** and is positive if we go to the East of Greenwich Meridian and negative if we are going to the West of it. These days, the Greenwich Meridian is usually called *Prime Meridian* when talkign about time, not geography.

- **saving** is a time correction applied in some cities to save energy by adjusting time to daylight cycles. It is a complex matter because in the same country, some towns or states use it and some others don't. For example in USA, Californa actually uses it but Arizona doesn't.

To avoid mistakes, ambiguities and miscalculations with time (not very difficult to achieve), we **strongly recommend** to use ```time_t``` to hold time, making sure it does represents **only system time** (UTC). This is important because it is not strange to find code that assigns local time to ```time_t```.

So the advise is to handle time **always as system time**, and when you need to show it, convert it to hour, min, sec, etc.. as local time, which is the way people handle local time. This will surely avoid mistakes and inconsistencies.

In the code we also provide some other time types to ease handling different units:

- ```mtime_t``` Milliseconds elapsed since Unix epoch. Basically a time_t that holds milliseconds instead of seconds
- ```utime_t``` Microseconds elapsed since Unix epoch. Basically a time_t that holds microsends instead of seconds

Look into ```ticks.h```, as we provide functions to obtain and manage them, as well as interesting stuff like: stop watches, easter dates, calendar sheets and even a **gettimeofday()** function for Windows.

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

$./test_ticks

*** Test Ticks ***

Stop watch started...
It took 0.232767 seconds to excute the loop 134217727 times

Time has been taken and one of the stop watches started

Please move the system clock back some time and press enter:

You took 23.528 seconds to change time and press the key

Start CPU =2745994707744
End CPU   =2746229989476
Difference=2746229989476
Elapsed 23 secs 528 msecs 173 usecs

Start NIX =2745994707744
End NIX   =2746229989476
Difference=2746229989476
Elapsed -10780 secs -825 msecs -487 usecs

Put back the correct time and press enter again:

You took 6.464 seconds to change time and press the second key key

Start CPU =2745994707744
End CPU   =2746294626687
Difference=2746294626687
Elapsed 29 secs 991 msecs 894 usecs

Start NIX =2745994707744
End NIX   =2746294626687
Difference=2746294626687
Elapsed 29 secs 992 msecs 512 usecs

Total execution time was 30.225 seconds (29.992 of them changing time)

*** Time test ***

unix_ticks()    = 17544749717923230 
current_time()  = 1754474971 (1754474971 from time(NULL)
current_mtime() = 1754474971792 
current_utime() = 1754474971792324 

time_to_pot(1754474971,LOCAL) = Wed, Aug 6th 2025 at 12:09:31.0-0
time_from_pot(LOCAL) = 1754478571

mtime_to_pot(1754474971792,LOCAL) = Wed, Aug 6th 2025 at 12:09:31.792-0
mtime_from_pot(LOCAL) = 1754478571792

utime_to_pot(1754478571792,LOCAL) = Wed, Aug 6th 2025 at 12:09:31.792-324
utime_from_pot(LOCAL) = 1754478571792324

*** Test Calendar ***

---> Today starting on Sunday
calendar_sheet(0,0) Returned 8 of 2025
Easter falls on Sunday 20/04/2025

Sun Mon Tue Wed Thu Fri Sat 
27  28  29  30  31   1   2  
 3   4   5   6   7   8   9  
10  11  12  13  14  15  16  
17  18  19  20  21  22  23  
24  25  26  27  28  29  30  
31   1   2   3   4   5   6  

---> Today starting on Monday
calendar_sheet(1000,999999) Returned 8 of 2025
Easter falls on Sunday 20/04/2025

Mon Tue Wed Thu Fri Sat Sun 
28  29  30  31   1   2   3  
 4   5   6   7   8   9  10  
11  12  13  14  15  16  17  
18  19  20  21  22  23  24  
25  26  27  28  29  30  31  
 1   2   3   4   5   6   7  

---> April 1968 starting on Tuesday (only this month days)
calendar_sheet(4,1968) Returned 4 of 1968
Easter falls on Sunday 14/04/1968

Tue Wed Thu Fri Sat Sun Mon 
                         1  
 2   3   4   5   6   7   8  
 9  10  11  12  13  14  15  
16  17  18  19  20  21  22  
23  24  25  26  27  28  29  
30                          

---> August 2025 starting on Wednesday (only this month days)
calendar_sheet(8,2025) Returned 8 of 2025
Easter falls on Sunday 20/04/2025

Wed Thu Fri Sat Sun Mon Tue 
         1   2   3   4   5  
 6   7   8   9  10  11  12  
13  14  15  16  17  18  19  
20  21  22  23  24  25  26  
27  28  29  30  31          
                            

---> June 2006 starting on Thursday (only this month days)
calendar_sheet(6,2006) Returned 6 of 2006
Easter falls on Sunday 16/04/2006

Thu Fri Sat Sun Mon Tue Wed 
 1   2   3   4   5   6   7  
 8   9  10  11  12  13  14  
15  16  17  18  19  20  21  
22  23  24  25  26  27  28  
29  30                      
                            

---> February 2000 (leap) starting on Friday (only this month days)
calendar_sheet(2,2000) Returned 2 of 2000
Easter falls on Sunday 23/04/2000

Fri Sat Sun Mon Tue Wed Thu 
                 1   2   3  
 4   5   6   7   8   9  10  
11  12  13  14  15  16  17  
18  19  20  21  22  23  24  
25  26  27  28  29          
                            

---> April 1601 starting on Saturday (only this month days)
calendar_sheet(4,1601) Returned 4 of 1601
Easter falls on Sunday 22/04/1601

Sat Sun Mon Tue Wed Thu Fri 
     1   2   3   4   5   6  
 7   8   9  10  11  12  13  
14  15  16  17  18  19  20  
21  22  23  24  25  26  27  
28  29  30                  
                            

---> April 4601 starting on Sunday
calendar_sheet(4,4601) Returned 4 of 4601
Easter falls on Sunday 29/03/4601

Sun Mon Tue Wed Thu Fri Sat 
             1   2   3   4  
 5   6   7   8   9  10  11  
12  13  14  15  16  17  18  
19  20  21  22  23  24  25  
26  27  28  29  30          
                     
```




