#pragma once

#include "deps.h"
#include "helpers.h"

//fundamental building blocks

namespace ast::_b
{
    template <typename O>
    class stream
    {
    public:
        using object = O;

        stream(O *cur) : cur(cur) {}

        inline O &operator *()
        {
            return *cur;
        }

        inline const O &operator *() const
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
            return !cur || !(*cur);
        }

        inline bool valid() const
        {
            return cur != nullptr;
        }

        //return nullptr if the stream is not able to backtrack
        inline void *snapshot()
        {
            return cur;
        }

        //close the stream if we can't backtrack
        inline bool restore(void *snapshot)
        {
            cur = (O *)snapshot;
            return eof();
        }

        O *cur = nullptr;
    };

    template <typename O>
    class nb_stream : public stream<O> //non-backtrackable stream
    {
    public:
        //close the stream if we have to backtrack
        inline bool restore(void *snapshot)
        {
            if (snapshot == cur)
                return true;
            cur = nullptr;
            return eof();
        }
    };

    template <typename S>
    std::enable_if<std::is_pointer_v<S>, bool> default_eof(S &s)
    {
        return !s || !(*s);
    }

    template <typename S>
    std::enable_if<std::is_pointer_v<S>, void *> default_snapshot(S &s)
    {
        return s;
    }

    template <typename S>
    std::enable_if<std::is_pointer_v<S>, bool> default_restore(S &s, void *snapshot)
    {
        s = (decltype(s))snapshot;
        return !s || !(*s);
    }

    optional_call_decl(stream_eof, bool, eof, default_eof)
    optional_call_decl(stream_snapshot, void *, snapshot, default_snapshot)
    optional_call_decl_1(stream_restore, bool, restore, default_restore, void *)

    template <typename O>
    class lexeme
    {
    public:
        using object = O;
        using o_t = typename std::remove_const_t<O>;
        using v_t = std::vector<o_t>;
        using V_t = std::vector<lexeme<O> *>;

        lexeme(char type)
        {
            memset(this, 0, sizeof(lexeme));
            this->type = type;
            switch (type)
            {
            case 'v':
                v = new_v();
                break;
            case 'V':
                V = new_V();
                break;
            }
        }

        lexeme()
        {
            memset(this, 0, sizeof(lexeme));
        }

        lexeme(lexeme *l, char type = '*')
        {
            memset(this, 0, sizeof(lexeme));
            this->type = type;
            lex = l;
        }

        ~lexeme()
        {
            invalidate();
        }

        void (*invalidate_cb)(lexeme<O> *l) = nullptr;
        
        static inline v_t *new_v(size_t size)
        {
            return new v_t(size);
        }
        
        static inline v_t *new_v()
        {
            return new v_t();
        }

        static inline lexeme<O> *new_lexeme()
        {
            return new lexeme<O>();
        }

        static inline std::vector<lexeme<O>*> *new_V()
        {
            return new std::vector<lexeme<O>*>();
        }

        bool append(O value)
        {
            if (type == 'o')
            {
                if (o == 0)
                {
                    o = value;
                }
                else
                {
                    type = 'v';
                    v = new_v();
                    v->push_back(o);
                    v->push_back(value);
                }
            }
            else if (type == 'v')
            {
                v->push_back(value);
            }
            else
            {
                return false;
            }
            return true;
        }

        bool append(lexeme<O> *l)
        {
            if (!l)
                return false;
            
            if (type == 'V')
            {
                V->push_back(l);
            }
            else if (type == '*' || type >= '0' && type <= '9')
            {
                type = 'V';
                V = new_V();
                V->push_back(lex);
                V->push_back(l);
            }
            else if (type == 'v' && l->type == 'v')
            {
                v->insert(v->end(), l->v->begin(), l->v->end());
            }
            else
            {
                return false;
            }
            return true;
        }

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
            o_t o;                      // type 'o'
            v_t *v;                     // type 'v' //used by token_repeat
            V_t *V;                     // type 'V' //used by sequence and repeat
            lexeme *lex;                // type '0' to '9', used by choice, or '*', as a reference to another lexeme
            char c;
            int8_t b;
            uint8_t B;
            bool qm;                    //question mark character '?'
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
}