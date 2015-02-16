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
#include <iterator>
#include <boost/static_assert.hpp>
#include <sstream>

namespace uut {

template <typename InputIterator>
void selection_sort(InputIterator b, InputIterator e, bool make_mistakes = false)
{
    //Selection sort performs the following steps:
    //1) From the current iterator, find the smallest value
    //2) Swap the smallest value with the current iterator
    //3) Continue until end of range

    make_mistakes && b != e ? ++b : b;
    for(InputIterator c = b; c != e ; ++c)
    {
        std::swap(*(std::min_element(c, e)), *c);
    }
}

}

struct PropTestSort: cppqc::Property<std::vector<int>>
{
    bool check(const std::vector<int> &v) const override
    {
        std::vector<int> v_copy(v);
        uut::selection_sort(std::begin(v_copy), std::end(v_copy), true);
        return std::is_sorted(std::begin(v_copy), std::end(v_copy));
    }
    std::string name() const override
    {
        return "Sorting should be sorted";
    }
    std::string classify(const std::vector<int> &v) const override
    {
        std::ostringstream sstr;
        sstr << "size " << v.size();
        return sstr.str();
    }
    bool trivial(const std::vector<int> &v) const override
    {
        return v.empty() || v.size() == 1;
    }
};

int main()
{
    cppqc::quickCheckOutput(PropTestSort());
}
