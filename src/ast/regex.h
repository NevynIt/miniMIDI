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
 * '.' '[' ']' '(' ')' '{' '}' '+' '*' '?' '\\' '-' '/'
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
 * (p)        Group, match the pattern p
 * (?:p)      Non-capturing group, match the pattern but don't capture it
 * 'a(bc)'    Match 'a' followed by 'bc'
 * 'a(?:bc)'  Match 'a' followed by 'bc' but don't capture 'bc'
 * 'a([bc])'   Match 'a' followed by 'b' or 'c'
 * 
 * Alternatives:
 * '|'        Pipe, match either the left or the right unit, left associative. Use parentheses to group.
 * this allows for:
 * 'a|b'      Match either 'a' or 'b'
 * 'a|b|c'    Match either 'a', 'b', or 'c'
 * '(ab|cd)'  Match either 'ab' or 'cd'
 * 'a|bc'     Match either 'ac' or 'bc'
 * '(ab|cd)+' Match one or more of 'ab' or 'cd'
*/

namespace ast_char
{
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

        template<typename _StreamType>
        char get_escape(_StreamType &s)
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
                if (ast::_b::stream_eof(++s)) return 0;
                char high = *s;
                if (ast::_b::stream_eof(++s)) return 0;
                char low = *s;
                return (char)strtol({high, low}, nullptr, 16);
            }
            default:
                return c;
            }
        }

        template<typename _StreamType>
        void add_escape(_StreamType &s)
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
            s++;
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

        template<typename _StreamType>
        void set_class(_StreamType &s)
        {
            clear();
            bool group = false;
            bool range = false;
            bool start_set = false;
            bool escape = false;
            char start;

            while (!ast::_b::stream_eof(s))
            {
                if (!group && escape)
                {
                    add_escape(s);
                    return;
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
                    }
                    else if (c == '[')
                    {
                        group = true;
                    }
                    else
                    {
                        add(c);
                        return;
                    }
                }
                else (!escape && c == '\\')
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
                        s++;
                        if (start_set)
                        {
                            toggle(start);
                            start_set = false;
                        }
                        group = false;
                        return;
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
                    else //finishing a range
                    {
                        toggle_range(start, c);
                        range = false;
                        start_set = false;
                    }
                }
                s++;
            }
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

        regex_repetition(int min, int max) : min(min), max(max) {}

        template<typename _StreamType>
        void set(_StreamType &s)
        {
            min = 1;
            max = 1;

            if (*s == '{')
            {
                s++;
                min = 0;
                max = -1;
                if (*s == '}')
                {
                    s++;
                    return;
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
                    if (*s == '}')
                    {
                        s++;
                        return;
                    }
                    while (*s >= '0' && *s <= '9')
                    {
                        max = max * 10 + (*s - '0');
                        s++;
                    }
                }
                if (*s == '}')
                    return;
                //error
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
        }
    };

    struct regex_group
    {
    public:
        char *start = nullptr;
        int rep_count = 0;
    };

    typedef const char *char_cptr;

    //the pattern is considered a group
    //if a group has no subgroups, it returns a lexeme of type v
    //if a group has subgroups, it returns a lexeme of type V
    //where the index 0 is the type v for the main group
    //and each subsequent one represents one subgroup
    //the subgroup lexeme is of type v if the group has repetition {1,1}
    //it's of type V otherwise, with a lexeme for each match inside
    //alternatives just return themselves, which could make the subgroup structure unpredictable
    template<typename _StreamType>
    lexeme_S *match_regex(_StreamType &s, char_cptr &pattern)
    {
        lexeme_S *l = new lexeme_S('v');
        void *sshot = ast::_b::stream_snapshot(s);
        regex_mask mask;
        regex_repetition repetition(1, 1);

        while (!ast::_b::stream_eof(s))
        {
            // Ending the pattern or a group
            if (*pattern == '\0' || *pattern == ')')
            {
                // Wrap up and return
                return l;
            }
            else if (*pattern == '|')
            {
                //todo skip the alternative and move on
                int depth = 0;
                while ((*pattern != '|' || depth > 0) && *pattern != '\0')
                {
                    if (*pattern == '(')
                    {
                        depth++;
                    }
                    else if (*pattern == ')')
                    {
                        depth--;
                    }
                    else if (*pattern == '\\')
                    {
                        pattern++;
                    }
                    pattern++;
                }

            }
            // Starting a group
            else if (*pattern == '(')
            {
                if (l->type == 'v')
                { //convert to V
                    auto tmp = new lexeme_S('V');
                    tmp->append(l);
                    l = tmp;
                }

                pattern++;
                void *group_sshot = ast::_b::stream_snapshot(s); // to be used if repetition count is not met
                char_cptr group_start = pattern;
                lexeme_S *submatch = match_regex(s, pattern);
                
                pattern++;
                // Set repetition if specified
                repetition.set(pattern);

                lexeme_S *subgroup = nullptr;

                if (repetition.min != 1 || repetition.max != 1)
                {
                    subgroup = new lexeme_S('V');
                }
                else if (submatch)
                {
                    l->append(submatch);
                    //append all the contents to l->V[0]->v as well
                    if (submatch->type == 'v')
                        l->V[0]->append(submatch);
                    else if (submatch->type == 'V')
                        l->V[0]->append(submatch->V[0]);
                }
                else
                {
                    // Restore stream snapshot
                    ast::_b::stream_restore(s, group_sshot);
                    // Check for alternatives
                    if (*pattern != '|')
                    {
                        delete l;
                        return nullptr;
                    }
                    else
                    {
                        // Skip '|' and proceed
                        pattern++;
                        continue;
                    }
                }

                int count = 0;
                while (submatch && (repetition.max == -1 || count < repetition.max))
                {
                    subgroup->append(submatch);
                    count++;
                    pattern = group_start;
                    submatch = match_regex(s, pattern);
                }

                if (count < repetition.min)
                {
                    // Restore stream snapshot
                    ast::_b::stream_restore(s, group_sshot);
                    delete subgroup;
                    // Check for alternatives
                    if (*pattern != '|')
                    {
                        delete l;
                        return nullptr;
                    }
                    else
                    {
                        // Skip '|' and proceed
                        pattern++;
                        continue;
                    }
                }
                else
                {
                    l->append(subgroup);
                    //append all the contents to l->V[0]->v as well
                    for (auto m = subgroup->V->begin(); m != subgroup->V->end(); m++)
                    {
                        if ((*m)->type == 'v')
                            l->V[0]->append(*m);
                        else if ((*m)->type == 'V')
                            l->V[0]->append((*m)->V[0]);
                    }
                }
            }
            else //no group
            {
                void *char_sshot = ast::_b::stream_snapshot(s);
                mask.set_class(pattern);

                bool found = mask[*s];
                pattern++;

                // Set repetition if specified
                repetition.set(pattern);

                if (repetition.min == 1 && repetition.max == 1)
                {
                    if (found)
                    {
                        if (l->type == 'v')
                        {
                            l->append(*s);
                        }
                        else if (l->type == 'V')
                        {
                            l->V->at(0)->append(*s);
                        }
                    }
                    else
                    {
                        // Restore stream snapshot
                        ast::_b::stream_restore(s, char_sshot);
                        // Check for alternatives
                        if (*pattern != '|')
                        {
                            delete l;
                            return nullptr;
                        }
                        else
                        {
                            // Skip '|' and proceed
                            pattern++;
                            continue;
                        }
                    }
                }
                else
                {
                    //create a temporary lexeme of type v, store the contents while matching
                    //if the count is met, append it to the main lexeme
                    lexeme_S *subgroup = new lexeme_S('v');
                    int count = 0;
                    while (found && (repetition.max == -1 || count < repetition.max))
                    {
                        subgroup->append(*s);
                        s++;
                        count++;
                        found = mask[*s];
                    }
                    if (count < repetition.min)
                    {
                        // Restore stream snapshot
                        ast::_b::stream_restore(s, char_sshot);
                        delete subgroup;
                        // Check for alternatives
                        if (*pattern != '|')
                        {
                            delete l;
                            return nullptr;
                        }
                        else
                        {
                            // Skip '|' and proceed
                            pattern++;
                            continue;
                        }
                    }
                    else
                    {
                        if (l->type == 'v')
                        {
                            l->append(subgroup);
                        }
                        else if (l->type == 'V')
                        {
                            l->V->at(0)->append(subgroup);
                        }
                    }
                }
            }
        }

        if (ast::_b::stream_eof(s) && *pattern == '\0')
        {
            // Wrap up and return
            return l;
        }

        // Cleanup and return nullptr
        delete l;
        return nullptr;
    }

    template<const char *arr>
    class token_regex
    {
    public:
        match_method(s)
        {
            return match_regex(s, arr);
        }
    };

    #define re_decl(_NAME_, _PATTERN_) \
    char_array_decl(_NAME_##_pattern) = _PATTERN_; \
    using _NAME_ = token_regex<_NAME_##_pattern>

    #define re_match(_NAME_, _PATTERN_) \
    char_array_decl(_NAME_##_pattern) = _PATTERN_; \
    using _NAME_ = ast::_d::dec<token_regex<_NAME_##_pattern>,ast::_f::select<0>>
};
