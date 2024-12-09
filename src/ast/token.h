#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_t
{
    using namespace ast::_b;
    using namespace ast::_h;

    template<typename _StreamType, typename _ValueType>
    lexeme *base_token_match(_StreamType &s, _ValueType value)
    {
        if (stream_eof(s))
            return nullptr;

        if (*s == value)
        {
            s++;
            return new lex_o<_ValueType>(value);
        }
        return nullptr;
    }

    template<typename O, O value>
    class token : public rule_base
    {
    public:
        signature_decl(token, signature_get_type<O>, signature_get_char<value>) //TODO FIXME: assuming obj == char

        match_method(s)
        {
            return base_token_match(s, value);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_range_match(_StreamType &s, _ValueType start, _ValueType end)
    {
        if (stream_eof(s))
            return nullptr;

        if (*s >= start && *s <= end)
        {
            auto value = *s;
            s++;
            return new lex_o<_ValueType>(value);
        }
        return nullptr;
    }

    template<typename O, O start, O end>
    class token_range : public rule_base
    {
    public:
        signature_decl(token_range, signature_get_type<O>, signature_get_char<start>, signature_get_char<end>) //very hacky, but it should work most of the time

        match_method(s)
        {
            return token_range_match(s, start, end);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_choice_match(_StreamType &s, const _ValueType *arr, std::size_t size)
    {
        if (stream_eof(s))
            return nullptr;

        for (size_t i = 0; i < size; i++)
        {
            if (*s == arr[i])
            {
                auto value = *s;
                s++;
                return new lex_o<_ValueType>(value);
            }
        }
        return nullptr;
    }

    template<typename O, const O *arr, int size>
    class token_choice : public rule_base
    {
    public:
        signature_decl(token_choice, signature_get_type<O>, signature_get_string<arr>, signature_get_int<size>) //FIX: assuming obj == char and arr is a string

        match_method(s)
        {
            return token_choice_match(s, arr, size);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_choice_delimited_match(_StreamType &s, const _ValueType *arr, _ValueType end)
    {
        if (stream_eof(s))
            return nullptr;

        for (size_t i = 0; arr[i] != end; i++)
        {
            if (*s == arr[i])
            {
                auto value = *s;
                s++;
                return new lex_o<_ValueType>(value);
            }
        }
        return nullptr;
    }

    template<typename O, const O *arr, O end = 0>
    class token_choice_delimited : public rule_base
    {
    public:
        signature_decl(token_choice_delimited, signature_get_type<O>, signature_get_string<arr>, signature_get_int<end>) //FIX: assuming obj == char and arr is a string

        match_method(s)
        {
            return token_choice_delimited_match(s, arr, end);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_string_match(_StreamType &s, const _ValueType *arr, std::size_t size)
    {
        if (stream_eof(s))
            return nullptr;

        auto l = new lex_v<object_S>(size);
        for (size_t i = 0; i < size; i++)
        {
            if (stream_eof(s))
            {
                delete l;
                return nullptr;
            }
            else if (*s == arr[i])
            {
                l->at(i) = *s;
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
    class token_string : public rule_base
    {
    public:
        signature_decl(token_string, signature_get_type<O>, signature_get_string<arr>, signature_get_int<size>) //FIX: assuming obj == char and arr is a string

        match_method(s)
        {
            return token_string_match(s, arr, size);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_string_delimited_match(_StreamType &s, const _ValueType *arr, _ValueType end)
    {
        if (stream_eof(s))
            return nullptr;

        auto l = new lex_v<object_S>();
        for (size_t i = 0; arr[i] != end; i++)
        {
            if (stream_eof(s))
            {
                delete l;
                return nullptr;
            }
            else if (*s == arr[i])
            {
                l->push_back(*s);
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
    class token_string_delimited : public rule_base
    {
    public:
        signature_decl(token_string_delimited, signature_get_type<O>, signature_get_string<arr>, signature_get_int<end>) //FIX: assuming obj == char and arr is a string

        match_method(s)
        {
            return token_string_delimited_match(s, arr, end);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_delimited_match(_StreamType &s, _ValueType start, _ValueType escape, _ValueType end)
    {
        if (stream_eof(s) || *s != start)
            return nullptr;
        s++;

        auto l = new lex_v<object_S>();
        while (!stream_eof(s) && *s != end)
        {
            if (*s == escape)
            {
                s++;
                if (stream_eof(s))
                {
                    delete l;
                    return nullptr;
                }
                if (*s != escape && *s != end)
                {
                    l->push_back(escape);
                }
            }
            l->push_back(*s);
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
    class token_delimited : public rule_base
    {
    public:
        signature_decl(token_delimited, signature_get_type<O>, signature_get_int<start>, signature_get_int<escape>, signature_get_int<end>)

        match_method(s)
        {
            return token_delimited_match(s, start, escape, end);
        }
    };
}