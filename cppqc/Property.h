#ifndef CPPQC_PROPERTY_H
#define CPPQC_PROPERTY_H

#include "Generator.h"
#include "Arbitrary.h"

#include <stdexcept>
#include <typeinfo>

namespace cppqc {

/*
 * User defined properties must model a "PropertyConcept<T1>", this is, they
 * must have member functions compatible with the following signatures:
 *
 *      bool unProp(const T1 &) const;
 *      Generator<T1> gen() const;
 *
 * The unProp function is the function that is run to actually test the
 * property with generated input. 
 *
 * PropertyConcept<T1>:
 *      bool unProp(const T1 &) const;
 *      Generator<T1> gen() const;
 *
 * ArbitraryPropertyConcept<T1>:
 *      bool unProp(const T1 &) const;
 */


namespace detail {
    struct null_type
    {
    };

    template<class T1>
    std::string defaultClassifyFun(const T1 &)
    { return ""; }
    template<class T1, class T2>
    std::string defaultClassifyFun(const T1 &, const T2 &)
    { return ""; }
    template<class T1, class T2, class T3>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &)
    { return ""; }
    template<class T1, class T2, class T3, class T4>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &,
            const T4 &)
    { return ""; }
    template<class T1, class T2, class T3, class T4, class T5>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &)
    { return ""; }
    template<class T1, class T2, class T3, class T4, class T5, class T6>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &)
    { return ""; }
    template<class T1, class T2, class T3, class T4, class T5, class T6,
        class T7>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &, const T7 &)
    { return ""; }
    template<class T1, class T2, class T3, class T4, class T5, class T6,
        class T7, class T8>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &, const T7 &, const T8 &)
    { return ""; }
    template<class T1, class T2, class T3, class T4, class T5, class T6,
        class T7, class T8, class T9>
    std::string defaultClassifyFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &, const T7 &, const T8 &,
            const T9 &)
    { return ""; }

    template<class T1>
    bool defaultTrivialFun(const T1 &)
    { return false; }
    template<class T1, class T2>
    bool defaultTrivialFun(const T1 &, const T2 &)
    { return false; }
    template<class T1, class T2, class T3>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &)
    { return false; }
    template<class T1, class T2, class T3, class T4>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &,
            const T4 &)
    { return false; }
    template<class T1, class T2, class T3, class T4, class T5>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &)
    { return false; }
    template<class T1, class T2, class T3, class T4, class T5, class T6>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &)
    { return false; }
    template<class T1, class T2, class T3, class T4, class T5, class T6,
        class T7>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &, const T7 &)
    { return false; }
    template<class T1, class T2, class T3, class T4, class T5, class T6,
        class T7, class T8>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &, const T7 &, const T8 &)
    { return false; }
    template<class T1, class T2, class T3, class T4, class T5, class T6,
        class T7, class T8, class T9>
    bool defaultTrivialFun(const T1 &, const T2 &, const T3 &,
            const T4 &, const T5 &, const T6 &, const T7 &, const T8 &,
            const T9 &)
    { return false; }

    class PropertyBase
    {
        public:
            PropertyBase() :
                m_name(""), m_expect(true)
            {
            }

            std::string name__() const
            { return m_name; }
            bool expect__() const
            { return m_expect; }

        private:
            std::string m_name;
            bool m_expect;

            template<class Prop> friend
            Prop named(const std::string &, const Prop &);
            template<class Prop> friend
            Prop expectFailure(const Prop &);
    };

    /// Gives a name to the given property.
    template<class Prop>
    Prop named(const std::string &name, const Prop &prop)
    {
        Prop ret(prop);
        ret.detail::PropertyBase::m_name = name;
        return ret;
    }

    /// Makes the test framework assume that it is ok if this property fails.
    template<class Prop>
    Prop expectFailure(const Prop &prop)
    {
        Prop ret(prop);
        ret.detail::PropertyBase::m_expect = false;
        return ret;
    }
}

using detail::named;
using detail::expectFailure;

namespace detail {
    template<class T1>
    struct Input
    {
        T1 x1;

        Input(const T1 &x1) :
            x1(x1)
        {
        }
    };
}

template<class T1>
class Property : public detail::PropertyBase
{
    public:
        typedef boost::function<bool (T1)> CheckFun;
        typedef boost::function<std::string (T1)> ClassifyFun;
        typedef boost::function<bool (T1)> TrivialFun;
        typedef detail::Input<T1> Input;

        Property(CheckFun f, const Generator<T1> &g1 = Arbitrary<T1>()) :
            m_check(f), m_gen1(g1),
            m_classifyfun(detail::defaultClassifyFun<T1>),
            m_trivialfun(detail::defaultTrivialFun<T1>)
        {
        }

        Input generate__(RngEngine &rng, std::size_t size) const
        {
            return Input(m_gen1.unGen(rng, size));
        }
        std::vector<Input> shrink__(const Input &in) const
        {
            std::vector<T1> shrinks1 = m_gen1.shrink(in.x1);
            std::vector<Input> ret;
            ret.reserve(shrinks1.size());
            for (typename std::vector<T1>::const_iterator it = shrinks1.begin();
                    it != shrinks1.end(); ++it) {
                ret.push_back(Input(in.x1));
            }
            return ret;
        }
        bool check__(const Input &in) const
        { return m_check(in.x1); }
        bool trivial__(const Input &in) const
        { return m_trivialfun(in.x1); }
        std::string classify__(const Input &in) const
        { return m_classifyfun(in.x1); }

    private:
        CheckFun m_check;
        Generator<T1> m_gen1;

        ClassifyFun m_classifyfun;
        TrivialFun m_trivialfun;

        template<class U1> friend
        Property<U1> classifyWith(typename Property<U1>::ClassifyFun,
                const Property<U1> &);
        template<class U1> friend
        Property<U1> trivializeWith(typename Property<U1>::TrivialFun,
                const Property<U1> &);
};

template<class T1>
std::ostream &operator<<(std::ostream &out, const detail::Input<T1> &in)
{
    return out << "  1: " << in.x1 << '\n'
        << std::flush;
}

template<class T1>
Property<T1> classifyWith(typename Property<T1>::ClassifyFun f,
        const Property<T1> &prop)
{
    Property<T1> ret(prop);
    ret.m_classifyfun = f;
    return ret;
}

template<class T1>
Property<T1> trivializeWith(typename Property<T1>::TrivialFun f,
        const Property<T1> &prop)
{
    Property<T1> ret(prop);
    ret.m_trivialfun = f;
    return ret;
}

















}

#endif
