#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"
#include "regex.h"

/* Regex replace v4, returns a lex_re
 * the syntax is:
 *      range ::= integer? (':' integer?)?              -regex- (<range>(<min>(-?\d+)?(<colon>:<max>(-?\d+)?)?)%)
 *          --if min is not present, it defaults to 0
 *          --if max is not present, it defaults to -1
 *          --return the elements of cur from min to max (in reverse order if min > max), if they exist
 * 
 *      name ::= '<' '^'? literal* '>'                       -regex- (\<<name>([^>]*)\>)
 *          --return the elements of cur that have the rule name name
 *          --if ^ is present, return the elements of cur that do not have the rule name name
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
 * "(\*<setname>@\<\>)|"
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
 * if % is applied to a lex_re, it extracts the groups of the lex_re
 * this is the only way to extract the groups of a lex_re in a replace string
*/

namespace ast::_rp
{
    using namespace ast::_b;
    using namespace ast::_h;

    using ast::_re::lex_re;
    using ast::_re::get_escape;

    typedef const char *char_cptr;

    lex_re *select_regex(const lexeme *l, char_cptr &pattern, bool internal = false);
    lex_re *replace_regex(lexeme *l, char_cptr pattern);

    bool apply_selector(lexeme *&cur, char_cptr &pattern);

    template<typename T0, const auto arr>
    ast_internal_rule(replace)
    {
        static_assert(std::is_same_v<decltype(arr.data()), const char *>, "arr must be a char array");
    public:
        set_signature<ast_str("replace"), arr, uti::sig_of<T0>()>();
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            return replace_regex(l, arr.data());
        }
    };

} // namespace ast::_rp