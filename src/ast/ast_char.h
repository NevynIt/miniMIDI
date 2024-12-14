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
    template<auto arr>
    using token_choice = ast::_t::token_choice<obj, arr>;
    template<auto arr>
    using token_string = ast::_t::token_string<obj, arr>;
    template<obj start, obj escape = start, obj end = start>
    using token_delimited = ast::_t::token_delimited<obj, start, escape, end>;
    // template<typename T0>
    // using concat = ast::_s::concat<T0, obj>;

    lexeme *tolong_decorator(lexeme *l)
    {
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
    ast_internal_rule(tolong)
    {
    public:
        set_signature<ast_str("tolong"), sig_of(T0)>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            return tolong_decorator(l);
        }

    };

    lexeme *tofloat_decorator(lexeme *l)
    {
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
        return new lex_f(number);
    }

    template<typename T0>
    ast_internal_rule(tofloat)
    {
    public:
        set_signature<ast_str("tofloat"), sig_of(T0)>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            return tofloat_decorator(l);
        }
    };

    lexeme *tostring_decorator(lexeme *l)
    {
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
    ast_internal_rule(tostring)
    {
    public:
        set_signature<ast_str("tostring"), sig_of(T0)>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            return tostring_decorator(l);
        }
    };

    lexeme *stdEscape_decorator(lexeme *l)
    {
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
    ast_internal_rule(stdEscape)
    { //standard escape sequences, including \n, \t, \r, \0, \\ and \xHH
    public:
        set_signature<ast_str("stdEscape"), sig_of(T0)>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            return stdEscape_decorator(l);
        }
    };
    //useful tokens and rules for char streams

    ast_internal_alias(digit) = token_range<'0', '9'>; ast_alias_end;
    ast_internal_regex(whitespace, "[ \\t\\n\\r]+");
    ast_internal_regex(alpha, "[a-zA-Z]");
    ast_internal_regex(identifier, "[a-zA-Z_][a-zA-Z0-9_]*");
    ast_internal_regex(integer, "[+-]?[0-9]+");
    ast_internal_regex(fractional, "[+-]?[0-9]+(\\.[0-9]+)?");

    ast_internal_alias(str2long) =  tolong<integer>; ast_alias_end;
    ast_internal_alias(str2float) = tofloat<fractional>; ast_alias_end;
    
    ast_internal_alias(dblQuote_str) = tostring<stdEscape<token_delimited<'"', '"', '\\'>>>; ast_alias_end;
    ast_internal_alias(sglQuote_str) = tostring<stdEscape<token_delimited<'\'' , '\'', '\\'>>>; ast_alias_end;
}