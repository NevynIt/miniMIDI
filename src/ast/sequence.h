#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_s
{
    using namespace ast::_b;
    using namespace ast::_h;

    template<typename _StreamType, typename T0, typename ...types>
    lexeme *seq_match(_StreamType &s, int _trace_, lex_V *l)
    {
        lexeme *ll = T0::match(s, _trace_ ? _trace_ +  1 : 0);
        if (!ll)
        {
            return nullptr;
        }
        l->push_back(ll);
        if constexpr (sizeof...(types) == 0)
        {
            return l;
        }
        else
        {
            return seq_match<_StreamType, types...>(s, _trace_, l);
        }
    }

    template<typename ...types>
    ast_internal_rule(seq)
    {
    public:
        ast_set_signature<ast_str("seq"), signing::get_from<types...>{}>;

        ast_primary_implementation(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll = seq_match<_StreamType, types...>(s, _trace_, l);
            if (!ll)
            {
                ast::_b::stream_restore(s, sshot);
                delete l;
                return nullptr;
            }
            return l;
        }
    };


    // template<typename T0, typename T1>
    // ast_internal_rule(seq2)
    // {
    // public:
    //     ast_set_signature<ast_str("seq2"), ast_sig(T0), ast_sig(T1)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2>
    // ast_internal_rule(seq3)
    // {
    // public:
    //     ast_set_signature<ast_str("seq3"), ast_sig(T0), ast_sig(T1), ast_sig(T2)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3>
    // ast_internal_rule(seq4)
    // {
    // public:
    //     ast_set_signature<ast_str("seq4"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4>
    // ast_internal_rule(seq5)
    // {
    // public:
    //     ast_set_signature<ast_str("seq5"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3), ast_sig(T4)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    // ast_internal_rule(seq6)
    // {
    // public:
    //     ast_set_signature<ast_str("seq6"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3), ast_sig(T4), ast_sig(T5)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    // ast_internal_rule(seq7)
    // {
    // public:
    //     ast_set_signature<ast_str("seq7"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3), ast_sig(T4), ast_sig(T5), ast_sig(T6)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    // ast_internal_rule(seq8)
    // {
    // public:
    //     ast_set_signature<ast_str("seq8"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3), ast_sig(T4), ast_sig(T5), ast_sig(T6), ast_sig(T7)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T7, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    // ast_internal_rule(seq9)
    // {
    // public:
    //     ast_set_signature<ast_str("seq9"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3), ast_sig(T4), ast_sig(T5), ast_sig(T6), ast_sig(T7), ast_sig(T8)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T7, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T8, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    // ast_internal_rule(seq10)
    // {
    // public:
    //     ast_set_signature<ast_str("seq10"), ast_sig(T0), ast_sig(T1), ast_sig(T2), ast_sig(T3), ast_sig(T4), ast_sig(T5), ast_sig(T6), ast_sig(T7), ast_sig(T8), ast_sig(T9)>;

    //     ast_primary_implementation(s)
    //     {
    //         auto l = new lex_V();
    //         void *sshot = ast::_b::stream_snapshot(s);

    //         lexeme *ll;
    //         ll = sub_match(T0, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T1, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T2, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T3, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T4, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T5, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T6, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T7, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T8, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);
    //         ll = sub_match(T9, s); if (!ll) { ast::_b::stream_restore(s, sshot); delete l; return nullptr; } l->push_back(ll);

    //         return l;
    //     };

    // };

}