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

    char get_escape(char_cptr &s)
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

    lexeme *element_at(lexeme *l, int n)
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

    //completed in theory, LOTS of debugging needed
    lexeme *replace_regex(lexeme *l, char_cptr &pattern)
    {
        if (!l)
            return nullptr;
        lexeme *cur = nullptr;
        lex_v<char> *v = nullptr;
        lex_V *V = nullptr;

        while(*pattern)
        {
            char c = *pattern;
            if (c == '$')
            {
                if (cur)
                {
                    if (!V)
                        V = new lex_V();
                    V->push_back(cur);
                    cur = nullptr;
                }
                if (v)
                {
                    if (!V)
                        V = new lex_V();
                    V->push_back(v);
                    v = nullptr;
                }
                cur = (lexeme *)l->clone();
                pattern++;

                while (*pattern)
                {
                    c = *pattern;
                    if (c == '.')
                    {
                        //skip the dot
                    }
                    else if (c >= '0' && c <= '9')
                    { //start subscripting
                        int n = c - '0';
                        pattern++;
                        while (*pattern >= '0' && *pattern <= '9')
                        {
                            n = n * 10 + (*pattern - '0');
                            pattern++;
                        }

                        lexeme *e = element_at(cur, n);
                        if (e)
                        {
                            e = (lexeme *)e->clone();
                            delete cur;
                            cur = e;
                        }
                        else
                        {
                            delete cur;
                            cur = new lex_V();
                        }
                    }
                    else if (c == '[')
                    { //start range
                        //use another regex to parse the range
                        pattern++;
                        const char *range_re = "(\\s*" //skip spaces
                                                "(-?\\d+)?\\s*" //start index
                                                "(:\\s*(-?\\d+)?)?\\s*,?)*\\]"; //end index
                        //(\s*(-?\d+)?\s*(:\s*(-?\d+)?)?\s*,?)*
                        lex_re *range = ast::_re::match_regex(pattern, range_re);
                        //for each match of group 0
                            //subgroup 0 is the start index
                            //if subgroup 1 is not empty, there is a :
                                //and its subgroup 0 is the end index
                        if (range)
                        {
                            lex_V *new_V = new lex_V();
                            for (auto i : *(lex_V *)(*range)[0])
                            {
                                int start = 0;
                                int end = 0;
                                lex_re *g = i->as<lex_re>();
                                if (g)
                                {
                                    if (((lex_V *)(*g)[0])->size() > 0)
                                    {
                                        start = atoi(((lex_V *)(*g)[0])->at(0)->as<lex_v<char>>()->data());
                                        end = start;
                                    }
                                    if (((lex_V *)(*g)[1])->size() > 0)
                                    {
                                        end = -1;
                                        if ( (*((lex_V *)(*g)[1])->at(0)->as<lex_re>())[0]->as<lex_V>()->size() > 0)
                                        {
                                            end = atoi((*((lex_V *)(*g)[1])->at(0)->as<lex_re>())[0]->as<lex_V>()->at(0)->as<lex_v<char>>()->data());
                                        }
                                    }
                                    if (start < 0) start = cur->as<lex_V>()->size() + start;
                                    if (end < 0) end = cur->as<lex_V>()->size() + end;
                                    if (start < 0) start = 0;
                                    if (end < 0) end = 0;
                                    if (start >= cur->as<lex_V>()->size()) start = cur->as<lex_V>()->size() - 1;
                                    if (end >= cur->as<lex_V>()->size()) end = cur->as<lex_V>()->size() - 1;
                                    if (start > end)
                                    {
                                        for (int i = start; i >= end; i--)
                                        {
                                            lexeme *e = (lexeme *)element_at(cur, i)->clone();
                                            if (e)
                                            {
                                                new_V->push_back(e);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        for (int i = start; i <= end; i++)
                                        {
                                            lexeme *e = (lexeme *)element_at(cur, i)->clone();
                                            if (e)
                                            {
                                                new_V->push_back(e);
                                            }
                                        }
                                    }
                                }
                            }
                            delete range;
                            delete cur;
                            cur = new_V;
                        }
                    }
                    else if (c == '(')
                    { //start group
                        auto cur_V = cur->as<lex_V>();
                        if (cur_V)
                        {
                            char_cptr group_start = pattern;
                            lex_V *new_V = new lex_V();
                            for (auto i = cur_V->begin(); i != cur_V->end(); i++)
                            {
                                pattern = group_start;
                                lexeme *sub = replace_regex(*i, pattern);
                                if (sub)
                                {
                                    new_V->push_back(sub);
                                }
                                else
                                {
                                    //ignore empty groups
                                }
                            }
                            if (*pattern == ')')
                            {
                                pattern++;
                            }
                            delete cur;
                            cur = new_V;
                            if (new_V->size() == 1)
                            {
                                lexeme *tmp = new_V->at(0);
                                new_V->clear();
                                delete new_V;
                                cur = tmp;
                            }
                        }
                        else
                        {
                            lexeme *sub = replace_regex(cur, pattern);
                            if (sub)
                            {
                                delete cur;
                                cur = sub;
                            }
                            else
                            {
                                delete cur;
                                cur = new lex_V();
                            }
                        }

                    }
                    else if (c == '{')
                    { //start interleaving
                        //skip the bracket
                        pattern++;
                        //collect the separator
                        lex_v<char> *sep = new lex_v<char>();
                        while (*pattern && *pattern != '}')
                        {
                            if (*pattern == '\\')
                            {
                                pattern++;
                                sep->push_back(get_escape(pattern));
                            }
                            else
                            {
                                sep->push_back(*pattern);
                            }
                            pattern++;
                        }
                        if (*pattern == '}')
                        {
                            pattern++;
                        }
                        else
                        {
                            delete sep;
                            break;
                        }
                        //interleave the contents of cur with the contents of sep
                        lex_V *cur_V = cur->as<lex_V>();
                        if (cur_V)
                        {
                            //create a new lex_V
                            lex_V *new_V = new lex_V();
                            for (auto i = cur_V->begin(); i != cur_V->end(); i++)
                            {
                                new_V->push_back(*i);
                                if (i != cur_V->end() - 1)
                                {
                                    new_V->push_back((lexeme *)sep->clone());
                                }
                            }
                            cur_V->clear();
                            delete cur;
                            cur = new_V;
                            if (new_V->size() == 1)
                            {
                                lexeme *tmp = new_V->at(0);
                                new_V->clear();
                                delete new_V;
                                cur = tmp;
                            }
                        }
                        else
                        {
                            //do nothing
                        }
                    }
                    else if (c == '!')
                    { //concatenate, end of expression
                        cur = ast::_f::concat_decorator<char>(cur);
                        pattern++;
                        break;
                    }
                    else
                    { //something else, end of expression
                        break;
                    }
                }
            }
            else if (c == ')')
            {   //end of group
                break;
            }
            else
            {
                if (c == '\\')
                {
                    pattern++;
                    c = get_escape(pattern);
                }
                if (cur)
                {
                    if (!V)
                        V = new lex_V();
                    V->push_back(cur);
                    cur = nullptr;
                }
                if (!v)
                    v = new lex_v<char>();
                v->push_back(c);
                pattern++;
            }
        }

        delete l;
        if (cur)
        {
            if (!V)
                return cur;
            V->push_back(cur);
            return V;
        }
        else if (v)
        {
            if (!V)
                return v;
            V->push_back(v);
            return V;
        }
        else if (V)
        {
            return V;
        }
        else
        {
            return nullptr;
        }
    }

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