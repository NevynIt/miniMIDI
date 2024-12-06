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
        match_method(s)
        {
            lexeme_S *l = new lexeme_S();
            l->type = 'V';
            l->V = l->new_V();
            auto s_backup = s;
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

    template<typename T0>
    using opt = rep<T0, 0, 1>; //optional

    template<typename T0>
    using any = rep<T0, 0, -1>; //any number of

    template<typename T0>
    using some = rep<T0, 1, -1>; //at least one of
}