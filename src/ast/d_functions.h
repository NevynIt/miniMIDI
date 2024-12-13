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

    // class dec_base
    // {
    // public:
    //     //called before attempting the match
    //     //return false to skip the match (no_match will not be called in this case)
    //     //do any preparation here
    //     pre_match_method(s)
    //     {
    //         return true;
    //     }

    //     //called to match the input, when the object is used as a rule, return nullptr to indicate no match
    //     match_method(s)
    //     {
    //         return nullptr;
    //     }

    //     //called after a successful match
    //     //do any post processing and cleanup here
    //     //return the lexeme to be used in the parent rule
    //     post_match_method(l)
    //     {
    //         return l;
    //     }

    //     //called after a failed match
    //     //do any cleanup here
    //     no_match_method(s)
    //     {
    //         //no op
    //     }
    // };

    // using noop = dec_base;

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
        ast_set_signature<ast_str("select"), ast_sig(T0), n>;
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
        ast_set_signature<ast_str("concat"), ast_sig(T0), ast_sig(O)>;
        ast_decorator_implementation(l)
        {
            return concat_decorator<O>(l);
        }
    };

    ast_internal_rule(fail_always)
    {
    public:
        ast_set_signature<ast_str("fail_always")>;
        ast_primary_implementation(s)
        {
            return nullptr;
        }
    };

    ast_internal_rule(pass_always)
    {
    public:
        ast_set_signature<ast_str("pass_always")>;
        ast_primary_implementation(s)
        {
            return new lexeme();
        }
    };

    template<typename T0>
    ast_internal_rule(trace_on)
    {
    public:
        ast_set_signature<ast_str("trace_on"), ast_sig(T0)>;

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
        ast_set_signature<ast_str("trace_off"), ast_sig(T0)>;

        ast_primary_implementation(s)
        {
            _trace_ = 0;
            return sub_match(T0, s);
        }
    };

    // static int indent = 0;

    // template<typename O> void print_object(O o) { print_object_const(o); }
    // template<typename O> void print_object_const(const O o) {}
    // template<> void print_object_const(const char o) { printf("'%c'", o); }
    // template<> void print_object_const(const char *o) { printf("\"%s\"", o); }
    // template<> void print_object_const(const int o) { printf("%d", o); }
    // template<> void print_object_const(const unsigned int o) { printf("%u", o); }
    // template<> void print_object_const(const long o) { printf("%ld", o); }
    // template<> void print_object_const(const unsigned long o) { printf("%lu", o); }
    // template<> void print_object_const(const long long o) { printf("%lld", o); }
    // template<> void print_object_const(const unsigned long long o) { printf("%llu", o); }
    // template<> void print_object_const(const float o) { printf("%f", o); }
    // template<> void print_object_const(const double o) { printf("%f", o); }

    // template<typename T0, const char *name>
    // class trace
    // {
    // public:
    //     internal_match_method(s)
    //     {
    //         printf("%*s%s {", indent*2, "", name);
    //         print_object(*s);
    //         printf("\n");
    //         indent++;
    //         lexeme_S *l = T0::match(s);
    //         indent--;
    //         if (l)
    //         {
    //             printf("%*s} PASS //%s\n", indent*2, "", name);
    //         }
    //         else
    //         {
    //             printf("%*s} FAIL //%s\n", indent*2, "", name);
    //         }   
    //         return l;
    //     }
    // };
}