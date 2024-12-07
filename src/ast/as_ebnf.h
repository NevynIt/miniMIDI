#pragma once

#include "ast_char.h"
#include "regex.h"

namespace ast_grammar
{
    using namespace ast_char;
    /*  
    --Parser and compiler to transform a grammar file into a ast using the ast library
    --The grammar of the grammar is as follows
    -- x ==> y represent syntactic sugar for transformations

    $#include "ast_char.h"
    $namespace ast_grammar
    ${
    $  using namespace ast_char;
    -- primary operators
    character ::= delimited('\'', '\\', '\'');
        --parse and convert to string in the ast
        --the compiler will use ast_char::token to match

    string ::= delimited('"', '\\', '"');
        --parse and convert to string in the ast
        --the compiler will use ast_char::token_string to match the string

    regex ::= delimited('/', '\\', '/');
        --parse and convert to string in the ast
        --the compiler will use ast_char::token_regex to match the string and select group 0

    name ::= "[a-zA-Z_][a-zA-Z0-9_]+(::[a-zA-Z_][a-zA-Z0-9_]+)*";
        --the compiler will use it as a class name

    positive_integer ::= "[0-9]+";
    integer ::= "[+-]?[0-9]+";
    hex_number ::= "0x[0-9a-fA-F]+";
    dec_number ::= "[+-]?[0-9]+(\.[0-9]+)?";
    to_eol ::= "[^\n]"*;
    number ::= (dec_number | hex_number).tolong;
    comment ::= regex("--([^n]*)\n").select(1);
    S ::= ((' ' | '\t' | '\n' | '\r' | comment)*).concat;
        -- whitespace and comments
        -- the compiler will ignore them

    -- decorators
    rep(exp, min, max, sep) ::= ast_char::rep(exp, min, max, sep);
    delimited(start,escape,end) ::= ast::_t::token_delimited(start, escape, end>
    list(exp, sep) ::= rep(exp,1,-1,sep);
    concat(exp) ::= ast::_f::concat(exp);
        --concatenate the string lexemes into a single string

    select(exp, i) ::= ast::_f::select(exp, i);
        --select the lexeme i from the list
    
    tolong(exp) ::= ast_char::tolong(exp);
        --convert the string lexeme into a long number

    regex(exp) ::= ast::_t::token_regex(exp);
        --parse the string lexeme as a regex
        --return the whole set of groups and matches

    -- rules
    primary ::= name | string | regex | '(' S expression S ')';
    argument ::= primary | number;
        --the compiler will use these as template arguments

    decorator ::= '.' name ( '(' S argument.list(S ',' S) ')' S )?;
    decorated ::= primary decorator*;
        -- xxx.yyy(a,b,c) ==> yyy(xxx,a,b,c)
        -- x.y.z          ==> z(y(x))
        -- x(y) compiles as template instantiation x<y>

    factor ::= decorated ('*' | '+' | '?' | '{' S number? (S ',' S (number)?)? S '}')?;
        -- xxx+     ==> xxx.rep(1,-1)
        -- xxx*     ==> xxx.rep(0,-1)
        -- xxx?     ==> xxx.rep(0,1)
        -- xxx{n}   ==> xxx.rep(n,n)
        -- xxx{n,}  ==> xxx.rep(n,-1)
        -- xxx{n,m} ==> xxx.rep(n,m)
        -- xxx{,m}  ==> xxx.rep(0,m)
    
    term ::= factor.list(S);
        -- the compiler will use ast::_s::seq to concatenate the factors
        -- x y       compiles as    seq<x,y>
        -- x y z     compiles as    seq<x,seq<y,z>>

    expression ::= term.list(S '|' S);
        -- the compiler will use ast::_c::choice to select the _first_successful_ term
        -- x | y     compiles as    choice<x,y>
        -- x | y | z compiles as    choice<x,choice<y,z>>

    params ::= '(' S name.list(S ',' S) S ')';
        -- the compiler will use these as template parameters

    rule ::= name ( params )? S '::=' S expression S ';';
        -- the compiler will use the name as the class name
        -- x = y      compiles as    using x = y;
        -- x(y) = z   compiles as    using x<y> = z;

    copyline ::= regex('$([^\n]*)\n').select(1);
    grammar ::= S (rule | copyline ).list(S);

    -- the grammar is self-contained, so it can be parsed by itself
    $}
    */
    
    //----Decorators----

    //rep is already defined
    template<typename exp, typename sep>
    using list = rep<exp, 1, -1, sep>;

    template<typename exp>
    using concat = dec<exp, ast::_f::concat>;

    template<typename exp, int i>
    using select = dec<exp, ast::_f::select<i>>;

    template<typename exp>
    using tolong = dec<exp, ast_char::tolong>;

    template<typename start, typename escape, typename end>
    using delimited = ast::_t::token_delimited<start, escape, end>;

    //----Primary operators----
    using character = delimited<token<'\''>, token<'\\'>, token<'\''>>;
    using string = delimited<token<'"'>, token<'\\'>, token<'"'>>;
    using regex = delimited<token<'/'>, token<'\\'>, token<'/'>>;

    re_match(name, "[a-zA-Z_][a-zA-Z0-9_]+(::[a-zA-Z_][a-zA-Z0-9_]+)*");
    re_match(positive_integer, "[0-9]+");
    re_match(integer, "[+-]?[0-9]+");
    re_match(hex_number, "0x[0-9a-fA-F]+");
    re_match(dec_number, "[+-]?[0-9]+(\\.[0-9]+)?");
    re_match(to_eol, "[^\n]*");
    using number = tolong<choice<dec_number, hex_number>>;
    re_decl(comment_re, "--([^n]*)\n");
    using comment = select<comment_re, 1>;
    re_decl(S_re, "[ \t\n\r]*");
    using S = concat<any<choice<S_re, comment>>>;

    //----Rules----
    alias_declare(expression);
    using primary = choice4<name, string, regex, seq5<token<'('>, S, expression, S, token<')'>>>;
    using argument = choice2<primary, number>;
    using decorator = seq3<token<'.'>, name, opt<seq5<token<'('>, S, list<argument, seq<token<','>, S>>, S, token<')'>>>>;

    using decorated = seq2<primary, any<decorator>>;
    using factor = seq2<decorated, opt<choice4<token<'*'>, token<'+'>, token<'?'>, seq6<token<'{'>, S, opt<number>, opt<seq4<S, token<','>, S, number>>, S, token<'}'>>>>>;
    using term = list<factor, S>;
    using expression_impl = list<term, seq3<S, token<'|'>, S>>;
    alias_define(expression, expression_impl);

    using params = seq5<token<'('>, S, list<name, seq3<S, token<','>, S>>, S, token<')'>>;

    char_array_decl(eq_str) = "::=";
    using eq = token_string<char_array(eq_str)>;
    using rule = seq6<name, opt<params>, S, eq, S, seq3<expression, S, token<';'>>>;
    re_decl(copyline_re, "$([^n]*)\n");
    using copyline = select<copyline_re, 1>;
    using grammar = seq<S, list<choice2<rule, copyline>, S>>;

};