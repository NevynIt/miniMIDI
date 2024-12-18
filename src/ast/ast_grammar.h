#pragma once

#include "ast_char.h"
#include "regex.h"

namespace ast_grammar
{
    using namespace ast_char;

    //----Decorators----
    //rep is already defined
    template<char start, char escape, char end>
    ast_alias(delimited) = token_delimited<start, escape, end>; ast_alias_end;

    //----Primary operators----
    ast_regex_rule(character, "('\\\\.')|('.')");
    ast_alias(string) = delimited<'"', '\\', '"'>; ast_alias_end;
    ast_alias(regex_pattern) = replace<delimited<'/', '\\', '/'>,ast_str("regex<\"$0\">")>; ast_alias_end;
    ast_alias(replace_pattern) = replace<delimited<'/', '\\', '/'>,ast_str("replace<\"$0\">")>; ast_alias_end;

    ast_regex_rule(name, "[a-zA-Z_][a-zA-Z0-9_]*(::[a-zA-Z_][a-zA-Z0-9_]*)*");
    ast_regex_rule(positive_integer, "[0-9]+");
    ast_regex_rule(integer, "[+-]?[0-9]+");
    ast_regex_rule(hex_number, "0x[0-9a-fA-F]+");
    ast_regex_rule(dec_number, "[+-]?[0-9]+(\\.[0-9]+)?");

    ast_alias(number) = choice<dec_number, hex_number>; ast_alias_end;

    ast_regex_rule(comment, "--([^\\n]*)\\n");

    ast_regex_rule(whitespace, "[ \\t\\n\\r]+");
    ast_alias(S) = drop<any<choice<whitespace, comment>>>; ast_alias_end;
    //----Rules----
    class expression;
    class primary;

    ast_alias(argument)  = choice<primary, number>; ast_alias_end;

    //------------------------------------------------------------------------------------------------
    ast_alias(call_params) = opt<replace<seq<token<'<'>, S, list<argument, seq<token<','>, S>>, S, token<'>'>>,ast_str("<$1{, }>")>>; ast_alias_end;
    ast_alias(call) = seq<name, call_params>; ast_alias_end;
    ast_alias(primary) = choice<call, character, string, regex_pattern, seq<token<'('>, S, expression, S, token<')'>>>; ast_alias_end;

    ast_alias(decorator) = seq<token<'.'>, choice<replace_pattern, call>>; ast_alias_end;

    ast_alias(decorated) = seq<primary, any<decorator>>; ast_alias_end;

    ast_regex_rule(rep_flag, "[*?+]");
    ast_alias(factor) = seq<decorated, opt<choice<rep_flag, seq<token<'{'>, S, opt<number>, opt<seq<S, token<','>, S, number>>, S, token<'}'>>>>>; ast_alias_end;

    ast_alias(term) = replace<list<factor, S>,ast_str("seq<${, }>")>; ast_alias_end;

    ast_alias(expression) = replace<list<term, seq<S, token<'|'>, S>>,ast_str("choice<${, }>")>; ast_alias_end;

    ast_regex_rule(valid_type, "([a-zA-Z_][a-zA-Z0-9_]*\\s*(<[^>]*>)?\\s*(\\*|\\&|\\[\\])?)*");
    // ast_alias(param) = choice<seq<valid_type, S, name>, name>; ast_alias_end;
    ast_alias(param_list) = replace<seq<token<'<'>, S, list<valid_type, seq<S, token<','>, S>>, S, token<'>'>>,ast_str("<$1{, }>")>; ast_alias_end;

    ast_string_rule(equal_sign, "::=");
    ast_alias(rule) = seq<name, opt<param_list>, S, equal_sign, S, seq<expression, S, token<';'>>>; ast_alias_end;

    ast_regex_rule(copyline, "$([^\\n]*)\\n");
    
    ast_alias(grammar) = seq<S, list<choice<drop<out<rule>>, drop<out<replace<copyline,ast_str("$1!")>>>>, S>>; ast_alias_end;
    // ast_alias(grammar) = trace_on<seq<S, list<choice<rule, copyline>, S>>>; ast_alias_end;
};