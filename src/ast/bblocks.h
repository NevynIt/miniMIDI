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
        lexeme(char type)
        {
            memset(this, 0, sizeof(lexeme));
            this->type = type;
            switch (type)
            {
            case 'v':
                v = new std::vector<O>();
                break;
            case 'V':
                V = new std::vector<lexeme<O>*>();
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
                    v = new std::vector<O>();
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
                V = new std::vector<lexeme<O>*>();
                V->push_back(lex);
                V->push_back(l);
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
            object o;                 // type 'o'
            std::vector<object> *v;   // type 'v' //used by token_repeat
            std::vector<lexeme*> *V;  // type 'V' //used by sequence and repeat
            lexeme *lex;              // type '0' to '9', used by choice, or '*', as a reference to another lexeme
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
}