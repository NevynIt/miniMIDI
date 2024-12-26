#pragma once
#include "deps.h"
#include "helpers.h"
#include "bblocks.h"
#include "d_functions.h"

/*
 * Regular expressions for 8 bit characters
 *
 * Whitespace
 *            non-escaped whitespace in the pattern between units is ignored
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
 * ^          matches only if the current group has not matched any character yet
 * $          matches only at the end of the string
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

namespace ast::_re
{
    using namespace ast::_b;
    using namespace ast::_h;

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
            printf("(%s) re\"%.*s\"", rule ? rule : "", match->size(), match->data());
            if (groups->size())
            {
                print_ind(indent, "\nGroups:");
                groups->printvalue(indent);
            }
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
            //skip not escaped whitespace in the pattern
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                p++;

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

            //skip not escaped whitespace in the pattern
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                p++;

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
            group = nullptr;
            special = 0;

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
                special = '@';
                p++;
                if (!delimited_start.set_class(p)) return false;
                p++;
                if (!delimited_end.set_class(p)) return false;
                p++;
            }
            else if (*p == '%')
            {
                special = '%';
                p++;
            }
            else if (*p == '^')
            {
                special = '^';
                p++;
            }
            else if (*p == '$')
            {
                special = '$';
                p++;
            }
            else
            { // use a mask otherwise
                if (!mask.set_class(p)) return false;
                p++;
            }
            
            //repetition
            if (!repetition.set(p)) return false;

            //skip not escaped whitespace in the pattern
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                p++;
            
            return true;
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
                return ru.match_regex(s, group, true, total_count);
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
            else if (special == '^')
            {
                if (total_count > 0)
                    return nullptr;
                else
                    return new lex_v<char>();
            }
            else if (special == '$')
            {
                if (!ast::_b::stream_eof(s))
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
                if (auto re = submatch->as<lex_re>())
                    for (int i = 0; i < re->groups->size(); i++)
                        result->groups->push_back(re->groups->take(i));
                delete submatch;
            }
        }

        int lenghtof(lexeme *l)
        {
            if (auto v = l->as<lex_v<char>>())
                return v->size();
            else if (auto re = l->as<lex_re>())
                return re->match->size();
            else if (auto V = l->as<lex_V>())
            {
                int len = 0;
                for (auto i : *V)
                    len += lenghtof(i);
                return len;
            }
            else if (auto o = l->as<lex_o<char>>())
                return 1;
            return 0;
        }

        template<typename _StreamType>
        lex_re *match_regex(_StreamType &s, char_cptr pattern, bool internal = false, int total = 0)
        {
            result = new lex_re();
            total_count = total;
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
                    if (repetition.min == 1 && repetition.max == 1)
                    {
                        success = true;
                        auto sm = match_one(s);
                        int pre_append = result->match->size();
                        if (sm)
                            append_one(sm);
                        else
                            success = false;
                        last_count = result->match->size() - pre_append;
                        total_count += last_count;
                    }
                    else
                    {
                        int total_bkp = total_count;
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
                            total_count += lenghtof(submatch);
                        }
                        if (count < repetition.min)
                        {
                            ast::_b::stream_restore(s, group_sshot);
                            delete subgroup;
                            success = false;
                            last_count = 0;
                            total_count = total_bkp;
                            continue;
                        }
                        success = true;
                        int pre_append = result->match->size();
                        for (auto m : *subgroup)
                            append_one(m);
                        last_count = result->match->size() - pre_append;
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

        int last_count = 0;
        int total_count = 0;
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
