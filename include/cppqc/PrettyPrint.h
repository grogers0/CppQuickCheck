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

#ifndef CPPQC_PRETTY_PRINT_H
#define CPPQC_PRETTY_PRINT_H

#include "cxx-prettyprint.h"
#include <string>
#include <sstream>

namespace cppqc {

    // The default implementation falls back to the std::ofstream
    // implementation. Note that because of the usage of "cxx-prettyprint",
    // it can handle understands C++ containers, pairs and tuples.
    //
    // Strings are quoted to improve readability.
    //
    template <class T>
    struct PrettyPrint
    {
        static std::string toString(const T& x)
        {
            std::ostringstream out;
            out << x;
            return out.str();
        }
    };

    template <>
    struct PrettyPrint<std::string>
    {
        static std::string toString(const std::string& x)
        {
            return '\"' + x + '\"';
        }
    };

    template <class T>
    std::string prettyPrint(const T& x)
    {
        return PrettyPrint<T>::toString(x);
    }

}

#endif
