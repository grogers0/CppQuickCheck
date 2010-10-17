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

        struct Input {
            T1 x1;
            Input(const T1 &x1) : x1(x1) {}
        };

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return Input(generator1()(rng, size));
        }

        bool checkInput(const Input &input) const
        {
            return check(input.x1);
        }

        bool isTrivialForInput(const Input &input) const
        {
            return isTrivialFor(input.x1);
        }

        std::string classifyInput(const Input &input) const
        {
            return classify(input.x1);
        }

        void printInput(std::ostream &out, const Input &input) const
        {
            out << "  1: " << input.x1 << std::endl;
        }

        std::size_t shrinkInput(std::ostream &out, const Input &input) const
        {
            std::size_t numShrinks = 0;
            try {
                Input shrunk = input;

continueShrinking:
                out << "Shrinking..." << std::flush;

                std::vector<T1> xshr1 = shrink1()(shrunk.x1);
                for (typename std::vector<T1>::const_iterator it = xshr1.begin();
                        it != xshr1.end(); ++it) {
                    Input inTest(*it);
                    if (!checkInput(inTest)) {
                        shrunk = inTest;
                        out << "Found shrunk input:" << std::endl;
                        printInput(out, shrunk);
                        goto continueShrinking;
                    }
                }

                out << "Could not shrink any farther (Shrunk "
                    << numShrinks << " times)." << std::endl;
            } catch (...) {
                out << "Caught exception while generating shrinks (Shrunk "
                    << numShrinks << " times)!" << std::endl;
            }
            return numShrinks;
        }

    private:
        virtual bool check(const T1 &) const = 0;
        virtual bool isTrivialFor(const T1 &) const;
        virtual std::string classify(const T1 &) const;

        virtual typename Gen<T1>::type generator1() const;
        virtual typename Shrink<T1>::type shrink1() const;
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

template<class T1>
bool Property<T1>::isTrivialFor(const T1 &x1) const
{
    return false;
}

template<class T1>
std::string Property<T1>::classify(const T1 &x1) const
{
    return std::string();
}

}

#endif
