#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"
#include "regex.h"

/* Regex replace syntax v2, as a decorator, l is the lexeme being decorated
 * /abcd$abcd/              create lex_V with {abcd}, l, {abcd}
 * /$0/                     return l[0] if l is a lex_V, otherwise return l
 * /$1/                     return l[1] if l is a lex_V, otherwise return an empty lex_V
 * /$m[n]/                  return l[m][n] if l is a lex_V and l[m] is a lex_V, return l if m==n==0, otherwise return an empty lex_V
 * /$$$/                    return a lex_V with three copies of l
 * /$[a:b]/                 return a lex_V with l[a], l[a+1], ..., l[b]
 * /$[a:b]{, }/             return a lex_V with l[a], l[a+1], ..., l[b] separated by ", "
 * /$!/                     return a concatenation of the contents of l
 * /$[a:b]{, }!/            return a concatenation of the contents of l[a], l[a+1], ..., l[b] separated by ", "
 * /$[a,b,c,d]/             return a lex_V with l[a], l[b], l[c], l[d]
 * /$[a,b,c,d]{, }/         return a lex_V with l[a], l[b], l[c], l[d] separated by ", "
 * /$[a:b,c:d]/             return a lex_V with l[a], l[a+1], ..., l[b] and l[c], l[c+1], ..., l[d]
 * /$(abc $ abc)/           return a lex_V with {{abc}, x, {abc}} for each x in l
 * 
 * The syntax can be seen also as:
 * $                        take the current lexeme and start an expression
 * []                       take elements from the preceding expression, returning a lex_V with the results if there is more than one or zero elements
 * $n                       equivalent to $n[0]
 * .m                       equivalent to [m] 
 * ()                       apply the pattern inside the brackets (which define a group) to each element of the preceding expression, returning a lex_V with the results if there is more than one or zero elements
 * {}                       interleave the contents of the preceding expression (a lex_V) with the contents of the pattern inside the brackets, returning a lex_V with the results if there is more than one element
 * !                        concatenate recursively the preceding expression (a lex_V) into a single lex_v
 * \                        escape character, the following character is taken as a literal or escape sequence (like \n, \t, etc)
 * 
 * The syntax of [] is:
 * [a]                      take the a-th element of the preceding expression
 * [a,b,c,d]                take the a-th, b-th, c-th, and d-th elements of the preceding expression
 * 
 * a colon operator can be used to take ranges of elements. if the indexes are in descending order, they are taken in reverse
 * [a:b]                    take the a-th to b-th elements of the preceding expression
 * [a:b,c:d]                take the a-th to b-th and c-th to d-th elements of the preceding expression
 * [:]                      take all elements of the preceding expression
 * [b:]                     take all elements from the b-th to the end of the preceding expression
 * [:b]                     take all elements from the beginning to the b-th element of the preceding expression
 * 
 * if indexes are negative, they are taken as relative to the end of the expression
 * [-a]                     take the a-th element from the end of the preceding expression
 * 
 * lex_re are considered as lex_V with element 0 being the lex_re itself and element 1+ being the groups
 * spaces are taken literally, except inside the [] brackets
 * use a regex to match the pattern for the [] first: the complete syntax is represented by (ignoring whitespaces and comments):
 * ([^$]*                       //match anything that is not a $ (unless escaped)
 *  ($                          //match a $ followed by
 *    (\.?\d+                   //an optional dot followed by one or more digits
 *    |\[\s*                    //or a bracketed expression
 *       ((-?\d+)?\s*            //which can start with a negative number
 *       (:\s*(-?\d+)?)?)\s*     //followed by an optional colon and another number
 *       (,\s*                  //followed by an optional comma
 *        (-?\d+)?\s*           //and another number
 *        (:\s*(-?\d+)?)?\s*    //and a colon and another number
 *        )*\s*                 //this can be repeated
 *       \]
 *    |\{([^$]*)\}              //or a braced expression ($ is illegal here unless escaped)
 *    |\(()\)                   //or a grouped expression (recursive the same syntax)
 *  ))?
 * )*"
*/

namespace ast::_rp
{
    using namespace ast::_b;
    using namespace ast::_h;

    using ast::_re::lex_re;

    typedef const char *char_cptr;

    inline char get_escape(char_cptr &s)
    {
        char c = *s;
        switch (c)
        {
        case 'n':
            return '\n';
            break;
        case 't':
            return '\t';
            break;
        case 'r':
            return '\r';
            break;
        case '0':
            return '\0';
            break;
        case 'x':
        {
            char h[3] = {0};
            if (ast::_b::stream_eof(++s)) return 0;
            h[0] = *s;
            if (ast::_b::stream_eof(++s)) return 0;
            h[1] = *s;
            return (char)strtol(h, nullptr, 16);
        }
        default:
            return c;
        }
    }

    inline lexeme *element_at(lexeme *l, int n)
    {
        lex_V *V = l->as<lex_V>();
        if (V)
        {
            if (n >= 0)
            {
                if (n < V->size())
                    return V->at(n);
            }
            else
            {
                if (-n <= V->size())
                    return V->at(V->size() + n);
            }
            return nullptr;
        }
        lex_re *re = l->as<lex_re>();
        if (re)
        {
            if (n == 0 || (!re->groups && n == -1))
                return re;
            if (!re->groups)
                return nullptr;
            if (n >= 1)
                return element_at(re->groups, n-1);
            else if (n < 0)
                return element_at(re->groups, re->groups->size() + n);
            return nullptr;
        }
        if (n == 0)
            return l;
        return nullptr;
    }

    lexeme *select_regex(const lexeme *l, char_cptr &pattern);
    //completed in theory, LOTS of debugging needed
    lexeme *replace_regex(lexeme *l, char_cptr &pattern);

    template<typename T0, const auto arr>
    ast_internal_rule(replace)
    {
        static_assert(std::is_same_v<decltype(arr.data()), const char *>, "arr must be a char array");
    public:
        set_signature<ast_str("replace"), arr>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            char_cptr pattern = arr.data();
            return replace_regex(l, pattern);
        }
    };

} // namespace ast::_rp