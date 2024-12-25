#pragma once
#include "wave_root.h"

namespace dsp
{
    extern wave wave_dump;

    class op0 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 0;
        set_signature<uti::str("op0")>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return nullptr; }

        template<int N>
        inline auto& get() { static_assert(false, "Invalid operator index"); return *this; }
    };

    template <typename Base1 = wave>
    class op1 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 1;
        Base1 o1;
        set_signature<uti::str("op1"), uti::sig_of<Base1>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : nullptr; }

        template<int N>
        inline auto& get() { static_assert(N == 0, "Invalid operator index"); return o1; }
    };

    template <typename Base1 = wave, typename Base2 = wave>
    class op2 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 2;
        Base1 o1;
        Base2 o2;
        set_signature<uti::str("op2"), uti::sig_of<Base1>(), uti::sig_of<Base2>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : nullptr); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave>
    class op3 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 3;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        set_signature<uti::str("op3"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : nullptr)); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave>
    class op4 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 4;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        set_signature<uti::str("op4"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : nullptr))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave>
    class op5 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 5;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        set_signature<uti::str("op5"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : nullptr)))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave>
    class op6 : public uti::variant
    {
    public:
        static constexpr uint8_t op_count = 6;
        Base1 o1;
        Base2 o2;
        Base3 o3;
        Base4 o4;
        Base5 o5;
        Base6 o6;
        set_signature<uti::str("op6"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : nullptr))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave>
    class op7 : public uti::variant
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
        set_signature<uti::str("op7"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : nullptr)))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave>
    class op8 : public uti::variant
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
        set_signature<uti::str("op8"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : nullptr))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave>
    class op9 : public uti::variant
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
        set_signature<uti::str("op9"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : nullptr)))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave>
    class op10 : public uti::variant
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
        set_signature<uti::str("op10"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : nullptr))))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else if constexpr (N == 9) return o10; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave>
    class op11 : public uti::variant
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
        set_signature<uti::str("op11"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>(), uti::sig_of<Base11>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : nullptr)))))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else if constexpr (N == 9) return o10; else if constexpr (N == 10) return o11; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave>
    class op12 : public uti::variant
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
        set_signature<uti::str("op12"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>(), uti::sig_of<Base11>(), uti::sig_of<Base12>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : nullptr))))))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else if constexpr (N == 9) return o10; else if constexpr (N == 10) return o11; else if constexpr (N == 11) return o12; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave>
    class op13 : public uti::variant
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
        set_signature<uti::str("op13"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>(), uti::sig_of<Base11>(), uti::sig_of<Base12>(), uti::sig_of<Base13>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : nullptr)))))))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else if constexpr (N == 9) return o10; else if constexpr (N == 10) return o11; else if constexpr (N == 11) return o12; else if constexpr (N == 12) return o13; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave, typename Base14 = wave>
    class op14 : public uti::variant
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
        set_signature<uti::str("op14"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>(), uti::sig_of<Base11>(), uti::sig_of<Base12>(), uti::sig_of<Base13>(), uti::sig_of<Base14>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : (index == 13 ? &o14 : nullptr))))))))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else if constexpr (N == 9) return o10; else if constexpr (N == 10) return o11; else if constexpr (N == 11) return o12; else if constexpr (N == 12) return o13; else if constexpr (N == 13) return o14; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave, typename Base14 = wave, typename Base15 = wave>
    class op15 : public uti::variant
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
        set_signature<uti::str("op15"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>(), uti::sig_of<Base11>(), uti::sig_of<Base12>(), uti::sig_of<Base13>(), uti::sig_of<Base14>(), uti::sig_of<Base15>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : (index == 13 ? &o14 : (index == 14 ? &o15 : nullptr)))))))))))))); }

        template<int N>
        inline auto& get() { if constexpr (N == 0) return o1; else if constexpr (N == 1) return o2; else if constexpr (N == 2) return o3; else if constexpr (N == 3) return o4; else if constexpr (N == 4) return o5; else if constexpr (N == 5) return o6; else if constexpr (N == 6) return o7; else if constexpr (N == 7) return o8; else if constexpr (N == 8) return o9; else if constexpr (N == 9) return o10; else if constexpr (N == 10) return o11; else if constexpr (N == 11) return o12; else if constexpr (N == 12) return o13; else if constexpr (N == 13) return o14; else if constexpr (N == 14) return o15; else static_assert(false, "Invalid operator index"); }
    };

    template <typename Base1 = wave, typename Base2 = wave, typename Base3 = wave, typename Base4 = wave, typename Base5 = wave, typename Base6 = wave, typename Base7 = wave, typename Base8 = wave, typename Base9 = wave, typename Base10 = wave, typename Base11 = wave, typename Base12 = wave, typename Base13 = wave, typename Base14 = wave, typename Base15 = wave, typename Base16 = wave>
    class op16 : public uti::variant
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
        set_signature<uti::str("op16"), uti::sig_of<Base1>(), uti::sig_of<Base2>(), uti::sig_of<Base3>(), uti::sig_of<Base4>(), uti::sig_of<Base5>(), uti::sig_of<Base6>(), uti::sig_of<Base7>(), uti::sig_of<Base8>(), uti::sig_of<Base9>(), uti::sig_of<Base10>(), uti::sig_of<Base11>(), uti::sig_of<Base12>(), uti::sig_of<Base13>(), uti::sig_of<Base14>(), uti::sig_of<Base15>(), uti::sig_of<Base16>()>();
        variant_inherit(uti::variant)
        wave* get_wave(uint8_t index) { return index == 0 ? &o1 : (index == 1 ? &o2 : (index == 2 ? &o3 : (index == 3 ? &o4 : (index == 4 ? &o5 : (index == 5 ? &o6 : (index == 6 ? &o7 : (index == 7 ? &o8 : (index == 8 ? &o9 : (index == 9 ? &o10 : (index == 10 ? &o11 : (index == 11 ? &o12 : (index == 12 ? &o13 : (index == 13 ? &o14 : (index == 14 ? &o15 : (index == 15 ? &o16 : nullptr))))))))))))))); }

        template<int N>
        inline auto& get() { 
            if constexpr (N == 0) return o1; 
            else if constexpr (N == 1) return o2; 
            else if constexpr (N == 2) return o3; 
            else if constexpr (N == 3) return o4; 
            else if constexpr (N == 4) return o5; 
            else if constexpr (N == 5) return o6; 
            else if constexpr (N == 6) return o7; 
            else if constexpr (N == 7) return o8; 
            else if constexpr (N == 8) return o9; 
            else if constexpr (N == 9) return o10; 
            else if constexpr (N == 10) return o11; 
            else if constexpr (N == 11) return o12; 
            else if constexpr (N == 12) return o13; 
            else if constexpr (N == 13) return o14; 
            else if constexpr (N == 14) return o15; 
            else if constexpr (N == 15) return o16; 
            else static_assert(false, "Invalid operator index"); 
        }
    };
}