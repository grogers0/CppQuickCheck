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


    CPPQC_STAMP_HasGenXMemberVariable(1);
    CPPQC_STAMP_HasGenXMemberFunction(1);

    template<class T1>
    struct Input
    {
        T1 x1;

        Input(const T1 &x1) :
            x1(x1)
        {
        }
    };

    template<class T1>
    struct PropConcept
    {
        virtual ~PropConcept()
        {
        }
        virtual detail::Input<T1> generate(RngEngine &, std::size_t) const = 0;
        virtual std::vector<detail::Input<T1> >
        shrink(const detail::Input<T1> &) const = 0;
        virtual bool check(const detail::Input<T1> &) const = 0;
        virtual std::string name() const = 0;
        virtual bool expect() const = 0;
        virtual bool trivial(const detail::Input<T1> &) const = 0;
        virtual std::string classify(const detail::Input<T1> &) const = 0;

        virtual PropConcept *clone() const = 0;
    };

    template<class T1, class PropertyModel>
    typename boost::enable_if<HasStatelessGen1MemberVariable<PropertyModel, T1>,
        const StatelessGenerator<T1> &>::type
    gen1Dispatch(const PropertyModel &pm)
    {
        return pm.gen1;
    }
    template<class T1, class PropertyModel>
    typename boost::enable_if<HasGen1MemberVariable<PropertyModel, T1>,
        const Generator<T1> &>::type
    gen1Dispatch(const PropertyModel &pm)
    {
        return pm.gen1;
    }
    template<class T1, class PropertyModel>
    typename boost::enable_if<HasStatelessGen1MemberFunction<PropertyModel, T1>,
        const StatelessGenerator<T1> &>::type
    gen1Dispatch(const PropertyModel &pm)
    {
        return pm.gen1();
    }
    template<class T1, class PropertyModel>
    typename boost::enable_if<HasGen1MemberFunction<PropertyModel, T1>,
        const Generator<T1> &>::type
    gen1Dispatch(const PropertyModel &pm)
    {
        return pm.gen1();
    }
    template<class T1, class PropertyModel>
    typename boost::disable_if_c<
        HasGen1MemberVariable<PropertyModel, T1>::value ||
            HasStatelessGen1MemberVariable<PropertyModel, T1>::value ||
            HasGen1MemberFunction<PropertyModel, T1>::value ||
            HasStatelessGen1MemberFunction<PropertyModel, T1>::value,
        StatelessGenerator<T1> >::type
    gen1Dispatch(const PropertyModel &pm)
    {
        return Arbitrary<T1>();
    }

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

    template<class PropertyModel, class T1>
    typename boost::enable_if<HasClassifyMemberFunction<PropertyModel, T1>,
        std::string>::type
    classifyDispatch(const PropertyModel &pm, const T1 &x1)
    {
        return pm.classify(x1);
    }
    template<class PropertyModel, class T1>
    typename boost::disable_if<HasClassifyMemberFunction<PropertyModel, T1>,
        std::string>::type
    classifyDispatch(const PropertyModel &pm, const T1 &x1)
    {
        return detail::defaultClassifyFun(x1);
    }

    template<class PropertyModel, class T1>
    typename boost::enable_if<HasTrivialMemberFunction<PropertyModel, T1>,
        bool>::type
    trivialDispatch(const PropertyModel &pm, const T1 &x1)
    {
        return pm.trivial(x1);
    }
    template<class PropertyModel, class T1>
    typename boost::disable_if<HasTrivialMemberFunction<PropertyModel, T1>,
        bool>::type
    trivialDispatch(const PropertyModel &pm, const T1 &x1)
    {
        return detail::defaultTrivialFun(x1);
    }
}

template<class T1>
class Property
{
        template<class PropertyModel> class PropModel;
    public:
        typedef detail::Input<T1> Input;

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
            delete m_prop;
            m_prop = new PropModel<PropertyModel>(pm);
            return *this;
        }

        Property &operator=(const Property &p)
        {
            delete m_prop;
            m_prop = p.m_prop->clone();
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
        template<class PropertyModel>
        class PropModel : public detail::PropConcept<T1>
        {
            public:
                PropModel(const PropertyModel &pm) : m_obj(pm)
                {
                }

                Input generate(RngEngine &rng, std::size_t size) const
                {
                    return Input(detail::gen1Dispatch<T1>(m_obj).unGen(rng,
                                size));
                }

                std::vector<Input> shrink(const Input &in) const
                {
                    std::vector<T1> shrinks1 =
                        detail::gen1Dispatch<T1>(m_obj).shrink(in.x1);
                    std::vector<Input> ret;
                    ret.reserve(shrinks1.size());
                    for (typename std::vector<T1>::const_iterator it =
                            shrinks1.begin(); it != shrinks1.end(); ++it) {
                        ret.push_back(Input(*it));
                    }
                    return ret;
                }

                // note: all PropertyModel's must have a check function, if the
                // property is not a class with this function, use funProp to
                // create a Property.
                bool check(const Input &in) const
                {
                    return m_obj.check(in.x1);
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
                    return detail::trivialDispatch(m_obj, in.x1);
                }

                std::string classify(const Input &in) const
                {
                    return detail::classifyDispatch(m_obj, in.x1);
                }

                detail::PropConcept<T1> *clone() const
                {
                    return new PropModel(m_obj);
                }

            private:
                const PropertyModel m_obj;
        };

        detail::PropConcept<T1> *m_prop;
};


namespace detail {
    template<class T1>
    struct FunProp
    {
        typedef typename Property<T1>::Input Input;

        FunProp(boost::function<bool (T1)> checkFun,
                const Generator<T1> &gen1) :
            m_check(checkFun), gen1(gen1)
        {
        }

        bool check(const Input &in) const
        {
            return m_check(in.x1);
        }

        const boost::function<bool (T1)> m_check;
        const Generator<T1> gen1;
    };
}

template<class T1>
Property<T1> funProp(boost::function<bool (T1)> checkFun,
        const Generator<T1> &gen1 = Arbitrary<T1>())
{
    return detail::FunProp<T1>(checkFun, gen1);
}


// property combinators

// named
// expectFailure
// classifyWith
// trivializeWith


}

namespace std {
    template<class T1>
    std::ostream &operator<<(std::ostream &out,
            const cppqc::detail::Input<T1> &in)
    {
        return out << "  1: " << in.x1 << '\n'
            << std::flush;
    }
}

#endif
