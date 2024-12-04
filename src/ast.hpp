#pragma once
#include <cstdint>
#include <vector>
#include <cstring>
#include <type_traits>

namespace ast
{
    //parser of objects of type O

    // Helper trait to detect if T has an eof() method
    template<typename T, typename = void>
    struct has_eof_method : std::false_type {};

    template<typename T>
    struct has_eof_method<T, std::void_t<decltype(std::declval<T>().eof())>> : std::true_type {};

    // Overload when T has an eof() method
    template <typename T>
    std::enable_if_t<has_eof_method<T>::value, bool>
    call_eof(const T& obj)
    {
        return obj.eof();
    }

    // Overload when T does not have an eof() method
    template <typename T>
    std::enable_if_t<!has_eof_method<T>::value, bool>
    call_eof(const T& obj)
    {
        return *obj == T(0);
    }

    class void_param
    {
    public:
        using object = void;
    };

    template <typename O>
    class stream
    {
    public:
        using object = O;

        inline O operator *()
        {
            return *cur;
        }

        inline stream &operator ++()
        {
            cur++;
        }

        inline stream operator ++(int)
        {
            stream s = *this;
            cur++;
            return s;
        }

        inline stream &operator +=(size_t n)
        {
            cur += n;
            return *this;
        }

        inline bool eof() const
        {
            return *cur == 0;
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
            int32_t l;
            uint32_t L;
            int64_t q;
            uint64_t Q;
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
    class noop_fn
    {
    public:
        static inline lexeme<O> *refine(lexeme<O> *l)
        {
            return l;
        }
    };

    template<typename O, int n>
    class select_fn
    {
    public:
        static inline lexeme<O> *refine(lexeme<O> *l)
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
    class choice_fn
    {
    public:
        static inline lexeme<O> *refine(lexeme<O> *l)
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
    class concat_fn
    {
    public:
        static inline bool append(lexeme<O> *l, std::vector<O> *v)
        {
            if (l->type == 'o')
            {
                v->push_back(l->o);
            }
            else if (l->type == 'v')
            {
                for (auto li : *l->v)
                {
                    v->push_back(li);
                }
            }
            else if (l->type == 'V')
            {
                for (auto li : *l->V)
                {
                    if (!append(li, v))
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
            return true;
        }

        static inline lexeme<O> *refine(lexeme<O> *l)
        {
            lexeme<O> *tmp = new lexeme<O>();
            tmp->type = 'v';
            tmp->v = new std::vector<O>();
            if (append(l, tmp->v))
            {
                delete l;
                return tmp;
            }
            else
            {
                delete l;
                delete tmp;
                return nullptr;
            }
        }
    };

    template<typename O>
    class no_match
    {
    public:
        using object = O;

        static inline lexeme<O> *refine(lexeme<O> *l)
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

    template<typename O, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class refinery
    {
    public:
        using object = O;

        static inline lexeme<O> *refine(lexeme<O> *l)
        {
            if constexpr (!std::is_same<F0, void_param>::value)
                if (l)
                    l = F0::refine(l);

            if constexpr (!std::is_same<F1, void_param>::value)
                if (l)
                    l = F1::refine(l);

            if constexpr (!std::is_same<F2, void_param>::value)
                if (l)
                    l = F2::refine(l);

            if constexpr (!std::is_same<F3, void_param>::value)
                if (l)
                    l = F3::refine(l);

            return l;
        }
    };

    template<typename O, O value, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class token : public refinery<O, F0, F1, F2, F3>
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            if (call_eof(s))
                return nullptr;

            if (*s == value)
            {
                lexeme<O> *l = new lexeme<O>();
                l->type = 'o';
                l->o = value;
                s++;
                return refinery<O, F0, F1, F2, F3>::refine(l);
            }
            return nullptr;
        }
    };

    template<typename O, O start, O end, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class token_range : public refinery<O, F0, F1, F2, F3>
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            if (call_eof(s))
                return nullptr;

            if (*s >= start && *s <= end)
            {
                lexeme<O> *l = new lexeme<O>();
                l->type = 'o';
                l->o = *s;
                s++;
                return refinery<O, F0, F1, F2, F3>::refine(l);
            }
            return nullptr;
        }
    };

    template <typename O, std::size_t N>
    static constexpr std::size_t getSize(const O (&arr)[N])
    {
        return N;
    }

    #define token_array_decl(_TYPE_, _NAME_) inline constexpr _TYPE_ _NAME_[]
    #define token_array(arr) (arr), ast::getSize(arr)

    template<typename O, const O *arr, std::size_t size, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class token_choice : public refinery<O, F0, F1, F2, F3>
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            if (call_eof(s))
                return nullptr;

            lexeme<O> *l = new lexeme<O>();
            for (size_t i = 0; i < size; i++)
            {
                if (*s == arr[i])
                {
                    l->type = 'o';
                    l->o = *s;
                    s++;
                    return refinery<O, F0, F1, F2, F3>::refine(l);
                }
            }
            delete l;
            return nullptr;
        }
    };

    template<typename O, const O *arr, std::size_t size, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class token_string : public refinery<O, F0, F1, F2, F3>
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            if (call_eof(s))
                return nullptr;

            lexeme<O> *l = new lexeme<O>();
            l->type = 'v';
            l->v = new std::vector<O>(size);
            for (size_t i = 0; arr[i] != 0; i++)
            {
                if (call_eof(s))
                {
                    delete l;
                    return nullptr;
                }
                else if (*s == arr[i])
                {
                    l->v->at(i) = *s;
                    s++;
                }
                else
                {
                    delete l;
                    return nullptr;
                }
            }
            return refinery<O, F0, F1, F2, F3>::refine(l);
        }
    };

    //make the escape equal to the delimiter to disable it. escape skips the delimiter only, otherwise is passed as literal, so that Fx can handle it
    template<typename O, O delimiter, O escape = delimiter, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class token_delimited : refinery<O, F0, F1, F2, F3>
    {
    public:
        using object = O;

        template <typename S>
        static inline lexeme<O> *match(S &s)
        {
            if (call_eof(s) || *s != delimiter)
                return nullptr;
            s++;

            lexeme<O> *l = new lexeme<O>();
            l->type = 'v';
            l->v = new std::vector<O>();
            while (!call_eof(s) && *s != delimiter)
            {
                if (*s == escape)
                {
                    s++;
                    if (call_eof(s))
                    {
                        delete l;
                        return nullptr;
                    }
                    if (*s != delimiter)
                    {
                        l->v->push_back(escape);
                    }
                }
                l->v->push_back(*s);
                s++;
            }
            if (*s == delimiter)
            {
                s++;
                return refinery<O, F0, F1, F2, F3>::refine(l);
            }
            else
            {
                delete l;
                return nullptr;
            }
        }
    };

    template<typename T0, typename F0, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param, typename F4 = void_param, typename F5 = void_param, typename F6 = void_param>
    class decorator
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline lexeme<object> *match(S &s)
        {
            lexeme<object> *l = T0::match(s);
            if (l)
                l = F0::refine(l);

            if constexpr (!std::is_same<F1, void_param>::value)
                if (l)
                    l = F1::refine(l);

            if constexpr (!std::is_same<F2, void_param>::value)
                if (l)
                    l = F2::refine(l);
            
            if constexpr (!std::is_same<F3, void_param>::value)
                if (l)
                    l = F3::refine(l);
            
            if constexpr (!std::is_same<F4, void_param>::value)
                if (l)
                    l = F4::refine(l);
            
            if constexpr (!std::is_same<F5, void_param>::value)
                if (l)
                    l = F5::refine(l);
            
            if constexpr (!std::is_same<F6, void_param>::value)
                if (l)
                    l = F6::refine(l);
            
            return l;
        }
    };

    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class sequence7 : public refinery<typename T0::object, F0, F1, F2, F3>
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
            if (call_eof(s))
                return nullptr;

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

            return refinery<object, F0, F1, F2, F3>::refine(l);
        }
    };

    template<typename T0, typename T1, typename T2 = void_param, typename T3 = void_param, typename T4 = void_param, typename T5 = void_param, typename T6 = void_param, typename F0 = choice_fn<typename T0::object>, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class choice7 : public refinery<typename T0::object, F0, F1, F2, F3>
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
            if (call_eof(s))
                return nullptr;

            lexeme<object> *l = new lexeme<object>();
            S s_backup = s;

            auto l0 = T0::match(s);
            if (l0)
            {
                l->type = '0';
                l->lex = l0;
                return refinery<object, F0, F1, F2, F3>::refine(l);
            }

            auto l1 = T1::match(s);
            if (l1)
            {
                l->type = '1';
                l->lex = l1;
                return refinery<object, F0, F1, F2, F3>::refine(l);
            }

            if constexpr (!std::is_same<T2, void_param>::value)
            {
                auto l2 = T2::match(s);
                if (l2)
                {
                    l->type = '2';
                    l->lex = l2;
                    return refinery<object, F0, F1, F2, F3>::refine(l);
                }
            }

            if constexpr (!std::is_same<T3, void_param>::value)
            {
                auto l3 = T3::match(s);
                if (l3)
                {
                    l->type = '3';
                    l->lex = l3;
                    return refinery<object, F0, F1, F2, F3>::refine(l);
                }
            }

            if constexpr (!std::is_same<T4, void_param>::value)
            {
                auto l4 = T4::match(s);
                if (l4)
                {
                    l->type = '4';
                    l->lex = l4;
                    return refinery<object, F0, F1, F2, F3>::refine(l);
                }
            }

            if constexpr (!std::is_same<T5, void_param>::value)
            {
                auto l5 = T5::match(s);
                if (l5)
                {
                    l->type = '5';
                    l->lex = l5;
                    return refinery<object, F0, F1, F2, F3>::refine(l);
                }
            }

            if constexpr (!std::is_same<T6, void_param>::value)
            {
                auto l6 = T6::match(s);
                if (l6)
                {
                    l->type = '6';
                    l->lex = l6;
                    return refinery<object, F0, F1, F2, F3>::refine(l);
                }
            }
            
            delete l;
            s = s_backup;
            return nullptr;
        }
    };

    template<typename T0, int min = 0, int max = -1, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    class repeat : public refinery<typename T0::object, F0, F1, F2, F3>
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

            while ((max == -1 || count < max) && !call_eof(s))
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
                return refinery<object, F0, F1, F2, F3>::refine(l);
            }
            s = s_backup;
            delete l;
            return nullptr;
        }
    };

    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using optional = repeat<T0, 0, 1, F0, F1, F2, F3>;

    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using zeroOrMore = repeat<T0, 0, -1, F0, F1, F2, F3>;

    template<typename T0, typename F0 = void_param, typename F1 = void_param, typename F2 = void_param, typename F3 = void_param>
    using oneOrMore = repeat<T0, 1, -1, F0, F1, F2, F3>;

}
