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

/* Regex select/replace syntax v3, as a decorator, l is the lexeme being decorated
 * #abcd$abcd#              create lex_V with {abcd}, l, {abcd}
 * #$0#                     return l[0] if l is a lex_V, otherwise return l
 * #$1#                     return l[1] if l is a lex_V, otherwise return an empty lex_V
 * #$m[n]#                  return l[m][n] if l is a lex_V and l[m] is a lex_V, return l if m==n==0, otherwise return an empty lex_V
 * #$$$#                    return a lex_V with three copies of l
 * #$[a:b]#                 return a lex_V with l[a], l[a+1], ..., l[b]
 * #$[a:b]{, }#             return a lex_V with l[a], l[a+1], ..., l[b] separated by ", "
 * #$!#                     return a concatenation of the contents of l
 * #$[a:b]{, }!#            return a concatenation of the contents of l[a], l[a+1], ..., l[b] separated by ", "
 * #$[a,b,c,d]#             return a lex_V with l[a], l[b], l[c], l[d]
 * #$[a,b,c,d]{, }#         return a lex_V with l[a], l[b], l[c], l[d] separated by ", "
 * #$[a:b,c:d]#             return a lex_V with l[a], l[a+1], ..., l[b] and l[c], l[c+1], ..., l[d]
 * #$*(abc$abc)#            return a lex_V with {{abc}, x, {abc}} for each x in l
 * #$(abc$abc)#             return a lex_V with {{abc}, l, {abc}}, if l is not empty, otherwise return an empty lex_V
 * #$<name>#                return a lex_V with the lexemes in l that have the rule name "name"
 * #$<n1>.<n2>{, }#         return a lex_V with the lexemes matching n2 from the ones matching rule n1 in l
 * #$[<n1>,<n2>,<n3>]#      return a lex_V with the lexemes matching n1, n2, and n3 in l
 * 
 * The syntax can be seen also as:
 * $                        take the current lexeme and start an expression
 * []                       take elements from the preceding expression, returning a lex_V with the results if there is more than one or zero elements
 * $n                       equivalent to $[n][0], n can be a number or a rule name
 * .m                       dots are skipped, this allows to do $n.m to get the m-th element of the n-th element of the preceding expression
 * ()                       apply the pattern inside the brackets (which define a group) to the preceding expression, $ inside the () represent the preceding expression
 * *()                      apply the pattern inside the brackets (which define a group) to each element of the preceding expression, returning a lex_V with the results if there is more than one or zero elements
 * $...|$...                if the first $... returns a non-empty lex_V, the any $ after the | are skipped, this can be chained $...|$...|$...|...
 * {}                       interleave the contents of the preceding expression (a lex_V) with the contents of the pattern inside the brackets, returning a lex_V with the results if there is more than one element
 * !                        concatenate recursively the preceding expression (a lex_V) into a single lex_v
 * \                        escape character, the following character is taken as a literal or escape sequence (like \n, \t, etc)
 * %                        flatten the array, if the preceding expression is a lex_V with lex_V elements, return a lex_V with the elements of the lex_V elements
 * /regex/ TODO             return the re result of /regex/ applied to each of the elements of the preceding expression
 * 
 * The syntax of [] for integer indexes is:
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
 * If indexes are not integers, they are considered as rule names, and the lexemes in the preceding expression are selected based on their rule name
 * [<re1>,<re2>]                take all the lexemes in the preceding expression that have rule name re1, followed by the ones that have rule name re2
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

/* Regex replace v4, returns a lex_re
 * the syntax is:
 *      range ::= integer? (':' integer?)?              -regex- (<range>(<min>(-?\d+)?(<colon>:<max>(-?\d+)?)?)%)
 *          --if min is not present, it defaults to 0
 *          --if max is not present, it defaults to -1
 *          --return the elements of cur from min to max (in reverse order if min > max), if they exist
 * 
 *      name ::= '<' literal* '>'                       -regex- (\<<name>([^>]*)\>)
 *          --return the elements of cur that have the rule name name
 * 
 *      separator ::= '.'                               -regex- (<separator>\.)
 *          --skip the separator
 * 
 *      concat ::= '!'                                  -regex- (<concat>\!)
 *          --return a lex_v with the contents of cur concatenated
 *      regex ::= '/' regex_pattern '/'                 -regex- (<regex>@\/\/)
 *          --return the result of applying the regex to the concatenated contents of cur
 * 
 *      interleave ::= '{' literal* '}'                 -regex- (\{<interleave>([^}]*)\})
 *          --return a lex_v with the contents of cur interleaved with the contents of the interleave pattern
 * 
 *      flatten ::= '%'                                 -regex- (<flatten>\%)
 *          --return a lex_V exploding each lex_V element of cur, and maintaining the others
 * 
 *      aggregate ::= '@'                               -regex- (<aggregate>\@)
 *          --return a lex_V with the contents of cur aggregated into a single lex_V
 * 
 *      group ::= '(' replace_string ')'                -regex- (<group>@\(\))
 *          --return the result of applying the replace_string to cur
 * 
 *      loop ::= '*' '(' replace_string ')'             -regex- (\*<loop>@\(\))
 *          --return a lex_V with the result of applying the replace_string to each element of cur
 * 
 *      array ::= '[' selector* (',' selector*)* ']'    -regex- (<array>@\[\])
 *          --return a lex_V with the results of applying each series of selector to cur. start over from cur at each comma
 * 
 *      selector ::= range | name | separator | array | concat | interleave | flatten | regex | group | loop
 * 
 *      literal ::= '\\' escape | [^$]                  -regex- ([^$]*)
 *      replace_string ::= literal* (('$' selector* ('|$' selector* )* )* literal*)*
 * 
 * The complete regex for selector is:
 * "(<range>(<min>(-?\d+)?(<colon>:<max>(-?\d+)?)?)%)|"
 * "(\<<name>([^>]*)\>)|"
 * "(<separator>\.)|"
 * "(<concat>\!)|"
 * "(<regex>@\/\/)|"
 * "(\{<interleave>([^}]*)\})|"
 * "(<flatten>\%)|"
 * "(<aggregate>\@)|"
 * "(<group>@\(\))|"
 * "(\*<loop>@\(\))|"
 * "(<array>@\[\])"
 * 
 * The process is as follows:
 * 1. copy all the literals to the output
 * 1a. if the next character is a $, go to 2
 * 1b. if the next character is a \, copy the next character to the output
 * 1c. if the string is over, return the output 
 * 2. once a $ is found, start an expression, with the starting lexeme as cur
 * 2a. fetch a selector from the replace string
 * 2b. cur = apply_selector(cur, selector)
 * 2c. if the next character is a | and cur is empty, skip the | and go to 2a
 * 2d. if the next character is a | skip the | and all subsequent alternative expressions
 * 2e. output cur, i.e. append the concatenated contents of cur to result[0], and the individual elements to result[]
 * 3. repeat from 1
 * 
 * when applying a selector, anything that is not a lex_V is enclosed in a lex_V first
 * this is true for lex_re too, except for the flatten selector
 * if % is applied directly to a lex_re, it extracts the groups of the lex_re from [1:]
 * this is the only way to extract the groups of a lex_re in a replace string
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
        lex_re *re = l->as<lex_re>();
        lex_V *V = nullptr;
        if (re)
            V = re->groups;
        else
            V = l->as<lex_V>();
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
        if (n == 0)
            return l;
        return nullptr;
    }

    lexeme *select_regex(const lexeme *l, char_cptr &pattern, bool internal = false);
    //completed in theory, LOTS of debugging needed
    lexeme *replace_regex(lexeme *l, char_cptr pattern);

    bool apply_selector(lexeme *&cur, char_cptr &pattern);

    // lexeme *select_regex_2(const lexeme *l, char_cptr &pattern, bool internal = false);

    template<typename T0, const auto arr>
    ast_internal_rule(replace)
    {
        static_assert(std::is_same_v<decltype(arr.data()), const char *>, "arr must be a char array");
    public:
        set_signature<ast_str("replace"), arr>();
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            return replace_regex(l, arr.data());
        }
    };

} // namespace ast::_rp