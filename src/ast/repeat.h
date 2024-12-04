#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"

namespace ast::_r
{
    template<typename T0, int min = 0, int max = -1>
    class rep
    {
    public:
        using object = typename T0::object;

        template <typename S>
        static inline ast::_b::lexeme<object> *match(S &s)
        {
            ast::_b::lexeme<object> *l = new ast::_b::lexeme<object>();
            l->type = 'V';
            l->V = new std::vector<ast::_b::lexeme<object>*>();
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
                return l;
            }
            s = s_backup;
            delete l;
            return nullptr;
        }
    };

    // template<typename T0>
    // using optional = repeat<T0, 0, 1>;

    // template<typename T0>
    // using zeroOrMore = repeat<T0, 0>;

    // template<typename T0>
    // using oneOrMore = repeat<T0, 1, -1>;

    // template<typename T0, int min = 0, int max = -1>
    // using repeat = rep<T0, min, max>;

    template<typename T0>
    using opt = rep<T0, 0, 1>;

    template<typename T0>
    using any = rep<T0, 0, -1>;

    template<typename T0>
    using some = rep<T0, 1, -1>;
}