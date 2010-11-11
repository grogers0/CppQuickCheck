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

#include <algorithm>
#include <boost/static_assert.hpp>
#include <sstream>

namespace std {
    std::ostream &operator<<(std::ostream &out, const std::vector<int> &v)
    {
        out << "[";
        for (std::vector<int>::const_iterator it = v.begin(); it != v.end();
                ++it) {
            if (it != v.begin())
                out << ", ";
            out << *it;
        }
        out << "]";
        return out;
    }
}

struct PropTestReverse : cppqc::Property<std::vector<int> >
{
    PropTestReverse() : Property(cppqc::listOf<int>()) {}
    bool check(const std::vector<int> &v) const
    {
        std::vector<int> vrev(v);
        std::reverse(vrev.begin(), vrev.end());
        std::reverse(vrev.begin(), vrev.end());
        return std::equal(v.begin(), v.end(), vrev.begin());
    }
    std::string name() const
    {
        return "Reversing Twice is Identity";
    }
    std::string classify(const std::vector<int> &v) const
    {
        std::ostringstream sstr;
        sstr << "size " << v.size();
        return sstr.str();
    }
    bool trivial(const std::vector<int> &v) const
    {
        return v.empty() || v.size() == 1;
    }
};

int main()
{
    cppqc::quickCheckOutput(PropTestReverse());
}
