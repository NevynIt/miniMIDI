#pragma once

#include <cstdint>
#include <array>
#include <type_traits>
#include <cstdio>

namespace uti
{
    template<size_t n1, size_t n2>
    constexpr inline auto concat_str_comma(const std::array<char, n1> &__a1, const std::array<char, n2> &__a2) //concatenate char arrays
    {
        std::array<char, n1 + n2 - 1 + 2> __r{};
        for (size_t __i = 0; __i < n1 - 1; ++__i)
        {
            __r[__i] = __a1[__i];
        }
        __r[n1-1] = ',';
        __r[n1] = ' ';
        for (size_t __i = 0; __i < n2 - 1; ++__i)
        {
            __r[n1 + 1 + __i] = __a2[__i];
        }
        return __r;
    }

    template<size_t n1, size_t n2>
    constexpr inline auto concat_str(const std::array<char, n1> &__a1, const std::array<char, n2> &__a2) //concatenate char arrays
    {
        std::array<char, n1 + n2 - 1> __r{};
        for (size_t __i = 0; __i < n1 - 1; ++__i)
        {
            __r[__i] = __a1[__i];
        }
        for (size_t __i = 0; __i < n2 - 1; ++__i)
        {
            __r[n1 - 1 + __i] = __a2[__i];
        }
        return __r;
    }

    template<size_t n1, size_t n2, size_t ...n>
    constexpr inline auto concat_str(const std::array<char, n1> &__a1, const std::array<char, n2> &__a2, const std::array<char, n> &...__args) //concatenate char arrays
    {
        return concat_str(concat_str(__a1, __a2), __args...);
    }

    template<typename _ty>
    struct is_char_array : std::false_type {};

    template<size_t _sz>
    struct is_char_array<const std::array<char, _sz>> : std::true_type {};

    template<size_t _sz>
    struct is_char_array<std::array<char, _sz>> : std::true_type {};

    template<size_t _Nm>
    constexpr inline auto str(const char (&__a)[_Nm]) //char string
    {
        return std::to_array(__a);
    }

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

    template<int n>
    constexpr inline auto str();

    template<double d>
    constexpr inline auto str();

    template<auto A1, auto ...args>
    constexpr inline auto concat()
    {
        if constexpr (sizeof...(args) == 0)
            if constexpr (is_char_array<decltype(A1)>::value)
                return A1;
            else
                return str<A1>();
        else
            return concat_str_comma(concat<A1>(), concat<args...>());
    }

    template<int n>
    constexpr inline auto str() //integer to string
    {
        if constexpr(n < 0)
            return concat_str(str("-"), str<-n>());
        else if constexpr (n == 0)
            return str("0");
        else if constexpr (n == 1)
            return str("1");
        else if constexpr (n == 2)
            return str("2");
        else if constexpr (n == 3)
            return str("3");
        else if constexpr (n == 4)
            return str("4");
        else if constexpr (n == 5)
            return str("5");
        else if constexpr (n == 6)
            return str("6");
        else if constexpr (n == 7)
            return str("7");
        else if constexpr (n == 8)
            return str("8");
        else if constexpr (n == 9)
            return str("9");
        else
            return concat_str(str<n / 10>(), str<n % 10>());
    }

    constexpr inline double frac(const double n)
    {
        double d = n - (int)n;
        return d;
    }

    template<double d>
    constexpr inline auto str_frac()
    {
        return concat_str(str("."), str<(int)(frac(d) * 1000000)>());
    }

    template<double d>
    constexpr inline auto str()
    {
        if constexpr (d < 0)
            return concat_str(str("-"), str<-d>());
        else
            return concat_str(str<(int)d>(), str_frac<d>());
    }

    template<char c>
    constexpr inline auto c2str() //single char
    {
        std::array<char, 4> __r{};
        __r[0] = '\'';
        __r[1] = c;
        __r[2] = '\'';
        __r[3] = '\0';
        return __r;
    }

    template<typename T, typename Enable = void>
    struct get_from_one
    {
        static constexpr auto s = str("??");
    };

    template<typename T>
    struct get_from_one<T, std::enable_if_t<is_char_array<decltype(T::signature)>::value>> //get signature from type
    {
        static constexpr auto s = T::signature;
    };

    template<>
    struct get_from_one<int>
    {
        static constexpr auto s =  str("int");
    };

    template<>
    struct get_from_one<char>
    {
        static constexpr auto s = str("char");
    };

    template<>
    struct get_from_one<const char>
    {
        static constexpr auto s = str("const char");
    };

    template<typename T0, typename ...types>
    inline constexpr auto sig_of()
    {
        if constexpr (sizeof...(types) == 0)
            return get_from_one<T0>::s;
        else
            return concat_str_comma(get_from_one<T0>::s, sig_of<types...>());
    }

    template<auto A0, auto ...args>
    constexpr inline auto concat_param()
    {
        return concat_str(concat<A0>(), str("<"), concat<args...>(), str(">"));
    }

    template<auto ...args>
    constexpr inline auto sign()
    {
        if constexpr (sizeof...(args) == 0)
            return str("");
        else if constexpr (sizeof...(args) == 1)
            return concat<args...>();
        else
            return concat_param<args...>();
    }

    typedef const char *signature_id; 

    #define set_signature static constexpr auto signature = uti::sign
}
