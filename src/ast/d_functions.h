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

    class lex_drop : public lexeme
    {
    public:
        set_signature<ast_str("drop")>;
        variant_inherit(lexeme)
    };

    template<typename T0>
    ast_internal_rule(drop)
    {
    public:
        ast_base_rule = T0;
        set_signature<ast_str("drop"), sig_of(T0)>;
        ast_decorator_implementation(l)
        {
            delete l;
            return new lex_drop();
        }
    };

    template<typename O>
    void concat_into(lex_V *V, lex_v<O> *v)
    {
        for (auto i = V->begin(); i != V->end(); i++)
        {
            auto tmp_V = (*i)->as<lex_V>();
            if (tmp_V)
            {
                concat_into<O>(tmp_V, v);
                continue;
            }

            auto *tmp = (*i)->as<lex_v<O>>();
            if (tmp)
            {
                for (auto j = tmp->begin(); j != tmp->end(); j++)
                {
                    v->push_back(*j);
                }
                continue;
            }

            auto *tmp_o = (*i)->as<lex_o<O>>();
            if (tmp_o)
            {
                v->push_back(tmp_o->o);
                continue;
            }
        }
    }

    template<typename O>
    lexeme *concat_decorator(lexeme *l)
    {
        if (!l)
            return nullptr;
    
        lex_V *tmp1 = l->as<lex_V>();
        if (tmp1)
        {
            lex_v<O> *v = new lex_v<O>();
            concat_into<O>(tmp1, v);
            delete l;
            return v;
        }
        lex_v<O> *tmp2 = l->as<lex_v<O>>();
        if (tmp2)
        {
            return tmp2;
        }
        return nullptr;
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

    typedef void (*out_func_t)(const lexeme *l);

    out_func_t cur_out_func(out_func_t f = nullptr);

    template<typename T0>
    ast_internal_rule(out)
    {
        public:
        set_signature<ast_str("out")>;
        ast_base_rule = T0;
        ast_decorator_implementation(l)
        {
            cur_out_func()(l);
            return l;
        }
    };
    
}