#pragma once

#include "ast_char.h"
#include "regex.h"

namespace ast_grammar
{
    using namespace ast_char;

    //rule: name<param_list> ::= definition;
    //if param_list:
    //  template<param_list>
    //ast_alias(name) = definition; ast_alias_en;

    //----Decorators----
    //rep is already defined
    template<char start, char escape, char end>
    ast_alias(delimited) = token_delimited<start, escape, end>; ast_alias_end;

    //----Primary operators----
    ast_regex_rule(character, "('\\\\.')|('.')");
    ast_alias(string) = delimited<'"', '\\', '"'>; ast_alias_end;
    ast_alias(regex_pattern) = delimited<'/', '\\', '/'>; ast_alias_end;
    ast_alias(replace_pattern) = delimited<'/', '\\', '/'>; ast_alias_end;

    ast_regex_rule(name, "[a-zA-Z_][a-zA-Z0-9_]*(::[a-zA-Z_][a-zA-Z0-9_]*)*");
    ast_regex_rule(positive_integer, "[0-9]+");
    ast_regex_rule(integer, "[-+]?[0-9]+");
    ast_regex_rule(hex_number, "0x[0-9a-fA-F]+");
    ast_regex_rule(dec_number, "[-+]?[0-9]+(\\.[0-9]+)?");

    ast_alias(number) = choice<dec_number, hex_number>; ast_alias_end;

    ast_regex_rule(comment, "--([^\\n]*)\\n");

    ast_regex_rule(whitespace, "[ \\t\\n\\r]+");
    ast_alias(S) = drop<any<choice<whitespace, comment>>>; ast_alias_end;
    //----Rules----
    class expression;
    class call;

    ast_alias(argument)  = choice<character, string, number, call, select<seq<token<'('>, S, expression, S, token<')'>>,1>>; ast_alias_end;

    //------------------------------------------------------------------------------------------------
    ast_alias(call_params) = opt<seq<token<'<'>, S, list<argument, seq<token<','>, S>>, S, token<'>'>>>; ast_alias_end;
    ast_alias(call) = seq<name, call_params>; ast_alias_end;
    ast_alias(primary) = choice<call, character, string, regex_pattern, select<seq<token<'('>, S, expression, S, token<')'>>,1>>;
    ast_alias_decorator(l)
    {
        if (l->rule == ast_id(call))
        {
            auto V = l->as<lex_V>();
            auto name = V->at(0);
            auto params = V->at(1)->as<lex_V>();
            if (params->size() == 0)
            {
                l = V->take(0);
                delete V;
                return l;
            }
            else //V->size must be >1 
            {
                V->replace(1, "<$0.1{, }>");
                return V;
            }
        }
        else if (l->rule == ast_id(character))
        {
            //l is a lex_v<char>, use a ast::_t::token<char, c>
            const char *p = "token<$0>";
            return replace_regex(l, p);
        }
        else if (l->rule == ast_id(string))
        {
            const char *p = "token_string<ast_str(\"$0\"\\)>";
            return replace_regex(l, p);
        }
        else if (l->rule == ast_id(regex_pattern))
        {
            const char *p = "regex<ast_str(\"$0\"\\)>";
            return replace_regex(l, p);
        }
        else //parenthesised expression
        {
            return l;
        }
    }
    ast_alias_end;

    ast_alias(decorator) = seq<token<'.'>, choice<replace_pattern, call>>; ast_alias_end;

    ast_alias(decorated) = seq<primary, any<decorator>>;
    ast_alias_decorator(l)
    {
        auto V = l->as<lex_V>();
        if (V->size() == 1)
        {
            auto l = V->take(0);
            delete V;
            return l;
        }
        else //V->size must be >1 
        {
            auto primary = V->take(0);
            auto decorators = V->take(1)->as<lex_V>();
            delete V;
            if (decorators->size() == 0)
            {
                return primary;
            }
            else //V->size must be >1 
            {
                for (auto d = decorators->begin(); d != decorators->end(); d++)
                {
                    if ((*d)->as<lex_V>()->at(1)->rule_is<replace_pattern>())
                    {
                        //replace
                        const char *pattern = "replace<$,";
                        primary = replace_regex(primary, pattern);
                        pattern = "ast_str(\"$1\"\\)>";
                        primary->as<lex_V>()->push_back(select_regex(*d, pattern));
                    }
                    else
                    {
                        auto V2 = (*d)->as<lex_V>()->at(1)->as<lex_V>();
                        auto params = V2->at(1)->as<lex_V>();
                        if (params->size() == 0)
                        {
                            params->insert(params->begin(), primary);
                            primary = (*V2)["$0<$1{, }>"];
                        }
                        else //V->size must be >1 
                        {
                            params = params->at(0)->as<lex_V>()->at(1)->as<lex_V>();
                            params->insert(params->begin(), primary);
                            primary = (*V2)["$0<$1.0.1{, }>"];
                        }
                    }
                }
                delete decorators;
                return primary;
            }
        }
    }
    ast_alias_end;

    ast_regex_rule(rep_flag, "[*?+]");
    ast_alias(factor) = seq<decorated, opt<choice<rep_flag, seq<token<'{'>, S, opt<number>, opt<seq<S, token<','>, S, number>>, S, token<'}'>>>>>;
    //if there is a rep_flag or {}, then the factor is a rep<decorated>
    ast_alias_decorator(l)
    {
        auto V = l->as<lex_V>();
        auto r = (*V)["$1.0"];
        if (r->rule_is<rep_flag>())
        {
            auto o = r->as<lex_re>()->at(0);;
            if (o == '*')
            {
                auto tmp = (*V)["rep<$0, 0, -1>"];
                delete l;
                return tmp;
            }
            else if (o == '+')
            {
                auto tmp = (*V)["rep<$0, 1, -1>"];
                delete l;
                return tmp;
            }
            else //if (O->o == '?') //only other option
            {
                auto tmp = (*V)["rep<$0, 0, 1>"];
                delete l;
                return tmp;
            }
        }

        auto rV = r->as<lex_V>();
        if (rV && rV->size()>0) //{a,b}
        {
            rV->print();
            auto tmp = (*V)["rep<$0, "]->as<lex_V>();
            auto min = (*V)["$1.0.1.0"];
            if (min->rule_is<number>())
            {
                tmp->push_back(min);
            }
            else
            {
                tmp->push_back((*tmp)["0"]);
            }
            tmp->push_back((*tmp)[", "]);
            auto max = (*V)["$1.0.2.0.1"];
            if (max->rule_is<number>())
            {
                tmp->push_back(max);
            }
            else
            {
                tmp->push_back((*tmp)["-1"]);
            }
            tmp->push_back((*tmp)[">"]);
            delete l;
            return tmp;
        }
        return l;
    }
    ast_alias_end;

    ast_alias(term) = list<factor, S>;
    ast_alias_decorator(l)
    {
        auto V = l->as<lex_V>();
        if (V->size() == 1)
        {
            auto l = V->at(0);
            V->at(0) = nullptr;
            delete V;
            return l;
        }
        else //V->size must be >1 
        {
            const char *pattern = "seq<${, }>";
            return replace_regex(l, pattern);
        }
    }
    ast_alias_end;

    ast_alias(expression) = list<term, seq<S, token<'|'>, S>>;
    ast_alias_decorator(l)
    {
        auto V = l->as<lex_V>();
        if (V->size() == 1)
        {
            auto l = V->at(0);
            V->at(0) = nullptr;
            delete V;
            return l;
        }
        else //V->size must be >1 
        {
            const char *pattern = "choice<${, }>";
            return replace_regex(l, pattern);
        }
    }
    ast_alias_end;

    ast_regex_rule(valid_type, "([a-zA-Z_][a-zA-Z0-9_]*\\s*(<[^>]*>)?\\s*(\\*|\\&|\\[\\])?)*");
    // ast_alias(param) = choice<seq<valid_type, S, name>, name>; ast_alias_end;
    ast_alias(param_list) = replace<seq<token<'<'>, S, list<valid_type, seq<S, token<','>, S>>, S, token<'>'>>,ast_str("<$1{, }>")>; ast_alias_end;

    ast_string_rule(equal_sign, "::=");
    ast_alias(rule) = seq<name, opt<param_list>, S, equal_sign, S, expression, S, token<';'>>; ast_alias_end;

    ast_regex_rule(copyline, "$([^\\n]*)\\n");
    
    ast_alias(grammar) = seq<S, list<choice<drop<out<rule>>, drop<out<replace<copyline,ast_str("$1")>>>>, S>>; ast_alias_end;
    // ast_alias(grammar) = trace_on<seq<S, list<choice<rule, copyline>, S>>>; ast_alias_end;

};