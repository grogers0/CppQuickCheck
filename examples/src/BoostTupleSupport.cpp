/*
 * Copyright (c) 2015, Gregory Rogers All rights reserved.
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

// It demonstrates how to add support for boost::tuple by
// writing a custom generator (using std::tuple).
//
// Note this file uses C++14 features.

#include "cppqc.h"

#include <boost/tuple/tuple.hpp>
#include "boost/tuple/tuple_comparison.hpp"
#include <boost/tuple/tuple_io.hpp>
#include <utility>

using namespace cppqc;

namespace {

    template <typename StdTuple, std::size_t... Is>
    auto asBoostTuple(StdTuple &&stdTuple, std::index_sequence<Is...>) {
        return boost::tuple<std::tuple_element_t<Is, std::decay_t<StdTuple>>...>
                            (std::get<Is>(std::forward<StdTuple>(stdTuple))...);
    }

    template <typename BoostTuple, std::size_t... Is>
    auto asStdTuple(BoostTuple &&boostTuple, std::index_sequence<Is...>) {
        return std::tuple<typename boost::tuples::element<Is, std::decay_t<BoostTuple>>::type...>
                         (boost::get<Is>(std::forward<BoostTuple>(boostTuple))...);
    }

    template <typename StdTuple>
    auto asBoostTuple(StdTuple &&stdTuple) {
        return asBoostTuple(std::forward<StdTuple>(stdTuple),
                 std::make_index_sequence<std::tuple_size<std::decay_t<StdTuple>>::value>());
    }

    template <typename BoostTuple>
    auto asStdTuple(BoostTuple&& boostTuple) {
        return asStdTuple(std::forward<BoostTuple>(boostTuple),
                 std::make_index_sequence<boost::tuples::length<std::decay_t<BoostTuple>>::value>());
    }

    template<typename... T>
    struct BoostTupleGenerator
    {
        BoostTupleGenerator(const Generator<T> &...g) :
            m_tupleGenerator(tupleOf(g...))
        {
        }

        boost::tuple<T...> unGen(RngEngine &rng, std::size_t size) const
        {
            return asBoostTuple(m_tupleGenerator.unGen(rng, size));
        }

        std::vector<boost::tuple<T...>> shrink(boost::tuple<T...> shrinkInput) const
        {
            std::vector<boost::tuple<T...>> result;
            for (const auto &shrink : m_tupleGenerator.shrink(asStdTuple(shrinkInput))) {
                result.push_back(asBoostTuple(shrink));
            }
            return result;
        }

    private:

        Generator<std::tuple<T...>> m_tupleGenerator;
    };

}

template<typename... T>
Generator<boost::tuple<T...>> boostTupleOf(const Generator<T> &...g)
{
    return BoostTupleGenerator<T...>(g...);
}

template<typename... T>
Generator<boost::tuple<T...>> boostTupleOf()
{
    return BoostTupleGenerator<T...>(Arbitrary<T>()...);
}


struct AntisymmetricRelationProp : Property<boost::tuple<int, std::string, int>,
                                            boost::tuple<int, std::string, int>>
{
    AntisymmetricRelationProp() : Property(
        boostTupleOf<int, std::string, int>(),
        boostTupleOf<int, std::string, int>())
    {}

    bool check(const boost::tuple<int, std::string, int> &v1,
               const boost::tuple<int, std::string, int> &v2) const override
    {
        return (v1 <= v2 && v1 >= v2) == (v1 == v2);
    }

    std::string name() const override
    {
        return "Comparison must be antisymmetric";
    }
};

int main()
{
    cppqc::quickCheckOutput(AntisymmetricRelationProp());
}
