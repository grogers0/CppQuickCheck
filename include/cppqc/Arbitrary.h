/*
 * Copyright (c) 2010, Gregory Rogers All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CPPQC_ARBITRARY_H
#define CPPQC_ARBITRARY_H

#include "Generator.h"

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
Integral arbitraryBoundedIntegral(RngEngine &rng, std::size_t /*size*/)
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
    boost::uniform_real<Real> dist(-Real(size + 1.0), Real(size + 1.0));
    return dist(rng);
}

// default shrinkers

template<class T>
std::vector<T> shrinkNothing(const T &)
{
    return std::vector<T>();
}

template<class Integral>
std::vector<Integral> shrinkIntegral(Integral x)
{
    std::vector<Integral> ret;
    if (std::numeric_limits<Integral>::is_signed && x < 0) {
        if (x == std::numeric_limits<Integral>::min()) {
            ret.push_back(std::numeric_limits<Integral>::max());
        } else {
            assert(-x > 0);
            ret.push_back(-x);
        }
    }

    for (Integral n = x; n != 0; n /= 2)
        ret.push_back(x - n);
    return ret;
}

template<class Real>
std::vector<Real> shrinkReal(Real x)
{
    std::vector<Real> ret;
    if (x == 0)
        return ret;
    if (x < 0)
        ret.push_back(-x);
    ret.push_back(Real(0));
    if (abs(x) >= 2) {
        ret.push_back(x / Real(2));
    }
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

// Note: The call is wrapped in a function to avoid issues
//       with static ordering when ArbitraryImpl is defined
//       in another compilation unit. Do not simplify it
//       by replacing it with an assignment.
template<class T>
const typename Arbitrary<T>::unGenType Arbitrary<T>::unGen = [](RngEngine &rng,
                                                                std::size_t size) {
    return ArbitraryImpl<T>::unGen(rng, size);
};

// (function call is needed: see above)
template<class T>
const typename Arbitrary<T>::shrinkType Arbitrary<T>::shrink = [](const T &v) {
    return ArbitraryImpl<T>::shrink(v);
};

// included specializations

inline bool arbitraryBool(RngEngine &rng, std::size_t /*size*/)
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

template<>
struct ArbitraryImpl<signed char>
{
    static const Arbitrary<signed char>::unGenType unGen;
    static const Arbitrary<signed char>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<unsigned char>
{
    static const Arbitrary<unsigned char>::unGenType unGen;
    static const Arbitrary<unsigned char>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<signed short>
{
    static const Arbitrary<signed short>::unGenType unGen;
    static const Arbitrary<signed short>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<unsigned short>
{
    static const Arbitrary<unsigned short>::unGenType unGen;
    static const Arbitrary<unsigned short>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<signed int>
{
    static const Arbitrary<signed int>::unGenType unGen;
    static const Arbitrary<signed int>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<unsigned int>
{
    static const Arbitrary<unsigned int>::unGenType unGen;
    static const Arbitrary<unsigned int>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<signed long>
{
    static const Arbitrary<signed long>::unGenType unGen;
    static const Arbitrary<signed long>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<unsigned long>
{
    static const Arbitrary<unsigned long>::unGenType unGen;
    static const Arbitrary<unsigned long>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<float>
{
    static const Arbitrary<float>::unGenType unGen;
    static const Arbitrary<float>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<double>
{
    static const Arbitrary<double>::unGenType unGen;
    static const Arbitrary<double>::shrinkType shrink;
};

template<>
struct ArbitraryImpl<long double>
{
    static const Arbitrary<long double>::unGenType unGen;
    static const Arbitrary<long double>::shrinkType shrink;
};

inline char arbitraryChar(RngEngine &rng, std::size_t)
{
    boost::uniform_int<char> dist(0x20, 0x7f);
    return dist(rng);
}
inline std::vector<char> shrinkChar(char c)
{
    const char possShrinks[] = {'a', 'b', 'c', 'A', 'B', 'C', '1', '2', '3',
        ' ', '\n', '\0'};
    std::vector<char> ret;
    for (auto & possShrink : possShrinks) {
        if (possShrink < c)
            ret.push_back(possShrink);
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

template<>
struct ArbitraryImpl<wchar_t>
{
    static const Arbitrary<wchar_t>::unGenType unGen;
    static const Arbitrary<wchar_t>::shrinkType shrink;
};

template<class String>
String arbitraryString(RngEngine &rng, std::size_t size)
{
    boost::uniform_int<std::size_t> dist(0, size);
    std::size_t n = dist(rng);
    String ret;
    ret.reserve(n);
    while (n-- > 0)
        ret.push_back(Arbitrary<typename String::value_type>::unGen(rng, size));
    return ret;
}
template<class String>
std::vector<String> shrinkString(const String &x)
{
    std::vector<String> ret;
    ret.reserve(x.size());
    for (auto it = x.begin(); it != x.end(); ++it) {
        ret.push_back(String());
        ret.back().reserve(x.size() - 1);
        ret.back().insert(ret.back().end(), x.begin(), it);
        ret.back().insert(ret.back().end(), it + 1, x.end());
    }
    return ret;
}
template<class CharT, class Traits, class Alloc>
struct ArbitraryImpl<std::basic_string<CharT, Traits, Alloc> >
{
    static const typename
        Arbitrary<std::basic_string<CharT, Traits, Alloc> >::unGenType unGen;
    static const typename
        Arbitrary<std::basic_string<CharT, Traits, Alloc> >::shrinkType shrink;
};
template<class CharT, class Traits, class Alloc>
const typename Arbitrary<std::basic_string<CharT, Traits, Alloc> >::unGenType
ArbitraryImpl<std::basic_string<CharT, Traits, Alloc> >::unGen =
arbitraryString<std::basic_string<CharT, Traits, Alloc> >;
template<class CharT, class Traits, class Alloc>
const typename Arbitrary<std::basic_string<CharT, Traits, Alloc> >::shrinkType
ArbitraryImpl<std::basic_string<CharT, Traits, Alloc> >::shrink =
shrinkString<std::basic_string<CharT, Traits, Alloc> >;

template<class PairType>
PairType arbitraryPair(RngEngine &rng, std::size_t size)
{
    return PairType(Arbitrary<typename PairType::first_type>::unGen(rng, size),
            Arbitrary<typename PairType::second_type>::unGen(rng, size));
}
template<class PairType>
std::vector<PairType> shrinkPair(const PairType &x)
{
    typedef typename PairType::first_type FirstType;
    typedef typename PairType::second_type SecondType;
    std::vector<FirstType> shrinks1 = Arbitrary<FirstType>::shrink(x.first);
    std::vector<SecondType> shrinks2 = Arbitrary<SecondType>::shrink(x.second);
    std::vector<PairType> ret;
    ret.reserve(shrinks1.size() + shrinks2.size());
    for (auto it = shrinks1.begin(); it != shrinks1.end(); ++it) {
        ret.push_back(PairType(*it, x.second));
    }
    for (auto it = shrinks2.begin(); it != shrinks2.end(); ++it) {
        ret.push_back(PairType(x.first, *it));
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
ArbitraryImpl<std::pair<T1, T2> >::unGen = arbitraryPair<std::pair<T1, T2> >;
template<class T1, class T2>
const typename Arbitrary<std::pair<T1, T2> >::shrinkType
ArbitraryImpl<std::pair<T1, T2> >::shrink = shrinkPair<std::pair<T1, T2> >;

template<typename T>
struct ArbitraryImpl<std::vector<T>>
{
  static const typename Arbitrary<std::vector<T>>::unGenType unGen;
  static const typename Arbitrary<std::vector<T>>::shrinkType shrink;
};

template <typename T>
const typename Arbitrary<std::vector<T>>::unGenType
    ArbitraryImpl<std::vector<T>>::unGen = [](RngEngine &rng,
                                              std::size_t size) {
    const auto& vectorGenerator = listOf<T>();
    return vectorGenerator.unGen(rng, size);
};

template <typename T>
const typename Arbitrary<std::vector<T>>::shrinkType
    ArbitraryImpl<std::vector<T>>::shrink = [](const std::vector<T> &v) {

    const auto& vectorGenerator = listOf<T>();
    return vectorGenerator.shrink(v);
};


template<typename T, std::size_t N>
struct ArbitraryImpl<std::array<T, N>>
{
    static const typename Arbitrary<std::array<T, N>>::unGenType unGen;
    static const typename Arbitrary<std::array<T, N>>::shrinkType shrink;
};

/// Note: N is the fixed size of the array.
/// It differs from the "size" param in the "unGen" function:
///
/// If "size" is increased, the output array will still contain
/// N elements, but each element will, in general, be more complex.
template <typename T, std::size_t N>
const typename Arbitrary<std::array<T, N>>::unGenType
    ArbitraryImpl<std::array<T, N>>::unGen = [](RngEngine &rng,
                                              std::size_t size) {
    const auto& arrayGenerator = arrayOf<T, N>();
    return arrayGenerator.unGen(rng, size);
};

template <typename T, std::size_t N>
const typename Arbitrary<std::array<T, N>>::shrinkType
    ArbitraryImpl<std::array<T, N>>::shrink = [](const std::array<T, N> &arr) {

    const auto& arrayGenerator = arrayOf<T, N>();
    return arrayGenerator.shrink(arr);
};

}

#endif
