#include "cppqc/Test.h"
#include <algorithm>
#include <boost/static_assert.hpp>
#include <sstream>

namespace std{
std::ostream &operator<<(std::ostream &out, const std::vector<int> &v)
{
    out << "[";
    for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it) {
        if (it != v.begin())
            out << ", ";
        out << *it;
    }
    out << "]";
    return out;
}
}

struct PropTestReverse
{
    static bool check(const std::vector<int> &v)
    {
        std::vector<int> vrev(v);
        std::reverse(vrev.begin(), vrev.end());
        std::reverse(vrev.begin(), vrev.end());
        return std::equal(v.begin(), v.end(), vrev.begin());
    }
    static const std::string name;
    static std::string classify(const std::vector<int> &v)
    {
        std::ostringstream sstr;
        sstr << "size " << v.size();
        return sstr.str();
    }
    static bool trivial(const std::vector<int> &v)
    {
        return v.empty() || v.size() == 1;
    }
    static const cppqc::Generator<std::vector<int> > gen1;
};
const cppqc::Generator<std::vector<int> > PropTestReverse::gen1 = cppqc::listOf<int>();
const std::string PropTestReverse::name = "Reversing Twice is Identity";

/*
std::string PropTestReverse::name() const
{
    return "PropTestReverse";
}
*/

int main()
{
    cppqc::quickCheckOutput(cppqc::Property<std::vector<int> >(PropTestReverse()));
}
