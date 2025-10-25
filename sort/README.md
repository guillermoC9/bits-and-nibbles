# Sorting and Searching
## Learning how to sort and search stuff in arrays of data structures

Unlike most of this repository, this code here only contains a programa with some tests instead of a library.

### Sorting

In this test you will see how differences between the initial state of a list will affect the speed of sorting it, and how it will make a *theoretically more efficient* algorithm behave worse than a *theoretically slow* algorithm. That is the [**Cocktail Shaker Sort**](https://en.wikipedia.org/wiki/Cocktail_shaker_sort), which is regarded as more efficient than the [**Bubble Sort**](https://en.wikipedia.org/wiki/Bubble_sort).  It also affects how [**Quick Sort**](https://en.wikipedia.org/wiki/Quicksort) performs, but this algorithm always outperform the others.

If you look the execution output you will see that we test the 3 algorithms using several versions of a sorted list of 3001 words, which we reverse, unsort and make another which has the bottom half of the list (still ordered) on the top, and the top half in the bottom (halved).

The outputs are quite interesting, as if we use the sorted list is very fast, although ```qsort()``` doubles the compares than the other two algorithms. So even if the three mark 0 as the speed, that one should have take longer.

- When using the halved list, surprisingly ```buble_sort()``` outperforms quite heavily ```cocktail_shaker()```, which more than duplicate the number of compares (the changes are obviously the same) but none of them can compete with ```qsort()``` which actually uses a fraction of the compares than the other two (we cannot measure changes in this algorithm).

- When using the reversed list, ```buble_sort()``` performs similar to ```cocktail_shaker()```, with exactly the same number of compares. Again ```qsort()``` overperform the others and uses a fraction of the compares.

- When using the unsorted list, ```cocktail_sort()``` unsurprisingly outperforms ```bubble_shaker()```, and uses less than half compares, which is why this algorithm is preferred for assword stuff. Of course, ```qsort()``` overperform the others again and uses a fraction of the compares.

A last sorting experiment consits on using the sorted list of words to build a list of word sorted by the lenght of the word, an alphabetically sorted in each lenght list. This let's you see how to achieve this kind of sort, as well as to see that ```qsort()``` keeps beating the other two, which differ in speed depending -like onthe other tests- depending on the original sort of the list.

To see the stuff and let you play with them, the otupu program creates a set of txt files with the different sortings of the list.

### Searching

The test here is just to show how effient is a binary search when comnpared with a sequential search on a sorted word list. We search different words of each size and few more and compare results, as expected, the binary search greatly overperform the sequential search,  especially because the number of compares is tiny (never bigger than 12). 

There are also a couple of unexistent words to show the difference of behaviour when the stuff is not found. You can learn about binary search in [**Wikipedia**](https://en.wikipedia.org/wiki/Binary_search).

Obviously you cannot use binary search on a list with is not ordered.

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

$ ./test_sort

*** Test Sort ***

Stop watch started...
Loaded 3001 words
It took 0.008318 seconds to Load, Half, Reverse and Unsort the words

It took 0.000 seconds, 0 changes and 3000 compares to sort sorted using bubble_sort() 
It took 0.000 seconds, 0 changes and 3000 compares to sort sorted using cocktail_shaker()
It took 0.000 seconds, 0 changes and 6009 compares to sort sorted using qsort()

It took 0.034 seconds, 2251500 changes and 4506000 compares to sort halved using bubble_sort()
It took 0.091 seconds, 2251500 changes and 9003000 compares to sort halved using cocktail_shaker()
It took 0.000 seconds, 0 changes and 26988 compares to sort halved using qsort()

It took 0.068 seconds, 4501499 changes and 9003000 compares to sort reversed using bubble_sort()
It took 0.073 seconds, 4501499 changes and 9003000 compares to sort reversed using cocktail_shaker()
It took 0.000 seconds, 0 changes and 29490 compares to sort reversed using qsort()

It took 0.092 seconds, 2231425 changes and 8640000 compares to sort unsorted using bubble_sort()
It took 0.043 seconds, 2231425 changes and 4521000 compares to sort unsorted using cocktail_shaker()
It took 0.000 seconds, 0 changes and 35585 compares to sort unsorted using qsort()

It took 0.086 seconds, 2247686 changes and 8883000 compares to sort by len using bubble_sort()
It took 0.038 seconds, 2247686 changes and 4554000 compares to sort by len using cocktail_shaker()
It took 0.000 seconds, 0 changes and 35705 compares to sort by len using qsort()

Total execution time was 0.536 seconds

*** Test Search ***

Stop watch started...
Loaded 3001 words
It took 0.006972 seconds to Load, Half, Reverse and Unsort the words

It took 0.611 seconds, to sequentially search 'pm' and it was FOUND after 1974 compares
It took 0.003 seconds, to binary search 'pm' and it was FOUND after 9 compares

It took 0.441 seconds, to sequentially search 'job' and it was FOUND after 1446 compares
It took 0.003 seconds, to binary search 'job' and it was FOUND after 11 compares

It took 0.089 seconds, to sequentially search 'bird' and it was FOUND after 291 compares
It took 0.004 seconds, to binary search 'bird' and it was FOUND after 12 compares

It took 0.490 seconds, to sequentially search 'marry' and it was FOUND after 1611 compares
It took 0.003 seconds, to binary search 'marry' and it was FOUND after 9 compares

It took 0.908 seconds, to sequentially search 'worker' and it was FOUND after 2971 compares
It took 0.004 seconds, to binary search 'worker' and it was FOUND after 12 compares

It took 0.340 seconds, to sequentially search 'forward' and it was FOUND after 1107 compares
It took 0.003 seconds, to binary search 'forward' and it was FOUND after 9 compares

It took 0.822 seconds, to sequentially search 'terrible' and it was FOUND after 2673 compares
It took 0.002 seconds, to binary search 'terrible' and it was FOUND after 6 compares

It took 0.811 seconds, to sequentially search 'tablespoon' and it was FOUND after 2631 compares
It took 0.003 seconds, to binary search 'tablespoon' and it was FOUND after 9 compares

It took 0.747 seconds, to sequentially search 'significant' and it was FOUND after 2417 compares
It took 0.003 seconds, to binary search 'significant' and it was FOUND after 10 compares

It took 0.798 seconds, to sequentially search 'surprisingly' and it was FOUND after 2613 compares
It took 0.004 seconds, to binary search 'surprisingly' and it was FOUND after 12 compares

It took 0.915 seconds, to sequentially search 'zone' and it was FOUND after 3001 compares
It took 0.004 seconds, to binary search 'zone' and it was FOUND after 12 compares

It took 0.873 seconds, to sequentially search 'violence' and it was FOUND after 2869 compares
It took 0.003 seconds, to binary search 'violence' and it was FOUND after 10 compares

It took 0.906 seconds, to sequentially search 'writing' and it was FOUND after 2984 compares
It took 0.003 seconds, to binary search 'writing' and it was FOUND after 9 compares

It took 0.912 seconds, to sequentially search 'yourself' and it was FOUND after 2999 compares
It took 0.003 seconds, to binary search 'yourself' and it was FOUND after 10 compares

It took 0.919 seconds, to sequentially search 'YouCannotFindMe' and it was NOT FOUND after 3001 compares
It took 0.003 seconds, to binary search 'YouCannotFindMe' and it was NOT FOUND after 11 compares

It took 0.915 seconds, to sequentially search 'not_here' and it was NOT FOUND after 3001 compares
It took 0.004 seconds, to binary search 'not_here' and it was NOT FOUND after 12 compares

Total execution time was 11.555 seconds

                     
```




