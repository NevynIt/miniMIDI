#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_t
{
    template<typename O, O value>
    class token
    {
    public:
        using object = O;

        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            if (*s == value)
            {
                ast::_b::lexeme<O> *l = new ast::_b::lexeme<O>();
                l->type = 'o';
                l->o = value;
                s++;
                return l;
            }
            return nullptr;
        }
    };

    template<typename O, O start, O end>
    class token_range
    {
    public:
        using object = O;

        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            if (*s >= start && *s <= end)
            {
                ast::_b::lexeme<O> *l = new ast::_b::lexeme<O>();
                l->type = 'o';
                l->o = *s;
                s++;
                return l;
            }
            return nullptr;
        }
    };

    template<typename O, const O *arr, std::size_t size>
    class token_choice
    {
    public:
        using object = O;

        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            ast::_b::lexeme<O> *l = new ast::_b::lexeme<O>();
            for (size_t i = 0; i < size; i++)
            {
                if (*s == arr[i])
                {
                    l->type = 'o';
                    l->o = *s;
                    s++;
                    return l;
                }
            }
            delete l;
            return nullptr;
        }
    };

    template<typename O, const O *arr, std::size_t size>
    class token_string
    {
    public:
        using object = O;

        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            ast::_b::lexeme<O> *l = new ast::_b::lexeme<O>();
            l->type = 'v';
            l->v = new std::vector<O>(size);
            for (size_t i = 0; arr[i] != 0; i++)
            {
                if (ast::_h::stream_eof(s))
                {
                    delete l;
                    return nullptr;
                }
                else if (*s == arr[i])
                {
                    l->v->at(i) = *s;
                    s++;
                }
                else
                {
                    delete l;
                    return nullptr;
                }
            }
            return l;
        }
    };

    //make the escape equal to the delimiter to disable it. escape skips the delimiter only, otherwise is passed as literal, so that Fx can handle it
    template<typename O, O delimiter, O escape = delimiter>
    class token_delimited
    {
    public:
        using object = O;

        template <typename S>
        static inline ast::_b::lexeme<O> *match(S &s)
        {
            if (ast::_h::stream_eof(s) || *s != delimiter)
                return nullptr;
            s++;

            ast::_b::lexeme<O> *l = new ast::_b::lexeme<O>();
            l->type = 'v';
            l->v = new std::vector<O>();
            while (!ast::_h::stream_eof(s) && *s != delimiter)
            {
                if (*s == escape)
                {
                    s++;
                    if (ast::_h::stream_eof(s))
                    {
                        delete l;
                        return nullptr;
                    }
                    if (*s != delimiter)
                    {
                        l->v->push_back(escape);
                    }
                }
                l->v->push_back(*s);
                s++;
            }
            if (*s == delimiter)
            {
                s++;
                return l;
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };
}