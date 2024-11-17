#pragma once

#include <array>
#include <iostream>

namespace uti
{
    template <std::size_t N>
    constexpr std::array<char, N> STR2A(const char (&str)[N])
    {
        std::array<char, N> result = {};
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = str[i];
        }
        return result;
    }

    template <int value>
    constexpr int log10()
    {
        return 1 + log10<value / 10>();
    }

    template <>
    constexpr int log10<0>()
    {
        return 0;
    }

    template <int value>
    constexpr auto INT2A()
    {
        constexpr int pad = log10<value>();
        std::array<char, pad+1> result = {};
        std::size_t index = pad;
        int temp_value = value;
        bool is_negative = temp_value < 0;
        if (is_negative)
        {
            temp_value = -temp_value;
        }
        do
        {
            result[--index] = '0' + (temp_value % 10);
            temp_value /= 10;
        } while (temp_value > 0 && index > 0);
        if (is_negative && index > 0)
        {
            result[--index] = '-';
        }
        while (index > 0)
        {
            result[--index] = ' ';
        }
        result[pad] = '\0';
        return result;
    }

    constexpr std::size_t findChar(const char* str, char ch, std::size_t start=0, std::size_t end=1024) {
        for (std::size_t i = start; i < end; ++i) {
            if (str[i] == ch) {
                return i;
            }
            if (str[i] == '\0') {
                return std::size_t(-1);
            }
        }
        return std::size_t(-1);
    }

    template <std::size_t N1, std::size_t N2>
    constexpr auto operator+(const std::array<char, N1> &str1, const std::array<char, N2> &str2)
    {
        std::array<char, N1 + N2 + 1> result = {};
        std::size_t i = 0, j = 0;
        while (i < N1 && str1[i] != '\0')
        {
            result[i] = str1[i];
            ++i;
        } 
        for (j = 0; j < N2 && str2[j] != '\0'; ++j)
        {
            result[i + j] = str2[j];
        }
        result[i+j] = '\0';
        return result;
    }

    template <std::size_t N>
    const char *exectractParamName(uint16_t index, const std::array<char, N> &split_signature) {
        uint16_t cur = 0;
        for (int i=0; i<N; i++)
        {
            if (split_signature[i] == '\0')
            {
                while (split_signature[i] == '\0') i++;
                while (split_signature[i] == ' ') i++;
                if (cur == index)
                {
                    return split_signature.data() + i;
                }
                cur++;
            }
        }
        return nullptr;
    }

    template <std::size_t N>
    constexpr auto split(const std::array<char, N> &str)
    {
        std::array<char, N> result = {};
        std::size_t i = 0;
        for (i = 0; i < N; ++i)
        {
            if (str[i] == '(' || str[i] == ',' || str[i] == ')')
                result[i] = '\0';
            else
                result[i] = str[i];
        }
        return result;
    }
}