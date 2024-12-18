#pragma once

#include "deps.h"
#include "helpers.h"

//fundamental building blocks

namespace ast::_b
{
    using namespace ast::_h;
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

        inline size_t operator -(const stream &s) const
        {
            return cur - s.cur;
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
            if (snapshot == this->cur)
                return true;
            this->cur = nullptr;
            return this->eof();
        }
    };

    //lexemes print as:
    //(rule) signature: value
    class lexeme : public uti::variant
    {
    public:
        set_signature<ast_str("lexeme")>;
        variant_implementation
        uti::signature_id rule = nullptr;

        virtual void printvalue(int indent = 0) const { printf("??"); };
        virtual void print(int indent = 0) const
        {
            print_ind(indent, "(");
            if (rule)
                rule(); //print the signature of the rule
            printf(") ");
            signature::print(); //print the signature of the lexeme
            printf(": ");
            printvalue(indent);
        }
    };
    
    template<typename O>
    class lex_o : public lexeme
    {
    public:
        set_signature<ast_str("lex_o"), sig_of(O)>;
        variant_inherit(lexeme)
        using object = O;
        O o;
        void printvalue(int indent = 0) const override;

        lex_o() {}
        lex_o(O o) : o(o) {}
    };

    template<>
    inline void lex_o<char>::printvalue(int indent) const
    {
        printf("'%c'", o);
    }

    template<>
    inline void lex_o<char const>::printvalue(int indent) const
    {
        printf("'%c'", o);
    }

    template<typename O>
    class lex_v : public lexeme, public std::vector<O>
    {
    public:
        using object = O;
        set_signature<ast_str("lex_v"), sig_of(O)>;
        variant_inherit(lexeme)
        
        void printvalue(int indent = 0) const override;
    };

    template <>
    inline void lex_v<char>::printvalue(int indent) const
    {
        printf("\"");
        for (auto i = this->begin(); i != this->end(); i++)
        {
            printf("%c", *i);
        }
        printf("\"");
    }

    template<typename O>
    inline void lex_v<O>::printvalue(int indent) const
    {
        printf("{\n");
        for (auto i = this->begin(); i != this->end(); i++)
        {
            lex_o<O>(*i).print(indent + 1);
            if (i + 1 != this->end())
                printf(",\n");
        }
        printf("\n");
        print_ind(indent, "}");
    }

    class lex_V : public lexeme, public std::vector<lexeme *>
    {
    public:
        set_signature<ast_str("lex_V")>;
        variant_inherit(lexeme)

        lex_V() {}
        
        lex_V(const lex_V &V) : std::vector<lexeme *>(V.size())
        {
            for (size_t i = 0; i < V.size(); i++)
                (*this)[i] = (lexeme *)V[i]->clone();
        }

        void printvalue(int indent = 0) const override
        {
            if (this->size() == 0)
            {
                printf("{}");
                return;
            }
            printf("{\n");
            for (auto i = this->begin(); i != this->end(); i++)
            {
                (*i)->print(indent + 1);
                if (i + 1 != this->end())
                    printf(",\n");
            }
            printf("\n");
            print_ind(indent, "} (");
            if (rule)
                rule(); //print the signature of the rule
            printf(")");
        }

        ~lex_V()
        {
            for (auto lex : *this)
                if (lex)
                    delete lex;
        }
    };

    class lex_l : public lexeme
    {
    public:
        set_signature<ast_str("lex_l")>;
        variant_inherit(lexeme)
        int32_t l = 0;
        
        lex_l(long l = 0) : l(l) {}

        void printvalue(int indent = 0) const override
        {
            print_ind(indent, "%ld", l);
        }
    };

    class lex_f : public lexeme
    {
    public:
        set_signature<ast_str("lex_f")>;
        variant_inherit(lexeme)
        float f = 0.0;

        lex_f(float f = 0.0) : f(f) {}

        void printvalue(int indent = 0) const override
        {
            print_ind(indent, "%f", f);
        }
    };

    class lex_s : public lexeme
    {
    public:
        set_signature<ast_str("lex_s")>;
        variant_inherit(lexeme)
        char *s = nullptr;

        lex_s(char *s = nullptr) : s(s) {}
        ~lex_s() { if(s) delete[] s; }

        void print(int indent = 0) const override
        {
            print_ind(indent, "\"%s\"", s);
        }
    };

    class lex_p : public lex_s
    {
    public:
        set_signature<ast_str("lex_s")>;
        variant_inherit(lex_s)

        lex_p(char *s = nullptr, int len = 0) : lex_s(s), len(len) {}
        int len = 0;

        void print(int indent = 0) const override
        {
            print_ind(indent, "'%.*s'", len, s);
        }
    };

    class rule_base : public uti::variant
    {
    public:
        set_signature<ast_str("rule_base")>;
        variant_implementation

        static constexpr bool _ast_internal_rule_ = false;
    };


    class rule_base_internal : public rule_base
    {
    public:
        set_signature<ast_str("rule_base_internal")>;
        variant_inherit(rule_base)

        static constexpr bool _ast_internal_rule_ = true;
    };

            // char c;
            // int8_t b;
            // uint8_t B;
            // bool qm;                    //question mark character '?'
            // int16_t h;
            // uint16_t H;
            // int32_t l;
            // uint32_t L;
            // int64_t q;
            // uint64_t Q;
            // float f;
            // double d;
            // char *s;
            // pascal_string *p;
            // void *P;


    // template <typename O>
    // class lexeme_old
    // {
    // public:
    //     using object = O;
    //     using o_t = typename std::remove_const_t<O>;
    //     using v_t = std::vector<o_t>;
    //     using V_t = std::vector<lexeme<O> *>;

    //     lexeme(char type)
    //     {
    //         memset(this, 0, sizeof(lexeme));
    //         this->type = type;
    //         switch (type)
    //         {
    //         case 'v':
    //             v = new_v();
    //             break;
    //         case 'V':
    //             V = new_V();
    //             break;
    //         }
    //     }

    //     lexeme()
    //     {
    //         memset(this, 0, sizeof(lexeme));
    //     }

    //     lexeme(lexeme *l, char type = '*')
    //     {
    //         memset(this, 0, sizeof(lexeme));
    //         this->type = type;
    //         lex = l;
    //     }

    //     ~lexeme()
    //     {
    //         invalidate();
    //     }

    //     void (*invalidate_cb)(lexeme<O> *l) = nullptr;
        
    //     static inline v_t *new_v(size_t size)
    //     {
    //         return new v_t(size);
    //     }
        
    //     static inline v_t *new_v()
    //     {
    //         return new v_t();
    //     }

    //     static inline lexeme<O> *new_lexeme()
    //     {
    //         return new lexeme<O>();
    //     }

    //     static inline std::vector<lexeme<O>*> *new_V()
    //     {
    //         return new std::vector<lexeme<O>*>();
    //     }

    //     bool append(O value)
    //     {
    //         if (type == 'o')
    //         {
    //             if (o == 0)
    //             {
    //                 o = value;
    //             }
    //             else
    //             {
    //                 type = 'v';
    //                 v = new_v();
    //                 v->push_back(o);
    //                 v->push_back(value);
    //             }
    //         }
    //         else if (type == 'v')
    //         {
    //             v->push_back(value);
    //         }
    //         else
    //         {
    //             return false;
    //         }
    //         return true;
    //     }

    //     bool append(lexeme<O> *l)
    //     {
    //         if (!l)
    //             return false;
            
    //         if (type == 'V')
    //         {
    //             V->push_back(l);
    //         }
    //         else if (type == '*' || type >= '0' && type <= '9')
    //         {
    //             type = 'V';
    //             V = new_V();
    //             V->push_back(lex);
    //             V->push_back(l);
    //         }
    //         else if (type == 'v' && l->type == 'v')
    //         {
    //             v->insert(v->end(), l->v->begin(), l->v->end());
    //         }
    //         else
    //         {
    //             return false;
    //         }
    //         return true;
    //     }

    //     struct pascal_string
    //     {
    //         uint8_t size;
    //         char *str;
    //     };

    //     char type = 0;
    //         // 0 invalid
    //         // any of the single char variables below => the value is in that variable
    //         // any value > 127 => the value is user defined
    //     union
    //     {
    //         o_t o;                      // type 'o'
    //         v_t *v;                     // type 'v' //used by token_repeat
    //         V_t *V;                     // type 'V' //used by sequence and repeat
    //         lexeme *lex;                // type '0' to '9', used by choice, or '*', as a reference to another lexeme
    //         char c;
    //         int8_t b;
    //         uint8_t B;
    //         bool qm;                    //question mark character '?'
    //         int16_t h;
    //         uint16_t H;
    //         int32_t l;
    //         uint32_t L;
    //         int64_t q;
    //         uint64_t Q;
    //         float f;
    //         double d;
    //         char *s;
    //         pascal_string *p;
    //         void *P;
    //     };

    //     void invalidate()
    //     {
    //         if (type == 'v' && v)
    //         {
    //             delete v;
    //         }
    //         else if (type == 'V' && V)
    //         {
    //             for (auto lex : *V)
    //                 if (lex)
    //                     delete lex;
    //             delete V;
    //         }
    //         else if (type >= '0' && type <= '9' && lex)
    //         {
    //             delete lex;
    //         }
    //         else if (type == 's' && s)
    //         {
    //             delete[] s;
    //         }
    //         else if (type == 'p' && p)
    //         {
    //             delete p->str;
    //             delete p;
    //         }
    //         else if ((type > 127 || type == 'P') && invalidate_cb)
    //         {
    //             invalidate_cb(this);
    //         }
    //         type = 0;
    //     }
    // };

    // template <typename O>
    // void print_lexeme_object(O o)
    // {
    //     printf("??");
    // }

    // template <>
    // void print_lexeme_object(char o)
    // {
    //     printf("'%c'", o);
    // }

    // template <>
    // void print_lexeme_object(const char *o)
    // {
    //     printf("\"%s\"", o);
    // }

    // template <typename O>
    // void print_lexeme_array(std::vector<O> *v)
    // {
    //         printf("[");
    //         for (auto i = v->begin(); i != v->end(); i++)
    //         {
    //             print_lexeme_object(*i);
    //             if (i + 1 != v->end())
    //                 printf(", ");
    //         }
    //         printf("]");
    // }
    
    // template <>
    // void print_lexeme_array(std::vector<char> *v)
    // {
    //     printf("\"");
    //     for (auto i = v->begin(); i != v->end(); i++)
    //     {
    //         printf("%c", *i);
    //     }
    //     printf("\"");
    // }


    // #define _indent_ printf("%*s", indent, "")

    // template <typename O>
    // void print_lexeme(lexeme<O> *l, int indent)
    // {
    //     if (!l)
    //     {
    //         _indent_;
    //         printf("nullptr");
    //         return;
    //     }
    //     switch (l->type)
    //     {
    //     case 'o':
    //         _indent_;
    //         print_lexeme_object(l->o);
    //         break;
    //     case 'v':
    //         _indent_;
    //         print_lexeme_array(l->v);
    //         break;
    //     case 'V':
    //         _indent_;
    //         printf("{\n");
    //         for (auto i = l->V->begin(); i != l->V->end(); i++)
    //         {
    //             print_lexeme(*i, indent + 1);
    //             if (i + 1 != l->V->end())
    //                 printf(",\n");
    //         }
    //         printf("\n");
    //         _indent_;    
    //         printf("}");
    //         break;
    //     case 's':
    //         _indent_;
    //         printf("\"%s\"", l->s);
    //         break;
    //     case 'p':
    //         _indent_;
    //         printf("'%.*s'", l->p->size, l->p->str);
    //         break;
    //     case 'P':
    //         _indent_;
    //         printf("*%p", l->P);
    //         break;
    //     case '0' ... '9':
    //         _indent_;
    //         printf("->%d\n", l->type - '0');
    //         print_lexeme(l->lex, indent+1);
    //         break;
    //     case 'c':
    //         _indent_;
    //         printf("'%c'", l->c);
    //         break;
    //     case 'b':
    //         _indent_;
    //         printf("%d", l->b);
    //         break;
    //     case 'B':
    //         _indent_;
    //         printf("%u", l->B);
    //         break;
    //     case 'h':
    //         _indent_;
    //         printf("%d", l->h);
    //         break;
    //     case 'H':
    //         _indent_;
    //         printf("%u", l->H);
    //         break;
    //     case 'l':
    //         _indent_;
    //         printf("%d", l->l);
    //         break;
    //     case 'L':
    //         _indent_;
    //         printf("%u", l->L);
    //         break;
    //     case 'q':
    //         _indent_;
    //         printf("%ld", l->q);
    //         break;
    //     case 'Q':
    //         _indent_;
    //         printf("%lu", l->Q);
    //         break;
    //     case 'f':
    //         _indent_;
    //         printf("%f", l->f);
    //         break;
    //     case 'd':
    //         _indent_;
    //         printf("%f", l->d);
    //         break;
    //     case '?':
    //         _indent_;
    //         printf("%s", l->qm ? "true" : "false");
    //         break;
    //     default:
    //         _indent_;
    //         printf("??");
    //         break;
    //     }
    // }
}