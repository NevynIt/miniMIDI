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
            type = 'x' | 'X' | 'c' | 'b' | 'B' | '?' | 'h' | 'H' | 'i' | 'I' | 'l' | 'L' | 'q' | 'Q' | 'n' | 'N' | 'f' | 'd' | 's' | 'p' | 'P'
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

    class make_bitfield_fn : public base_fn
    {
    public:
        static inline lexeme *post_match(lexeme *l)
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

    class make_format_fn : public base_fn
    {
    public:
        static inline lexeme *post_match(lexeme *l)
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

    class make_field_fn : public base_fn
    {
    public:
        static inline lexeme *post_match(lexeme *l)
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
                    f->field_name = l->V->at(0)->V->at(0)->s;
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

    class make_struct_fn : public base_fn
    {
    public:
        static inline lexeme *post_match(lexeme *l)
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

    char_array_decl(char, type_chars) = "xXcbB?hHiIlLqQnNfdspP";
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

field_info *field_info::parse(char *fmt)
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
    {'x', "pad byte"}, //insert or skip exactly one byte of padding
    {'X', "pad int32"}, //insert or skip unitl the next int32 boundary
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
    {'f', "float"},
    {'d', "double"},
    {'s', "string"},
    {'p', "pascal string"},
    {'P', "pointer"}};

void field_info::print(const char *indent)
{
    //print the field in the format: type_string name

    if (count > 1)
        printf("%s//repeat %d times\n", indent, count);
    
    printf("%s", indent, count);
    if (type == 1)
    {
        printf("<");
        for (auto b : *bitfield)
        {
            printf("%d:", b);
        }
        printf(">");
    }
    else if (type == 2)
    {
        printf("struct {\n");
        for (auto ff : *fields)
        {
            std::string new_indent = std::string(indent) + "  ";
            ff->print(new_indent.c_str());
        }
        printf("%s}", indent);
    }
    else
    {
        printf("%s", typestrings[type]);
    }

    const char *name = (field_name) ? field_name : "_";
    printf(" %s", name);

    if (array_size_name)
    {
        printf("[%s]", array_size_name);
    }
    else if (array_size > 1)
        printf("[%d]", array_size);
    printf(";\n");
}

void append_value(lua_State *L)
{
    //push the value onto the stack on the table at -2, at the last index
    lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
}

int unpack_simple_value(lua_State *L, field_info *f, ast_char::stream &s, int &size)
{
    bool append = true;
    //simple type
    switch (f->type)
    {
    case 'x':
        s++;
        size++;
        append = false;
        break;
    case 'X':
        {
            int padbytes = 4 - (size % 4);
            if (padbytes == 4)
                padbytes = 0;
            s += padbytes;
            size += padbytes;
            append = false;
        }
        break;
    case 'c':
        lua_pushinteger(L, *s++);
        size++;
        break;
    case 'b':
        lua_pushinteger(L, (int8_t)*s++);
        size++;
        break;
    case 'B':
        lua_pushinteger(L, (uint8_t)*s++);
        size++;
        break;
    case '?':
        lua_pushboolean(L, *s++);
        size++;
        break;
    case 'h':
        {
            int16_t value = *(int16_t *)s.cur;
            lua_pushinteger(L, value);
            s += 2;
            size += 2;
        }
        break;
    case 'H':
        {
            uint16_t value = *(uint16_t *)s.cur;
            lua_pushinteger(L, value);
            s += 2;
            size += 2;
        }
        break;
    case 'i':
    case 'l':
    case 'n':
        {
            int32_t value = *(int32_t *)s.cur;
            lua_pushinteger(L, value);
            s += 4;
            size += 4;
        }
        break;
    case 'I':
    case 'L':
    case 'N':
        {
            uint32_t value = *(uint32_t *)s.cur;
            lua_pushinteger(L, value);
            s += 4;
            size += 4;
        }
        break;
    case 'q':
        {
            int64_t value = *(int64_t *)s.cur;
            lua_pushinteger(L, value);
            s += 8;
            size += 8;
        }
        break;
    case 'Q':
        {
            uint64_t value = *(uint64_t *)s.cur;
            lua_pushinteger(L, value);
            s += 8;
            size += 8;
        }
        break;
    case 'f':
    {
        float value = *(float *)s.cur;
        lua_pushnumber(L, value);
        s += 4;
        size += 4;
    }
        break;
    case 'd':
    {
        double value = *(double *)s.cur;
        lua_pushnumber(L, value);
        s += 8;
        size += 8;
    }
        break;
    case 's':
        {
            std::string value;
            while (*s)
            {
                value.push_back(*s++);
            }
            lua_pushlstring(L, value.data(), value.size());
            size += value.size() + 1;
        }
        break;
    case 'p':
        {
            uint8_t sz = *s++;
            std::string value;
            for (int i = 0; i < sz; i++)
            {
                value.push_back(*s++);
            }
            lua_pushlstring(L, value.data(), value.size());
            size += value.size() + 1;
        }
        break;
    case 'P':
    {
        void *value = *(void **)s.cur;
        lua_pushlightuserdata(L, value);
        s += sizeof(void *);
        size += sizeof(void *);
    }
    break;
    default:
        return luaL_error(L, "Invalid type");
    }
    if (append)
        append_value(L);
    return 1;
}

int unpack_value(lua_State *L, field_info *f, ast_char::stream &s)
{   //unpack the field into the table at the top of the stack and leave it there
    //if the table does not have a __size, add it and set it to 0
    int size = 0;
    lua_getfield(L, -1, "__size");
    if (lua_isnil(L, -1))
    {
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, "__size");
    }
    else
    {
        size = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    //handle repetition first
    int count = f->count;
    for (int i=0; i<count; i++)
    {
        if (f->type == 1)
        {
            //bitfield
            //take the total size in bits and get the closer between 1, 2, 4 and 8 bytes
            int total_size = 0;
            for (auto b : *f->bitfield)
            {
                total_size += b;
            }
            int bytes = (total_size + 7) / 8;
            if (bytes == 3)
                bytes = 4;
            else if (bytes == 5 || bytes == 6 || bytes == 7)
                bytes = 8;
            else if (bytes > 8)
                return luaL_error(L, "Invalid bitfield size");
            //update the size of the struct
            size += bytes;
           
            //read the bytes
            uint64_t value = 0;
            for (int i = 0; i < bytes; i++)
            {
                value |= (uint64_t)((uint8_t)*s++) << (i * 8);
            }
            //push the values onto the stack
            int shift = 0;
            for (auto b : *f->bitfield)
            {
                lua_pushinteger(L, (value >> shift) & ((1 << b) - 1));
                append_value(L);
                shift += b;
            }
        }
        else if (f->type == 2)
        {
            //TODO: handle arrays of structs if array_size > 1 or array_size_name != nullptr
            //struct
            lua_newtable(L);
            for (auto ff : *f->fields)
            {
                if (unpack_value(L, ff, s) != 1)
                {
                    return luaL_error(L, "Error unpacking struct");
                }
            }
            lua_getfield(L, -1, "__size");
            size += lua_tointeger(L, -1);
            lua_pop(L, 1);
            append_value(L);
        }
        else
        {
            int array_size = f->array_size;
            if (f->array_size_name != nullptr)
            {
                lua_getfield(L, -1, f->array_size_name);
                if (lua_isnil(L, -1))
                    return luaL_error(L, "Array size field not found");
                array_size = lua_tointeger(L, -1);
                lua_pop(L, 1);
            }

            if (array_size == 1)
            {
                if (unpack_simple_value(L, f, s, size) != 1)
                    return luaL_error(L, "Error unpacking simple value");
            }
            else
            {
                lua_newtable(L);
                for (int i = 0; i < array_size; i++)
                {
                    if (unpack_simple_value(L, f, s, size) != 1)
                        return luaL_error(L, "Error unpacking simple value");
                }
                append_value(L);
            }
        }
    }
    if (f->field_name)
    {
        //pop the last value and set it also as a field in the table
        lua_rawgeti(L, -1, lua_rawlen(L, -1));
        lua_setfield(L, -2, f->field_name);
    }
    
    //update the size of the struct
    lua_pushinteger(L, size);
    lua_setfield(L, -2, "__size");
    return 1;
}

int field_info::unpack(lua_State *L)
{
    //take a light userdata as second parameter (the first is the table)
    //unpack the data into a new table
    ast_char::stream s{(char *)lua_touserdata(L, 2)};
    if (!s.valid())
        return luaL_error(L, "Invalid pointer");
    lua_newtable(L);
    return unpack_value(L, this, s);
}

int pack_simple_value(lua_State *L, field_info *f, ast_char::stream &s, int &size)
{
    // Serialize a simple value from the Lua stack into the stream
    switch (f->type)
    {
    case 'x':
        *s++ = 0;  // Pad byte
        size++;
        break;
    case 'X':
        {
            int padbytes = 4 - (size % 4);
            if (padbytes == 4)
                padbytes = 0;
            memset(s.cur, 0, padbytes);
            s += padbytes;
            size += padbytes;
        }
        break;
    case 'c':
    case 'b':
        {
            int8_t value = (int8_t)luaL_checkinteger(L, -1);
            *s++ = value;
            size++;
        }
        break;
    case 'B':
        {
            uint8_t value = (uint8_t)luaL_checkinteger(L, -1);
            *s++ = value;
            size++;
        }
        break;
    case '?':
        {
            int value = lua_toboolean(L, -1);
            *s++ = value ? 1 : 0;
            size++;
        }
        break;
    case 'h':
        {
            int16_t value = (int16_t)luaL_checkinteger(L, -1);
            *(int16_t *)s.cur = value;
            s += 2;
            size += 2;
        }
        break;
    case 'H':
        {
            uint16_t value = (uint16_t)luaL_checkinteger(L, -1);
            *(uint16_t *)s.cur = value;
            s += 2;
            size += 2;
        }
        break;
    case 'i':
    case 'l':
    case 'n':
        {
            int32_t value = (int32_t)luaL_checkinteger(L, -1);
            *(int32_t *)s.cur = value;
            s += 4;
            size += 4;
        }
        break;
    case 'I':
    case 'L':
    case 'N':
        {
            uint32_t value = (uint32_t)luaL_checkinteger(L, -1);
            *(uint32_t *)s.cur = value;
            s += 4;
            size += 4;
        }
        break;
    case 'q':
        {
            int64_t value = (int64_t)luaL_checkinteger(L, -1);
            *(int64_t *)s.cur = value;
            s += 8;
            size += 8;
        }
        break;
    case 'Q':
        {
            uint64_t value = (uint64_t)luaL_checkinteger(L, -1);
            *(uint64_t *)s.cur = value;
            s += 8;
            size += 8;
        }
        break;
    case 'f':
        {
            float value = (float)luaL_checknumber(L, -1);
            *(float *)s.cur = value;
            s += 4;
            size += 4;
        }
        break;
    case 'd':
        {
            double value = (double)luaL_checknumber(L, -1);
            *(double *)s.cur = value;
            s += 8;
            size += 8;
        }
        break;
    case 's':
        {
            size_t len;
            const char *str = luaL_checklstring(L, -1, &len);
            memcpy(s.cur, str, len);
            s.cur[len] = 0; // Null-terminate
            s += len + 1;
            size += len + 1;
        }
        break;
    case 'p':
        {
            size_t len;
            const char *str = luaL_checklstring(L, -1, &len);
            if (len > 255)
                return luaL_error(L, "String too long for 'p' format");
            *s++ = (uint8_t)len;
            memcpy(s.cur, str, len);
            s += len;
            size += len + 1;
        }
        break;
    case 'P':
        {
            void *value = lua_touserdata(L, -1);
            *(void **)s.cur = value;
            s += sizeof(void *);
            size += sizeof(void *);
        }
        break;
    default:
        return luaL_error(L, "Invalid type '%c'", f->type);
    }
    lua_pop(L, 1);
    return 1;
}

int pack_value(lua_State *L, field_info *f, ast_char::stream &s)
{
    int size = 0;
    int count = f->count > 0 ? f->count : 1;
    for (int i = 0; i < count; i++)
    {
        if (f->type == 1)
        {
            //TODO: implement bitfield serialization
            // Bitfield
        }
        else if (f->type == 2)
        {
            //TODO: handle arrays of structs if array_size > 1 or array_size_name != nullptr
            // Struct
            lua_getfield(L, -1, f->field_name ? f->field_name : std::to_string(i + 1).c_str()); //TODO: lua_rawgeti in all cases
            if (!lua_istable(L, -1))
                return luaL_error(L, "Expected table for struct field '%s'", f->field_name); //TODO: print the index as well as the name, which could be null
            //TODO: if field_name is set, verify that the table contains the same as the content for that key and at index i+1

            for (auto ff : *f->fields)
            {
                if (pack_value(L, ff, s) != 1)
                    return luaL_error(L, "Error packing struct field '%s'", ff->field_name ? ff->field_name : "");//TODO: print the index as well as the name, which could be null
            }
            lua_pop(L, 1);
        }
        else
        {
            int array_size = f->array_size > 0 ? f->array_size : 1;
            if (f->array_size_name != nullptr)
            {
                lua_getfield(L, -1, f->array_size_name);
                if (lua_isnil(L, -1))
                    return luaL_error(L, "Array size field '%s' not found", f->array_size_name);
                array_size = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
            }

            lua_getfield(L, -1, f->field_name ? f->field_name : std::to_string(i + 1).c_str()); //TODO: lua_rawgeti in all cases
            //TODO: if field_name is set, verify that the table contains the same as the content for that key and at index i+1

            if (array_size == 1)
            {
                if (lua_isnil(L, -1)) //TODO: not needed, as we are using lua_rawgeti
                    return luaL_error(L, "Field '%s' not found", f->field_name); //TODO: print the index as well as the name, which could be null
                if (pack_simple_value(L, f, s, size) != 1)
                    return luaL_error(L, "Error packing value '%s'", f->field_name); //TODO: print the index as well as the name, which could be null
            }
            else
            {
                if (!lua_istable(L, -1))
                    return luaL_error(L, "Expected table for array field '%s'", f->field_name); //TODO: print the index as well as the name, which could be null
                for (int j = 1; j <= array_size; j++)
                {
                    lua_rawgeti(L, -1, j);
                    if (pack_simple_value(L, f, s, size) != 1)
                        return luaL_error(L, "Error packing array element %d of '%s'", j, f->field_name); //TODO: print the index as well as the name, which could be null
                }
                lua_pop(L, 1); // Pop array table
            }
            lua_pop(L, 1); // Pop field value
        }
    }
    return 1;
}

int field_info::pack(lua_State *L)
{
    // Serialize data from the Lua table onto the stream
    ast_char::stream s{reinterpret_cast<char *>(lua_touserdata(L, 2))};
    auto tmp = s.cur;
    if (!s.valid())
        return luaL_error(L, "Invalid pointer");
    if (pack_value(L, this, s) != 1)
        return luaL_error(L, "Error packing data");
    // Return the total size packed
    lua_pushinteger(L, tmp - s.cur);
    return 1;
}