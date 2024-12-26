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
                                    "(@\\<\\>)|"
                                    "(\\*@\\<\\>)|"
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
                                    "(<name>@\\<\\>)|"
                                    "(\\*<setname>@\\<\\>)|"
                                    "(<separator>\\.)|"
                                    "(<concat>\\!)|"
                                    "(<regex>@\\/\\/)|"
                                    "(\\{<interleave>([^}]*)\\})|"
                                    "(<flatten>\\%)|"
                                    "(<aggregate>\\@)|"
                                    "(<group>@\\(\\))|"
                                    "(\\*<loop>@\\(\\))|"
                                    "(<array>@\\[\\])";

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

    bool ast::_rp::apply_selector(lexeme *&cur, char_cptr &pattern)
    {
        lex_re *selector = ast::_re::match_regex(pattern, selector_regex);
        if (!selector)
            return false;

        const char *rule = selector->groups->at(0)->rule;
        if (strcmp(rule, "range") == 0)
        {
            int min = 0;
            int max = -1;
            lex_re *range = (lex_re *)selector->groups->at(0);
            for (int i=0; i < range->groups->size(); i++)
            {
                lexeme *r = range->groups->at(i);
                if (r->same_rule("min"))
                    max = min = atoi_n(r->as<lex_re>()->match->data(), r->as<lex_re>()->match->size());
                else if (r->same_rule("colon"))
                    max = -1;
                else if (r->same_rule("max"))
                    max = atoi_n(r->as<lex_re>()->match->data(), r->as<lex_re>()->match->size());
            }
            delete range;
            bool single = min == max;
            auto V = prepare_V(cur);
            int sz = V->size();
            if (min < 0)
                min = sz + min;

            if (single)
            {
                if (min < 0 || min >= sz)
                {
                    delete cur;
                    cur = new lex_V();
                }
                else
                {
                    auto tmp = V->take(min);
                    delete cur;
                    cur = tmp;
                }
            }
            else
            {
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
                    result->push_back(V->take(i));
                delete cur;
                cur = result;
            }
        }
        else if (strcmp(rule, "setname") == 0)
        {
            auto n = selector->groups->at(0)->as<lex_v<char>>();
            char *name = new char[n->size() - 1];
            strncpy(name, n->data()+1, n->size()-2);
            name[n->size()-2] = 0;
            lex_V *V = prepare_V(cur);
            for (int i = 0; i<V->size(); i++)
            {
                lexeme *l = V->at(i);
                if (l->own_rule)
                    delete l->rule;
                l->rule = strdup(name);
                l->own_rule = true;
            }
            delete [] name;
        }
        else if (strcmp(rule, "name") == 0)
        {
            lex_V *result = new lex_V();
            lex_V *V = prepare_V(cur);
            auto n = selector->groups->at(0)->as<lex_v<char>>();
            char *name = new char[n->size() - 1];
            strncpy(name, n->data()+1, n->size()-2);
            name[n->size()-2] = 0;
            for (int i = 0; i<V->size(); i++)
            {
                lexeme *l = V->at(i);
                if (*name != '^')
                {
                    if (l->same_rule(name))
                        result->push_back(V->take(i));
                }
                else
                {
                    if (!l->same_rule(name + 1))
                        result->push_back(V->take(i));
                }
            }
            delete [] name;
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
            const char *pattern = selector->match->data();
            int sz = selector->match->size();
            char *tmp = new char[sz - 1];
            strncpy(tmp, pattern + 1, sz - 2);
            tmp[selector->match->size() - 2] = 0;

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
            char *str = src->match->data();
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
            lex_re *interleave = (lex_re *)selector->groups->at(0);
            for (int i = 0; i<V->size(); i++)
            {
                result->push_back(V->take(i));
                if (i < V->size() - 1)
                {
                    auto l = (lexeme *)interleave->match->clone();
                    l->rule = "separator";
                    result->push_back(l);
                }
            }
            delete cur;
            cur = result;
        }
        else if (strcmp(rule, "flatten") == 0)
        {
            if (auto V = cur->as<lex_V>())
            {
                if (V->size() > 1)
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
                        else if (auto re = l->as<lex_re>())
                        {
                            auto groups = re->groups;
                            for (int i=0; i<groups->size(); i++)
                                result->push_back(groups->take(i));
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
                else
                {
                    cur = V->take(0);
                    delete V;
                }
            }
            else if (auto re = cur->as<lex_re>())
            {
                lex_V *result = new lex_V();
                auto groups = re->groups;
                for (int i=0; i<groups->size(); i++)
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
            const char *pattern = selector->match->data();
            pattern++;
            auto tmp = select_regex(cur, pattern, true);
            delete cur;
            cur = tmp;
        }
        else if (strcmp(rule, "loop") == 0)
        {
            lex_V *result = new lex_V();
            for (auto l : *prepare_V(cur))
            {
                const char *pattern = selector->match->data() + 2;
                result->push_back(select_regex(l, pattern, true));
            }
            delete cur;
            cur = result;
        }
        else if (strcmp(rule, "array") == 0)
        {
            const char *pattern = selector->match->data();
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
        lexeme *result = nullptr;
        while (*pattern && !(internal && *pattern == ')')) 
        {
            const char *lit_re;
            if (internal)
                lit_re = "(([^\\)$\\\\]|(\\\\.)?)%)+";
            else
                lit_re = "(([^$\\\\]|(\\\\.)?)%)+";
            auto literals = ast::_re::match_regex(pattern, lit_re);
            if (literals)
            {
                lex_re *re = result ? result->as<lex_re>() : nullptr;
                if (!re)
                {
                    re = new lex_re();
                    if (!result)
                        result = re;
                    else if (auto V = result->as<lex_V>())
                    {
                        re->append(V);
                        for (int i = 0; i < V->size(); i++)
                            re->groups->push_back(V->take(i));
                        delete V;
                        result = re;
                    }
                }
                const char *str = literals->match->data();
                for (int i = 0; i < literals->match->size(); i++, str++)
                {
                    char c = *str;
                    if (c == '\\')
                    {
                        i++; str++;
                        c = ast::_re::get_escape(str);
                    }
                    re->append(c);
                }
                delete literals;
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
                if (!result)
                    result = cur;
                else if (auto re = result->as<lex_re>())
                {
                    re->append(cur);
                    re->groups->push_back(cur);
                }
                else if (auto V = result->as<lex_V>())
                {
                    V->push_back(cur);
                }
                else
                {
                    lex_V *tmp = new lex_V();
                    tmp->push_back(result);
                    tmp->push_back(cur);
                    result = tmp;
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
}