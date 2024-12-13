#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"

namespace ast::_r
{
    using namespace ast::_b;
    using namespace ast::_h;

    template<typename T0, int min = 0, int max = -1, typename Sep = ast::_f::pass_always>
    ast_internal_rule(rep)
    {
    public:
        ast_set_signature<ast_str("rep"), ast_sig(T0), min, max, ast_sig(Sep)>;

        ast_primary_implementation(s)
        {
            auto l = new lex_V();
            void *sshot = stream_snapshot(s);
            int count = 0;

            while (max == -1 || count < max)
            {
                if (count > 0)
                {
                    void *sshot2 = stream_snapshot(s);
                    auto ls = sub_match(Sep, s);
                    if (ls)
                        delete ls;
                    else
                    {
                        stream_restore(s, sshot2);
                        break;
                    }
                }

                auto li = sub_match(T0, s);
                if (li)
                {
                    l->push_back(li);
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
            stream_restore(s, sshot);
            return nullptr;
        }
    };


    template<typename T0>
    using opt = rep<T0, 0, 1>;

    template<typename T0>
    using any = rep<T0, 0, -1>;

    template<typename T0>
    using some = rep<T0, 1, -1>;

    template<typename T0, typename Sep>
    using list = rep<T0, 0, -1, Sep>;
}