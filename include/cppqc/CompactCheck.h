/*
 * Copyright (c) 2016, Vladimir Strisovsky All rights reserved.
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

#include "Test.h"
#include "Arbitrary.h"

namespace cppqc
{

template<typename Function>
struct CompactCheckFunction
{
    using type = Function;
    Function m_Function;

    CompactCheckFunction(Function&& fnc)
    : m_Function(std::move(fnc))
    {}

    CompactCheckFunction(CompactCheckFunction&&) = default;

    static const bool valid = true;

    template<typename ReturnType, typename ... T>
    ReturnType apply(ReturnType, const T& ... v) const
    {
        return m_Function(v ...);
    }
};

template<>
struct CompactCheckFunction<void>
{
    using type = void;
    static const bool valid = false;

    CompactCheckFunction() = default;
    CompactCheckFunction(CompactCheckFunction&&) = default;

    template<typename ReturnType, typename ... T>
    ReturnType apply(ReturnType r, const T& ... v) const
    {
        return r;
    }
};

template<typename CheckFunction, typename TrivialFunction, typename ClassifyFunction, typename ... T>
class CompactCheck : public Property<T ...>
{
    std::string m_name;

    template<typename _CheckFunction, typename _TrivialFunction, typename _ClassifyFunction, typename ... _T>
    friend class CompactCheck;

    using CheckFunctionT = CompactCheckFunction<CheckFunction>;
    CheckFunctionT m_checkFnc;

    using TrivialFunctionT = CompactCheckFunction<TrivialFunction>;
    TrivialFunctionT m_trivialFnc;

    using ClassifyFunctionT = CompactCheckFunction<ClassifyFunction>;
    ClassifyFunctionT m_classifyFnc;

    CompactCheck( std::string name,
                  CheckFunctionT&& checkFunction,
                  TrivialFunctionT&& trivialFunctions,
                  ClassifyFunctionT&& classifyFunction)
    : m_name(std::move(name))
    , m_checkFnc(std::move(checkFunction))
    , m_trivialFnc(std::move(trivialFunctions))
    , m_classifyFnc(std::move(classifyFunction))
    {}

    bool check(const T& ... v) const override
    {
        return m_checkFnc.apply(true, v ...);
    }

    bool trivial(const T& ... v) const override
    {
        return m_trivialFnc.apply(false, v ...);
    }

    std::string classify(const T& ... v) const override
    {
        return m_classifyFnc.apply(std::string(), v ...);
    }

    std::string name() const
    {
        return m_name.empty() ? "no-name" : m_name;
    }

public:
    CompactCheck()
    {}

    CompactCheck(const Generator<T>& ... g)
    : Property<T ...>(g ...)
    {}

    template<typename _CheckFunction>
    CompactCheck<_CheckFunction, TrivialFunction, ClassifyFunction, T ...> property(const std::string& name, _CheckFunction&& checkFnc)
    {
        static_assert(CheckFunctionT::valid == false, "Check function is already set");
        return CompactCheck<_CheckFunction, TrivialFunction, ClassifyFunction, T ...>( name,
                                                                                       std::move(checkFnc),
                                                                                       std::move(m_trivialFnc),
                                                                                       std::move(m_classifyFnc));
    }

    template<typename _TrivialFunction>
    CompactCheck<CheckFunction, _TrivialFunction, ClassifyFunction, T ...> trivial(_TrivialFunction&& trivialFnc)
    {
        static_assert(TrivialFunctionT::valid == false, "Trivial function is already set");
        return CompactCheck<CheckFunction, _TrivialFunction, ClassifyFunction, T ...>( std::move(m_name),
                                                                                       std::move(m_checkFnc),
                                                                                       std::move(trivialFnc),
                                                                                       std::move(m_classifyFnc));
    }

    template<typename _ClassifyFunction>
    CompactCheck<CheckFunction, TrivialFunction, _ClassifyFunction, T ...> classify(_ClassifyFunction&& classifyFnc)
    {
        static_assert(ClassifyFunctionT::valid == false, "Classsify function is already set");
        return CompactCheck<CheckFunction, TrivialFunction, _ClassifyFunction, T ...>( std::move(m_name),
                                                                                       std::move(m_checkFnc),
                                                                                       std::move(m_trivialFnc),
                                                                                       std::move(classifyFnc));
    }

    Result test( std::size_t maxSuccess = 100, std::size_t maxDiscarded = 0, std::size_t maxSize = 0)
    {
        return quickCheck(*this, maxSuccess, maxDiscarded, maxSize);
    }

    Result testWithOutput( std::ostream &out = std::cout,  std::size_t maxSuccess = 100, std::size_t maxDiscarded = 0, std::size_t maxSize = 0)
    {
        return quickCheckOutput(*this, out, maxSuccess, maxDiscarded, maxSize);
    }
};

template<typename ... T>
CompactCheck<void, void, void, T ...> gen()
{
    return CompactCheck<void, void, void, T ...>();
}

template<typename ... T>
CompactCheck<void, void, void, T ...> gen(const Generator<T>& ... g)
{
    return CompactCheck<void, void, void, T ...>(g ...);
}


}
