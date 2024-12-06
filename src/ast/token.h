#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_t
{
    template<typename _StreamType, typename _ValueType>
    lexeme_S *base_token_match(_StreamType &s, _ValueType value)
    {
        if (ast::_h::stream_eof(s))
            return nullptr;

        if (*s == value)
        {
            lexeme_S *l = new lexeme_S();
            l->type = 'o';
            l->o = value;
            s++;
            return l;
        }
        return nullptr;
    }

    template<typename O, O value>
    class token
    {
    public:
        match_method(s)
        {
            return base_token_match(s, value);
        }
    };

    template<typename O, O start, O end>
    class token_range
    {
    public:
        match_method(s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            if (*s >= start && *s <= end)
            {
                lexeme_S *l = new lexeme_S();
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
        match_method(s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            lexeme_S *l = new lexeme_S();
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
        match_method(s)
        {
            if (ast::_h::stream_eof(s))
                return nullptr;

            lexeme_S *l = new lexeme_S();
            l->type = 'v';
            l->v = l->new_v(size);
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
        match_method(s)
        {
            if (ast::_h::stream_eof(s) || *s != delimiter)
                return nullptr;
            s++;

            lexeme_S *l = new lexeme_S();
            l->type = 'v';
            l->v = l->new_v();
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