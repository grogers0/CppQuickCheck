#include "cppqc/Arbitrary.h"

namespace cppqc {

const Arbitrary<bool>::unGenType ArbitraryImpl<bool>::unGen = arbitraryBool;
const Arbitrary<bool>::shrinkType ArbitraryImpl<bool>::shrink = shrinkBool;

const Arbitrary<signed char>::unGenType ArbitraryImpl<signed char>::unGen =
    arbitrarySizedBoundedIntegral<signed char>;
const Arbitrary<signed char>::shrinkType ArbitraryImpl<signed char>::shrink =
    shrinkIntegral<signed char>;

const Arbitrary<unsigned char>::unGenType ArbitraryImpl<unsigned char>::unGen =
    arbitrarySizedBoundedIntegral<unsigned char>;
const Arbitrary<unsigned char>::shrinkType
    ArbitraryImpl<unsigned char>::shrink = shrinkIntegral<unsigned char>;

const Arbitrary<signed short>::unGenType ArbitraryImpl<signed short>::unGen =
    arbitrarySizedBoundedIntegral<signed short>;
const Arbitrary<signed short>::shrinkType ArbitraryImpl<signed short>::shrink =
    shrinkIntegral<signed short>;

const Arbitrary<unsigned short>::unGenType
    ArbitraryImpl<unsigned short>::unGen =
    arbitrarySizedBoundedIntegral<unsigned short>;
const Arbitrary<unsigned short>::shrinkType
    ArbitraryImpl<unsigned short>::shrink = shrinkIntegral<unsigned short>;

const Arbitrary<signed int>::unGenType ArbitraryImpl<signed int>::unGen =
    arbitrarySizedBoundedIntegral<signed int>;
const Arbitrary<signed int>::shrinkType ArbitraryImpl<signed int>::shrink =
    shrinkIntegral<signed int>;

const Arbitrary<unsigned int>::unGenType ArbitraryImpl<unsigned int>::unGen =
    arbitrarySizedBoundedIntegral<unsigned int>;
const Arbitrary<unsigned int>::shrinkType ArbitraryImpl<unsigned int>::shrink =
    shrinkIntegral<unsigned int>;

const Arbitrary<signed long>::unGenType ArbitraryImpl<signed long>::unGen =
    arbitrarySizedBoundedIntegral<signed long>;
const Arbitrary<signed long>::shrinkType ArbitraryImpl<signed long>::shrink =
    shrinkIntegral<signed long>;

const Arbitrary<unsigned long>::unGenType ArbitraryImpl<unsigned long>::unGen =
    arbitrarySizedBoundedIntegral<unsigned long>;
const Arbitrary<unsigned long>::shrinkType ArbitraryImpl<unsigned long>::shrink =
    shrinkIntegral<unsigned long>;

const Arbitrary<float>::unGenType ArbitraryImpl<float>::unGen =
    arbitrarySizedReal<float>;
const Arbitrary<float>::shrinkType ArbitraryImpl<float>::shrink = shrinkReal<float>;

const Arbitrary<double>::unGenType ArbitraryImpl<double>::unGen =
    arbitrarySizedReal<double>;
const Arbitrary<double>::shrinkType ArbitraryImpl<double>::shrink = shrinkReal<double>;

const Arbitrary<long double>::unGenType ArbitraryImpl<long double>::unGen =
    arbitrarySizedReal<long double>;
const Arbitrary<long double>::shrinkType ArbitraryImpl<long double>::shrink =
    shrinkReal<long double>;

const Arbitrary<char>::unGenType ArbitraryImpl<char>::unGen = arbitraryChar;
const Arbitrary<char>::shrinkType ArbitraryImpl<char>::shrink = shrinkChar;

const Arbitrary<wchar_t>::unGenType ArbitraryImpl<wchar_t>::unGen =
    arbitraryBoundedIntegral<wchar_t>;
const Arbitrary<wchar_t>::shrinkType ArbitraryImpl<wchar_t>::shrink =
    shrinkIntegral<wchar_t>;

}
