#pragma once
#include <cstdint>
#include <vector>
#include <cstring>

namespace ast
{
    //parser of objects of type O

    template <typename O>
    class stream
    {
    public:
        using object = O;
        O operator *()
        {
            return *cur;
        }

        stream &operator ++()
        {
            cur++;
        }

        stream operator ++(int)
        {
            stream s = *this;
            cur++;
            return s;
        }

        stream &operator +=(size_t n)
        {
            cur += n;
            return *this;
        }

        O *cur = nullptr;
    };

    template <typename O>
    class lexeme
    {
    public:
        using object = O;
        lexeme()
        {
            memset(this, 0, sizeof(lexeme));
        }

        ~lexeme()
        {
            invalidate();
        }

        void (*invalidate_cb)(lexeme<O> *l) = nullptr;

        struct pascal_string
        {
            uint8_t size;
            char *str;
        };

        char type = 0;
            // 0 invalid
            // any of the single char variables below => the value is in that variable
            // any value > 127 => the value is user defined
        union
        {
            object o;                 // type 'o'
            std::vector<object> *v;   // type 'v' //used by token_repeat
            std::vector<lexeme*> *V;  // type 'V' //used by sequence and repeat
            lexeme *lex;              // type '0' to '9' //used by choice
            char c;
            int8_t b;
            uint8_t B;
            bool qm; //question mark character '?'
            int16_t h;
            uint16_t H;
            int32_t i;
            uint32_t I;
            int32_t l;
            uint32_t L;
            int64_t q;
            uint64_t Q;
            int32_t n;
            uint32_t N;
            float e;
            float f;
            double d;
            char *s;
            pascal_string *p;
            void *P;
        };

        void invalidate()
        {
            if (type == 'v' && v)
            {
                delete v;
            }
            else if (type == 'V' && V)
            {
                for (auto lex : *V)
                    if (lex)
                        delete lex;
                delete V;
            }
            else if (type >= '0' && type <= '9' && lex)
            {
                delete lex;
            }
            else if (type == 's' && s)
            {
                delete[] s;
            }
            else if (type == 'p' && p)
            {
                delete p->str;
                delete p;
            }
            else if ((type > 127 || type == 'P') && invalidate_cb)
            {
                invalidate_cb(this);
            }
            type = 0;
        }
    };

    template<typename O>
    class noop_fire_fn
    {
    public:
        static inline lexeme<O> *fire(lexeme<O> *l)
        {
            return l;
        }
    };

    template<typename O, int n>
    class select_fire_fn
    {
    public:
        static inline lexeme<O> *fire(lexeme<O> *l)
        {
            if (l->type == 'V' && l->V->size() > n)
            {
                lexeme<O> *tmp = l->V->at(n);
                l->V->at(n) = nullptr;
                delete l;
                return tmp;
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };

    template<typename O>
    class choice_fire_fn
    {
    public:
        static inline lexeme<O> *fire(lexeme<O> *l)
        {
            if (l->type >= '0' && l->type <= '9')
            {
                lexeme<O> *tmp = l->lex;
                l->lex = nullptr;
                delete l;
                return tmp;
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };

    template<typename O>
    class no_match
    {
    public:
        using object = O;

        static inline lexeme<O> *fire(lexeme<O> *l)
        {
            if (l)
            delete l;
            return nullptr;
        }

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            return nullptr;
        }
    };

    template<typename O, O value, typename F = noop_fire_fn<O>>
    class token
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            lexeme<O> *l = new lexeme<O>();
            if (*s == value)
            {
                l->type = 'o';
                l->o = value;
                s++;
                return F::fire(l);
            }
            return nullptr;
        }
    };

    template<typename O, O start, O end, typename F = noop_fire_fn<O>>
    class token_range
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            lexeme<O> *l = new lexeme<O>();
            if (*s >= start && *s <= end)
            {
                l->type = 'o';
                l->o = *s;
                s++;
                return F::fire(l);
            }
            return nullptr;
        }
    };

    template<typename O, typename A, typename F = noop_fire_fn<O>>
    class token_array
    {
    public:
        using object = O;
        static constexpr auto arr = A::arr;
        static constexpr int size = A::size;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            lexeme<O> *l = new lexeme<O>();
            for (size_t i = 0; i < size; i++)
            {
                if (*s == arr[i])
                {
                    l->type = 'o';
                    l->o = *s;
                    s++;
                    return F::fire(l);
                }
            }
            return nullptr;
        }
    };

    template<typename T0, int min = 0, int max = -1, typename F = noop_fire_fn<typename T0::object>>
    class token_repeat
    {
    public:
        using object = typename T0::object;
        template <typename S>
        static inline lexeme<object> *match(S &s)
        {
            lexeme<object> *l = new lexeme<object>();
            l->type = 'v';
            l->v = new std::vector<object>();
            S s_backup = s;
            int count = 0;

            while (max == -1 || count < max)
            {
                auto li = T0::match(s);
                if (li != nullptr)
                {
                    if (li->type == 'o' )
                    {
                        l->v->push_back(li->o);
                        count++;
                        delete li;
                    }
                    else if (li->type == 'v')
                    {
                        for (auto lii : *li->v)
                        {
                            l->v->push_back(lii);
                            count++;
                        }
                        delete li;
                    }
                    else
                    {
                        delete li;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
            if (count >= min)
            {
                return F::fire(l);
            }
            s = s_backup;
            delete l;
            return nullptr;
        }
    };

    template<typename T0, typename F = noop_fire_fn<typename T0::object>>
    using token_optional = token_repeat<T0, 0, 1, F>;
    template<typename T0, typename F = noop_fire_fn<typename T0::object>>
    using token_zeroOrMore = token_repeat<T0, 0, -1, F>;
    template<typename T0, typename F = noop_fire_fn<typename T0::object>>
    using token_oneOrMore = token_repeat<T0, 1, -1, F>;

    class void_param
    {
    public:
        using object = void;
    };

    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F = noop_fire_fn<typename T0::object>>
    class sequence7
    {
    public:
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value || std::is_same<T2, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value || std::is_same<T3, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value || std::is_same<T4, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T5::object>::value || std::is_same<T5, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T6::object>::value || std::is_same<T6, void_param>::value, "choice types must have the same object type");

        using object = typename T0::object;

        template <typename S>
        static inline lexeme<object> *match(S &s)
        {
            lexeme<object> *l = new lexeme<object>();
            l->type = 'V';
            l->V = new std::vector<lexeme<object>*>();
            S s_backup = s;

            auto l0 = T0::match(s);
            if (l0) { l->V->push_back(l0); }
            else { s = s_backup; delete l; return nullptr; }

            auto l1 = T1::match(s);
            if (l1) { l->V->push_back(l1); }
            else { s = s_backup; delete l; return nullptr; }

            if constexpr (!std::is_same<T2, void_param>::value)
            {
                auto l2 = T2::match(s);
                if (l2) { l->V->push_back(l2); }
                else { s = s_backup; delete l; return nullptr; }
            }

            if constexpr (!std::is_same<T3, void_param>::value)
            {
                auto l3 = T3::match(s);
                if (l3) { l->V->push_back(l3); }
                else { s = s_backup; delete l; return nullptr; }
            }

            if constexpr (!std::is_same<T4, void_param>::value)
            {
                auto l4 = T4::match(s);
                if (l4) { l->V->push_back(l4); }
                else { s = s_backup; delete l; return nullptr; }
            }

            if constexpr (!std::is_same<T5, void_param>::value)
            {
                auto l5 = T5::match(s);
                if (l5) { l->V->push_back(l5); }
                else { s = s_backup; delete l; return nullptr; }
            }

            if constexpr (!std::is_same<T6, void_param>::value)
            {
                auto l6 = T6::match(s);
                if (l6) { l->V->push_back(l6); }
                else { s = s_backup; delete l; return nullptr; }
            }

            return F::fire(l);
        }
    };

    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F = noop_fire_fn<typename T0::object>>
    using sequence_base = sequence7<T0, T1, T2, T3, T4, T5, T6, F>;

    template<typename T0, typename T1, typename F = noop_fire_fn<typename T0::object>>
    using sequence2 = sequence_base<T0, T1, void_param, void_param, void_param, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename F = noop_fire_fn<typename T0::object>>
    using sequence3 = sequence_base<T0, T1, T2, void_param, void_param, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename T3, typename F = noop_fire_fn<typename T0::object>>
    using sequence4 = sequence_base<T0, T1, T2, T3, void_param, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename F = noop_fire_fn<typename T0::object>>
    using sequence5 = sequence_base<T0, T1, T2, T3, T4, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F = noop_fire_fn<typename T0::object>>
    using sequence6 = sequence_base<T0, T1, T2, T3, T4, T5, void_param, F>;

    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F = choice_fire_fn<typename T0::object>>
    class choice7
    {
    public:
        using object = typename T0::object;
        static_assert(std::is_same<typename T0::object, typename T1::object>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T2::object>::value || std::is_same<T2, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T3::object>::value || std::is_same<T3, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T4::object>::value || std::is_same<T4, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T5::object>::value || std::is_same<T5, void_param>::value, "choice types must have the same object type");
        static_assert(std::is_same<typename T0::object, typename T6::object>::value || std::is_same<T6, void_param>::value, "choice types must have the same object type");

        template <typename S>
        static inline lexeme<object> *match(S &s)
        {
            lexeme<object> *l = new lexeme<object>();
            S s_backup = s;

            auto l0 = T0::match(s);
            if (l0)
            {
                l->type = '0';
                l->lex = l0;
                return F::fire(l);
            }

            auto l1 = T1::match(s);
            if (l1)
            {
                l->type = '1';
                l->lex = l1;
                return F::fire(l);
            }

            if constexpr (!std::is_same<T2, void_param>::value)
            {
                auto l2 = T2::match(s);
                if (l2)
                {
                    l->type = '2';
                    l->lex = l2;
                    return F::fire(l);
                }
            }

            if constexpr (!std::is_same<T3, void_param>::value)
            {
                auto l3 = T3::match(s);
                if (l3)
                {
                    l->type = '3';
                    l->lex = l3;
                    return F::fire(l);
                }
            }

            if constexpr (!std::is_same<T4, void_param>::value)
            {
                auto l4 = T4::match(s);
                if (l4)
                {
                    l->type = '4';
                    l->lex = l4;
                    return F::fire(l);
                }
            }

            if constexpr (!std::is_same<T5, void_param>::value)
            {
                auto l5 = T5::match(s);
                if (l5)
                {
                    l->type = '5';
                    l->lex = l5;
                    return F::fire(l);
                }
            }

            if constexpr (!std::is_same<T6, void_param>::value)
            {
                auto l6 = T6::match(s);
                if (l6)
                {
                    l->type = '6';
                    l->lex = l6;
                    return F::fire(l);
                }
            }

            s = s_backup;
            return nullptr;
        }
    };

    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F = choice_fire_fn<typename T0::object>>
    using choice_base = choice7<T0, T1, T2, T3, T4, T5, T6, F>;

    template<typename T0, typename T1, typename F = choice_fire_fn<typename T0::object>>
    using choice2 = choice_base<T0, T1, void_param, void_param, void_param, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename F = choice_fire_fn<typename T0::object>>
    using choice3 = choice_base<T0, T1, T2, void_param, void_param, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename T3, typename F = choice_fire_fn<typename T0::object>>
    using choice4 = choice_base<T0, T1, T2, T3, void_param, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename F = choice_fire_fn<typename T0::object>>
    using choice5 = choice_base<T0, T1, T2, T3, T4, void_param, void_param, F>;

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename F = choice_fire_fn<typename T0::object>>
    using choice6 = choice_base<T0, T1, T2, T3, T4, T5, void_param, F>;

    template<typename T0, int min = 0, int max = -1, typename F = noop_fire_fn<typename T0::object>>
    class repeat
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline lexeme<object> *match(S &s)
        {
            lexeme<object> *l = new lexeme<object>();
            l->type = 'V';
            l->V = new std::vector<lexeme<object>*>();
            S s_backup = s;
            int count = 0;

            while (max == -1 || count < max)
            {
                auto li = T0::match(s);
                if (li)
                {
                    l->V->push_back(li);
                    count++;
                }
                else
                {
                    break;
                }
            }
            if (count >= min)
            {
                return F::fire(l);
            }
            s = s_backup;
            delete l;
            return nullptr;
        }
    };

    template<typename T0, typename F = noop_fire_fn<typename T0::object>>
    using optional = repeat<T0, 0, 1, F>;

    template<typename T0, typename F = noop_fire_fn<typename T0::object>>
    using zeroOrMore = repeat<T0, 0, -1, F>;

    template<typename T0, typename F = noop_fire_fn<typename T0::object>>
    using oneOrMore = repeat<T0, 1, -1, F>;

}