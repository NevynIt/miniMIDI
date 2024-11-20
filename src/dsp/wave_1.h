#pragma once
#include "wave_0.h"

namespace dsp
{
    extern wave wave_dump;

    class op0
    {
    public:
        static constexpr uint8_t op_count = 0;
        static constexpr auto signature = uti::STR2A("<>");
        wave* get_wave(uint8_t index) { return nullptr; }
    };

    template <typename Base1 = wave>
    class op1
    {
    public:
        static constexpr uint8_t op_count = 1;
        Base1 o1;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : nullptr; }
    };

    template <typename Base1 = wave, typename Base2 = wave>
    class op2
    {
    public:
        static constexpr uint8_t op_count = 2;
        Base1 o1;
        Base2 o2;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : nullptr); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave>
    class op3
    {
    public:
        static constexpr uint8_t op_count = 3;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : nullptr)); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave>
    class op4
    {
    public:
        static constexpr uint8_t op_count = 4;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : nullptr))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave>
    class op5
    {
    public:
        static constexpr uint8_t op_count = 5;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : nullptr)))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave>
    class op6
    {
    public:
        static constexpr uint8_t op_count = 6;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : nullptr))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave>
    class op7
    {
    public:
        static constexpr uint8_t op_count = 7;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : nullptr)))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave>
    class op8
    {
    public:
        static constexpr uint8_t op_count = 8;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : nullptr))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave>
    class op9
    {
    public:
        static constexpr uint8_t op_count = 9;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : nullptr)))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave>
    class op10
    {
    public:
        static constexpr uint8_t op_count = 10;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : nullptr))))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave>
    class op11
    {
    public:
        static constexpr uint8_t op_count = 11;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        Base11 o11;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(", ") + Base11::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : nullptr)))))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave>
    class op12
    {
    public:
        static constexpr uint8_t op_count = 12;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        Base11 o11;
        Base12 o12;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(", ") + Base11::signature + uti::STR2A(", ") + Base12::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : nullptr))))))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave>
    class op13
    {
    public:
        static constexpr uint8_t op_count = 13;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        Base11 o11;
        Base12 o12;
        Base13 o13;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(", ") + Base11::signature + uti::STR2A(", ") + Base12::signature + uti::STR2A(", ") + Base13::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : nullptr)))))))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave, typename Base14 = wave>
    class op14
    {
    public:
        static constexpr uint8_t op_count = 14;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        Base11 o11;
        Base12 o12;
        Base13 o13;
        Base14 o14;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(", ") + Base11::signature + uti::STR2A(", ") + Base12::signature + uti::STR2A(", ") + Base13::signature + uti::STR2A(", ") + Base14::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : (index == 13 ? &o14 : nullptr))))))))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave, typename Base14 = wave, typename Base15 = wave>
    class op15
    {
    public:
        static constexpr uint8_t op_count = 15;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        Base11 o11;
        Base12 o12;
        Base13 o13;
        Base14 o14;
        Base15 o15;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(", ") + Base11::signature + uti::STR2A(", ") + Base12::signature + uti::STR2A(", ") + Base13::signature + uti::STR2A(", ") + Base14::signature + uti::STR2A(", ") + Base15::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : (index == 13 ? &o14 : (index == 14 ? &o15 : nullptr)))))))))))))); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave, typename Base14 = wave, typename Base15 = wave, typename Base16 = wave>
    class op16
    {
    public:
        static constexpr uint8_t op_count = 16;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        Base7 o7;
        Base8 o8;
        Base9 o9;
        Base10 o10;
        Base11 o11;
        Base12 o12;
        Base13 o13;
        Base14 o14;
        Base15 o15;
        Base16 o16;
        static constexpr auto signature = uti::STR2A("<") + Base1::signature + uti::STR2A(", ") + Base2::signature + uti::STR2A(", ") + Base3::signature + uti::STR2A(", ") + Base4::signature + uti::STR2A(", ") + Base5::signature + uti::STR2A(", ") + Base6::signature + uti::STR2A(", ") + Base7::signature + uti::STR2A(", ") + Base8::signature + uti::STR2A(", ") + Base9::signature + uti::STR2A(", ") + Base10::signature + uti::STR2A(", ") + Base11::signature + uti::STR2A(", ") + Base12::signature + uti::STR2A(", ") + Base13::signature + uti::STR2A(", ") + Base14::signature + uti::STR2A(", ") + Base15::signature + uti::STR2A(", ") + Base16::signature + uti::STR2A(">");
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : (index == 13 ? &o14 : (index == 14 ? &o15 : (index == 15 ? &o16 : nullptr))))))))))))))); }
    };
}