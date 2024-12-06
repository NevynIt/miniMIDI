#pragma once

#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

//basic decorators for lexemes

namespace ast::_d
{
    template<typename T0, typename F0>
    class dec//orator
    {
    public:
        match_method(s)
        {
            if (!F0::pre_match(s)) return nullptr;
            lexeme_S *l = T0::match(s);
            if (l) l = F0::post_match(l); else F0::no_match(s);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1>
    class dec2
    {
    public:
        match_method(s)
        {
            if (!F1::pre_match(s)) return nullptr;
            if (!F0::pre_match(s)) { F1::no_match(s); return nullptr; }
            lexeme_S *l = T0::match(s);
            if (l) l = F0::post_match(l); else { F0::no_match(s); F1::no_match(s); }
            if (l) l = F1::post_match(l); else F1::no_match(s);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2>
    class dec3
    {
    public:
        match_method(s)
        {
            if (!F2::pre_match(s)) return nullptr;
            if (!F1::pre_match(s)) { F2::no_match(s); return nullptr; }
            if (!F0::pre_match(s)) { F1::no_match(s); F2::no_match(s); return nullptr; }
            lexeme_S *l = T0::match(s);
            if (l) l = F0::post_match(l); else { F0::no_match(s); F1::no_match(s); F2::no_match(s); }
            if (l) l = F1::post_match(l); else { F1::no_match(s); F2::no_match(s); }
            if (l) l = F2::post_match(l); else F2::no_match(s);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2, typename F3>
    class dec4
    {
    public:
        match_method(s)
        {
            if (!F3::pre_match(s)) return nullptr;
            if (!F2::pre_match(s)) { F3::no_match(s); return nullptr; }
            if (!F1::pre_match(s)) { F2::no_match(s); F3::no_match(s); return nullptr; }
            if (!F0::pre_match(s)) { F1::no_match(s); F2::no_match(s); F3::no_match(s); return nullptr; }
            lexeme_S *l = T0::match(s);
            if (l) l = F0::post_match(l); else { F0::no_match(s); F1::no_match(s); F2::no_match(s); F3::no_match(s); }
            if (l) l = F1::post_match(l); else { F1::no_match(s); F2::no_match(s); F3::no_match(s); }
            if (l) l = F2::post_match(l); else { F2::no_match(s); F3::no_match(s); }
            if (l) l = F3::post_match(l); else F3::no_match(s);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2, typename F3, typename F4>
    class dec5
    {
    public:
        match_method(s)
        {
            if (!F4::pre_match(s)) return nullptr;
            if (!F3::pre_match(s)) { F4::no_match(s); return nullptr; }
            if (!F2::pre_match(s)) { F3::no_match(s); F4::no_match(s); return nullptr; }
            if (!F1::pre_match(s)) { F2::no_match(s); F3::no_match(s); F4::no_match(s); return nullptr; }
            if (!F0::pre_match(s)) { F1::no_match(s); F2::no_match(s); F3::no_match(s); F4::no_match(s); return nullptr; }
            lexeme_S *l = T0::match(s);
            if (l) l = F0::post_match(l); else { F0::no_match(s); F1::no_match(s); F2::no_match(s); F3::no_match(s); F4::no_match(s); }
            if (l) l = F1::post_match(l); else { F1::no_match(s); F2::no_match(s); F3::no_match(s); F4::no_match(s); }
            if (l) l = F2::post_match(l); else { F2::no_match(s); F3::no_match(s); F4::no_match(s); }
            if (l) l = F3::post_match(l); else { F3::no_match(s); F4::no_match(s); }
            if (l) l = F4::post_match(l); else F4::no_match(s);
            return l;
        }
    };

    template<typename T0, typename F0, typename F1, typename F2, typename F3, typename F4, typename F5>
    class dec6
    {
    public:
        match_method(s)
        {
            if (!F5::pre_match(s)) return nullptr;
            if (!F4::pre_match(s)) { F5::no_match(s); return nullptr; }
            if (!F3::pre_match(s)) { F4::no_match(s); F5::no_match(s); return nullptr; }
            if (!F2::pre_match(s)) { F3::no_match(s); F4::no_match(s); F5::no_match(s); return nullptr; }
            if (!F1::pre_match(s)) { F2::no_match(s); F3::no_match(s); F4::no_match(s); F5::no_match(s); return nullptr; }
            if (!F0::pre_match(s)) { F1::no_match(s); F2::no_match(s); F3::no_match(s); F4::no_match(s); F5::no_match(s); return nullptr; }
            lexeme_S *l = T0::match(s);
            if (l) l = F0::post_match(l); else { F0::no_match(s); F1::no_match(s); F2::no_match(s); F3::no_match(s); F4::no_match(s); F5::no_match(s); }
            if (l) l = F1::post_match(l); else { F1::no_match(s); F2::no_match(s); F3::no_match(s); F4::no_match(s); F5::no_match(s); }
            if (l) l = F2::post_match(l); else { F2::no_match(s); F3::no_match(s); F4::no_match(s); F5::no_match(s); }
            if (l) l = F3::post_match(l); else { F3::no_match(s); F4::no_match(s); F5::no_match(s); }
            if (l) l = F4::post_match(l); else { F4::no_match(s); F5::no_match(s); }
            if (l) l = F5::post_match(l); else F5::no_match(s);
            return l;
        }
    };
}