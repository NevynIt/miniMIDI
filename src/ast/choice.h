#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_c
{
    using namespace ast::_b;
    using namespace ast::_h;

    class lex_choice : public lexeme
    {
    public:
        ast_set_signature<ast_str("lex_choice")>;
        ast_variant_inherit(lexeme)

        lex_choice(lexeme *l = nullptr, int choice = 0) : l(l), choice(choice) {}
        ~lex_choice() { if (l) delete l; }

        lexeme *l = nullptr;
        int choice = 0;
    };

    template<typename _StreamType, typename T0, typename ...types>
    lexeme *choice_match(_StreamType &s, int _trace_)
    {
        lexeme *l = T0::match(s, _trace_ ? _trace_ +  1 : 0);
        if (l)
        {
            return l;
        }
        if constexpr (sizeof...(types) == 0)
        {
            return nullptr;
        }
        else
        {
            return choice_match<_StreamType, types...>(s, _trace_);
        }
    }

    template<typename ...types>
    ast_internal_rule(choice)
    {
    public:
        ast_set_signature<ast_str("choice"), signing::get_from<types...>{}>;

        ast_primary_implementation(s)
        {
            return choice_match<_StreamType, types...>(s, _trace_);
        }
    };

    template<typename _StreamType, typename T0, typename ...types>
    lexeme *choicei_match(_StreamType &s, int n, int _trace_)
    {
        lexeme *l = T0::match(s, _trace_ ? _trace_ +  1 : 0);
        if (l)
        {
            return new lex_choice(l, n);
        }
        if constexpr (sizeof...(types) == 0)
        {
            return nullptr;
        }
        else
        {
            return choicei_match<_StreamType, types...>(s, n+1, _trace_);
        }
    }

    template<typename ...types>
    ast_internal_rule(choicei)
    {
    public:
        ast_set_signature<ast_str("choicei"), signing::get_from<types...>{}>;

        ast_primary_implementation(s)
        {
            return choicei_match<_StreamType, types...>(s, 0, _trace_);
        }
    };

    // template<typename T0, typename T1>
    // class choice2 : public rule_base
    // {
    // public:
    //     signature_decl(choice2, T0, T1)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1>
    // class choice2i : public rule_base
    // {
    // public:
    //     signature_decl(choice2i, T0, T1)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2>
    // class choice3 : public rule_base
    // {
    // public:
    //     signature_decl(choice3, T0, T1, T2)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2>
    // class choice3i : public rule_base
    // {
    // public:
    //     signature_decl(choice3i, T0, T1, T2)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3>
    // class choice4 : public rule_base
    // {
    // public:
    //     signature_decl(choice4, T0, T1, T2, T3)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3>
    // class choice4i : public rule_base
    // {
    // public:
    //     signature_decl(choice4i, T0, T1, T2, T3)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4>
    // class choice5 : public rule_base
    // {
    // public:
    //     signature_decl(choice5, T0, T1, T2, T3, T4)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         l = sub_match(T4, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4>
    // class choice5i : public rule_base
    // {
    // public:
    //     signature_decl(choice5i, T0, T1, T2, T3, T4)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         l = sub_match(T4, s); if (l) return new lex_choice(l, 4);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    // class choice6 : public rule_base
    // {
    // public:
    //     signature_decl(choice6, T0, T1, T2, T3, T4, T5)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         l = sub_match(T4, s); if (l) return l;
    //         l = sub_match(T5, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    // class choice6i : public rule_base
    // {
    // public:
    //     signature_decl(choice6i, T0, T1, T2, T3, T4, T5)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         l = sub_match(T4, s); if (l) return new lex_choice(l, 4);
    //         l = sub_match(T5, s); if (l) return new lex_choice(l, 5);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    // class choice7 : public rule_base
    // {
    // public:
    //     signature_decl(choice7, T0, T1, T2, T3, T4, T5, T6)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         l = sub_match(T4, s); if (l) return l;
    //         l = sub_match(T5, s); if (l) return l;
    //         l = sub_match(T6, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
    // class choice7i : public rule_base
    // {
    // public:
    //     signature_decl(choice7i, T0, T1, T2, T3, T4, T5, T6)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         l = sub_match(T4, s); if (l) return new lex_choice(l, 4);
    //         l = sub_match(T5, s); if (l) return new lex_choice(l, 5);
    //         l = sub_match(T6, s); if (l) return new lex_choice(l, 6);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    // class choice8 : public rule_base
    // {
    // public:
    //     signature_decl(choice8, T0, T1, T2, T3, T4, T5, T6, T7)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         l = sub_match(T4, s); if (l) return l;
    //         l = sub_match(T5, s); if (l) return l;
    //         l = sub_match(T6, s); if (l) return l;
    //         l = sub_match(T7, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
    // class choice8i : public rule_base
    // {
    // public:
    //     signature_decl(choice8i, T0, T1, T2, T3, T4, T5, T6, T7)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         l = sub_match(T4, s); if (l) return new lex_choice(l, 4);
    //         l = sub_match(T5, s); if (l) return new lex_choice(l, 5);
    //         l = sub_match(T6, s); if (l) return new lex_choice(l, 6);
    //         l = sub_match(T7, s); if (l) return new lex_choice(l, 7);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    // class choice9 : public rule_base
    // {
    // public:
    //     signature_decl(choice9, T0, T1, T2, T3, T4, T5, T6, T7, T8)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         l = sub_match(T4, s); if (l) return l;
    //         l = sub_match(T5, s); if (l) return l;
    //         l = sub_match(T6, s); if (l) return l;
    //         l = sub_match(T7, s); if (l) return l;
    //         l = sub_match(T8, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
    // class choice9i : public rule_base
    // {
    // public:
    //     signature_decl(choice9i, T0, T1, T2, T3, T4, T5, T6, T7, T8)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         l = sub_match(T4, s); if (l) return new lex_choice(l, 4);
    //         l = sub_match(T5, s); if (l) return new lex_choice(l, 5);
    //         l = sub_match(T6, s); if (l) return new lex_choice(l, 6);
    //         l = sub_match(T7, s); if (l) return new lex_choice(l, 7);
    //         l = sub_match(T8, s); if (l) return new lex_choice(l, 8);
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    // class choice10 : public rule_base
    // {
    // public:
    //     signature_decl(choice10, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return l;
    //         l = sub_match(T1, s); if (l) return l;
    //         l = sub_match(T2, s); if (l) return l;
    //         l = sub_match(T3, s); if (l) return l;
    //         l = sub_match(T4, s); if (l) return l;
    //         l = sub_match(T5, s); if (l) return l;
    //         l = sub_match(T6, s); if (l) return l;
    //         l = sub_match(T7, s); if (l) return l;
    //         l = sub_match(T8, s); if (l) return l;
    //         l = sub_match(T9, s); if (l) return l;
    //         return nullptr;
    //     }
    // };

    // template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
    // class choice10i : public rule_base
    // {
    // public:
    //     signature_decl(choice10i, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)

    //     internal_match_method(s)
    //     {
    //         lexeme *l;
    //         l = sub_match(T0, s); if (l) return new lex_choice(l, 0);
    //         l = sub_match(T1, s); if (l) return new lex_choice(l, 1);
    //         l = sub_match(T2, s); if (l) return new lex_choice(l, 2);
    //         l = sub_match(T3, s); if (l) return new lex_choice(l, 3);
    //         l = sub_match(T4, s); if (l) return new lex_choice(l, 4);
    //         l = sub_match(T5, s); if (l) return new lex_choice(l, 5);
    //         l = sub_match(T6, s); if (l) return new lex_choice(l, 6);
    //         l = sub_match(T7, s); if (l) return new lex_choice(l, 7);
    //         l = sub_match(T8, s); if (l) return new lex_choice(l, 8);
    //         l = sub_match(T9, s); if (l) return new lex_choice(l, 9);
    //         return nullptr;
    //     }
    // };

}