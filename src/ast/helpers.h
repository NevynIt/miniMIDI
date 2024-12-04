#pragma once

//helpers
#include "deps.h"

namespace ast::_h
{
    // Helper trait to detect if T has an eof() method
    template<typename T, typename = void>
    struct has_eof_method : std::false_type {};

    template<typename T>
    struct has_eof_method<T, std::void_t<decltype(std::declval<T>().eof())>> : std::true_type {};

    // Overload when T has an eof() method
    template <typename T>
    std::enable_if_t<has_eof_method<T>::value, bool>
    call_eof(const T& obj)
    {
        return obj.eof();
    }

    // Overload when T does not have an eof() method
    template <typename T>
    std::enable_if_t<!has_eof_method<T>::value, bool>
    call_eof(const T& obj)
    {
        return *obj == T(0);
    }

    template <typename O, std::size_t N>
    static constexpr std::size_t getSize(const O (&arr)[N])
    {
        return N;
    }

    #define token_array_decl(_TYPE_, _NAME_) inline constexpr _TYPE_ _NAME_[]
    #define token_array(arr) (arr), ast::_h::getSize(arr)

    template<typename T> struct get_argument_type;
    template<typename T, typename U> struct get_argument_type<T(U)> { typedef U type; };

    #ifdef AST_DEBUG
        #define ast_rule(_name_, _rule_) \
        static inline constexpr char _name_##_string_constant_[] = #_name_; \
        using _name_ = ast::_f::trace<ast::_h::get_argument_type<void(_rule_)>::type, _name_##_string_constant_>
    #else
        #define ast_rule(_name_, _rule_) using _name_ = ast::_h::get_argument_type<void(_rule_)>::type
    #endif

}