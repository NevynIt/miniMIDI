#pragma once

#include "ast_char.h"
#include "regex.h"

namespace ast_grammar
{
    using namespace ast_char;

    //----Primary operators----
    ast_regex_rule(character, R"( ('\\.') | ('.') )");
    ast_regex_rule(string, R"( @"" )");
    ast_regex_rule(regex_pattern, R"( @// )");
    ast_regex_rule(replace_pattern, R"( @## )");

    ast_regex_rule(name, "[a-zA-Z_][a-zA-Z0-9_]*(::[a-zA-Z_][a-zA-Z0-9_]*)*");
    ast_regex_rule(number, R"( <hex>(0x[0-9a-fA-F]+) | <dec>([\+\-]?\d+(\.\d+)?) )");

    ast_regex_drop(S, R"( ((\s+) | (--[^\n]*\n))* )");

    //----Rules----
    ast_alias_declare(expression);
    ast_alias_declare(call);

    ast_alias(argument)  = choice<character, string, number, call, select<seq<token<'('>, S, expression, S, token<')'>>,1>>; ast_alias_end;

    //------------------------------------------------------------------------------------------------
    ast_alias(call_params) = opt<seq<token<'<'>, S, list<argument, seq<token<','>, S>>, S, token<'>'>>>; ast_alias_end;
    ast_alias_define(call) = replace<seq<name, call_params>,ast_str("$0$1(<$[1:-2]{, }>)*<params>")>; ast_alias_define_end(call);
    ast_alias(primary) = choice<call, character, string, regex_pattern, select<seq<token<'('>, S, expression, S, token<')'>>,1>>;
    ast_alias_decorator(l)
    {
        //the call is already decorated
        if (l->rule_is<character>())
        {
            return replace_regex(l, "token<$>");
        }
        else if (l->rule_is<string>())
        {
            return replace_regex(l, "token_string<ast_str\\($\\)>");
        }
        else if (l->rule_is<regex_pattern>())
        {
            return replace_regex(l, "regex<ast_str(\"$%\")>");
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
        { 
            auto primary = V->take(0);
            for (int i = 1; i<V->size(); i++)
            {
                auto d = V->take(i);
                if (d->rule_is<replace_pattern>())
                {
                    //replace
                    auto re = replace_regex(primary, "replace<$,");
                    re->append(replace_regex(d, "ast_str(\"$%\"\\)>"));
                    primary = re;
                }
                else if (d->rule_is<call>())
                {
                    auto *re = d->as<lex_re>();
                    auto params = re->get_group("params");
                    if (!params)
                    {
                        re->append('<');
                        re->append(primary);
                        re->append_group(primary);
                        re->append('>');
                        primary = re;
                    }
                    else
                    {
                        auto g = params->as<lex_re>()->groups;
                        g->insert(g->begin(), primary);
                        primary = replace_regex(d, "$%($0\\<$1%<^separator>{, }\\>)");
                    }
                }
            }
            delete V;
            return primary;
        }
    }
    ast_alias_end;

    // ast_regex_rule(rep_flag, "[*?+]");
    // ast_alias(rep_min_max) = seq<token<'{'>, S, opt<number>, opt<seq<S, token<','>, S, number>>, S, token<'}'>>; ast_alias_end;
    ast_regex_rule(rep_range, R"( ( <flag>([*?+]) | ({<min>(\d+)?(<comma>,<max>(\d+)?)?}) )? )");
    ast_alias(factor) = seq<decorated, rep_range>;
    // ast_alias(factor) = seq<decorated, opt<choice<rep_flag, rep_min_max>>>;
    //if there is a rep_flag or {}, then the factor is a rep<decorated>
    ast_alias_decorator(l)
    {
        auto V = l->as<lex_V>();
        auto r = V->at(1)->as<lex_re>();

        if (!r->groups)
            return l;
        
        lex_re *re = new lex_re();
        if (auto fl = r->get_group("flag"))
        {
            if (fl->data()[0] == '?')
            {
                re->append("opt<");
                re->append(V->at(0));
                re->append(">");
            }
            else
            {
                re->append("rep<");
                re->append(V->at(0));
                if (fl->data()[0] == '*')
                    re->append(",0,-1>");
                else if (fl->data()[0] == '+')
                    re->append(",1,-1>");
            }
        }
        else
        {
            auto mn = r->get_group("min");
            auto cm = r->get_group("comma");
            auto mx = r->get_group("max");
            if (mn)
            {
                re->append(",");
                re->append(mn);
                if (mx)
                {
                    re->append(",");
                    re->append(mx);
                }
                else if (cm)
                {
                    re->append(",-1");
                }
                else
                {
                    re->append(",");
                    re->append(mn);
                }
            }
            else
            {
                re->append(",0");
                if (mx)
                {
                    re->append(",");
                    re->append(mx);
                }
                else
                {
                    re->append(",-1");
                }
            }
            re->append(">");
        }
        delete l;
        return re;
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

    ast_regex_rule(valid_type, "([a-zA-Z_][a-zA-Z0-9_]*\\s*(@<>)?\\s*(\\*|\\&|\\[\\])?)*");
    // ast_alias(param) = choice<seq<valid_type, S, name>, name>; ast_alias_end;
    ast_alias(param_list) = replace<seq<token<'<'>, S, list<valid_type, seq<S, token<','>, S>>, S, token<'>'>>,ast_str("<$[1:-2]{, }>")>; ast_alias_end;

    ast_string_rule(equal_sign, "::=");
    ast_alias(rule) = seq<name, opt<param_list>, S, equal_sign, S, expression, S, token<';'>>;
    ast_alias_decorator(l)
    {
        // l->print();

        // auto V = l->as<lex_V>();
        // auto name = V->take(0);
        // auto params = V->take(1);
        // auto expr = V->take(2);
        // delete V;
        // if (params)
        // {
        //     return replace_regex(l, "ast_internal_rule($0) = $2; ast_alias_end;");
        // }
        // else
        // {
        //     return replace_regex(l, "ast_internal_rule($0) = $1; ast_alias_end;");
        // }
        return l;
    }
    ast_alias_end;

    ast_regex_rule(copyline, "\\$<>([^\\n]*)\\n");
    
    ast_alias(grammar) = seq<S, list<choice<drop<out<rule>>, drop<out<replace<copyline,ast_str("$%")>>>>, S>>; ast_alias_end;
};