#pragma once

#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

//basic decorators for lexemes

namespace ast::_f
{
    template<typename O>
    class base_d
    {
    public:
        using object = O;

        //called before attempting the match
        //return false to skip the match (no_match will not be called in this case)
        //do any preparation here
        template <typename S>
        static inline bool pre_match(S &s)
        {
            return true;
        }

        //called to match the input, when the object is used as a rule, return nullptr to indicate no match
        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            return nullptr;
        }

        //called after a successful match
        //do any post processing and cleanup here
        //return the lexeme to be used in the parent rule
        static inline ast::_b::lexeme<O> *post_match(ast::_b::lexeme<O> *l)
        {
            return l;
        }

        //called after a failed match
        //do any cleanup here
        template <typename S>
        static inline void no_match(S &s)
        {
            //no op
        }
    };

    template<typename O>
    class noop_d : public base_d<O>
    {
    public:
    };

    template<typename O, int n>
    class select_d : public base_d<O>
    {
    public:
        static inline ast::_b::lexeme<O> *post_match(ast::_b::lexeme<O> *l)
        {
            if (l->type == 'V' && l->V->size() > n)
            {
                ast::_b::lexeme<O> *tmp = l->V->at(n);
                l->V->at(n) = nullptr;
                delete l;
                return tmp;
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };

    template<typename O>
    class choice_d : public base_d<O>
    {
    public:
        static inline ast::_b::lexeme<O> *post_match(ast::_b::lexeme<O> *l)
        {
            if (l->type >= '0' && l->type <= '9')
            {
                ast::_b::lexeme<O> *tmp = l->lex;
                l->lex = nullptr;
                delete l;
                return tmp;
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };

    template<typename O>
    class concat_d : public base_d<O>
    {
    public:
        static inline bool append(ast::_b::lexeme<O> *l, std::vector<O> *v)
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

        static inline ast::_b::lexeme<O> *post_match(ast::_b::lexeme<O> *l)
        {
            ast::_b::lexeme<O> *tmp = new ast::_b::lexeme<O>();
            tmp->type = 'v';
            tmp->v = new std::vector<O>();
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

    template<typename O>
    class fail_always : public base_d<O>
    {
    public:
        using object = O;
        template <typename S>
        static inline bool pre_match(S &s)
        {
            return false;
        }

        static inline ast::_b::lexeme<O> *post_match(ast::_b::lexeme<O> *l)
        {
            if (l)
                delete l;
            return nullptr;
        }

        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            return nullptr;
        }
    };

    static int indent = 0;

    template<typename T0, const char *name>
    class trace
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            printf("%*s%s {", indent*2, "", name);
            //if O happens to be char, we can print it
            if (std::is_same<object, char>::value)
            {
                printf("'%c'", *s);
            }
            printf("\n");
            indent++;
            ast::_b::lexeme<object> *l = T0::match(s);
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
    };
}