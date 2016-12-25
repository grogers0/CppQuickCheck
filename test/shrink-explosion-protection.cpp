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

#include "cppqc/cxx-prettyprint.h"

using namespace cppqc;

// Shrinking one value should eventually end, and the
// intermediate state should not contain too many entries.
template <typename T, typename ShrinkFunction,
	  int MAX_EXPECTED_RUNS = 1000, int MAX_EXPECTED_ENTRIES = 1000>
void checkShrinkExplosions(T initialValue, ShrinkFunction shrinkFunction)
{
    std::vector<T> current = shrinkFunction(initialValue);
    for (int i = 0; i < MAX_EXPECTED_RUNS; i++) {
        std::vector<T> worstCase;
        for (const auto& x : current) {
            auto newValues = shrinkFunction(x);
            REQUIRE(newValues.size() < MAX_EXPECTED_ENTRIES);

            if (newValues.size() > worstCase.size()) {
                worstCase = std::move(newValues);
            }
        }

        current = std::move(worstCase);
        if (current.empty()) {
            return; // test passed
        }
        // std::cout << "current=" << current << '\n';
    }
    FAIL("should not be reached");
}

template <typename RealType>
void checkShrinkExplosions_Real()
{
  for(RealType x : { RealType(0.0), RealType(0.5), RealType(1.0), RealType(-1.0),
                     RealType(1.75), RealType(100.0), RealType(-100.0), RealType(NAN),
                     std::numeric_limits<RealType>::min(),
                     std::numeric_limits<RealType>::max(),
                     std::numeric_limits<RealType>::infinity(),
                     -std::numeric_limits<RealType>::infinity() })
  {
    checkShrinkExplosions<RealType>(x, [](RealType x) { return shrinkReal(x); });
  }
}

TEST_CASE("Number of elements should not explode during shrinking (float)",
          "[shrinkReal][float]")
{
    checkShrinkExplosions_Real<float>();
}

TEST_CASE("Number of elements should not explode during shrinking (double)",
          "[shrinkReal][double]")
{
    checkShrinkExplosions_Real<double>();
}

TEST_CASE("Number of elements should not explode during shrinking (long double)",
          "[shrinkReal][long double]")
{
    checkShrinkExplosions_Real<long double>();
}

template <typename IntegralType>
void checkShrinkExplosions_IntegralType()
{
    for (IntegralType x : { IntegralType(0), IntegralType(1),  IntegralType(-1),
                            IntegralType(3), IntegralType(-3),
                            IntegralType(100), IntegralType(5555), IntegralType(-5555),
                            std::numeric_limits<IntegralType>::min(),
                            std::numeric_limits<IntegralType>::max() })
    {

      checkShrinkExplosions<IntegralType>(x, [](IntegralType x)
                                          {
                                            return shrinkIntegral(x);
                                          });
    }
}

TEST_CASE("Number of elements should not explode during shrinking (char)",
          "[shrinkIntegral][char]")
{
    checkShrinkExplosions_IntegralType<char>();
}

TEST_CASE("Number of elements should not explode during shrinking (signed char)",
          "[shrinkIntegral][signed char]")
{
    checkShrinkExplosions_IntegralType<signed char>();
}

TEST_CASE("Number of elements should not explode during shrinking (unsigned char)",
          "[shrinkIntegral][unsigned char]")
{
    checkShrinkExplosions_IntegralType<unsigned char>();
}

TEST_CASE("Number of elements should not explode during shrinking (short)",
          "[shrinkIntegral][short]")
{
    checkShrinkExplosions_IntegralType<short>();
}

TEST_CASE("Number of elements should not explode during shrinking (unsigned short)",
          "[shrinkIntegral][unsigned short]")
{
    checkShrinkExplosions_IntegralType<unsigned short>();
}

TEST_CASE("Number of elements should not explode during shrinking (int)",
          "[shrinkIntegral][int]")
{
    checkShrinkExplosions_IntegralType<int>();
}

TEST_CASE("Number of elements should not explode during shrinking (unsigned int)",
          "[shrinkIntegral][unsigned int]")
{
    checkShrinkExplosions_IntegralType<unsigned int>();
}

TEST_CASE("Number of elements should not explode during shrinking (long)",
          "[shrinkIntegral][long]")
{
    checkShrinkExplosions_IntegralType<long>();
}

TEST_CASE("Number of elements should not explode during shrinking (unsigned long)",
          "[shrinkIntegral][unsigned long]")
{
    checkShrinkExplosions_IntegralType<unsigned long>();
}

TEST_CASE("Number of elements should not explode during shrinking (long long)",
          "[shrinkIntegral][long long]")
{
    checkShrinkExplosions_IntegralType<long long>();
}

TEST_CASE("Number of elements should not explode during shrinking (unsigned long long)",
          "[shrinkIntegral][unsigned long long]")
{
    checkShrinkExplosions_IntegralType<unsigned long long>();
}
