#pragma once

#include "ast.h"
#include <stdlib.h>
#include "regex.h"

namespace ast_char
{ //specializations for obj = const char
    using namespace ast::_h;
    using namespace ast::_s;
    using namespace ast::_c;
    using namespace ast::_r;
    using namespace ast::_d;
    using namespace ast::_f;
    using namespace ast::_re;

    using obj = char;

    using stream = ast::_b::stream<obj>;
    template<obj value>
    using token = ast::_t::token<obj, value>;
    template<obj start, obj end>
    using token_range = ast::_t::token_range<obj, start, end>;
    template<const obj *arr, std::size_t size>
    using token_choice = ast::_t::token_choice<obj, arr, size>;
    template<const obj *arr, obj end = 0>
    using token_choice_delimited = ast::_t::token_choice_delimited<obj, arr, end>;
    template<const obj *arr, std::size_t size>
    using token_string = ast::_t::token_string<obj, arr, size>;
    template<const obj *arr, obj end = 0>
    using token_string_delimited = ast::_t::token_string_delimited<obj, arr, end>;
    template<obj start, obj escape = start, obj end = start>
    using token_delimited = ast::_t::token_delimited<obj, start, escape, end>;

    // //take care of the null terminator when using strings as char arrays
    // #define char_array_decl(_NAME_) inline constexpr char _NAME_[]
    // #define char_array(arr) (arr), (ast::_h::getSize(arr)-1)

    lexeme *tolong_decorator(lexeme *l)
    {
        if (!l)
            return nullptr;
        if (l->is<lex_l>())
            return l;

        auto v = l->as<lex_v<char>>();
        if (!v)
        {
            delete l;
            return nullptr;
        }
        v->push_back(0);
        long number = atol(v->data());
        delete v;
        return new lex_l(number);
    }

    //useful decorators
    template<typename T0>
    class tolong : public rule_base
    {
    public:
        signature_decl(tolong, T0)

        match_method(s)
        {
            return tolong_decorator(sub_match(T0, s));
        }
    };

    lexeme *tofloat_decorator(lexeme *l)
    {
        if (!l)
            return nullptr;
        if (l->is<lex_f>())
            return l;

        auto v = l->as<lex_v<char>>();
        if (!v)
        {
            delete l;
            return nullptr;
        }
        v->push_back(0);
        float number = atof(v->data());
        delete v;
        return new lex_l(number);
    }

    template<typename T0>
    class tofloat : public rule_base
    {
    public:
        signature_decl(todouble, T0)

        match_method(s)
        {
            return tofloat_decorator(sub_match(T0, s));
        }
    };

    lexeme *tostring_decorator(lexeme *l)
    {
        if (!l)
            return nullptr;
        if (l->is<lex_s>())
            return l;
        auto v = l->as<lex_v<char>>();
        if (!v)
        {
            delete l;
            return nullptr;
        }
        v->push_back(0);
        char *str = new char[v->size()];
        for (size_t i = 0; i < v->size(); i++)
            str[i] = (char)v->at(i);
        delete v;
        return new lex_s(str);
    }

    template<typename T0>
    class tostring : public rule_base
    {
    public:
        signature_decl(tostring, T0)

        match_method(l)
        {
            return tostring_decorator(sub_match(T0, l));
        }
    };

    lexeme *stdEscape_decorator(lexeme *l)
    {
        if (!l)
            return nullptr;
        lex_v<char> *lv = l->as<lex_v<char>>();
        if (!lv)
        {
            delete l;
            return nullptr;
        }
        lex_v<char> *v = new lex_v<char>();
        bool escape = false;
        for (auto c = lv->begin(); c != lv->end(); c++)
        {
            if (escape)
            {
                switch (*c)
                {
                case 'n':
                    v->push_back('\n');
                    break;
                case 't':
                    v->push_back('\t');
                    break;
                case 'r':
                    v->push_back('\r');
                    break;
                case '0':
                    v->push_back('\0');
                    break;
                case '\\':
                    v->push_back('\\');
                    break;
                case 'x':
                {
                    if (c + 2 >= lv->end())
                    {
                        delete v;
                        delete l;
                        return nullptr;
                    }
                    char hex[3] = {*++c, *++c, 0};
                    v->push_back(strtol(hex, nullptr, 16));
                    break;
                }
                default:
                    v->push_back('\\');
                    v->push_back(*c);
                    break;
                }
                escape = false;
            }
            else
            {
                if (*c == '\\')
                {
                    escape = true;
                }
                else
                {
                    v->push_back(*c);
                }
            }
        }
        delete l;
        return v;
    }

    template<typename T0>
    class stdEscape : public rule_base
    { //standard escape sequences, including \n, \t, \r, \0, \\ and \xHH
    public:
        signature_decl(stdEscape, T0)

        match_method(s)
        {
            return stdEscape_decorator(sub_match(T0, s));
        }
    };
    //useful tokens and rules for char streams

    using digit_impl = token_range<'0', '9'>;
    alias_declare(digit);
    alias_define(digit, digit_impl);

    re_decl(whitespace, "[ \\t\\n\\r]+");
    re_decl(alpha, "[a-zA-Z]");
    re_decl(identifier, "[a-zA-Z_][a-zA-Z0-9_]*");
    re_decl(integer, "[+-]?[0-9]+");
    re_decl(fractional, "[+-]?[0-9]+(\\.[0-9]+)?");

    using str2long_impl =  tolong<integer>;
    alias_declare(str2long);
    alias_define(str2long, str2long_impl);

    using str2float_impl = tofloat<fractional>;
    alias_declare(str2float);
    alias_define(str2float, str2float_impl);
    
    using dblQuote_str_impl = tostring<stdEscape<ast::_t::token_delimited<obj, '"', '\\'>>>;
    alias_declare(dblQuote_str);
    alias_define(dblQuote_str, dblQuote_str_impl);

    using sglQuote_str_impl = tostring<stdEscape<ast::_t::token_delimited<obj, '\'', '\\'>>>;
    alias_declare(sglQuote_str);
    alias_define(sglQuote_str, sglQuote_str_impl);
}