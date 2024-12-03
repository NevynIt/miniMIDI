#include "mod_Lua.h"
#include "mod_Stdio.h"
#include "App.h"

void mod_Lua::PreInit() {
    // Initialize Lua engine
    L = luaL_newstate();
    luaL_openlibs(L);
}

void mod_Lua::Init() {
    registerModule(this, "lua");

    // Load Lua boot script, which should configure the system
    if (luaL_dofile(L, "boot.lua") != LUA_OK) {
        // LOG_WARNING("Warning: cannot load Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    printf("Lua initialized\n");
}

void mod_Lua::Tick() {
    // Do nothing
}

void mod_Lua::doString(const char *str) {
    auto t = lua_gettop(L);
    if (luaL_dostring(L, str) != LUA_OK) {
        LOG_WARNING("Warning: cannot execute Lua script: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_settop(L, t); //discard any return values
}

bool mod_Lua::registerModule(Module *mod, const char *name, const char *topTable)
{
    lua_getglobal(L, topTable);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, topTable);
        lua_getglobal(L, topTable);
    }
    lua_getfield(L, -1, name);
    if (!lua_isnil(L, -1))
    {
        LOG_WARNING("Warning: module %s already registered, mixing\n", name);
    }
    else
    {
        lua_pop(L, 1);
        lua_createtable(L, 0, 0);
        lua_setfield(L, -2, name);
        lua_getfield(L, -1, name);
    }

    for (const char **ptr = mod->GetCommands(); ptr && *ptr; ptr++) {
        lua_pushlightuserdata(L, mod);
        lua_pushinteger(L, ptr-mod->GetCommands());
        lua_pushcclosure(L, [](lua_State *L) -> int {
            Module *mod = (Module *)lua_touserdata(L, lua_upvalueindex(1));
            return mod->DoCommand(lua_tointeger(L, lua_upvalueindex(2)), L);
        }, 2);
        lua_setfield(L, -2, *ptr);
    }

    lua_pop(L, 2);
    return true;   
}

const char **mod_Lua::GetCommands() const {
    static const char *commands[] = {
        "doString",
        "dump", //dump(ptr, size) return the memory at ptr of size bytes as a string
        "unpack", //unpack(ptr, fmt) decode a struct at ptr according to fmt
        "ptr", //ptr(addr) return addr as lightuserdata
        "pack",
        "calcsize",
        nullptr
    };
    return commands;
}

int mod_Lua::DoCommand(int i, lua_State *L) {
    switch (i) {
        case 0: // doString
            doString(lua_tostring(L, 1));
            return lua_gettop(L);
        case 1: // dump
            {
                uint32_t addr;
                if (lua_islightuserdata(L, 1)) {
                    addr = (uint32_t)lua_touserdata(L, 1);
                } else {
                    addr = luaL_checkinteger(L, 1);
                } 
                uint32_t size = luaL_checkinteger(L, 2);
                uint8_t *p = (uint8_t *)addr;
                lua_pushlstring(L, (const char *)p, size);
                return 1;
            }
        case 2: // unpack
            {
                // Get string from Lua stack instead of address
                size_t len;
                const char *buffer = luaL_checklstring(L, 1, &len);
                const char *fmt = luaL_checkstring(L, 2);

                unpack(buffer, (int)len, fmt);
                return 1;
            }
        case 3: //ptr
            lua_pushlightuserdata(L, (void *)luaL_checkinteger(L, 1));
            return 1;
        case 4: // pack
            {
                // Ensure the data is a table and a string is provided
                luaL_checktype(L, 1, LUA_TTABLE);
                const char *fmt = luaL_checkstring(L, 2);

                return pack(fmt);
            }
        case 5: // calcsize
            {
                const char *fmt = luaL_checkstring(L, 1);
                int size = calcsize(fmt);
                if (size < 0) {
                    return luaL_error(L, "Unknown format specifier in format string");
                }
                lua_pushinteger(L, size);
                return 1;
            }
    }
    return 0;
}

/*
    format string: {[<count>]<type>["[" <arraysize> "]"][{<whitespace>}]}

    Format characters (the number after : is the default size in bits):
    x:8   pad byte (packing -> insert a 0x00; unpacking -> skip a byte)
    X:8   unsigned int in hex (see note below)
    c:8   char (packs/unpacks as string)
    b:8   signed 8 bit integer
    B:8   unsigned 8 bit integer
    ?:8   _Bool (packing -> int; unpacking -> bool)
    h:16  signed 16 bit integer (short)
    H:16  unsigned 16 bit integer (unsigned short)
    i:32  signed 32 bit integer (int)
    I:32  unsigned 32 bit integer (unsigned int)
    l:32  signed 32 bit integer (long)
    L:32  unsigned 32 bit integer (unsigned long)
    q:64  signed 64 bit integer (long long)
    Q:64  unsigned 64 bit integer (unsigned long long)
    n:32  ssize_t (same as i)
    N:32  size_t (same as I)
    e:16  float (half precision)
    f:32  float
    d:64  double
    s:8   char[] (zero terminated)
    p:8   char[] (pascal style, 8 bit for size, followed by the characters)
    P:32  void * (packing -> int; unpacking -> lightuserdata)

    for X, packing is the same as B, unpacking produces a string of hex digits
*/

int mod_Lua::calcsize(const char *fmt)
{
    int size = 0;
    while (*fmt) {
        int count = 1;
        if (*fmt >= '0' && *fmt <= '9') {
            count = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                count = count * 10 + (*fmt++ - '0');
            }
        }
        char type = *fmt++;
        int type_size = 0;
        switch (type) {
            case 'x': type_size = 1; break;
            case 'X': type_size = 1; break;
            case 'c': type_size = 1; break;
            case 'b': type_size = 1; break;
            case 'B': type_size = 1; break;
            case '?': type_size = 1; break;
            case 'h': type_size = 2; break;
            case 'H': type_size = 2; break;
            case 'i': type_size = 4; break;
            case 'I': type_size = 4; break;
            case 'l': type_size = 4; break;
            case 'L': type_size = 4; break;
            case 'q': type_size = 8; break;
            case 'Q': type_size = 8; break;
            case 'n': type_size = 4; break;
            case 'N': type_size = 4; break;
            case 'e': type_size = 2; break;
            case 'f': type_size = 4; break;
            case 'd': type_size = 8; break;
            case 's': type_size = 1; break;
            case 'p': type_size = 1; break;
            case 'P': type_size = 4; break;
            default: return -1; // Unknown format specifier
        }
        size += count * type_size;
    }
    return size;
}

//pack a struct according to a format string
//if buffer is nullptr, return the size of the buffer needed (not including variable length strings)
//if buffer is not nullptr, return the number of bytes written
int mod_Lua::pack(const char *fmt) {
    std::string buffer; // Dynamic string to build the result
    luaL_checktype(L, 1, LUA_TTABLE); // Ensure the first argument is a table
    int data_index = 1; // Index within the data table

    while (*fmt) {
        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        int count = 1;
        if (isdigit(*fmt)) {
            count = 0;
            while (isdigit(*fmt)) {
                count = count * 10 + (*fmt++ - '0');
            }
        }

        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        char type = *fmt++;

        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        int array_size = 1;
        if (*fmt == '[') {
            fmt++; // Skip '['
            // Skip whitespace
            while (isspace(*fmt)) fmt++;

            array_size = 0;
            while (isdigit(*fmt)) {
                array_size = array_size * 10 + (*fmt++ - '0');
            }

            // Skip whitespace
            while (isspace(*fmt)) fmt++;
            if (*fmt == ']') {
                fmt++; // Skip ']'
            } else {
                return luaL_error(L, "Expected ']' in format string");
            }
        }

        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        for (int c = 0; c < count; c++) {
            if (array_size > 1) {
                if (type == 'c' || type == 'X') {
                    // 'c' or 'X' with array_size specifies a fixed-length string
                    lua_rawgeti(L, 1, data_index++);
                    size_t str_len;
                    const char *str = luaL_checklstring(L, -1, &str_len);
                    lua_pop(L, 1);

                    if (type == 'X') {
                        // Convert hex string to binary data
                        if (str_len != (size_t)(array_size * 2)) {
                            luaL_error(L, "Invalid hex string length for 'X[%d]'", array_size);
                        }
                        for (int i = 0; i < array_size * 2; i += 2) {
                            unsigned int byte;
                            sscanf(str + i, "%2X", &byte);
                            buffer.push_back(static_cast<char>(byte));
                        }
                    } else {
                        // For 'c', copy the string directly
                        if ((int)str_len > array_size) str_len = array_size;
                        buffer.append(str, str_len);
                    }
                } else {
                    // Handle array of values for other types
                    lua_rawgeti(L, 1, data_index++);
                    if (!lua_istable(L, -1)) {
                        lua_pop(L, 1);
                        return luaL_error(L, "Expected table for array type in data");
                    }
                    for (int j = 1; j <= array_size; j++) {
                        lua_rawgeti(L, -1, j);
                        if (lua_isnil(L, -1)) {
                            lua_pop(L, 2); // Pop value and array table
                            lua_pushlstring(L, buffer.data(), buffer.size());
                            return 1;
                        }
                        // Pack the value based on type
                        if (!pack_value(buffer, type)) {
                            lua_pop(L, 2); // Pop value and array table
                            lua_pushlstring(L, buffer.data(), buffer.size());
                            return 1;
                        }
                        lua_pop(L, 1); // Pop the value
                    }
                    lua_pop(L, 1); // Pop the array table
                }
            } else {
                // Single value
                lua_rawgeti(L, 1, data_index++);
                if (lua_isnil(L, -1)) {
                    lua_pop(L, 1); // Pop nil
                    lua_pushlstring(L, buffer.data(), buffer.size());
                    return 1;
                }
                // Pack the value based on type
                if (!pack_value(buffer, type)) {
                    lua_pushlstring(L, buffer.data(), buffer.size());
                    return 1;
                }
                lua_pop(L, 1); // Pop the value
            }
        }
    }
    lua_pushlstring(L, buffer.data(), buffer.size());
    return 1;
}

bool mod_Lua::pack_value(std::string &buffer, char type) {
    // Helper function to pack a single value based on the format specifier
    switch (type) {
        case 'x': {
            // Pad byte
            buffer.push_back(0x00);
            break;
        }
        case 'c': {
            // Single character
            size_t str_len;
            const char *str = luaL_checklstring(L, -1, &str_len);
            if (str_len < 1) return false;
            buffer.push_back(str[0]);
            break;
        }
        case 'b': {
            int8_t value = (int8_t)luaL_checkinteger(L, -1);
            buffer.push_back(value);
            break;
        }
        case 'B': {
            uint8_t value = (uint8_t)luaL_checkinteger(L, -1);
            buffer.push_back(value);
            break;
        }
        case 'X': {
            // Convert hex string to binary data
            size_t hex_len;
            const char *hex_str = luaL_checklstring(L, -1, &hex_len);
            if (hex_len % 2 != 0) return false; // Hex string should have even length
            for (size_t i = 0; i < hex_len; i += 2) {
                unsigned int byte;
                if (sscanf(hex_str + i, "%2X", &byte) != 1) return false;
                buffer.push_back(static_cast<char>(byte));
            }
            break;
        }
        case '?': {
            int value = lua_toboolean(L, -1);
            buffer.push_back((uint8_t)value);
            break;
        }
        case 'h': {
            int16_t value = (int16_t)luaL_checkinteger(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 2);
            break;
        }
        case 'H': {
            uint16_t value = (uint16_t)luaL_checkinteger(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 2);
            break;
        }
        case 'i':
        case 'l':
        case 'n': {
            int32_t value = (int32_t)luaL_checkinteger(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 4);
            break;
        }
        case 'I':
        case 'L':
        case 'N': {
            uint32_t value = (uint32_t)luaL_checkinteger(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 4);
            break;
        }
        case 'q': {
            int64_t value = (int64_t)luaL_checkinteger(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 8);
            break;
        }
        case 'Q': {
            uint64_t value = (uint64_t)luaL_checkinteger(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 8);
            break;
        }
        case 'f': {
            float value = (float)luaL_checknumber(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 4);
            break;
        }
        case 'd': {
            double value = luaL_checknumber(L, -1);
            buffer.append(reinterpret_cast<const char*>(&value), 8);
            break;
        }
        case 's': {
            // Null-terminated string
            size_t str_len;
            const char *str = luaL_checklstring(L, -1, &str_len);
            buffer.append(str, str_len + 1);
            break;
        }
        case 'p': {
            // Pascal string
            size_t str_len;
            const char *str = luaL_checklstring(L, -1, &str_len);
            if (str_len > 255) return false; // Length byte is 1 byte
            buffer.push_back((uint8_t)str_len);
            buffer.append(str, str_len);
            break;
        }
        case 'P': {
            void *ptr = lua_touserdata(L, -1);
            uintptr_t value = (uintptr_t)ptr;
            buffer.append(reinterpret_cast<const char*>(&value), sizeof(void *));
            break;
        }
        default:
            return false; // Unknown format specifier
    }
    return true;
}

int mod_Lua::unpack(const char *buffer, int len, const char *fmt) {
    int offset = 0; // Current position in the buffer
    lua_newtable(L); // Table to hold the unpacked values
    int table_index = lua_gettop(L);

    int data_index = 1; // Lua table index for the next value

    while (*fmt && offset < len) {
        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        int count = 1;
        if (isdigit(*fmt)) {
            count = 0;
            while (isdigit(*fmt)) {
                count = count * 10 + (*fmt++ - '0');
            }
        }

        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        char type = *fmt++;

        // Skip whitespace
        while (isspace(*fmt)) fmt++;

        int array_size = 1;
        if (*fmt == '[') {
            fmt++; // Skip '['
            while (isspace(*fmt)) fmt++;

            array_size = 0;
            while (isdigit(*fmt)) {
                array_size = array_size * 10 + (*fmt++ - '0');
            }

            while (isspace(*fmt)) fmt++;
            if (*fmt == ']') {
                fmt++; // Skip ']'
            } else {
                return luaL_error(L, "Expected ']' in format string");
            }
        }

        while (isspace(*fmt)) fmt++;

        for (int c = 0; c < count; c++) {
            if (array_size > 1 || (type == 'c' && array_size > 1)) {
                if (type == 'c') {
                    // Fixed-length string 'c[5]'
                    if (offset + array_size > len) {
                        return luaL_error(L, "Buffer overrun while reading fixed-length string");
                    }
                    lua_pushlstring(L, buffer + offset, array_size);
                    offset += array_size;
                    lua_rawseti(L, table_index, data_index++); // result[data_index] = string
                } else if (type == 'X') {
                    // Read array_size bytes and convert to hex string
                    if (offset + array_size > len) return luaL_error(L, "Buffer overrun while reading hex string");
                    std::string hex_str;
                    for (int i = 0; i < array_size; ++i) {
                        uint8_t value = static_cast<uint8_t>(buffer[offset++]);
                        char hex_byte[3];
                        snprintf(hex_byte, sizeof(hex_byte), "%02X", value);
                        hex_str += hex_byte;
                    }
                    lua_pushlstring(L, hex_str.c_str(), hex_str.size());
                    lua_rawseti(L, table_index, data_index++); // result[data_index] = hex string
                } else {
                    // Array of values
                    lua_newtable(L); // Create a new table for the array
                    int array_table_index = lua_gettop(L);

                    for (int j = 1; j <= array_size; j++) {
                        if (!unpack_value(buffer, len, offset, type)) {
                            lua_pop(L, 1); // Pop the array table
                            return luaL_error(L, "Failed to unpack value");
                        }
                        lua_rawseti(L, array_table_index, j); // array[j] = value
                    }
                    lua_rawseti(L, table_index, data_index++); // result[data_index] = array
                }
            } else {
                // Single value
                if (!unpack_value(buffer, len, offset, type)) {
                    return luaL_error(L, "Failed to unpack value");
                }
                lua_rawseti(L, table_index, data_index++); // result[data_index] = value
            }
        }
    }

    return 1; // Return the result table
}

bool mod_Lua::unpack_value(const char *buffer, int len, int &offset, char type, int array_size) {
    switch (type) {
        case 'x': {
            // Pad byte, skip
            if (offset + 1 > len) return false;
            offset += 1;
            break;
        }
        case 'c': {
            // Single character
            if (offset + 1 > len) return false;
            char value = buffer[offset];
            lua_pushlstring(L, &value, 1);
            offset += 1;
            break;
        }
        case 'b': {
            if (offset + 1 > len) return false;
            int8_t value = *(int8_t *)(buffer + offset);
            lua_pushinteger(L, value);
            offset += 1;
            break;
        }
        case 'B': {
            if (offset + 1 > len) return false;
            uint8_t value = *(uint8_t *)(buffer + offset);
            lua_pushinteger(L, value);
            offset += 1;
            break;
        }
        case 'X': {
            // Read array_size bytes and convert to hex string
            if (offset + array_size > len) return false;
            std::string hex_str;
            for (int i = 0; i < array_size; ++i) {
                uint8_t value = static_cast<uint8_t>(buffer[offset++]);
                char hex_byte[3];
                snprintf(hex_byte, sizeof(hex_byte), "%02X", value);
                hex_str += hex_byte;
            }
            lua_pushlstring(L, hex_str.c_str(), hex_str.size());
            break;
        }
        case '?': {
            if (offset + 1 > len) return false;
            uint8_t value = *(uint8_t *)(buffer + offset);
            lua_pushboolean(L, value);
            offset += 1;
            break;
        }
        case 'h': {
            if (offset + 2 > len) return false;
            int16_t value;
            memcpy(&value, buffer + offset, 2);
            lua_pushinteger(L, value);
            offset += 2;
            break;
        }
        case 'H': {
            if (offset + 2 > len) return false;
            uint16_t value;
            memcpy(&value, buffer + offset, 2);
            lua_pushinteger(L, value);
            offset += 2;
            break;
        }
        case 'i':
        case 'l':
        case 'n': {
            if (offset + 4 > len) return false;
            int32_t value;
            memcpy(&value, buffer + offset, 4);
            lua_pushinteger(L, value);
            offset += 4;
            break;
        }
        case 'I':
        case 'L':
        case 'N': {
            if (offset + 4 > len) return false;
            uint32_t value;
            memcpy(&value, buffer + offset, 4);
            lua_pushinteger(L, value);
            offset += 4;
            break;
        }
        case 'q': {
            if (offset + 8 > len) return false;
            int64_t value;
            memcpy(&value, buffer + offset, 8);
            lua_pushinteger(L, value);
            offset += 8;
            break;
        }
        case 'Q': {
            if (offset + 8 > len) return false;
            uint64_t value;
            memcpy(&value, buffer + offset, 8);
            lua_pushinteger(L, value);
            offset += 8;
            break;
        }
        case 'f': {
            if (offset + 4 > len) return false;
            float value;
            memcpy(&value, buffer + offset, 4);
            lua_pushnumber(L, value);
            offset += 4;
            break;
        }
        case 'd': {
            if (offset + 8 > len) return false;
            double value;
            memcpy(&value, buffer + offset, 8);
            lua_pushnumber(L, value);
            offset += 8;
            break;
        }
        case 's': {
            // Null-terminated string
            size_t str_len = strlen(buffer + offset);
            if (offset + str_len + 1 > (size_t)len) return false;
            lua_pushlstring(L, buffer + offset, str_len);
            offset += str_len + 1;
            break;
        }
        case 'p': {
            // Pascal string
            if (offset + 1 > len) return false;
            uint8_t str_len = *(uint8_t *)(buffer + offset);
            offset += 1;
            if (offset + str_len > len) return false;
            lua_pushlstring(L, buffer + offset, str_len);
            offset += str_len;
            break;
        }
        case 'P': {
            if (offset + sizeof(void *) > len) return false;
            void *ptr;
            memcpy(&ptr, buffer + offset, sizeof(void *));
            lua_pushlightuserdata(L, ptr);
            offset += sizeof(void *);
            break;
        }
        default:
            return false; // Unknown format specifier
    }
    return true;
}


#include "stateMachine.hpp"

/*
    Grammar definition for format strings:
        struct = { [ <whitespace> ] <field> } [ <whitespace> ]
        field = [ <number> ] <format> [ "[" <number> "]" ]
        number = <digit> { <digit> }
        whitespace = ' ' | '\t' | '\n' | '\r'
        digit = '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
        format = <type> | "{" <struct> "}" | "<" <bitfield> ">"
        type = 'x' | 'X' | 'c' | 'b' | 'B' | '?' | 'h' | 'H' | 'i' | 'I' | 'l' | 'L' | 'q' | 'Q' | 'n' | 'N' | 'e' | 'f' | 'd' | 's' | 'p' | 'P'
        bitfield = { <number> [ ":" ] }
*/

class formatStringParser
{
public:
    enum State
    {
        start = 0,
        whitespace,
        count,
        size,
        structStart,
        structEnd,
        fieldStart,
        fieldEnd,
        formatStart,
        formatEnd,
        bitfieldStart,
        bits,
        bitfieldEnd,
        type,
        end,
        error
    };

    using Event = char;
        // '.' any character
        // ' ' any whitespace
        // '0' any number
        // '{' start of struct
        // '}' end of struct
        // '[' start of array size
        // ']' end of array size
        // '<' start of bitfield
        // '>' end of bitfield
        // 't' any of the valid types
        // ':' bitfield separator
        // '\0' end of string

    using transition = transition<formatStringParser>;

    formatStringParser(const char *fmt) : fmt(fmt) {}
    const char *fmt;

    struct field
    {
        int count = 1;
        char type = 0;
            // 0 invalid
            // 1 struct
            // 2 bitfield
            // X, b, B, ?, h, H, i, I, l, L, q, Q, n, N, e, f, d, s, p, P type
        field *struct_fields = nullptr;
        std::vector<uint8_t> *bitfield = nullptr;
        int array_size = 1;
    };

    State getStartState() {
        return start;
    }

    void *match(State state, Event event, transition &t) {
        if (state == t.from)
        {
            bool valid = false;
            switch(t.event)
            {
                case '.':
                    valid = true;
                    break;
                case ' ':
                    valid = isspace(event);
                    break;
                case '0':
                    valid = isdigit(event);
                    break;
                case '{':
                    valid = event == '{';
                    break;
                case '}':
                    valid = event == '}';
                    break;
                case '[':
                    valid = event == '[';
                    break;
                case ']':
                    valid = event == ']';
                    break;
                case '<':
                    valid = event == '<';
                    break;
                case '>':
                    valid = event == '>';
                    break;
                case 't':
                    valid = strchr("XbB?hHiIlLqQnNeFdspP", event) != nullptr;
                    break;
                case ':':
                    valid = event == ':';
                    break;
                case '\0':
                    valid = event == '\0';
                    break;                    
            }
            if (valid)
                return (void *)&t;
        }
        return nullptr;
    }

    std::array<transition, 4> transitions = {
        transition(start, '.', structStart, nullptr),
        transition{structStart, ' ', structStart, consume}, //consume all whitespace

        transition{structStart, '.', fieldStart, start_field},

        //optional count
        transition{fieldStart, '0', count, start_count},
        transition{count, '0', count, next_count},

        //substrucutre
        transition{fieldStart, '{', structStart, start_sub_struct},
        transition{count, '{', structStart, start_sub_struct},

        //bitfield
        transition{fieldStart, '<', bits, start_bits},
        transition{count, '<', bits, start_bits},
        transition{bits, '0', bits, next_bits},
        transition{bits, ':', bits, next_bit_field},
        transition{bits, '>', formatEnd, consume},

        //type
        transition{fieldStart, 'x', type, consume},
    };

    State next_bit_field(void *p) {
        transition *t = (transition *)p;
        field &f = stack.back();
        f.bitfield->push_back(0);
        return t->to;
    }

    State next_bits(void *p) {
        transition *t = (transition *)p;
        field &f = stack.back();
        f.bitfield->back() = f.bitfield->back() * 10 + (*fmt++ - '0');
        return t->to;
    }

    State start_bits(void *p) {
        transition *t = (transition *)p;
        field &f = stack.back();
        f.bitfield = new std::vector<uint8_t>();
        f.type = 2;
        f.bitfield->push_back(0);
        return t->to;
    }

    State consume(void *p) {
        transition *t = (transition *)p;
        fmt++;
        return t->to;
    }

    State start_field(void *p) {
        transition *t = (transition *)p;
        stack.push_back(field());
        return t->to;
    }

    State start_count(void *p) {
        transition *t = (transition *)p;
        field &f = stack.back();
        f.count = 0;
        f.count = f.count * 10 + (*fmt++ - '0');
        return t->to;
    }

    State next_count(void *p) {
        transition *t = (transition *)p;
        field &f = stack.back();
        f.count = f.count * 10 + (*fmt++ - '0');
        return t->to;
    }

    State start_sub_struct(void *p) {
        transition *t = (transition *)p;
        stack.push_back(field());
        return t->to;
    }

    Event nextEvent() {
        return *fmt;
    }

    std::vector<field> stack;
};