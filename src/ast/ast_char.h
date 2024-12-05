#pragma once

#include "ast.h"
#include <stdlib.h>

namespace ast_char
{ //specializations for obj = const char
    using obj = char;

    using base_d = ast::_f::base_d<obj>;
    using stream = ast::_b::stream<obj>;
    using stream_const = ast::_b::stream<const obj>;
    using lexeme = ast::_b::lexeme<obj>;
    using noop_d = ast::_f::noop_d<obj>;
    using concat_d = ast::_f::concat_d<obj>;
    
    template <int n>
    using select_d = ast::_f::select_d<obj, n>;
    using choice_d = ast::_f::choice_d<obj>;
    using fail_always = ast::_f::fail_always<obj>;

    template<obj value>
    using token = ast::_t::token<obj, value>;
    template<obj start, obj end>
    using token_range = ast::_t::token_range<obj, start, end>;
    template<const obj *arr, std::size_t size>
    using token_choice = ast::_t::token_choice<obj, arr, size>;
    template<const obj *arr, std::size_t size>
    using token_string = ast::_t::token_string<obj, arr, size>;

    using namespace ast::_s;
    using namespace ast::_c;
    using namespace ast::_r;
    using namespace ast::_d;

    #define char_array_decl(_TYPE_, _NAME_) inline constexpr _TYPE_ _NAME_[]
    #define char_array(arr) (arr), (ast::_h::getSize(arr)-1)

    class tolong_d : public base_d
    {
    public:
        static inline lexeme *post_match(lexeme *l)
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

    class todouble_d : public base_d
    {
    public:
        static inline lexeme *post_match(lexeme *l)
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

    class tostring_d : public base_d
    {
    public:
        static inline lexeme *post_match(lexeme *l)
        {
            if (!l)
                return nullptr;
            if (l->type == 'v')
            {
                l->v->push_back(0);
                char *str = new char[l->v->size()];
                strcpy(str, l->v->data());
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

    // ast_rule(digit, (token_range<'0', '9'>));
    using digit = token_range<'0', '9'>;

    inline constexpr obj whitespace_objs[] = " \t\n\r"; 
    using whitespace = dec<some<token_choice<char_array(whitespace_objs)>>, concat_d>;
    // ast_rule(whitespace, (dec<some<token_choice<char_array(whitespace_objs)>>, concat_d>));

    inline constexpr obj alpha_objs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    using alpha = token_choice<char_array(alpha_objs)>;
    // ast_rule(alpha, (token_choice<char_array(alpha_objs)>));

    inline constexpr obj identif_objs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    using identifier = dec2<seq<alpha,any<token_choice<char_array(identif_objs)>>>,concat_d, tostring_d>;
    // ast_rule(identifier, (dec2<seq<alpha,any<token_choice<char_array(identif_objs)>>>,concat_d, tostring_d>));

    using integer = dec<seq<opt<choice<token<'-'>, token<'+'>>>, some<digit>>, concat_d>;
    // ast_rule(integer, (dec<seq<opt<choice<token<'-'>, token<'+'>>>, some<digit>>, concat_d>));

    using fractional = dec<seq3<opt<choice<token<'-'>, token<'+'>>>,
                            some<digit>,
                            opt<seq<token<'.'>, some<digit>>>>,
                            concat_d>;
    // ast_rule(fractional, fractional_def);

    using str2long =  dec<integer, tolong_d>;

    using str2double = dec<fractional, todouble_d>;

    class stdEscape_d : public base_d
    { //standard escape sequences, including \n, \t, \r, \0, \\ and \xHH
    public:
        static inline lexeme *post_match(lexeme *l)
        {
            if (!l)
                return nullptr;
            if (l->type != 'v')
            {
                delete l;
                return nullptr;
            }
            auto *v = new std::vector<obj>();
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
    
        using dblQuote_str = dec2<ast::_t::token_delimited<obj, '"', '\\'>, stdEscape_d, tostring_d>;
        using sglQuote_str = dec2<ast::_t::token_delimited<obj, '\'', '\\'>, stdEscape_d, tostring_d>;
    };
}