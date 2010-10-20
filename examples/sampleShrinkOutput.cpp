#include "cppqc/Generator.h"
#include "cppqc/Arbitrary.h"

#include <map>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

using namespace cppqc;

template<class T1, class T2>
std::ostream &operator<<(std::ostream &out, const std::pair<T1, T2> &x)
{
	return out << '(' << x.first << ',' << x.second << ')';
}

const std::map<std::string, boost::function<void ()> >
sampleShrinkOutputCommand = boost::assign::map_list_of<std::string, boost::function<void ()> >
("bool",           boost::bind(sampleShrinkOutput<bool>,                Arbitrary<bool>(),                boost::ref(std::cout), 0, false, 0))
("char",           boost::bind(sampleShrinkOutput<char>,                Arbitrary<char>(),                boost::ref(std::cout), 0, false, 0))
("wchar_t",        boost::bind(sampleShrinkOutput<wchar_t>,             Arbitrary<wchar_t>(),             boost::ref(std::cout), 0, false, 0))
("signed char",    boost::bind(sampleShrinkOutput<signed char>,         Arbitrary<signed char>(),         boost::ref(std::cout), 0, false, 0))
("unsigned char",  boost::bind(sampleShrinkOutput<unsigned char>,       Arbitrary<unsigned char>(),       boost::ref(std::cout), 0, false, 0))
("short",          boost::bind(sampleShrinkOutput<short>,               Arbitrary<short>(),               boost::ref(std::cout), 0, false, 0))
("signed short",   boost::bind(sampleShrinkOutput<signed short>,        Arbitrary<signed short>(),        boost::ref(std::cout), 0, false, 0))
("unsigned short", boost::bind(sampleShrinkOutput<unsigned short>,      Arbitrary<unsigned short>(),      boost::ref(std::cout), 0, false, 0))
("int",            boost::bind(sampleShrinkOutput<int>,                 Arbitrary<int>(),                 boost::ref(std::cout), 0, false, 0))
("signed",         boost::bind(sampleShrinkOutput<signed>,              Arbitrary<signed>(),              boost::ref(std::cout), 0, false, 0))
("unsigned",       boost::bind(sampleShrinkOutput<unsigned>,            Arbitrary<unsigned>(),            boost::ref(std::cout), 0, false, 0))
("signed int",     boost::bind(sampleShrinkOutput<signed int>,          Arbitrary<signed int>(),          boost::ref(std::cout), 0, false, 0))
("unsigned int",   boost::bind(sampleShrinkOutput<unsigned int>,        Arbitrary<unsigned int>(),        boost::ref(std::cout), 0, false, 0))
("long",           boost::bind(sampleShrinkOutput<long>,                Arbitrary<long>(),                boost::ref(std::cout), 0, false, 0))
("signed long",    boost::bind(sampleShrinkOutput<signed long>,         Arbitrary<signed long>(),         boost::ref(std::cout), 0, false, 0))
("unsigned long",  boost::bind(sampleShrinkOutput<unsigned long>,       Arbitrary<unsigned long>(),       boost::ref(std::cout), 0, false, 0))
("float",          boost::bind(sampleShrinkOutput<float>,               Arbitrary<float>(),               boost::ref(std::cout), 0, false, 0))
("double",         boost::bind(sampleShrinkOutput<double>,              Arbitrary<double>(),              boost::ref(std::cout), 0, false, 0))
("long double",    boost::bind(sampleShrinkOutput<long double>,         Arbitrary<long double>(),         boost::ref(std::cout), 0, false, 0))
("pair",           boost::bind(sampleShrinkOutput<std::pair<int,int> >, Arbitrary<std::pair<int,int> >(), boost::ref(std::cout), 0, false, 0))
("string",         boost::bind(sampleShrinkOutput<std::string>,         Arbitrary<std::string>(),         boost::ref(std::cout), 0, false, 0));

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        std::map<std::string, boost::function<void ()> >::const_iterator it =
            sampleShrinkOutputCommand.find(argv[i]);
        if (it != sampleShrinkOutputCommand.end())
            it->second();
        else
            std::cout << "unrecognized type \"" << argv[i] << "\"\n";
    }
}
