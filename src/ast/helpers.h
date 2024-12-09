#pragma once

//helpers
#include "deps.h"
#include <cstdarg>
#include "../uti/signature.h"

namespace ast::_h
{
//-------------------------------------------------------------------------------------------------
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
    
    template <typename S>
    typename std::enable_if<std::is_pointer_v<S>, bool>::type default_eof(S &s)
    {
        return !s || !(*s);
    }

    template <typename S>
    typename std::enable_if<std::is_pointer_v<S>, void *>::type default_snapshot(S &s)
    {
        return (void *)s;
    }

    template <typename S>
    typename std::enable_if<std::is_pointer_v<S>, bool>::type default_restore(S &s, void *snapshot)
    {
        s = (decltype(s))snapshot;
        return !s || !(*s);
    }

    optional_call_decl(stream_eof, bool, eof, default_eof)
    optional_call_decl(stream_snapshot, void *, snapshot, default_snapshot)
    optional_call_decl_1(stream_restore, bool, restore, default_restore, void *)
//-------------------------------------------------------------------------------------------------
    //helpers to work with string constants
    template <typename O, std::size_t N>
    static constexpr int getSize(const O (&arr)[N])
    {
        return N;
    }

    #define token_array_decl(_TYPE_, _NAME_) inline constexpr _TYPE_ _NAME_[]
    #define token_array(arr) (arr), ast::_h::getSize(arr)

    template<typename T> struct get_argument_type;
    template<typename T, typename U> struct get_argument_type<T(U)> { typedef U type; };
//-------------------------------------------------------------------------------------------------
    // #ifdef AST_DEBUG
    //     #define ast_rule(_name_, _rule_) \
    //     static inline constexpr char _name_##_string_constant_[] = #_name_; \
    //     using _name_ = ast::_f::trace<ast::_h::get_argument_type<void(_rule_)>::type, _name_##_string_constant_>
    // #else
    //     #define ast_rule(_name_, _rule_) using _name_ = ast::_h::get_argument_type<void(_rule_)>::type
    // #endif

    // static constexpr int hashString(const char* str)
    // {
    //     if (!*str)
    //         return 5381;
    //     int tmp = hashString(str + 1);
    //     return (((tmp & 0x7FFFF)  << 5) + tmp) ^ *str;
    // }
//-------------------------------------------------------------------------------------------------
    // //TODO: remove
    // #define lexeme_S ast::_b::lexeme<object_S>
    // //TODO: remove thse helpers
    // #define pre_match_method(_varname_)\
    // template <typename _StreamType>\
    // static inline bool pre_match(_StreamType &_varname_)

    // #define post_match_method(_varname_)\
    // template <typename _ObjectType>\
    // static inline ast::_b::lexeme<_ObjectType> *post_match(ast::_b::lexeme<_ObjectType> *_varname_)

    // #define no_match_method(_varname_)\
    // template <typename _StreamType>\
    // static inline void no_match(_StreamType &_varname_)

    // #define invalidate_lambda(_varname_)\
    // invalidate_cb = [](ast::_b::lexeme<_ObjectType> *_varname_)
    // //until here

//-------------------------------------------------------------------------------------------------
    // Helper to define alias and have recursive rules
    #define alias_declare(_NAME_) \
    class _NAME_ : public rule_base \
    { \
    public: \
        signature_noargs(_NAME_) \
        match_method(s);\
    }

    #define alias_define(_NAME_, _SRC_) \
    template <typename _StreamType>\
    inline lexeme *_NAME_::match_impl(_StreamType &_stream_, int _trace_, const char *_rule_name_) \
    { \
        _rule_name_ = signature.data(); \
        return sub_match(_SRC_, _stream_);\
    }

//-------------------------------------------------------------------------------------------------
    // Helper to print with indentation
    inline void print_ind(int indent, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        printf("%*s", indent, "");
        vprintf(fmt, args);
        va_end(args);
    }
//-------------------------------------------------------------------------------------------------
    // Helper to define match functions and enable tracing and backtracking
    #define object_S ::std::remove_reference_t<decltype(*::std::declval<_StreamType>())>

    #define match_method(_varname_)\
    template <typename _StreamType>\
    static inline lexeme *match(_StreamType &_stream_, int _trace_ = 0, const char *_rule_name_ = nullptr) \
    { \
        if (_trace_) \
        { \
            print_ind(_trace_, "(%s) %s: ", _rule_name_ ? _rule_name_ : "", signature.data());\
            lex_o<object_S>(*_stream_).printvalue();\
            printf("\n");\
        } \
        auto l = match_impl(_stream_, _trace_, _rule_name_); \
        if (_trace_) \
        { \
            print_ind(_trace_, "(%s) %s: %s\n", _rule_name_ ? _rule_name_ : "", signature.data(), l ? "PASS" : "FAIL");\
        } \
        return l; \
    } \
    template <typename _StreamType>\
    static inline lexeme *match_impl(_StreamType &_varname_, int _trace_ = 0, const char *_rule_name_ = nullptr)
    //TODO: separate match and match impl, and add tracing and adding the rule name in match
    //change the parameters to stream s, bool trace and const char* rule_name

    #define sub_match(_T0_, _varname_) _T0_::match(_varname_, _trace_ ? _trace_ + 1 : 0, _rule_name_)
    
//-------------------------------------------------------------------------------------------------
}