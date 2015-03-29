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

#include "cppqc.h"

#include <map>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

using namespace cppqc;

namespace std {
    template<class T1, class T2>
    std::ostream &operator<<(std::ostream &out, const std::pair<T1, T2> &x)
    {
        return out << '(' << x.first << ',' << x.second << ')';
    }
}

const std::map<std::string, boost::function<void ()> >
sampleShrinkOutputCommand = boost::assign::map_list_of<std::string, boost::function<void ()> >
("bool",           boost::bind(sampleShrinkOutput<bool>,                Arbitrary<bool>(),                boost::ref(std::cout), 0, true, 0))
("char",           boost::bind(sampleShrinkOutput<char>,                Arbitrary<char>(),                boost::ref(std::cout), 0, true, 0))
("wchar_t",        boost::bind(sampleShrinkOutput<wchar_t>,             Arbitrary<wchar_t>(),             boost::ref(std::cout), 0, true, 0))
("signed char",    boost::bind(sampleShrinkOutput<signed char>,         Arbitrary<signed char>(),         boost::ref(std::cout), 0, true, 0))
("unsigned char",  boost::bind(sampleShrinkOutput<unsigned char>,       Arbitrary<unsigned char>(),       boost::ref(std::cout), 0, true, 0))
("short",          boost::bind(sampleShrinkOutput<short>,               Arbitrary<short>(),               boost::ref(std::cout), 0, true, 0))
("signed short",   boost::bind(sampleShrinkOutput<signed short>,        Arbitrary<signed short>(),        boost::ref(std::cout), 0, true, 0))
("unsigned short", boost::bind(sampleShrinkOutput<unsigned short>,      Arbitrary<unsigned short>(),      boost::ref(std::cout), 0, true, 0))
("int",            boost::bind(sampleShrinkOutput<int>,                 Arbitrary<int>(),                 boost::ref(std::cout), 0, true, 0))
("signed",         boost::bind(sampleShrinkOutput<signed>,              Arbitrary<signed>(),              boost::ref(std::cout), 0, true, 0))
("unsigned",       boost::bind(sampleShrinkOutput<unsigned>,            Arbitrary<unsigned>(),            boost::ref(std::cout), 0, true, 0))
("signed int",     boost::bind(sampleShrinkOutput<signed int>,          Arbitrary<signed int>(),          boost::ref(std::cout), 0, true, 0))
("unsigned int",   boost::bind(sampleShrinkOutput<unsigned int>,        Arbitrary<unsigned int>(),        boost::ref(std::cout), 0, true, 0))
("long",           boost::bind(sampleShrinkOutput<long>,                Arbitrary<long>(),                boost::ref(std::cout), 0, true, 0))
("signed long",    boost::bind(sampleShrinkOutput<signed long>,         Arbitrary<signed long>(),         boost::ref(std::cout), 0, true, 0))
("unsigned long",  boost::bind(sampleShrinkOutput<unsigned long>,       Arbitrary<unsigned long>(),       boost::ref(std::cout), 0, true, 0))
("float",          boost::bind(sampleShrinkOutput<float>,               Arbitrary<float>(),               boost::ref(std::cout), 0, true, 0))
("double",         boost::bind(sampleShrinkOutput<double>,              Arbitrary<double>(),              boost::ref(std::cout), 0, true, 0))
("long double",    boost::bind(sampleShrinkOutput<long double>,         Arbitrary<long double>(),         boost::ref(std::cout), 0, true, 0))
("pair",           boost::bind(sampleShrinkOutput<std::pair<int,int> >, Arbitrary<std::pair<int,int> >(), boost::ref(std::cout), 0, true, 0))
("tuple",          boost::bind(sampleShrinkOutput<std::tuple<int,int,int> >, tupleOf<int,int,int>(),    boost::ref(std::cout), 0, true, 0))
("string",         boost::bind(sampleShrinkOutput<std::string>,         Arbitrary<std::string>(),         boost::ref(std::cout), 0, true, 0));

int main(int argc, char **argv)
{
    if(argc == 1) {
        std::cout << "Usage: TYPES... (e.g., int, double, string)\n";
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        std::map<std::string, boost::function<void ()> >::const_iterator it =
            sampleShrinkOutputCommand.find(argv[i]);
        if (it != sampleShrinkOutputCommand.end())
            it->second();
        else
            std::cout << "unrecognized type \"" << argv[i] << "\"\n";
    }
}
