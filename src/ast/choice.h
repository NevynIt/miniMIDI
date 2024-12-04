#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

//put back choice_fn<typename T0::object>
namespace ast::_c
{
    template<typename T0, typename T1>
    class choice//2
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
            if (l = T0::match(s)) return l;
            if (l = T1::match(s)) return l;
            return nullptr;
        }
    };

    template<typename T0, typename T1>
    class choicei//2
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
            if (l = T0::match(s)) return new ast::_b::lexeme<object>(l, '0');
            if (l = T1::match(s)) return new ast::_b::lexeme<object>(l, '1');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2>
    class choice3
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
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
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        
        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
            if (l = T0::match(s)) return new ast::_b::lexeme<object>(l, '0');
            if (l = T1::match(s)) return new ast::_b::lexeme<object>(l, '1');
            if (l = T2::match(s)) return new ast::_b::lexeme<object>(l, '2');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3>
    class choice4
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
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
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
            if (l = T0::match(s)) return new ast::_b::lexeme<object>(l, '0');
            if (l = T1::match(s)) return new ast::_b::lexeme<object>(l, '1');
            if (l = T2::match(s)) return new ast::_b::lexeme<object>(l, '2');
            if (l = T3::match(s)) return new ast::_b::lexeme<object>(l, '3');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class choice5
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
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
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
            if (l = T0::match(s)) return new ast::_b::lexeme<object>(l, '0');
            if (l = T1::match(s)) return new ast::_b::lexeme<object>(l, '1');
            if (l = T2::match(s)) return new ast::_b::lexeme<object>(l, '2');
            if (l = T3::match(s)) return new ast::_b::lexeme<object>(l, '3');
            if (l = T4::match(s)) return new ast::_b::lexeme<object>(l, '4');
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class choice6
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T5::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
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
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T5::object>::value, "choice types must have the same object type");

        template<typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l;
            if (l = T0::match(s)) return new ast::_b::lexeme<object>(l, '0');
            if (l = T1::match(s)) return new ast::_b::lexeme<object>(l, '1');
            if (l = T2::match(s)) return new ast::_b::lexeme<object>(l, '2');
            if (l = T3::match(s)) return new ast::_b::lexeme<object>(l, '3');
            if (l = T4::match(s)) return new ast::_b::lexeme<object>(l, '4');
            if (l = T5::match(s)) return new ast::_b::lexeme<object>(l, '5');
            return nullptr;
        }
    };

}