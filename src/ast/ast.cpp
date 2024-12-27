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
        lex_re *selector = ast::_re::match_regex(pattern, selector_regex, lex_re::storage_type::REF); //static
        if (!selector)
            return false;
        lex_re *sel = selector->groups->at(0)->as<lex_re>(); 

        // cur->print();
        // printf("\n");
        // printf("selector %.*s: %.*s\n", sel->rule_len(), sel->rule, sel->size(), sel->data());

        if (sel->same_rule("range"))
        {
            int min = 0;
            int max = -1;
            for (int i=0; i < sel->groups->size(); i++)
            {
                lexeme *r = sel->groups->at(i);
                if (r->same_rule("min"))
                    max = min = atoi_n(r->as<lex_re>()->data(), r->as<lex_re>()->size());
                else if (r->same_rule("colon"))
                    max = -1;
                else if (r->same_rule("max"))
                    max = atoi_n(r->as<lex_re>()->data(), r->as<lex_re>()->size());
            }
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
        else if (sel->same_rule("setname"))
        {
            const char *name = sel->data()+1;
            lex_V *V = prepare_V(cur);
            for (int i = 0; i<V->size(); i++)
            {
                lexeme *l = V->at(i); 
                l->rule = name;
            }
        }
        else if (sel->same_rule("name"))
        {
            lex_V *result = new lex_V();
            lex_V *V = prepare_V(cur);
            const char *name = sel->data()+1;
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
            delete cur;
            cur = result; 
        }
        else if (sel->same_rule("separator"))
        {
            return true;
        }
        else if (sel->same_rule("concat"))
        {
            lex_re *result = new lex_re();
            result->append(cur);
            delete cur;
            cur = result; 
        }
        else if (sel->same_rule("regex"))
        {
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

            regex_unit ru;
            ru.pattern = sel->data()+1;
            ru.pattern_end = ru.pattern + sel->size()-2;
            const char *str = src->data();
            ru.stream_end = str + src->size();
            ru.storage = src->get_storage_type(); //same storage as src
            lex_re *result = ru.match_regex(str);

            delete cur;

            if (result)
                cur = result;
            else
                cur = new lex_V();
        }
        else if (sel->same_rule("interleave"))
        {
            lex_V *result = new lex_V();
            lex_V *V = prepare_V(cur);
            for (int i = 0; i<V->size(); i++)
            {
                result->push_back(V->take(i));
                if (i < V->size() - 1)
                {
                    auto l = (lexeme *)sel->match->clone();
                    l->rule = "separator";
                    result->push_back(l);
                }
            }
            delete cur;
            cur = result;
        }
        else if (sel->same_rule("flatten"))
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
                            if (re->groups)
                                for (int i=0; i<re->groups->size(); i++)
                                    result->push_back(re->groups->take(i));
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
                if(re->groups)
                    for (int i=0; i<re->groups->size(); i++)
                        result->push_back(re->groups->take(i));
                delete cur;
                cur = result;
            }
        }
        else if (sel->same_rule("aggregate"))
        {
            auto V = new lex_V();
            V->push_back(cur);
            cur = V;
        }
        else if (sel->same_rule("group"))
        {
            const char *pattern = sel->data() + 1;
            auto tmp = select_regex(cur, pattern, true);
            delete cur;
            cur = tmp;
        }
        else if (sel->same_rule("loop"))
        {
            lex_V *result = new lex_V();
            for (auto l : *prepare_V(cur))
            {
                const char *pattern = sel->data() + 1;
                result->push_back(select_regex(l, pattern, true));
            }
            delete cur;
            cur = result;
        }
        else if (sel->same_rule("array"))
        {
            const char *pattern = sel->data()+1;

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
                        auto skip = ast::_re::match_regex(pattern, selector_skip_regex,lex_re::storage_type::DROP); //static, no need to return anything
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
            printf("Unknown rule %s\n", sel->rule);
            //error here!
            return false;
        }
        delete selector;
        return true;
    }

    lexeme *ast::_rp::select_regex(const lexeme *l, char_cptr &pattern, bool internal)
    {
        if (!l)
            return nullptr;
        lex_re *result = new lex_re();
        while (*pattern && !(internal && *pattern == ')')) 
        {
            const char *lit_re;
            if (internal)
                lit_re = "(([^\\)$\\\\]|(\\\\.)?)%)+";
            else
                lit_re = "(([^$\\\\]|(\\\\.)?)%)+";
            auto literals = ast::_re::match_regex(pattern, lit_re, lex_re::storage_type::REF); //static
            if (literals)
            {
                const char *str = literals->data();
                for (int i = 0; i < literals->size(); i++, str++)
                {
                    char c = *str;
                    if (c == '\\')
                    {
                        i++; str++;
                        c = ast::_re::get_escape(str);
                    }
                    result->append(c);
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
                        auto skip = ast::_re::match_regex(pattern, selector_skip_regex, lex_re::storage_type::DROP); //static and no need to return anything
                        delete skip;
                        break;
                    }
                }
                result->append(cur);
                result->append_group(cur);
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