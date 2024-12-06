#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_c
{
    template<typename T0, typename T1>
    class choice//2
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return l;
            if (l = T1::match(s)) return l;
            return nullptr;
        }
    };

    template<typename T0, typename T1>
    class choicei//2
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return new lexeme_S(l, '0');
            if (l = T1::match(s)) return new lexeme_S(l, '1');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2>
    class choice3
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return l;
            if (l = T1::match(s)) return l;
            if (l = T2::match(s)) return l;
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2>
    class choice3i
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return new lexeme_S(l, '0');
            if (l = T1::match(s)) return new lexeme_S(l, '1');
            if (l = T2::match(s)) return new lexeme_S(l, '2');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3>
    class choice4
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return l;
            if (l = T1::match(s)) return l;
            if (l = T2::match(s)) return l;
            if (l = T3::match(s)) return l;
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3>
    class choice4i
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return new lexeme_S(l, '0');
            if (l = T1::match(s)) return new lexeme_S(l, '1');
            if (l = T2::match(s)) return new lexeme_S(l, '2');
            if (l = T3::match(s)) return new lexeme_S(l, '3');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class choice5
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return l;
            if (l = T1::match(s)) return l;
            if (l = T2::match(s)) return l;
            if (l = T3::match(s)) return l;
            if (l = T4::match(s)) return l;
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class choice5i
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return new lexeme_S(l, '0');
            if (l = T1::match(s)) return new lexeme_S(l, '1');
            if (l = T2::match(s)) return new lexeme_S(l, '2');
            if (l = T3::match(s)) return new lexeme_S(l, '3');
            if (l = T4::match(s)) return new lexeme_S(l, '4');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class choice6
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return l;
            if (l = T1::match(s)) return l;
            if (l = T2::match(s)) return l;
            if (l = T3::match(s)) return l;
            if (l = T4::match(s)) return l;
            if (l = T5::match(s)) return l;
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class choice6i
    {
    public:
        match_method(s)
        {
            lexeme_S *l;
            if (l = T0::match(s)) return new lexeme_S(l, '0');
            if (l = T1::match(s)) return new lexeme_S(l, '1');
            if (l = T2::match(s)) return new lexeme_S(l, '2');
            if (l = T3::match(s)) return new lexeme_S(l, '3');
            if (l = T4::match(s)) return new lexeme_S(l, '4');
            if (l = T5::match(s)) return new lexeme_S(l, '5');
            return nullptr;
        }
    };

}