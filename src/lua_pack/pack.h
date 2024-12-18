#pragma once
#include <vector>
#include <cstdint>
#include "lua/lua.hpp"

    /*
    format string: { [ <count> | '\'' <name> '\'' ]<type>["[" <arraysize> | '\'' <name> '\'' "]"] }
    <count> is the number of times to repeat the field
    <name> before the type is the name of the field
    <arraysize> is the size of the array
    <name> after the type is a reference to a field set before, that contains the size of the array

    whitespace is ignored before and after a format string
    <type> can be a character (see below), a bitfield <n:n:...>, or a struct {<format> <format> ...}
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
    f:32  float
    d:64  double
    s:8   char[] (zero terminated)
    p:8   char[] (pascal style, 8 bit for size, followed by the characters)
    P:32  void * (packing -> int; unpacking -> lightuserdata)
    */

class field_info
{
public:
    int count = 1;
    char *field_name = nullptr;
    char type = 0; //0 = invalid, 1 = bitfield, 2 = struct, other chars are the type
    union
    {
        std::vector<uint8_t> *bitfield = nullptr;
        std::vector<field_info *> *fields;
    };
    int array_size = 0; //0 = not an array, >0 = array size
    char *array_size_name = nullptr;
    field_info() {}
    field_info(const field_info &f)
    {
        count = f.count;
        if (f.field_name)
        {
            field_name = new char[strlen(f.field_name) + 1];
            strcpy(field_name, f.field_name);
        }
        type = f.type;
        if (type == 1)
        {
            bitfield = new std::vector<uint8_t>(*f.bitfield);
        }
        else if (type == 2)
        {
            fields = new std::vector<field_info *>(*f.fields);
        }
        array_size = f.array_size;
        if (f.array_size_name)
        {
            array_size_name = new char[strlen(f.array_size_name) + 1];
            strcpy(array_size_name, f.array_size_name);
        }
    }
    ~field_info()
    {
        if (type == 1)
        {
            delete bitfield;
        }
        else if (type == 2)
        {
            delete fields;
        }
        if (field_name)
        {
            delete field_name;
        }
        if (array_size_name)
        {
            delete array_size_name;
        }
    }

    static field_info *parse(const char *fmt);
    void print(const char *indent = "");
    int pack(lua_State *L);
    int unpack(lua_State *L);
};


