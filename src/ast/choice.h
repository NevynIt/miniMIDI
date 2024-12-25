#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_c
{
    using namespace ast::_b;
    using namespace ast::_h;

    class lex_choice : public lexeme
    {
    public:
        set_signature<ast_str("lex_choice")>();
        variant_inherit(lexeme)

        lex_choice() {}
        lex_choice(const lex_choice &c) : choice(c.choice) { l = (lexeme *)c.l->clone(); }
        lex_choice(lexeme *l = nullptr, int choice = 0) : l(l), choice(choice) {}
        ~lex_choice() { if (l) delete l; }

        lexeme *l = nullptr;
        int choice = 0;
    };

    template<typename _StreamType, typename T0, typename ...types>
    lexeme *choice_match(_StreamType &s, int _trace_)
    {
        lexeme *l = T0::match(s, _trace_ ? _trace_ +  1 : 0);
        if (l)
        {
            return l;
        }
        if constexpr (sizeof...(types) == 0)
        {
            return nullptr;
        }
        else
        {
            return choice_match<_StreamType, types...>(s, _trace_);
        }
    }

    template<typename ...types>
    ast_internal_rule(choice)
    {
    public:
        set_signature<ast_str("choice"), uti::sig_of<types...>()>();

        ast_primary_implementation(s)
        {
            return choice_match<_StreamType, types...>(s, _trace_);
        }
    };

    template<typename _StreamType, typename T0, typename ...types>
    lexeme *choicei_match(_StreamType &s, int n, int _trace_)
    {
        lexeme *l = T0::match(s, _trace_ ? _trace_ +  1 : 0);
        if (l)
        {
            return new lex_choice(l, n);
        }
        if constexpr (sizeof...(types) == 0)
        {
            return nullptr;
        }
        else
        {
            return choicei_match<_StreamType, types...>(s, n+1, _trace_);
        }
    }

    template<typename ...types>
    ast_internal_rule(choicei)
    {
    public:
        set_signature<ast_str("choicei"), uti::sig_of<types...>()>();

        ast_primary_implementation(s)
        {
            return choicei_match<_StreamType, types...>(s, 0, _trace_);
        }
    };

}