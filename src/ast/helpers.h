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
    _NAME_(T& obj) \
    { \
        return obj._FNC_(); \
    } \
    \
    template <typename T> \
    std::enable_if_t<!_NAME_##_has_##_FNC_##_method<T>::value, _RET_> \
    _NAME_(T& obj) \
    { \
        return (_ELSE_)(obj); \
    }

    #define optional_call_decl_1(_NAME_, _RET_, _FNC_, _ELSE_, _PARAM_) \
    template<typename T, typename = void> \
    struct _NAME_##_has_##_FNC_##_method1 : std::false_type {}; \
    \
    template<typename T> \
    struct _NAME_##_has_##_FNC_##_method1<T, std::void_t<decltype(std::declval<T>()._FNC_(std::declval<_PARAM_>()))>> : std::true_type {}; \
    \
    template <typename T> \
    std::enable_if_t<_NAME_##_has_##_FNC_##_method1<T>::value, _RET_> \
    _NAME_(T& obj, _PARAM_ p) \
    { \
        return obj._FNC_(p); \
    } \
    \
    template <typename T> \
    std::enable_if_t<!_NAME_##_has_##_FNC_##_method1<T>::value, _RET_> \
    _NAME_(T& obj, _PARAM_ p) \
    { \
        return (_ELSE_)(obj, p); \
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

    static constexpr int hashString(const char* str)
    {
        if (!*str)
            return 5381;
        int tmp = hashString(str + 1);
        return (((tmp & 0x7FFFF)  << 5) + tmp) ^ *str;
    }

    #define object_S std::remove_reference_t<decltype(*std::declval<_StreamType>())>
    #define lexeme_S ast::_b::lexeme<object_S>

    #define alias_define(_NAME_, _SRC_) \
    template <typename _StreamType> \
    lexeme_S *_NAME_::match(_StreamType &s) \
    { \
        return _SRC_::match(s); \
    }

    #define match_method(_varname_)\
    template <typename _StreamType>\
    static inline lexeme_S *match(_StreamType &_varname_)

    #define pre_match_method(_varname_)\
    template <typename _StreamType>\
    static inline bool pre_match(_StreamType &_varname_)

    #define post_match_method(_varname_)\
    template <typename _ObjectType>\
    static inline ast::_b::lexeme<_ObjectType> *post_match(ast::_b::lexeme<_ObjectType> *_varname_)

    #define no_match_method(_varname_)\
    template <typename _StreamType>\
    static inline void no_match(_StreamType &_varname_)

    #define invalidate_lambda(_varname_)\
    invalidate_cb = [](ast::_b::lexeme<_ObjectType> *_varname_)

    #define alias_declare(_NAME_) \
    class _NAME_ \
    { \
    public: \
        match_method(s);\
    }
}