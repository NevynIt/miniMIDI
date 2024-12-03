#include <string>
#include "ast.hpp"
#include "ast_pack.h"

namespace _detail
{
    using namespace ast;
    
    /*
        Grammar definition for format strings:
            digit = '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
            type = 'x' | 'X' | 'c' | 'b' | 'B' | '?' | 'h' | 'H' | 'i' | 'I' | 'l' | 'L' | 'q' | 'Q' | 'n' | 'N' | 'e' | 'f' | 'd' | 's' | 'p' | 'P'
            whitespace = ' ' | '\t' | '\n' | '\r'
            number = <digit> { <digit> }
            bitfield = { <number> [ ":" ] }

            formaT1 = <type> | "<" <bitfield> ">"
            field2 = [ <number> ] <formaT1> [ "[" <number> "]" ]
            strucT1 = { [ <whitespace> ] <field2> } [ <whitespace> ]
            formaT0 = <type> | "{" <strucT1> "}" | "<" <bitfield> ">"
            field1 = [ <number> ] <formaT0> [ "[" <number> "]" ]
            strucT0 = { [ <whitespace> ] <field1> } [ <whitespace> ]
            format = <type> | "{" <strucT0> "}" | "<" <bitfield> ">"
            field = [ <number> ] <format> [ "[" <number> "]" ]
            struct = { [ <whitespace> ] <field> } [ <whitespace> ]
    */

    using digit = token_range<char, '0', '9'>;
    struct whitespace_array
    {
        static constexpr char arr[] = {' ', '\t', '\n', '\r'};
        static constexpr int size = sizeof(arr) / sizeof(arr[0]);
    };
    using whitespace = token_oneOrMore<char, whitespace_array>;
    struct type_array
    {
        static constexpr char arr[] = {'x', 'X', 'c', 'b', 'B', '?', 'h', 'H', 'i', 'I', 'l', 'L', 'q', 'Q', 'n', 'N', 'e', 'f', 'd', 's', 'p', 'P'};
        static constexpr int size = sizeof(arr) / sizeof(arr[0]);
    };
    using type = token_array<char, type_array>;

    class make_number_fn
    {
    public:
        static inline lexeme<char> *fire(lexeme<char> *l)
        {
            //convert the vector of digits l.v to a number
            int number = 0;
            for (auto li : *l->v)
            {
                number = number * 10 + li - '0';
            }
            delete l->v;
            l->type = 'l';
            l->l = number;
            return l;
        }
    };

    using number = token_oneOrMore<digit, make_number_fn>;

    class make_bitfield_fn
    {
    public:
        static inline lexeme<char> *fire(lexeme<char> *l)
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
            l->invalidate_cb = [](lexeme<char> *l) {
                delete (std::vector<uint8_t> *)l->P;
            };
            return l;
        }
    };

    using bitfield = repeat<sequence2<number, 
                                    optional<token<char, ':'>>,
                                    select_fire_fn<char,0>>, 0, -1, make_bitfield_fn>;

    using array_size = sequence3<token<char, '['>, number, token<char, ']'>, select_fire_fn<char, 1>>;

    class make_format_fn
    {
    public:
        static inline lexeme<char> *fire(lexeme<char> *l)
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
            l->invalidate_cb = [](lexeme<char> *l) {
                delete (field_info *)l->P;
            };
            return l;
        };
    };

    class make_field_fn
    {
    public:
        static inline lexeme<char> *fire(lexeme<char> *l)
        {
            if (l->type != 'V' || l->V->size() != 3 || l->V->at(0)->type != 'V' || l->V->at(1)->type != 'P' || l->V->at(2)->type != 'V')
            {
                delete l;
                return nullptr;
            }
            field_info *f = (field_info *)l->V->at(1)->P;
            if (l->V->at(0)->V->size() == 1)
            {
                if (!l->V->at(0)->V->at(0)->type == 'l')
                {
                    delete l;
                    return nullptr;
                }
                f->count = l->V->at(0)->V->at(0)->l;
            }
            if (l->V->at(2)->V->size() == 1)
            {
                if (!l->V->at(2)->V->at(0)->type == 'l')
                {
                    delete l;
                    return nullptr;
                }
                f->array_size = l->V->at(2)->V->at(0)->l;
            }
            l->V->at(1)->P = nullptr;
            l->invalidate();
            l->type = 'P';
            l->P = f;
            l->invalidate_cb = [](lexeme<char> *l) {
                delete (field_info *)l->P;
            };
            return l;
        }
    };

    class make_struct_fn
    {
    public:
        static inline lexeme<char> *fire(lexeme<char> *l)
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
            l->invalidate_cb = [](lexeme<char> *l) {
                delete (std::vector<field_info *> *)l->P;
            };
            return l;
        };
    };

    using format2 = choice2<type,
                            sequence3<token<char, '<'>,
                                    bitfield,
                                    token<char, '>'>,
                                    select_fire_fn<char, 1>>,
                            make_format_fn>;
    using field2 = sequence3<optional<number>, 
                            format2, 
                            optional<array_size>, 
                            make_field_fn>;
    using struct2 = sequence2<oneOrMore<sequence2<optional<whitespace>, 
                                                field2, 
                                                select_fire_fn<char, 1>>, make_struct_fn>, 
                            optional<whitespace>, 
                            select_fire_fn<char, 0>>;

    using format1 = choice3<type,
                            sequence3<token<char, '<'>, bitfield, token<char, '>'>, select_fire_fn<char, 1>>,
                            sequence3<token<char, '{'>, struct2, token<char, '}'>, select_fire_fn<char, 1>>, 
                            make_format_fn>;
    using field1 = sequence3<optional<number>, format1, optional<array_size>, make_field_fn>;
    using struct1 = sequence2<oneOrMore<sequence2<optional<whitespace>, field1, select_fire_fn<char, 1>>, make_struct_fn>, optional<whitespace>, select_fire_fn<char, 0>>;

    using format0 = choice3<type,
                            sequence3<token<char, '<'>, bitfield, token<char, '>'>, select_fire_fn<char, 1>>,
                            sequence3<token<char, '{'>, struct1, token<char, '}'>, select_fire_fn<char, 1>>, 
                            make_format_fn>;
    using field0 = sequence3<optional<number>, format0, optional<array_size>, make_field_fn>;
    using struct0 = sequence2<oneOrMore<sequence2<optional<whitespace>, field0, select_fire_fn<char, 1>>, make_struct_fn>, optional<whitespace>, select_fire_fn<char, 0>>;

    using structure = choice3<no_match<char>, no_match<char>, struct0, make_format_fn>;
} // namespace _detail

field_info *parse_pack_format_string(const char *fmt)
{
    using namespace _detail;
    _detail::stream<const char> s{fmt};
    auto l = _detail::structure::match(s);
    field_info *f = (field_info *)l->P;
    l->P = nullptr;
    delete l;
    return f;
}

void print_field(field_info *f, const char *indent)
{
    printf("%s%d", indent, f->count);
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
        printf("{\n");
        for (auto ff : *f->fields)
        {
            std::string new_indent = std::string(indent) + "  ";
            print_field(ff, new_indent.c_str());
        }
        printf("%s}", indent);
    }
    else
    {
        printf("%c", f->type);
    }
    printf("[%d]\n", f->array_size);
}