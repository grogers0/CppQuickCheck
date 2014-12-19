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

#ifndef CPPQC_GEN_H
#define CPPQC_GEN_H

#include <boost/function.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/tuple/tuple.hpp>
#include <cstddef>
#include <iosfwd>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>

namespace cppqc {

typedef boost::mt19937 RngEngine;

template<class T> struct Arbitrary;

/*
 * When creating user defined generators, they must model a
 * "GeneratorConcept<T>", that is, they must be copy-constructable and have
 * member functions compatible with the following signatures:
 * 
 *      T unGen(RngEngine &, std::size_t);
 *      std::vector<T> shrink(const T &);
 * 
 * The unGen function generates a new value, and the shrink function shrinks
 * the last previously generated value down - generating an std::vector<T> of
 * possible alternative shrinks. It possible to create a stateful generator
 * modelling GenConcept, which in the generator stores some state in unGen that
 * is looked up when calling shrink. Note however that the shrink function may
 * be called multiple times, the subsequent calls may be called with the
 * argument being one of the values returned by the previous call to shrink.
 * This limits the types of statefulness possible.
 * 
 * GeneratorConcept<T>:
 *      T unGen(RngEngine &, std::size_t);
 *      std::vector<T> shrink(const T &);
 * 
 * StatelessGeneratorConcept<T> (also models GeneratorConcept<T>):
 *      T unGen(RngEngine &, std::size_t) const;
 *      std::vector<T> shrink(const T &) const;
 */


namespace detail {
    template<class T>
    struct GenConcept
    {
        virtual ~GenConcept()
        {
        }
        virtual T unGen(RngEngine &, std::size_t) = 0;
        virtual std::vector<T> shrink(const T &) = 0;
        virtual GenConcept *clone() const = 0;
    };

    template<class T>
    struct StatelessGenConcept : GenConcept<T>
    {
        virtual ~StatelessGenConcept()
        {
        }
        virtual T unGen(RngEngine &, std::size_t) override = 0;
        virtual std::vector<T> shrink(const T &) override = 0;
        virtual StatelessGenConcept *clone() const override = 0;
    };
}

template<class T>
class Generator;

template<class T>
class StatelessGenerator
{
        template<class StatelessGeneratorModel> class StatelessGenModel;
    public:
        template<class StatelessGeneratorModel>
        StatelessGenerator(const StatelessGeneratorModel &gm) :
            m_gen(new StatelessGenModel<StatelessGeneratorModel>(gm))
        {
        }

        StatelessGenerator(const StatelessGenerator &g) :
            m_gen(g.m_gen->clone())
        {
        }

        template<class StatelessGeneratorModel>
        StatelessGenerator &operator=(const StatelessGeneratorModel &gm)
        {
            detail::StatelessGenConcept<T> *tmp =
                new StatelessGenModel<StatelessGeneratorModel>(gm);
            delete m_gen;
            m_gen = tmp;
            return *this;
        }

        StatelessGenerator &operator=(const StatelessGenerator &g)
        {
            detail::StatelessGenConcept<T> *tmp = g.m_gen.clone();
            delete m_gen;
            m_gen = tmp;
            return *this;
        }

        ~StatelessGenerator()
        {
            delete m_gen;
        }

        T unGen(RngEngine &rng, std::size_t size) const
        {
            return m_gen->unGen(rng, size);
        }

        std::vector<T> shrink(const T &x) const
        {
            return m_gen->shrink(x);
        }

    private:
        template<class StatelessGeneratorModel>
        class StatelessGenModel : public detail::StatelessGenConcept<T>
        {
            public:
                StatelessGenModel(StatelessGeneratorModel gm) : m_obj(std::move(gm))
                {
                }

                T unGen(RngEngine &rng, std::size_t size)
                {
                    return m_obj.unGen(rng, size);
                }

                T unGen(RngEngine &rng, std::size_t size) const
                {
                    return m_obj.unGen(rng, size);
                }

                std::vector<T> shrink(const T &x)
                {
                    return m_obj.shrink(x);
                }

                std::vector<T> shrink(const T &x) const
                {
                    return m_obj.shrink(x);
                }

                detail::StatelessGenConcept<T> *clone() const
                {
                    return new StatelessGenModel(m_obj);
                }

            private:
                const StatelessGeneratorModel m_obj;
        };

        friend class Generator<T>;
        detail::StatelessGenConcept<T> *m_gen;
};

template<class T>
class Generator
{
        template<class GeneratorModel> class GenModel;
    public:
        template<class GeneratorModel>
        Generator(const GeneratorModel &gm) :
            m_gen(new GenModel<GeneratorModel>(gm))
        {
        }

        Generator(const Generator &g) :
            m_gen(g.m_gen->clone())
        {
        }

        Generator(const StatelessGenerator<T> &g) :
            m_gen(g.m_gen->clone())
        {
        }

        template<class GeneratorModel>
        Generator &operator=(const GeneratorModel &gm)
        {
            detail::GenConcept<T> *tmp = new GenModel<GeneratorModel>(gm);
            delete m_gen;
            m_gen = tmp;
            return *this;
        }

        Generator &operator=(const Generator &g)
        {
            detail::GenConcept<T> *tmp = g.m_gen->clone();
            delete m_gen;
            m_gen = tmp;
            return *this;
        }

        Generator &operator=(const StatelessGenerator<T> &g)
        {
            detail::GenConcept<T> *tmp = g.m_gen->clone();
            delete m_gen;
            m_gen = tmp;
            return *this;
        }

        ~Generator()
        {
            delete m_gen;
        }

        T unGen(RngEngine &rng, std::size_t size) const
        {
            return m_gen->unGen(rng, size);
        }

        std::vector<T> shrink(const T &x) const
        {
            return m_gen->shrink(x);
        }

    private:
        template<class GeneratorModel>
        class GenModel : public detail::GenConcept<T>
        {
            public:
                GenModel(GeneratorModel gm) : m_obj(std::move(gm))
                {
                }

                T unGen(RngEngine &rng, std::size_t size) override
                {
                    return m_obj.unGen(rng, size);
                }

                std::vector<T> shrink(const T &x) override
                {
                    return m_obj.shrink(x);
                }

                detail::GenConcept<T> *clone() const override
                {
                    return new GenModel(m_obj);
                }

            private:
                GeneratorModel m_obj;
        };

        detail::GenConcept<T> *m_gen;
};


/// Generates some example values and returns them.
template<class T>
std::vector<T> sample(const Generator<T> &g, std::size_t num = 0,
        std::size_t seed = 0)
{
    if (num == 0)
        num = 20;
    if (seed == 0)
        seed = time(nullptr);
    RngEngine rng(seed);
    std::vector<T> ret;
    ret.reserve(num);
    try {
        for (std::size_t i = 0; i < num; ++i)
            ret.push_back(g.unGen(rng, i));
    } catch (...) {
    }
    return ret;
}

/// Generates some example values and prints them.
template<class T>
void sampleOutput(const Generator<T> &g,
        std::ostream &out, std::size_t num = 0,
        std::size_t seed = 0)
{
    if (num == 0)
        num = 20;
    if (seed == 0)
        seed = time(nullptr);
    RngEngine rng(seed);
    try {
        for (std::size_t i = 0; i < num; ++i) {
            if (i != 0)
                out << ' ';
            out << g.unGen(rng, i);
        }
    } catch (...) {
    }
    out << std::endl;
}

/// Generates some example values, then generates a list of possible shrinks
/// for each value and returns both the value and the shrinks.
template<class T>
std::vector<std::pair<T, std::vector<T> > > sampleShrink(const Generator<T> &g,
        std::size_t num = 0, std::size_t seed = 0)
{
    if (num == 0)
        num = 20;
    if (seed == 0)
        seed = time(nullptr);
    RngEngine rng(seed);
    std::vector<std::pair<T, std::vector<T> > > ret;
    ret.reserve(num);
    try {
        for (std::size_t i = 0; i < num; ++i) {
            T x = g.unGen(rng, i);
            ret.push_back(std::make_pair(x, g.shrink(x)));
        }
    } catch (...) {
    }
    return ret;
}

/// Outputs some example values from a generator. For each value, outputs
/// several possible shrinks of that value.
template<class T>
void sampleShrinkOutput(const Generator<T> &g, std::ostream &out,
        std::size_t num = 0, bool randomized = false, std::size_t seed = 0)
{
    if (num == 0)
        num = 20;
    if (seed == 0)
        seed = time(nullptr);
    RngEngine rng(seed);
    try {
        for (std::size_t i = 0; i < num; ++i) {
            T x = g.unGen(rng, i);
            std::vector<T> shr = g.shrink(x);
            if (randomized)
                std::random_shuffle(shr.begin(), shr.end());
            out << x << " ->";
            for (std::size_t j = 0; j < num && j < shr.size(); ++j)
                out << ' ' << shr[j];
            out << '\n';
        }
    } catch (...) {
    }
    out << std::flush;
}


// generator combinators

namespace detail {
    template<class T>
    class NoShrinkGenerator
    {
        public:
            NoShrinkGenerator(const Generator<T> &g) :
                m_gen(g)
            {
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                return m_gen.unGen(rng, size);
            }

            std::vector<T> shrink(const T &x)
            {
                return std::vector<T>();
            }

        private:
            Generator<T> m_gen;
    };

    template<class T>
    class NoShrinkStatelessGenerator
    {
        public:
            NoShrinkStatelessGenerator(const Generator<T> &g) :
                m_gen(g)
            {
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                return m_gen.unGen(rng, size);
            }

            std::vector<T> shrink(const T &x)
            {
                return std::vector<T>();
            }

        private:
            StatelessGenerator<T> m_gen;
    };
}

// Generates a value the same way as the input generator, but doesn't shrink.
template<class T>
Generator<T> noShrink(const Generator<T> &g)
{
    return detail::NoShrinkGenerator<T>(g);
}
template<class T>
StatelessGenerator<T> noShrink(const StatelessGenerator<T> &g)
{
    return detail::NoShrinkStatelessGenerator<T>(g);
}


namespace detail {
    template<class T>
    class SizedGenerator
    {
        public:
            SizedGenerator(boost::function<Generator<T> (std::size_t)> f) :
                m_genfun(f), m_lastgen(f(0))
            {
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                m_lastgen = m_genfun(size);
                return m_lastgen.unGen(rng, size);
            }

            std::vector<T> shrink(const T &x)
            {
                return m_lastgen.shrink(x);
            }

        private:
            const boost::function<Generator<T> (std::size_t)> m_genfun;
            Generator<T> m_lastgen;
    };
}

/// Used to create generators that depend on the size parameter.
template<class T>
Generator<T> sized(boost::function<Generator<T> (std::size_t)> f)
{
    return detail::SizedGenerator<T>(f);
}


namespace detail {
    template<class T>
    class ResizeGenerator
    {
        public:
            ResizeGenerator(std::size_t size, const Generator<T> &g) :
                m_size(size), m_gen(g)
            {
            }

            T unGen(RngEngine &rng, std::size_t)
            {
                return m_gen.unGen(rng, m_size);
            }

            std::vector<T> shrink(const T &x)
            {
                return m_gen.shrink(x);
            }

        private:
            const std::size_t m_size;
            const Generator<T> m_gen;
    };

    template<class T>
    class ResizeStatelessGenerator
    {
        public:
            ResizeStatelessGenerator(std::size_t size,
                    const StatelessGenerator<T> &g) :
                m_size(size), m_gen(g)
            {
            }

            T unGen(RngEngine &rng, std::size_t) const
            {
                return m_gen.unGen(rng, m_size);
            }

            std::vector<T> shrink(const T &x) const
            {
                return m_gen.shrink(x);
            }

        private:
            const std::size_t m_size;
            const StatelessGenerator<T> m_gen;
    };
}

/// Overrides the size parameter. Returns a generator which uses the given size
/// instead of the runtime size parameter.
template<class T>
Generator<T> resize(std::size_t size, const Generator<T> &g)
{
    return detail::ResizeGenerator<T>(size, g);
}
template<class T>
StatelessGenerator<T> resize(std::size_t size, const StatelessGenerator<T> &g)
{
    return detail::ResizeStatelessGenerator<T>(size, g);
}


namespace detail {
    template<class Integer>
    class ChooseStatelessGenerator
    {
        public:
            ChooseStatelessGenerator(Integer min, Integer max) :
                m_min(min), m_max(max)
            {
                assert(min <= max);
            }

            Integer unGen(RngEngine &rng, std::size_t) const
            {
                boost::uniform_int<Integer> dist(m_min, m_max);
                return dist(rng);
            }

            std::vector<Integer> shrink(Integer x) const
            {
                std::vector<Integer> ret;
                ret.reserve(x - m_min);
                if (abs(m_min) <= abs(m_max)) {
                    for (Integer i = m_min; i != x; ++i)
                        ret.push_back(i);
                } else {
                    for (Integer i = m_max; i != x; --i)
                        ret.push_back(i);
                }
                return ret;
            }

        private:
            const Integer m_min;
            const Integer m_max;
    };
}

/// Generates a random integer in the range min..max inclusive, requires that
/// min <= max. Shrinks towards smaller absolute values.
template<class Integer>
StatelessGenerator<Integer> choose(Integer min, Integer max)
{
    return detail::ChooseStatelessGenerator<Integer>(min, max);
}


namespace detail {
    template<class T>
    class SuchThatGenerator
    {
        public:
            SuchThatGenerator(const Generator<T> &g,
                    boost::function<bool (T)> pred) :
                m_gen(g), m_pred(pred)
            {
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                T ret = m_gen.unGen(rng, size);
                if (!m_pred(ret))
                    throw std::runtime_error("suchThat: generated value did not satisfy pred");
                return ret;
            }

            std::vector<T> shrink(const T &x)
            {
                std::vector<T> ret = m_gen.shrink(x);
                ret.erase(std::remove_if(ret.begin(), ret.end(),
                            std::not1(m_pred)), ret.end());
                return ret;
            }

        private:
            const Generator<T> m_gen;
            const boost::function<bool (T)> m_pred;
    };

    template<class T>
    class SuchThatStatelessGenerator
    {
        public:
            SuchThatStatelessGenerator(const StatelessGenerator<T> &g,
                    boost::function<bool (T)> pred) :
                m_gen(g), m_pred(pred)
            {
            }

            T unGen(RngEngine &rng, std::size_t size) const
            {
                T ret = m_gen.unGen(rng, size);
                if (!m_pred(ret))
                    throw std::runtime_error("suchThat: generated value did not satisfy pred");
                return ret;
            }

            std::vector<T> shrink(const T &x) const
            {
                std::vector<T> ret = m_gen.shrink(x);
                ret.erase(std::remove_if(ret.begin(), ret.end(),
                            std::not1(m_pred)), ret.end());
                return ret;
            }

        private:
            const StatelessGenerator<T> m_gen;
            const boost::function<bool (T)> m_pred;
    };
}

/// Generates a value that satisfies a predicate.
template<class T, class Pred>
Generator<T> suchThat(const Generator<T> &g, Pred pred)
{
    return detail::SuchThatGenerator<T>(g, pred);
}
template<class T, class Pred>
StatelessGenerator<T> suchThat(const StatelessGenerator<T> &g, Pred pred)
{
    return detail::SuchThatStatelessGenerator<T>(g, pred);
}


namespace detail {
    template<class T>
    class OneOfGenerator
    {
        public:
            OneOfGenerator &operator()(const Generator<T> &g)
            {
                m_gens.push_back(g);
                return *this;
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                boost::uniform_int<std::size_t> dist(0, m_gens.size() - 1);
                m_last_index = dist(rng);
                return m_gens[m_last_index].unGen(rng, size);
            }

            std::vector<T> shrink(const T &x)
            {
                return m_gens[m_last_index].shrink(x);
            }

        private:
            std::vector<Generator<T> > m_gens;
            std::size_t m_last_index;
    };
}

/// Randomly uses one of the given generators.
template<class T>
detail::OneOfGenerator<T> oneof(const Generator<T> &g)
{
    detail::OneOfGenerator<T> ret;
    return ret(g);
}


namespace detail {
    template<class T>
    class FrequencyGenerator
    {
        public:
            FrequencyGenerator() : m_tot(0)
            {
            }

            FrequencyGenerator &operator()(std::size_t weight,
                    const Generator<T> &g)
            {
                if (weight != 0) {
                    m_tot += weight;
                    m_gens.insert(std::make_pair(m_tot, g));
                }
                return *this;
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                boost::uniform_int<std::size_t> dist(1, m_tot);
                std::size_t weight = dist(rng);
                typename std::map<std::size_t, Generator<T> >::iterator it =
                    m_gens.lower_bound(weight);
                if (it == m_gens.end()) {
                    throw std::logic_error("frequency: all generators have weight 0");
                } else {
                    m_last_index = it->first;
                    return it->second.unGen(rng, size);
                }
            }

            std::vector<T> shrink(const T &x)
            {
                typename std::map<std::size_t, Generator<T> >::iterator it =
                    m_gens.find(m_last_index);
                assert(it != m_gens.end());
                return it->second.shrink(x);
            }

        private:
            std::map<std::size_t, Generator<T> > m_gens;
            std::size_t m_tot;
            std::size_t m_last_index;
    };
}

/// Chooses one of the given generators, with a weighted random distribution.
/// Any generator with weight "0" will not be chosen.
template<class T>
detail::FrequencyGenerator<T> frequency(std::size_t f, const Generator<T> &g)
{
    detail::FrequencyGenerator<T> ret;
    return ret(f, g);
}


namespace detail {
    template<class T>
    class ElementsGenerator
    {
        public:
            ElementsGenerator &operator()(const T &x)
            {
                m_elems.push_back(x);
                return *this;
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                boost::uniform_int<std::size_t> dist(0, m_elems.size() - 1);
                m_last_index = dist(rng);
                return m_elems[m_last_index];
            }

            std::vector<T> shrink(const T &x)
            {
                std::vector<T> ret;
                ret.reserve(m_last_index);
                for (std::size_t i = 0; i != m_last_index; ++i)
                    ret.push_back(m_elems[i]);
                return ret;
            }

        private:
            std::vector<T> m_elems;
            std::size_t m_last_index;
    };
}

/// Generates one of the given values.
template<class T>
detail::ElementsGenerator<T> elements(const T &x)
{
    detail::ElementsGenerator<T> ret;
    return ret(x);
}


namespace detail {
    template<class T>
    class FixedGenerator
    {
        public:
            FixedGenerator() : m_last_index_plus_one(0)
            {
            }

            FixedGenerator &operator()(const T &x)
            {
                m_fixed.push_back(x);
                return *this;
            }

            T unGen(RngEngine &, std::size_t)
            {
                ++m_last_index_plus_one;
                if (m_last_index_plus_one >= m_fixed.size()) {
                    throw std::runtime_error("fixed: exhausted fixed test cases");
                } else {
                    return m_fixed[m_last_index_plus_one];
                }
            }

            std::vector<T> shrink(const T &x)
            {
                return std::vector<T>();
            }

        private:
            std::size_t m_last_index_plus_one;
            std::vector<T> m_fixed;
    };
}

/// Generates each fixed test case until all have been generated.  Does not
/// shrink. Especially useful with "chain".
template<class T>
detail::FixedGenerator<T> fixed(const T &x)
{
    detail::FixedGenerator<T> ret;
    return ret(x);
}


namespace detail {
    template<class T>
    class ChainGenerator
    {
        public:
            ChainGenerator() : m_last_index(0)
            {
            }

            ChainGenerator &operator()(const Generator<T> &x)
            {
                m_gens.push_back(x);
                return *this;
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                for (m_last_index = 0; m_last_index < m_gens.size();
                        ++m_last_index) {
                    try {
                        return m_gens[m_last_index].unGen(rng, size);
                    } catch (...) {
                    }
                }

                throw std::runtime_error("chain: exhausted all possible generators");
            }

            std::vector<T> shrink(const T &x)
            {
                return m_gens[m_last_index].shrink(x);
            }

        private:
            std::size_t m_last_index;
            std::vector<Generator<T> > m_gens;
    };
}

/// Attempts to generate from each generator, if a generator fails to generate
/// input, the next one is tried.
template<class T>
detail::ChainGenerator<T> chain(const Generator<T> &g)
{
    detail::ChainGenerator<T> ret;
    return ret(g);
}


namespace detail {
    template<class T, class U>
    class ConvertGenerator
    {
        public:
            ConvertGenerator(boost::function<T (U)> f, const Generator<U> &g) :
                m_convert(f), m_gen(g)
            {
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                m_lastgen_u.clear();
                m_lastgen_t.clear();
                m_lastgen_u.push_back(m_gen.unGen(rng, size));
                m_lastgen_t.push_back(m_convert(m_lastgen_u.back()));
                return m_lastgen_t.back();
            }

            std::vector<T> shrink(const T &x)
            {
                typename std::vector<T>::iterator it =
                    std::find(m_lastgen_t.begin(), m_lastgen_t.end(), x);
                assert(it != m_lastgen_t.end());
                U last_u = m_lastgen_u[it - m_lastgen_t.begin()];
                m_lastgen_t.clear();
                m_lastgen_u = m_gen.shrink(last_u);
                m_lastgen_t.reserve(m_lastgen_u.size());
                std::transform(m_lastgen_u.begin(), m_lastgen_u.end(),
                        std::back_inserter(m_lastgen_t), m_convert);
                return m_lastgen_t;
            }

        private:
            boost::function<T (U)> m_convert;
            const Generator<U> m_gen;
            std::vector<T> m_lastgen_t;
            std::vector<U> m_lastgen_u;
    };
}

/// Converts a generator of U's into a generator of T's by passing the results
/// through a function which converts U's into T's.
template<class T, class U>
Generator<T> convert(boost::function<T (U)> f,
        const Generator<U> &g = Arbitrary<U>())
{
    return detail::ConvertGenerator<T, U>(f, g);
}

/// Convenience function to aid generic programming, equivalent to convert.
template<class T, class U1>
Generator<T> combine(boost::function<T (U1)> f,
        const Generator<U1> &g1 = Arbitrary<U1>())
{
    return detail::ConvertGenerator<T, U1>(f, g1);
}


namespace detail {
    template<class T, class U1, class U2>
    class CombineGenerator
    {
        public:
            CombineGenerator(boost::function<T (U1, U2)> f,
                    const Generator<U1> &g1,
                    const Generator<U2> &g2) :
                m_combine(f), m_gen1(g1), m_gen2(g2)
            {
            }

            T unGen(RngEngine &rng, std::size_t size)
            {
                m_lastgen_u1.clear();
                m_lastgen_u2.clear();
                m_lastgen_t.clear();
                m_lastgen_u1.push_back(m_gen1.unGen(rng, size));
                m_lastgen_u2.push_back(m_gen2.unGen(rng, size));
                m_lastgen_t.push_back(m_combine(m_lastgen_u1.back(),
                            m_lastgen_u2.back()));
                return m_lastgen_t.back();
            }

            std::vector<T> shrink(const T &x)
            {
                typename std::vector<T>::const_iterator it =
                    std::find(m_lastgen_t.begin(), m_lastgen_t.end(), x);
                assert(it != m_lastgen_t.end());
                std::size_t dist = it - m_lastgen_t.begin();
                U1 last_u1 = m_lastgen_u1[dist];
                U2 last_u2 = m_lastgen_u2[dist];
                m_lastgen_t.clear();
                std::vector<U1> shrink1 = m_gen1.shrink(last_u1);
                std::vector<U2> shrink2 = m_gen2.shrink(last_u2);
                m_lastgen_u1.assign(shrink1.begin(), shrink1.end());
                m_lastgen_u1.insert(m_lastgen_u1.end(),
                        shrink2.size(), last_u1);
                m_lastgen_u2.assign(shrink1.size(), last_u2);
                m_lastgen_u2.insert(m_lastgen_u2.end(),
                        shrink2.begin(), shrink2.end());

                m_lastgen_t.reserve(m_lastgen_u1.size());
                std::transform(m_lastgen_u1.begin(), m_lastgen_u1.end(),
                        m_lastgen_u2.begin(), std::back_inserter(m_lastgen_t),
                        m_combine);
                return m_lastgen_t;
            }

        private:
            boost::function<T (U1, U2)> m_combine;
            const Generator<U1> m_gen1;
            const Generator<U2> m_gen2;
            std::vector<T> m_lastgen_t;
            std::vector<U1> m_lastgen_u1;
            std::vector<U2> m_lastgen_u2;
    };
}

/// Combines generators of U1's and U2's into a generator of T's by passing the
/// results through a function which converts U1's and U2's into T's. Ie. the
/// same as convert for functions taking more than one argument.
template<class T, class U1, class U2>
Generator<T> combine(boost::function<T (U1, U2)> f,
        const Generator<U1> &g1 = Arbitrary<U1>(),
        const Generator<U2> &g2 = Arbitrary<U2>())
{
    return detail::CombineGenerator<T, U1, U2>(f, g1, g2);
}


namespace detail {
    template<class T>
    class ListOfStatelessGenerator
    {
        public:
            ListOfStatelessGenerator(const StatelessGenerator<T> &g) : m_gen(g)
            {
            }

            std::vector<T> unGen(RngEngine &rng, std::size_t size) const
            {
                boost::uniform_int<std::size_t> dist(0, size);
                std::size_t n = dist(rng);
                std::vector<T> ret;
                ret.reserve(n);
                while (n-- > 0)
                    ret.push_back(m_gen.unGen(rng, size));
                return ret;
            }

            std::vector<std::vector<T> > shrink(const std::vector<T> &x) const
            {
                std::vector<std::vector<T> > ret;
                ret.reserve(x.size());
                for (typename std::vector<T>::const_iterator it = x.begin();
                        it != x.end(); ++it) {
                    ret.push_back(std::vector<T>());
                    ret.back().reserve(x.size() - 1);
                    ret.back().insert(ret.back().end(), x.begin(), it);
                    ret.back().insert(ret.back().end(), it + 1, x.end());
                }
                return ret;
            }

        private:
            const StatelessGenerator<T> m_gen;
    };
}

/// Generates an std::vector of random length. The maximum length depends on the
/// generation size parameter. Shrinks by removing elements from the vector.
template<class T>
StatelessGenerator<std::vector<T> > listOf(
        const StatelessGenerator<T> &g = Arbitrary<T>())
{
    return detail::ListOfStatelessGenerator<T>(g);
}


namespace detail {
    template<class T>
    class ListOfNonEmptyStatelessGenerator
    {
        public:
            ListOfNonEmptyStatelessGenerator(const StatelessGenerator<T> &g) :
                m_gen(g)
            {
            }

            std::vector<T> unGen(RngEngine &rng, std::size_t size) const
            {
                boost::uniform_int<std::size_t> dist(1,
                        std::max<std::size_t>(1, size));
                std::size_t n = dist(rng);
                std::vector<T> ret;
                ret.reserve(n);
                while (n-- > 0)
                    ret.push_back(m_gen.unGen(rng, size));
                return ret;
            }

            std::vector<std::vector<T> > shrink(const std::vector<T> &x) const
            {
                std::vector<std::vector<T> > ret;
                if (x.size() == 1)
                    return ret;
                ret.reserve(x.size());
                for (typename std::vector<T>::const_iterator it = x.begin();
                        it != x.end(); ++it) {
                    ret.push_back(std::vector<T>());
                    ret.back().reserve(x.size() - 1);
                    ret.back().insert(ret.back().end(), x.begin(), it);
                    ret.back().insert(ret.back().end(), it + 1, x.end());
                }
                return ret;
            }

        private:
            const StatelessGenerator<T> m_gen;
    };
}

/// Generates a non-empty std::vector of random length. The maximum length
/// depends on the generation size parameter. Shrinks by removing elements from
/// the vector.
template<class T>
StatelessGenerator<std::vector<T> > listOfNonEmpty(
        const StatelessGenerator<T> &g = Arbitrary<T>())
{
    return detail::ListOfNonEmptyStatelessGenerator<T>(g);
}


namespace detail {
    template<class T>
    class VectorOfStatelessGenerator
    {
        public:
            VectorOfStatelessGenerator(std::size_t size,
                    const StatelessGenerator<T> &g) :
                m_size(size), m_gen(g)
            {
            }

            std::vector<T> unGen(RngEngine &rng, std::size_t size) const
            {
                std::vector<T> ret;
                std::size_t n = m_size;
                ret.reserve(n);
                while (n-- > 0)
                    ret.push_back(m_gen.unGen(rng, size));
                return ret;
            }

            std::vector<std::vector<T> > shrink(const std::vector<T> &x) const
            {
                return std::vector<std::vector<T> >();
            }

        private:
            std::size_t m_size;
            const StatelessGenerator<T> m_gen;
    };
}

/// Generates an std::vector of the given length. Does not shrink. Prefer using
/// listOf or listOfNonEmpty to vectorOf because those can shrink.
template<class T>
StatelessGenerator<std::vector<T> > vectorOf(std::size_t size,
        const StatelessGenerator<T> &g = Arbitrary<T>())
{
    return detail::VectorOfStatelessGenerator<T>(size, g);
}


namespace detail {
    struct null_type {};

    template<class T0, class T1 = null_type, class T2 = null_type,
        class T3 = null_type, class T4 = null_type, class T5 = null_type,
        class T6 = null_type, class T7 = null_type, class T8 = null_type,
        class T9 = null_type>
    class TupleGenerator
    {
            typedef boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>
                tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3,
                    const Generator<T4> &g4, const Generator<T5> &g5,
                    const Generator<T6> &g6, const Generator<T7> &g7,
                    const Generator<T8> &g8, const Generator<T9> &g9) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3), m_gen4(g4),
                m_gen5(g5), m_gen6(g6), m_gen7(g7), m_gen8(g8), m_gen9(g9)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size), m_gen4.unGen(rng, size),
                        m_gen5.unGen(rng, size), m_gen6.unGen(rng, size),
                        m_gen7.unGen(rng, size), m_gen8.unGen(rng, size),
                        m_gen9.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<T4> shrinks4 = m_gen4.shrink(boost::get<4>(x));
                std::vector<T5> shrinks5 = m_gen5.shrink(boost::get<5>(x));
                std::vector<T6> shrinks6 = m_gen6.shrink(boost::get<6>(x));
                std::vector<T7> shrinks7 = m_gen7.shrink(boost::get<7>(x));
                std::vector<T8> shrinks8 = m_gen8.shrink(boost::get<8>(x));
                std::vector<T9> shrinks9 = m_gen9.shrink(boost::get<9>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size() + shrinks4.size() +
                        shrinks5.size() + shrinks6.size() + shrinks7.size() +
                        shrinks8.size() + shrinks9.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it,
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T4>::const_iterator it =
                        shrinks4.begin(); it != shrinks4.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), *it, boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T5>::const_iterator it =
                        shrinks5.begin(); it != shrinks5.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x), *it,
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T6>::const_iterator it =
                        shrinks6.begin(); it != shrinks6.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), *it, boost::get<7>(x),
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T7>::const_iterator it =
                        shrinks7.begin(); it != shrinks7.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), boost::get<6>(x), *it,
                                boost::get<8>(x), boost::get<9>(x)));
                for (typename std::vector<T8>::const_iterator it =
                        shrinks8.begin(); it != shrinks8.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), boost::get<6>(x),
                                boost::get<7>(x), *it, boost::get<9>(x)));
                for (typename std::vector<T9>::const_iterator it =
                        shrinks9.begin(); it != shrinks9.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), boost::get<6>(x),
                                boost::get<7>(x), boost::get<8>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
            const Generator<T4> m_gen4;
            const Generator<T5> m_gen5;
            const Generator<T6> m_gen6;
            const Generator<T7> m_gen7;
            const Generator<T8> m_gen8;
            const Generator<T9> m_gen9;
    };

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8>
    class TupleGenerator<T0, T1, T2, T3, T4, T5, T6, T7, T8, null_type>
    {
            typedef boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3,
                    const Generator<T4> &g4, const Generator<T5> &g5,
                    const Generator<T6> &g6, const Generator<T7> &g7,
                    const Generator<T8> &g8) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3), m_gen4(g4),
                m_gen5(g5), m_gen6(g6), m_gen7(g7), m_gen8(g8)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size), m_gen4.unGen(rng, size),
                        m_gen5.unGen(rng, size), m_gen6.unGen(rng, size),
                        m_gen7.unGen(rng, size), m_gen8.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<T4> shrinks4 = m_gen4.shrink(boost::get<4>(x));
                std::vector<T5> shrinks5 = m_gen5.shrink(boost::get<5>(x));
                std::vector<T6> shrinks6 = m_gen6.shrink(boost::get<6>(x));
                std::vector<T7> shrinks7 = m_gen7.shrink(boost::get<7>(x));
                std::vector<T8> shrinks8 = m_gen8.shrink(boost::get<8>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size() + shrinks4.size() +
                        shrinks5.size() + shrinks6.size() + shrinks7.size() +
                        shrinks8.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it,
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T4>::const_iterator it =
                        shrinks4.begin(); it != shrinks4.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), *it, boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T5>::const_iterator it =
                        shrinks5.begin(); it != shrinks5.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x), *it,
                                boost::get<6>(x), boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T6>::const_iterator it =
                        shrinks6.begin(); it != shrinks6.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), *it, boost::get<7>(x),
                                boost::get<8>(x)));
                for (typename std::vector<T7>::const_iterator it =
                        shrinks7.begin(); it != shrinks7.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), boost::get<6>(x), *it,
                                boost::get<8>(x)));
                for (typename std::vector<T8>::const_iterator it =
                        shrinks8.begin(); it != shrinks8.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), boost::get<6>(x),
                                boost::get<7>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
            const Generator<T4> m_gen4;
            const Generator<T5> m_gen5;
            const Generator<T6> m_gen6;
            const Generator<T7> m_gen7;
            const Generator<T8> m_gen8;
    };

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7>
    class TupleGenerator<T0, T1, T2, T3, T4, T5, T6, T7, null_type, null_type>
    {
            typedef boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3,
                    const Generator<T4> &g4, const Generator<T5> &g5,
                    const Generator<T6> &g6, const Generator<T7> &g7) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3), m_gen4(g4),
                m_gen5(g5), m_gen6(g6), m_gen7(g7)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size), m_gen4.unGen(rng, size),
                        m_gen5.unGen(rng, size), m_gen6.unGen(rng, size),
                        m_gen7.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<T4> shrinks4 = m_gen4.shrink(boost::get<4>(x));
                std::vector<T5> shrinks5 = m_gen5.shrink(boost::get<5>(x));
                std::vector<T6> shrinks6 = m_gen6.shrink(boost::get<6>(x));
                std::vector<T7> shrinks7 = m_gen7.shrink(boost::get<7>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size() + shrinks4.size() +
                        shrinks5.size() + shrinks6.size() + shrinks7.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it,
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x)));
                for (typename std::vector<T4>::const_iterator it =
                        shrinks4.begin(); it != shrinks4.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), *it, boost::get<5>(x),
                                boost::get<6>(x), boost::get<7>(x)));
                for (typename std::vector<T5>::const_iterator it =
                        shrinks5.begin(); it != shrinks5.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x), *it,
                                boost::get<6>(x), boost::get<7>(x)));
                for (typename std::vector<T6>::const_iterator it =
                        shrinks6.begin(); it != shrinks6.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), *it, boost::get<7>(x)));
                for (typename std::vector<T7>::const_iterator it =
                        shrinks7.begin(); it != shrinks7.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), boost::get<6>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
            const Generator<T4> m_gen4;
            const Generator<T5> m_gen5;
            const Generator<T6> m_gen6;
            const Generator<T7> m_gen7;
    };

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6>
    class TupleGenerator<T0, T1, T2, T3, T4, T5, T6,
        null_type, null_type, null_type>
    {
            typedef boost::tuple<T0, T1, T2, T3, T4, T5, T6> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3,
                    const Generator<T4> &g4, const Generator<T5> &g5,
                    const Generator<T6> &g6) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3), m_gen4(g4),
                m_gen5(g5), m_gen6(g6)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size), m_gen4.unGen(rng, size),
                        m_gen5.unGen(rng, size), m_gen6.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<T4> shrinks4 = m_gen4.shrink(boost::get<4>(x));
                std::vector<T5> shrinks5 = m_gen5.shrink(boost::get<5>(x));
                std::vector<T6> shrinks6 = m_gen6.shrink(boost::get<6>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size() + shrinks4.size() +
                        shrinks5.size() + shrinks6.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it,
                                boost::get<4>(x), boost::get<5>(x),
                                boost::get<6>(x)));
                for (typename std::vector<T4>::const_iterator it =
                        shrinks4.begin(); it != shrinks4.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), *it, boost::get<5>(x),
                                boost::get<6>(x)));
                for (typename std::vector<T5>::const_iterator it =
                        shrinks5.begin(); it != shrinks5.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x), *it,
                                boost::get<6>(x)));
                for (typename std::vector<T6>::const_iterator it =
                        shrinks6.begin(); it != shrinks6.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x),
                                boost::get<5>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
            const Generator<T4> m_gen4;
            const Generator<T5> m_gen5;
            const Generator<T6> m_gen6;
    };

    template<class T0, class T1, class T2, class T3, class T4,
        class T5>
    class TupleGenerator<T0, T1, T2, T3, T4, T5,
        null_type, null_type, null_type, null_type>
    {
            typedef boost::tuple<T0, T1, T2, T3, T4, T5> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3,
                    const Generator<T4> &g4, const Generator<T5> &g5) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3), m_gen4(g4),
                m_gen5(g5)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size), m_gen4.unGen(rng, size),
                        m_gen5.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<T4> shrinks4 = m_gen4.shrink(boost::get<4>(x));
                std::vector<T5> shrinks5 = m_gen5.shrink(boost::get<5>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size() + shrinks4.size() +
                        shrinks5.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x),
                                boost::get<4>(x), boost::get<5>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it,
                                boost::get<4>(x), boost::get<5>(x)));
                for (typename std::vector<T4>::const_iterator it =
                        shrinks4.begin(); it != shrinks4.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), *it, boost::get<5>(x)));
                for (typename std::vector<T5>::const_iterator it =
                        shrinks5.begin(); it != shrinks5.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), boost::get<4>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
            const Generator<T4> m_gen4;
            const Generator<T5> m_gen5;
    };

    template<class T0, class T1, class T2, class T3, class T4>
    class TupleGenerator<T0, T1, T2, T3, T4,
          null_type, null_type, null_type, null_type, null_type>
    {
            typedef boost::tuple<T0, T1, T2, T3, T4> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3,
                    const Generator<T4> &g4) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3), m_gen4(g4)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size), m_gen4.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<T4> shrinks4 = m_gen4.shrink(boost::get<4>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size() + shrinks4.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x),
                                boost::get<4>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x),
                                boost::get<4>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it,
                                boost::get<4>(x)));
                for (typename std::vector<T4>::const_iterator it =
                        shrinks4.begin(); it != shrinks4.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x),
                                boost::get<3>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
            const Generator<T4> m_gen4;
    };

    template<class T0, class T1, class T2, class T3>
    class TupleGenerator<T0, T1, T2, T3, null_type,
          null_type, null_type, null_type, null_type, null_type>
    {
            typedef boost::tuple<T0, T1, T2, T3> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2, const Generator<T3> &g3) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2), m_gen3(g3)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size),
                        m_gen3.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<T3> shrinks3 = m_gen3.shrink(boost::get<3>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size() + shrinks3.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x), boost::get<3>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x), boost::get<3>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it, boost::get<3>(x)));
                for (typename std::vector<T3>::const_iterator it =
                        shrinks3.begin(); it != shrinks3.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), boost::get<2>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
            const Generator<T3> m_gen3;
    };

    template<class T0, class T1, class T2>
    class TupleGenerator<T0, T1, T2, null_type, null_type,
          null_type, null_type, null_type, null_type, null_type>
    {
            typedef boost::tuple<T0, T1, T2> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1,
                    const Generator<T2> &g2) :
                m_gen0(g0), m_gen1(g1), m_gen2(g2)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size), m_gen2.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<T2> shrinks2 = m_gen2.shrink(boost::get<2>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size() +
                        shrinks2.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x),
                                boost::get<2>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it,
                                boost::get<2>(x)));
                for (typename std::vector<T2>::const_iterator it =
                        shrinks2.begin(); it != shrinks2.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x),
                                boost::get<1>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
            const Generator<T2> m_gen2;
    };

    template<class T0, class T1>
    class TupleGenerator<T0, T1, null_type, null_type, null_type,
          null_type, null_type, null_type, null_type, null_type>
    {
            typedef boost::tuple<T0, T1> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0, const Generator<T1> &g1) :
                m_gen0(g0), m_gen1(g1)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size),
                        m_gen1.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<T1> shrinks1 = m_gen1.shrink(boost::get<1>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size() + shrinks1.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it, boost::get<1>(x)));
                for (typename std::vector<T1>::const_iterator it =
                        shrinks1.begin(); it != shrinks1.end(); ++it)
                    ret.push_back(tuple_type(boost::get<0>(x), *it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
            const Generator<T1> m_gen1;
    };

    template<class T0>
    class TupleGenerator<T0, null_type, null_type, null_type, null_type,
          null_type, null_type, null_type, null_type, null_type>
    {
            typedef boost::tuple<T0> tuple_type;
        public:
            TupleGenerator(const Generator<T0> &g0) :
                m_gen0(g0)
            {
            }

            tuple_type unGen(RngEngine &rng, std::size_t size)
            {
                return tuple_type(m_gen0.unGen(rng, size));
            }

            std::vector<tuple_type> shrink(const tuple_type &x)
            {
                std::vector<T0> shrinks0 = m_gen0.shrink(boost::get<0>(x));
                std::vector<tuple_type> ret;
                ret.reserve(shrinks0.size());
                for (typename std::vector<T0>::const_iterator it =
                        shrinks0.begin(); it != shrinks0.end(); ++it)
                    ret.push_back(tuple_type(*it));
                return ret;
            }

        private:
            const Generator<T0> m_gen0;
    };
}

template<class T0>
Generator<boost::tuple<T0> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>())
{
    return detail::TupleGenerator<T0>(g0);
}

template<class T0, class T1>
Generator<boost::tuple<T0, T1> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>())
{
    return detail::TupleGenerator<T0, T1>(g0, g1);
}

template<class T0, class T1, class T2>
Generator<boost::tuple<T0, T1, T2> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>())
{
    return detail::TupleGenerator<T0, T1, T2>(g0, g1, g2);
}

template<class T0, class T1, class T2, class T3>
Generator<boost::tuple<T0, T1, T2, T3> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>())
{
    return detail::TupleGenerator<T0, T1, T2, T3>(g0, g1, g2, g3);
}

template<class T0, class T1, class T2, class T3, class T4>
Generator<boost::tuple<T0, T1, T2, T3, T4> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>(),
        const Generator<T4> &g4 = Arbitrary<T4>())
{
    return detail::TupleGenerator<T0, T1, T2, T3, T4>(g0, g1, g2, g3, g4);
}

template<class T0, class T1, class T2, class T3, class T4, class T5>
Generator<boost::tuple<T0, T1, T2, T3, T4, T5> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>(),
        const Generator<T4> &g4 = Arbitrary<T4>(),
        const Generator<T5> &g5 = Arbitrary<T5>())
{
    return detail::TupleGenerator<T0, T1, T2, T3, T4, T5>(
            g0, g1, g2, g3, g4, g5);
}

template<class T0, class T1, class T2, class T3, class T4, class T5, class T6>
Generator<boost::tuple<T0, T1, T2, T3, T4, T5, T6> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>(),
        const Generator<T4> &g4 = Arbitrary<T4>(),
        const Generator<T5> &g5 = Arbitrary<T5>(),
        const Generator<T6> &g6 = Arbitrary<T6>())
{
    return detail::TupleGenerator<T0, T1, T2, T3, T4, T5, T6>(
            g0, g1, g2, g3, g4, g5, g6);
}

template<class T0, class T1, class T2, class T3, class T4,
    class T5, class T6, class T7>
Generator<boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>(),
        const Generator<T4> &g4 = Arbitrary<T4>(),
        const Generator<T5> &g5 = Arbitrary<T5>(),
        const Generator<T6> &g6 = Arbitrary<T6>(),
        const Generator<T7> &g7 = Arbitrary<T7>())
{
    return detail::TupleGenerator<T0, T1, T2, T3, T4, T5, T6, T7>(
            g0, g1, g2, g3, g4, g5, g6, g7);
}

template<class T0, class T1, class T2, class T3, class T4,
    class T5, class T6, class T7, class T8>
Generator<boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>(),
        const Generator<T4> &g4 = Arbitrary<T4>(),
        const Generator<T5> &g5 = Arbitrary<T5>(),
        const Generator<T6> &g6 = Arbitrary<T6>(),
        const Generator<T7> &g7 = Arbitrary<T7>(),
        const Generator<T8> &g8 = Arbitrary<T8>())
{
    return detail::TupleGenerator<T0, T1, T2, T3, T4, T5, T6, T7, T8>(
            g0, g1, g2, g3, g4, g5, g6, g7, g8);
}

template<class T0, class T1, class T2, class T3, class T4,
    class T5, class T6, class T7, class T8, class T9>
Generator<boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> >
tupleOf(const Generator<T0> &g0 = Arbitrary<T0>(),
        const Generator<T1> &g1 = Arbitrary<T1>(),
        const Generator<T2> &g2 = Arbitrary<T2>(),
        const Generator<T3> &g3 = Arbitrary<T3>(),
        const Generator<T4> &g4 = Arbitrary<T4>(),
        const Generator<T5> &g5 = Arbitrary<T5>(),
        const Generator<T6> &g6 = Arbitrary<T6>(),
        const Generator<T7> &g7 = Arbitrary<T7>(),
        const Generator<T8> &g8 = Arbitrary<T8>(),
        const Generator<T9> &g9 = Arbitrary<T9>())
{
    return detail::TupleGenerator<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>(
            g0, g1, g2, g3, g4, g5, g6, g7, g8, g9);
}



}

#endif
