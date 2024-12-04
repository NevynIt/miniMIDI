#pragma once

#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

//basic decorators for lexemes

namespace ast::_d
{
    template<typename T0, typename F0>
    class dec//orator
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = T0::match(s);
            if (l) l = F0::refine(l);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1>
    class dec2
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = T0::match(s);
            if (l) l = F0::refine(l);
            if (l) l = F1::refine(l);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2>
    class dec3
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = T0::match(s);
            if (l) l = F0::refine(l);
            if (l) l = F1::refine(l);
            if (l) l = F2::refine(l);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2, typename F3>
    class dec4
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = T0::match(s);
            if (l) l = F0::refine(l);
            if (l) l = F1::refine(l);
            if (l) l = F2::refine(l);
            if (l) l = F3::refine(l);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2, typename F3, typename F4>
    class dec5
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = T0::match(s);
            if (l) l = F0::refine(l);
            if (l) l = F1::refine(l);
            if (l) l = F2::refine(l);
            if (l) l = F3::refine(l);
            if (l) l = F4::refine(l);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2, typename F3, typename F4, typename F5>
    class dec6
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = T0::match(s);
            if (l) l = F0::refine(l);
            if (l) l = F1::refine(l);
            if (l) l = F2::refine(l);
            if (l) l = F3::refine(l);
            if (l) l = F4::refine(l);
            if (l) l = F5::refine(l);
            return l;
        }
    };
}