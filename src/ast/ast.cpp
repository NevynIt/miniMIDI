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

    lexeme *ast::_rp::replace_regex(lexeme *l, char_cptr &pattern)
    {
        auto tmp = select_regex(l, pattern);
        delete l;
        return tmp;
    }

    lexeme *ast::_rp::select_regex(const lexeme *l, char_cptr &pattern)
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
                        pattern++;
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
                            delete sep;
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

}