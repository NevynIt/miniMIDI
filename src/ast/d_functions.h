#pragma once

#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "decorator.h"

//basic decorators for lexemes

namespace ast::_f
{
    using namespace ast::_b;
    using namespace ast::_h;

    static inline lexeme *select_decorator(lexeme *l, int n)
    {
        if (!l)
            return nullptr;
        lex_V *tmp = l->as<lex_V>();
        if (tmp)
        {
            if (n < 0)
            {
                n = tmp->size() + n;
            }
            if (n < 0 || n >= tmp->size())
            {
                delete l;
                return nullptr;
            }
            auto tmp2 = tmp->at(n);
            tmp->at(n) = nullptr;
            delete l;
            return tmp2;
        }
        else if (n == 0)
        {
            return l;
        }
        else
        {
            delete l;
            return nullptr;
        }
    }

    template<typename T0, int n>
    ast_internal_rule(select)
    {
    public:
        ast_base_rule = T0;
        set_signature<ast_str("select"), sig_of(T0), n>;
        ast_decorator_implementation(l)
        {
            return select_decorator(l, n);
        }
    };

    template<typename O>
    lexeme *concat_decorator(lexeme *l)
    {
        if (!l)
            return nullptr;
    
        lex_V *tmp = l->as<lex_V>();
        if (tmp)
        {
            lex_v<O> *v = new lex_v<O>();
            for (auto i = tmp->begin(); i != tmp->end(); i++)
            {
                auto tmp = (*i)->as<lex_v<O>>();
                if (tmp)
                {
                    for (auto j = tmp->begin(); j != tmp->end(); j++)
                    {
                        v->push_back(*j);
                    }
                }
            }
            delete l;
            return v;
        }
        return l;
    }

    template<typename T0, typename O>
    ast_internal_rule(concat)
    {
    public:
        ast_base_rule = T0;
        set_signature<ast_str("concat"), sig_of(T0), sig_of(O)>;
        ast_decorator_implementation(l)
        {
            return concat_decorator<O>(l);
        }
    };

    ast_internal_rule(fail_always)
    {
    public:
        set_signature<ast_str("fail_always")>;
        ast_primary_implementation(s)
        {
            return nullptr;
        }
    };

    ast_internal_rule(pass_always)
    {
    public:
        set_signature<ast_str("pass_always")>;
        ast_primary_implementation(s)
        {
            return new lexeme();
        }
    };

    template<typename T0>
    ast_internal_rule(trace_on)
    {
    public:
        set_signature<ast_str("trace_on"), sig_of(T0)>;

        ast_primary_implementation(s)
        {
            _trace_ = _trace_ ? _trace_ : 1;
            return sub_match(T0, s);
        }
    };

    template<typename T0>
    ast_internal_rule(trace_off)
    {
    public:
        set_signature<ast_str("trace_off"), sig_of(T0)>;

        ast_primary_implementation(s)
        {
            _trace_ = 0;
            return sub_match(T0, s);
        }
    };
}