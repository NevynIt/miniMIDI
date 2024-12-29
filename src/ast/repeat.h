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
        set_signature<ast_str("rep"), uti::sig_of<T0>(), min, max, uti::sig_of<Sep>()>();

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
                    auto ls = ast_sub_match(Sep, s);
                    if (ls)
                        delete ls;
                    else
                    {
                        stream_restore(s, sshot2);
                        break;
                    }
                }

                auto li = ast_sub_match(T0, s);
                if (li)
                {
                    if (li->template is<_f::lex_drop>())
                        delete li;
                    else
                    {
                        auto V = li->template as<lex_V>();
                        if (V && V->rule == nullptr)
                        {
                            for (auto &v : *V)
                            {
                                l->push_back(v);
                            }
                            V->clear();
                            delete li;
                        }
                        else
                        {
                            l->push_back(li);
                        }
                    }
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
    using any = rep<T0, 0, -1>;

    template<typename T0>
    using some = rep<T0, 1, -1>;

    template<typename T0, typename Sep>
    using list = rep<T0, 0, -1, Sep>;
}