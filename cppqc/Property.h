#ifndef CPPQC_PROPERTY_H
#define CPPQC_PROPERTY_H

#include "Generator.h"
#include "Arbitrary.h"

#include <stdexcept>
#include <typeinfo>

namespace cppqc {

template<class T1>
class Property
{
    public:
        Property(const Generator<T1> &g1 = Arbitrary<T1>()) : m_gen1(g1) {}

        virtual bool expect() const;
        virtual std::string name() const;

        struct Input {
            T1 x1;
            Input(const T1 &x1) : x1(x1) {}
        };

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return Input(m_gen1.unGen(rng, size));
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

        std::pair<std::size_t, Input> shrinkInput(const Input &input) const
        {
            std::size_t numShrinks = 0;
            Input shrunk = input;

continueShrinking:

            try {
                std::vector<T1> xshr1 = m_gen1.shrink(shrunk.x1);
                for (typename std::vector<T1>::const_iterator it = xshr1.begin();
                        it != xshr1.end(); ++it) {
                    Input inTest(*it);
                    if (!checkInput(inTest)) {
                        shrunk = inTest;
                        numShrinks++;
                        goto continueShrinking;
                    }
                }
            } catch (...) {
            }
            return std::make_pair(numShrinks, shrunk);
        }

    private:
        virtual bool check(const T1 &) const = 0;
        virtual bool isTrivialFor(const T1 &) const;
        virtual std::string classify(const T1 &) const;

        Generator<T1> m_gen1;
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
