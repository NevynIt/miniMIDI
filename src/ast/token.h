#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_t
{
    template<typename _StreamType, typename _ValueType>
    lexeme_S *base_token_match(_StreamType &s, _ValueType value)
    {
        if (ast::_b::stream_eof(s))
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

    template<typename _StreamType, typename _ValueType>
    lexeme_S *token_range_match(_StreamType &s, _ValueType start, _ValueType end)
    {
        if (ast::_b::stream_eof(s))
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

    template<typename O, O start, O end>
    class token_range
    {
    public:
        match_method(s)
        {
            return token_range_match(s, start, end);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme_S *token_choice_match(_StreamType &s, const _ValueType *arr, std::size_t size)
    {
        if (ast::_b::stream_eof(s))
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

    template<typename O, const O *arr, std::size_t size>
    class token_choice
    {
    public:
        match_method(s)
        {
            return token_choice_match(s, arr, size);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme_S *token_choice_delimited_match(_StreamType &s, const _ValueType *arr, _ValueType end)
    {
        if (ast::_b::stream_eof(s))
            return nullptr;

        lexeme_S *l = new lexeme_S();
        for (size_t i = 0; arr[i] != end; i++)
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

    template<typename O, const O *arr, O end = 0>
    class token_choice_delimited
    {
    public:
        match_method(s)
        {
            return token_choice_delimited_match(s, arr, end);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme_S *token_string_match(_StreamType &s, const _ValueType *arr, std::size_t size)
    {
        if (ast::_b::stream_eof(s))
            return nullptr;

        lexeme_S *l = new lexeme_S();
        l->type = 'v';
        l->v = l->new_v(size);
        for (size_t i = 0; i < size; i++)
        {
            if (ast::_b::stream_eof(s))
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

    template<typename O, const O *arr, std::size_t size>
    class token_string
    {
    public:
        match_method(s)
        {
            return token_string_match(s, arr, size);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme_S *token_string_delimited_match(_StreamType &s, const _ValueType *arr, _ValueType end)
    {
        if (ast::_b::stream_eof(s))
            return nullptr;

        lexeme_S *l = new lexeme_S();
        l->type = 'v';
        l->v = l->new_v();
        for (size_t i = 0; arr[i] != end; i++)
        {
            if (ast::_b::stream_eof(s))
            {
                delete l;
                return nullptr;
            }
            else if (*s == arr[i])
            {
                l->v->push_back(*s);
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

    template<typename O, const O *arr, O end = 0>
    class token_string_delimited
    {
    public:
        match_method(s)
        {
            return token_string_delimited_match(s, arr, end);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme_S *token_delimited_match(_StreamType &s, _ValueType start, _ValueType escape, _ValueType end)
    {
        if (ast::_b::stream_eof(s) || *s != start)
            return nullptr;
        s++;

        lexeme_S *l = new lexeme_S();
        l->type = 'v';
        l->v = l->new_v();
        while (!ast::_b::stream_eof(s) && *s != end)
        {
            if (*s == escape)
            {
                s++;
                if (ast::_b::stream_eof(s))
                {
                    delete l;
                    return nullptr;
                }
                if (*s != escape && *s != end)
                {
                    l->v->push_back(escape);
                }
            }
            l->v->push_back(*s);
            s++;
        }
        if (*s == end)
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

    //make the escape equal to the end to disable it. escape skips the end or escape delimiters only, otherwise it's passed as literal, so that other decorators can handle it
    template<typename O, O start, O escape = start, O end = start>
    class token_delimited
    {
    public:
        match_method(s)
        {
            return token_delimited_match(s, start, escape, end);
        }
    };
}