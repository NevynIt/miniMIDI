#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"

namespace ast::_s
{
    using namespace ast::_b;
    using namespace ast::_h;

    template<typename _StreamType, typename T0, typename ...types>
    lexeme *seq_match(_StreamType &s, int _trace_, lex_V *l)
    {
        lexeme *ll = T0::match(s, _trace_ ? _trace_ +  1 : 0);
        if (!ll)
        {
            return nullptr;
        }
        if (ll->is<_f::lex_drop>())
        {
            delete ll;
        }
        else
        {
            l->push_back(ll);
        }
        if constexpr (sizeof...(types) == 0)
        {
            return l;
        }
        else
        {
            return seq_match<_StreamType, types...>(s, _trace_, l);
        }
    }

    template<typename ...types>
    ast_internal_rule(seq)
    {
    public:
        set_signature<ast_str("seq"), uti::get_from<types...>{}>;

        ast_primary_implementation(s)
        {
            auto l = new lex_V();
            void *sshot = ast::_b::stream_snapshot(s);

            lexeme *ll = seq_match<_StreamType, types...>(s, _trace_, l);
            if (!ll)
            {
                ast::_b::stream_restore(s, sshot);
                delete l;
                return nullptr;
            }
            return l;
        }
    };

}