#pragma once

#include <cstdint>
#include <array>
#include <type_traits>
#include <cstdio>

// template <std::size_t N>
// constexpr std::array<char, N> STR2A(const char (&str)[N])
// {
//     std::array<char, N> result = {};
//     for (std::size_t i = 0; i < N; ++i)
//     {
//         result[i] = str[i];
//     }
//     return result;
// }

// template <int value>
// constexpr int log10()
// {
//     return 1 + log10<value / 10>();
// }

// template <>
// constexpr int log10<0>()
// {
//     return 0;
// }

// template <int value>
// constexpr auto INT2A()
// {
//     constexpr int pad = log10<value>();
//     std::array<char, pad+1> result = {};
//     std::size_t index = pad;
//     int temp_value = value;
//     bool is_negative = temp_value < 0;
//     if (is_negative)
//     {
//         temp_value = -temp_value;
//     }
//     do
//     {
//         result[--index] = '0' + (temp_value % 10);
//         temp_value /= 10;
//     } while (temp_value > 0 && index > 0);
//     if (is_negative && index > 0)
//     {
//         result[--index] = '-';
//     }
//     while (index > 0)
//     {
//         result[--index] = ' ';
//     }
//     result[pad] = '\0';
//     return result;
// }

// template<>
// constexpr auto INT2A<0>()
// {
//     return STR2A("0");
// }

// template <std::size_t N1, std::size_t N2>
// constexpr auto operator+(const std::array<char, N1> &str1, const std::array<char, N2> &str2)
// {
//     std::array<char, N1 + N2 + 1> result = {};
//     std::size_t i = 0, j = 0;
//     while (i < N1 && str1[i] != '\0')
//     {
//         result[i] = str1[i];
//         ++i;
//     } 
//     for (j = 0; j < N2 && str2[j] != '\0'; ++j)
//     {
//         result[i + j] = str2[j];
//     }
//     result[i+j] = '\0';
//     return result;
// }

// template<int N>
// class signature_get_int {
// public:
//     static constexpr auto signature = INT2A<N>();
// };

// template<char C>
// class signature_get_char {
// public:
//     static constexpr auto signature = STR2A({C, '\0'});
// };

// template<const char *S> //insormontabile, un const char * non puó avere dimensione fissata a compile time
// class signature_get_string {
// public:
//     static constexpr auto signature = STR2A("STR");
//     // static constexpr auto signature = STR2A(S);
// };

// template<typename T>
// class signature_get_type {
// public:
//     static constexpr auto signature = STR2A("??");
// };

// template<>
// class signature_get_type<int> {
// public:
//     static constexpr auto signature = STR2A("int");
// };

// template<>
// class signature_get_type<char> {
// public:
//     static constexpr auto signature = STR2A("char");
// };

// template<>
// class signature_get_type<const char> {
// public:
//     static constexpr auto signature = STR2A("const char");
// };

// // Helper function to concatenate signatures
// constexpr auto concat_signatures() {
//     return STR2A("");
// }

// template <std::size_t N1, std::size_t N2>
// constexpr auto concat_array(const std::array<char, N1> &str1, const std::array<char, N2> &str2)
// {
//     std::array<char, N1 + N2 + 1> result = {};
//     std::size_t i = 0, j = 0;
//     while (i < N1 && str1[i] != '\0')
//     {
//         result[i] = str1[i];
//         ++i;
//     } 
//     for (j = 0; j < N2 && str2[j] != '\0'; ++j)
//     {
//         result[i + j] = str2[j];
//     }
//     result[i+j] = '\0';
//     return result;
// }

// template<typename First, typename... Rest>
// constexpr auto concat_signatures(const First& first, const Rest&... rest) {
//     return concat_array(first, concat_array(STR2A(","), concat_signatures(rest...)));
//     // return concat_array(first, sizeof...(rest) > 0 ? concat_array(STR2A(", "), concat_signatures(rest...)) : STR2A(""));
// }

// #define GET_SIGNATURE_ARGS_0() STR2A("")
// #define GET_SIGNATURE_ARGS_1(_1) _1::signature
// #define GET_SIGNATURE_ARGS_2(_1, _2) concat_signatures(_1::signature, _2::signature)
// #define GET_SIGNATURE_ARGS_3(_1, _2, _3) concat_signatures(_1::signature, _2::signature, _3::signature)
// #define GET_SIGNATURE_ARGS_4(_1, _2, _3, _4) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature)
// #define GET_SIGNATURE_ARGS_5(_1, _2, _3, _4, _5) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature, _5::signature)
// #define GET_SIGNATURE_ARGS_6(_1, _2, _3, _4, _5, _6) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature, _5::signature, _6::signature)
// #define GET_SIGNATURE_ARGS_7(_1, _2, _3, _4, _5, _6, _7) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature, _5::signature, _6::signature, _7::signature)
// #define GET_SIGNATURE_ARGS_8(_1, _2, _3, _4, _5, _6, _7, _8) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature, _5::signature, _6::signature, _7::signature, _8::signature)
// #define GET_SIGNATURE_ARGS_9(_1, _2, _3, _4, _5, _6, _7, _8, _9) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature, _5::signature, _6::signature, _7::signature, _8::signature, _9::signature)
// #define GET_SIGNATURE_ARGS_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10) concat_signatures(_1::signature, _2::signature, _3::signature, _4::signature, _5::signature, _6::signature, _7::signature, _8::signature, _9::signature, _10::signature)

// #define GET_SIGNATURE_ARGS_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, COUNT, ...) COUNT
// #define GET_SIGNATURE_ARGS_RECOMPOSER(argsWithParentheses) GET_SIGNATURE_ARGS_COUNT argsWithParentheses
// #define GET_SIGNATURE_ARGS(...) GET_SIGNATURE_ARGS_RECOMPOSER((__VA_ARGS__, GET_SIGNATURE_ARGS_10, GET_SIGNATURE_ARGS_9, GET_SIGNATURE_ARGS_8, GET_SIGNATURE_ARGS_7, GET_SIGNATURE_ARGS_6, GET_SIGNATURE_ARGS_5, GET_SIGNATURE_ARGS_4, GET_SIGNATURE_ARGS_3, GET_SIGNATURE_ARGS_2, GET_SIGNATURE_ARGS_1, GET_SIGNATURE_ARGS_0))
// // #define GET_SIGNATURE_ARGS(...) GET_SIGNATURE_ARGS_RECOMPOSER((__VA_ARGS__, GET_SIGNATURE_ARGS_10, GET_SIGNATURE_ARGS_9, GET_SIGNATURE_ARGS_8, GET_SIGNATURE_ARGS_7, GET_SIGNATURE_ARGS_6, GET_SIGNATURE_ARGS_5, GET_SIGNATURE_ARGS_4, GET_SIGNATURE_ARGS_3, GET_SIGNATURE_ARGS_2, GET_SIGNATURE_ARGS_1, GET_SIGNATURE_ARGS_0))(__VA_ARGS__)

// #define SIGNATURE_ARGS(...) \
//     concat_array(STR2A("<"), concat_array(GET_SIGNATURE_ARGS(__VA_ARGS__)(__VA_ARGS__), STR2A(">")))

// // Modified signature_decl macro for no arguments
// #define signature_noargs(_NAME_) \
//     static constexpr auto signature = STR2A(#_NAME_); \
//     virtual const char *get_signature() const override { return signature.data(); }

// // New signature_decl_args macro for variadic arguments
// #define signature_decl(_NAME_, ...) \
//     static constexpr auto signature = concat_array( \
//         STR2A(#_NAME_), \
//         SIGNATURE_ARGS(__VA_ARGS__)); \
//     virtual const char *get_signature() const override { return signature.data(); }

// #define signature_inherit(_NAME_) \
//     virtual bool is(const char *s) const override { return get_signature() == s || _NAME_::get_signature() == s; }


#include <type_traits>
#include <string>
#include <array>

namespace signing
{
    struct signature_canary {};

    template<typename T>
    static inline void print_val(T val)
    {
        if constexpr (std::is_same_v<typename T::canary, signature_canary>)
        {
            T::print();
        }
        else
        {
            printf("?");
        }
    }

    static inline void print_val(char val)
    {
        printf("'%c'", val);
    }

    static inline void print_val(const char *val)
    {
        printf("\"%s\"", val);
    }

    static inline void print_val(int val)
    {
        printf("%d", val);
    }

    template<size_t N>
    static inline void print_val(const std::array<char, N> &val)
    {
        printf("%s", val.data());
    }

    static inline void print_one(auto v1, auto ...v)
    {
        print_val(v1);
        if constexpr (sizeof...(v) > 0)
        {
            printf(", ");
            print_one(v...);
        }
    }

    static inline void print_args(auto v1, auto ...v)
    {
        v1.print();
        if constexpr (sizeof...(v) > 0)
        {
            printf(", ");
            print_args(v...);
        }
    }

    static inline void print_signature(auto name, auto ...v)
    {
        print_val(name);
        if constexpr (sizeof...(v) > 0)
        {
            printf("<");
            print_one(v...);
            printf(">");
        }
    }

    template<size_t _Nm>
    constexpr inline auto cs(const char (&__a)[_Nm]) //char string
    {
        return std::to_array(__a);
    }

    template<auto ...str>
    class sign //signature
    {
    public:
        using canary = signature_canary;

        static inline void print()
        {
            if constexpr (sizeof...(str) > 0)
                print_signature(str...);
            else
                printf("??");
        }
    };

    typedef void (*signature_id)(); 

    // Primary template (not defined)
    template<typename T, typename Enable = void>
    struct get_signature
    {
        static constexpr auto s = sign<>{};
    };

    // Specialization for types with canary member
    template<typename T>
    struct get_signature<T, std::enable_if_t<std::is_same_v<typename T::canary, signature_canary>>> {
        static constexpr auto s = T{};
    };

    // Specialization for types with signature member, which has a canary member
    template<typename T>
    struct get_signature<T, std::enable_if_t<std::is_same_v<typename T::signature::canary, signature_canary>>> {
        static constexpr auto s = typename T::signature{};
    };

    // // Specialization for types without canary member
    // template<typename T>
    // struct get_signature<T, std::enable_if_t<!std::is_same_v<typename T::canary, signature_canary>>> {
    //     static constexpr auto s = sign<>{};
    // };

    // Specialization for T == int
    template<>
    struct get_signature<int> {
        static constexpr auto s = sign<cs("int")>{};
    };

    // Specialization for T == char
    template<>
    struct get_signature<char> {
        static constexpr auto s = sign<cs("char")>{};
    };


    template<typename T, typename ...types>
    static inline void print_type()
    {
        get_signature<T>::s.print();
        if constexpr (sizeof...(types) > 0)
        {
            printf(", ");
            print_type<types...>();
        }
    }

    template<typename ...types>
    static inline void print_types()
    {
        if constexpr (sizeof...(types) > 0)
        {
            print_type<types...>();
        }
        else
        {
            printf("??");
        }
    }

    template<typename ...types>
    class get_from 
    {
    public:
        using canary = signature_canary;

        static inline void print()
        {
            if constexpr (sizeof...(types) > 0)
                print_types<types...>();
            else
                printf("??");
        }
    };
}
