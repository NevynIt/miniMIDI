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
    ast_internal_rule(token)
    {
    public:
        set_signature<ast_str("token"), uti::sig_of<O>(), value>();

        ast_primary_implementation(s)
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
    ast_internal_rule(token_range)
    {
    public:
        set_signature<ast_str("token_range"), uti::sig_of<O>(), start, end>();

        ast_primary_implementation(s)
        {
            return token_range_match(s, start, end);
        }
    };

    template<typename _StreamType, typename _ValueType, size_t size>
    lexeme *token_choice_match(_StreamType &s, const std::array<_ValueType, size> arr)
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

    template<typename O, auto arr>
    ast_internal_rule(token_choice)
    {
        static_assert(is_std_array_v<decltype(arr)>, "arr must be an array");
    public:
        set_signature<ast_str("token_choice"), uti::sig_of<O>(), arr>();

        ast_primary_implementation(s)
        {
            return token_choice_match(s, arr);
        }
    };

    template<typename _StreamType, typename _ValueType, size_t size>
    lexeme *token_string_match(_StreamType &s, const std::array<_ValueType, size> arr)
    {
        if (stream_eof(s))
            return nullptr;

        auto l = new lex_v<_ValueType>();
        for (size_t i = 0; i < size; i++)
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

    template<typename O, auto arr>
    ast_internal_rule(token_string)
    {
        //assert that arr is std::array
        static_assert(is_std_array_v<decltype(arr)>, "arr must be an array");
    public:
        set_signature<ast_str("token_string"), uti::sig_of<O>(), arr>();

        ast_primary_implementation(s)
        {
            return token_string_match(s, arr);
        }
    };

    template<typename _StreamType, typename _ValueType>
    lexeme *token_delimited_match(_StreamType &s, _ValueType start, _ValueType escape, _ValueType end)
    {
        if (stream_eof(s) || *s != start)
            return nullptr;
        s++;

        auto l = new lex_v<_ValueType>();
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
    ast_internal_rule(token_delimited)
    {
    public:
        set_signature<ast_str("token_delimited"), uti::sig_of<O>(), start, escape, end>();

        ast_primary_implementation(s)
        {
            return token_delimited_match(s, start, escape, end);
        }
    };
}