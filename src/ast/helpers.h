#pragma once

//helpers
#include "deps.h"

namespace ast::_h
{
    // Helper trait called _NAME_, which calls _FNC_ if T has a _FNC_() method, otherwise calls _ELSE_
    #define optional_call_decl(_NAME_, _RET_, _FNC_, _ELSE_) \
    template<typename T, typename = void> \
    struct _NAME_##_has_##_FNC_##_method : std::false_type {}; \
    \
    template<typename T> \
    struct _NAME_##_has_##_FNC_##_method<T, std::void_t<decltype(std::declval<T>()._FNC_())>> : std::true_type {}; \
    \
    template <typename T> \
    std::enable_if_t<_NAME_##_has_##_FNC_##_method<T>::value, _RET_> \
    _NAME_(const T& obj) \
    { \
        return obj._FNC_(); \
    } \
    \
    template <typename T> \
    std::enable_if_t<!_NAME_##_has_##_FNC_##_method<T>::value, _RET_> \
    _NAME_(const T& obj) \
    { \
        return (_ELSE_); \
    }

    optional_call_decl(stream_eof, bool, eof, *obj == T(0))
    
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