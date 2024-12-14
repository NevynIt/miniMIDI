#pragma once

#include "../uti/signature.h"
#include "../uti/variant.h"

namespace uti
{
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