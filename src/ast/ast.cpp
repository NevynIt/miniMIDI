#include "ast.h"

namespace ast
{
    using namespace ast::_h; //helpers
    using namespace ast::_b; //bblocks

    lexeme *ast::_b::lexeme::operator[](const char *pattern) const
    {
        return ast::_rp::select_regex(this, pattern);
    }

    void ast::_b::lex_V::replace(int i, const char *pattern)
    {
        lexeme *l = this->at(i);
        if (l)
            (*this)[i] = ast::_rp::replace_regex(l, pattern);
    }

    void base_out_func(const lexeme *l)
    {
        if (l)
        {
            lex_v<char> *ll = concat_decorator<char>((lexeme *)l->clone())->as<lex_v<char>>();
            if (ll)
            {
                for (auto i = ll->begin(); i != ll->end(); i++)
                {
                    printf("%c", *i);
                }
                delete ll;
            }
            printf("\n");
        }
        else
        {
            printf("nullptr\n");
        }
    }

    out_func_t ast::_f::cur_out_func(out_func_t f)
    {
        static out_func_t _f = base_out_func;
        if (f)
            _f = f;
        return _f;
    }

    static constexpr char selector_skip_regex[] =  "(\\|$((((-?\\d+)?(:(-?\\d+)?)?)%)|"
                                    "(\\<([^>]*)\\>)|"
                                    "(\\.)|"
                                    "(\\!)|"
                                    "(@\\/\\/)|"
                                    "(\\{([^}]*)\\})|"
                                    "(\\%)|"
                                    "(\\@)|"
                                    "(@\\(\\))|"
                                    "(\\*@\\(\\))|"
                                    "(@\\[\\]))*)*";

    static constexpr char selector_regex[] =  "(<range>(<min>(-?\\d+)?(<colon>:<max>(-?\\d+)?)?)%)|"
                                    "(\\<<name>([^>]*)\\>)|"
                                    "(<separator>\\.)|"
                                    "(<concat>\\!)|"
                                    "(<regex>@\\/\\/)|"
                                    "(\\{<interleave>([^}]*)\\})|"
                                    "(<flatten>\\%)|"
                                    "(<aggregate>\\@)|"
                                    "(<group>@\\(\\))|"
                                    "(\\*<loop>@\\(\\))|"
                                    "(<array>@\\[\\])";
//for testing: (<range>(<min>(-?\d+)?(<colon>:<max>(-?\d+)?)?)%)|(\<<name>([^>]*)\>)|(<separator>\.)|(<concat>\!)|(<regex>@\/\/)|(\{<interleave>([^}]*)\})|(<flatten>\%)|(<aggregate>\@)|(<group>@\(\))|(\*<loop>@\(\))|(<array>@\[\])
//<range>((<min>(-?\d+)?(<colon>:<max>(-?\d+)?)?)%)

    lex_V *prepare_V(lexeme *&cur)
    {
        lex_V *V = cur->as<lex_V>();
        if (!V)
        {
            V = new lex_V();
            V->push_back(cur);
            cur = V;
        }
        return V;
    }

    int str2num(const char *s, int len)
    {
        if (len == 0)
            return 0;
        int ret = 0;
        int sign = 1;
        if (*s == '-')
        {
            sign = -1;
            s++;
            len--;
        }
        for (int i = 0; i < len; i++)
        {
            if (*s < '0' || *s > '9')
                return 0;
            ret = ret * 10 + (*s - '0');
            s++;
        }
        return ret * sign;
    }

    bool ast::_rp::apply_selector(lexeme *&cur, char_cptr &pattern)
    {
        lex_re *selector = ast::_re::match_regex(pattern, selector_regex);
        if (!selector)
            return false;

        const char *rule = selector->groups->at(1)->rule;
        if (strcmp(rule, "range") == 0)
        {
            int min = 0;
            int max = -1;
            lex_re *range = (lex_re *)selector->groups->at(1);
            for (int i=1; i < range->groups->size(); i++)
            {
                lexeme *r = range->groups->at(i);
                if (r->same_rule("min"))
                    max = min = str2num(r->as<lex_re>()->match()->data(), r->as<lex_re>()->match()->size());
                else if (r->same_rule("colon"))
                    max = -1;
                else if (r->same_rule("max"))
                    max = str2num(r->as<lex_re>()->match()->data(), r->as<lex_re>()->match()->size());
            }
            delete range;
            auto V = prepare_V(cur);
            int sz = V->size();
            if (min < 0)
                min = sz + min;
            if (max < 0)
                max = sz + max;

            int dir = 1;
            if (min > max)
                dir = -1;

            lex_V *result = new lex_V();
            if (dir == 1)
            {
                if (min < 0)
                    min = 0;
                if (max >= sz)
                    max = sz-1;
                max++;
            }
            else
            {
                if (min >= sz)
                    min = sz-1;
                if (max < 0)
                    max = 0;
                max--;
            }
            for (int i = min; i != max; i += dir)
            {
                result->push_back(V->take(i));
            }
            delete cur;
            cur = result;
        }
        else if (strcmp(rule, "name") == 0)
        {
            lex_V *result = new lex_V();
            lex_V *V = prepare_V(cur);
            for (int i = 0; i<V->size(); i++)
            {
                lexeme *l = V->at(i);
                if (l->same_rule(selector->groups->at(1)
                                    ->as<lex_re>()
                                    ->match()->data()))
                    result->push_back(V->take(i));
            }
            delete cur;
            cur = result; 
        }
        else if (strcmp(rule, "separator") == 0)
        {
            return true;
        }
        else if (strcmp(rule, "concat") == 0)
        {
            lex_re *result = new lex_re();
            result->append(cur);
            delete cur;
            cur = result; 
        }
        else if (strcmp(rule, "regex") == 0)
        {
            const char *pattern = selector->match()->data();
            int sz = selector->match()->size();
            char *tmp = new char[sz - 1];
            strncpy(tmp, pattern + 1, sz - 2);
            tmp[selector->match()->size() - 2] = 0;

            lex_re *src;
            if (auto re = cur->as<lex_re>())
                src = re;
            else
            {
                src = new lex_re();
                src->append(cur);
                delete cur;
                cur = src;
            }
            src->append('\0');
            char *str = src->match()->data();
            lex_re *result = match_regex(str, tmp);

            delete [] tmp;
            delete cur;

            if (result)
                cur = result;
            else
                cur = new lex_V();
        }
        else if (strcmp(rule, "interleave") == 0)
        {
            lex_V *result = new lex_V();
            lex_V *V = prepare_V(cur);
            lex_re *interleave = (lex_re *)selector->groups->at(1);
            for (int i = 0; i<V->size(); i++)
            {
                result->push_back(V->take(i));
                if (i < V->size())
                    result->push_back((lexeme *)interleave->clone());
            }
            delete cur;
            cur = result;
        }
        else if (strcmp(rule, "flatten") == 0)
        {
            if (auto V = cur->as<lex_V>())
            {
                lex_V *result = new lex_V();
                for (auto l : *V)
                {
                    if (auto V2 = l->as<lex_V>())
                    {
                        for (auto l2 : *V2)
                            result->push_back(l2);
                        V2->clear();
                    }
                    else
                    {
                        result->push_back(l);
                    }
                }
                V->clear();
                delete cur;
                cur = result;
            }
            else if (auto re = cur->as<lex_re>())
            {
                lex_V *result = new lex_V();
                auto groups = re->groups;
                for (int i=1; i<groups->size(); i++)
                    result->push_back(groups->take(i));
                delete cur;
                cur = result;
            }
        }
        else if (strcmp(rule, "aggregate") == 0)
        {
            auto V = new lex_V();
            V->push_back(cur);
            cur = V;
        }
        else if (strcmp(rule, "group") == 0)
        {
            const char *pattern = selector->match()->data();
            pattern++;
            auto tmp = select_regex(cur, pattern, true);
            delete cur;
            cur = tmp;
        }
        else if (strcmp(rule, "loop") == 0)
        {
            const char *pattern = selector->match()->data();
            pattern+=2;

            lex_V *result = new lex_V();
            for (auto l : *prepare_V(cur))
                result->push_back(select_regex(l, pattern, true));
            delete cur;
            cur = result;
        }
        else if (strcmp(rule, "array") == 0)
        {
            const char *pattern = selector->match()->data();
            pattern++;

            lexeme *tmp = cur;
            cur = (lexeme *)tmp->clone();

            ast::_re::regex_mask whitespace;
            whitespace.toggle_escape('s');

            lex_V *result = new lex_V();

            while (*pattern != ']')
            {
                while (*pattern && whitespace[*pattern])
                    pattern++;
                if (*pattern == ',')
                {
                    pattern++;
                    //store the current result
                    auto V = prepare_V(cur);
                    for (int i = 0; i < V->size(); i++)
                        result->push_back(V->take(i));
                    delete cur;

                    //start over
                    cur = (lexeme *)tmp->clone();
                }
                while (apply_selector(cur, pattern))
                {
                    if (*pattern == '|' && cur->as<lex_V>() && cur->as<lex_V>()->size() == 0)
                    {
                        pattern++;
                        delete cur;
                        //start over
                        cur = (lexeme *)tmp->clone();
                    }
                    else if (*pattern == '|')
                    {
                        auto skip = ast::_re::match_regex(pattern, selector_skip_regex);
                        delete skip;
                        break;
                    }
                }
            }
            //store the current result
            auto V = prepare_V(cur);
            for (int i = 0; i < V->size(); i++)
                result->push_back(V->take(i));
            delete cur;
            cur = result;
        }
        else
        {
            printf("Unknown rule %s\n", rule);
            //error here!
            return false;
        }
        return true;
    }

    lexeme *ast::_rp::select_regex(const lexeme *l, char_cptr &pattern, bool internal)
    {
        if (!l)
            return nullptr;
        lex_re *result = new lex_re();
        while (*pattern)
        {
            auto literals = ast::_re::match_regex(pattern, "[^$\\\\]*");
            result->append(literals);
            if (*pattern == '\\')
            {
                pattern++;
                result->append(get_escape(pattern));
            }
            if (*pattern == '$')
            {
                lexeme *cur = (lexeme *)l->clone();
                pattern++;
                while (apply_selector(cur, pattern))
                {
                    if (*pattern == '|' && cur->as<lex_V>() && cur->as<lex_V>()->size() == 0)
                    { //start over
                        pattern++;
                        delete cur;
                        cur = (lexeme *)l->clone();
                    }
                    else if (*pattern == '|')
                    {
                        auto skip = ast::_re::match_regex(pattern, selector_skip_regex);
                        delete skip;
                        break;
                    }
                }
                result->append(cur);
                //append the subgroups to result->groups
                if (auto V = cur->as<lex_V>())
                {
                    for (int i = 0; i < V->size(); i++)
                        result->groups->push_back(V->take(i));
                    delete cur;
                }
            }
        }
        return result;
    }

    lexeme *ast::_rp::replace_regex(lexeme *l, char_cptr pattern)
    {
        auto tmp = select_regex(l, pattern);
        delete l;
        return tmp;
    }

    // lexeme *ast::_rp::select_regex(const lexeme *l, char_cptr &pattern, bool internal)
    // {
    //     if (!l)
    //         return nullptr;
    //     lexeme *cur = nullptr;
    //     lex_v<char> *v = nullptr;
    //     lex_V *V = nullptr;

    //     while(*pattern)
    //     {
    //         char c = *pattern;
    //         if (c == '$')
    //         {
    //             if (cur)
    //             {
    //                 if (!V)
    //                     V = new lex_V();
    //                 V->push_back(cur);
    //                 cur = nullptr;
    //             }
    //             if (v)
    //             {
    //                 if (!V)
    //                     V = new lex_V();
    //                 V->push_back(v);
    //                 v = nullptr;
    //             }
    //             cur = (lexeme *)l->clone();
    //             pattern++;

    //             while (*pattern)
    //             {
    //                 c = *pattern;
    //                 if (c == '.')
    //                 {
    //                     //skip the dot
    //                     pattern++;
    //                 }
    //                 else if (c >= '0' && c <= '9')
    //                 { //start subscripting
    //                     int n = c - '0';
    //                     pattern++;
    //                     while (*pattern >= '0' && *pattern <= '9')
    //                     {
    //                         n = n * 10 + (*pattern - '0');
    //                         pattern++;
    //                     }

    //                     lexeme *e = element_at(cur, n);
    //                     if (e)
    //                     {
    //                         e = (lexeme *)e->clone();
    //                         delete cur;
    //                         cur = e;
    //                     }
    //                     else
    //                     {
    //                         delete cur;
    //                         cur = new lex_V();
    //                     }
    //                 }
    //                 else if (c == '[')  //TODO FIX THIS
    //                 { //start range
    //                     //use another regex to parse the range
    //                     pattern++;
    //                     const char *range_re = "(\\s*" //skip spaces
    //                                             "<min>(-?\\d+)?\\s*" //start index
    //                                             "(<>:\\s*<>(-?\\d+)?)?\\s*,?)*\\]"; //end index
    //                     //(\s*(-?\d+)?\s*(:\s*(-?\d+)?)?\s*,?)*
    //                     lex_re *range = ast::_re::match_regex(pattern, range_re);
    //                     //for each match of group 0
    //                         //subgroup 0 is the start index
    //                         //if subgroup 1 is not empty, there is a :
    //                             //and its subgroup 0 is the end index
    //                     if (range)
    //                     {
    //                         lex_V *new_V = new lex_V();
    //                         for (auto i : *(lex_V *)(*range)[0])
    //                         {
    //                             int start = 0;
    //                             int end = 0;
    //                             lex_re *g = i->as<lex_re>();
    //                             if (g)
    //                             {
    //                                 if (((lex_V *)(*g)[0])->size() > 0)
    //                                 {
    //                                     start = atoi(((lex_V *)(*g)[0])->at(0)->as<lex_v<char>>()->data());
    //                                     end = start;
    //                                 }
    //                                 if (((lex_V *)(*g)[1])->size() > 0)
    //                                 {
    //                                     end = -1;
    //                                     if ( (*((lex_V *)(*g)[1])->at(0)->as<lex_re>())[0]->as<lex_V>()->size() > 0)
    //                                     {
    //                                         end = atoi((*((lex_V *)(*g)[1])->at(0)->as<lex_re>())[0]->as<lex_V>()->at(0)->as<lex_v<char>>()->data());
    //                                     }
    //                                 }
    //                                 if (start < 0) start = cur->as<lex_V>()->size() + start;
    //                                 if (end < 0) end = cur->as<lex_V>()->size() + end;
    //                                 if (start < 0) start = 0;
    //                                 if (end < 0) end = 0;
    //                                 if (start >= cur->as<lex_V>()->size()) start = cur->as<lex_V>()->size() - 1;
    //                                 if (end >= cur->as<lex_V>()->size()) end = cur->as<lex_V>()->size() - 1;
    //                                 if (start > end)
    //                                 {
    //                                     for (int i = start; i >= end; i--)
    //                                     {
    //                                         lexeme *e = (lexeme *)element_at(cur, i)->clone();
    //                                         if (e)
    //                                         {
    //                                             new_V->push_back(e);
    //                                         }
    //                                     }
    //                                 }
    //                                 else
    //                                 {
    //                                     for (int i = start; i <= end; i++)
    //                                     {
    //                                         lexeme *e = (lexeme *)element_at(cur, i)->clone();
    //                                         if (e)
    //                                         {
    //                                             new_V->push_back(e);
    //                                         }
    //                                     }
    //                                 }
    //                             }
    //                         }
    //                         delete range;
    //                         delete cur;
    //                         cur = new_V;
    //                     }
    //                 }
    //                 else if (c == '(')
    //                 {
    //                     pattern++; 
    //                     lexeme *sub = select_regex(cur, pattern, true);
    //                     if (sub)
    //                     {
    //                         delete cur;
    //                         cur = sub;
    //                     }
    //                     else
    //                     {
    //                         delete cur;
    //                         cur = new lex_V();
    //                     }
    //                     if (*pattern == ')')
    //                         pattern++;
    //                 }
    //                 else if (c == '|')
    //                 {
    //                     if (cur->as<lex_V>() && cur->as<lex_V>()->size() == 0)
    //                     {
    //                         delete cur;
    //                         cur = nullptr;
    //                         pattern++;
    //                     }
    //                     else
    //                     {
    //                         //skip all alernatives
    //                         const char *p = "(\\|($("
    //                                              "(\\d+)|" //subscript
    //                                              "(!)|" //concatenate
    //                                              "(\\<[^>]*\\>)|"  //named rule
    //                                              "(\\*?\\(\\p\\))|" //nested groups
    //                                              "(\\{[^\\}]*\\))|" //interleave
    //                                              "(\\.)|" //separator
    //                                              "(\\[([^\\]]*)\\))|" //range
    //                                         ")*))+";
    //                         ast::_re::match_regex(pattern, p);
    //                     }
    //                 }
    //                 else if (c == '*' && (*(pattern + 1) == '('))
    //                 { //start group
    //                     pattern+=2;
    //                     auto cur_V = cur->as<lex_V>();
    //                     if (cur_V)
    //                     {
    //                         char_cptr group_start = pattern;
    //                         lex_V *new_V = new lex_V();
    //                         for (auto i = cur_V->begin(); i != cur_V->end(); i++)
    //                         {
    //                             pattern = group_start;
    //                             lexeme *sub = select_regex(*i, pattern, true);
    //                             if (sub)
    //                             {
    //                                 new_V->push_back(sub);
    //                             }
    //                             else
    //                             {
    //                                 //ignore empty groups
    //                             }
    //                         }
    //                         if (*pattern == ')')
    //                         {
    //                             pattern++;
    //                         }
    //                         delete cur;
    //                         cur = new_V;
    //                         if (new_V->size() == 1)
    //                         {
    //                             lexeme *tmp = new_V->at(0);
    //                             new_V->clear();
    //                             delete new_V;
    //                             cur = tmp;
    //                         }
    //                     }
    //                     else
    //                     {
    //                         lexeme *sub = select_regex(cur, pattern, true);
    //                         if (sub)
    //                         {
    //                             delete cur;
    //                             cur = sub;
    //                         }
    //                         else
    //                         {
    //                             delete cur;
    //                             cur = new lex_V();
    //                         }
    //                     }
    //                 }
    //                 else if (c == '{')
    //                 { //start interleaving
    //                     //skip the bracket
    //                     pattern++;
    //                     //collect the separator
    //                     lex_v<char> *sep = new lex_v<char>();
    //                     while (*pattern && *pattern != '}')
    //                     {
    //                         if (*pattern == '\\')
    //                         {
    //                             pattern++;
    //                             sep->push_back(get_escape(pattern));
    //                         }
    //                         else
    //                         {
    //                             sep->push_back(*pattern);
    //                         }
    //                         pattern++;
    //                     }
    //                     if (*pattern == '}')
    //                     {
    //                         pattern++;
    //                     }
    //                     else
    //                     {
    //                         delete sep;
    //                         break;
    //                     }
    //                     //interleave the contents of cur with the contents of sep
    //                     lex_V *cur_V = cur->as<lex_V>();
    //                     if (cur_V)
    //                     {
    //                         //create a new lex_V
    //                         lex_V *new_V = new lex_V();
    //                         for (auto i = cur_V->begin(); i != cur_V->end(); i++)
    //                         {
    //                             new_V->push_back(*i);
    //                             if (i != cur_V->end() - 1)
    //                             {
    //                                 new_V->push_back((lexeme *)sep->clone());
    //                             }
    //                         }
    //                         cur_V->clear();
    //                         delete cur;
    //                         delete sep;
    //                         cur = new_V;
    //                         if (new_V->size() == 1)
    //                         {
    //                             lexeme *tmp = new_V->at(0);
    //                             new_V->clear();
    //                             delete new_V;
    //                             cur = tmp;
    //                         }
    //                     }
    //                     else
    //                     {
    //                         //do nothing
    //                     }
    //                 }
    //                 else if (c == '!')
    //                 { //concatenate
    //                     cur = ast::_f::concat_decorator<char>(cur);
    //                     pattern++;
    //                 }
    //                 else if (c == '<')
    //                 { //named rules
    //                     pattern++;
    //                     auto name_start = pattern;
    //                     while (*pattern && *pattern != '>')
    //                     {
    //                         pattern++;
    //                     }
    //                     char *rule_name = new char[pattern - name_start + 1];
    //                     strncpy(rule_name, name_start, pattern - name_start);
    //                     rule_name[pattern - name_start] = '\0';
    //                     if (*pattern == '>')
    //                     {
    //                         pattern++;
    //                     }

    //                     lex_V *cur_V = cur->as<lex_V>();
    //                     if (cur_V)
    //                     {
    //                         //delete all elements which do not match the name
    //                         for (int i=0; i<cur_V->size(); i++)
    //                         {
    //                             lexeme *e = cur_V->at(i);
    //                             if (!e->same_rule(rule_name))
    //                             {
    //                                 delete e;
    //                                 cur_V->erase(cur_V->begin() + i);
    //                                 i--;
    //                             }
    //                         }
    //                     }
    //                     else
    //                     {
    //                         auto cur_re = cur->as<lex_re>();   //TODO FIX THIS
    //                         if (cur_re)
    //                         {
    //                             auto new_V = new lex_V();
    //                             cur_re->print();
    //                         }
    //                         else if (!cur->same_rule(rule_name))
    //                         {
    //                             delete cur;
    //                             cur = new lex_V();
    //                         }
    //                     }
    //                     delete [] rule_name;
    //                 }
    //                 else
    //                 { //something else, end of expression
    //                     break;
    //                 }
    //             }
    //         }
    //         else if (internal && c == ')')
    //         {   //end of group
    //             break;
    //         }
    //         else
    //         {
    //             if (c == '\\')
    //             {
    //                 pattern++;
    //                 c = get_escape(pattern);
    //             }
    //             if (cur)
    //             {
    //                 if (!V)
    //                     V = new lex_V();
    //                 V->push_back(cur);
    //                 cur = nullptr;
    //             }
    //             if (!v)
    //                 v = new lex_v<char>();
    //             v->push_back(c);
    //             pattern++;
    //         }
    //     }

    //     if (cur)
    //     {
    //         if (!V)
    //             return cur;
    //         V->push_back(cur);
    //         return V;
    //     }
    //     else if (v)
    //     {
    //         if (!V)
    //             return v;
    //         V->push_back(v);
    //         return V;
    //     }
    //     else if (V)
    //     {
    //         return V;
    //     }
    //     else
    //     {
    //         return nullptr;
    //     }
    // }

}