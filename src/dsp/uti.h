#pragma once

#include <array>
#include <iostream>

namespace uti
{
    template <std::size_t N1, std::size_t N2>
    constexpr std::array<char, N1 + N2 - 1> concatenate(const std::array<char, N1>& str1, const std::array<char, N2>& str2) {
        std::array<char, N1 + N2 - 1> result = {};
        for (std::size_t i = 0; i < N1 - 1; ++i) {
            result[i] = str1[i];
        }
        for (std::size_t i = 0; i < N2; ++i) {
            result[N1 - 1 + i] = str2[i];
        }
        return result;
    }

    template <std::size_t N>
    constexpr std::array<char, N> to_array(const char (&str)[N]) {
        std::array<char, N> result = {};
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = str[i];
        }
        return result;
    }

    template <std::size_t N>
    constexpr std::array<char, N> reverse_array(const std::array<char, N>& arr) {
        std::array<char, N> result = {};
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = arr[N - 1 - i];
        }
        return result;
    }

    template <std::size_t N>
    constexpr std::array<char, N + 1> add_null_terminator(const std::array<char, N>& arr) {
        std::array<char, N + 1> result = {};
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = arr[i];
        }
        result[N] = '\0';
        return result;
    }

    template <std::size_t N>
    constexpr std::array<char, N> int_to_string_impl(int value) {
        std::array<char, N> result = {};
        std::size_t index = 0;
        bool is_negative = value < 0;
        if (is_negative) {
            value = -value;
        }
        do {
            result[index++] = '0' + (value % 10);
            value /= 10;
        } while (value > 0);
        if (is_negative) {
            result[index++] = '-';
        }
        return reverse_array(result);
    }

    template <int value>
    constexpr auto int_to_string() {
        constexpr std::size_t buffer_size = 20; // Adjust size as needed
        return add_null_terminator(int_to_string_impl<buffer_size>(value));
    }
}