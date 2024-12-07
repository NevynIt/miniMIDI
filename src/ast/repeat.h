#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"

namespace ast::_r
{
    template<typename T0, int min = 0, int max = -1, typename Sep = ast::_f::pass_always>
    class rep
    {
    public:
        match_method(s)
        {
            lexeme_S *l = new lexeme_S();
            l->type = 'V';
            l->V = l->new_V();
            void *sshot = ast::_b::stream_snapshot(s);
            int count = 0;

            while (max == -1 || count < max)
            {
                if (count > 0)
                {
                    void *sshot2 = ast::_b::stream_snapshot(s);
                    auto ls = Sep::match(s);
                    if (ls)
                        delete ls;
                    else
                    {
                        ast::_b::stream_restore(s, sshot2);
                        break;
                    }
                }

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

            delete l;
            ast::_b::stream_restore(s, sshot);
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