#ifndef CPPQC_ARBITRARY_H
#define CPPQC_ARBITRARY_H

#include "Gen.h"

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
    static const typename Gen<T>::type generator;
    static const typename Shrink<T>::type shrink;
};

template<class T>
T arbitraryThrow(RngEngine &, std::size_t)
{
    throw std::logic_error("no default generator");
}

/*
 * specialize ArbitraryImpl and implement the members:
 *     static const Gen<T>::type generator;
 *     static const Shrink<T>::type shrink;
 */
template<class T>
struct ArbitraryImpl
{
    // no default implementation - users must specialize ArbitraryImpl
    // and give an implementation of generator and shrink. If they do not
    // and they try to use Arbitrary<TheirClass>, a compile error will result.
};

template<class T>
const typename Gen<T>::type Arbitrary<T>::generator = ArbitraryImpl<T>::generator;
template<class T>
const typename Shrink<T>::type Arbitrary<T>::shrink = ArbitraryImpl<T>::shrink;

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
    static const Gen<bool>::type generator;
    static const Shrink<bool>::type shrink;
};
const Gen<bool>::type ArbitraryImpl<bool>::generator = arbitraryBool;
const Shrink<bool>::type ArbitraryImpl<bool>::shrink = shrinkBool;

template<class T1, class T2>
std::pair<T1, T2> arbitraryPair(RngEngine &rng, std::size_t size)
{
    return std::pair<T1, T2>(Arbitrary<T1>::generator(rng, size),
            Arbitrary<T2>::generator(rng, size));
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
    static const typename Gen<std::pair<T1, T2> >::type generator;
    static const typename Shrink<std::pair<T1, T2> >::type shrink;
};
template<class T1, class T2>
const typename Gen<std::pair<T1, T2> >::type
ArbitraryImpl<std::pair<T1, T2> >::generator = arbitraryPair<T1, T2>;
template<class T1, class T2>
const typename Shrink<std::pair<T1, T2> >::type
ArbitraryImpl<std::pair<T1, T2> >::shrink = shrinkPair<T1, T2>;

template<>
struct ArbitraryImpl<signed char>
{
    static const Gen<signed char>::type generator;
    static const Shrink<signed char>::type shrink;
};
const Gen<signed char>::type ArbitraryImpl<signed char>::generator =
arbitrarySizedBoundedIntegral<signed char>;
const Shrink<signed char>::type ArbitraryImpl<signed char>::shrink =
shrinkIntegral<signed char>;

template<>
struct ArbitraryImpl<unsigned char>
{
    static const Gen<unsigned char>::type generator;
    static const Shrink<unsigned char>::type shrink;
};
const Gen<unsigned char>::type ArbitraryImpl<unsigned char>::generator =
arbitrarySizedBoundedIntegral<unsigned char>;
const Shrink<unsigned char>::type ArbitraryImpl<unsigned char>::shrink =
shrinkIntegral<unsigned char>;

template<>
struct ArbitraryImpl<signed short>
{
    static const Gen<signed short>::type generator;
    static const Shrink<signed short>::type shrink;
};
const Gen<signed short>::type ArbitraryImpl<signed short>::generator =
arbitrarySizedBoundedIntegral<signed short>;
const Shrink<signed short>::type ArbitraryImpl<signed short>::shrink =
shrinkIntegral<signed short>;

template<>
struct ArbitraryImpl<unsigned short>
{
    static const Gen<unsigned short>::type generator;
    static const Shrink<unsigned short>::type shrink;
};
const Gen<unsigned short>::type ArbitraryImpl<unsigned short>::generator =
arbitrarySizedBoundedIntegral<unsigned short>;
const Shrink<unsigned short>::type ArbitraryImpl<unsigned short>::shrink =
shrinkIntegral<unsigned short>;

template<>
struct ArbitraryImpl<signed int>
{
    static const Gen<signed int>::type generator;
    static const Shrink<signed int>::type shrink;
};
const Gen<signed int>::type ArbitraryImpl<signed int>::generator =
arbitrarySizedBoundedIntegral<signed int>;
const Shrink<signed int>::type ArbitraryImpl<signed int>::shrink =
shrinkIntegral<signed int>;

template<>
struct ArbitraryImpl<unsigned int>
{
    static const Gen<unsigned int>::type generator;
    static const Shrink<unsigned int>::type shrink;
};
const Gen<unsigned int>::type ArbitraryImpl<unsigned int>::generator =
arbitrarySizedBoundedIntegral<unsigned int>;
const Shrink<unsigned int>::type ArbitraryImpl<unsigned int>::shrink =
shrinkIntegral<unsigned int>;

template<>
struct ArbitraryImpl<signed long>
{
    static const Gen<signed long>::type generator;
    static const Shrink<signed long>::type shrink;
};
const Gen<signed long>::type ArbitraryImpl<signed long>::generator =
arbitrarySizedBoundedIntegral<signed long>;
const Shrink<signed long>::type ArbitraryImpl<signed long>::shrink =
shrinkIntegral<signed long>;

template<>
struct ArbitraryImpl<unsigned long>
{
    static const Gen<unsigned long>::type generator;
    static const Shrink<unsigned long>::type shrink;
};
const Gen<unsigned long>::type ArbitraryImpl<unsigned long>::generator =
arbitrarySizedBoundedIntegral<unsigned long>;
const Shrink<unsigned long>::type ArbitraryImpl<unsigned long>::shrink =
shrinkIntegral<unsigned long>;

template<>
struct ArbitraryImpl<float>
{
    static const Gen<float>::type generator;
    static const Shrink<float>::type shrink;
};
const Gen<float>::type ArbitraryImpl<float>::generator =
arbitrarySizedReal<float>;
const Shrink<float>::type ArbitraryImpl<float>::shrink = shrinkReal<float>;

template<>
struct ArbitraryImpl<double>
{
    static const Gen<double>::type generator;
    static const Shrink<double>::type shrink;
};
const Gen<double>::type ArbitraryImpl<double>::generator =
arbitrarySizedReal<double>;
const Shrink<double>::type ArbitraryImpl<double>::shrink = shrinkReal<double>;

template<>
struct ArbitraryImpl<long double>
{
    static const Gen<long double>::type generator;
    static const Shrink<long double>::type shrink;
};
const Gen<long double>::type ArbitraryImpl<long double>::generator =
arbitrarySizedReal<long double>;
const Shrink<long double>::type ArbitraryImpl<long double>::shrink =
shrinkReal<long double>;

template<class T>
typename Gen<std::vector<T> >::type vector(std::size_t n)
{
    return vectorOf(n, Arbitrary<T>::generator);
}

// todo - add a char implementation

}

#endif
