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

#include "cppqc.h"
#include "catch.hpp"

using namespace cppqc;

namespace FunctionalTestsFixtures {

struct MinimalPassingProperty : cppqc::Property<bool>
{
    bool check(const bool &v) const override
    {
        return true;
    }
    std::string name() const override
    {
        return "Dummy check (will always pass)";
    }
};

struct MinimalFailingProperty : cppqc::Property<bool>
{
    bool check(const bool &v) const override
    {
        return false; // fails intentionally
    }
    std::string name() const override
    {
        return "Dummy check (will always fail)";
    }
};

} // end FunctionalTestsFixtures

TEST_CASE("minimal passing example",
          "[functional]")
{
    const Result result =
        quickCheckOutput(FunctionalTestsFixtures::MinimalPassingProperty{});

    REQUIRE(result.result == QC_SUCCESS);
}

TEST_CASE("minimal failing example",
          "[functional]")
{
    const Result result =
        quickCheckOutput(FunctionalTestsFixtures::MinimalFailingProperty{});

    REQUIRE(result.result == QC_FAILURE);
}
