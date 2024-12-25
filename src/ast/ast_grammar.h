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
    ast_alias(delimited) = token_delimited<start, escape, end>; ast_set_internal; ast_alias_end;

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
    ast_alias_declare(expression);
    ast_alias_declare(call);

    ast_alias(argument)  = choice<character, string, number, call, select<seq<token<'('>, S, expression, S, token<')'>>,1>>; ast_alias_end;

    //------------------------------------------------------------------------------------------------
    ast_alias(call_params) = opt<replace<seq<token<'<'>, S, list<argument, seq<token<','>, S>>, S, token<'>'>>,ast_str("$[1:-2]")>>; ast_alias_end;
    ast_alias_define(call) = seq<name, call_params>; ast_alias_define_end(call);
    ast_alias(primary) = choice<call, character, string, regex_pattern, select<seq<token<'('>, S, expression, S, token<')'>>,1>>;
    ast_alias_decorator(l)
    {
        if (l->rule_is<call>())
        {
            return replace_regex(l, "$0$1(<${, }>)");
        }
        else if (l->rule_is<character>())
        {
            return replace_regex(l, "token<$0>");
        }
        else if (l->rule_is<string>())
        {
            return replace_regex(l, "token_string<ast_str(\"$0\"\\)>");
        }
        else if (l->rule_is<regex_pattern>())
        {
            return replace_regex(l, "regex<ast_str(\"$0\"\\)>");
        }
        else //parenthesised expression
        {
            return l;
        }
    }
    ast_alias_end;

    ast_alias(decorator) = select<seq<token<'.'>, choice<replace_pattern, call>>,1>; ast_alias_end;

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
        {  //TODO From here: it's now just a single list
            auto primary = V->take(0);
            for (int i = 1; i<V->size(); i++)
            {
                auto d = V->take(i);
                if (d->rule_is<replace_pattern>())
                {
                    //replace
                    primary = replace_regex(primary, "replace<$,");
                    primary->as<lex_V>()->push_back(replace_regex(d, "ast_str(\"$\"\\)>"));
                }
                else if (d->rule_is<call>())
                {
                    auto params = d->as<lex_V>()->at(1)->as<lex_V>();
                    params->insert(params->begin(), primary);
                    primary = (*d)["$0<$1{, }>"];
                    delete d;
                }
            }
            delete V;
            return primary;
        }
    }
    ast_alias_end;

    ast_regex_rule(rep_flag, "[*?+]");
    ast_alias(rep_min_max) = seq<token<'{'>, S, opt<number>, opt<seq<S, token<','>, S, number>>, S, token<'}'>>; ast_alias_end;
    ast_alias(factor) = seq<decorated, opt<choice<rep_flag, rep_min_max>>>;
    //if there is a rep_flag or {}, then the factor is a rep<decorated>
    ast_alias_decorator(l)
    {
        auto V = l->as<lex_V>();
        auto r = (*V)["$1"];
        if (r->rule_is<rep_flag>())
        {
            auto o = r->as<lex_re>()->match()->at(0);
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
        else if (r->rule_is<rep_min_max>())
        {
            //TODO here
            delete l;
            return nullptr;
        }

        // auto rV = r->as<lex_V>();
        // l->print();
        // if (rV && rV->size()>0) //{a,b}
        // {
        //     rV->print();
        //     auto tmp = (*V)["rep<$0, "]->as<lex_V>();
        //     auto min = (*V)["$1.0.1.0"];
        //     if (min->rule_is<number>())
        //     {
        //         tmp->push_back(min);
        //     }
        //     else
        //     {
        //         tmp->push_back((*tmp)["0"]);
        //     }
        //     tmp->push_back((*tmp)[", "]);
        //     auto max = (*V)["$1.0.2.0.1"];
        //     if (max->rule_is<number>())
        //     {
        //         tmp->push_back(max);
        //     }
        //     else
        //     {
        //         tmp->push_back((*tmp)["-1"]);
        //     }
        //     tmp->push_back((*tmp)[">"]);
        //     delete l;
        //     return tmp;
        // }
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
            return replace_regex(l, "seq<${, }>");
        }
    }
    ast_alias_end;

    ast_alias_define(expression) = list<term, seq<S, token<'|'>, S>>;
    ast_alias_define_decorator(expression, l)
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
            return replace_regex(l, "choice<${, }>");
        }
    }
    ast_alias_define_end(expression);

    ast_regex_rule(valid_type, "([a-zA-Z_][a-zA-Z0-9_]*\\s*(<[^>]*>)?\\s*(\\*|\\&|\\[\\])?)*");
    // ast_alias(param) = choice<seq<valid_type, S, name>, name>; ast_alias_end;
    ast_alias(param_list) = replace<seq<token<'<'>, S, list<valid_type, seq<S, token<','>, S>>, S, token<'>'>>,ast_str("<$[1:-2]{, }>")>; ast_alias_end;

    ast_string_rule(equal_sign, "::=");
    ast_alias(rule) = seq<name, opt<param_list>, S, equal_sign, S, expression, S, token<';'>>; ast_alias_end;

    ast_regex_rule(copyline, "$([^\\n]*)\\n");
    
    ast_alias(grammar) = seq<S, list<choice<drop<out<rule>>, drop<out<replace<copyline,ast_str("$1")>>>>, S>>; ast_alias_end;
};