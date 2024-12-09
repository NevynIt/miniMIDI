// #pragma once

// #include "ast_char.h"
// #include "regex.h"

// namespace ast_grammar
// {
//     using namespace ast_char;

//     //----Decorators----

//     //rep is already defined
//     template<char start, char escape, char end>
//     // using delimited = token_delimited<start, escape, end>;
//     class delimited : public token_delimited<start, escape, end> {};

//     template<typename exp, typename sep>
//     // using list = rep<exp, 1, -1, sep>;
//     class list : public rep<exp, 1, -1, sep> {};

//     template<typename exp>
//     // using concat = dec<exp, ast::_f::concat>;
//     class concat : public dec<exp, ast::_f::concat> {};

//     template<typename exp, int i>
//     // using select = dec<exp, ast::_f::select<i>>;
//     class select : public dec<exp, ast::_f::select<i>> {};

//     template<typename exp>
//     // using tolong = dec<exp, ast_char::tolong>;
//     class tolong : public dec<exp, ast_char::tolong> {};

//     template<const char *pattern>
//     // using regex = ast_char::token_regex<pattern>;
//     class regex : public ast_char::token_regex<pattern> {};

//     //----Primary operators----
//     // using character = delimited<'\'', '\\', '\''>;
//     class character : public delimited<'\'', '\\', '\''> {};

//     // using string = delimited<'"', '\\', '"'>;
//     class string : public delimited<'"', '\\', '"'> {};
//     // using re_pattern = delimited<'/', '\\', '/'>;
//     class re_pattern : public delimited<'/', '\\', '/'> {};

//     re_match(name, "[a-zA-Z_][a-zA-Z0-9_]*(::[a-zA-Z_][a-zA-Z0-9_]*)*");
//     re_match(positive_integer, "[0-9]+");
//     re_match(integer, "[+-]?[0-9]+");
//     re_match(hex_number, "0x[0-9a-fA-F]+");
//     re_match(dec_number, "[+-]?[0-9]+(\\.[0-9]+)?");

//     // using number = tolong<choice<dec_number, hex_number>>;
//     class number : public tolong<choice<dec_number, hex_number>> {};

//     re_decl(comment_re01, "--([^\\n]*)\\n");
//     // using comment = select<comment_re01, 1>;
//     class comment : public select<comment_re01, 1> {};

//     re_decl(S_re01, "[ \\t\\n\\r]+");
//     // using S = concat<any<choice<S_re01, comment>>>;
//     class S : public concat<any<choice<S_re01, comment>>> {};

//     //----Rules----
//     alias_declare(expression);
//     alias_declare(primary);

//     // using argument = choice2<primary, number>;
//     class argument : public choice2<primary, number> {};
//     //------------------------------------------------------------------------------------------------
//     class call : public seq2<name, opt<seq5<token<'('>, S, list<argument, seq<token<','>, S>>, S, token<')'>>>> {};
//     // using primary = choice4<name, string, re_pattern, seq5<token<'('>, S, expression, S, token<')'>>>;
//     class primary_impl : public choice4<call, string, re_pattern, seq5<token<'('>, S, expression, S, token<')'>>> {};
//     alias_define(primary, primary_impl);

//     // using decorator = seq3<token<'.'>, name, opt<seq5<token<'('>, S, list<argument, seq<token<','>, S>>, S, token<')'>>>>;
//     class decorator : public seq2<token<'.'>, call> {};

//     // using decorated = seq2<primary, any<decorator>>;
//     class decorated : public seq2<primary, any<decorator>> {};
//     re_decl(factor_re01, "[*?+]");
//     // using factor = seq2<decorated, opt<choice<factor_re01, seq6<token<'{'>, S, opt<number>, opt<seq4<S, token<','>, S, number>>, S, token<'}'>>>>>;
//     class factor : public seq2<decorated, opt<choice<factor_re01, seq6<token<'{'>, S, opt<number>, opt<seq4<S, token<','>, S, number>>, S, token<'}'>>>>> {};
//     // using term = list<factor, S>;
//     class term : public list<factor, S> {};
//     // using expression_impl = list<term, seq3<S, token<'|'>, S>>;
//     class expression_impl : public list<term, seq3<S, token<'|'>, S>> {};
//     alias_define(expression, expression_impl);

//     re_match(valid_type, "([a-zA-Z_][a-zA-Z0-9_]*\\s*(<[^>]*>)?\\s*(\\*|\\&|\\[\\])?)*");
//     // using param = seq3<opt<valid_type>, S, name>; ---------------------------------------------------------
//     class param : public choice<seq3<valid_type, S, name>, name> {};
//     // using param_list = seq5<token<'('>, S, list<param, seq3<S, token<','>, S>>, S, token<')'>>;
//     class param_list : public seq5<token<'('>, S, list<param, seq3<S, token<','>, S>>, S, token<')'>> {};

//     str_decl(rule_str01, "::=");
//     // using rule = seq6<name, opt<param_list>, S, rule_str01, S, seq3<expression, S, token<';'>>>;
//     class rule : public seq6<name, opt<param_list>, S, rule_str01, S, seq3<expression, S, token<';'>>> {};

//     re_decl(copyline_re01, "$([^\\n]*)\\n");
//     // using copyline = select<copyline_re01, 1>;
//     class copyline : public select<copyline_re01, 1> {};
//     // using grammar = seq<S, list<choice2<rule, copyline>, S>>;
//     class grammar : public seq<S, list<choice2<rule, copyline>, S>> {};

// };