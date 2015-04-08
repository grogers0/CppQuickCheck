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

#ifndef CPPQC_PROPERTY_H
#define CPPQC_PROPERTY_H

#include "Generator.h"
#include "PrettyPrint.h"

namespace cppqc {

namespace detail {
    struct null_type {};
}

class PropertyBase
{
    public:
        // By default, it is expected that every check passes.
        virtual bool expect() const { return true; }

        // Should be overwriten by subclasses, as the default
        // implementation is compiler dependent.
        // (However, in practice, gcc and clang produce useful defaults.)
        virtual std::string name() const { return typeid(*this).name(); }

        virtual ~PropertyBase() {}
};

template<class T0, class T1 = detail::null_type, class T2 = detail::null_type,
    class T3 = detail::null_type, class T4 = detail::null_type>
class Property : public PropertyBase
{
    public:
        typedef std::tuple<T0, T1, T2, T3, T4> Input;

        Property(const Generator<T0> &g0 = Arbitrary<T0>(),
                const Generator<T1> &g1 = Arbitrary<T1>(),
                const Generator<T2> &g2 = Arbitrary<T2>(),
                const Generator<T3> &g3 = Arbitrary<T3>(),
                const Generator<T4> &g4 = Arbitrary<T4>()) :
            m_gen(tupleOf(g0, g1, g2, g3, g4))
        {
        }

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return m_gen.unGen(rng, size);
        }
        std::vector<Input> shrinkInput(const Input &in) const
        {
            return m_gen.shrink(in);
        }
        bool trivialInput(const Input &in) const
        {
            return trivial(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in), std::get<3>(in), std::get<4>(in));
        }
        std::string classifyInput(const Input &in) const
        {
            return classify(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in), std::get<3>(in), std::get<4>(in));
        }
        bool checkInput(const Input &in) const
        {
            return check(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in), std::get<3>(in), std::get<4>(in));
        }

    private:
        virtual bool trivial(const T0 &, const T1 &, const T2 &, const T3 &,
                const T4 &) const
        {
            return false;
        }

        virtual std::string classify(const T0 &, const T1 &, const T2 &,
                const T3 &, const T4 &) const
        {
            return "";
        }

        virtual bool check(const T0 &, const T1 &, const T2 &,
                const T3 &, const T4 &) const = 0;

    private:
        const Generator<Input> m_gen;
};

template<class T0>
class Property<T0, detail::null_type, detail::null_type, detail::null_type,
      detail::null_type> : public PropertyBase
{
    public:
        typedef std::tuple<T0> Input;

        Property(const Generator<T0> &g0 = Arbitrary<T0>()) :
            m_gen(tupleOf(g0))
        {
        }

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return m_gen.unGen(rng, size);
        }
        std::vector<Input> shrinkInput(const Input &in) const
        {
            return m_gen.shrink(in);
        }
        bool trivialInput(const Input &in) const
        {
            return trivial(std::get<0>(in));
        }
        std::string classifyInput(const Input &in) const
        {
            return classify(std::get<0>(in));
        }
        bool checkInput(const Input &in) const
        {
            return check(std::get<0>(in));
        }

    private:
        virtual bool trivial(const T0 &) const
        {
            return false;
        }

        virtual std::string classify(const T0 &) const
        {
            return "";
        }

        virtual bool check(const T0 &) const = 0;

    private:
        const Generator<Input> m_gen;
};

template<class T0, class T1>
class Property<T0, T1, detail::null_type, detail::null_type,
      detail::null_type> : public PropertyBase
{
    public:
        typedef std::tuple<T0, T1> Input;

        Property(const Generator<T0> &g0 = Arbitrary<T0>(),
                const Generator<T1> &g1 = Arbitrary<T1>()) :
            m_gen(tupleOf(g0, g1))
        {
        }

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return m_gen.unGen(rng, size);
        }
        std::vector<Input> shrinkInput(const Input &in) const
        {
            return m_gen.shrink(in);
        }
        bool trivialInput(const Input &in) const
        {
            return trivial(std::get<0>(in), std::get<1>(in));
        }
        std::string classifyInput(const Input &in) const
        {
            return classify(std::get<0>(in), std::get<1>(in));
        }
        bool checkInput(const Input &in) const
        {
            return check(std::get<0>(in), std::get<1>(in));
        }

    private:
        virtual bool trivial(const T0 &, const T1 &) const
        {
            return false;
        }

        virtual std::string classify(const T0 &, const T1 &) const
        {
            return "";
        }

        virtual bool check(const T0 &, const T1 &) const = 0;

    private:
        const Generator<Input> m_gen;
};

template<class T0, class T1, class T2>
class Property<T0, T1, T2, detail::null_type, detail::null_type> :
    public PropertyBase
{
    public:
        typedef std::tuple<T0, T1, T2> Input;

        Property(const Generator<T0> &g0 = Arbitrary<T0>(),
                const Generator<T1> &g1 = Arbitrary<T1>(),
                const Generator<T2> &g2 = Arbitrary<T2>()) :
            m_gen(tupleOf(g0, g1, g2))
        {
        }

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return m_gen.unGen(rng, size);
        }
        std::vector<Input> shrinkInput(const Input &in) const
        {
            return m_gen.shrink(in);
        }
        bool trivialInput(const Input &in) const
        {
            return trivial(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in));
        }
        std::string classifyInput(const Input &in) const
        {
            return classify(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in));
        }
        bool checkInput(const Input &in) const
        {
            return check(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in));
        }

    private:
        virtual bool trivial(const T0 &, const T1 &, const T2 &) const
        {
            return false;
        }

        virtual std::string classify(const T0 &, const T1 &, const T2 &) const
        {
            return "";
        }

        virtual bool check(const T0 &, const T1 &, const T2 &) const = 0;

    private:
        const Generator<Input> m_gen;
};

template<class T0, class T1, class T2, class T3>
class Property<T0, T1, T2, T3, detail::null_type> : public PropertyBase
{
    public:
        typedef std::tuple<T0, T1, T2, T3> Input;

        Property(const Generator<T0> &g0 = Arbitrary<T0>(),
                const Generator<T1> &g1 = Arbitrary<T1>(),
                const Generator<T2> &g2 = Arbitrary<T2>(),
                const Generator<T3> &g3 = Arbitrary<T3>()) :
            m_gen(tupleOf(g0, g1, g2, g3))
        {
        }

        Input generateInput(RngEngine &rng, std::size_t size) const
        {
            return m_gen.unGen(rng, size);
        }
        std::vector<Input> shrinkInput(const Input &in) const
        {
            return m_gen.shrink(in);
        }
        bool trivialInput(const Input &in) const
        {
            return trivial(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in), std::get<3>(in));
        }
        std::string classifyInput(const Input &in) const
        {
            return classify(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in), std::get<3>(in));
        }
        bool checkInput(const Input &in) const
        {
            return check(std::get<0>(in), std::get<1>(in),
                    std::get<2>(in), std::get<3>(in));
        }

    private:
        virtual bool trivial(const T0 &, const T1 &, const T2 &,
                const T3 &) const
        {
            return false;
        }

        virtual std::string classify(const T0 &, const T1 &, const T2 &,
                const T3 &) const
        {
            return "";
        }

        virtual bool check(const T0 &, const T1 &, const T2 &,
                const T3 &) const = 0;

    private:
        const Generator<Input> m_gen;
};


// named
// expectFailure
// classifyWith
// trivializeWith

    template<class T0>
    std::ostream &printInput(std::ostream &out, const std::tuple<T0> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1>
    std::ostream &printInput(std::ostream &out, const std::tuple<T0, T1> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3, T4> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << "  4: " << prettyPrint(std::get<4>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4, class T5>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3, T4, T5> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << "  4: " << prettyPrint(std::get<4>(in)) << '\n'
                   << "  5: " << prettyPrint(std::get<5>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3, T4, T5, T6> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << "  4: " << prettyPrint(std::get<4>(in)) << '\n'
                   << "  5: " << prettyPrint(std::get<5>(in)) << '\n'
                   << "  6: " << prettyPrint(std::get<6>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3, T4, T5, T6, T7> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << "  4: " << prettyPrint(std::get<4>(in)) << '\n'
                   << "  5: " << prettyPrint(std::get<5>(in)) << '\n'
                   << "  6: " << prettyPrint(std::get<6>(in)) << '\n'
                   << "  7: " << prettyPrint(std::get<7>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << "  4: " << prettyPrint(std::get<4>(in)) << '\n'
                   << "  5: " << prettyPrint(std::get<5>(in)) << '\n'
                   << "  6: " << prettyPrint(std::get<6>(in)) << '\n'
                   << "  7: " << prettyPrint(std::get<7>(in)) << '\n'
                   << "  8: " << prettyPrint(std::get<8>(in)) << '\n'
                   << std::flush;
    }

    template<class T0, class T1, class T2, class T3, class T4,
        class T5, class T6, class T7, class T8, class T9>
    std::ostream &printInput(std::ostream &out,
            const std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> &in)
    {
        return out << "  0: " << prettyPrint(std::get<0>(in)) << '\n'
                   << "  1: " << prettyPrint(std::get<1>(in)) << '\n'
                   << "  2: " << prettyPrint(std::get<2>(in)) << '\n'
                   << "  3: " << prettyPrint(std::get<3>(in)) << '\n'
                   << "  4: " << prettyPrint(std::get<4>(in)) << '\n'
                   << "  5: " << prettyPrint(std::get<5>(in)) << '\n'
                   << "  6: " << prettyPrint(std::get<6>(in)) << '\n'
                   << "  7: " << prettyPrint(std::get<7>(in)) << '\n'
                   << "  8: " << prettyPrint(std::get<8>(in)) << '\n'
                   << "  9: " << prettyPrint(std::get<9>(in)) << '\n'
                   << std::flush;
    }

}

#endif
