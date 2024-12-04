#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_s
{
    template<typename T0, typename T1>
    class seq//uence2
    {
    public:
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");

        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = new ast::_b::lexeme<object>('V');
            S s_backup = s;

            if (!l->append(T0::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T1::match(s))) { s = s_backup; delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2>
    class seq3
    {
    public:
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");

        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = new ast::_b::lexeme<object>('V');
            S s_backup = s;

            if (!l->append(T0::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T1::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T2::match(s))) { s = s_backup; delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3>
    class seq4
    {
    public:
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");

        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = new ast::_b::lexeme<object>('V');
            S s_backup = s;

            if (!l->append(T0::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T1::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T2::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T3::match(s))) { s = s_backup; delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class seq5
    {
    public:
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value, "choice types must have the same object type");

        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = new ast::_b::lexeme<object>('V');
            S s_backup = s;

            if (!l->append(T0::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T1::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T2::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T3::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T4::match(s))) { s = s_backup; delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class seq6
    {
    public:
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T5::object>::value, "choice types must have the same object type");

        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = new ast::_b::lexeme<object>('V');
            S s_backup = s;

            if (!l->append(T0::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T1::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T2::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T3::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T4::match(s))) { s = s_backup; delete l; return nullptr; }
            if (!l->append(T5::match(s))) { s = s_backup; delete l; return nullptr; }

            return l;
        };

    };

}