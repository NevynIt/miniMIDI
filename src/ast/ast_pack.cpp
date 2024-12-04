#include <string>
#include "ast_char.h"
#include "ast_pack.h"
#include <map>

namespace _detail
{
    using namespace ast_char;
    /*
        Grammar definition for format strings:
            digit = '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
            type = 'x' | 'X' | 'c' | 'b' | 'B' | '?' | 'h' | 'H' | 'i' | 'I' | 'l' | 'L' | 'q' | 'Q' | 'n' | 'N' | 'e' | 'f' | 'd' | 's' | 'p' | 'P'
            whitespace = ' ' | '\t' | '\n' | '\r'
            number = <digit> { <digit> }
            bitfield = { <number> [ ":" ] }
            alpha = 'a' to 'z' | 'A' to 'Z'
            identifier = <alpha> { <alpha> | <digit> | '_' }
            quoted_name = '\'' <identifier> '\''

            formaT1 = <type> | "<" <bitfield> ">"
            field2 = [ <number> | <quoted_name> ] <formaT1> [ "[" <number> | <quoted_name> "]" ]
            strucT1 = { [ <whitespace> ] <field2> } [ <whitespace> ]
            formaT0 = <type> | "{" <strucT1> "}" | "<" <bitfield> ">"
            field1 = [ <number> | <quoted_name> ] <formaT0> [ "[" <number> | <quoted_name> "]" ]
            strucT0 = { [ <whitespace> ] <field1> } [ <whitespace> ]
            format = <type> | "{" <strucT0> "}" | "<" <bitfield> ">"
            field = [ <number> | <quoted_name> ] <format> [ "[" <number> | <quoted_name> "]" ]
            struct = { [ <whitespace> ] <field> } [ <whitespace> ]
    */

    class make_bitfield_fn
    {
    public:
        static inline lexeme *refine(lexeme *l)
        {
            std::vector<uint8_t> *bitfield = new std::vector<uint8_t>();
            if (!l->type == 'V')
            {
                delete l;
                return nullptr;
            }
            for (auto li : *l->V)
            {
                if (li->type != 'l')
                {
                    delete l;
                    return nullptr;
                }
                int number = li->l;
                bitfield->push_back(number);
            }
            delete l->V;
            l->type = 'P';
            l->P = bitfield;
            l->invalidate_cb = [](lexeme *l) {
                delete (std::vector<uint8_t> *)l->P;
            };
            return l;
        }
    };

    class make_format_fn
    {
    public:
        static inline lexeme *refine(lexeme *l)
        {
            //l is a choice between:
            //  0 = type
            //  1 = bitfield
            //  2 = struct

            field_info *f = new field_info();

            if (l->type == '0') //type
            {
                if (l->lex->type != 'o')
                {
                    delete l;
                    delete f;
                    return nullptr;
                }
                f->type = l->lex->o;
            }
            else if (l->type == '1') //bitfield
            {
                if (l->lex->type != 'P')
                {
                    delete l;
                    delete f;
                    return nullptr;
                }
                f->type = 1;
                f->bitfield = (std::vector<uint8_t> *)l->lex->P;
                l->lex->P = nullptr;
            }
            else if (l->type == '2') //struct
            {
                if (l->lex->type != 'P')
                {
                    delete l;
                    delete f;
                    return nullptr;
                }
                f->type = 2;
                f->fields = (std::vector<field_info *> *)l->lex->P;
                l->lex->P = nullptr;
            }
            l->invalidate();
            l->type = 'P';
            l->P = f;
            l->invalidate_cb = [](lexeme *l) {
                delete (field_info *)l->P;
            };
            return l;
        };
    };

    class make_field_fn
    {
    public:
        static inline lexeme *refine(lexeme *l)
        {
            if (l->type != 'V' || l->V->size() != 3 || l->V->at(0)->type != 'V' || l->V->at(1)->type != 'P' || l->V->at(2)->type != 'V')
            {
                delete l;
                return nullptr;
            }
            field_info *f = (field_info *)l->V->at(1)->P;
            if (l->V->at(0)->V->size() == 1)
            {
                if (l->V->at(0)->V->at(0)->type == 'l')
                    f->count = l->V->at(0)->V->at(0)->l;
                else if (l->V->at(0)->V->at(0)->type == 's')
                {
                    f->count_name = l->V->at(0)->V->at(0)->s;
                    l->V->at(0)->V->at(0)->s = nullptr;
                }
                else
                {
                    delete l;
                    return nullptr;
                }
            }
            if (l->V->at(2)->V->size() == 1)
            {
                if (l->V->at(2)->V->at(0)->type == 'l')
                    f->array_size = l->V->at(2)->V->at(0)->l;
                else if (l->V->at(2)->V->at(0)->type == 's')
                {
                    f->array_size_name = l->V->at(2)->V->at(0)->s;
                    l->V->at(2)->V->at(0)->s = nullptr;
                }
                else
                {
                    delete l;
                    return nullptr;
                }
            }
            l->V->at(1)->P = nullptr;
            l->invalidate();
            l->type = 'P';
            l->P = f;
            l->invalidate_cb = [](lexeme *l) {
                delete (field_info *)l->P;
            };
            return l;
        }
    };

    class make_struct_fn
    {
    public:
        static inline lexeme *refine(lexeme *l)
        {
            if (l->type != 'V')
            {
                delete l;
                return nullptr;
            }
            std::vector<field_info *> *fields = new std::vector<field_info *>();
            for (auto li : *l->V)
            {
                if (li->type != 'P')
                {
                    for (auto fi : *fields)
                    {
                        delete fi;
                    }
                    delete fields;
                    delete l;
                    return nullptr;
                }
                fields->push_back((field_info *)li->P);
                li->P = nullptr;
            }
            l->invalidate();
            l->type = 'P';
            l->P = fields;
            l->invalidate_cb = [](lexeme *l) {
                delete (std::vector<field_info *> *)l->P;
            };
            return l;
        };
    };

    //these are already defined in ast_char.hpp
    // dogit = '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    // alpha = 'a' to 'z' | 'A' to 'Z'
    // identifier = <alpha> { <alpha> | <digit> | '_' }
    
    // using quoted_name = dec<seq3<token<'\''>, identifier, token<'\''>>, select_fn<1>>;
    ast_rule(quoted_name, (dec<seq3<token<'\''>, identifier, token<'\''>>, select_fn<1>>));

    using number = str2long;
    // ast_rule(number, (str2long));

    char_array_decl(char, type_chars) = "xXcbB?hHiIlLqQnNefdspP";
    using type = token_choice<char_array(type_chars)>;

    using bitfield_def = dec<rep<dec<seq<number, 
                                    opt<token<':'>>>,
                                    select_fn<0>>, 0, -1>, make_bitfield_fn>;
    ast_rule(bitfield, bitfield_def);

    // using array_size = dec<seq3<token<'['>, choice<number,quoted_name>, token<']'>>, select_fn<1>>;
    ast_rule(array_size, (dec<seq3<token<'['>, choice<number,quoted_name>, token<']'>>, select_fn<1>>));

    using field_count = choice<number,quoted_name>;

    using format2 = dec<choicei<type,
                            dec<seq3<token<'<'>,
                                    bitfield,
                                    token<'>'>>,
                                    select_fn<1>>>,
                            make_format_fn>;

    using field2_def = dec<seq3<opt<field_count>, 
                            format2, 
                            opt<array_size>>, 
                            make_field_fn>;
    ast_rule(field2, field2_def);

    using struct2 = dec<seq<
                            dec<some<dec<seq<opt<whitespace>,field2>, select_fn<1>>>, make_struct_fn>,
                            opt<whitespace>>,
                        select_fn<0>>;

    using format1 = dec<choice3i<type,
                            dec<seq3<token<'<'>, bitfield, token<'>'>>, select_fn<1>>,
                            dec<seq3<token<'{'>, struct2,  token<'}'>>, select_fn<1>>>, 
                            make_format_fn>;
    using field1_def = dec<seq3<opt<field_count>, 
                            format1, 
                            opt<array_size>>, 
                            make_field_fn>;
    ast_rule(field1, field1_def);

    using struct1 = dec<seq<
                            dec<some<dec<seq<opt<whitespace>,field1>, select_fn<1>>>, make_struct_fn>,
                            opt<whitespace>>,
                        select_fn<0>>;

    using format0 = dec<choice3i<type,
                            dec<seq3<token<'<'>, bitfield, token<'>'>>, select_fn<1>>,
                            dec<seq3<token<'{'>, struct1,  token<'}'>>, select_fn<1>>>, 
                            make_format_fn>;
    using field0_def = dec<seq3<opt<field_count>, 
                            format0, 
                            opt<array_size>>, 
                            make_field_fn>;
    ast_rule(field0, field0_def);
    
    using struct0 = dec<seq<
                            dec<some<dec<seq<opt<whitespace>,field0>, select_fn<1>>>, make_struct_fn>,
                            opt<whitespace>>,
                        select_fn<0>>;

    using structure = dec<choice3i<fail_always, fail_always, struct0>, make_format_fn>;
} // namespace _detail

field_info *parse_pack_format_string(const char *fmt)
{
    using namespace _detail;
    _detail::stream s{fmt};
    _detail::lexeme *l = _detail::structure::match(s);
    if (l == nullptr)
    {
        return nullptr;
    }
    field_info *f = (field_info *)l->P;
    l->P = nullptr;
    delete l;
    return f;
}

std::map<char, const char *> typestrings = {
    {'x', "pad byte"},
    {'X', "pad word"},
    {'c', "char"},
    {'b', "signed char"},
    {'B', "unsigned char"},
    {'?', "bool"},
    {'h', "short"},
    {'H', "unsigned short"},
    {'i', "int"},
    {'I', "unsigned int"},
    {'l', "long"},
    {'L', "unsigned long"},
    {'q', "long long"},
    {'Q', "unsigned long long"},
    {'n', "ssize_t"},
    {'N', "size_t"},
    {'e', "float"},
    {'f', "float"},
    {'d', "double"},
    {'s', "string"},
    {'p', "pascal string"},
    {'P', "pointer"}};

void print_field(field_info *f, const char *indent)
{
    if (!f)
        return;
    
    //print the field in the format: type_string name

    if (f->count > 1)
        printf("%s//repeat %d times\n", indent, f->count);
    
    printf("%s", indent, f->count);
    if (f->type == 1)
    {
        printf("<");
        for (auto b : *f->bitfield)
        {
            printf("%d:", b);
        }
        printf(">");
    }
    else if (f->type == 2)
    {
        printf("struct {\n");
        for (auto ff : *f->fields)
        {
            std::string new_indent = std::string(indent) + "  ";
            print_field(ff, new_indent.c_str());
        }
        printf("%s}", indent);
    }
    else
    {
        printf("%s", typestrings[f->type]);
    }

    const char *name = (f->count_name) ? f->count_name : "_";
    printf(" %s", name);

    if (f->array_size_name)
    {
        printf("[%s]", f->array_size_name);
    }
    else if (f->array_size > 1)
        printf("[%d]", f->array_size);
    printf(";\n");
}