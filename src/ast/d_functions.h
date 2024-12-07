#pragma once

#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "decorator.h"

//basic decorators for lexemes

namespace ast::_f
{
    class dec_base
    {
    public:
        //called before attempting the match
        //return false to skip the match (no_match will not be called in this case)
        //do any preparation here
        pre_match_method(s)
        {
            return true;
        }

        //called to match the input, when the object is used as a rule, return nullptr to indicate no match
        match_method(s)
        {
            return nullptr;
        }

        //called after a successful match
        //do any post processing and cleanup here
        //return the lexeme to be used in the parent rule
        post_match_method(l)
        {
            return l;
        }

        //called after a failed match
        //do any cleanup here
        no_match_method(s)
        {
            //no op
        }
    };

    using noop = dec_base;

    template <typename O>
    static inline ast::_b::lexeme<O> *select_post_match(ast::_b::lexeme<O> *l, int n)
    {
        if (l->type == 'V')
        {
            if (n < 0)
            {
                n = l->V->size() + n;
            }
            if (n < 0 || n >= l->V->size())
            {
                delete l;
                return nullptr;
            }
            auto tmp = l->V->at(n);
            l->V->at(n) = nullptr;
            delete l;
            return tmp;
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

    template<int n> //todo: select from the end if n is negative, and separate impl on a helper fnc
    class select : public dec_base
    {
    public:
        post_match_method(l)
        {
            return select_post_match(l, n);
        }
    };

    class concat : public dec_base
    {
    public:
        template<typename O>
        static inline bool append(ast::_b::lexeme<O> *l, typename ast::_b::lexeme<O>::v_t *v)
        {
            if (l->type == 'o')
            {
                v->push_back(l->o);
            }
            else if (l->type == 'v')
            {
                for (auto li : *l->v)
                {
                    v->push_back(li);
                }
            }
            else if (l->type == 'V')
            {
                for (auto li : *l->V)
                {
                    if (!append(li, v))
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
            return true;
        }

        post_match_method(l)
        {
            auto tmp = l->new_lexeme();
            tmp->type = 'v';
            tmp->v = tmp->new_v();
            if (append(l, tmp->v))
            {
                delete l;
                return tmp;
            }
            else
            {
                delete l;
                delete tmp;
                return nullptr;
            }
        }
    };

    class fail_always : public dec_base
    {
    public:
        pre_match_method(s)
        {
            return false;
        }

        post_match_method(l)
        {
            if (l)
                delete l;
            return nullptr;
        }

        match_method(s)
        {
            return nullptr;
        }
    };

    class pass_always : public dec_base
    {
    public:
        pre_match_method(s)
        {
            return true;
        }

        post_match_method(l)
        {
            return l;
        }

        match_method(s)
        {
            return new lexeme_S();
        }
    };

    static int indent = 0;

    template<typename O> void print_object(O o) { print_object_const(o); }
    template<typename O> void print_object_const(const O o) {}
    template<> void print_object_const(const char o) { printf("'%c'", o); }
    template<> void print_object_const(const char *o) { printf("\"%s\"", o); }
    template<> void print_object_const(const int o) { printf("%d", o); }
    template<> void print_object_const(const unsigned int o) { printf("%u", o); }
    template<> void print_object_const(const long o) { printf("%ld", o); }
    template<> void print_object_const(const unsigned long o) { printf("%lu", o); }
    template<> void print_object_const(const long long o) { printf("%lld", o); }
    template<> void print_object_const(const unsigned long long o) { printf("%llu", o); }
    template<> void print_object_const(const float o) { printf("%f", o); }
    template<> void print_object_const(const double o) { printf("%f", o); }

    template<typename _StreamType>
    lexeme_S *trace_match(_StreamType &s, const char *name)
    {
        printf("%*s%s {", indent*2, "", name);
        print_object(*s);
        printf("\n");
        indent++;
        lexeme_S *l = T0::match(s);
        indent--;
        if (l)
        {
            printf("%*s} PASS //%s\n", indent*2, "", name);
        }
        else
        {
            printf("%*s} FAIL //%s\n", indent*2, "", name);
        }   
        return l;
    }

    template<typename T0, const char *name>
    class trace
    {
    public:
        match_method(s)
        {
            return trace_match(s, name);
        }
    };
}