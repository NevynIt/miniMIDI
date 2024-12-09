#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_s
{
    using namespace ast::_b;
    using namespace ast::_h;

    template<typename T0, typename T1>
    class seq2 : public rule_base
    {
    public:
        signature_decl(seq2, T0, T1)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2>
    class seq3 : public rule_base
    {
    public:
        signature_decl(seq3, T0, T1, T2)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3>
    class seq4 : public rule_base
    {
    public:
        signature_decl(seq4, T0, T1, T2, T3)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    class seq5 : public rule_base
    {
    public:
        signature_decl(seq5, T0, T1, T2, T3, T4)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    class seq6 : public rule_base
    {
    public:
        signature_decl(seq6, T0, T1, T2, T3, T4, T5)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    class seq7 : public rule_base
    {
    public:
        signature_decl(seq7, T0, T1, T2, T3, T4, T5, T6)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    class seq8 : public rule_base
    {
    public:
        signature_decl(seq8, T0, T1, T2, T3, T4, T5, T6, T7)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T7, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    class seq9 : public rule_base
    {
    public:
        signature_decl(seq9, T0, T1, T2, T3, T4, T5, T6, T7, T8)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T7, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T8, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    class seq10 : public rule_base
    {
    public:
        signature_decl(seq10, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)

        match_method(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll;
            ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T7, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T8, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
            ll = sub_match(T9, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

            return l;
        };

    };

}