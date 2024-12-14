#pragma once

#include <cstdint>
#include <array>
#include <type_traits>
#include <cstdio>

namespace uti
{
    template<size_t _Nm>
    constexpr auto str_no0(const char (&__a)[_Nm]) -> std::array<char, _Nm - 1>
    {
        std::array<char, _Nm - 1> __r{};
        for (size_t __i = 0; __i < _Nm - 1; ++__i)
        {
            __r[__i] = __a[__i];
        }
        return __r;
    }

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

    static inline void print_val(unsigned int val)
    {
        printf("%d", val);
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
    constexpr inline auto str(const char (&__a)[_Nm]) //char string
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
        static constexpr auto s = sign<str("int")>{};
    };

    // Specialization for T == char
    template<>
    struct get_signature<char> {
        static constexpr auto s = sign<str("char")>{};
    };

    // Specialization for T == const char *
    template<>
    struct get_signature<const char *> {
        static constexpr auto s = sign<str("const char *")>{};
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

    #define set_signature using signature = uti::sign
    #define sig_of(_TYPE_) uti::get_from<_TYPE_>{}

}
