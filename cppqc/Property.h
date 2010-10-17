#ifndef CPPQC_PROPERTY_H
#define CPPQC_PROPERTY_H

#include "Gen.h"
#include "Arbitrary.h"

#include <stdexcept>
#include <typeinfo>

namespace cppqc {

template<class T1>
class Property
{
    public:
        virtual bool expect() const;
        virtual std::string name() const;
        bool test(RngEngine &rng, std::size_t size) const
        {
            x1 = generator1()(rng, size);
            return property(x1);
        }

    private:
        virtual bool property(const T1 &) const = 0;
        virtual typename Gen<T1>::type generator1() const;
        virtual typename Shrink<T1>::type shrink1() const;

        // fixme - make it so we don't need a default constructor, only
        // a copy constructor
        T1 x1;
};

template<class T1>
bool Property<T1>::expect() const
{
    return true;
}

template<class T1>
std::string Property<T1>::name() const
{
    return typeid(*this).name();
}

template<class T1>
typename Gen<T1>::type Property<T1>::generator1() const
{
    return Arbitrary<T1>::generator;
}

template<class T1>
typename Shrink<T1>::type Property<T1>::shrink1() const
{
    return Arbitrary<T1>::shrink;
}

}

#endif
