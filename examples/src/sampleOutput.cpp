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
("tuple",          boost::bind(sampleOutput<std::tuple<int,int,int> >, tupleOf<int,int,int>(),    boost::ref(std::cout), 0, 0))
("string",         boost::bind(sampleOutput<std::string>,         Arbitrary<std::string>(),         boost::ref(std::cout), 0, 0));

int main(int argc, char **argv)
{
    if(argc == 1) {
        std::cout << "Usage: TYPES... (e.g., int, double, string)\n";
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        auto it = sampleOutputCommand.find(argv[i]);
        if (it != sampleOutputCommand.end())
            it->second();
        else
            std::cout << "unrecognized type \"" << argv[i] << "\"\n";
    }
}
