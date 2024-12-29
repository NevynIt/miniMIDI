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

        enum storage_type : uint8_t
        {
            REF,
            OWN,
            DROP
        };

        lex_re(storage_type t = OWN, const char *ptr = nullptr) : lexeme()
        {
            if (t == REF)
                set_ref(ptr);
            else if (t == OWN)
                set_own();
            else if (t == DROP)
                set_drop();      
        }

        lex_re(const lex_re &r) : lexeme(r)
        {
            if (r.groups)
                groups = new lex_V(*r.groups);
            if (r.match)
                match = (lexeme *)r.match->clone();
        }

        ~lex_re()
        {
            delete groups; //ok also for nullptr
            delete match;
        }

        void set_ref(const char *ptr, int len = 0)
        {
            delete match;
            delete groups;

            match = new lex_ref<const char>(ptr, len);
            groups = nullptr;
        }

        storage_type get_storage_type() const
        {
            if (match)
                return match->as<lex_ref<const char>>() ? REF : OWN;
            return DROP;
        }

        lex_re *get_group(const char *name)
        {
            if (!groups)
                return nullptr;
            for (auto g : *groups)
            {
                if (g->same_rule(name))
                    return g->as<lex_re>();
            }
            return nullptr;
        }

        lex_re *get_group(int n)
        {
            if (!groups)
                return nullptr;
            if (n < 0 || n >= groups->size())
                return nullptr;
            return groups->at(n)->as<lex_re>();
        }

        void set_own()
        {
            delete match;
            delete groups;

            match = new lex_v<char>();
            groups = nullptr;
        }

        void set_drop()
        {
            delete match;
            delete groups;

            match = nullptr;
            groups = nullptr;
        }

        const char *data() const
        {
            if (!match)
                return "";
            if (auto ref = match->as<lex_ref<const char>>())
                return ref->ptr;
            else if (auto v = match->as<lex_v<char>>())
                return v->data();
            return "";
        }

        int size() const
        {
            if (!match)
                return 0;
            if (auto ref = match->as<lex_ref<const char>>())
                return ref->len;
            else if (auto v = match->as<lex_v<char>>())
                return v->size();
            return 0;
        }

        void printvalue(int indent = 0) const override
        {
            printf("re\"%.*s\"", size(), data());
            if (groups && groups->size())
            {
                print_ind(indent, "\nGroups:");
                groups->printvalue(indent);
            }
        }

        void append(const char c)
        {
            if (!match)
                return;
            if (auto v = match->as<lex_v<char>>())
                v->push_back(c);
            else if (auto ref = match->as<lex_ref<const char>>())
                ref->len++; //assume it's the same characer
            //if match is a lex_drop, ignore
        }

        void append(const char *c)
        {
            if (!match)
                return;
            if (auto v = match->as<lex_v<char>>())
                v->insert(v->end(), c, c + strlen(c));
            else if (auto ref = match->as<lex_ref<const char>>())
                ref->len += strlen(c); //this should check that it's the same string
            //if match is a lex_drop, ignore
        }

        void append(const char *c, int len)
        {
            if (!match)
                return;
            if (auto v = match->as<lex_v<char>>())
                v->insert(v->end(), c, c + len);
            else if (auto ref = match->as<lex_ref<const char>>())
                ref->len += len; //this should check that it's the same string
            //if match is a lex_drop, ignore
        }

        void append(const lexeme *l)
        {
            if (!l)
                return;
            
            if (auto d = l->as<lex_drop>())
                return;
            if (!match)
                return;

            if (auto v = match->as<lex_v<char>>())
            {
                if (auto V = l->as<lex_V>())
                {
                    for (auto i : *V)
                        append(i);
                }
                else if (auto re = l->as<lex_re>())
                {
                    v->reserve(v->size() + re->size());
                    v->insert(v->end(), re->data(), re->data() + re->size());
                }
                else if (auto lv = l->as<lex_v<char>>())
                {
                    v->reserve(v->size() + lv->size());
                    v->insert(v->end(), lv->data(), lv->data() + lv->size());
                }
                else if (auto lr = l->as<lex_ref<const char>>())
                {
                    v->reserve(v->size() + lr->len);
                    v->insert(v->end(), lr->ptr, lr->ptr + lr->len);
                }
                else if (auto o = l->as<lex_o<char>>())
                {
                    v->push_back(o->o);
                }
                else
                {
                    printf("Unknown lexeme type\n");
                }
            }
            else if (auto ref = match->as<lex_ref<const char>>())
            { //TODO: it should check if what is being added is just the continuation of ptr, and transform into a lex_v<char> if not
                if (auto V = l->as<lex_V>())
                {
                    for (auto i : *V)
                        append(i);
                }
                else if (auto re = l->as<lex_re>())
                {
                    ref->len += re->size();
                }
                else if (auto lv = l->as<lex_v<char>>())
                {
                    ref->len += lv->size();
                }
                else if (auto lr = l->as<lex_ref<const char>>())
                {
                    ref->len += lr->len;
                }
                else if (auto o = l->as<lex_o<char>>())
                {
                    ref->len++;
                }
                else
                {
                    printf("Unknown lexeme type\n");
                }
            }
        }

        void append_group(lexeme *l)
        {
            if (!l)
                return;
            
            if (!match)
                return;
            if (auto d = l->as<lex_drop>())
                return;

            if (!groups)
                groups = new lex_V();
            groups->push_back(l);
        }

        lexeme *match = nullptr; //match can be a lex_drop, a lex_ref<const char> or a lex_v<char>
        lex_V *groups = nullptr;
    };

    class regex_unit
    {
    public:
        regex_unit() {}

        bool setup()
        {
            char_cptr &p = pattern;

            //skip not escaped whitespace in the pattern
            while (*p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                p++;

            if (pattern_eof()) return false;
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

            if (pattern_eof()) return false;
            //capture flag
            if (*p == '<')
            {
                p++;
                rule = p;
                while (*p != '>' && *p != '\0')
                    p++;
                if (pattern_eof()) return false;
                rule_len = p - rule;
                p++;
            }
            else
            {
                rule = nullptr;
                rule_len = 0;
            }

            if (pattern_eof()) return false;
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
                if (pattern_eof()) return false;
                group = p;
                int depth = 0;
                while (*p != ')' || depth > 0)
                {
                    if (pattern_eof()) return false;
                    if (*p == '(')
                        depth++;
                    else if (*p == ')')
                        depth--;
                    else if (*p == '\\')
                    {
                        p++;
                        if (pattern_eof()) return false;
                    }
                    p++;
                }
                group_end = p;
                p++;
            }
            else if (*p == '@')
            {
                special = '@';
                p++;
                if (pattern_eof()) return false;
                if (!delimited_start.set_class(p)) return false;
                p++;
                if (pattern_eof()) return false;
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
        lex_re *match_parentheses(_StreamType &s)
        {
            if (!delimited_start[*s])
                return nullptr;

            auto sshot = ast::_b::stream_snapshot(s);

            lex_re *l = new lex_re(storage, s);
            l->append(*s);
            s++;

            int depth = 0;
            while (!stream_eof(s))
            {
                if (delimited_end[*s])
                {
                    if (depth == 0)
                    {
                        l->append(*s);
                        s++;

                        lex_re *cnt = new lex_re(storage, l->data()+1);
                        cnt->append(l->data()+1, l->size()-2);
                        l->append_group(cnt);

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
                    l->append(*s);
                    s++;
                }
                if (!stream_eof(s))
                {
                    l->append(*s);
                    s++;
                }
            }
            ast::_b::stream_restore(s, sshot);
            delete l;
            return nullptr;
        }

        template<typename _StreamType>
        lex_re *match_one(_StreamType &s)
        {
            if (group)
            {
                regex_unit ru;
                ru.pattern = group;
                ru.pattern_end = group_end;
                ru.storage = storage;
                // ru.internal = true;
                ru.total_count = total_count;
                lex_re *re = ru.match_regex(s);
                return re;
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
                    return new lex_re(lex_re::DROP);
            }          
            else if (special == '^')
            {
                if (total_count > 0)
                    return nullptr;
                else
                    return new lex_re(lex_re::DROP);
            }
            else if (special == '$')
            {
                if (!stream_eof(s))
                    return nullptr;
                else
                    return new lex_re(lex_re::DROP);
            }
        
            if (stream_eof(s))
                return nullptr;
            if (mask[*s])
            {
                auto l = new lex_re(storage, s);
                l->append(*s);
                s++;
                return l;
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
                    submatch->rule = rule;
                }
                result->append_group(submatch);
            }
            else
            {
                if (auto re = submatch->as<lex_re>())
                    if (re->groups)
                        for (int i = 0; i < re->groups->size(); i++)
                            result->append_group(re->groups->take(i));
                delete submatch;
            }
        }

        bool pattern_eof()
        {
            return !*pattern || pattern == pattern_end;
        }

        template<typename _StreamType>
        bool stream_eof(_StreamType &s)
        {
            return  ast::_b::stream_eof(s) || (stream_end && s >= stream_end); //this will likekly explode when the stream is not just a char *, will deal with it later
        }

        template<typename _StreamType>
        lex_re *match_regex(_StreamType &s) //any
        {
            result = new lex_re(storage, s); //this will likekly explode when the stream is not just a char *, will deal with it later
            bool success = true;
            void *sshot = ast::_b::stream_snapshot(s);
            while (true)
            {
                if (success && pattern_eof())
                    return result;
                if (!setup() || (!success && !alternative))
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
                        int pre_append = result->size();
                        if (sm)
                            append_one(sm);
                        else
                            success = false;
                        last_count = result->size() - pre_append;
                        total_count += last_count;
                    }
                    else
                    {
                        int total_bkp = total_count;
                        auto group_sshot = ast::_b::stream_snapshot(s);
                        lex_V *subgroup = new lex_V();
                        lex_re *submatch = nullptr;
                        int count = 0;
                        for (; repetition.max == -1 || count < repetition.max; count++)
                        {
                            submatch = match_one(s);
                            if (!submatch)
                                break;
                            subgroup->push_back(submatch);
                            total_count += submatch->size();
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
                        int pre_append = result->size();
                        for (auto m : *subgroup)
                            append_one(m);
                        last_count = result->size() - pre_append;
                        subgroup->clear();
                        delete subgroup;
                    }
                }
                // else do nothing and skip the alternative
            }
        }

        char_cptr pattern = nullptr;
        char_cptr pattern_end = nullptr;
        lex_re::storage_type storage = lex_re::OWN;
        // bool internal = false;

        char_cptr stream_end = nullptr;

        bool alternative = false;
        const char *rule = nullptr; //null = no capture
        int rule_len = 0;
        const char *group = nullptr; //null = use the mask
        const char *group_end = nullptr;
        char special = 0;
        regex_mask delimited_start, delimited_end;
        regex_mask mask;
        regex_repetition repetition;

        int last_count = 0;
        int total_count = 0;
        lex_re *result = nullptr;
    };

    template<typename _StreamType>
    inline lex_re *match_regex(_StreamType &s, char_cptr pattern, lex_re::storage_type storage = lex_re::storage_type::OWN) //any
    {
        regex_unit ru;
        ru.pattern = pattern;
        ru.storage = storage;
        return ru.match_regex(s); //any
    }

    template<const auto arr, lex_re::storage_type storage = lex_re::storage_type::OWN>
    ast_internal_rule(regex)
    {
        static_assert(std::is_same_v<decltype(arr.data()), const char *>, "arr must be a char array");
    public:
        set_signature<ast_str("regex"), arr>();

        ast_primary_implementation(s)
        {
            return match_regex(s, arr.data(), storage);
        }
    };

}
