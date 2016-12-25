/*
 * Copyright (c) 2016, Philipp Classen All rights reserved.
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

#include "cppqc/Arbitrary.h"
#include "catch.hpp"
#include "cppqc/CompactCheck.h"

using namespace cppqc;

TEST_CASE("minimal passing example in compact check representation",
          "[functional][compact]")
{
    const Result result = cppqc::gen<bool>()
        .property("Dummy check (always passing)",
            [](const bool &v)
            {
                return true;
            })
        .testWithOutput();

    REQUIRE(result.result == QC_SUCCESS);
}

TEST_CASE("minimal failing example in compact check representation",
          "[functional][compact]")
{
    const Result result = cppqc::gen<bool>()
        .property("Dummy check (always failing)",
            [](const bool &v)
            {
                return false; // Intended to fail
            })
        .testWithOutput();

    REQUIRE(result.result == QC_FAILURE);
}

TEST_CASE("non-trivial example with std::sort should pass all tests",
          "[functional][compact]")
{
    const Result result = cppqc::gen<std::vector<int>>()
        .property("Should be sorted after calling std::sort",
            [](const std::vector<int> &v)
            {
                std::vector<int> v_copy(v);
                std::sort(std::begin(v_copy), std::end(v_copy));
                return std::is_sorted(std::begin(v_copy), std::end(v_copy));
            })
        .classify([](const std::vector<int> &v)
            {
                return std::to_string(v.size());
            })
        .trivial([](const std::vector<int> &v)
            {
                return v.empty() || v.size() == 1;
            })
        .testWithOutput();

    REQUIRE(result.result == QC_SUCCESS);
}
