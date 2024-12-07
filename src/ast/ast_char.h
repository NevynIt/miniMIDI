#pragma once

#include "ast.h"
#include <stdlib.h>

namespace ast_char
{ //specializations for obj = const char
    using namespace ast::_h;
    using namespace ast::_s;
    using namespace ast::_c;
    using namespace ast::_r;
    using namespace ast::_d;
    using namespace ast::_f;

    using obj = char;

    using stream = ast::_b::stream<obj>;
    template<obj value>
    using token = ast::_t::token<obj, value>;
    template<obj start, obj end>
    using token_range = ast::_t::token_range<obj, start, end>;
    template<const obj *arr, std::size_t size>
    using token_choice = ast::_t::token_choice<obj, arr, size>;
    template<const obj *arr, std::size_t size>
    using token_string = ast::_t::token_string<obj, arr, size>;

    //take care of the null terminator when using strings as char arrays
    #define char_array_decl(_NAME_) inline constexpr char _NAME_[]
    #define char_array(arr) (arr), (ast::_h::getSize(arr)-1)
    #define str_decl(_NAME_, _VALUE_)\
    char_array_decl(_NAME_##_str) = _VALUE_;\
    using _NAME_ = token_string<char_array(_NAME_##_str)>

    //useful decorators
    class tolong : public dec_base
    {
    public:
        post_match_method(l)
        {
            if (!l)
                return nullptr;
            else if (l->type == 'l')
                return l;
            else if (l->type == 'd')
            {
                l->l = (long)l->d;
                l->type = 'l';
                return l;
            }
            else if (l->type != 'v')
            {
                delete l;
                return nullptr;
            }
            l->v->push_back(0);
            long number = atol(l->v->data());
            l->invalidate();
            l->type = 'l';
            l->l = number;
            return l;
        }
    };

    class todouble : public dec_base
    {
    public:
        post_match_method(l)
        {
            if (!l)
                return nullptr;
            else if (l->type == 'd')
                return l;
            else if (l->type == 'l')
            {
                l->d = (double)l->l;
                l->type = 'd';
                return l;
            }
            else if (l->type != 'v')
            {
                delete l;
                return nullptr;
            }
            l->v->push_back(0);
            double number = atof(l->v->data());
            l->invalidate();
            l->type = 'd';
            l->d = number;
            return l;
        }
    };

    class tostring : public dec_base
    {
    public:
        post_match_method(l)
        {
            if (!l)
                return nullptr;
            if (l->type == 'v')
            {
                l->v->push_back(0);
                char *str = new char[l->v->size()];
                for (size_t i = 0; i < l->v->size(); i++)
                    str[i] = (char)l->v->at(i); //object type agnostic copy
                l->invalidate();
                l->type = 's';
                l->s = str;
                return l;
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };

    class stdEscape : public dec_base
    { //standard escape sequences, including \n, \t, \r, \0, \\ and \xHH
    public:
        post_match_method(l)
        {
            if (!l)
                return nullptr;
            if (l->type != 'v')
            {
                delete l;
                return nullptr;
            }
            auto *v = l->new_v();
            bool escape = false;
            for (auto c = l->v->begin(); c != l->v->end(); c++)
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
                        if (c + 2 >= l->v->end())
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
            l->invalidate();
            l->type = 'v';
            l->v = v;
            return l;
        }
    };
    //useful tokens and rules for char streams

    // ast_rule(digit, (token_range<'0', '9'>));
    using digit = token_range<'0', '9'>;

    inline constexpr obj whitespace_objs[] = " \t\n\r"; 
    using whitespace = dec<some<token_choice<char_array(whitespace_objs)>>, concat>;

    inline constexpr obj alpha_objs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    using alpha = token_choice<char_array(alpha_objs)>;

    inline constexpr obj identif_objs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    using identifier = dec2<seq<alpha,any<token_choice<char_array(identif_objs)>>>,concat, tostring>;

    using integer = dec<seq<opt<choice<token<'-'>, token<'+'>>>, some<digit>>, concat>;

    using fractional = dec<seq3<opt<choice<token<'-'>, token<'+'>>>,
                            some<digit>,
                            opt<seq<token<'.'>, some<digit>>>>,
                            concat>;

    using str2long =  dec<integer, tolong>;

    using str2double = dec<fractional, todouble>;
    
    using dblQuote_str = dec2<ast::_t::token_delimited<obj, '"', '\\'>, stdEscape, tostring>;
    using sglQuote_str = dec2<ast::_t::token_delimited<obj, '\'', '\\'>, stdEscape, tostring>;
}