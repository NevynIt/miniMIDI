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
        match_method(s)
        {
            lexeme_S *l = new lexeme_S('V');
            void *sshot = ast::_b::stream_snapshot(s);

            if (!l->append(T0::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T1::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1>
    using seq2 = seq<T0, T1>;

    template<typename T0, typename T1, typename T2>
    class seq3
    {
    public:
        match_method(s)
        {
            lexeme_S *l = new lexeme_S('V');
            void *sshot = ast::_b::stream_snapshot(s);

            if (!l->append(T0::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T1::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T2::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3>
    class seq4
    {
    public:
        match_method(s)
        {
            lexeme_S *l = new lexeme_S('V');
            void *sshot = ast::_b::stream_snapshot(s);

            if (!l->append(T0::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T1::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T2::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T3::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class seq5
    {
    public:
        match_method(s)
        {
            lexeme_S *l = new lexeme_S('V');
            void *sshot = ast::_b::stream_snapshot(s);

            if (!l->append(T0::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T1::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T2::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T3::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T4::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class seq6
    {
    public:
        match_method(s)
        {
            lexeme_S *l = new lexeme_S('V');
            void *sshot = ast::_b::stream_snapshot(s);

            if (!l->append(T0::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T1::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T2::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T3::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T4::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }
            if (!l->append(T5::match(s))) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; }

            return l;
        };

    };
}