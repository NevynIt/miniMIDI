#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"

/*
 * Regular expressions for 8 bit characters
 *
 * Literals: ' ... ' define a literal
 * 'a'        Literal, match the character 'a'
 * '.'        Dot, matches any character
 * 
 * Escape characters
 * '\'        Escape character, use the next character(s) as either a defined class or a literal
 * '\n'       Newline
 * '\r'       Carriage return
 * '\t'       Tab
 * '\0'       Null character
 * '\xhh'     Hexadecimal character, where hh is a 2 digit hexadecimal number
 * The following characthers should be escaped to be used as literals
 * '.' '[' ']' '(' ')' '{' '}' '+' '*' '?' '\\' '-' '/' '<' '>' '#' '@'
 * it's not mandatory, but it might be very confusing if you don't
 * 
 * Escape classes
 * '\s'       Whitespace, \t \f \r \n \v and spaces
 * '\S'       Non-whitespace
 * '\w'       Alphanumeric, [a-zA-Z0-9_]
 * '\W'       Non-alphanumeric
 * '\d'       Digits, [0-9]
 * '\D'       Non-digits
 * 
 * Classes: [ ... ] define a class by toggling a mask
 * ^          Invert the character mask
 * \          Toggle the character represented by the escape sequence
 * a-z        Character range, toggle any character between a and z (inclusive), a and z can be escaped and must be in ascending order
 * this allows for:
 * '[abc]'    Character class, match if one of {'a', 'b', 'c'}
 * '[^abc]'   Inverted class, match if NOT one of {'a', 'b', 'c'} -- NOTE: feature is currently broken!
 * '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
 * '[a-zcde]' [a-z] but not c, d, or e
 * '[^\n]'    Match any character except newline
 * 
 * Special escapes
 * @xy        matches any nested pattern delimited by x and y.
 *            x and y can be any literal, escape class or character class. if they are the same, there can be no nesting
 *            typical examples are @() for parentheses, @[] for brackets, @"" for quotes, and so on
 * %          matches only if the previous unit returned a non-empty match
 * 
 * Greedy quantifiers:
 * '*'        Asterisk, match zero or more
 * '+'        Plus, match one or more
 * '?'        Question, match zero or one
 * '{}'       Match zero or more times when empty
 * '{n}'      Match exactly n times
 * '{n,}'     Match n or more times
 * '{n,m}'    Match between n and m times
 * '{,m}'     Match at most m times
 * this allows for:
 * 'a*'       Match zero or more 'a'
 * 'a+'       Match one or more 'a'
 * 'a?'       Match zero or one 'a'
 * 'a{3}'     Match exactly 3 'a'
 * 'a{3,}'    Match 3 or more 'a'
 * 'a{3,5}'   Match between 3 and 5 'a'
 * 'a{,5}'    Match at most 5 'a'
 * 
 * Groups: ( ... ) define a group, which will act as a single unit
 * (p)          Group, match the pattern p
 * <>(p)        Unnamed group, match the pattern p and capture it
 * <name>(p)    Named group, match the pattern p and give it rule name 'name'
 * 
 * Alternatives:
 * '|'          Pipe, match either the left or the right unit, left associative. Use parentheses to group.
 * this allows for:
 * 'a|b'        Match either 'a' or 'b'
 * 'a|b|c'      Match either 'a', 'b', or 'c'
 * '(ab|cd)+'   Match one or more of 'abd' or 'acd'
 * '((ab)|(cd))'  Match either 'ab' or 'cd'
 * 'a|bc'       Match either 'ac' or 'bc'
*/

/* regex refactoring of the engine (always greedy)
 *  ---A regex string can be described as, using its own sintax and ignoring whitespace and -- comments
 * (
 *   \|?                    --alternative flag, if present, the pattern is considered only if the previous pattern failed
 *   <capture>(             --capture flag, if present the match is captured as a group
 *     \<[\>]*\>            --capture name, if present the match is captured with the name
 *   )?                   --this is optional
 *   <match>(               --match pattern
 *     \p|                  --parenthesised group
 *     \.|                  --literal dot
 *     (\\p)|               --special escapes
 *     (\\x\d\d)|           --hexadecimal character
 *     (\\.)|               --escaped character
 *     (\[[^\]]*\])|        --character class
 *     .                    --any other literal character
 *   )                    --this is the only mandatory part
 *   <repetition>(          --repetition flag, if the capture flag is also present, returns an array, with one element for each repetition (the array can be empty if the match is optional and not present)
 *      \*|                 --kleene star, match zero or more times
 *      \+|                 --plus, match one or more times
 *      \?|                 --question, match zero or one time
 *      (\{[0-9]*(,[0-9]?)?\}) --range {min, max}, match between min and max times. If not provided, min defaults to 0, max defaults to -1 (infinite)
 *   )?                   --this is optional
 * )*
 * 
 * test1 "aaaabbccdccdotsf(asdf\fer(ddd\(joke))c"./a+[bcd]*e?.{2,4}f\pa|b|c|d|e|f/ --> "aaaabbccdccdotsf(asdf\fer(ddd\(joke))c"
 * test2 " abppppab"./ <>(ab<>(p+)ab)/ --> (" ", ("ab", ["p", "p", "p", "p"], "ab"))
 * test3 " abppppab"./ (ab<>(p+)ab)/ --> (" ab", ["p", "p", "p", "p"], "ab")
 * test4 " cd1111efgh cd1ef2gh "./<>( cd<n1>(1)+ef<n2>(2)?gh)* / --> ([(" cd", [<n1>"1", <n1>"1", <n1>"1", <n1>"1"], "ef", [], "gh"), (" cd", [<n1>"1"], "ef", [<n2>"2"], "gh")], " ")
 * test5 " cd1111efgh cd1ef2gh "./( cd<n1>(1)+ef<n2>(2)?gh)* / --> (" cd", [<n1>"1", <n1>"1", <n1>"1", <n1>"1"], "ef", [], "gh cd", [<n1>"1"], "ef", [<n2>"2"], "gh ")
 */

namespace ast::_re
{
    using namespace ast::_b;
    using namespace ast::_h;

    typedef const char *char_cptr;

    // class lex_re : public lex_v<char>
    // {
    // public:
    //     set_signature<ast_str("lex_re")>();
    //     variant_inherit(lex_v<char>)

    //     lex_re() {}
    //     lex_re(const lex_re &r) : lex_v<char>(r)
    //     {
    //         if (r.groups)
    //             groups = (lex_V *)r.groups->clone();
    //     }

    //     void printvalue(int indent = 0) const override
    //     {
    //         if (rule)
    //             printf("(%s) ", rule);
    //         printf("\"%.*s\"", this->size(), this->data());
    //     }

    //     void printgroups(int indent = 0) const
    //     {
    //         print_ind(indent, "");
    //         if (rule)
    //             printf("(%s) ", rule);
    //         printf("\"%.*s\"\n", this->size(), this->data());
    //         if (groups)
    //         {
    //             // print_ind(indent, "Groups:\n");
    //             for (int i = 0; i < groups->size(); i++)
    //             {
    //                 print_ind(indent, "Group %d: ", i+1);
    //                 // groups->at(i)->printvalue(indent + 1);
    //                 auto gre = groups->at(i)->as<lex_re>();
    //                 if (gre)
    //                     gre->printgroups(indent + 1);
    //                 auto grV = groups->at(i)->as<lex_V>();
    //                 if (grV)
    //                 {
    //                     print_ind(indent, "{\n");
    //                     int i = 0;
    //                     for (auto j : *grV)
    //                     {
    //                         print_ind(indent, "[%d]: ", i++);
    //                         j->as<lex_re>()->printgroups(indent + 1);
    //                     }
    //                     print_ind(indent, "}");
    //                 }
    //                 printf("\n");
    //             }
    //         }
    //         else
    //         {
    //             print_ind(indent, "No groups");
    //         }
    //     }

    //     lexeme *operator[](int i) { return groups ? groups->at(i) : nullptr; }
    //     lex_V *groups = nullptr;
    //     ~lex_re()
    //     {
    //         if (groups)
    //             delete groups;
    //     }
    // };

    struct regex_mask
    {
    public:
        uint8_t mask[256/8] = {0};

        bool operator[](char c) const
        {
            return mask[c / 8] & (1 << (c % 8));
        }

        void add(char c)
        {
            mask[c / 8] |= (1 << (c % 8));
        }

        void remove(char c)
        {
            mask[c / 8] &= ~(1 << (c % 8));
        }

        void toggle(char c)
        {
            mask[c / 8] ^= (1 << (c % 8));
        }

        void add_range(char start, char end)
        {
            for (char c = start; c <= end; c++)
                add(c);
        }

        void remove_range(char start, char end)
        {
            for (char c = start; c <= end; c++)
                remove(c);
        }

        void toggle_range(char start, char end)
        {
            for (char c = start; c <= end; c++)
                toggle(c);
        }

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
                s++;
                if (!*s) return 0;
                h[0] = *s;
                s++;
                if (!*s) return 0;
                h[1] = *s;
                return (char)strtol(h, nullptr, 16);
            }
            default:
                return c;
            }
        }

        void add_escape(char_cptr &s)
        {
            char c = *s;
            switch (c)
            {
                case 'd':
                    add_range('0', '9');
                    break;
                case 'D':
                    remove_range('0', '9');
                    break;
                case 'w':
                    add_range('a', 'z');
                    add_range('A', 'Z');
                    add_range('0', '9');
                    add('_');
                    break;
                case 'W':
                    remove_range('a', 'z');
                    remove_range('A', 'Z');
                    remove_range('0', '9');
                    remove('_');
                    break;
                case 's':
                    add('\t');
                    add('\f');
                    add('\r');
                    add('\n');
                    add('\v');
                    add(' ');
                    break;
                case 'S':
                    remove('\t');
                    remove('\f');
                    remove('\r');
                    remove('\n');
                    remove('\v');
                    remove(' ');
                    break;
                default:
                    add(get_escape(s));
                    break;
            }
        }

        void toggle_all()
        {
            for (int i = 0; i < sizeof(mask); i++)
                mask[i] = ~mask[i];
        }

        void toggle_escape(char c)
        {
            switch (c)
            {
                case 'd':
                    toggle_range('0', '9');
                    break;
                case 'D':
                    toggle_all();
                    toggle_range('0', '9');
                    break;
                case 'w':
                    toggle_range('a', 'z');
                    toggle_range('A', 'Z');
                    toggle_range('0', '9');
                    toggle('_');
                    break;
                case 'W':
                    toggle_all();
                    toggle_range('a', 'z');
                    toggle_range('A', 'Z');
                    toggle_range('0', '9');
                    toggle('_');
                    break;
                case 's':
                    toggle('\t');
                    toggle('\f');
                    toggle('\r');
                    toggle('\n');
                    toggle('\v');
                    toggle(' ');
                    break;
                case 'S':
                    toggle_all();
                    toggle('\t');
                    toggle('\f');
                    toggle('\r');
                    toggle('\n');
                    toggle('\v');
                    toggle(' ');
                    break;
                default:
                    toggle(c);
                    break;
            }
        }

        bool set_class(char_cptr &s)
        {
            clear();
            bool group = false;
            bool range = false;
            bool start_set = false;
            bool escape = false;
            char start;

            while (s)
            {
                if (!group && escape)
                {
                    add_escape(s);
                    return true;
                }

                char c = *s;
                if (!group)
                {
                    if (c == '\\')
                    {
                        escape = true;
                    }
                    else if (c == '.')
                    {
                        add_all();
                        return true;
                    }
                    else if (c == '[')
                    {
                        group = true;
                    }
                    else
                    {
                        add(c);
                        return true;
                    }
                }
                else if (!escape && c == '\\')
                {
                    escape = true;
                }
                else
                {
                    if (escape)
                    {
                        c = get_escape(s);
                        escape = false;
                        if (range)
                        {
                            toggle_range(start, c);
                            range = false;
                            start_set = false;
                        }
                        else if (!start_set)
                        {
                            start = c;
                            start_set = true;
                        }
                        else
                        {
                            toggle(start);
                            start = c;
                        }
                    }
                    else if (c == '^')
                    {
                        toggle_all();
                    }
                    else if (c == ']')
                    {
                        if (start_set)
                        {
                            toggle(start);
                            start_set = false;
                        }
                        group = false;
                        return true;
                    }
                    else if (c == '-' && range)
                    { //- is the actual end character
                        toggle_range(start, c);
                        range = false;
                        start_set = false;
                    }
                    else if (c == '-' && start_set) //&& !range
                    { //starting a range
                        range = true;
                    }
                    else if (range)//finishing a range
                    {
                        toggle_range(start, c);
                        range = false;
                        start_set = false;
                    }
                    else
                    {
                        if (start_set)
                            toggle(start);
                        start = c;
                        start_set = true;
                    }
                }
                s++;
            }
            return false;
        }

        void add_all()
        {
            memset(mask, 0xFF, sizeof(mask));
        }

        void clear()
        {
            memset(mask, 0, sizeof(mask));
        }

        void invert()
        {
            for (int i = 0; i < sizeof(mask); i++)
                mask[i] = ~mask[i];
        }
    };

    class regex_repetition
    {
    public:
        int min = 1;
        int max = 1;

        regex_repetition() {}
        regex_repetition(int min, int max) : min(min), max(max) {}

        bool set(char_cptr &s)
        {
            min = 1;
            max = 1;

            if (!s) return true;

            if (*s == '{')
            {
                s++;
                if (!*s) return false;
                min = 0;
                max = -1;
                if (*s == '}')
                {
                    s++;
                    return true;
                }
                //add the digit to min, one by one
                while (*s >= '0' && *s <= '9')
                {
                    min = min * 10 + (*s - '0');
                    s++;
                }
                max = min;
                if (*s == ',')
                {
                    max = -1;
                    s++;
                    if (!*s) return false;
                    if (*s == '}')
                    {
                        s++;
                        return true;
                    }
                    max = 0;
                    while (*s >= '0' && *s <= '9')
                    {
                        max = max * 10 + (*s - '0');
                        s++;
                    }
                }
                if (*s == '}')
                {
                    s++;
                    return true;
                }
                return false;
            }
            else if (*s == '+')
            {
                min = 1;
                max = -1;
                s++;
            }
            else if (*s == '*')
            {
                min = 0;
                max = -1;
                s++;
            }
            else if (*s == '?')
            {
                min = 0;
                max = 1;
                s++;
            }
            return true;
        }
    };

    // //Most likely not needed anymore
    // inline void skip_alternative(char_cptr &pattern)
    // {
    //     int depth = 0;
    //     while ((*pattern != '|' || depth > 0) && *pattern != '\0')
    //     {
    //         if (*pattern == '(')
    //         {
    //             depth++;
    //         }
    //         else if (*pattern == ')')
    //         {
    //             if (depth == 0)
    //                 return;
    //             depth--;
    //         }
    //         else if (*pattern == '\\')
    //         {
    //             pattern++;
    //         }
    //         pattern++;
    //     }
    // }

    // template<typename _StreamType>
    // lex_re *match_regex(_StreamType &s, char_cptr &pattern, bool internal = false);

    // template<typename _StreamType>
    // lex_v<char> *match_parentheses(_StreamType &s, char open = '(', char close = ')')
    // {
    //     if (*s != open)
    //         return nullptr;

    //     auto sshot = ast::_b::stream_snapshot(s);

    //     lex_v<char> *l = new lex_v<char>();
    //     l->push_back(*s);
    //     s++;

    //     int depth = 0;
    //     while (!ast::_b::stream_eof(s))
    //     {
    //         if (*s == open)
    //         {
    //             depth++;
    //         }
    //         else if (*s == close)
    //         {
    //             if (depth == 0)
    //             {
    //                 l->push_back(*s);
    //                 s++;
    //                 return l;
    //             }
    //             depth--;
    //         }
    //         else if (*s == '\\')
    //         {
    //             l->push_back(*s);
    //             s++;
    //         }
    //         if (!ast::_b::stream_eof(s))
    //         {
    //             l->push_back(*s);
    //             s++;
    //         }
    //     }
    //     ast::_b::stream_restore(s, sshot);
    //     delete l;
    //     return nullptr;
    // }

    // //the pattern is considered a group
    // //if a group has no subgroups, it returns a lexeme of type v
    // //if a group has subgroups, it returns a lexeme of type V
    // //where the index 0 is the type v for the main group
    // //and each subsequent one represents one subgroup
    // //the subgroup lexeme is of type v if the group has repetition {1,1}
    // //it's of type V otherwise, with a lexeme for each match inside
    // //alternatives just return themselves, which could make the subgroup structure unpredictable
    // template<typename _StreamType>
    // lex_re *match_regex(_StreamType &s, char_cptr &pattern, bool internal = false)
    // {
    //     lex_re *l = new lex_re();
    //     void *sshot = ast::_b::stream_snapshot(s);
    //     regex_mask mask;
    //     regex_repetition repetition(1, 1);

    //     // while (!ast::_b::stream_eof(s))
    //     while (true)
    //     {
    //         // Ending the pattern or a group
    //         if (*pattern == '\0' || (internal && *pattern == ')'))
    //         {
    //             // Wrap up and return
    //             return l;
    //         }
    //         else if (*pattern == '|')
    //         {
    //             pattern++;
    //             skip_alternative(pattern);
    //         }
    //         // Starting a group
    //         else if (*pattern == '(')
    //         {
    //             bool capture = true;
    //             if (*(pattern + 1) == '?' && *(pattern + 2) == ':')
    //             {
    //                 capture = false;
    //                 pattern += 2;
    //             }

    //             char *rule_name = nullptr;
    //             if (*(pattern + 1) == '?' && *(pattern + 2) == '<')
    //             {
    //                 char_cptr name_end = nullptr;
    //                 char_cptr name_start = nullptr;
    //                 pattern += 2;
    //                 pattern++;
    //                 name_start = pattern;
    //                 while (*pattern != '>')
    //                 {
    //                     pattern++;
    //                 }
    //                 name_end = pattern;

    //                 rule_name = new char[name_end - name_start + 1];
    //                 strncpy(rule_name, name_start, name_end - name_start);
    //                 rule_name[name_end - name_start] = '\0';
    //                 // printf("Group name: %.*s\n", name_end - name_start, name_start);
    //             }

    //             if (capture && !l->groups)
    //             { //prepare to capture the group
    //                 l->groups = new lex_V();
    //             }

    //             pattern++;
    //             void *group_sshot = ast::_b::stream_snapshot(s); // to be used if repetition count is not met
    //             char_cptr group_start = pattern;
    //             lex_re *submatch = match_regex(s, pattern, true);
    //             //if *pattern is not ')' then the group is not closed
    //             //skip until the end of the group, considering nested groups
    //             if (*pattern != ')')
    //             {
    //                 int depth = 0;
    //                 while ((*pattern != ')' || depth > 0) && *pattern != '\0')
    //                 {
    //                     if (*pattern == '(')
    //                     {
    //                         depth++;
    //                     }
    //                     else if (*pattern == ')')
    //                     {
    //                         depth--;
    //                     }
    //                     else if (*pattern == '\\')
    //                     {
    //                         pattern++;
    //                     }
    //                     pattern++;
    //                 }
    //             }
                
    //             pattern++;
    //             // Set repetition if specified
    //             repetition.set(pattern);
                
    //             char_cptr group_end = pattern;

    //             lex_V *subgroup = nullptr;

    //             if (repetition.min == 1 && repetition.max == 1)
    //             {
    //                 if (submatch)
    //                 {
    //                     submatch->rule = rule_name;
    //                     submatch->own_rule = true;
    //                     if (capture)
    //                         l->groups->push_back(submatch);
    //                     //append all the contents to l as well
    //                     l->reserve(l->size() + distance(submatch->begin(),submatch->end()));
    //                     l->insert(l->end(),submatch->begin(),submatch->end());
    //                 }
    //                 else
    //                 {
    //                     // Check for alternatives
    //                     if (*pattern != '|')
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, sshot);
    //                         delete l;
    //                         delete [] rule_name;
    //                         return nullptr;
    //                     }
    //                     else
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, group_sshot);
    //                         // Skip '|' and proceed
    //                         delete [] rule_name;
    //                         pattern++;
    //                         continue;
    //                     }
    //                 }
    //             }
    //             else
    //             {
    //                 subgroup = new lex_V();
    //                 int count = 0;
    //                 while (submatch && submatch->size() > 0)
    //                 {
    //                     submatch->rule = strdup(rule_name);
    //                     submatch->own_rule = true;
    //                     subgroup->push_back(submatch);
    //                     count++;
    //                     if (ast::_b::stream_eof(s) || (repetition.max != -1 && count >= repetition.max))
    //                         break;
    //                     pattern = group_start;
    //                     submatch = match_regex(s, pattern, true);
    //                 }
    //                 pattern = group_end;

    //                 if (count < repetition.min)
    //                 {
    //                     // Check for alternatives
    //                     if (*pattern != '|')
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, sshot);
    //                         delete l;
    //                         delete subgroup;
    //                         delete [] rule_name;
    //                         return nullptr;
    //                     }
    //                     else
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, group_sshot);
    //                         // Skip '|' and proceed
    //                         delete subgroup;
    //                         delete [] rule_name;
    //                         pattern++;
    //                         continue;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     //append all the contents to l as well
    //                     for (auto m = subgroup->begin(); m != subgroup->end(); m++)
    //                     {
    //                         auto v_prime = (*m)->as<lex_re>();
    //                         l->reserve(l->size() + distance(v_prime->begin(),v_prime->end()));
    //                         l->insert(l->end(),v_prime->begin(),v_prime->end());
    //                     }
    //                     if (capture)
    //                         l->groups->push_back(subgroup);
    //                     else
    //                         delete subgroup;
    //                     delete [] rule_name;
    //                 }
    //             }
    //         }
    //         //matching parentheses
    //         else if (*pattern == '\\' && *(pattern + 1) == 'p')
    //         {
    //             pattern += 2;
    //             void *par_sshot = ast::_b::stream_snapshot(s);
                
    //             // Set repetition if specified
    //             repetition.set(pattern);

    //             auto submatch = match_parentheses(s);
    //             if (repetition.min == 1 && repetition.max == 1)
    //             {
    //                 if (submatch)
    //                 {
    //                     l->insert(l->end(),submatch->begin(),submatch->end());
    //                     delete submatch;
    //                 }
    //                 else
    //                 {
    //                     // Check for alternatives
    //                     if (*pattern != '|')
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, sshot);
    //                         delete l;
    //                         return nullptr;
    //                     }
    //                     else
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, par_sshot);
    //                         // Skip '|' and proceed
    //                         pattern++;
    //                         continue;
    //                     }
    //                 }
    //             }
    //             else
    //             {
    //                 auto subgroup = new lex_v<char>();
    //                 int count = 0;
    //                 while (submatch)
    //                 {
    //                     subgroup->insert(subgroup->end(),submatch->begin(),submatch->end());
    //                     delete submatch;
    //                     count++;
    //                     if (ast::_b::stream_eof(s) || (repetition.max != -1 && count >= repetition.max))
    //                         break;
    //                     submatch = match_parentheses(s);
    //                 }

    //                 if (count < repetition.min)
    //                 {
    //                     delete subgroup;
    //                     // Check for alternatives
    //                     if (*pattern != '|')
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, sshot);
    //                         delete l;
    //                         return nullptr;
    //                     }
    //                     else
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, par_sshot);
    //                         // Skip '|' and proceed
    //                         pattern++;
    //                         continue;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     l->insert(subgroup->begin(),subgroup->end());
    //                     delete subgroup;
    //                 }
    //             }
    //         }
    //         else //no group
    //         {
    //             void *char_sshot = ast::_b::stream_snapshot(s);
    //             mask.set_class(pattern);

    //             bool found = mask[*s];
    //             pattern++;

    //             // Set repetition if specified
    //             repetition.set(pattern);

    //             if (repetition.min == 1 && repetition.max == 1)
    //             {
    //                 if (found)
    //                 {
    //                     l->push_back(*s);
    //                     s++;
    //                 }
    //                 else
    //                 {
    //                     // Check for alternatives
    //                     if (*pattern != '|')
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, sshot);
    //                         delete l;
    //                         return nullptr;
    //                     }
    //                     else
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, char_sshot);
    //                         // Skip '|' and proceed
    //                         pattern++;
    //                         continue;
    //                     }
    //                 }
    //             }
    //             else
    //             {
    //                 //create a temporary lexeme of type v, store the contents while matching
    //                 //if the count is met, append it to the main lexeme
    //                 lex_v<char> *subgroup = new lex_v<char>();
    //                 int count = 0;
    //                 while (found)
    //                 {
    //                     subgroup->push_back(*s);
    //                     s++;
    //                     count++;
    //                     if  (ast::_b::stream_eof(s) || (repetition.max != -1 && count >= repetition.max))
    //                         break;
    //                     found = mask[*s];
    //                 }
    //                 if (count < repetition.min)
    //                 {
    //                     // Check for alternatives
    //                     if (*pattern != '|')
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, sshot);
    //                         delete l;
    //                         return nullptr;
    //                     }
    //                     else
    //                     {
    //                         // Restore stream snapshot
    //                         ast::_b::stream_restore(s, char_sshot);
    //                         // Skip '|' and proceed
    //                         pattern++;
    //                         continue;
    //                     }
    //                 }
    //                 else
    //                 {
    //                     l->reserve(l->size() + distance(subgroup->begin(),subgroup->end()));
    //                     l->insert(l->end(),subgroup->begin(),subgroup->end());
    //                     subgroup->clear();
    //                     delete subgroup;
    //                 }
    //             }
    //         }
    //     }
    //     while (*pattern == '|')
    //     {
    //         pattern++;
    //         skip_alternative(pattern);
    //     }

    //     if (ast::_b::stream_eof(s) && (*pattern == '\0' || *pattern == ')'))
    //     {
    //         // Wrap up and return
    //         return l;
    //     }

    //     // Cleanup and return nullptr
    //     stream_restore(s, sshot);
    //     delete l;
    //     return nullptr;
    // }

    class lex_re : public lexeme
    {
    public:
        set_signature<ast_str("lex_re")>();
        variant_inherit(lexeme)

        lex_re()
        {
            groups = new lex_V();
            match = new lex_v<char>();
        }
        lex_re(const lex_re &r) : lexeme(r)
        {
            groups = (lex_V *)r.groups->clone();
            match = (lex_v<char> *)r.match->clone();
        }
        ~lex_re()
        {
            delete groups;
            delete match;
        }

        void printvalue(int indent = 0) const override
        {
            printf("regex: (%s) ", rule ? rule : "");
            printf("\"%.*s\"\n", match->size(), match->data());
            print_ind(indent, "Groups:");
            groups->printvalue(indent);
        }

        lexeme *operator[](int i)
        {
            if (i < groups->size())
                return groups->at(i);
            return nullptr;
        }

        void append(const char c)
        {
            match->push_back(c);
        }

        void append(const lexeme *l)
        {
            if (!l)
                return;

            if (auto v = l->as<lex_v<char>>())
                match->insert(match->end(), v->begin(), v->end());
            else if (auto re = l->as<lex_re>())
                append(re->match);
            else if (auto V = l->as<lex_V>())
                for (auto i : *V)
                    append(i);
            else if (auto o = l->as<lex_o<char>>())
                match->push_back(o->o);
            else
                printf("Unknown lexeme type\n");
        }

        lex_v<char> *match;
        lex_V *groups;
    };

    class regex_unit
    {
    public:
        regex_unit() {}

        bool setup(char_cptr &p)
        {
            if (!*p) return false;
            //alternative flag
            if (*p == '|')
            {
                alternative = true;
                p++;
            }
            else
            {
                alternative = false;
            }

            if (!*p) return false;
            //capture flag
            if (*p == '<')
            {
                p++;
                rule = p;
                while (*p != '>' && *p != '\0')
                    p++;
                if (!*p) return false;
                rule_len = p - rule;
                p++;
            }
            else
            {
                rule = nullptr;
                rule_len = 0;
            }

            if (!*p) return false;
            //match pattern
            if (*p == '*' || *p == '+' || *p == '?' || *p == '{')
            {
                //malformed pattern
                return false;
            }
            else if (*p == '(')
            { //check if there is a group
                p++;
                if (!*p) return false;
                group = p;
                int depth = 0;
                while (*p != ')' || depth > 0)
                {
                    if (!*p) return false;
                    if (*p == '(')
                        depth++;
                    else if (*p == ')')
                        depth--;
                    else if (*p == '\\')
                    {
                        p++;
                        if (!*p) return false;
                    }
                    p++;
                }
                p++;
            }
            else if (*p == '@')
            {
                group = nullptr;
                special = '@';
                p++;
                if (!delimited_start.set_class(p)) return false;
                p++;
                if (!delimited_end.set_class(p)) return false;
                p++;
            }
            else if (*p == '%')
            {
                group = nullptr;
                special = '%';
                p++;
            }
            else
            { // use a mask otherwise
                group = nullptr;
                special = 0;
                if (!mask.set_class(p)) return false;
                p++;
            }
            
            //repetition
            return repetition.set(p);
        }

        template<typename _StreamType>
        lex_v<char> *match_parentheses(_StreamType &s)
        {
            if (!delimited_start[*s])
                return nullptr;

            auto sshot = ast::_b::stream_snapshot(s);

            lex_v<char> *l = new lex_v<char>();
            l->push_back(*s);
            s++;

            int depth = 0;
            while (!ast::_b::stream_eof(s))
            {
                if (delimited_end[*s])
                {
                    if (depth == 0)
                    {
                        l->push_back(*s);
                        s++;
                        return l;
                    }
                    depth--;
                }
                else if (delimited_start[*s])
                {
                    depth++;
                }
                else if (*s == '\\')
                {
                    l->push_back(*s);
                    s++;
                }
                if (!ast::_b::stream_eof(s))
                {
                    l->push_back(*s);
                    s++;
                }
            }
            ast::_b::stream_restore(s, sshot);
            delete l;
            return nullptr;
        }

        template<typename _StreamType>
        lexeme *match_one(_StreamType &s)
        {
            if (group)
            {
                regex_unit ru;
                return ru.match_regex(s, group, true);
            }
            else if (special == '@')
            {
                return match_parentheses(s);
            }
            else if (special == '%')
            {
                if (last_count == 0)
                    return nullptr;
                else
                    return new lex_v<char>();
            }                
            if (ast::_b::stream_eof(s))
                return nullptr;
            if (mask[*s])
            {
                auto c = *s;
                s++;
                return new lex_o<char>(c);
            }
            return nullptr;
        }

        void append_one(lexeme *submatch)
        {
            result->append(submatch);
            if (rule)
            {
                if (rule_len)
                {
                    submatch->rule = strdup(rule, rule_len);
                    submatch->own_rule = true;
                }
                result->groups->push_back(submatch);
            }
            else
            {
                auto re = submatch->as<lex_re>();
                if (re && re->groups->size() > 1)
                {
                    for (int i = 1; i < re->groups->size(); i++)
                    {
                        result->groups->push_back(re->groups->at(i));
                        re->groups->at(i) = nullptr;
                    }
                }
                delete submatch;
            }
        }

        template<typename _StreamType>
        lex_re *match_regex(_StreamType &s, char_cptr pattern, bool internal = false)
        {
            result = new lex_re();
            bool success = true;
            void *sshot = ast::_b::stream_snapshot(s);
            while (true)
            {
                if (success && (*pattern == 0 || (internal && *pattern == ')')))
                    return result;
                if (!setup(pattern) || (!success && !alternative))
                {
                    ast::_b::stream_restore(s, sshot);
                    delete result;
                    return nullptr;
                }
                if ((alternative && !success) || (success && !alternative))
                {
                    //multiple cases to be considered
                    //(A) single vs (a) not single repetition, combined with
                    //(B) submatch vs (b) no submask, i.e. mask is used
                    //(C) capturing vs (c) not capturing, combined with
                    //8 option total

                    //ABc -> append lexeme to l[0], append any subgroup tp l[]
                    //ABC -> append lexeme to l[0] & to l[]
                    //Abc -> append char to l[0]
                    //AbC -> append char to l[0] & a new lexeme to l[]

                    
                    //aBC -> append each lexeme to l[0], append a lex_V to l[]
                    //abC -> append each char to l[0], append a new lex_V with new lexemes to l[]
                    //aBc -> append each lexeme to l[0], append any subgroup tp l[]
                    //abc -> append each char to l[0]
                    if (repetition.min == 1 && repetition.max == 1)
                    {
                        success = true;
                        auto sm = match_one(s);
                        int pre_append = result->match()->size();
                        if (sm)
                            append_one(sm);
                        else
                            success = false;
                        last_count = result->match()->size() - pre_append;
                    }
                    else
                    {
                        auto group_sshot = ast::_b::stream_snapshot(s);
                        lex_V *subgroup = new lex_V();
                        lexeme *submatch = nullptr;
                        int count = 0;
                        for (; repetition.max == -1 || count < repetition.max; count++)
                        {
                            submatch = match_one(s);
                            if (!submatch)
                                break;
                            subgroup->push_back(submatch);
                        }
                        if (count < repetition.min)
                        {
                            ast::_b::stream_restore(s, group_sshot);
                            delete subgroup;
                            success = false;
                            last_count = 0;
                            continue;
                        }
                        success = true;
                        int pre_append = result->match()->size();
                        for (auto m : *subgroup)
                            append_one(m);
                        last_count = result->match()->size() - pre_append;
                        subgroup->clear();
                        delete subgroup;
                    }
                }
                // else do nothing and skip the alternative
            }
        }

        bool alternative = false;
        const char *rule = nullptr; //null = no capture
        int rule_len = 0;
        const char *group = nullptr; //null = use the mask
        char special = 0;
        regex_mask delimited_start, delimited_end;
        regex_mask mask;
        regex_repetition repetition;

        uint16_t last_count = 0;
        lex_re *result = nullptr;
    };


    template<typename _StreamType>
    lex_re *match_regex(_StreamType &s, char_cptr pattern)
    {
        regex_unit ru;
        return ru.match_regex(s, pattern);
    }

    template<const auto arr>
    ast_internal_rule(regex)
    {
        static_assert(std::is_same_v<decltype(arr.data()), const char *>, "arr must be a char array");
    public:
        set_signature<ast_str("regex"), arr>();

        ast_primary_implementation(s)
        {
            return match_regex(s, arr.data());
        }
    };

}
