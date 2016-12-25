***CppQuickCheck***

## ABOUT

CppQuickCheck is a library for automated testing of C++ code. You provide a
specification how your code is supposed to behave through *properties* and
CppQuickCheck will generate a large number of random test cases that attempt to
prove or disprove the properties. The specifications are written in pure C++
using the functionality of the CppQuickCheck library.

## DOCUMENTATION

You can find examples in 'examples/src'.

## BACKGROUND

CppQuickCheck is inspired and based on the
[QuickCheck](http://code.haskell.org/QuickCheck/) library for
Haskell. CppQuickCheck differs in some aspects of the implementation and
interface (as well as being written in C++ instead of Haskell), but tries to
maintain similar functionality to the QuickCheck library.

A similar library for C++ exists called
[QuickCheck++](http://software.legiasoft.com/quickcheck/). QuickCheck++ does
not support several important things that the Haskell QuickCheck supports
including:

  * *Generator combinators* - In QuickCheck++ custom generators for user
    defined types are written by hand, with no provided random number
    generation facility. One example of a way that Haskell's QuickCheck (and
    CppQuickCheck) improve on this is the function `oneof` whick takes a list
    of generators and creates a new generator that when called, selects a
    random generator from the list and uses that to generate the input. In
    QuickCheck++ this has to be written by hand using some external random
    number generating library.
  * *Shrinking the input for failed test cases* - When a randomly generated
    test case fails, Haskell's QuickCheck (and CppQuickCheck, but not
    QuickCheck++) will try to shrink the input to provide a minimal failing
    case. This makes debugging the failure easier because instead of working
    with a potentially large input case, much of the input can be removed to
    reveal the structure of the failure.

For these reasons, CppQuickCheck tries to maintain all the functionality of the
Haskell QuickCheck, and so taking a different implementation approach than
QuickCheck++.

## UNIT TESTS

Unit tests exists in the directory 'test'.

```
$ cmake .
$ make check
```

To get a more detailed output, rerun it with

```
$ ./all-catch-tests
```

## LICENSE

CppQuickCheck is distributed under a BSD license (see LICENSE).
