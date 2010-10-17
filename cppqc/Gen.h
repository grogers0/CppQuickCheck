#ifndef CPPQC_GEN_H
#define CPPQC_GEN_H

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <iostream>
#include <vector>

namespace cppqc {

typedef boost::mt19937 RngEngine;

template<class T>
struct Gen
{
    typedef boost::function<T (RngEngine &, std::size_t)> type;
};

template<class T>
struct Shrink
{
    typedef boost::function<std::vector<T> (T)> type;
};

// generator combinators

// Used to create generators that depend on the size parameter.
template<class T>
typename Gen<T>::type sized(boost::function<typename Gen<T>::type (std::size_t)> f)
{
    return boost::bind(boost::bind(f, _2), _1, _2);
}

// Overrides the size parameter. Returns a generator which uses the given size
// instead of the runtime size parameter
template<class T>
typename Gen<T>::type resize(std::size_t size, typename Gen<T>::type f)
{
    return boost::bind(f, _1, size);
}

// Generates a random integer in the range min..max inclusive.
template<class Integer>
typename Gen<Integer>::type choose(Integer min, Integer max)
{
    boost::uniform_int<Integer> dist(min, max);
    return boost::bind(dist, _1);
}

// Generates some example values and returns them
template<class T>
std::vector<T> sample(typename Gen<T>::type g, std::size_t num = 20)
{
    RngEngine rng;
    std::vector<T> ret;
    ret.reserve(num);
    for (std::size_t i = 0; i < num; ++i)
        ret.push_back(g(rng, i));
    return ret;
}

// Generates some example values and returns them
template<class T>
void sampleOutput(typename Gen<T>::type g, std::ostream &out = std::cout,
        std::size_t num = 20)
{
    RngEngine rng;
    for (std::size_t i = 0; i < num; ++i)
        out << g(rng, i) << '\n';
}

namespace detail {
    template<class T>
    T suchThatFun(typename Gen<T>::type g,
            boost::function<bool (T)> pred, RngEngine &rng, std::size_t size)
    {
        T x = g(rng, size);
        if (!pred(x))
            throw std::runtime_error("generated value failed to satisfy predicate");
        return x;
    }
}

template<class T>
typename Gen<T>::type suchThat(typename Gen<T>::type g,
        boost::function<bool (T)> pred)
{
    return boost::bind(detail::suchThatFun, g, pred);
}

namespace detail {
    template<class T>
    class OneOfContainer
    {
        public:
            OneOfContainer &operator()(typename Gen<T>::type g)
            {
                m_cont.push_back(g);
                return *this;
            }

            T operator()(RngEngine &rng, std::size_t size) const
            {
                boost::uniform_int<std::size_t> dist(0, m_cont.size() - 1);
                return m_cont[dist(rng)](rng, size);
            }

        private:
            std::vector<typename Gen<T>::type> m_cont;
    };
}

template<class T>
detail::OneOfContainer<T> oneof(typename Gen<T>::type g)
{
    detail::OneOfContainer<T> ret;
    return ret(g);
}

namespace detail {
    template<class T>
    class FrequencyContainer
    {
        public:
            FrequencyContainer() : m_tot(0), m_cont() {}

            FrequencyContainer &operator()(std::size_t f,
                    typename Gen<T>::type g)
            {
                m_tot += f;
                m_cont.push_back(std::make_pair(f, g));
                return *this;
            }

            T operator()(RngEngine &rng, std::size_t size) const
            {
                boost::uniform_int<std::size_t> dist(1, m_tot);
                std::size_t n = dist(rng);
                // todo - improve to a O(log(n)) algorithm instead of O(n)
                for (typename Cont::const_iterator it = m_cont.begin();
                        it != m_cont.end(); n -= it->first, ++it) {
                    if (n <= it->first)
                        return it->second(rng, size);
                }
                assert(false);
            }

        private:
            std::size_t m_tot;
            typedef std::vector<std::pair<std::size_t,
                    typename Gen<T>::type> > Cont;
            Cont m_cont;
    };
}

template<class T>
detail::FrequencyContainer<T> frequency(std::size_t f, typename Gen<T>::type g)
{
    detail::FrequencyContainer<T> ret;
    return ret(f, g);
}

namespace detail {
    template<class T>
    class ElementsContainer
    {
        public:
            ElementsContainer &operator()(const T &x)
            {
                m_cont.push_back(x);
                return *this;
            }

            T operator()(RngEngine &rng, std::size_t size) const
            {
                boost::uniform_int<std::size_t> dist(0, m_cont.size() - 1);
                return m_cont[dist(rng)];
            }

        private:
            std::vector<T> m_cont;
    };
}

template<class T>
detail::ElementsContainer<T> elements(const T &x)
{
    detail::ElementsContainer<T> ret;
    return ret(x);
}

namespace detail {
    template<class T>
    std::vector<T> listOfFun(typename Gen<T>::type g, RngEngine &rng,
            std::size_t size)
    {
        std::size_t n = boost::uniform_int<std::size_t>(0, size)(rng);
        std::vector<T> ret;
        ret.reserve(n);
        while (n-- > 0)
            ret.push_back(g(rng, size));
        return ret;
    }
}

template<class T>
typename Gen<std::vector<T> >::type listOf(typename Gen<T>::type g)
{
    return boost::bind(detail::listOfFun, g);
}

namespace detail {
    template<class T>
    std::vector<T> listOf1Fun(typename Gen<T>::type g, RngEngine &rng,
            std::size_t size)
    {
        std::size_t n = boost::uniform_int<std::size_t>(1,
                std::max<std::size_t>(1, size))(rng);
        std::vector<T> ret;
        ret.reserve(n);
        while (n-- > 0)
            ret.push_back(g(rng, size));
        return ret;
    }
}

template<class T>
typename Gen<std::vector<T> >::type listOf1(typename Gen<T>::type g)
{
    return boost::bind(detail::listOf1Fun, g);
}

namespace detail {
    template<class T>
    std::vector<T> vectorOfFun(std::size_t n, typename Gen<T>::type g,
            RngEngine &rng, std::size_t size)
    {
        std::vector<T> ret;
        ret.reserve(n);
        while (n-- > 0)
            ret.push_back(g(rng, size));
        return ret;
    }
}

template<class T>
typename Gen<std::vector<T> >::type vectorOf(std::size_t n,
        typename Gen<T>::type g)
{
    return boost::bind(detail::vectorOfFun, n, g);
}





}

#endif
