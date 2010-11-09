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

#ifndef CPPQC_PROPERTY_H
#define CPPQC_PROPERTY_H

#include "Generator.h"
#include "Arbitrary.h"

#include <stdexcept>
#include <typeinfo>

#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/utility/enable_if.hpp>

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

    typedef char SmallType;
    struct LargeType
    {
        SmallType dummy__[2];
    };

    template<class P>
    struct ClassHasNameMemberFunction
    {
        template<std::string (P::*)() const> struct tester1;
        template<std::string (*)()> struct tester2;

        template<class U>
        static SmallType hasNameMember(tester1<&U::name> *);
        template<class U>
        static SmallType hasNameMember(tester2<&U::name> *);
        template<class U>
        static LargeType hasNameMember(...);

        static const bool value =
            (sizeof(hasNameMember<P>(0)) == sizeof(SmallType));
    };

    template<class P, bool IsClassType = boost::is_class<P>::value>
    struct HasNameMemberFunction : boost::false_type
    {
    };

    template<class P>
    struct HasNameMemberFunction<P, true> :
        boost::integral_constant<bool, ClassHasNameMemberFunction<P>::value>
    {
    };

    template<class P>
    struct ClassHasNameMemberVariable
    {
        template<const std::string P::*> struct tester1;
        template<const std::string *> struct tester2;

        template<class U>
        static SmallType hasNameMember(tester1<&U::name> *);
        template<class U>
        static SmallType hasNameMember(tester2<&U::name> *);
        template<class U>
        static LargeType hasNameMember(...);

        static const bool value =
            (sizeof(hasNameMember<P>(0)) == sizeof(SmallType));
    };

    template<class P, bool IsClassType = boost::is_class<P>::value>
    struct HasNameMemberVariable : boost::false_type
    {
    };

    template<class P>
    struct HasNameMemberVariable<P, true> :
        boost::integral_constant<bool, ClassHasNameMemberVariable<P>::value>
    {
    };

    template<class P>
    struct ClassHasExpectMemberFunction
    {
        template<bool (P::*)() const> struct tester1;
        template<bool (*)()> struct tester2;

        template<class U>
        static SmallType hasExpectMember(tester1<&U::expect> *);
        template<class U>
        static SmallType hasExpectMember(tester2<&U::expect> *);
        template<class U>
        static LargeType hasExpectMember(...);

        static const bool value =
            (sizeof(hasExpectMember<P>(0)) == sizeof(SmallType));
    };

    template<class P, bool IsClassType = boost::is_class<P>::value>
    struct HasExpectMemberFunction : boost::false_type
    {
    };

    template<class P>
    struct HasExpectMemberFunction<P, true> :
        boost::integral_constant<bool, ClassHasExpectMemberFunction<P>::value>
    {
    };

    template<class P>
    struct ClassHasExpectMemberVariable
    {
        template<const bool P::*> struct tester1;
        template<const bool *> struct tester2;

        template<class U>
        static SmallType hasExpectMember(tester1<&U::expect> *);
        template<class U>
        static SmallType hasExpectMember(tester2<&U::expect> *);
        template<class U>
        static LargeType hasExpectMember(...);

        static const bool value =
            (sizeof(hasExpectMember<P>(0)) == sizeof(SmallType));
    };

    template<class P, bool IsClassType = boost::is_class<P>::value>
    struct HasExpectMemberVariable : boost::false_type
    {
    };

    template<class P>
    struct HasExpectMemberVariable<P, true> :
        boost::integral_constant<bool, ClassHasExpectMemberVariable<P>::value>
    {
    };

    template<class P, class V>
    struct ClassHasClassifyMemberFunction
    {
        template<std::string (P::*)(const V &) const> struct tester1;
        template<std::string (P::*)(V) const> struct tester2;
        template<std::string (*)(const V &)> struct tester3;
        template<std::string (*)(V)> struct tester4;

        template<class U>
        static SmallType hasClassifyMember(tester1<&U::classify> *);
        template<class U>
        static SmallType hasClassifyMember(tester2<&U::classify> *);
        template<class U>
        static SmallType hasClassifyMember(tester3<&U::classify> *);
        template<class U>
        static SmallType hasClassifyMember(tester4<&U::classify> *);
        template<class U>
        static LargeType hasClassifyMember(...);

        static const bool value =
            (sizeof(hasClassifyMember<P>(0)) == sizeof(SmallType));
    };

    template<class P, class V, bool IsClassType = boost::is_class<P>::value>
    struct HasClassifyMemberFunction : boost::false_type
    {
    };

    template<class P, class V>
    struct HasClassifyMemberFunction<P, V, true> :
        boost::integral_constant<bool,
            ClassHasClassifyMemberFunction<P, V>::value>
    {
    };

    template<class P, class V>
    struct ClassHasTrivialMemberFunction
    {
        template<bool (P::*)(const V &) const> struct tester1;
        template<bool (P::*)(V) const> struct tester2;
        template<bool (*)(const V &)> struct tester3;
        template<bool (*)(V)> struct tester4;

        template<class U>
        static SmallType hasTrivialMember(tester1<&U::trivial> *);
        template<class U>
        static SmallType hasTrivialMember(tester2<&U::trivial> *);
        template<class U>
        static SmallType hasTrivialMember(tester3<&U::trivial> *);
        template<class U>
        static SmallType hasTrivialMember(tester4<&U::trivial> *);
        template<class U>
        static LargeType hasTrivialMember(...);

        static const bool value =
            (sizeof(hasTrivialMember<P>(0)) == sizeof(SmallType));
    };

    template<class P, class V, bool IsClassType = boost::is_class<P>::value>
    struct HasTrivialMemberFunction : boost::false_type
    {
    };

    template<class P, class V>
    struct HasTrivialMemberFunction<P, V, true> :
        boost::integral_constant<bool,
            ClassHasTrivialMemberFunction<P, V>::value>
    {
    };

#define CPPQC_STAMP_HasGenXMemberVariable(x) \
    template<class P, class V> \
    struct ClassHasGen ## x ## MemberVariable \
    { \
        template<const Generator<V> P::*> struct tester1; \
        template<const Generator<V> *> struct tester2; \
        template<class U> \
        static SmallType hasGenXMember(tester1<&U::gen ## x> *); \
        template<class U> \
        static SmallType hasGenXMember(tester2<&U::gen ## x> *); \
        template<class U> \
        static LargeType hasGenXMember(...); \
        static const bool value = \
            (sizeof(hasGenXMember<P>(0)) == sizeof(SmallType)); \
    }; \
    template<class P, class V, bool IsClassType = boost::is_class<P>::value> \
    struct HasGen ## x ## MemberVariable : boost::false_type \
    { \
    }; \
    template<class P, class V> \
    struct HasGen ## x ## MemberVariable<P, V, true> : \
        boost::integral_constant<bool, \
            ClassHasGen ## x ## MemberVariable<P, V>::value> \
    { \
    }; \
    template<class P, class V> \
    struct ClassHasStatelessGen ## x ## MemberVariable \
    { \
        template<const StatelessGenerator<V> P::*> struct tester1; \
        template<const StatelessGenerator<V> *> struct tester2; \
        template<class U> \
        static SmallType hasGenXMember(tester1<&U::gen ## x> *); \
        template<class U> \
        static SmallType hasGenXMember(tester2<&U::gen ## x> *); \
        template<class U> \
        static LargeType hasGenXMember(...); \
        static const bool value = \
            (sizeof(hasGenXMember<P>(0)) == sizeof(SmallType)); \
    }; \
    template<class P, class V, bool IsClassType = boost::is_class<P>::value> \
    struct HasStatelessGen ## x ## MemberVariable : boost::false_type \
    { \
    }; \
    template<class P, class V> \
    struct HasStatelessGen ## x ## MemberVariable<P, V, true> : \
        boost::integral_constant<bool, \
            ClassHasStatelessGen ## x ## MemberVariable<P, V>::value> \
    { \
    }

#define CPPQC_STAMP_HasGenXMemberFunction(x) \
    template<class P, class V> \
    struct ClassHasGen ## x ## MemberFunction \
    { \
        template<const Generator<V> (P::*)()> struct tester1; \
        template<const Generator<V> (*)()> struct tester2; \
        template<class U> \
        static SmallType hasGenXMember(tester1<&U::gen ## x> *); \
        template<class U> \
        static SmallType hasGenXMember(tester2<&U::gen ## x> *); \
        template<class U> \
        static LargeType hasGenXMember(...); \
        static const bool value = \
            (sizeof(hasGenXMember<P>(0)) == sizeof(SmallType)); \
    }; \
    template<class P, class V, bool IsClassType = boost::is_class<P>::value> \
    struct HasGen ## x ## MemberFunction : boost::false_type \
    { \
    }; \
    template<class P, class V> \
    struct HasGen ## x ## MemberFunction<P, V, true> : \
        boost::integral_constant<bool, \
            ClassHasGen ## x ## MemberFunction<P, V>::value> \
    { \
    }; \
    template<class P, class V> \
    struct ClassHasStatelessGen ## x ## MemberFunction \
    { \
        template<const StatelessGenerator<V> P::*> struct tester1; \
        template<const StatelessGenerator<V> *> struct tester2; \
        template<class U> \
        static SmallType hasGenXMember(tester1<&U::gen ## x> *); \
        template<class U> \
        static SmallType hasGenXMember(tester2<&U::gen ## x> *); \
        template<class U> \
        static LargeType hasGenXMember(...); \
        static const bool value = \
            (sizeof(hasGenXMember<P>(0)) == sizeof(SmallType)); \
    }; \
    template<class P, class V, bool IsClassType = boost::is_class<P>::value> \
    struct HasStatelessGen ## x ## MemberFunction : boost::false_type \
    { \
    }; \
    template<class P, class V> \
    struct HasStatelessGen ## x ## MemberFunction<P, V, true> : \
        boost::integral_constant<bool, \
            ClassHasStatelessGen ## x ## MemberFunction<P, V>::value> \
    { \
    }

#define CPPQC_STAMP_genXDispatch(x) \
    template<class V, class PM> \
    typename boost::enable_if<HasStatelessGen ## x ## MemberVariable<PM, V>, \
        const StatelessGenerator<V> &>::type \
    gen ## x ## Dispatch(const PM &pm) \
    { \
        return pm.gen ## x; \
    } \
    template<class V, class PM> \
    typename boost::enable_if<HasGen ## x ## MemberVariable<PM, V>, \
        const Generator<V> &>::type \
    gen ## x ## Dispatch(const PM &pm) \
    { \
        return pm.gen ## x; \
    } \
    template<class V, class PM> \
    typename boost::enable_if<HasStatelessGen ## x ## MemberFunction<PM, V>, \
        const StatelessGenerator<V> &>::type \
    gen ## x ## Dispatch(const PM &pm) \
    { \
        return pm.gen ## x(); \
    } \
    template<class V, class PM> \
    typename boost::enable_if<HasGen ## x ## MemberFunction<PM, V>, \
        const Generator<V> &>::type \
    gen ## x ## Dispatch(const PM &pm) \
    { \
        return pm.gen ## x(); \
    } \
    template<class V, class PM> \
    typename boost::disable_if_c< \
        HasGen ## x ## MemberVariable<PM, V>::value || \
            HasStatelessGen ## x ## MemberVariable<PM, V>::value || \
            HasGen ## x ## MemberFunction<PM, V>::value || \
            HasStatelessGen ## x ## MemberFunction<PM, V>::value, \
        StatelessGenerator<V> >::type \
    gen ## x ## Dispatch(const PM &pm) \
    { \
        return Arbitrary<V>(); \
    }


    CPPQC_STAMP_HasGenXMemberVariable(0);
    CPPQC_STAMP_HasGenXMemberFunction(0);
    CPPQC_STAMP_HasGenXMemberVariable(1);
    CPPQC_STAMP_HasGenXMemberFunction(1);
    CPPQC_STAMP_HasGenXMemberVariable(2);
    CPPQC_STAMP_HasGenXMemberFunction(2);

    CPPQC_STAMP_genXDispatch(0);
    CPPQC_STAMP_genXDispatch(1);
    CPPQC_STAMP_genXDispatch(2);

    template<class PropertyModel>
    typename boost::enable_if<HasNameMemberVariable<PropertyModel>,
         std::string>::type
    nameDispatch(const PropertyModel &pm)
    {
        return pm.name;
    }
    template<class PropertyModel>
    typename boost::enable_if<HasNameMemberFunction<PropertyModel>,
         std::string>::type
    nameDispatch(const PropertyModel &pm)
    {
        return pm.name();
    }
    template<class PropertyModel>
    typename boost::disable_if_c<HasNameMemberVariable<PropertyModel>::value ||
        HasNameMemberFunction<PropertyModel>::value, std::string>::type
    nameDispatch(const PropertyModel &pm)
    {
        return "";
    }

    template<class PropertyModel>
    typename boost::enable_if<HasExpectMemberVariable<PropertyModel>,
        bool>::type
    expectDispatch(const PropertyModel &pm)
    {
        return pm.expect;
    }
    template<class PropertyModel>
    typename boost::enable_if<HasExpectMemberFunction<PropertyModel>,
        bool>::type
    expectDispatch(const PropertyModel &pm)
    {
        return pm.expect();
    }
    template<class PropertyModel>
    typename boost::disable_if_c<
        HasExpectMemberVariable<PropertyModel>::value ||
        HasExpectMemberFunction<PropertyModel>::value, bool>::type
    expectDispatch(const PropertyModel &pm)
    {
        return true;
    }

    template<class PropertyModel, class T0>
    typename boost::enable_if<HasClassifyMemberFunction<PropertyModel, T0>,
        std::string>::type
    classifyDispatch(const PropertyModel &pm, const T0 &x0)
    {
        return pm.classify(x0);
    }
    template<class PropertyModel, class T0>
    typename boost::disable_if<HasClassifyMemberFunction<PropertyModel, T0>,
        std::string>::type
    classifyDispatch(const PropertyModel &pm, const T0 &x0)
    {
        return detail::defaultClassifyFun(x0);
    }

    template<class PropertyModel, class T0>
    typename boost::enable_if<HasTrivialMemberFunction<PropertyModel, T0>,
        bool>::type
    trivialDispatch(const PropertyModel &pm, const T0 &x0)
    {
        return pm.trivial(x0);
    }
    template<class PropertyModel, class T0>
    typename boost::disable_if<HasTrivialMemberFunction<PropertyModel, T0>,
        bool>::type
    trivialDispatch(const PropertyModel &pm, const T0 &x0)
    {
        return detail::defaultTrivialFun(x0);
    }
}

template<class T0, class T1 = detail::null_type, class T2 = detail::null_type>
class Property
{
        template<class PropertyModel> class PropModel;
    public:
        typedef boost::tuple<T0, T1, T2> Input;

        template<class PropertyModel>
        Property(const PropertyModel &pm) :
            m_prop(new PropModel<PropertyModel>(pm))
        {
        }

        Property(const Property &p) :
            m_prop(p.m_prop->clone())
        {
        }

        template<class PropertyModel>
        Property &operator=(const PropertyModel &pm)
        {
            PropConcept *tmp = new PropModel<PropertyModel>(pm);
            delete m_prop;
            m_prop = tmp;
            return *this;
        }

        Property &operator=(const Property &p)
        {
            PropConcept *tmp = p.m_prop->clone();
            delete m_prop;
            m_prop = tmp;
            return *this;
        }

        ~Property()
        {
            delete m_prop;
        }

        Input generate(RngEngine &rng, std::size_t size) const
        {
            return m_prop->generate(rng, size);
        }

        std::vector<Input> shrink(const Input &in) const
        {
            return m_prop->shrink(in);
        }

        bool check(const Input &in) const
        {
            return m_prop->check(in);
        }

        std::string name() const
        {
            return m_prop->name();
        }

        bool expect() const
        {
            return m_prop->expect();
        }

        bool trivial(const Input &in) const
        {
            return m_prop->trivial(in);
        }

        std::string classify(const Input &in) const
        {
            return m_prop->classify(in);
        }

    private:
        struct PropConcept
        {
            virtual ~PropConcept()
            {
            }
            virtual Input generate(RngEngine &, std::size_t) const = 0;
            virtual std::vector<Input> shrink(const Input &) const = 0;
            virtual bool check(const Input &) const = 0;
            virtual std::string name() const = 0;
            virtual bool expect() const = 0;
            virtual bool trivial(const Input &) const = 0;
            virtual std::string classify(const Input &) const = 0;

            virtual PropConcept *clone() const = 0;
        };

        template<class PropertyModel>
        class PropModel : public PropConcept
        {
            public:
                PropModel(const PropertyModel &pm) :
                    m_obj(pm), m_gen(tupleOf(detail::gen0Dispatch<T0>(pm),
                                detail::gen1Dispatch<T1>(pm),
                                detail::gen2Dispatch<T2>(pm)))
                {
                }

                Input generate(RngEngine &rng, std::size_t size) const
                {
                    return m_gen.unGen(rng, size);
                }

                std::vector<Input> shrink(const Input &in) const
                {
                    return m_gen.shrink(in);
                }

                bool check(const Input &in) const
                {
                    return m_obj.check(boost::get<0>(in), boost::get<1>(in),
                            boost::get<2>(in));
                }

                std::string name() const
                {
                    return detail::nameDispatch(m_obj);
                }

                bool expect() const
                {
                    return detail::expectDispatch(m_obj);
                }

                bool trivial(const Input &in) const
                {
                    return detail::trivialDispatch(m_obj, boost::get<0>(in),
                            boost::get<1>(in), boost::get<2>(in));
                }

                std::string classify(const Input &in) const
                {
                    return detail::classifyDispatch(m_obj, boost::get<0>(in),
                            boost::get<1>(in), boost::get<2>(in));
                }

                PropConcept *clone() const
                {
                    return new PropModel(m_obj);
                }

            private:
                const PropertyModel m_obj;
                const Generator<Input> m_gen;
        };

        PropConcept *m_prop;
};

template<class T0, class T1>
class Property<T0, T1, detail::null_type>
{
        template<class PropertyModel> class PropModel;
    public:
        typedef boost::tuple<T0, T1> Input;

        template<class PropertyModel>
        Property(const PropertyModel &pm) :
            m_prop(new PropModel<PropertyModel>(pm))
        {
        }

        Property(const Property &p) :
            m_prop(p.m_prop->clone())
        {
        }

        template<class PropertyModel>
        Property &operator=(const PropertyModel &pm)
        {
            PropConcept *tmp = new PropModel<PropertyModel>(pm);
            delete m_prop;
            m_prop = tmp;
            return *this;
        }

        Property &operator=(const Property &p)
        {
            PropConcept *tmp = p.m_prop->clone();
            delete m_prop;
            m_prop = tmp;
            return *this;
        }

        ~Property()
        {
            delete m_prop;
        }

        Input generate(RngEngine &rng, std::size_t size) const
        {
            return m_prop->generate(rng, size);
        }

        std::vector<Input> shrink(const Input &in) const
        {
            return m_prop->shrink(in);
        }

        bool check(const Input &in) const
        {
            return m_prop->check(in);
        }

        std::string name() const
        {
            return m_prop->name();
        }

        bool expect() const
        {
            return m_prop->expect();
        }

        bool trivial(const Input &in) const
        {
            return m_prop->trivial(in);
        }

        std::string classify(const Input &in) const
        {
            return m_prop->classify(in);
        }

    private:
        struct PropConcept
        {
            virtual ~PropConcept()
            {
            }
            virtual Input generate(RngEngine &, std::size_t) const = 0;
            virtual std::vector<Input> shrink(const Input &) const = 0;
            virtual bool check(const Input &) const = 0;
            virtual std::string name() const = 0;
            virtual bool expect() const = 0;
            virtual bool trivial(const Input &) const = 0;
            virtual std::string classify(const Input &) const = 0;

            virtual PropConcept *clone() const = 0;
        };

        template<class PropertyModel>
        class PropModel : public PropConcept
        {
            public:
                PropModel(const PropertyModel &pm) :
                    m_obj(pm), m_gen(tupleOf(detail::gen0Dispatch<T0>(pm),
                                detail::gen1Dispatch<T1>(pm)))
                {
                }

                Input generate(RngEngine &rng, std::size_t size) const
                {
                    return m_gen.unGen(rng, size);
                }

                std::vector<Input> shrink(const Input &in) const
                {
                    return m_gen.shrink(in);
                }

                bool check(const Input &in) const
                {
                    return m_obj.check(boost::get<0>(in), boost::get<1>(in));
                }

                std::string name() const
                {
                    return detail::nameDispatch(m_obj);
                }

                bool expect() const
                {
                    return detail::expectDispatch(m_obj);
                }

                bool trivial(const Input &in) const
                {
                    return detail::trivialDispatch(m_obj, boost::get<0>(in),
                            boost::get<1>(in));
                }

                std::string classify(const Input &in) const
                {
                    return detail::classifyDispatch(m_obj, boost::get<0>(in),
                            boost::get<1>(in));
                }

                PropConcept *clone() const
                {
                    return new PropModel(m_obj);
                }

            private:
                const PropertyModel m_obj;
                const Generator<Input> m_gen;
        };

        PropConcept *m_prop;
};

template<class T0>
class Property<T0, detail::null_type, detail::null_type>
{
        template<class PropertyModel> class PropModel;
    public:
        typedef boost::tuple<T0> Input;

        template<class PropertyModel>
        Property(const PropertyModel &pm) :
            m_prop(new PropModel<PropertyModel>(pm))
        {
        }

        Property(const Property &p) :
            m_prop(p.m_prop->clone())
        {
        }

        template<class PropertyModel>
        Property &operator=(const PropertyModel &pm)
        {
            PropConcept *tmp = new PropModel<PropertyModel>(pm);
            delete m_prop;
            m_prop = tmp;
            return *this;
        }

        Property &operator=(const Property &p)
        {
            PropConcept *tmp = p.m_prop->clone();
            delete m_prop;
            m_prop = tmp;
            return *this;
        }

        ~Property()
        {
            delete m_prop;
        }

        Input generate(RngEngine &rng, std::size_t size) const
        {
            return m_prop->generate(rng, size);
        }

        std::vector<Input> shrink(const Input &in) const
        {
            return m_prop->shrink(in);
        }

        bool check(const Input &in) const
        {
            return m_prop->check(in);
        }

        std::string name() const
        {
            return m_prop->name();
        }

        bool expect() const
        {
            return m_prop->expect();
        }

        bool trivial(const Input &in) const
        {
            return m_prop->trivial(in);
        }

        std::string classify(const Input &in) const
        {
            return m_prop->classify(in);
        }

    private:
        struct PropConcept
        {
            virtual ~PropConcept()
            {
            }
            virtual Input generate(RngEngine &, std::size_t) const = 0;
            virtual std::vector<Input> shrink(const Input &) const = 0;
            virtual bool check(const Input &) const = 0;
            virtual std::string name() const = 0;
            virtual bool expect() const = 0;
            virtual bool trivial(const Input &) const = 0;
            virtual std::string classify(const Input &) const = 0;

            virtual PropConcept *clone() const = 0;
        };

        template<class PropertyModel>
        class PropModel : public PropConcept
        {
            public:
                PropModel(const PropertyModel &pm) :
                    m_obj(pm), m_gen(tupleOf(detail::gen0Dispatch<T0>(pm)))
                {
                }

                Input generate(RngEngine &rng, std::size_t size) const
                {
                    return m_gen.unGen(rng, size);
                }

                std::vector<Input> shrink(const Input &in) const
                {
                    return m_gen.shrink(in);
                }

                bool check(const Input &in) const
                {
                    return m_obj.check(boost::get<0>(in));
                }

                std::string name() const
                {
                    return detail::nameDispatch(m_obj);
                }

                bool expect() const
                {
                    return detail::expectDispatch(m_obj);
                }

                bool trivial(const Input &in) const
                {
                    return detail::trivialDispatch(m_obj, boost::get<0>(in));
                }

                std::string classify(const Input &in) const
                {
                    return detail::classifyDispatch(m_obj, boost::get<0>(in));
                }

                PropConcept *clone() const
                {
                    return new PropModel(m_obj);
                }

            private:
                const PropertyModel m_obj;
                const Generator<Input> m_gen;
        };

        PropConcept *m_prop;
};



namespace detail {
    template<class T0>
    struct FunProp
    {
        typedef typename Property<T0>::Input Input;

        FunProp(boost::function<bool (T0)> checkFun,
                const Generator<T0> &gen0) :
            m_check(checkFun), gen0(gen0)
        {
        }

        bool check(const Input &in) const
        {
            return m_check(boost::get<0>(in));
        }

        const boost::function<bool (T0)> m_check;
        const Generator<T0> gen0;
    };
}

template<class T0>
Property<T0> funProp(boost::function<bool (T0)> checkFun,
        const Generator<T0> &gen0 = Arbitrary<T0>())
{
    return detail::FunProp<T0>(checkFun, gen0);
}


// property combinators

// named
// expectFailure
// classifyWith
// trivializeWith


    template<class T0>
    std::ostream &printInput(std::ostream &out, const boost::tuple<T0> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1>
    std::ostream &printInput(std::ostream &out, const boost::tuple<T0, T1> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3, T4> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << "  4: " << boost::get<4>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4, class T5>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3, T4, T5> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << "  4: " << boost::get<4>(in) << '\n'
            << "  5: " << boost::get<5>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3, T4, T5, T6> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << "  4: " << boost::get<4>(in) << '\n'
            << "  5: " << boost::get<5>(in) << '\n'
            << "  6: " << boost::get<6>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << "  4: " << boost::get<4>(in) << '\n'
            << "  5: " << boost::get<5>(in) << '\n'
            << "  6: " << boost::get<6>(in) << '\n'
            << "  7: " << boost::get<7>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << "  4: " << boost::get<4>(in) << '\n'
            << "  5: " << boost::get<5>(in) << '\n'
            << "  6: " << boost::get<6>(in) << '\n'
            << "  7: " << boost::get<7>(in) << '\n'
            << "  8: " << boost::get<8>(in) << '\n'
            << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
    std::ostream &printInput(std::ostream &out,
            const boost::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> &in)
    {
        return out << "  0: " << boost::get<0>(in) << '\n'
            << "  1: " << boost::get<1>(in) << '\n'
            << "  2: " << boost::get<2>(in) << '\n'
            << "  3: " << boost::get<3>(in) << '\n'
            << "  4: " << boost::get<4>(in) << '\n'
            << "  5: " << boost::get<5>(in) << '\n'
            << "  6: " << boost::get<6>(in) << '\n'
            << "  7: " << boost::get<7>(in) << '\n'
            << "  8: " << boost::get<8>(in) << '\n'
            << "  9: " << boost::get<9>(in) << '\n'
            << std::flush;
    }




}

#endif
