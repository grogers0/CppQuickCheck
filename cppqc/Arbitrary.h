#ifndef CPPQC_ARBITRARY_H
#define CPPQC_ARBITRARY_H

#include "Generator.h"

#include <vector>
#include <limits>

#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/poisson_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace cppqc {

// default generators

template<class Integral>
Integral arbitrarySizedIntegral(RngEngine &rng, std::size_t size)
{
    boost::uniform_int<Integral> dist(std::numeric_limits<Integral>::is_signed ?
            -Integral(size) : Integral(size),
            Integral(size));
    return dist(rng);
}

template<class Integral>
Integral arbitraryBoundedIntegral(RngEngine &rng, std::size_t size)
{
    boost::uniform_int<Integral> dist(std::numeric_limits<Integral>::min(),
            std::numeric_limits<Integral>::max());
    return dist(rng);
}

template<class Integral>
Integral arbitrarySizedBoundedIntegral(RngEngine &rng, std::size_t size)
{
    boost::poisson_distribution<Integral> dist(size == 0 ? 1 : size);
    boost::variate_generator<RngEngine&, boost::uniform_01<> > gen(rng, boost::uniform_01<>());
    Integral r = dist(gen);
    if (std::numeric_limits<Integral>::is_signed) {
        if (boost::uniform_smallint<int>(0, 1)(rng))
            r = -r;
    }
    return r;
}

template<class Real>
Real arbitrarySizedReal(RngEngine &rng, std::size_t size)
{
    boost::uniform_real<Real> dist(-Real(size), Real(size));
    return dist(rng);
}

// default shrinkers

template<class T>
std::vector<T> shrinkNothing(const T &x)
{
    return std::vector<T>();
}

template<class Integral>
std::vector<Integral> shrinkIntegral(Integral x)
{
    std::vector<Integral> ret;
    if (x < 0 && -x > x)
        ret.push_back(-x);
    for (Integral n = x; n != 0; n /= 2)
        ret.push_back(x - n);
    return ret;
}

template<class Real>
std::vector<Real> shrinkReal(Real x)
{
    std::vector<Real> ret;
    if (x < 0)
        ret.push_back(-x);
    ret.push_back(Real(0));
    ret.push_back(x / Real(2));
    return ret;
}


template<class T>
struct Arbitrary
{
    typedef boost::function<T (RngEngine &, std::size_t)> unGenType;
    typedef boost::function<std::vector<T> (T)> shrinkType;

    static const unGenType unGen;
    static const shrinkType shrink;
};

/*
 * specialize ArbitraryImpl and implement the members:
 *     static const Arbitrary<T>::unGenType unGen;
 *     static const Arbitrary<T>::shrinkType shrink;
 */
template<class T>
struct ArbitraryImpl
{
    // no default implementation - users must specialize ArbitraryImpl
    // and give an implementation of unGen and shrink. If they do not
    // and they try to use Arbitrary<TheirClass>, a compile error will result.
};

template<class T>
const typename Arbitrary<T>::unGenType Arbitrary<T>::unGen =
ArbitraryImpl<T>::unGen;
template<class T>
const typename Arbitrary<T>::shrinkType Arbitrary<T>::shrink =
ArbitraryImpl<T>::shrink;

// included specializations

inline bool arbitraryBool(RngEngine &rng, std::size_t size)
{
    if (boost::uniform_smallint<int>(0, 1)(rng))
        return true;
    return false;
}
inline std::vector<bool> shrinkBool(bool x)
{
    std::vector<bool> ret;
    if (x) ret.push_back(false);
    return ret;
}
template<>
struct ArbitraryImpl<bool>
{
    static const Arbitrary<bool>::unGenType unGen;
    static const Arbitrary<bool>::shrinkType shrink;
};
const Arbitrary<bool>::unGenType ArbitraryImpl<bool>::unGen = arbitraryBool;
const Arbitrary<bool>::shrinkType ArbitraryImpl<bool>::shrink = shrinkBool;

template<class T1, class T2>
std::pair<T1, T2> arbitraryPair(RngEngine &rng, std::size_t size)
{
    return std::pair<T1, T2>(Arbitrary<T1>::unGen(rng, size),
            Arbitrary<T2>::unGen(rng, size));
}
template<class T1, class T2>
std::vector<std::pair<T1, T2> > shrinkPair(const std::pair<T1, T2> &x)
{
    std::vector<T1> shrinks1 = Arbitrary<T1>::shrink(x.first);
    std::vector<T2> shrinks2 = Arbitrary<T2>::shrink(x.second);
    std::vector<std::pair<T1, T2> > ret;
    for (typename std::vector<T1>::const_iterator it = shrinks1.begin();
            it != shrinks1.end(); ++it) {
        ret.push_back(std::make_pair(*it, x.second));
    }
    for (typename std::vector<T2>::const_iterator it = shrinks2.begin();
            it != shrinks2.end(); ++it) {
        ret.push_back(std::make_pair(x.first, *it));
    }
    return ret;
}
template<class T1, class T2>
struct ArbitraryImpl<std::pair<T1, T2> >
{
    static const typename Arbitrary<std::pair<T1, T2> >::unGenType unGen;
    static const typename Arbitrary<std::pair<T1, T2> >::shrinkType shrink;
};
template<class T1, class T2>
const typename Arbitrary<std::pair<T1, T2> >::unGenType
ArbitraryImpl<std::pair<T1, T2> >::unGen = arbitraryPair<T1, T2>;
template<class T1, class T2>
const typename Arbitrary<std::pair<T1, T2> >::shrinkType
ArbitraryImpl<std::pair<T1, T2> >::shrink = shrinkPair<T1, T2>;

template<>
struct ArbitraryImpl<signed char>
{
    static const Arbitrary<signed char>::unGenType unGen;
    static const Arbitrary<signed char>::shrinkType shrink;
};
const Arbitrary<signed char>::unGenType ArbitraryImpl<signed char>::unGen =
arbitrarySizedBoundedIntegral<signed char>;
const Arbitrary<signed char>::shrinkType ArbitraryImpl<signed char>::shrink =
shrinkIntegral<signed char>;

template<>
struct ArbitraryImpl<unsigned char>
{
    static const Arbitrary<unsigned char>::unGenType unGen;
    static const Arbitrary<unsigned char>::shrinkType shrink;
};
const Arbitrary<unsigned char>::unGenType ArbitraryImpl<unsigned char>::unGen =
arbitrarySizedBoundedIntegral<unsigned char>;
const Arbitrary<unsigned char>::shrinkType
ArbitraryImpl<unsigned char>::shrink = shrinkIntegral<unsigned char>;

template<>
struct ArbitraryImpl<signed short>
{
    static const Arbitrary<signed short>::unGenType unGen;
    static const Arbitrary<signed short>::shrinkType shrink;
};
const Arbitrary<signed short>::unGenType ArbitraryImpl<signed short>::unGen =
arbitrarySizedBoundedIntegral<signed short>;
const Arbitrary<signed short>::shrinkType ArbitraryImpl<signed short>::shrink =
shrinkIntegral<signed short>;

template<>
struct ArbitraryImpl<unsigned short>
{
    static const Arbitrary<unsigned short>::unGenType unGen;
    static const Arbitrary<unsigned short>::shrinkType shrink;
};
const Arbitrary<unsigned short>::unGenType
ArbitraryImpl<unsigned short>::unGen =
arbitrarySizedBoundedIntegral<unsigned short>;
const Arbitrary<unsigned short>::shrinkType
ArbitraryImpl<unsigned short>::shrink = shrinkIntegral<unsigned short>;

template<>
struct ArbitraryImpl<signed int>
{
    static const Arbitrary<signed int>::unGenType unGen;
    static const Arbitrary<signed int>::shrinkType shrink;
};
const Arbitrary<signed int>::unGenType ArbitraryImpl<signed int>::unGen =
arbitrarySizedBoundedIntegral<signed int>;
const Arbitrary<signed int>::shrinkType ArbitraryImpl<signed int>::shrink =
shrinkIntegral<signed int>;

template<>
struct ArbitraryImpl<unsigned int>
{
    static const Arbitrary<unsigned int>::unGenType unGen;
    static const Arbitrary<unsigned int>::shrinkType shrink;
};
const Arbitrary<unsigned int>::unGenType ArbitraryImpl<unsigned int>::unGen =
arbitrarySizedBoundedIntegral<unsigned int>;
const Arbitrary<unsigned int>::shrinkType ArbitraryImpl<unsigned int>::shrink =
shrinkIntegral<unsigned int>;

template<>
struct ArbitraryImpl<signed long>
{
    static const Arbitrary<signed long>::unGenType unGen;
    static const Arbitrary<signed long>::shrinkType shrink;
};
const Arbitrary<signed long>::unGenType ArbitraryImpl<signed long>::unGen =
arbitrarySizedBoundedIntegral<signed long>;
const Arbitrary<signed long>::shrinkType ArbitraryImpl<signed long>::shrink =
shrinkIntegral<signed long>;

template<>
struct ArbitraryImpl<unsigned long>
{
    static const Arbitrary<unsigned long>::unGenType unGen;
    static const Arbitrary<unsigned long>::shrinkType shrink;
};
const Arbitrary<unsigned long>::unGenType ArbitraryImpl<unsigned long>::unGen =
arbitrarySizedBoundedIntegral<unsigned long>;
const Arbitrary<unsigned long>::shrinkType ArbitraryImpl<unsigned long>::shrink =
shrinkIntegral<unsigned long>;

template<>
struct ArbitraryImpl<float>
{
    static const Arbitrary<float>::unGenType unGen;
    static const Arbitrary<float>::shrinkType shrink;
};
const Arbitrary<float>::unGenType ArbitraryImpl<float>::unGen =
arbitrarySizedReal<float>;
const Arbitrary<float>::shrinkType ArbitraryImpl<float>::shrink = shrinkReal<float>;

template<>
struct ArbitraryImpl<double>
{
    static const Arbitrary<double>::unGenType unGen;
    static const Arbitrary<double>::shrinkType shrink;
};
const Arbitrary<double>::unGenType ArbitraryImpl<double>::unGen =
arbitrarySizedReal<double>;
const Arbitrary<double>::shrinkType ArbitraryImpl<double>::shrink = shrinkReal<double>;

template<>
struct ArbitraryImpl<long double>
{
    static const Arbitrary<long double>::unGenType unGen;
    static const Arbitrary<long double>::shrinkType shrink;
};
const Arbitrary<long double>::unGenType ArbitraryImpl<long double>::unGen =
arbitrarySizedReal<long double>;
const Arbitrary<long double>::shrinkType ArbitraryImpl<long double>::shrink =
shrinkReal<long double>;

inline char arbitraryChar(RngEngine &rng, std::size_t)
{
    boost::uniform_int<char> dist(0x20, 0x7f);
    return dist(rng);
}
inline std::vector<char> shrinkChar(char c)
{
    char possShrinks[] = {'a', 'b', 'c', 'A', 'B', 'C', '1', '2', '3', ' ',
        '\n'};
    std::vector<char> ret;
    for (std::size_t i = 0; i < sizeof(possShrinks); ++i) {
        if (possShrinks[i] < c)
            ret.push_back(possShrinks[i]);
    }
    if (isupper(c) &&
            std::find(possShrinks, possShrinks + sizeof(possShrinks),
                tolower(c)) != possShrinks + sizeof(possShrinks))
        ret.push_back(tolower(c));
    return ret;
}
template<>
struct ArbitraryImpl<char>
{
    static const Arbitrary<char>::unGenType unGen;
    static const Arbitrary<char>::shrinkType shrink;
};
const Arbitrary<char>::unGenType ArbitraryImpl<char>::unGen = arbitraryChar;
const Arbitrary<char>::shrinkType ArbitraryImpl<char>::shrink = shrinkChar;

}

#endif
