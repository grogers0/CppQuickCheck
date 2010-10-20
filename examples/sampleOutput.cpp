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
sampleOutputCommand = boost::assign::map_list_of<std::string, boost::function<void ()> >
("bool",           boost::bind(sampleOutput<bool>,                Arbitrary<bool>(),                boost::ref(std::cout), 0, 0))
("char",           boost::bind(sampleOutput<char>,                Arbitrary<char>(),                boost::ref(std::cout), 0, 0))
("wchar_t",        boost::bind(sampleOutput<wchar_t>,             Arbitrary<wchar_t>(),             boost::ref(std::cout), 0, 0))
("signed char",    boost::bind(sampleOutput<signed char>,         Arbitrary<signed char>(),         boost::ref(std::cout), 0, 0))
("unsigned char",  boost::bind(sampleOutput<unsigned char>,       Arbitrary<unsigned char>(),       boost::ref(std::cout), 0, 0))
("short",          boost::bind(sampleOutput<short>,               Arbitrary<short>(),               boost::ref(std::cout), 0, 0))
("signed short",   boost::bind(sampleOutput<signed short>,        Arbitrary<signed short>(),        boost::ref(std::cout), 0, 0))
("unsigned short", boost::bind(sampleOutput<unsigned short>,      Arbitrary<unsigned short>(),      boost::ref(std::cout), 0, 0))
("int",            boost::bind(sampleOutput<int>,                 Arbitrary<int>(),                 boost::ref(std::cout), 0, 0))
("signed",         boost::bind(sampleOutput<signed>,              Arbitrary<signed>(),              boost::ref(std::cout), 0, 0))
("unsigned",       boost::bind(sampleOutput<unsigned>,            Arbitrary<unsigned>(),            boost::ref(std::cout), 0, 0))
("signed int",     boost::bind(sampleOutput<signed int>,          Arbitrary<signed int>(),          boost::ref(std::cout), 0, 0))
("unsigned int",   boost::bind(sampleOutput<unsigned int>,        Arbitrary<unsigned int>(),        boost::ref(std::cout), 0, 0))
("long",           boost::bind(sampleOutput<long>,                Arbitrary<long>(),                boost::ref(std::cout), 0, 0))
("signed long",    boost::bind(sampleOutput<signed long>,         Arbitrary<signed long>(),         boost::ref(std::cout), 0, 0))
("unsigned long",  boost::bind(sampleOutput<unsigned long>,       Arbitrary<unsigned long>(),       boost::ref(std::cout), 0, 0))
("float",          boost::bind(sampleOutput<float>,               Arbitrary<float>(),               boost::ref(std::cout), 0, 0))
("double",         boost::bind(sampleOutput<double>,              Arbitrary<double>(),              boost::ref(std::cout), 0, 0))
("long double",    boost::bind(sampleOutput<long double>,         Arbitrary<long double>(),         boost::ref(std::cout), 0, 0))
("pair",           boost::bind(sampleOutput<std::pair<int,int> >, Arbitrary<std::pair<int,int> >(), boost::ref(std::cout), 0, 0))
("string",         boost::bind(sampleOutput<std::string>,         Arbitrary<std::string>(),         boost::ref(std::cout), 0, 0));

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        std::map<std::string, boost::function<void ()> >::const_iterator it =
            sampleOutputCommand.find(argv[i]);
        if (it != sampleOutputCommand.end())
            it->second();
        else
            std::cout << "unrecognized type \"" << argv[i] << "\"\n";
    }
}
