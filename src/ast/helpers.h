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
    //helpers to work with string constants

    // #define token_array_decl(_NAME_) inline constexpr _TYPE_ _NAME_[]
    // #define token_array(arr) (arr), ast::_h::getSize(arr)

    // #define str_decl(_NAME_, _VALUE_)\
    // char_array_decl(_NAME_##_string_) = _VALUE_;\
    // using _NAME_ = token_string<char_array(_NAME_##_string_)>

//-------------------------------------------------------------------------------------------------
    // template<typename T> struct get_argument_type;
    // template<typename T, typename U> struct get_argument_type<T(U)> { typedef U type; };
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
    // #define alias_declare(_NAME_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     match_method(s);\
    // }

    // #define internal_alias_declare(_NAME_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     internal_match_method(s);\
    // }

    // #define alias_define(_NAME_, _SRC_) \
    // template <typename _StreamType>\
    // inline lexeme *_NAME_::match_impl(_StreamType &_stream_, int _trace_, const char *_rule_name_, bool _internal_) \
    // { \
    //     return l = sub_match(_SRC_, _stream_);\
    // }

    // #define rule_declare(_NAME_, _SRC_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     match_method(_stream_) \
    //     { \
    //     _rule_name_ = signature.data(); \
    //     auto l = sub_match(_SRC_, _stream_);\
    //     if (l) l->rule = _rule_name_;\
    //     return l;\
    //     } \
    // }

    // #define internal_rule_declare(_NAME_, _SRC_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     internal_match_method(_stream_) \
    //     { \
    //     return sub_match(_SRC_, _stream_);\
    //     } \
    // }


    // #define re_decl(_NAME_, _PATTERN_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     static inline constexpr char _NAME_##_pattern_[] = _PATTERN_; \
    //     match_method(_stream_)\
    //     { \
    //         return sub_match(token_regex<_NAME_##_pattern_>, _stream_);\
    //     } \
    // };

    // #define internal_re_decl(_NAME_, _PATTERN_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     static inline constexpr char _NAME_##_pattern_[] = _PATTERN_; \
    //     internal_match_method(_stream_)\
    //     { \
    //         return sub_match(token_regex<_NAME_##_pattern_>, _stream_);\
    //     } \
    // };

    // template <typename O, std::size_t N>
    // static constexpr int getSize(const O (&arr)[N])
    // {
    //     return N;
    // }

    // #define string_decl(_NAME_, _ARR_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     static inline constexpr auto _NAME_##_array_ = _ARR_; \
    //     match_method(_stream_)\
    //     { \
    //         using T0 = token_string<_NAME_##_array_, getSize(_ARR_)-1>;\
    //         return sub_match(T0, _stream_);\
    //     } \
    // };

    // #define internal_string_decl(_NAME_, _ARR_) \
    // class _NAME_ : public rule_base \
    // { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     static inline constexpr char _NAME_##_array_[] = _ARR_; \
    //     internal_match_method(_stream_)\
    //     { \
    //         using T0 = token_string<_NAME_##_array_, getSize(_ARR_)-1>;\
    //         return sub_match(T0, _stream_);\
    //     } \
    // };

    // //template<a,b,c> if needed
    // #define rule_start(_NAME_) \
    // class _NAME_ : public rule_base { \
    // public: \
    //     signature_noargs(_NAME_) \
    //     using _rule_impl_ = 
        
    // #define rule_end(_NAME_) ; match_method(_stream_) { return _rule_impl_::match(_stream_, _trace_ ? _trace_ + (_internal_ ? 0 : 1) : 0, _rule_name_); }
    // #define internal_rule_end(_NAME_) ; internal_match_method(_stream_) { return _rule_impl_::match(_stream_, _trace_ ? _trace_ + (_internal_ ? 0 : 1) : 0, _rule_name_); }

    // #define re_match(_NAME_, _PATTERN_) \
    // char_array_decl(_NAME_##_pattern_) = _PATTERN_; \
    // class _NAME_ : public ast::_f::select<token_regex<_NAME_##_pattern_>,0> {};

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
//ast_set_signature<x,y,z>;
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

    template<size_t _Nm>
    constexpr auto ast_str_remove_last_char(const char (&__a)[_Nm]) -> std::array<char, _Nm - 1>
    {
        std::array<char, _Nm - 1> __r{};
        for (size_t __i = 0; __i < _Nm - 1; ++__i)
        {
            __r[__i] = __a[__i];
        }
        return __r;
    }

    #define ast_define_rule(_NAME_) class _NAME_ : public rule_base
    // #define ast_internal_rule(_NAME_) class _NAME_ : public rule_base
    #define ast_internal_rule(_NAME_) class _NAME_ : public rule_base_internal
    #define ast_base_rule using _ast_rule_base_
    #define ast_set_signature using signature = signing::sign
    #define ast_str(_STR_) std::to_array(_STR_)
    #define ast_str_arr(_STR_) ast::_h::ast_str_remove_last_char(_STR_)
    
    #define ast_sig(_TYPE_) signing::get_from<_TYPE_>{}
    #define ast_base_signature _ast_rule_base_::signature
    #define sub_match(_T0_, _varname_) _T0_::match(_varname_, _trace_ ? _trace_ + (_ast_internal_rule_ ? 0 : 1) : 0)
    #define ast_variant_implementation \
        static const signing::signature_id static_get_typeid() { return &signature::print; } \
        virtual const signing::signature_id get_typeid() const override { return static_get_typeid(); } \
        virtual bool is_same_as(signing::signature_id s) const override { return static_get_typeid() == s; }
    #define ast_variant_inherit(_BASE_) \
        static const signing::signature_id static_get_typeid() { return &signature::print; } \
        virtual const signing::signature_id get_typeid() const override { return static_get_typeid(); } \
        virtual bool is_same_as(signing::signature_id s) const override { return static_get_typeid() == s || _BASE_::is_same_as(s); }

    #define ast_alias_implementation \
    ast_variant_inherit(rule_base) \
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
                signature::print();\
                printf(") : ");\
                _ast_rule_base_::signature::print();\
                printf("\n"); \
            } \
            auto l = _ast_rule_base_::match(_stream_, _trace_ ? _trace_ +  1 : 0); \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                signature::print();\
                printf(") : %s\n", l ? "PASS" : "FAIL");\
            } \
            if (l) \
            { \
                l->rule = static_get_typeid();\
            } \
            return l; \
        } \
    } 

    #define ast_decorator_implementation(_L_) \
    ast_variant_inherit(rule_base) \
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
                signature::print();\
                printf(") : ");\
                _ast_rule_base_::signature::print();\
                printf("\n"); \
            } \
            auto l = _ast_rule_base_::match(_stream_, _trace_ ? _trace_ +  1 : 0); \
            if (l) l = decorate(l); \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                signature::print();\
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
    ast_variant_inherit(rule_base) \
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
                signature::print();\
                printf(") \n");\
            } \
            auto l = match_impl(_stream_, _trace_); \
            if (_trace_) \
            { \
                print_ind(_trace_, "");\
                lex_o<object_S>(*_stream_).printvalue();\
                printf(" (");\
                signature::print();\
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
    ast_set_signature<ast_str(#_NAME_)>; \
    ast_base_rule

    #define ast_alias(_NAME_) \
    ast_define_rule(_NAME_) { \
    public: \
    ast_set_signature<ast_str(#_NAME_)>; \
    ast_base_rule

    #define ast_alias_end \
    ast_alias_implementation \
    }

    #define ast_regex_rule(_NAME_, _PATTERN_) ast_alias(_NAME_) = regex<ast_str(_PATTERN_)>; ast_alias_end;
    #define ast_internal_regex(_NAME_, _PATTERN_) ast_internal_alias(_NAME_) = regex<ast_str(_PATTERN_)>; ast_alias_end;
    #define ast_string_rule(_NAME_, _STR_) ast_alias(_NAME_) = token_string<ast_str_arr(_STR_)>; ast_alias_end;
    #define ast_internal_string(_NAME_, _STR_) ast_internal_alias(_NAME_) = token_string<ast_str_arr(_STR_)>; ast_alias_end;
}