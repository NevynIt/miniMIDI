#pragma once

//helpers
#include "deps.h"

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

    // #define match_method(_varname_)\
    // template <typename _StreamType>\
    // static inline lexeme *match(_StreamType &_stream_, int _trace_ = 0, const char *_rule_name_ = nullptr) \
    // { \
    //     if (_trace_) \
    //     { \
    //         print_ind(_trace_, "");\
    //         lex_o<object_S>(*_stream_).printvalue();\
    //         printf(" (%s) %s\n", _rule_name_ ? _rule_name_ : "", signature.data());\
    //     } \
    //     _rule_name_ = signature.data(); \
    //     auto l = match_impl(_stream_, _trace_, _rule_name_, false); \
    //     if (_trace_) \
    //     { \
    //         print_ind(_trace_, "");\
    //         lex_o<object_S>(*_stream_).printvalue();\
    //         printf("(%s) %s: %s\n", _rule_name_ ? _rule_name_ : "", signature.data(), l ? "PASS" : "FAIL");\
    //     } \
    //     if (l) \
    //     { \
    //         l->rule = _rule_name_;\
    //     } \
    //     return l; \
    // } \
    // template <typename _StreamType>\
    // static inline lexeme *match_impl(_StreamType &_varname_, int _trace_ = 0, const char *_rule_name_ = nullptr, bool _internal_ = false)

    // #define internal_match_method(_varname_)\
    // template <typename _StreamType>\
    // static inline lexeme *match(_StreamType &_stream_, int _trace_ = 0, const char *_rule_name_ = nullptr) \
    // { \
    //     auto l = match_impl(_stream_, _trace_, _rule_name_, true); \
    //     return l; \
    // } \
    // template <typename _StreamType>\
    // static inline lexeme *match_impl(_StreamType &_varname_, int _trace_ = 0, const char *_rule_name_ = nullptr, bool _internal_ = true)

    // #define sub_match(_T0_, _varname_) _T0_::match(_varname_, _trace_ ? _trace_ + (_internal_ ? 0 : 1) : 0, _rule_name_)


    template<typename T>
    struct is_std_array : std::false_type {};

    template<typename T, std::size_t N>
    struct is_std_array<std::array<T, N>> : std::true_type {};

    template<typename T, std::size_t N>
    struct is_std_array<const std::array<T, N>> : std::true_type {};

    //is_std_array_v
    template<typename T>
    inline constexpr bool is_std_array_v = is_std_array<T>::value;

//-------------------------------------------------------------------------------------------------
//inside the implementation: ast_sub_match(T, _stream_)


//use:
//template <x,y,z>
//ast_define_rule(_NAME_) {
//ast_base_rule = seq2<x,y>;
//set_signature<x,y,z>;
//optionally: ast_set_internal
//---one of:
//ast_alias_implementation
//ast_decorator_implementation(l) {×××}
//aet_primary_implementation(s) {×××}
//};

//submatch calls T::match
//in alias, T::match does all
//in decorator, T::match calls l = T::match, then l = l->decorate(l) if l is not nullptr, then return l
//in primary, T::match sets up and calls match_impl

    #define ast_define_rule(_NAME_) class _NAME_ : public rule_base
    // #define ast_internal_rule(_NAME_) class _NAME_ : public rule_base
    #define ast_internal_rule(_NAME_) class _NAME_ : public rule_base_internal
    #define ast_base_rule using _ast_rule_base_
    #define ast_str(_STR_) uti::str(_STR_)
    #define ast_str_arr(_STR_) uti::str_no0(_STR_)
    #define ast_id(_RULE_) (&_RULE_::signature::print)
    
    #define ast_base_signature _ast_rule_base_::signature
    #define sub_match(_T0_, _varname_) _T0_::match(_varname_, _trace_ ? _trace_ + (_ast_internal_rule_ ? 0 : 1) : 0)

    #define ast_alias_implementation \
    variant_inherit(rule_base) \
    template <typename _StreamType>\
    static inline lexeme *match(_StreamType &_stream_, int _trace_ = 0) \
    { \
        if constexpr (_ast_internal_rule_) \
        { \
            return _ast_rule_base_::match(_stream_, _trace_); \
        } \
        else \
        { \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                print_signature_static();\
                printf(") : ");\
                _ast_rule_base_::print_signature_static();\
                printf("\n"); \
            } \
            auto l = _ast_rule_base_::match(_stream_, _trace_ ? _trace_ +  1 : 0); \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                print_signature_static();\
                printf(") : %s\n", l ? "PASS" : "FAIL");\
            } \
            if (l) \
            { \
                if constexpr (std::is_same_v<decltype(decorate(l)), lexeme *>) \
                    l = decorate(l); \
                l->rule = static_get_typeid();\
            } \
            return l; \
        } \
    } 

    #define ast_decorator_implementation(_L_) \
    variant_inherit(rule_base) \
    template <typename _StreamType>\
    static inline lexeme *match(_StreamType &_stream_, int _trace_ = 0) \
    { \
        if constexpr (_ast_internal_rule_) \
        { \
            auto l = _ast_rule_base_::match(_stream_, _trace_); \
            if (l) l = decorate(l); \
            return l; \
        } \
        else \
        { \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                print_signature_static();\
                printf(") : ");\
                _ast_rule_base_::print_signature_static();\
                printf("\n"); \
            } \
            auto l = _ast_rule_base_::match(_stream_, _trace_ ? _trace_ +  1 : 0); \
            if (l) l = decorate(l); \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                print_signature_static();\
                printf(") : %s\n", l ? "PASS" : "FAIL");\
            } \
            if (l) \
            { \
                l->rule = static_get_typeid();\
            } \
            return l; \
        } \
    } \
    static inline lexeme *decorate(lexeme *_L_)

    #define ast_primary_implementation(_S_) \
    variant_inherit(rule_base) \
    template <typename _StreamType>\
    static inline lexeme *match(_StreamType &_stream_, int _trace_ = 0) \
    { \
        if constexpr (_ast_internal_rule_) \
        { \
            return match_impl(_stream_, _trace_); \
        } \
        else \
        { \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                print_signature_static();\
                printf(") \n");\
            } \
            auto l = match_impl(_stream_, _trace_); \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                print_signature_static();\
                printf(") : %s\n", l ? "PASS" : "FAIL");\
            } \
            if (l) \
            { \
                l->rule = static_get_typeid();\
            } \
            return l; \
        } \
    } \
    template <typename _StreamType>\
    static inline lexeme *match_impl(_StreamType &_S_, int _trace_)

    #define ast_internal_alias(_NAME_) \
    ast_internal_rule(_NAME_) { \
    public: \
    set_signature<ast_str(#_NAME_)>; \
    ast_base_rule

    #define ast_alias(_NAME_) \
    ast_define_rule(_NAME_) { \
    public: \
    set_signature<ast_str(#_NAME_)>; \
    ast_base_rule

    #define ast_alias_decorator(_NAME_) \
    static lexeme *decorate(lexeme *_NAME_)

    #define ast_alias_end \
    ast_alias_implementation \
    }

    #define ast_regex_rule(_NAME_, _PATTERN_) ast_alias(_NAME_) = regex<ast_str(_PATTERN_)>; ast_alias_end;
    #define ast_internal_regex(_NAME_, _PATTERN_) ast_internal_alias(_NAME_) = regex<ast_str(_PATTERN_)>; ast_alias_end;
    #define ast_string_rule(_NAME_, _STR_) ast_alias(_NAME_) = token_string<ast_str_arr(_STR_)>; ast_alias_end;
    #define ast_internal_string(_NAME_, _STR_) ast_internal_alias(_NAME_) = token_string<ast_str_arr(_STR_)>; ast_alias_end;
}