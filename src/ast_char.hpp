#pragma once

#include "ast.hpp"
#include <stdlib.h>

namespace ast_char
{ //specializations for obj = const char
    using obj = char;

    using void_param = ast::void_param;
    using stream = ast::stream<const obj>;
    using lexeme = ast::lexeme<obj>;
    using noop_fn = ast::noop_fn<obj>;
    using concat_fn = ast::concat_fn<obj>;
    
    template <int n>
    using select_fn = ast::select_fn<obj, n>;
    using choice_fn = ast::choice_fn<obj>;
    using no_match = ast::no_match<obj>;

    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param, typename F4 = void_param, typename F5 = void_param, typename F6 = void_param>
    using decorator = ast::decorator<T0, F0, F1, F2, F3, F4, F5, F6>;

    template<obj value, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using token = ast::token<obj, value, F0, F1, F2, F3>;
    template<obj start, obj end, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using token_range = ast::token_range<obj, start, end, F0, F1, F2, F3>;
    template<const obj *arr, std::size_t size, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using token_choice = ast::token_choice<obj, arr, size, F0, F1, F2, F3>;
    template<const obj *arr, std::size_t size, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using token_string = ast::token_string<obj, arr, size, F0, F1, F2, F3>;
    
    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using sequence7 = ast::sequence7<T0, T1, T2, T3, T4, T5, T6, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using sequence2 = sequence7<T0, T1, void_param, void_param, void_param, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using sequence3 = sequence7<T0, T1, T2, void_param, void_param, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename T3, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using sequence4 = sequence7<T0, T1, T2, T3, void_param, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using sequence5 = sequence7<T0, T1, T2, T3, T4, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using sequence6 = sequence7<T0, T1, T2, T3, T4, T5, void_param, F0, F1, F2, F3>;
    
    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F0 = choice_fn, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using choice7 = ast::choice7<T0, T1, T2, T3, T4, T5, T6, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename F0 = choice_fn, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using choice2 = choice7<T0, T1, void_param, void_param, void_param, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename F0 = choice_fn, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using choice3 = choice7<T0, T1, T2, void_param, void_param, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename T3, typename F0 = choice_fn, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using choice4 = choice7<T0, T1, T2, T3, void_param, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename F0 = choice_fn, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using choice5 = choice7<T0, T1, T2, T3, T4, void_param, void_param, F0, F1, F2, F3>;
    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F0 = choice_fn, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using choice6 = choice7<T0, T1, T2, T3, T4, T5, void_param, F0, F1, F2, F3>;
    
    template<typename T0, int min = 0, int max = -1, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using repeat = ast::repeat<T0, min, max, F0, F1, F2, F3>;
    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using optional = repeat<T0, 0, 1, F0, F1, F2, F3>;
    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using zeroOrMore = repeat<T0, 0, -1, F0, F1, F2, F3>;
    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using oneOrMore = repeat<T0, 1, -1, F0, F1, F2, F3>;

    #define char_array_decl(_TYPE_, _NAME_) inline constexpr _TYPE_ _NAME_[]
    #define char_array(arr) (arr), (ast::getSize(arr)-1)

    using digit = token_range<'0', '9'>;
    inline constexpr obj whitespace_objs[] = " \t\n\r"; 
    using whitespace = oneOrMore<token_choice<char_array(whitespace_objs)>, concat_fn>;
    inline constexpr obj alpha_objs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    using alpha = token_choice<char_array(alpha_objs)>;
    inline constexpr obj identif_objs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    using identifier = sequence2<alpha, 
                                zeroOrMore<token_choice<char_array(alpha_objs)>, concat_fn>,
                                concat_fn>;

    class tolong_fn
    {
    public:
        static inline lexeme *refine(lexeme *l)
        {
            if (!l || l->type != 'v')
                return nullptr;
            l->v->push_back(0);
            long number = atol(l->v->data());
            l->invalidate();
            l->type = 'l';
            l->l = number;
            return l;
        }
    };

    class todouble_fn
    {
    public:
        static inline lexeme *refine(lexeme *l)
        {
            if (!l || l->type != 'v')
                return nullptr;
            l->v->push_back(0);
            double number = atof(l->v->data());
            l->invalidate();
            l->type = 'd';
            l->d = number;
            return l;
        }
    };

    using integer = sequence2<optional<choice2<token<'-'>, token<'+'>>>, oneOrMore<digit>, concat_fn>;

    using fractional = sequence3<
                            optional<choice2<token<'-'>, token<'+'>>>,
                            oneOrMore<digit>,
                            optional<sequence2<token<'.'>, oneOrMore<digit>>>,
                            concat_fn>;

    using str2long =  decorator<integer, tolong_fn>;
    using str2double = decorator<fractional, todouble_fn>;
}