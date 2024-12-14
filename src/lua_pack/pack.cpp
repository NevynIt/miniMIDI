#include <string>
#include "../ast/ast_char.h"
#include "pack.h"
#include <map>

namespace _detail
{
    using namespace ast_char;
    using namespace ast::_b;

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

    class lex_bitfield : public lexeme
    {
    public:
        set_signature<ast_str("lex_bitfield")>;
        variant_inherit(lexeme)
        lex_bitfield(std::vector<uint8_t> *bitfield) : bitfield(bitfield) {}
        ~lex_bitfield() { if (bitfield) delete bitfield; }
        std::vector<uint8_t> *bitfield = nullptr;
    };

    template <typename T0>
    ast_define_rule(make_bitfield)
    {
    public:
        set_signature<ast_str("make_bitfield")>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            std::vector<uint8_t> *bitfield = new std::vector<uint8_t>();
            lex_V *V = l->template as<lex_V>();
            if (!V)
            {
                delete l;
                return nullptr;
            }
            for (auto li : *V)
            {
                lex_l *lil = li->template as<lex_l>();
                if (!lil)
                {
                    delete l;
                    return nullptr;
                }
                int number = lil->l;
                bitfield->push_back(number);
            }
            delete l;
            return new lex_bitfield(bitfield);
        }
    };

    class lex_field_info : public lexeme
    {
    public:
        set_signature<ast_str("lex_field_info")>;
        variant_inherit(lexeme)
        lex_field_info(field_info *f) : f(f) {}
        ~lex_field_info() { if (f) delete f; }
        field_info *f = nullptr;
    };

    class lex_struct : public lexeme
    {
    public:
        set_signature<ast_str("lex_struct")>;
        variant_inherit(lexeme)
        lex_struct(std::vector<field_info *> *fields = nullptr) : fields(fields) {}
        ~lex_struct()
        {
            if (fields)
            {
                for (auto f : *fields)
                {
                    delete f;
                }
                delete fields;
            }
        }
        std::vector<field_info *> *fields = nullptr;
    };

    template <typename T0>
    ast_define_rule(make_struct)
    {
    public:
        set_signature<ast_str("make_struct")>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            auto V = l->template as<lex_V>();
            if (!V)
            {
                delete l;
                return nullptr;
            }
            std::vector<field_info *> *fields = new std::vector<field_info *>();
            for (auto li : *V)
            {
                auto fi = li->template as<lex_field_info>();
                if (!fi)
                {
                    for (auto fi : *fields)
                    {
                        delete fi;
                    }
                    delete fields;
                    delete l;
                    return nullptr;
                }
                fields->push_back(fi->f);
                fi->f = nullptr;
            }
            delete l;
            return new lex_struct(fields);
        };
    };
    
    template <typename T0>
    ast_define_rule(make_format)
    {
    public:
        set_signature<ast_str("make_format")>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            //l is a choice between:
            //  0 = type
            //  1 = bitfield
            //  2 = struct

            lex_choice *lc = l->template as<lex_choice>();
            if (!lc)
            {
                delete l;
                return nullptr;
            }

            field_info *f = new field_info();
            if (lc->choice == 0) //type
            {
                lex_re *lo = lc->l->template as<lex_re>();
                if (!lo)
                {
                    delete l;
                    delete f;
                    return nullptr;
                }
                f->type = lo->at(0);
            }
            else if (lc->choice == 1) //bitfield
            {
                lex_bitfield *lb = lc->l->template as<lex_bitfield>();
                if (!lb)
                {
                    delete l;
                    delete f;
                    return nullptr;
                }
                f->type = 1;
                f->bitfield = lb->bitfield;
                lb->bitfield = nullptr;
            }
            else if (lc->choice == 2) //struct
            {
                lex_struct *ls = lc->l->template as<lex_struct>();
                if (!ls)
                {
                    delete l;
                    delete f;
                    return nullptr;
                }
                f->type = 2;
                f->fields = ls->fields;
                ls->fields = nullptr;
            }
            else
            {
                delete l;
                delete f;
                return nullptr;
            }
            delete l;
            return new lex_field_info(f);
        };
    };

    template <typename T0>
    ast_define_rule(make_field)
    {
    public:
        set_signature<ast_str("make_field")>;
        ast_base_rule = T0;

        ast_decorator_implementation(l)
        {
            auto V = l->template as<lex_V>();
            if (!V || V->size() != 3)
            {
                delete l;
                return nullptr;
            }
            lex_V *V0 = V->at(0)->template as<lex_V>();
            lex_field_info *V1 = V->at(1)->template as<lex_field_info>();
            lex_V *V2 = V->at(2)->template as<lex_V>();

            field_info *f = V1->f;
            V1->f = nullptr;

            if (V0->size() == 1)
            {
                auto V0l = V0->at(0)->template as<lex_l>();
                if (V0l)
                {
                    f->count = V0l->l;
                }
                else
                {
                    auto *V0s = V0->at(0)->template as<lex_re>();
                    if (V0s)
                    {
                        char *s = new char[V0s->size() + 1];
                        for (size_t i = 0; i < V0s->size(); i++)
                        {
                            s[i] = V0s->at(i);
                        }
                        s[V0s->size()] = 0;
                        f->field_name = s;
                    }
                    else
                    {
                        delete l;
                        return nullptr;
                    }
                }
            }
            if (V2->size() == 1)
            {
                lex_l *V2l = V2->at(0)->template as<lex_l>();
                if (V2l)
                {
                    f->array_size = V2l->l;
                }
                else
                {
                    auto *V0s = V0->at(0)->template as<lex_re>();
                    if (V0s)
                    {
                        char *s = new char[V0s->size() + 1];
                        for (size_t i = 0; i < V0s->size(); i++)
                        {
                            s[i] = V0s->at(i);
                        }
                        s[V0s->size()] = 0;
                        f->array_size_name = s;
                    }
                    else
                    {
                        delete l;
                        return nullptr;
                    }
                }
            }
            delete l;
            return new lex_field_info(f);
        }
    };

    //these are already defined in ast_char.hpp
    // dogit = '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    // alpha = 'a' to 'z' | 'A' to 'Z'
    // identifier = <alpha> { <alpha> | <digit> | '_' }
    
    ast_alias(quoted_name) = select<seq<token<'\''>, identifier, token<'\''>>, 1>; ast_alias_end;
    ast_alias(number) = str2long; ast_alias_end;
    ast_regex_rule(type, "[xXcbB?hHiIlLqQnNfdspP]");
    ast_alias(bitfield) = make_bitfield<rep<select<seq<number,opt<token<':'>>>,0>, 0, -1>>; ast_alias_end;
    ast_alias(array_size) = select<seq<token<'['>, choice<number,quoted_name>, token<']'>>, 1>; ast_alias_end;
    ast_alias(field_count) = choice<number,quoted_name>; ast_alias_end;


    class structure;

    ast_alias(format) = make_format<choicei<type,
                            select<seq<token<'<'>, bitfield, token<'>'>>, 1>,
                            select<seq<token<'{'>, structure,  token<'}'>>, 1>>>;
    ast_alias_end;

    ast_alias(field) = make_field<seq<opt<field_count>, 
                            format, 
                            opt<array_size>>>;
    ast_alias_end;
    
    ast_alias(structure) = select<seq<
                            make_struct<some<select<seq<opt<whitespace>,field>, 1>>>,
                            opt<whitespace>>, 0>;
    ast_alias_end;

    ast_alias(helper) = make_format<choicei<fail_always, fail_always, structure>>; ast_alias_end;
    // ast_alias(helper) = trace_on<make_format<choicei<fail_always, fail_always, structure>>>; ast_alias_end;

} // namespace _detail

field_info *field_info::parse(const char *fmt)
{
    using namespace _detail;
    auto *l = _detail::helper::match(fmt);
    if (l == nullptr)
    {
        return nullptr;
    }
    auto lf = l->as<lex_field_info>();
    field_info *f = lf->f;
    lf->f = nullptr;
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
{
    // Unpack the field into the table at the top of the stack and leave it there
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

    // Handle repetition
    int count = f->count > 0 ? f->count : 1;
    for (int i = 0; i < count; i++)
    {
        if (f->type == 1) // Bitfield deserialization
        {
            // Calculate total bits and bytes
            int total_bits = 0;
            for (auto b : *f->bitfield)
                total_bits += b;
            int bytes = (total_bits + 7) / 8;

            // Read bytes from the stream
            uint64_t value = 0;
            for (int j = 0; j < bytes; j++)
            {
                value |= (uint64_t)((uint8_t)*s++) << (j * 8);
                size++;
            }

            // Extract bitfield values
            lua_newtable(L); // Table to hold bitfield values
            int shift = 0;
            for (size_t k = 0; k < f->bitfield->size(); k++)
            {
                int bits = (*f->bitfield)[k];
                uint64_t mask = ((uint64_t)1 << bits) - 1;
                uint64_t field_value = (value >> shift) & mask;
                lua_pushinteger(L, field_value);
                lua_rawseti(L, -2, k + 1);
                shift += bits;
            }
            append_value(L);
        }
        else if (f->type == 2) // Struct
        {
            // Handle arrays of structs
            int struct_array_size = f->array_size > 0 ? f->array_size : 1;
            if (f->array_size_name != nullptr)
            {
                lua_getfield(L, -1, f->array_size_name);
                if (lua_isnil(L, -1))
                    return luaL_error(L, "Array size field '%s' not found", f->array_size_name);
                struct_array_size = lua_tointeger(L, -1);
                lua_pop(L, 1);
            }

            if (struct_array_size == 1)
            {
                // Unpack a single struct
                lua_newtable(L);
                for (auto ff : *f->fields)
                {
                    if (unpack_value(L, ff, s) != 1)
                        return luaL_error(L, "Error unpacking struct field");
                }
                lua_getfield(L, -1, "__size");
                size += lua_tointeger(L, -1);
                lua_pop(L, 1);
                append_value(L);
            }
            else
            {
                // Unpack an array of structs
                lua_newtable(L);
                for (int j = 0; j < struct_array_size; j++)
                {
                    lua_newtable(L);
                    for (auto ff : *f->fields)
                    {
                        if (unpack_value(L, ff, s) != 1)
                            return luaL_error(L, "Error unpacking struct field");
                    }
                    lua_getfield(L, -1, "__size");
                    size += lua_tointeger(L, -1);
                    lua_pop(L, 1);
                    lua_rawseti(L, -2, j + 1);
                }
                append_value(L);
            }
        }
        else
        {
            // Handle arrays of simple types
            int array_size = f->array_size > 0 ? f->array_size : 1;
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
                for (int k = 0; k < array_size; k++)
                {
                    if (unpack_simple_value(L, f, s, size) != 1)
                        return luaL_error(L, "Error unpacking array element");
                }
                append_value(L);
            }
        }
    }
    if (f->field_name)
    {
        // Pop the last value and set it also as a field in the table
        lua_rawgeti(L, -1, lua_rawlen(L, -1));
        // Verify that the value at field_name matches the value at index i + 1
        lua_pushvalue(L, -1); //I dont understand this part
        lua_setfield(L, -3, f->field_name);
    }

    // Update the size of the struct
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
        if (f->type == 1) // Bitfield serialization
        {
            // Calculate total bits and bytes
            int total_bits = 0;
            for (auto b : *f->bitfield)
                total_bits += b;
            int bytes = (total_bits + 7) / 8;

            uint64_t value = 0;
            int shift = 0;
            lua_getfield(L, -1, f->field_name ? f->field_name : std::to_string(i + 1).c_str());
            if (!lua_istable(L, -1))
                return luaL_error(L, "Expected table for bitfield '%s'", f->field_name ? f->field_name : "unnamed");

            int field_count = (int)f->bitfield->size();
            for (int k = 1; k <= field_count; k++)
            {
                lua_rawgeti(L, -1, k);
                uint64_t field_value = (uint64_t)luaL_checkinteger(L, -1);
                lua_pop(L, 1);
                int bits = (*f->bitfield)[k - 1];
                uint64_t mask = ((uint64_t)1 << bits) - 1;
                value |= (field_value & mask) << shift;
                shift += bits;
            }
            lua_pop(L, 1); // Pop bitfield table

            // Write value to stream
            for (int j = 0; j < bytes; j++)
            {
                *s++ = (uint8_t)((value >> (j * 8)) & 0xFF);
                size++;
            }
        }
        else if (f->type == 2) // Struct
        {
            // Handle arrays of structs
            int struct_array_size = f->array_size > 0 ? f->array_size : 1;
            if (f->array_size_name != nullptr)
            {
                lua_getfield(L, -1, f->array_size_name);
                if (lua_isnil(L, -1))
                    return luaL_error(L, "Array size field '%s' not found", f->array_size_name);
                struct_array_size = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
            }

            lua_getfield(L, -1, f->field_name ? f->field_name : std::to_string(i + 1).c_str());
            if (struct_array_size == 1)
            {
                // Pack a single struct
                if (!lua_istable(L, -1))
                    return luaL_error(L, "Expected table for struct field '%s'", f->field_name ? f->field_name : "unnamed");

                for (auto ff : *f->fields)
                {
                    if (pack_value(L, ff, s) != 1)
                        return luaL_error(L, "Error packing struct field '%s'", ff->field_name ? ff->field_name : "unnamed");
                }
                lua_pop(L, 1); // Pop struct table
            }
            else
            {
                // Pack an array of structs
                if (!lua_istable(L, -1))
                    return luaL_error(L, "Expected table for struct array field '%s'", f->field_name ? f->field_name : "unnamed");

                for (int j = 1; j <= struct_array_size; j++)
                {
                    lua_rawgeti(L, -1, j);
                    if (!lua_istable(L, -1))
                        return luaL_error(L, "Expected table for struct at index %d", j);

                    for (auto ff : *f->fields)
                    {
                        if (pack_value(L, ff, s) != 1)
                            return luaL_error(L, "Error packing struct field '%s'", ff->field_name ? ff->field_name : "unnamed");
                    }
                    lua_pop(L, 1); // Pop struct table
                }
                lua_pop(L, 1); // Pop struct array table
            }
        }
        else
        {
            // Handle arrays of simple types
            int array_size = f->array_size > 0 ? f->array_size : 1;
            if (f->array_size_name != nullptr)
            {
                lua_getfield(L, -1, f->array_size_name);
                if (lua_isnil(L, -1))
                    return luaL_error(L, "Array size field '%s' not found", f->array_size_name);
                array_size = luaL_checkinteger(L, -1);
                lua_pop(L, 1);
            }

            lua_getfield(L, -1, f->field_name ? f->field_name : std::to_string(i + 1).c_str());
            if (array_size == 1)
            {
                if (lua_isnil(L, -1))
                    return luaL_error(L, "Field '%s' not found", f->field_name ? f->field_name : "unnamed");
                if (pack_simple_value(L, f, s, size) != 1)
                    return luaL_error(L, "Error packing value '%s'", f->field_name ? f->field_name : "unnamed");
                lua_pop(L, 1); // Pop field value
            }
            else
            {
                if (!lua_istable(L, -1))
                    return luaL_error(L, "Expected table for array field '%s'", f->field_name ? f->field_name : "unnamed");
                for (int j = 1; j <= array_size; j++)
                {
                    lua_rawgeti(L, -1, j);
                    if (pack_simple_value(L, f, s, size) != 1)
                        return luaL_error(L, "Error packing array element %d of '%s'", j, f->field_name ? f->field_name : "unnamed");
                    lua_pop(L, 1); // Pop array element
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